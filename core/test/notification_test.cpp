/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/core/object.h"
#include "mud/test.h"
#include <memory>
#include <stdexcept>
#include <thread>
#include <type_traits>

/* clang-format off */

/* Test structures for dynamic subscribers */

class sample_publisher: public mud::core::object
{
public:
    sample_publisher(const mud::core::uuid& topic)
        : _topic(topic)
    {}
    
    /* Publish the notifications */
    void publish() {
        ::mud::core::broker::publish(_topic);
    }

    /* Publish the notification */
    void publish(const mud::core::message& msg) {
        ASSERT(_topic.str(), msg.topic().str());
        ::mud::core::broker::publish(msg);
    }

private:
    mud::core::uuid _topic;
};

class sample_subscriber: public mud::core::object
{
public:
    sample_subscriber() = default;

    /* Notification handlers */
    void on_notified(const mud::core::message& msg) {
        ++_notified_count;
        _msg = msg;
    }

    int notified_count() const {
        return _notified_count;
    }

    const mud::core::message& notification() const {
        return _msg;
    }

private:
    int _notified_count = 0;
    mud::core::message _msg;
};

/* Test structures for static subscribers */

int static_count;

void on_notified(const mud::core::message&)
{
    ++static_count;
}

/* Test structures for lambda subscribers */

int lambda_count;

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        _publisher = nullptr;
        _subscriber = nullptr;
        _other_subscriber = nullptr;
        static_count = 0;
        lambda_count = 0;
    }

    /* Destructor after each scenario */
    ~context() {
        delete _publisher;
        delete _subscriber;
        delete _other_subscriber;
    }

    /* The topic */
    mud::core::uuid _topic;

    /* A publisher */
    sample_publisher* _publisher;

    /* Subscribers */
    sample_subscriber* _subscriber;
    sample_subscriber* _other_subscriber;
END_CONTEXT()

