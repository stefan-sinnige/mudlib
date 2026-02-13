#include "mud/core/timer.h"
#include "mud/core/event_loop.h"
#include "timer_dispatcher.h"
#include "mud/test.h"
#include <iomanip>
#include <ostream>
#include <type_traits>

std::ostream&
operator<<(std::ostream& ostr, const std::chrono::system_clock::time_point& tp)
{
    // Get the time as a time_t and get the milliseconds too.
    std::time_t tp_t = std::chrono::system_clock::to_time_t(tp);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()).count() % 1000;

    // Output as local time
    auto saved_flags = ostr.flags();
    ostr << std::put_time(::localtime(&tp_t), "%F %T")
         << '.' << std::setw(3) << std::setfill('0') << std::dec << ms;
    ostr.flags(saved_flags);
    return ostr;
}

/* clang-format off */

CONTEXT(public mud::core::object)
    /* Constructor initialised for each scenario run */
    context() {
        struct tm simulated;
        simulated.tm_year = 2024 - 1900;    // 2024
        simulated.tm_mon = 10 - 1;          // October
        simulated.tm_mday = 25;             // 25
        simulated.tm_hour = 15;             // 3 PM
        simulated.tm_min = 38;              // 38 minutes
        simulated.tm_sec = 25;              // 25 seconds
        epoch = std::chrono::system_clock::from_time_t(mktime(&simulated));
        notification_count = 0;
        attach(timer.expired(), &context::on_timer);
    }

    /* Destructor after each scenario */
    ~context() {
        if (event_thread.joinable()) {
            mud::core::event_loop::global().terminate();
            event_thread.join();
        }
    }

    /* The timer notification. */
    void on_timer(const mud::core::message&) {
        ++notification_count;
    }

    /* A simulated epoch */
    std::chrono::system_clock::time_point epoch;

    /* An interval */
    std::chrono::milliseconds interval;

    /* An delay */
    std::chrono::milliseconds delay;

    /* A single timer */
    mud::core::timer timer;

    /* A set of timers */
    std::vector<mud::core::timer> timers;

    /* The timer notification count */
    int notification_count;

    /* The thread running the event-loop. */
    std::thread event_thread;
END_CONTEXT()

