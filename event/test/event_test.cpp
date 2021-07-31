#include "mud/event/event.h"
#include "mud/test.h"
#include <future>
#include <ostream>
#include <type_traits>

/* *INDENT-OFF* */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        event = nullptr;
        copy = nullptr;
        lookup = nullptr;
        handle = std::unique_ptr<mud::core::handle>(
                new mud::core::select_handle(10));
        different_handle = std::unique_ptr<mud::core::handle>(
                new mud::core::select_handle(20));
    }

    /* Destructor after each scenario */
    ~context() {
        delete event;
        delete copy;
        delete lookup;
    }

    /* A handle */
    std::unique_ptr<mud::core::handle> handle;

    /* A different handle */
    std::unique_ptr<mud::core::handle> different_handle;

    /* An event. */
    mud::event::event* event;

    /* A lookup event. */
    mud::event::event* lookup;

    /* A copy */
    mud::event::event* copy;
END_CONTEXT()

FEATURE("Event")

  /*
   * The pre-defined Gherkin steps.
   */
  DEFINE_GIVEN("An event object", 
      [](context& ctx) {
        ctx.event = new mud::event::event(ctx.handle,
                mud::event::event::signal_type::READY,
                [](){ return mud::event::event::return_type::REMOVE; });
      })
  DEFINE_GIVEN("A lookup event object", 
      [](context& ctx) {
        ctx.lookup = new mud::event::event(ctx.handle);
      })
  DEFINE_GIVEN("A lookup event object for a different handle",
      [](context& ctx) {
        ctx.lookup = new mud::event::event(ctx.different_handle);
      })
  DEFINE_THEN("The handle is valid",
          [](context& ctx) {
              ASSERT(true, ctx.event->handle()->valid());
      })
  DEFINE_THEN("The handle of the copy is valid",
          [](context& ctx) {
              ASSERT(true, ctx.copy->handle()->valid());
      })
  DEFINE_THEN("The handle of the lookup is valid",
          [](context& ctx) {
              ASSERT(true, ctx.lookup->handle()->valid());
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
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::event::event>::value);
        })
    THEN ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::event::event>::value);
        })
    THEN ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::event::event>::value);
        })
    THEN ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::event::event>::value);
        })

    SCENARIO("Event object creation")
      GIVEN("An event object")
      WHEN ("The event object is examined", [](context&){})
      THEN ("The handle is valid")

    SCENARIO("Lookup event object creation")
      GIVEN("A lookup event object")
      WHEN ("The event object is examined", [](context&){})
      THEN ("The handle of the lookup is valid")

    SCENARIO("Equality with lookup event with same handle")
      GIVEN("An event object")
      AND  ("A lookup event object")
      WHEN ("Comparing the event with the lookup event", [](context&){})
      THEN ("The comparison matches",
          [](context& ctx) {
              ASSERT(true,  (*ctx.event) == (*ctx.lookup));
              ASSERT(false, (*ctx.event) != (*ctx.lookup));
          })

    SCENARIO("Inequality with lookup event with a different handle")
      GIVEN("An event object")
      AND  ("A lookup event object for a different handle")
      WHEN ("Comparing the event with the lookup event", [](context&){})
      THEN ("The comparison does not match",
          [](context& ctx) {
              ASSERT(false, (*ctx.event) == (*ctx.lookup));
              ASSERT(true,  (*ctx.event) != (*ctx.lookup));
          })

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
