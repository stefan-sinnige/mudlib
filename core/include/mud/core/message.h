#ifndef _MUDLIB_CORE_MESSAGE_H_
#define _MUDLIB_CORE_MESSAGE_H_

#include <mud/core/uuid.h>
#include <mutex>
#include <map>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief The data related to a particular notification topic.
 * @details
 * In terms of the notification system, which is based on the publish/subscribe
 * pattern, the @c message contains the particular information of an instance
 * of a topic. It contains two identifications: the topic identification is
 * the group of message this instance belongs to and resembles the topic that
 * a publisher notifies to and a subscriber attaches to. Each message also
 * contains a unique identification itself.
 *
 * Custom data can be added to the notification and distributed to all the
 * subscribers to provide additional information to the notification being
 * raised. This data can be supplied and retrieved in a myriad of ways, provided
 * that the publisher and subscriber agree on the format of the custom data for
 * correct serialisation and deserialisation. These methods can include
 *  - Using @c std::vector<uint8_t> directly
 *  - Using @c std::stringstream to use streaming operators and convert it using
 *    the underlying @c std::string representation and @c std::vector<uint*t>.
 *    This is the recommended way for multi-value data represented like JSON,
 *    XML, protobuf or custom class streaming operators.
 *  - Using the template for single-value data, including @c std::string and
 *    built-in data-types (numbers and boolean).
 */
class MUDLIB_CORE_API message
{
public:
    /**
     * @brief Create an invalid message (null UUID).
     */
    message() = default;

    /**
     * @brief Create a unique message for a particular topic
     */
    message(const mud::core::uuid& topic)
        : _topic(topic)
    { }

    /**
     * @brief Copy constructor
     * @param other The message to copy.
     */
    message(const message& other) = default;

    /**
     * @brief Destructor
     */
    virtual ~message() = default;

    /**
     * @brief Copy assignment.
     * @param other The message to copy.
     * @return Reference to this message.
     */
    message& operator=(const message& other) = default;

    /**
     * @brief Get the message unique identification.
     * @return The unique UUID of this message.
     */
    const mud::core::uuid& id() const {
        return _id;
    }

    /**
     * @brief Get the message topic identification.
     * @return The UUID of the associated topic.
     */
    const mud::core::uuid& topic() const {
        return _topic;
    }

    /**
     * @brief Set the custom data.
     * @param custom The binary custom data to pass when notifying subscribers.
     */
    void data(const std::vector<uint8_t>& custom) {
        _data = custom;
    }

    /**
     * @brief Return the custom data.
     */
    const std::vector<uint8_t>& data() const {
        return _data;
    }

    /**
     * @brief Unsupported custom data type conversion
     */
    template<typename T>
    void data(T value) { static_assert(false, "No conversion of this type"); }

    /**
     * @brief Unsupported custom data type conversion
     */
    template<typename T>
    T data() const { static_assert(false, "No conversion of this type"); }

    /**
     * @brief Set the custom data as a single integral value.
     * @tparam T The type of he integral value
     * @param value The value to set
     */
    template<std::integral T>
    void data(T value) {
        std::vector<uint8_t> custom(sizeof(T));
        for (int i = 0; i < sizeof(T); ++i) {
            custom[i] = (value >> (i*8)) & 0xFF;
        }
        _data = custom;
    }

    /**
     * @brief Get the custom data as a single integral value.
     * @tparam T The type of he integral value
     * @return value The value of the custom data
     */
    template<std::integral T>
    T data() const {
        T custom = 0;
        for (int i = 0; i < sizeof(T) && i < _data.size(); ++i) {
            custom |= (_data[i] << (i*8));
        }
        return custom;
    }

    /**
     * @brief Set the custom data as a string value.
     * @param value The value to set
     */
    void data(const std::string& str) {
        _data = std::vector<uint8_t>(str.begin(), str.end());
    }

    /**
     * @brief Get the custom data as a string value.
     * @return value The value of the custom data
     * @note a copy of the data is created.
     */
    template<>
    std::string data<std::string>() const {
        std::string str(_data.begin(), _data.end());
        return str;
    }

private:
    /** The message unique ID */
    mud::core::uuid _id;

    /** The message topic ID */
    mud::core::uuid _topic;

    /** Arbitrary custom data conatiner */
    std::vector<uint8_t> _data;
};

/**
 * @brief Notification method.
 * @details
 * In terms of the notification system, which is based on the publish/subscribe
 * pattern, the @c method is a vessel to hold one invocable function. This
 * function is either a static function, lambda function or a class member
 * function with an associated physical object.
 *
 * All notification methods return @c void and accept one argument, a constant
 * reference to a notification @c message.
 *
 * A method is uniquely identified through a calculated hash-code and is
 * therefore searchable when stored inside a container.
 */
