#include "mud/core/impulse.h"
#include "mud/core/object.h"
#include "mud/test.h"
#include <memory>
#include <stdexcept>
#include <thread>
#include <type_traits>

/* clang-format off */

/* Test structures for dynamic effectors */

class sample_receptor: public mud::core::object
{
public:
    typedef mud::core::impulse<int>         val_impulse_t;
    typedef std::shared_ptr<val_impulse_t>  val_impulse_ptr_t;
    typedef mud::core::impulse<int&>        ref_impulse_t;
    typedef std::shared_ptr<ref_impulse_t>  ref_impulse_ptr_t;
    typedef mud::core::impulse<void>        void_impulse_t;
    typedef std::shared_ptr<void_impulse_t> void_impulse_ptr_t;

    sample_receptor() {
        _val_impulse_ptr  = std::make_shared<val_impulse_t>();
        _ref_impulse_ptr  = std::make_shared<ref_impulse_t>();
        _void_impulse_ptr = std::make_shared<void_impulse_t>();
    }
    
    /* The impulses that an effector can connect to. */
    val_impulse_ptr_t val_impulse() {
        return _val_impulse_ptr;
    }
    ref_impulse_ptr_t ref_impulse() {
        return _ref_impulse_ptr;
    }
    void_impulse_ptr_t void_impulse() {
        return _void_impulse_ptr;
    }

    /* Send notifications to any connected effector. */
    void val_pulse(int value) {
        _val_impulse_ptr->pulse(value);
    }
    void ref_pulse(int& value) {
        _ref_impulse_ptr->pulse(value);
    }
    void void_pulse() {
        _void_impulse_ptr->pulse();
    }

private:
    val_impulse_ptr_t  _val_impulse_ptr;
    ref_impulse_ptr_t  _ref_impulse_ptr;
    void_impulse_ptr_t _void_impulse_ptr;
};

class sample_effector: public mud::core::object
{
public:
    sample_effector(int& count, int& value)
        : _impulse_count(count), _impulse_value(value)
    {
    }

    sample_effector(int& count, int& value, const sample_effector& other)
        : mud::core::object(other)
        , _impulse_count(count), _impulse_value(value)
    {
    }

    sample_effector(int& count, int& value, sample_effector&& other) 
        : mud::core::object(std::move(other))
        , _impulse_count(count), _impulse_value(value)
    {
    }

    /* Functions that receive notifications from a receptor on an impulse */
    void on_val_impulse(int value) {
        ++_impulse_count;
        _impulse_value = value;
    }
    void on_ref_impulse(int& value) {
        ++_impulse_count;
        _impulse_value = value;
    }
    void on_void_impulse() {
        ++_impulse_count;
    }

private:
    int& _impulse_count;
    int& _impulse_value;
};

/* Test structures for static effectors */

int static_count;
int static_value;

void static_val_effector(int value)
{
    ++static_count;
    static_value = value;
}

void static_ref_effector(int& value)
{
    ++static_count;
    static_value = value;
}

void static_void_effector()
{
    ++static_count;
}

/* Test structures for lambda effectors */

int lambda_count;
int lambda_value;

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        _receptor = nullptr;
        _effector = nullptr;
        _other_effector = nullptr;
        _impulse_count[0] = _impulse_count[1] = 0;
        _impulse_value[0] = _impulse_value[1] = 0;
        static_count = static_value = 0;
        lambda_count = lambda_value = 0;
    }

    /* Destructor after each scenario */
    ~context() {
        delete _receptor;
        delete _effector;
        delete _other_effector;
    }

    /* A receptor */
    sample_receptor* _receptor;

    /* Effectors */
    sample_effector* _effector;
    sample_effector* _other_effector;

    /* Impulse counts and values */
    int _impulse_count[2];
    int _impulse_value[2];
END_CONTEXT()