FEATURE("Notification")
  DEFINE_GIVEN("A publisher", [](context& ctx) {
        ctx._publisher = new sample_publisher(ctx._topic);
    })
  DEFINE_GIVEN("A subscriber", [](context& ctx) {
        ctx._subscriber = new sample_subscriber();
    })
  DEFINE_GIVEN("A subscriber is attached", [](context& ctx) {
        if (ctx._subscriber == nullptr) {
            ctx._subscriber = new sample_subscriber();
        }
        mud::core::broker::attach(
            ctx._topic,
            ctx._subscriber, &sample_subscriber::on_notified);
    })
  DEFINE_GIVEN("The subscriber is attached twice",
    [](context& ctx) {
        mud::core::broker::attach(
            ctx._topic,
            ctx._subscriber, &sample_subscriber::on_notified);
        mud::core::broker::attach(
            ctx._topic,
            ctx._subscriber, &sample_subscriber::on_notified);
    })
  DEFINE_GIVEN("Another subscriber is attached",
    [](context& ctx) {
        if (ctx._other_subscriber == nullptr) {
            ctx._other_subscriber = new sample_subscriber();
        }
        mud::core::broker::attach(
            ctx._topic,
            ctx._other_subscriber, &sample_subscriber::on_notified);
    })
  DEFINE_GIVEN("Two subscribers are attached", [](context& ctx) {
        if (ctx._subscriber == nullptr) {
            ctx._subscriber = new sample_subscriber();
        }
        mud::core::broker::attach(
            ctx._topic,
            ctx._subscriber, &sample_subscriber::on_notified);
        if (ctx._other_subscriber == nullptr) {
            ctx._other_subscriber = new sample_subscriber();
        }
        mud::core::broker::attach(
            ctx._topic,
            ctx._other_subscriber, &sample_subscriber::on_notified);
    })
  DEFINE_GIVEN("A dynamic subscriber is attached",
    [](context& ctx) {
        if (ctx._subscriber == nullptr) {
            ctx._subscriber = new sample_subscriber();
        }
        mud::core::broker::attach(
            ctx._topic,
            ctx._subscriber, &sample_subscriber::on_notified);
    })
  DEFINE_GIVEN("A static subscriber is attached",
    [](context& ctx) {
        mud::core::broker::attach(
            ctx._topic,
            &on_notified);
    })
  DEFINE_GIVEN("A lambda subscriber is attached",
    [](context& ctx) {
        mud::core::broker::attach(
            ctx._topic,
            [](const mud::core::message&) {
                 ++lambda_count;
            });
    })
  DEFINE_WHEN("A notification is sent", [](context& ctx) {
        ctx._publisher->publish();
    })
  DEFINE_WHEN("The publisher is deleted", [](context& ctx) {
        delete ctx._publisher;
        ctx._publisher = nullptr;
    })
  DEFINE_WHEN("The subscriber is deleted", [](context& ctx) {
        delete ctx._subscriber;
        ctx._subscriber = nullptr;
    })
  DEFINE_WHEN("The other subscriber is deleted", [](context& ctx) {
        delete ctx._other_subscriber;
        ctx._other_subscriber = nullptr;
    })
  DEFINE_WHEN ("The static subscriber is detached",
    [](context& ctx) {
        mud::core::broker::detach(
            ctx._topic,
            &on_notified);
    })
  DEFINE_WHEN ("The dynamic subscriber is detached",
    [](context& ctx) {
        mud::core::broker::detach(
            ctx._topic,
            ctx._subscriber, &sample_subscriber::on_notified);
    })
  DEFINE_WHEN ("The subscriber is copied to another subscriber",
    [](context& ctx) {
        ctx._other_subscriber = new sample_subscriber(
                *ctx._subscriber);

    })
  DEFINE_WHEN ("The subscriber is moved to another subscriber",
    [](context& ctx) {
        ctx._other_subscriber = new sample_subscriber(
                std::move(*ctx._subscriber));
    })
  DEFINE_THEN("The static subscriber receives a notification",
    [](context& ctx) {
        ASSERT(1, static_count);
    })
  DEFINE_THEN("The lambda subscriber receives a notification",
    [](context& ctx) {
        ASSERT(1, lambda_count);
    })
  DEFINE_THEN("The subscriber receives a notification",
    [](context& ctx) {
        ASSERT(1, ctx._subscriber->notified_count());
    })
  DEFINE_THEN("The subscriber receives one notification",
    [](context& ctx) {
        ASSERT(1, ctx._subscriber->notified_count());
    })
  DEFINE_THEN("The other subscriber receives a notification", [](context& ctx) {
        ASSERT(1, ctx._other_subscriber->notified_count());
    })
  DEFINE_THEN("The subscriber does not receive a notification",
    [](context& ctx) {
        ASSERT(0, ctx._subscriber->notified_count());
    })
  DEFINE_THEN("The other subscriber does not receive a notification",
    [](context& ctx) {
        ASSERT(0, ctx._other_subscriber->notified_count());
    })
  DEFINE_THEN("The static subscriber does not receive a notification",
    [](context& ctx) {
        ASSERT(0, static_count);
    })
  DEFINE_THEN("Both subscribers receive a notification", [](context& ctx) {
        ASSERT(1, ctx._subscriber->notified_count());
        ASSERT(1, ctx._other_subscriber->notified_count());
    })
  DEFINE_THEN("No subscriber receive a notification", [](context& ctx) {
        ASSERT(0, ctx._subscriber->notified_count());
        ASSERT(0, ctx._other_subscriber->notified_count());
    })
  DEFINE_THEN ("The publisher has no subscriptions", [](context& ctx) {
        ASSERT(0, mud::core::broker::size(ctx._topic));
    })
  DEFINE_THEN ("The publisher has one subscription", [](context& ctx) {
        ASSERT(1, mud::core::broker::size(ctx._topic));
    })
  DEFINE_THEN ("The publisher has two subscriptions", [](context& ctx) {
        ASSERT(2, mud::core::broker::size(ctx._topic));
    })
  DEFINE_THEN ("The publisher has three subscriptions", [](context& ctx) {
        ASSERT(3, mud::core::broker::size(ctx._topic));
    })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Object type traits")
    GIVEN("An object type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::object>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::object>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::object>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::object>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::object>::value);
        })

  SCENARIO("Notification with no subscribers")
    GIVEN("A publisher")
     AND ("A subscriber")
    WHEN ("A notification is sent")
    THEN ("The subscriber does not receive a notification")

  SCENARIO("Notification with a subscriber")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("A notification is sent")
    THEN ("The subscriber receives a notification")

  SCENARIO("Notification with multiple subscribers")
    GIVEN("A publisher")
     AND ("Two subscribers are attached")
    WHEN ("A notification is sent")
    THEN ("Both subscribers receive a notification")

  SCENARIO("Notification on a deleted subscriber")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("The subscriber is deleted")
     AND ("A notification is sent")
    THEN("The publisher has no subscriptions")

  SCENARIO("Notification with a dynamic subscriber")
    GIVEN("A publisher")
     AND ("A dynamic subscriber is attached")
    WHEN ("A notification is sent")
    THEN ("The subscriber receives a notification")

  SCENARIO("Notification with a static subscriber")
    GIVEN("A publisher")
     AND ("A static subscriber is attached")
    WHEN ("A notification is sent")
    THEN ("The static subscriber receives a notification")

  SCENARIO("Notification with a lambda subscriber")
    GIVEN("A publisher")
     AND ("A lambda subscriber is attached")
    WHEN ("A notification is sent")
    THEN ("The lambda subscriber receives a notification")

  SCENARIO("Notification with a duplicate subscriber")
    GIVEN("A publisher")
     AND ("A subscriber")
     AND ("The subscriber is attached twice")
    WHEN ("A notification is sent")
    THEN ("The subscriber receives one notification")

  SCENARIO("A copied subscriber does not copy the notifications")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("The subscriber is copied to another subscriber")
     AND ("A notification is sent")
    THEN ("The subscriber receives a notification")
     AND ("The other subscriber does not receive a notification")

  SCENARIO("A moved subscriber moves the notifications")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("The subscriber is moved to another subscriber")
     AND ("A notification is sent")
    THEN ("The subscriber does not receive a notification")
     AND ("The other subscriber receives a notification")

  SCENARIO("A static subscriber can be detached")
    GIVEN("A publisher")
     AND ("A static subscriber is attached")
    WHEN ("The static subscriber is detached")
     AND ("A notification is sent")
    THEN ("The static subscriber does not receive a notification")

  SCENARIO("A dynamic subscriber can be detached")
    GIVEN("A publisher")
     AND ("A dynamic subscriber is attached")
    WHEN ("The dynamic subscriber is detached")
     AND ("A notification is sent")
    THEN ("The subscriber does not receive a notification")

  SCENARIO("A dynamic subscriber can be deleted while attached")
    GIVEN("A publisher")
     AND ("A dynamic subscriber is attached")
    WHEN ("The subscriber is deleted")
    THEN("The publisher has no subscriptions")

  SCENARIO("A publisher can be deleted with an attached subscriber")
    GIVEN("A publisher")
     AND ("A dynamic subscriber is attached")
    WHEN ("The publisher is deleted")
     AND ("The dynamic subscriber is detached")
    THEN("The publisher has no subscriptions")

  SCENARIO("Binary data can be passed with the notification")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("A notification is send with custom data", [](context& ctx) {
            std::vector<uint8_t> custom;
            for (uint8_t i = 0; i < 8; ++i) {
                custom.push_back(i);
            }
            mud::core::message msg(ctx._topic);
            msg.data(custom);
            ctx._publisher->publish(msg);
        })
    THEN ("The subscriber receives the notification with the custom data",
        [](context& ctx) {
            ASSERT(1, ctx._subscriber->notified_count());
            auto custom = ctx._subscriber->notification().data();
            ASSERT(8, custom.size());
            for (uint8_t i = 0; i < 8; ++i) {
                ASSERT(i, custom[i]);
                    ASSERT(i, custom[i]);
            }
        })

  SCENARIO("Integral data can be passed with the notification")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("A notification is send with custom data", [](context& ctx) {
            uint32_t custom = 0x12345678;
            mud::core::message msg(ctx._topic);
            msg.data(custom);
            ctx._publisher->publish(msg);
        })
    THEN ("The subscriber receives the notification with the custom data",
        [](context& ctx) {
            ASSERT(1, ctx._subscriber->notified_count());
            uint32_t custom = ctx._subscriber->notification().data<uint32_t>();
            ASSERT(0x12345678, custom);
        })

  SCENARIO("Boolean data can be passed with the notification")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("A notification is send with custom data", [](context& ctx) {
            bool custom = true;
            mud::core::message msg(ctx._topic);
            msg.data(custom);
            ctx._publisher->publish(msg);
        })
    THEN ("The subscriber receives the notification with the custom data",
        [](context& ctx) {
            ASSERT(1, ctx._subscriber->notified_count());
            bool custom = ctx._subscriber->notification().data<bool>();
            ASSERT(true, custom);
        })

  /*
   * Post-poned until an acceptable solution is found that works on clang and
   * gnu compilers. Problem is that gnu is not conform the C++17 standard in
   * regards to explicit specialization.
   */
#if 0
  SCENARIO("String data can be passed with the notification")
    GIVEN("A publisher")
     AND ("A subscriber is attached")
    WHEN ("A notification is send with custom data", [](context& ctx) {
            const std::string custom = "Hello World";
            mud::core::message msg(ctx._topic);
            msg.data(custom);
            ctx._publisher->publish(msg);
        })
    THEN ("The subscriber receives the notification with the custom data",
        [](context& ctx) {
            ASSERT(1, ctx._subscriber->notified_count());
            std::string custom = ctx._subscriber->notification().data<std::string>();
            ASSERT("Hello World", custom);
        })
#endif

END_FEATURE()

/* clang-format-on */

/* vi: set ai ts=4 expandtab: */
