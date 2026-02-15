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

#ifndef _MUDLIB_CORE_TASK_H_
#define _MUDLIB_CORE_TASK_H_

#include <chrono>
#include <condition_variable>
#include <exception>
#include <future>
#include <memory>
#include <mud/core/ns.h>
#include <mutex>
#include <queue>
#include <thread>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A queue of tasks that can be accessed asynchroneously.
 * @details
 * The task queue is most notably used with the @c task_worker_pool and its @c
 * task_worker threads and shared amongst them. The @c task_queue provides the
 * mechanism to safely push new tasks onto the queue and pop them from the
 * queue in a multi-threaded environment.
 */
template<typename Task>
class task_queue
{
public:
    /**
     * @brief Type definition of the task staored in the queue.
     */
    typedef std::packaged_task<Task> task_type;

    /**
     * @brief Constructor, creating an empty queue.
     */
    task_queue();

    /**
     * @brief Destructor.
     */
    virtual ~task_queue();

    /**
     * @brief Push a new task to the queue.
     */
    void push(task_type&& tsk);

    /**
     * @brief Pop a task off the queue if one is available.
     * @param tsk [out] The task if it has been popped.
     * @return True if a task has been popped. The task is available in @c tsk.
     */
    bool pop(task_type& tsk);

    /**
     * @brief Wait until a task is available and pop it off the queue.
     * @param tsk [out] The task if it has been popped.
     * @return True if a task has been popped. The task is available in @c tsk.
     *
     * If there is no task available on the queue, wait until one becomes
     * available, or until the synchronsiation object is flagged to terminate.
     *
     * If a task has been popped, it is moved to the @tsk object and @c true
     * is returned. If there is no task available, @c false is returned.
     */
    bool wait_pop(task_type& tsk);

    /**
     * @brief Initiate the task queue.
     * @details
     * A task queue can be inititiated, even if it has been terminated before.
     */
    void initiate();

    /**
     * @brief Terminate the task queue.
     * @details
     * Terminate the task queue and notify any thread that actively waited for
     * a task in the queue (see @c wait_pop).
     */
    void terminate();

    /**
     * @brief Return the termination status.
     * @return True if the task queue has terminated.
     */
    bool terminated();

    /**
     * Non-copyable.
     */
    task_queue(const task_queue&) = delete;
    task_queue& operator=(const task_queue&) = delete;

    /**
     * Non-moveable.
     */
    task_queue(task_queue&&) = delete;
    task_queue& operator=(task_queue&&) = delete;

private:
    /**
     * @brief Synchronisation between the task-queue and its workers is
     * controlled by a condition variable with two flags. One flag to indicate
     * that there is data available in the queue and another one to indicate
     * that the workers are requested to terminate.
     */
    
    /** The condition variable */
    std::condition_variable _cv;
    
    /** The mutex associated with the condition variable */
    std::mutex _cv_lock;
    
    /** The flag to indicate there is are tasks available in the queue. */
    bool _available;
    
    /** The flag to indicate that the workers are requested to terminate. */
    bool _terminate;

    /** The task queue and its mutext protecting its contents */
    std::queue<task_type> _tasks;
};

template<typename Task>
task_queue<Task>::task_queue()
    : _available(false)
    , _terminate(false)
{
    LOG(log);
    DEBUG(log) << "Creating task queue " << std::endl;
}

template<typename Task>
task_queue<Task>::~task_queue()
{
    LOG(log);
    DEBUG(log) << "Terminating task queue" << std::endl;
    terminate();
}

template<typename Task>
void
task_queue<Task>::push(task_type&& tsk)
{
    LOG(log);
    TRACE(log) << "Pushing task to queue" << std::endl;

    std::lock_guard<std::mutex> lock(_cv_lock);
    _tasks.push(std::move(tsk));
    _available = true;
    _cv.notify_one();
}

template<typename Task>
bool
task_queue<Task>::pop(task_type& tsk)
{
    std::lock_guard<std::mutex> lock(_cv_lock);
    bool popped = false;
    if (!_tasks.empty()) {
        LOG(log);
        TRACE(log) << "Popping task from queue" << std::endl;

        tsk = std::move(_tasks.front());
        _tasks.pop();
        popped = true;
    }
    _available = !_tasks.empty();
    return popped;
}

template<typename Task>
bool
task_queue<Task>::wait_pop(task_type& tsk)
{
    std::unique_lock<std::mutex> lock(_cv_lock);
    _cv.wait(lock, [this] { return _available || _terminate; });
    bool popped = false;
    if (!_terminate) {
        if (!_tasks.empty()) {
            LOG(log);
            TRACE(log) << "Popping task from queue" << std::endl;

            tsk = std::move(_tasks.front());
            _tasks.pop();
            popped = true;
        }
    }
    _available = !_tasks.empty();
    lock.unlock();
    return popped;
}

template<typename Task>
void
task_queue<Task>::initiate()
{
    std::unique_lock<std::mutex> lock(_cv_lock);
    _terminate = false;
}

template<typename Task>
void
task_queue<Task>::terminate()
{
    std::unique_lock<std::mutex> lock(_cv_lock);
    _terminate = true;
    _cv.notify_all();
}