class MUDLIB_CORE_API method
{
public: 
    /**
     * @brief Create a method that invokes a static or lambda function.
     * @param func The static or lambda function pointer.
     */
    method(std::function<void(const message& msg)> func) {
        _func = [func](void* /* object */, const message& msg) -> void {
            func(msg);
        };
        _object = nullptr;
        _hashcode = typeid(decltype(func)).hash_code();
    }

    /**
     * @brief Create a method that invokes a member function on an @c object.
     * @tparam Type The type of the @c obj object.
     * @param obj The object of type @c Type to invoke the method on.
     * @param func The @c Type member function pointer.
     */
    template <typename Type>
    method(Type* obj, void(Type::*func)(const message&)) {
        _func = [func](void* object, const message& msg) -> void {
            (static_cast<Type*>(object)->*func)(msg);
        };

        /* Establish a hash of the member function pointer. Use the function
         * pointer contents as the input. Note that this is compiler dependent
         * and the pointer might also be larger in size than an actual (void*)
         * pointer. For example, this may contain room for the 'this' object
         * pointer and an adjuster value in the vtable. Assume that it is a
         * maximum of 4 times of a size_t value.
         *
         * This is proven to work for
         *    * clang++ 17.0.6, and later
         */
        static_assert(
            sizeof(func) < 4*sizeof(size_t),
            "Pointer to member function is larger than supported.");
        union
        {
            size_t hash[4];
            void(Type::*mfp)(const message&);
            size_t ptr[32];
        } cv = { {0, 0, 0, 0} };
        cv.mfp = func;
        _hashcode = typeid(decltype(func)).hash_code();
        for (int i = 0; i < 4; i++) {
            _hashcode ^= cv.hash[i];
        }

        /* Save the details that are used for comparison */
        _object = (void*)obj;
    }

    /**
     * @brief Copy constructor.
     * @param other The method to copy from.
     */
    method(const method& other) = default;

    /**
     * @brief Move constructor.
     * @param other The method to move from.
     */
    method(method&& other) = default;

    /**
     * @brief Copy assignment.
     * @param other The method to copy from.
     * @return Reference to this object.
     */
    method& operator=(const method& other) = default;

    /**
     * @brief Move assignment.
     * @param other The method to move from.
     * @return Reference to this object.
     */
    method& operator=(method&& other) = default;

    /**
     * @brief Return the object of the function to invoke.
     * @return The object pointer associated to the method, or @c nullptr.
     */
    void* object() const {
        return _object;
    }

    /**
     * @brief Set the object of the function to invoke.
     * @tparam Type The type of the @c obj object.
     * @param obj The object of type @c Type to invoke the method on.
     * @return The object pointer associated to the method, or @c nullptr.
     * @details
     * The new object should be of the same class type as the original when the
     * method was created.
     */
    template<typename Type>
    void object(Type* obj) {
        // static_assert(
        //     std::...?
        // or record the typeid and verify
        _object = static_cast<void*>(obj);
    }

    /**
     * @brief Return the hash-code of the function to invoke.
     *
     * @details
     * The hash-code is a calculated hash of the function to be invoked. It is
     * not necessarily the same as the hash code of the function's @c decltype.
     *
     * @return The hash-code of the function to be invoked.
     */
    size_t hash_code() const {
        return _hashcode;
    }

    /**
     * @brief Invoke the method.
     * @param msg The unqiue message.
     */
    void operator()(const message& msg) const {
        _func(_object, msg);
    }
    /**
     * @brief Equality operator.
     *
     * @details
     * Returns true if the object is equal to @c other. A @c notifier is
     * equal if it points to the same object and member function.
     *
     * @param other The object to compare against.
     * @return True if the objects are equal.
     */
    bool operator==(const method& other) const {
        return _object == other._object &&
               _hashcode == other._hashcode;
    }

    /**
     * @brief Arbitrary ordering operator.
     * @param other The method to compare against.
     * @return True if the objects differ or if the hashcode is smaller than
     * the @c other hashcode.
     */
    bool operator<(const method& other) const {
        if (_object == other._object) {
            return _hashcode < other._hashcode;
        }
        return _object < other._object;
    }

private:
    /* Type definition for all notification functions. */
    typedef std::function<void(void*, const message&)> function_type;

    /* The function to invoke. This can be static, bound member or lambda. */
    function_type _func;

    /* If the function is a bound member function, this refers to the associated
     * object. */
    void* _object;

    /* The generated hash-code of the method. */
    size_t _hashcode;
};

/**
 * @brief The notification message broker.
 * @details
 * In terms of the notification system, which is based on the publish/subscribe
 * pattern, the @c broker maintains the management of the message topics, their
 * publishers and their subscribers. It is responsible for the management of
 * the subscriber attachments to a topic and detachments from them, and also
 * for the routing of a topic from a publisher to all the attached subscribers.
 */
