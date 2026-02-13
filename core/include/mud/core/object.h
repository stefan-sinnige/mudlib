#ifndef _MUDLIB_CORE_OBJECT_H_
#define _MUDLIB_CORE_OBJECT_H_

#include <memory>
#include <mud/core/ns.h>
#include <mud/core/message.h>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief Generic base class to interact with the notification system.
 * @details
 * In terms of the notification system, which is based on the publish/subscribe
 * pattern, the @c object class provides the basic mechanism for the message
 * notification handling. In this concept, an object can act both as the
 * initiator and as the receiver of notifications. As the initiator, an object
 * can @c notify a message to any interested party, and as a receiver, it can
 * attach to any particular message being notified. 
 *
 * In the concept of notification handling, the system has been modeled as a
 * publish-subscribe mechanism using particular @em topics. Any publisher can
 * @c notify a topic message and any subscriver to that same topic will then
 * be notified of its message. Only messages of a certain topic that a
 * subscriber attaches itself to will be received by that subscriber.
 *
 * As publishers and subscribers only negotiate a particular topic, it is
 * possible that subscribers can attach to a topic for which there are no
 * publishers (yet), a publisher can notify a topic message without any attached
 * subscribers, and multiple publishers can notify on the same topic. This
 * concept allows for a full range of flexibility and in general, a publisher
 * and a subscriber are not aware if either of them.
 *
 * A @em topic is a particular message identification that both subscribers and
 * publishers use for negotiation and routing of any message of that topic. Each
 * message that is sent will have the topic identification, but also a unique
 * message instance identification as well. The latter is unique per message
 * being notified by the publisher.
 *
 * When using @c attach, it is recommended to also call @c detach in the class's
 * destructor to avoid race conditions when a message notification is dispatched
 * to an object that is currently being destructed.
 */
class MUDLIB_CORE_API object
{
public:
    /**
     * @brief Constructor
     * @details
     * Construct an @c object that is able to be a participant in the
     * notification mechanism. Either as a publisher that issues notifications,
     * or as a subscriber that receives the notifications.
     */
    object() = default;

    /**
     * @brief Copy-constructor.
     * @param other The object to copy from.
     * @details
     * Create an object as a copy of another one, but does @em not copy the
     * subscriptions. Each instance of the @c object will need to manage its own
     * subscriptions.
     */
    object(const object& other) = default;

    /**
     * @brief Move-constructor.
     * @param other The object to move from. Its subscriptions will be moved as
     * well and the @c other will not have any subscriptions associated.
     * @details
     * Create the object, as a copy of another one, but @em moves the
     * subscriptions to this new instance.
     */
    object(object&& other) {
        mud::core::broker::move(&other, this);
    }

    /**
     * @brief Copy-assignment.
     * @param other The object to copy from.
     * @return A reference to itself.
     * @details
     * Assign the contents of another @c object, but does @em not copy the
     * subscriptions.
     */
    object& operator=(const object& other) = default;

    /**
     * @brief Move-assignment.
     * @param other The object to move from. Its subscriptions will be moved as
     * @return A reference to itself.
     * @details
     * Initialise the object, as a copy of another one, but @em moves the
     * subscriptions to this new instance.
     */
    object& operator=(object&& other) {
        mud::core::broker::move(&other, this);
        return *this;
    }

    /**
     * @brief Destructor.
     */
    virtual ~object() {
        detach();
    }

    /**
     * @brief Attach a member function of an object to a topic.
     * @param topic The topid ID to attach to.
     * @param func The member function pointer to notify.
     * @details
     * Create a notification subscription by attaching a specific member
     * function to a @c topic. The function will be called on this object
     * whenever a message of the @c topic type is notified by a publisher.
     */
    template<typename Type>
    void attach(
            const mud::core::uuid& topic,
            void(Type::*func)(const message&))
    {
        mud::core::broker::attach(topic, static_cast<Type*>(this), func);
    }

    /**
     * @brief Detach a member function of an object from a topic.
     * @param topic The topid ID to detach from.
     * @param func The member function pointer to notify.
     * @details
     * Delete a notification subscription by detaching a specific member
     * function from a @c topic.
     */
    template<typename Type>
    void detach(
            const mud::core::uuid& topic,
            void(Type::*func)(const message&))
    {
        mud::core::broker::detach(topic, static_cast<Type*>(this), func);
    }

    /**
     * @brief Detach from all topics.
     */
    void detach() {
        mud::core::broker::detach(this);
    }
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_OBJECT_H_ */
