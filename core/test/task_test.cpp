#include "mud/core/task.h"
#include "mud/test.h"
#include <memory>
#include <thread>
#include <type_traits>
#include <stdexcept>

/* *INDENT-OFF* */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
        if (queue != nullptr) {
            queue->synchronisation()->terminate();
        }
    }

    /* Operating system resource */
    std::thread thread;

    /* Simple task */
    mud::core::simple_task simple_task;
    std::future<void> void_future;

    /* Int task (parameter and return) */
    mud::core::task<int(int, int)> int_task;
    std::future<int> int_future;

    /* The task queue object */
    std::shared_ptr<mud::core::simple_task_queue> queue;

    /* Task worker and task worker pool */
    std::unique_ptr<mud::core::simple_task_worker> worker;
    std::unique_ptr<mud::core::simple_task_worker_pool> pool;

END_CONTEXT()

FEATURE("Task")
    DEFINE_GIVEN ("A simple task",
      [](context& ctx){
          mud::core::simple_task tsk([](){});
          ctx.simple_task   = std::move(tsk);
          ctx.void_future = ctx.simple_task.get_future();
      })
    DEFINE_GIVEN ("A task with a lambda expression",
      [](context& ctx){
          mud::core::task<int(int, int)> tsk([](int i, int j) {
              return i*j;
          });
          ctx.int_task   = std::move(tsk);
          ctx.int_future = ctx.int_task.get_future();
      })
    DEFINE_GIVEN ("A task with a bind expression",
      [](context& ctx){
          mud::core::simple_task tsk(
              std::bind([](int i, int j) { int r = i*j; }, 2, 3));
          ctx.simple_task   = std::move(tsk);
          ctx.void_future = ctx.simple_task.get_future();
      })
    DEFINE_GIVEN ("A task which throws an exception",
      [](context& ctx){
          mud::core::simple_task tsk([](){
              throw std::runtime_error("Some runtime error");});
          ctx.simple_task   = std::move(tsk);
          ctx.void_future = ctx.simple_task.get_future();
      })
    DEFINE_GIVEN ("Assigned to a thread",
      [](context& ctx){
          ctx.thread = std::move(std::thread(std::move(ctx.simple_task)));
      })
    DEFINE_GIVEN ("A task queue",
      [](context& ctx){
        ctx.queue = std::make_unique<mud::core::simple_task_queue>();
      })
    DEFINE_GIVEN ("A task worker",
      [](context& ctx){
        ctx.queue = std::make_unique<mud::core::simple_task_queue>();
        ctx.worker = std::make_unique<mud::core::simple_task_worker>(
                ctx.queue);
      })
    DEFINE_GIVEN ("A task worker pool",
      [](context& ctx){
        ctx.queue = std::make_unique<mud::core::simple_task_queue>();
        ctx.pool = std::make_unique<mud::core::simple_task_worker_pool>(
                ctx.queue, 4);
        ctx.pool->initiate();
      })
    DEFINE_GIVEN ("A restarted task worker pool",
      [](context& ctx){
        ctx.queue = std::make_unique<mud::core::simple_task_queue>();
        ctx.pool = std::make_unique<mud::core::simple_task_worker_pool>(
                ctx.queue, 4);
        ctx.pool->initiate();
        ctx.pool->terminate();
        ctx.pool->wait();
        ctx.pool->initiate();
      })
    DEFINE_WHEN ("The task is executed with arguments",
      [](context& ctx) {
          ctx.int_task(2, 3);
      })
    DEFINE_WHEN ("The task is executed without arguments",
      [](context& ctx) {
          ctx.simple_task();
      })
    DEFINE_WHEN ("The thread has run",
      [](context& ctx) {
          ctx.thread.join();
      })
    DEFINE_WHEN ("The task is pushed",
      [](context& ctx) {
          ctx.queue->push(std::move(ctx.simple_task));
      })
    DEFINE_THEN ("The result is available in the future",
      [](context& ctx) {
          ASSERT(6, ctx.int_future.get());
      })
    DEFINE_THEN ("The future is signaled",
      [](context& ctx) {
          ctx.void_future.get();
      })
    DEFINE_THEN ("The future signals the exception",
      [](context& ctx) {
          ASSERT_THROW(std::runtime_error, ctx.void_future.get());
      })
    DEFINE_THEN ("The task can be popped",
      [](context& ctx) {
          mud::core::simple_task task;
          ASSERT(true, ctx.queue->pop(task));
      })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Task type traits")
    GIVEN("A task type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::simple_task>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::simple_task>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::simple_task>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::simple_task>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::simple_task>::value);
        })

  SCENARIO("Executing a simple task")
    GIVEN("A simple task")
    WHEN ("The task is executed without arguments")
    THEN ("The future is signaled")

  SCENARIO("Executing a task using a lambda expression")
    GIVEN("A task with a lambda expression")
    WHEN ("The task is executed with arguments")
    THEN ("The result is available in the future")

  SCENARIO("Executing a task using a bind expression")
    GIVEN("A task with a bind expression")
    WHEN ("The task is executed without arguments")
    THEN ("The future is signaled")

  SCENARIO("Executing a task within a thread")
    GIVEN("A task with a bind expression")
      AND("Assigned to a thread")
    WHEN ("The thread has run")
    THEN ("The future is signaled")

  SCENARIO("Executing a task throwing an exception")
    GIVEN ("A task which throws an exception")
      AND("Assigned to a thread")
    WHEN ("The thread has run")
    THEN ("The future signals the exception")

  SCENARIO("Task Queue type traits")
    GIVEN("A task queue type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::simple_task_queue>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::simple_task_queue>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::simple_task_queue>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::core::simple_task_queue>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::core::simple_task_queue>::value);
        })

  SCENARIO("Pushing and popping a task into a queue")
    GIVEN("A task queue")
      AND("A simple task")
    WHEN ("The task is pushed")
    THEN ("The task can be popped")

  SCENARIO("Task worker type traits")
    GIVEN("A task worker type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::core::simple_task_worker>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::simple_task_worker>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::simple_task_worker>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::simple_task_worker>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::simple_task_worker>::value);
        })

  SCENARIO("Task worker terminates on request")
    GIVEN("A task worker")
    WHEN ("The task worker is requested to terminate", [](context& ctx) {
            ctx.queue->synchronisation()->terminate();
        })
    THEN ("The task worker is terminated", [](context& ctx) {
            ctx.worker->join();
        })

  SCENARIO("Task worker executes a queued task")
    GIVEN("A task worker")
      AND("A simple task")
    WHEN ("The task is pushed")
    THEN ("The future is signaled")

  SCENARIO("Task worker pool type traits")
    GIVEN("A task worker pool type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::core::simple_task_worker_pool>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::simple_task_worker_pool>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::simple_task_worker_pool>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::core::simple_task_worker_pool>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::core::simple_task_worker_pool>::value);
        })

  SCENARIO("Task worker pool executes a queued task")
    GIVEN("A task worker pool")
      AND("A simple task")
    WHEN ("The task is pushed")
    THEN ("The future is signaled")

  SCENARIO("Restarted task worker pool executes a queued task")
    GIVEN("A restarted task worker pool")
      AND("A simple task")
    WHEN ("The task is pushed")
    THEN ("The future is signaled")

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