FEATURE("Notification")
  DEFINE_GIVEN("A receptor", [](context& ctx) {
        ctx._receptor = new sample_receptor();
    })
  DEFINE_GIVEN("An effector", [](context& ctx) {
        ctx._effector = new sample_effector(
            ctx._impulse_count[0], ctx._impulse_value[0]);
    })
  DEFINE_GIVEN("An effector is attached to a receptor", [](context& ctx) {
        ctx._effector = new sample_effector(
            ctx._impulse_count[0], ctx._impulse_value[0]);
        ctx._receptor->val_impulse()->attach(
            ctx._effector,
            &sample_effector::on_val_impulse);
    })
  DEFINE_GIVEN("Another effector is attached to a receptor", [](context& ctx) {
        ctx._other_effector = new sample_effector(
            ctx._impulse_count[1], ctx._impulse_value[1]);
        ctx._receptor->val_impulse()->attach(
            ctx._other_effector,
            &sample_effector::on_val_impulse);
    })
  DEFINE_GIVEN("Two effectors are attached to a receptor", [](context& ctx) {
        ctx._effector = new sample_effector(
            ctx._impulse_count[0], ctx._impulse_value[0]);
        ctx._receptor->val_impulse()->attach(
            ctx._effector,
            &sample_effector::on_val_impulse);
        ctx._other_effector = new sample_effector(
            ctx._impulse_count[1], ctx._impulse_value[1]);
        ctx._receptor->val_impulse()->attach(
            ctx._other_effector,
            &sample_effector::on_val_impulse);
    })
  DEFINE_GIVEN("A dynamic effector with val arguments is attached to a receptor",
    [](context& ctx) {
        ctx._effector = new sample_effector(
            ctx._impulse_count[0], ctx._impulse_value[0]);
        ctx._receptor->val_impulse()->attach(
            ctx._effector,
            &sample_effector::on_val_impulse);
    })
  DEFINE_GIVEN("A static effector with val arguments is attached to a receptor",
    [](context& ctx) {
         ctx._receptor->val_impulse()->attach(
             &static_val_effector);
    })
  DEFINE_GIVEN("A lambda effector with ref arguments is attached to a receptor",
    [](context& ctx) {
         ctx._receptor->val_impulse()->attach([](int value) {
             ++lambda_count;
             lambda_value = value;
         });
    })
  DEFINE_GIVEN("A dynamic effector with ref arguments is attached to a receptor",
    [](context& ctx) {
        ctx._effector = new sample_effector(
            ctx._impulse_count[0], ctx._impulse_value[0]);
        ctx._receptor->ref_impulse()->attach(
            ctx._effector,
            &sample_effector::on_ref_impulse);
    })
  DEFINE_GIVEN("A static effector with ref arguments is attached to a receptor",
    [](context& ctx) {
         ctx._receptor->val_impulse()->attach(
             &static_val_effector);
    })
  DEFINE_GIVEN("A lambda effector with ref arguments is attached to a receptor",
    [](context& ctx) {
         ctx._receptor->ref_impulse()->attach([](int& value) {
             ++lambda_count;
             lambda_value = value;
         });
    })
  DEFINE_GIVEN("A dynamic effector with no arguments is attached to a receptor",
    [](context& ctx) {
        ctx._effector = new sample_effector(
            ctx._impulse_count[0], ctx._impulse_value[0]);
        ctx._receptor->void_impulse()->attach(
            ctx._effector,
            &sample_effector::on_void_impulse);
    })
  DEFINE_GIVEN("A static effector with no arguments is attached to a receptor",
    [](context& ctx) {
         ctx._receptor->void_impulse()->attach(
             &static_void_effector);
    })
  DEFINE_GIVEN("A lambda effector with no arguments is attached to a receptor",
    [](context& ctx) {
         ctx._receptor->void_impulse()->attach([]() {
             ++lambda_count;
         });
    })
  DEFINE_WHEN("A notification is sent", [](context& ctx) {
        int value = 100;
        ctx._receptor->val_pulse(value);
        ctx._receptor->ref_pulse(value);
        ctx._receptor->void_pulse();
    })
  DEFINE_WHEN("The effector is deleted", [](context& ctx) {
        delete ctx._effector;
        ctx._effector = nullptr;
    })
  DEFINE_WHEN("The other effector is deleted", [](context& ctx) {
        delete ctx._other_effector;
        ctx._other_effector = nullptr;
    })
  DEFINE_THEN("The static effector receives a notification", [](context& ctx) {
        ASSERT(1, static_count);
    })
  DEFINE_THEN("The lambda effector receives a notification", [](context& ctx) {
        ASSERT(1, lambda_count);
    })
  DEFINE_WHEN ("The effector is copied to another effector", [](context& ctx) {
        ctx._other_effector = new sample_effector(
            ctx._impulse_count[1], ctx._impulse_value[1],
            *ctx._effector);
    })
  DEFINE_WHEN ("The effector is moved to another effector", [](context& ctx) {
        ctx._other_effector = new sample_effector(
            ctx._impulse_count[1], ctx._impulse_value[1],
            std::move(*ctx._effector));
    })
  DEFINE_THEN("The effector receives a notification", [](context& ctx) {
        ASSERT(1, ctx._impulse_count[0]);
    })
  DEFINE_THEN("The static effector has received a value with the notification",
    [](context& ctx) {
        ASSERT(100, static_value);
    })
  DEFINE_THEN("The lambda effector has received a value with the notification",
    [](context& ctx) {
        ASSERT(100, lambda_value);
    })
  DEFINE_THEN("The effector has received a value with the notification",
    [](context& ctx) {
        ASSERT(100, ctx._impulse_value[0]);
    })
  DEFINE_THEN("The other effector has received a value with the notification",
    [](context& ctx) {
        ASSERT(100, ctx._impulse_value[1]);
    })
  DEFINE_THEN("Both effectors have received a value with the notification",
    [](context& ctx) {
        ASSERT(100, ctx._impulse_value[0]);
        ASSERT(100, ctx._impulse_value[1]);
    })
  DEFINE_THEN("The other effector receives a notification", [](context& ctx) {
        ASSERT(1, ctx._impulse_count[1]);
    })
  DEFINE_THEN("The effector does not receive a notification",
    [](context& ctx) {
        ASSERT(0, ctx._impulse_count[0]);
        ASSERT(0, ctx._impulse_value[0]);
    })
  DEFINE_THEN("The other effector does not receive a notification",
    [](context& ctx) {
        ASSERT(0, ctx._impulse_count[1]);
        ASSERT(0, ctx._impulse_value[1]);
    })
  DEFINE_THEN("Both effectors receive a notification", [](context& ctx) {
        ASSERT(1, ctx._impulse_count[0]);
        ASSERT(1, ctx._impulse_count[1]);
    })
  DEFINE_THEN("No effector receive a notification", [](context& ctx) {
        ASSERT(0, ctx._impulse_count[0]);
        ASSERT(0, ctx._impulse_count[1]);
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

  SCENARIO("Notification with no effector")
    GIVEN("A receptor")
     AND ("An effector")
    WHEN ("A notification is sent")
    THEN ("The effector does not receive a notification")

  SCENARIO("Notification with an effector")
    GIVEN("A receptor")
     AND ("An effector is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The effector receives a notification")
     AND ("The effector has received a value with the notification")

  SCENARIO("Notification with multiple effectors")
    GIVEN("A receptor")
     AND ("Two effectors are attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("Both effectors receive a notification")
     AND ("Both effectors have received a value with the notification")

  SCENARIO("Notification on a deleted effector")
    GIVEN("A receptor")
     AND ("An effector is attached to a receptor")
    WHEN ("The effector is deleted")
     AND ("A notification is sent")
    THEN ("The effector does not receive a notification")

  SCENARIO("Notification with a static effector using values")
    GIVEN("A receptor")
     AND ("A dynamic effector with val arguments is attached to a receptor")
     AND ("A static effector with val arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The static effector receives a notification")
     AND ("The static effector has received a value with the notification")

  SCENARIO("Notification with a static effector using values")
    GIVEN("A receptor")
     AND ("A static effector with val arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The static effector receives a notification")
     AND ("The static effector has received a value with the notification")

  SCENARIO("Notification with a lambda effector using values")
    GIVEN("A receptor")
     AND ("A lambda effector with ref arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The lambda effector receives a notification")
     AND ("The lambda effector has received a value with the notification")

  SCENARIO("Notification with a dynamic effector using references")
    GIVEN("A receptor")
     AND ("A dynamic effector with ref arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The effector receives a notification")
     AND ("The effector has received a value with the notification")

  SCENARIO("Notification with a static effector using references")
    GIVEN("A receptor")
     AND ("A static effector with ref arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The static effector receives a notification")
     AND ("The static effector has received a value with the notification")

  SCENARIO("Notification with a lambda effector using references")
    GIVEN("A receptor")
     AND ("A lambda effector with ref arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The lambda effector receives a notification")
     AND ("The lambda effector has received a value with the notification")

  SCENARIO("Notification with a dynamic effector with no arguments")
    GIVEN("A receptor")
     AND ("A dynamic effector with no arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The effector receives a notification")

  SCENARIO("Notification with a static effector with no arguments")
    GIVEN("A receptor")
     AND ("A static effector with no arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The static effector receives a notification")

  SCENARIO("Notification with a lambda effector with no arguments")
    GIVEN("A receptor")
     AND ("A lambda effector with no arguments is attached to a receptor")
    WHEN ("A notification is sent")
    THEN ("The lambda effector receives a notification")

  SCENARIO("A copied effector does not copy the notifications")
    GIVEN("A receptor")
     AND ("An effector is attached to a receptor")
    WHEN ("The effector is copied to another effector")
     AND ("A notification is sent")
    THEN ("The effector receives a notification")
     AND ("The other effector does not receive a notification")

  SCENARIO("A moved effector moves the notifications")
    GIVEN("A receptor")
     AND ("An effector is attached to a receptor")
    WHEN ("The effector is moved to another effector")
     AND ("A notification is sent")
    THEN ("The effector does not receive a notification")
     AND ("The other effector receives a notification")

END_FEATURE()

/* clang-format-on */

/* vi: set ai ts=4 expandtab: */
