#ifndef _MUDLIB_CORE_IMPULSE_H_
#define _MUDLIB_CORE_IMPULSE_H_

#include <mud/core/ns.h>
#include <mud/core/notifier.h>
#include <mud/core/object.h>

BEGIN_MUDLIB_CORE_NS

/*
 * Forward declarations.
 */
class object;

/**
 * @brief Base class for all @c impulses.
 *
 * @details
 * Base class for all @c impulse definitions.
 */
class basic_impulse
{
public:
    /**
     * @brief Remove a notifier from an impulse.
     *
     * @details
     * Detach a notifier from this @c impulse, such that it no longer receives
     * any notifications. The @c notifier is selected, based on its instance.
     *
     * @param obj The @c notifier object.
     */
    virtual void detach(const basic_notifier* obj) = 0;
};

/**
 * @brief The impulse class that defined the @c notifications.
 *
 * @details
 * In the concept of notification handling, the @c impulse defines the type
 * of a @c notfication can be sent by an @c object and received by another @c
 * object. A @c notification is a concrete instance of an @c impulse.
 *
 * The impulse itself holds a list of the objects that need to be notified.
 *
 * @tparam Args The signature of the function parameters to invoke.
 */
template<typename... Args>
class MUDLIB_CORE_API impulse: public basic_impulse
{
public:
    /**
     * @brief Constructor
     */
    impulse() = default;

    /**
     * @brief Not copyable.
     */
    impulse(const impulse&) = delete;

    /**
     * @brief Move-constructor.
     *
     * @details
     * Create the impulse, as a copy of another one, but @em moves the @c object
     * subscriptions to this new instance.
     *
     * @param other The object to move from.
     */
    impulse(impulse&& other);

    /**
     * @brief Not copy-assignable.
     */
    impulse& operator=(const impulse& other);

    /**
     * @brief Move-assignment.
     *
     * @details
     * Initialise the impulse, as a copy of another one, but @em moves the @c
     * object subscriptions to this new instance.
     *
     * @param other The object to move from.
     * @return A reference to itself.
     */
    impulse& operator=(impulse&& other);

    /**
     * @brief Attach a notifier to an impulse.
     *
     * @details
     * Attach an @c object as an object to be notified when the @c object
     * issues a @c notification for this @p impulse.
     *
     * @tparam FunctionArgs The arguments to be passed.
     * @param args The arguments to forward to the constructor of the
     * @c notifier object.
     *
     * @return The object inserted in the list.
     */
    template <typename... FunctionArgs>
    void attach(FunctionArgs&&... args) {
        auto ptr = _notifiers.emplace_back(
                this,
                std::forward<FunctionArgs>(args)...);
        mud::core::object* obj = static_cast<mud::core::object*>(ptr->object());
        if (obj != nullptr) {
            obj->attach(ptr);
        }
    }

    /**
     * @brief Remove a notifier from an impulse.
     *
     * @details
     * Detach a notifier of a @c object from this @c impulse, such that it no
     * longer receives any notifications. The @c notifier is selected, based
     * on the @c object object and signature.
     *
     * @tparam FunctionArgs The arguments to be passed.
     * @param args The arguments to forward to the constructor of the
     * @c notifier object.
     */
    template <typename... FunctionArgs>
    void detach(object& obj, FunctionArgs... args);

    /**
     * @brief Remove a notifier from an impulse.
     *
     * @details
     * Detach a notifier from this @c impulse, such that it no longer receives
     * any notifications. The @c notifier is selected, based on its instance.
     *
     * @param obj The @c notifier object.
     */
    virtual void detach(const basic_notifier* obj) override {
        _notifiers.remove(obj);
    }

    /*
     * @brief Send a notification to all the @c object.
     *
     * @details
     * Create a @c notification and send it to all registered @c objects. The
     * notification is sent in an asynchronous manner such that it would not
     * impact the @c object that issues the notification.
     *
     * @tparam FunctionArgs Parameter pack of any argument to the impulse
     * function.
     *
     * @param args Any argument of the @p Func impulse.
     */
    template<typename... FunctionArgs>
    void pulse(FunctionArgs&&... args) {
        _notifiers(std::forward<FunctionArgs>(args)...);
    }

private:
    /* The list of notifications to trigger. */
    notifier_list<Args...> _notifiers;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_IMPULSE_H_ */
