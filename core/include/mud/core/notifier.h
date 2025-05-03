#ifndef _MUDLIB_CORE_NOTIFIER_H_
#define _MUDLIB_CORE_NOTIFIER_H_

#include <functional>
#include <memory>
#include <mud/core/ns.h>
#include <mud/core/notifier.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Forward declarations.
 */
class basic_impulse;
class object;

/**
 * @brief Base class for all @c notifier classes.
 */
class basic_notifier
{
public:
    /**
     * @brief Create a notifier.
     *
     * @details
     * Create a notifier with a reference to the impulse it is associated with.
     *
     * @param impls The @c impulse associatied to this notification.
     */
    basic_notifier(basic_impulse* impls);

    /**
     * @brief Detach from the notifier.
     *
     * @details
     * Detach the notifier as the @c basic_notifier is no longer interested in
     * receiving any notification.
     */
    void detach(basic_notifier* unused);

    /**
     * @brief Detach from the impulse.
     *
     * @details
     * Detach the notifier as the @c basic_impulse is no longer available.
     */
    void detach(basic_impulse* unused);

    virtual void move_object(void* from, void* to) {}
private:
    basic_impulse* _impulse;
};

/**
 * @brief
 * Class to specify a callback for an @c impulse to invoke.
 *
 * @details
 * Define a function to be invoked when an @c impule is triggered. The
 * function signature is only defined by the arguments and should always
 * return @c void.
 *
 * @tparam Args The signature of the function parameters to invoke.
 */
template<typename... Args>
class notifier: public basic_notifier
{
public:
    /**
     * @brief Create a notifier instance.
     *
     * @details
     * Create an instance of a callable notifier that is invoked when an
     * @c impulse is triggered. The notifier is a member function @p func
     * of the @p object.
     *
     * The member function is expected to return @c void and would have the
     * function parameters defined as @c Args.
     *
     * @tparam Object The type of the object instance to invoke
     * @tparam Base The (base) class of the @p Object that defines the
     * member function @c func to invoke.
     * @param impls The impulse associated to this notification.
     * @param object The object to invoke.
     * @param func The member function of the @p object to invoke.
     */
    template <typename Object, typename Base>
    notifier(basic_impulse* impls, Object* object, void(Base::*func)(Args...))
        : basic_notifier(impls)
    {
        // Ensure the member function is a (Base) class of the Object
        static_assert(
            std::is_base_of<Base, Object>::value,
            "Notifier class member function is not a member of the object");

        // Ensure the Object is an object class
        static_assert(
            std::is_base_of<mud::core::object, Object>::value,
            "Notifier is not derived from mud::core::object");

        // Save the callable as a lambda
        _func = [this, func](Args... args) -> void {
            (static_cast<Object*>(this->_object)->*func)(args...);
        };

        // Establish a hash of the member function pointer. Use the function
        // pointer contents as the input. Note that this is compiler dependent
        // and the pointer might also be larger in size than an actual (void*)
        // pointer. For example, this may contain room for the 'this' object
        // pointer and an adjuster value in the vtable. Assume that it is a
        // maximum of 4 times of a size_t value.
        //
        // This is proven to work for
        //    * clang++ 17.0.6
        static_assert(
            sizeof(func) < 4*sizeof(size_t),
            "Pointer to member function is larger than supported.");
        union
        {
            size_t hash[4];;
            void(Base::*mfp)(Args...);
            size_t ptr[32];;
        } cv = { {0, 0, 0, 0} };
        cv.mfp = func;
        _hashcode = typeid(decltype(func)).hash_code();
        for (int i = 0; i < 4; i++) {
            _hashcode ^= cv.hash[i];
        }

        // Save the details used for comparison
        _object = (void*)object;
    }