FEATURE("Timer")

  /*
   * The pre-defined Gherkin steps.
   */
  DEFINE_GIVEN("A periodic 1 second timer", [](context& ctx) {
      ctx.interval = std::chrono::seconds(1);
      ctx.timer.start(ctx.interval, ctx.epoch);
  })
  DEFINE_GIVEN("A periodic 5 second timer", [](context& ctx) {
      ctx.interval = std::chrono::seconds(5);
      ctx.timer.start(ctx.interval, ctx.epoch);
  })
  DEFINE_GIVEN("A once-off timer in 1 hour", [](context& ctx) {
      ctx.timer.at(ctx.epoch + std::chrono::hours(1));
  })
  DEFINE_GIVEN("A running event loop",
      [](context& ctx) {
          ctx.epoch = std::chrono::system_clock::now();
          ctx.event_thread = std::thread([]() {
              mud::core::event_loop::global().loop();
          });
          mud::core::event_loop::global().ready().wait();
      })
  DEFINE_WHEN("The timer is examined", [](context& ctx){
  })
  DEFINE_THEN("The type is unknown", [](context& ctx){
      ASSERT(mud::core::timer::type_t::UNKNOWN, ctx.timer.type());
  })
  DEFINE_THEN("The type is periodic", [](context& ctx){
      ASSERT(mud::core::timer::type_t::PERIODIC, ctx.timer.type());
  })
  DEFINE_THEN("The type is once-off", [](context& ctx){
      ASSERT(mud::core::timer::type_t::ONCE_OFF, ctx.timer.type());
  })
  DEFINE_THEN("The timer is active", [](context& ctx){
      ASSERT(true, mud::core::timer::type_t::UNKNOWN != ctx.timer.type());
  })
  DEFINE_THEN("The timer is inactive", [](context& ctx){
      ASSERT(mud::core::timer::type_t::UNKNOWN, ctx.timer.type());
  })
  DEFINE_THEN("The timer is not triggered", [](context& ctx) {
      ASSERT(0, ctx.notification_count);
  })
  DEFINE_THEN("The timer is triggered once", [](context& ctx) {
      ASSERT(1, ctx.notification_count);
  })
  DEFINE_THEN("The timer is triggered twice", [](context& ctx) {
      ASSERT(2, ctx.notification_count);
  })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("An timer type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::timer>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::timer>::value);
        })
    THEN ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::timer>::value);
        })
    THEN ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::timer>::value);
        })
    THEN ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::timer>::value);
        })

  SCENARIO("Periodic Timer starting expiration time point")
    GIVEN("A periodic 5 second timer")
    WHEN ("The timer is examined")
    THEN ("The type is periodic")
     AND ("The expiration time is set for 5 second", [](context& ctx) {
            std::stringstream expected, actual;
            expected << ctx.epoch + ctx.interval;
            actual << ctx.timer.expiration();
            ASSERT(expected.str(), actual.str());
        })

  SCENARIO("Once-Off Timer expiration time point")
    GIVEN("A once-off timer in 1 hour")
    WHEN ("The timer is examined")
    THEN ("The type is once-off")
     AND ("The expiration time is set for 1 hour", [](context& ctx) {
            std::stringstream expected, actual;
            expected << ctx.epoch + std::chrono::hours(1);
            actual << ctx.timer.expiration();
            ASSERT(expected.str(), actual.str());
        })

  SCENARIO("A periodic timer remains active after a trigger")
    GIVEN("A periodic 5 second timer")
    WHEN ("The timer is triggered at 6 seconds", [](context& ctx) {
            auto& dispatcher = mud::core::event_loop::global().timers();
            auto time_point = ctx.epoch + std::chrono::seconds(6);
            dispatcher->dispatch(time_point);
        })
    THEN ("The timer is triggered once")
     AND ("The timer is active")
     AND ("The next expiration time is set for 10 seconds", [](context& ctx) {
            std::stringstream expected, actual;
            expected << ctx.epoch + std::chrono::seconds(10);
            actual << ctx.timer.expiration();
            ASSERT(expected.str(), actual.str());
        })

  SCENARIO("A once-off timer is deactivated after a trigger")
    GIVEN("A once-off timer in 1 hour")
    WHEN ("The timer is triggered in 1 hour", [](context& ctx) {
            auto& dispatcher = mud::core::event_loop::global().timers();
            auto time_point = ctx.epoch + std::chrono::hours(1);
            dispatcher->dispatch(time_point);
        })
    THEN ("The timer is triggered once")
     AND ("The timer is inactive")

  SCENARIO("A periodic timer that is not expired does not trigger")
    GIVEN("A periodic 5 second timer")
    WHEN ("The timer is triggered at 4 seconds", [](context& ctx) {
            auto& dispatcher = mud::core::event_loop::global().timers();
            auto time_point = ctx.epoch + std::chrono::seconds(4);
            dispatcher->dispatch(time_point);
        })
    THEN ("The timer is not triggered")

  SCENARIO("A once-off timer that is not expired does not trigger")
    GIVEN("A once-off timer in 1 hour")
    WHEN ("The timer is triggered in 30 miutes", [](context& ctx) {
            auto& dispatcher = mud::core::event_loop::global().timers();
            auto time_point = ctx.epoch + std::chrono::minutes(30);
            dispatcher->dispatch(time_point);
        })
    THEN ("The timer is not triggered")

  SCENARIO("A periodic timer is triggered through the event-loop")
    GIVEN("A running event loop")
      AND("A periodic 1 second timer")
     WHEN("Two and a half seconds have elapsed", [](context&) {
            std::this_thread::sleep_for(std::chrono::milliseconds(2500));
        })
    THEN ("The timer is triggered twice")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
