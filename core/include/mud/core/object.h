#ifndef _MUDLIB_CORE_OBJECT_H_
#define _MUDLIB_CORE_OBJECT_H_

#include <memory>
#include <mud/core/ns.h>
#include <mud/core/notifier.h>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief The @c object class provides the basic mechanism for the notification
 * handling. In this concept, an object can act as an initiator of notifications
 * to interested parties. In that case, the object is sending an @c impulse
 * to other @c objects thave have been attached to that impulse.
 *
 * @details
 * In the concept of notification handling, the system has been modeled as a
 * nervous system. A receptor object can issue impulses to a number of
 * effectors, each of which will define their own course of action when such
 * an impulse has been received. For example, when a TCP socket has received
 * data, it may send an impulse of that to any effector who may read or process
 * the data that was received. In this way, the receptor object would not need
 * to know intricate details about the actions in an effector, it merely is
 * responsible for issuing the impulse to any effector.
 *
 * The @c object class serves as both a receptor and an effector. This means
 * that an @c object can only send notifications to other @c objects. The @c
 * impulse is a definition of a certain stimulus that the @c object can
 * trigger. Any @c object can attach itself to such an impulse and be notified
 * when it is triggered. The @c object can also detach itself from an impulse
 * wen it no longer wishes to be notified or when it is destructed.
 */
class MUDLIB_CORE_API object
{
public:
    /**
     * @brief Constructor
     *
     * @details
     * Construct an @c object that is able to be a participant in the
     * notification mechanism. Either as a receptor that issues notifications
     * through an impulse, or as an effector that receives the notifications.
     */
    object() = default;

    /**
     * @brief Copy-constructor.
     *
     * @details
     * Create an object as a copy of another one, but does @em not copy the
     * @c notification subscriptions.
     *
     * @param other The object to copy from.
     */
    object(const object& other);

    /**
     * @brief Move-constructor.
     *
     * @details
     * Create the object, as a copy of another one, but @em moves the @c
     * notification subscriptions to this new instance.
     *
     * @param other The object to move from.
     */
    object(object&& other);

    /**
     * @brief Copy-assignment.
     *
     * @details
     * Assign the contents of another @c object, but does @em not copy the
     * @c notification subscriptions.
     *
     * @param other The object to copy from.
     * @return A reference to itself.
     */
    object& operator=(const object& other);

    /**
     * @brief Move-assignment.
     *
     * @details
     * Initialise the object, as a copy of another one, but @em moves the @c
     * notification subscriptions to this new instance.
     *
     * @param other The object to move from.
     * @return A reference to itself.
     */
    object& operator=(object&& other);

    /**
     * @brief Destructor.
     *
     * @details
     * When the object is destructed, all its attached notifiers are released
     * from their associated @c impulses.
     */
    virtual ~object() {
        for (auto notifier: _notifiers) {
            notifier->detach(notifier.get());
        }
    }

    /**
     * @brief Attach a @p notifier to this object.
     *
     * @details
     * Attach a @p notifier to the receptor such that it can manage the
     * notifications that reference this object. When the object is destructed,
     * all notifiers are detached properly.
     *
     * The @c notifier can be passed as
     *   - An object with its member function pointer
     *   - A static function pointer
     *   - A lambda function (without captures)
     *
     * @param notifier The notifier to attach.
     */
    void attach(std::shared_ptr<basic_notifier> notifier) {
        _notifiers.push_back(notifier);
    }

    /**
     * @brief Detach a @p notifier from the receptor.
     *
     * @details
     * Detach a @p notifier from the receptor. Note that the specific object
     * will be removed, it is not based on notification function signature.
     *
     * @param notifier The notifier to detach.
     */
    void detach(std::shared_ptr<basic_notifier> notifier) {
        auto iter = std::find(_notifiers.begin(), _notifiers.end(), notifier);
        if (iter != _notifiers.end()) {
            _notifiers.erase(iter);
        }
    }

private:
    /** The list of notifiers that target this object as the effector. */
    std::vector<std::shared_ptr<basic_notifier>> _notifiers;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_OBJECT_H_ */