class MUDLIB_CORE_API broker
{
public:
    /**
     * @brief Attach a static or lambda function to a topic.
     * @param topic The topic ID to attach to.
     * @param func The function pointer to notify.
     * @details
     * Creata a notification subscription by attaching a static or lambda
     * function without any associated object. The function will be called
     * whenever a message of the @c topic type is notified by a publisher.
     * @note Static functions can be detached, while lambda functions can not.
     */
    static void attach(
            const mud::core::uuid& topic,
            std::function<void(const mud::core::message&)> func)
    {
        method m(func);
        instance().attach(topic, std::move(m));
    }

    /**
     * @brief Attach a member function of an object to a topic.
     * @tparam Type The type of the @c obj object.
     * @param topic The topic ID to attach to.
     * @param obj The object of type @c Type to call the @c func function on.
     * @param func The member function pointer of @c Type  to notify.
     * @details
     * Create a notification subscription by attaching a specific object
     * member function to a @c topic. The function will be called on the object
     * whenever a message of the @c topic type is notified by a publisher.
     */
    template<typename Type>
    static void attach(
            const mud::core::uuid& topic,
            Type* obj,
            void(Type::*func)(const mud::core::message&))
    {
        method m(obj, func);
        instance().attach(topic, std::move(m));
    }

    /**
     * @brief Detach a static function from a topic.
     * @param topic The topic ID to detach from.
     * @param func The function pointer to notify.
     * @details
     * Delete a notification subscription by detaching a previously attached
     * function @c func from a @c topic.
     * @note Lambda functions can not be detached.
     */
    static void detach(
            const mud::core::uuid& topic,
            std::function<void(const mud::core::message&)> func)
    {
        method m(func);
        instance().detach(topic, std::move(m));
    }

    /**
     * @brief Detach a member function of an object from a topic.
     * @tparam Type The type of the @c obj object.
     * @param topic The topic ID to detach from.
     * @param obj The object of type @c Type to call the @c func function on.
     * @param func The member function pointer of @c Type  to notify.
     * @details
     * Delete a notification subscription by detaching a previously attached
     * specific object  member function from a @c topic.
     */
    template<typename Type>
    static void detach(
            const mud::core::uuid& topic,
            Type* obj,
            void(Type::*func)(const mud::core::message&))
    {
        method m(obj, func);
        instance().detach(topic, std::move(m));
    }

    /**
     * @brief Detach all member functions associated to an object.
     * @param obj The object of type @c Type to detach.
     * @details
     * Delete all notification subscriptions associated to an object by
     * detaching all of its member function from any topic.
     */
    template<typename Type>
    static void detach(Type* obj)
    {
        instance().detach(static_cast<void*>(obj));
    }

    /**
     * @brief Move all subscriptions to a different object.
     * @tparam Type The type of the @c src and @c dst objects.
     * @param src The source object to move the subscriptions from.
     * @param dst The destination object to move the subscriptions to.
     * @details
     * Moving all subscriptions from @c src to @c dst will let the broker route
     * any notification that targeted the @c src object to target the @c dst
     * object instead.
     *
     * The @c src object will no longer hold any subscriptions. The @c dst
     * object will hold all subscriptions that were previously held by the
     * @c src object, in addition to any subscription it already held.
     */
    template<typename Type>
    static void move(Type* src, Type* dst)
    {
        instance().move(static_cast<void*>(src), static_cast<void*>(dst));
    }

    /**
     * @brief Return the number of attached subscribers.
     * @param topic The topic ID to query the subscribers for.
     * @return The number of attached subscribers.
     */
    static size_t size(const mud::core::uuid& topic);

    /**
     * @brief Publish the message for a certain topic.
     * @param msg The notification message.
     * @details
     * Notify any subscriber for the topic of this message by invoking its
     * attached function.
     */
    static void publish(const mud::core::message& msg)
    {
        instance().notify(msg);
    }

private:
    /**
     * @brief Return the instance of the broker mechanism.
     */
    static broker& instance();

    /**
     * @brief Attach a method to a topic.
     * @param topic The topic ID to attach to.
     * @param meth The method to attach.
     */
    void attach(const mud::core::uuid& topic, mud::core::method&& meth);

    /**
     * @brief Detach a method from a topic.
     * @param topic The topic ID to detach from.
     * @param meth The method to detach.
     */
    void detach(const mud::core::uuid& topic, mud::core::method&& meth);

    /**
     * @brief Detach all methods associated to any object.
     * @param obj The object to remove the associated methods from.
     */
    void detach(void* obj);

    /**
     * @brief Move all subscriptions to a different object.
     * @param src The source object to move the subscriptions from.
     * @param dst The destination object to move the subscriptions to.
     */
    void move(void* src, void *dst);

    /**
     * @brief Notify all subscriptions of a new message.
     * @param The notification message.
     */
    void notify(const mud::core::message& msg);

    /** The active subscriptions. */
    typedef std::vector<mud::core::method> methods;
    std::map<mud::core::uuid, methods> _subscriptions;

    /** The mutex to protect the subscriptions. */
    std::recursive_mutex _mutex;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_MESSAGE_H_ */
