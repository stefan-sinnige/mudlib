#include "mud/core/event.h"
#include "mud/core/object.h"
#include "mud/test.h"
#include <future>
#include <ostream>
#include <type_traits>

/* clang-format off */

CONTEXT(public mud::core::object)
    /* Constructor initialised for each scenario run */
    context() {
        handle = std::make_shared<mud::core::select_handle>(10);
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* A handler for the event */
    void on_event(const mud::core::message&) {
    }

    /* A handler for the other event */
    void on_other(const mud::core::message&) {
    }

    /* A handle */
    std::shared_ptr<mud::core::handle> handle;

    /* An event. */
    mud::core::event event;

    /* Another event */
    mud::core::event other;
END_CONTEXT()

FEATURE("Event")

  /*
   * The pre-defined Gherkin steps.
   */
  DEFINE_GIVEN("An event object", 
      [](context& ctx) {
        ctx.event = mud::core::event(mud::core::uuid(), ctx.handle,
                mud::core::event::signal_type::READY);
        ctx.attach<context>(ctx.event.topic(), &context::on_event);
      })
  DEFINE_GIVEN("An other event object", 
      [](context& ctx) {
        ctx.other = mud::core::event(mud::core::uuid(), ctx.handle,
               mud::core::event::signal_type::READY);
        ctx.attach(ctx.other.topic(), &context::on_other);
      })
  DEFINE_THEN ("The event is valid",
      [](context& ctx){
        ASSERT(false, ctx.event.topic().null());
        ASSERT(true, ctx.event.handle()->valid());
      })
  DEFINE_THEN ("The other event is valid",
      [](context& ctx){
        ASSERT(false, ctx.other.topic().null());
        ASSERT(true, ctx.other.handle()->valid());
      })
  DEFINE_THEN ("The event is invalid",
      [](context& ctx){
        ASSERT(true, ctx.event.topic().null());
      })
  DEFINE_THEN ("The other event is invalid",
      [](context& ctx){
        ASSERT(true, ctx.other.topic().null());
      })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("An event type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::event>::value);
        })
     AND ("The type copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::event>::value);
        })
     AND ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::event>::value);
        })
     AND ("The type copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::event>::value);
        })
     AND ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::event>::value);
        })

    SCENARIO("Event object creation")
      GIVEN("An event object")
      WHEN ("The event object is examined", [](context&){})
      THEN ("The event is valid")

    SCENARIO("Moving an event")
      GIVEN("An event object")
      WHEN ("The event is moved to the other event",
          [](context& ctx){
              ctx.other = std::move(ctx.event);
          })
      THEN ("The event is valid")
      AND  ("The other event is valid")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
