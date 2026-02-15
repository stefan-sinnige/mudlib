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

#include "mud/core/task.h"
#include "mud/test.h"
#include <memory>
#include <stdexcept>
#include <thread>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
        if (queue != nullptr) {
            queue->terminate();
        }
    }

    /* Operating system resource */
    std::thread thread;

    /* Simple task */
    std::packaged_task<void(void)> simple_task;
    std::future<void> void_future;

    /* Int task (parameter and return) */
    std::packaged_task<int(int, int)> int_task;
    std::future<int> int_future;

    /* The task queue object */
    std::shared_ptr<mud::core::task_queue<void(void)>> queue;

    /* Task worker and task worker pool */
    std::unique_ptr<mud::core::task_worker<void(void)>> worker;
    std::unique_ptr<mud::core::task_worker_pool<void(void)>> pool;

END_CONTEXT()

FEATURE("Task")
    DEFINE_GIVEN ("A simple task",
      [](context& ctx){
          std::packaged_task<void(void)> tsk([](){});
          ctx.simple_task   = std::move(tsk);
          ctx.void_future = ctx.simple_task.get_future();
      })
    DEFINE_GIVEN ("A task with a lambda expression",
      [](context& ctx){
          std::packaged_task<int(int, int)> tsk([](int i, int j) {
              return i*j;
          });
          ctx.int_task   = std::move(tsk);
          ctx.int_future = ctx.int_task.get_future();
      })
    DEFINE_GIVEN ("A task with a bind expression",
      [](context& ctx){
          std::packaged_task<void(void)> tsk(
              std::bind([](int i, int j) { int r = i*j; }, 2, 3));
          ctx.simple_task   = std::move(tsk);
          ctx.void_future = ctx.simple_task.get_future();
      })
    DEFINE_GIVEN ("A task which throws an exception",
      [](context& ctx){
          std::packaged_task<void(void)> tsk([]{
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
        ctx.queue = std::make_unique<mud::core::task_queue<void(void)>>();
      })
    DEFINE_GIVEN ("A task worker",
      [](context& ctx){
        ctx.queue = std::make_unique<mud::core::task_queue<void(void)>>();
        ctx.worker = std::make_unique<mud::core::task_worker<void(void)>>(
                ctx.queue);
      })
    DEFINE_GIVEN ("A task worker pool",
      [](context& ctx){
        ctx.queue = std::make_unique<mud::core::task_queue<void(void)>>();
        ctx.pool = std::make_unique<mud::core::task_worker_pool<void(void)>>(
                ctx.queue, 4);
        ctx.pool->initiate();
      })
    DEFINE_GIVEN ("A restarted task worker pool",
      [](context& ctx){
        ctx.queue = std::make_unique<mud::core::task_queue<void(void)>>();
        ctx.pool = std::make_unique<mud::core::task_worker_pool<void(void)>>(
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
          std::packaged_task<void(void)> task;
          ASSERT(true, ctx.queue->pop(task));
      })
  END_DEFINES()

  /*
   * The scenarios 
   */

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
                  mud::core::task_queue<void(void)>>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::task_queue<void(void)>>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::task_queue<void(void)>>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::core::task_queue<void(void)>>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::core::task_queue<void(void)>>::value);
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
                  mud::core::task_worker<void(void)>>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::task_worker<void(void)>>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::task_worker<void(void)>>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::task_worker<void(void)>>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::task_worker<void(void)>>::value);
        })

  SCENARIO("Task worker terminates on request")
    GIVEN("A task worker")
    WHEN ("The task worker is requested to terminate", [](context& ctx) {
            ctx.queue->terminate();
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
                  mud::core::task_worker_pool<void(void)>>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::task_worker_pool<void(void)>>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::task_worker_pool<void(void)>>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::core::task_worker_pool<void(void)>>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::core::task_worker_pool<void(void)>>::value);
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

/* clang-format-on */

/* vi: set ai ts=4 expandtab: */