    /**
     * @brief Create a notifier instance.
     *
     * @details
     * Create an instance of a callable notifier that is invoked when an
     * @c impulse is triggered. The notifier is a static function @p func, or
     * a lambda function without captures.
     *
     * The member function is expected to return @c void and would have the
     * function parameters defined as @c Args.
     *
     * @param impls The impulse associated to this notification.
     * @param func The static or lambda function to invoke.
     */
    notifier(basic_impulse* impls, void(*func)(Args...))
        : basic_notifier(impls)
    {
        // Save the callable as a lambda
        _func = [func](Args... args) -> void {
            (*func)(args...);
        };

        // Save the details used for comparison
        _object = nullptr;
        _hashcode = typeid(decltype(func)).hash_code();
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
    bool operator==(const notifier& other) const {
        return _object == other._object &&
               _hashcode == other._hashcode;
    }

    /**
     * @brief Invoke the function.
     *
     * @details
     * Invoke the function with the @c impulse specific arguments @p args.
     */
    void operator()(Args&&... args) const {
        _func(std::forward<Args>(args)...);
    }

    /**
     * @brief Return the object to invoke.
     * @return Pointer a pointer to the object to be invoked.
     */
    void* object() const {
        return _object;
    }

    /**
     * @brief Move the object to invoke.
     *
     * @details
     * Move the object to invoke form the original @c from object to the @c to
     * object. This means that the @c from object will no longer be invoked when
     * the notification is pulsed.
     */
    void move_object(void* from, void* to) override {
        if (_object == from) {
            _object = to;
        }
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

private:
    /* The function to invoke. */
    std::function<void(Args...)> _func;

    /* The object to invoke (or nullptr for a static function. */
    void* _object;

    /* The hash-code of the function */
    size_t _hashcode;
};

/**
 * @brief
 * Class to specify a callback for an @c impulse to invoke.
 *
 * @details
 * Define a function to be invoked when an @c impule is triggered. The
 * function signature is without arguments and should always return @c void.
 *
 * This is a specialised @c notifier class to support impulses that have no
 * arguments.
 */
template<>
class notifier<void>: public basic_notifier
{
public:
    /**
     * @brief Create a notifier instance.
     *
     * @details
     * Create an instance of a callable notifier that is invoked when an
     * @c impulse is triggered. The notifier is a member function @p func
     * of the @p object.
     *
     * The member function is expected to return @c void and would have the
     * function parameters defined as @c Args.
     *
     * @tparam Object The type of the object instance to invoke
     * @tparam Base The (base) class of the @p Object that defines the
     * member function @c func to invoke.
     * @param impls The impulse associated to this notification.
     * @param object The object to invoke.
     * @param func The member function of the @p object to invoke.
     */
    template <typename Object, typename Base>
    notifier(basic_impulse* impls, Object* object, void(Base::*func)())
        : basic_notifier(impls)
    {
        // Ensure the member function is a (Base) class of the Object
        static_assert(
            std::is_base_of<Base, Object>::value,
            "Notifier class member function is not a member of the object");

        // Ensure the Object is an object class
        static_assert(
            std::is_base_of<mud::core::object, Object>::value,
            "Notifier is not derived from mud::core::object");

        // Save the callable as a lambda
        _func = [this, func]() -> void {
            (static_cast<Object*>(this->_object)->*func)();
        };

        // Establish a hash of the member function pointer. Use the function
        // pointer contents as the input. Note that this is compiler dependent
        // and the pointer might also be larger in size than an actual (void*)
        // pointer. For example, this may contain room for the 'this' object
        // pointer and an adjuster value in the vtable. Assume that it is a
        // maximum of 4 times of a size_t value.
        //
        // This is proven to work for
        //    * clang++ 17.0.6
        static_assert(
            sizeof(func) < 4*sizeof(size_t),
            "Pointer to member function is larger than supported.");
        union
        {
            size_t hash[4];;
            void(Base::*mfp)();
            size_t ptr[32];;
        } cv = { {0, 0, 0, 0} };
        cv.mfp = func;
        _hashcode = typeid(decltype(func)).hash_code();
        for (int i = 0; i < 4; i++) {
            _hashcode ^= cv.hash[i];
        }

        // Save the details used for comparison
        _object = (void*)object;
    }

    /**
     * @brief Create a notifier instance.
     *
     * @details
     * Create an instance of a callable notifier that is invoked when an
     * @c impulse is triggered. The notifier is a static function @p func, or
     * a lambda function without captures.
     *
     * The member function is expected to return @c void and would have the
     * function parameters defined as @c Args.
     *
     * @param impls The impulse associated to this notification.
     * @param func The static or lambda function to invoke.
     */
    notifier(basic_impulse* impls, void(*func)())
        : basic_notifier(impls)
    {
        // Save the callable as a lambda
        _func = [func]() -> void {
            (*func)();
        };

        // Save the details used for comparison
        _object = nullptr;
        _hashcode = typeid(decltype(func)).hash_code();
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
    bool operator==(const notifier& other) const {
        return _object == other._object &&
               _hashcode == other._hashcode;
    }

    /**
     * @brief Invoke the function.
     *
     * @details
     * Invoke the function with the @c impulse specific arguments @p args.
     */
    void operator()() const {
        _func();
    }

    /**
     * @brief Return the object to invoke.
     * @return Pointer a pointer to the object to be invoked.
     */
    void* object() const {
        return _object;
    }

    /**
     * @brief Move the object to invoke.
     *
     * @details
     * Move the object to invoke form the original @c from object to the @c to
     * object. This means that the @c from object will no longer be invoked when
     * the notification is pulsed.
     */
    void move_object(void* from, void* to) override {
        if (_object == from) {
            _object = to;
        }
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

private:
    /* The function to invoke. */
    std::function<void()> _func;

    /* The object to invoke (or nullptr for a static function. */
    void* _object;

    /* The hash-code of the function */
    size_t _hashcode;
};

/**
 * @brief The list of notifiers to invoke on an specific @c impulse.
 *
 * @details
 * The list of @c notifier objects to be invoked when an @c imnpulse is
 * triggered. All @c notifier's of the list will be invoked once on a single
 * trigger in a sequential manner on the same thread.
 *
 * All the @c notifier objects in the list will share the same function
 * signature.
 *
 * @tparam Args The signature of the function parameters to invoke.
 */
template<typename... Args>
class notifier_list
{
public:
    /**
     * @brief The type of the @c notifier object used in the list.
     */
    typedef notifier<Args...> value_type;

    /**
     * @brief The type of the underlying list.
     */
    typedef std::vector<std::shared_ptr<value_type>> list_type;

    /**
     * @brief Destructor.
     *
     * @details
     * The impulse no longer maintains this notification anymore. Remove the
     * impulse from all the notifier objects.
     */
    ~notifier_list() {
        for (auto notifier: _list) {
            notifier->detach((basic_impulse*)nullptr);
        }
    }

    /**
     * @brief Invoke all the notifier objects of the list.
     *
     * @details
     * Invoke the notifier functions with the @c impulse specific arguments
     * @p args.
     */
    void operator()(Args... args) const {
        for (auto notifier: _list) {
            (*notifier)(std::forward<Args>(args)...);
        }
    }

    /**
     * @brief Construct a new @c notifier at the end of the list.
     *
     * @details
     * Create a new @c notifier object and place it at the back of the list. The
     * object inserted (as a @c std::shared_ptr) is returned. If the same
     * notifier already exists, the existing notifier is returned.
     *
     * @tparam FunctionArgs The arguments to be passed.
     * @param impls The impulse associated to this notification.
     * @param args The arguments to forward to the constructor of the
     * @c notifier object.
     *
     * @return The object inserted in the list.
     */
    template <typename... FunctionArgs>
    std::shared_ptr<value_type> emplace_back(
        basic_impulse* impls, FunctionArgs&&... args)
    {
        // Create a shared_ptr of the item to insert.
        auto ptr = std::make_shared<value_type>(
                impls, std::forward<FunctionArgs>(args)...);

        // If the item already exists, return the existing item, else add it.
        auto iter = find(*ptr);
        if (iter == _list.end()) {
            _list.push_back(ptr);
            return ptr;
        }
        else {
            return *iter;
        }
    }

    /**
     * @brief Remove a @c notifier of the same signature.
     *
     * @details
     * The @c notifier with the same same signature is removed from the list.
     *
     * @tparam FunctionArgs The arguments to be passed.
     * @param args The arguments to forward to the constructor of the
     * @c notifier object.
     */
    template <typename... FunctionArgs>
    void remove(FunctionArgs&&... args) {
        auto item = value_type(std::forward<FunctionArgs>(args)...);
        auto iter = find(item);
        if (iter != _list.end()) {
            _list.erase(iter);
        }
    }

    /**
     * @brief Remove a specific @c notifier.
     *
     * @details
     * The specific @c notifier is removed from the list.
     *
     * @param item The notifier to be removed.
     */
    void remove(const std::shared_ptr<value_type>& item) {
        auto iter = find(item);
        if (iter != _list.end()) {
            _list.erase(iter);
        }
    }

    /**
     * @brief Remove a specific @c notifier.
     *
     * @details
     * The specific @c notifier is removed from the list.
     *
     * @param item The notifier to be removed.
     */
    void remove(const basic_notifier* item) {
        auto iter = find(item);
        if (iter != _list.end()) {
            _list.erase(iter);
        }
    }

    /**
     * @brief Return the number of notifiers in the list.
     * @return The number of notifiers in the list.
     */
    size_t size() const {
        return _list.size();
    }

private:
    /**
     * @brief Find a notifier with matching signature.
     *
     * @details
     * Find a notifier that matches the signature of @p item. If the item is
     * not found, the @c end iterator is returned.
     *
     * @return The iterator to the matching item, or @c end if no such item is
     * found.
     */
    typename list_type::const_iterator find(const value_type& item) const
    {
        return std::find_if(_list.begin(), _list.end(),
                [item](const std::shared_ptr<value_type>& list_item) {
                    return item == *list_item;
                });
    }

    /**
     * @brief Find a particular notifier instance.
     *
     * @details
     * Find a particular notifier instance of the same @c shared_ptr.
     *
     * @return The iterator to the matching item, or @c end if no such item is
     * found.
     */
    typename list_type::const_iterator find(
            const std::shared_ptr<value_type>& item) const
    {
        return std::find(_list.begin(), _list.end(), item);
    }

    /**
     * @brief Find a particular notifier instance.
     *
     * @details
     * Find a particular notifier instance of the same @c shared_ptr.
     *
     * @return The iterator to the matching item, or @c end if no such item is
     * found.
     */
    typename list_type::const_iterator find(
            const basic_notifier* item) const
    {
        return std::find_if(_list.begin(), _list.end(),
                [item](const std::shared_ptr<value_type>& list_item) {
                    return item == list_item.get();
                });
    }

    /* The list of notifier objects */
    list_type _list;
};

/**
 * @brief The list of notifiers to invoke on an specific @c impulse.
 *
 * @details
 * The list of @c notifier objects to be invoked when an @c imnpulse is
 * triggered. All @c notifier's of the list will be invoked once on a single
 * trigger in a sequential manner on the same thread.
 *
 * This is a specialised @c notifier_list class to support impulses that have no
 * arguments.
 */
template<>
class notifier_list<void>
{
public:
    /**
     * @brief The type of the @c notifier object used in the list.
     */
    typedef notifier<void> value_type;

    /**
     * @brief The type of the underlying list.
     */
    typedef std::vector<std::shared_ptr<value_type>> list_type;

    /**
     * @brief Destructor.
     *
     * @details
     * The impulse no longer maintains this notification anymore. Remove the
     * impulse from all the notifier objects.
     */
    ~notifier_list() {
        for (auto notifier: _list) {
            notifier->detach((basic_impulse*)nullptr);
        }
    }

    /**
     * @brief Invoke all the notifier objects of the list.
     *
     * @details
     * Invoke the notifier functions with the @c impulse specific arguments
     * @p args.
     */
    void operator()() const {
        for (auto notifier: _list) {
            (*notifier)();
        }
    }

    /**
     * @brief Construct a new @c notifier at the end of the list.
     *
     * @details
     * Create a new @c notifier object and place it at the back of the list. The
     * object inserted (as a @c std::shared_ptr) is returned. If the same
     * notifier already exists, the existing notifier is returned.
     *
     * @tparam FunctionArgs The arguments to be passed.
     * @param impls The impulse associated to this notification.
     * @param args The arguments to forward to the constructor of the
     * @c notifier object.
     *
     * @return The object inserted in the list.
     */
    template <typename... FunctionArgs>
    std::shared_ptr<value_type> emplace_back(
        basic_impulse* impls, FunctionArgs&&... args)
    {
        // Create a shared_ptr of the item to insert.
        auto ptr = std::make_shared<value_type>(
                impls, std::forward<FunctionArgs>(args)...);

        // If the item already exists, return the existing item, else add it.
        auto iter = find(*ptr);
        if (iter == _list.end()) {
            _list.push_back(ptr);
            return ptr;
        }
        else {
            return *iter;
        }
    }

    /**
     * @brief Remove a @c notifier of the same signature.
     *
     * @details
     * The @c notifier with the same same signature is removed from the list.
     *
     * @tparam FunctionArgs The arguments to be passed.
     * @param args The arguments to forward to the constructor of the
     * @c notifier object.
     */
    template <typename... FunctionArgs>
    void remove(FunctionArgs&&... args) {
        auto item = value_type(std::forward<FunctionArgs>(args)...);
        auto iter = find(item);
        if (iter != _list.end()) {
            _list.erase(iter);
        }
    }

    /**
     * @brief Remove a specific @c notifier.
     *
     * @details
     * The specific @c notifier is removed from the list.
     *
     * @param item The notifier to be removed.
     */
    void remove(const std::shared_ptr<value_type>& item) {
        auto iter = find(item);
        if (iter != _list.end()) {
            _list.erase(iter);
        }
    }

    /**
     * @brief Remove a specific @c notifier.
     *
     * @details
     * The specific @c notifier is removed from the list.
     *
     * @param item The notifier to be removed.
     */
    void remove(const basic_notifier* item) {
        auto iter = find(item);
        if (iter != _list.end()) {
            _list.erase(iter);
        }
    }

    /**
     * @brief Return the number of notifiers in the list.
     * @return The number of notifiers in the list.
     */
    size_t size() const {
        return _list.size();
    }

private:
    /**
     * @brief Find a notifier with matching signature.
     *
     * @details
     * Find a notifier that matches the signature of @p item. If the item is
     * not found, the @c end iterator is returned.
     *
     * @return The iterator to the matching item, or @c end if no such item is
     * found.
     */
    typename list_type::const_iterator find(const value_type& item) const
    {
        return std::find_if(_list.begin(), _list.end(),
                [item](const std::shared_ptr<value_type>& list_item) {
                    return item == *list_item;
                });
    }

    /**
     * @brief Find a particular notifier instance.
     *
     * @details
     * Find a particular notifier instance of the same @c shared_ptr.
     *
     * @return The iterator to the matching item, or @c end if no such item is
     * found.
     */
    typename list_type::const_iterator find(
            const std::shared_ptr<value_type>& item) const
    {
        return std::find(_list.begin(), _list.end(), item);
    }

    /**
     * @brief Find a particular notifier instance.
     *
     * @details
     * Find a particular notifier instance of the same @c shared_ptr.
     *
     * @return The iterator to the matching item, or @c end if no such item is
     * found.
     */
    typename list_type::const_iterator find(
            const basic_notifier* item) const
    {
        return std::find_if(_list.begin(), _list.end(),
                [item](const std::shared_ptr<value_type>& list_item) {
                    return item == list_item.get();
                });
    }

    /* The list of notifier objects */
    list_type _list;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_NOTIFIER_H_ */