template<typename Task>
bool
task_queue<Task>::terminated()
{
    std::unique_lock<std::mutex> lock(_cv_lock);
    return _terminate;
}

/**
 * @brief A worker thread that execute a task.
 *
 * A worker thread that executes tasks as thet become available on a task
 * queue.
 */
template<typename Task>
class task_worker
{
public:
    /**
     * @brief Type definition of the task to execute.
     */
    typedef std::packaged_task<Task> task_type;

    /**
     * @brief Constructor to associate the worker with a task-queue. The thread
     * is started after successful construction.
     */
    task_worker(std::shared_ptr<task_queue<Task>> queue);

    /**
     * @brief Move constructor.
     */
    task_worker(task_worker&&) = default;

    /**
     * @brief Move assignment.
     */
    task_worker& operator=(task_worker&&) = default;

    /**
     * @brief Destructor
     */
    virtual ~task_worker();

    /**
     * @brief Return the synchronisation future when the worker thread has
     * started.
     * @return The future that is set when the worker thread has started.
     */
    std::shared_future<void> ready() const;

    /**
     * @brief Wait for the worker to thread to finish execution.
     */
    void join();

    /**
     * Non-copyable.
     */
    task_worker(const task_worker&) = delete;
    task_worker& operator=(const task_worker&) = delete;

private:
    /**
     * Run the task.
     */
    void run();

    /** The task queue */
    std::shared_ptr<task_queue<Task>> _queue;

    /** The promise to hold the ready state of the thread. */
    std::promise<void> _promise;

    /** The future to the ready state promise */
    std::shared_future<void> _future;

    /** The thread */
    std::thread _thread;
};

template<typename Task>
task_worker<Task>::task_worker(std::shared_ptr<task_queue<Task>> queue)
    : _queue(queue)
    , _future(_promise.get_future())
{
    _thread = std::thread(&task_worker::run, this);
}

template<typename Task>
task_worker<Task>::~task_worker()
{
    join();
}

template<typename Task>
std::shared_future<void>
task_worker<Task>::ready() const
{
    return _future;
}

template<typename Task>
void
task_worker<Task>::join()
{
    if (_thread.joinable()) {
        _thread.join();
    }
}

template<typename Task>
void
task_worker<Task>::run()
{
    _promise.set_value();
    while (!_queue->terminated()) {
        task_type tsk;
        if (_queue->wait_pop(tsk)) {
            tsk();
        }
    }
}

/**
 * @brief A pool of @task_worker objects that all share one task queue.
 * @details
 * A @c task_worker_pool maintains a queue of tasks and a number of @c
 * task_worker threads.
 */
template<typename Task>
class task_worker_pool
{
public:
    /**
     * Type definition of the task to execute.
     */
    typedef std::packaged_task<Task> task_type;

    /**
     * Constructor, associating a queue to the pool and the number of task
     * workers.
     */
    task_worker_pool(std::shared_ptr<task_queue<Task>> queue,
                     size_t nr_workers = std::thread::hardware_concurrency());

    /** Destructor */
    virtual ~task_worker_pool();

    /**
     * @brief Start the task queue and all the task workers in the pool.
     */
    void initiate();

    /**
     * @brief Terminate all the task workers in the pool by terminating the
     * associated task queue.
     */
    void terminate();

    /**
     * @brief Wait for all the pool workers to have terminated.
     *
     * The termination signal is
     */
    void wait();

    /**
     * Non-copyable.
     */
    task_worker_pool(const task_worker_pool&) = delete;
    task_worker_pool& operator=(const task_worker_pool&) = delete;

    /**
     * Non-moveable.
     */
    task_worker_pool(task_worker_pool&&) = delete;
    task_worker_pool& operator=(task_worker_pool&&) = delete;

private:
    /** The task workers and the queue */
    size_t _nr_workers;
    std::vector<std::unique_ptr<task_worker<Task>>> _pool;
    std::shared_ptr<task_queue<Task>> _queue;
};

template<typename Task>
task_worker_pool<Task>::task_worker_pool(
    std::shared_ptr<task_queue<Task>> queue, size_t nr_workers)
  : _nr_workers(nr_workers), _queue(queue)
{
}

template<typename Task>
task_worker_pool<Task>::~task_worker_pool()
{
    terminate();
    wait();
}

template<typename Task>
void
task_worker_pool<Task>::initiate()
{
    // Do nothing if already started.
    if (_pool.size() > 0) {
        return;
    }

    LOG(log);
    DEBUG(log) << "Initiating task worker pool with " << _nr_workers
               << " workers" <<std::endl;

    // Initiate the queue
    _queue->initiate();

    // Create task-worker threads
    try {
        for (int i = 0; i < _nr_workers; ++i) {
            _pool.push_back(std::unique_ptr<task_worker<Task>>(
                new task_worker<Task>(_queue)));
        }
        for (auto& worker: _pool) {
            worker->ready().wait();
        }
    } catch (...) {
        _queue->terminate();
        throw;
    }
}

template<typename Task>
void
task_worker_pool<Task>::terminate()
{
    // Terminate the queue - this should in turn terminate all task_workers.
    _queue->terminate();
}

template<typename Task>
void
task_worker_pool<Task>::wait()
{
    for (auto& worker : _pool) {
        worker->join();
    }
    _pool.clear();
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_TASK_H_ */
