#include "mud/event/event.h"
#include "mud/test.h"
#include <future>
#include <ostream>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        handle = std::make_shared<mud::core::select_handle>(10);
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* A handle */
    std::shared_ptr<mud::core::handle> handle;

    /* An event. */
    mud::event::event event;

    /* Another event */
    mud::event::event other;
END_CONTEXT()

FEATURE("Event")

  /*
   * The pre-defined Gherkin steps.
   */
  DEFINE_GIVEN("An event object", 
      [](context& ctx) {
        ctx.event = mud::event::event(ctx.handle,
                mud::event::event::signal_type::READY,
                [](){ return mud::event::event::return_type::REMOVE; });
      })
  DEFINE_GIVEN("A copied event object", 
      [](context& ctx) {
        ctx.other = ctx.event;
      })
  DEFINE_GIVEN("A moved event object", 
      [](context& ctx) {
        ctx.other = std::move(ctx.event);
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
            ASSERT(false, std::is_default_constructible<
                  mud::event::event>::value);
        })
    THEN ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::event::event>::value);
        })
    THEN ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::event::event>::value);
        })
    THEN ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::event::event>::value);
        })
    THEN ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::event::event>::value);
        })

    SCENARIO("Event object creation")
      GIVEN("An event object")
      WHEN ("The event object is examined", [](context&){})
      THEN ("The event valid",
          [](context& ctx){
              ASSERT(false, ctx.event.id().null());
              ASSERT(true, ctx.event.handle()->valid());
          })

    SCENARIO("Copying an event")
      GIVEN("An event object")
      AND  ("A copied event object")
      WHEN ("The objects are examined", [](context&){})
      THEN ("The events are considered equal",
          [](context& ctx){
              ASSERT(true, ctx.event == ctx.other);
              ASSERT(true, ctx.event.id() == ctx.other.id());
              ASSERT(true, ctx.event.handle() == ctx.other.handle());
          })

    SCENARIO("Moving an event")
      GIVEN("An event object")
      AND  ("A moved event object")
      WHEN ("The objects are examined", [](context&){})
      THEN ("The moved event is valid",
          [](context& ctx){
              ASSERT(false, ctx.other.id().null());
              ASSERT(true, ctx.other.handle()->valid());
          })
      AND  ("The original event is invalid",
          [](context& ctx){
              ASSERT(true, ctx.event.id().null());
              ASSERT(nullptr, ctx.event.handle().get());
          })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
