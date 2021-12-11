#include "mud/core/handle.h"
#include "mud/test.h"
#include <future>
#include <memory>
#include <type_traits>
#if defined(WINDOWS) && defined(NATIVE)
    #include <windows.h>
#else
    #include <sys/select.h>
#endif

/* clang-format off */

typedef mud::core::select_handle::signal trigger;

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        select_result = -1;
        captures = 0;
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* The select signal */
    trigger self_signal;

    /* Result of the select */
    int select_result;

    /* Counter to indicate the number of captures */
    int captures;

    /* The future */
    std::future<void> future;
END_CONTEXT()

FEATURE("Select handle trigger")
  DEFINE_GIVEN("A select handle trigger",
      [](context& ctx) {
      })
  DEFINE_GIVEN("A tread waiting for the trigger",
      [](context& ctx) {
          ctx.future = std::async(std::launch::async, [&ctx]() {
              fd_set read_fds;
              FD_ZERO(&read_fds);
              int handle = mud::core::internal_handle<int>(
                    ctx.self_signal.handle());
              FD_SET(handle, &read_fds);
              struct timeval timeout;
              timeout.tv_sec = 0;
              timeout.tv_usec = 100;
              ctx.select_result = ::select(handle+1, &read_fds, nullptr,
                    nullptr, &timeout);
              if (ctx.select_result > 0)
              {
                  if (FD_ISSET(handle, &read_fds))
                  {
                      ctx.captures += ctx.self_signal.capture() ? 1 : 0;
                  }
              }
          });
      })
    DEFINE_WHEN("The signal is triggered",
        [](context& ctx) {
            ctx.self_signal.trigger();
        })
    DEFINE_THEN("The signal is captured",
        [](context& ctx) {
            ctx.future.wait();
            ASSERT(1, ctx.select_result);
            ASSERT(1, ctx.captures);
        })
    DEFINE_THEN("The signal cannot be not captured again",
        [](context& ctx) {
            ASSERT(false, ctx.self_signal.capture());
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Select handle trigger type traits")
    GIVEN("A select handle trigger type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                   trigger>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                   trigger>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                   trigger>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                   trigger>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                   trigger>::value);
        })

  SCENARIO("Triggered signal can be captured")
    GIVEN("A select handle trigger")
      AND("A tread waiting for the trigger")
    WHEN ("The signal is triggered")
    THEN ("The signal is captured")
      AND("The signal cannot be not captured again")
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
