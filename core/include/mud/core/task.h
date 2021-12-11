#ifndef _MUDLIB_CORE_TASK_H_
#define _MUDLIB_CORE_TASK_H_

#include <chrono>
#include <condition_variable>
#include <exception>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * Forward declaration of the template type such that specialisations can be
 * made.
 */
template <class> class task;

/**
 * @brief A representation of an executable task which can be run
 * asynchroneously and returns a non-void result.
 */
template <class Result, class... Args>
class task<Result(Args...)>
{
public:
    /**
     * @brief Type definition of the task function.
     */
    typedef std::function<Result(Args...)> function_type;

    /**
     * @brief Type definition of the result type.
     */
    typedef Result result_type;

    /**
     * @brief An invalid task.
     */
    task();

    /**
     * @brief Constructor of a task function to be executed.
     */
    task(function_type&& fn);

    /**
     * @brief Move constructor.
     */
    task(task&& other);

    /**
     * @brief Move assignment.
     */
    task& operator=(task&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~task();

    /**
     * @brief Execute the task function with the supplied arguments. The
     * result is available through the future.
     */
    void operator()(Args... );

    /**
     * @brief Return the future to access the return upon completion.
     */
    std::future<result_type> get_future();

    /**
     * @brief Return true if the task is valid.
     */
    bool valid() const;

    /**
     * Non-copyable.
     */
    task(const task&) = delete;
    task& operator=(const task&) = delete;

private:
    /**
     * @brief The task function to execute.
     */
    function_type _fn;

    /**
     * @brief The promise to pass the result.
     */
    std::promise<result_type> _promise;

};

template <class Result, class ...Args>
task<Result(Args...)>::task()
    : _fn(nullptr)
{
}

template <class Result, class ...Args>
task<Result(Args...)>::task(function_type&& fn)
    : _fn(std::move(fn))
{
}

template <class Result, class ...Args>
task<Result(Args...)>::task(task&& rhs)
{
    _fn.swap(rhs._fn);
    _promise.swap(rhs._promise);
}

template <class Result, class ...Args>
task<Result(Args...)>&
task<Result(Args...)>::operator=(task&& rhs)
{
    _fn.swap(rhs._fn);
    _promise.swap(rhs._promise);
    return *this;
}

template <class Result, class ...Args>
task<Result(Args...)>::~task()
{
}

template <class Result, class ...Args>
void
task<Result(Args...)>::operator()(Args... args)
{
    try
    {
        _promise.set_value(_fn(std::forward<Args>(args)...));
    }
    catch (...)
    {
        _promise.set_exception(std::current_exception());
    }
}

template <class Result, class ...Args>
std::future<Result>
task<Result(Args...)>::get_future()
{
    return _promise.get_future();
}

template <class Result, class ...Args>
bool
task<Result(Args...)>::valid() const
{
    return (_fn != nullptr);
}

/**
 * @brief A representation of an executable task which can be run
 * asynchroneously and returns a void result.
 */
template <class... Args>
class task<void(Args...)>
{
public:
    /**
     * @brief Type definition of the task function.
     */
    typedef std::function<void(Args...)> function_type;

    /**
     * @brief Type definition of the result type.
     */
    typedef void result_type;

    /**
     * @brief An invalid task.
     */
    task();

    /**
     * @brief Constructor of a task function to be executed.
     */
    task(function_type&& fn);

    /**
     * @brief Move constructor.
     */
    task(task&& other);

    /**
     * @brief Move assignment.
     */
    task& operator=(task&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~task();

    /**
     * @brief Execute the task function with the supplied arguments. The
     * result is available through the future.
     */
    void operator()(Args... );

    /**
     * @brief Return the future to access the return upon completion.
     */
    std::future<result_type> get_future();

    /**
     * @brief Return true if the task is valid.
     */
    bool valid() const;

    /**
     * Non-copyable.
     */
    task(const task&) = delete;
    task& operator=(const task&) = delete;

private:
    /**
     * @brief The task function to execute.
     */
    function_type _fn;

    /**
     * @brief The promise to pass the result.
     */
    std::promise<result_type> _promise;
};

template <class ...Args>
task<void(Args...)>::task()
    : _fn(nullptr)
{
}

template <class ...Args>
task<void(Args...)>::task(function_type&& fn)
    : _fn(std::move(fn))
{
}

template <class ...Args>
task<void(Args...)>::task(task&& rhs)
{
    _fn.swap(rhs._fn);
    _promise.swap(rhs._promise);
}

template <class ...Args>
task<void(Args...)>&
task<void(Args...)>::operator=(task&& rhs)
{
    _fn.swap(rhs._fn);
    _promise.swap(rhs._promise);
    return *this;
}

template <class ...Args>
task<void(Args...)>::~task()
{
}

template <class ...Args>
void
task<void(Args...)>::operator()(Args... args)
{
    try
    {
        _fn(std::forward<Args>(args)...);
        _promise.set_value();
    }
    catch (...)
    {
        _promise.set_exception(std::current_exception());
    }
}

template <class ...Args>
std::future<void>
task<void(Args...)>::get_future()
{
    return _promise.get_future();
}

template <class ...Args>
bool
task<void(Args...)>::valid() const
{
    return (_fn != nullptr);
}

/**
 * @brief Type definition of a simple, self-contained task.
 */
typedef task<void(void)> simple_task;

/**
 * @brief Synchronisation between the task-queue and its workers is controlled
 * by a condition variable with two flags. One flag to indicate that there is
 * data available in the queue and another one to indicate that the workers
 * are requested to terminate.
 */

typedef struct task_queue_synchronisation
{
    /**
     * @brief Create a synchronisation object that is initiated.
     */
    task_queue_synchronisation() {
        _available = false;
        _terminate = false;
    }

    /**
     * @brief Destruct a synchronisation object.
     */
    ~task_queue_synchronisation() = default;

    /**
     * @brief Initiate the synchronisation by resetting the termination flag.
     */
    void
    initiate() {
        std::lock_guard<std::mutex> lock(_cv_lock);
        _terminate = false;
    }

    /**
     * @brief Request all workers to terminate
     */
    void
    terminate() {
        std::lock_guard<std::mutex> lock(_cv_lock);
        _terminate = true;
        _cv.notify_all();
    }

    /** The condition variable */
    std::condition_variable _cv;

    /** The mutex associated with the condition variable */
    std::mutex _cv_lock;

    /** The flag to indicate there is are tasks available in the queue. */
    bool _available;

    /** The flag to indicate that the workers are requested to terminate. */
    bool _terminate;
} task_queue_synchronisation;

/**
 * @brief A queue of tasks that can be accessed asynchroneously. The queue can
 * be shared (e.g. through a @c shared_ptr) with mutlitple threads who can
 * either push new tasks or pop tasks from the queue.
 */
template <typename Task>
class task_queue
{
public:
    /**
     * @brief Type definition of the task staored in the queue.
     */
    typedef Task task_type;

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
     * @brief The reference to the synchronisation object which can be used
     * by consumers of the tasks to be notified when data is available or
     * by requesting waiting consumers to terminate.
     */
    const std::shared_ptr<task_queue_synchronisation>& synchronisation();

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
    /** The task queue and its mutext protecting its contents */
    std::queue<task_type> _tasks;

    /** The task queue synchronisation object */
    std::shared_ptr<task_queue_synchronisation> _sync;
};

template <typename Task>
task_queue<Task>::task_queue()
    : _sync(new task_queue_synchronisation)
{
}

template <typename Task>
task_queue<Task>::~task_queue()
{
    _sync->terminate();
}

template <typename Task>
void
task_queue<Task>::push(task_type&& tsk)
{
    std::lock_guard<std::mutex> lock(_sync->_cv_lock);
    _tasks.push(std::move(tsk));
    _sync->_available = true;
    _sync->_cv.notify_one();
}

template <typename Task>
bool
task_queue<Task>::pop(task_type& tsk)
{
    std::lock_guard<std::mutex> lock(_sync->_cv_lock);
    bool popped = false;
    if (!_tasks.empty()) {
        tsk = std::move(_tasks.front());
        _tasks.pop();
        popped = true;
    }
    _sync->_available = !_tasks.empty();
    return popped;
}

template <typename Task>
bool
task_queue<Task>::wait_pop(task_type& tsk)
{
    std::unique_lock<std::mutex> lock(_sync->_cv_lock);
    _sync->_cv.wait(lock, [this] {
        return _sync->_available || _sync->_terminate;
    });
    bool popped = false;
    if (!_sync->_terminate)
    {
        if (!_tasks.empty()) {
            tsk = std::move(_tasks.front());
            _tasks.pop();
            popped = true;
        }
    }
    _sync->_available = !_tasks.empty();
    lock.unlock();
    return popped;
}

template <typename Task>
const std::shared_ptr<task_queue_synchronisation>&
task_queue<Task>::synchronisation()
{
    return _sync;
}

/**
 * @brief Type definition of a queue for simple, self-contained tasks.
 */
typedef task_queue<task<void(void)>> simple_task_queue;

/**
 * @brief A worker thread that execute a task.
 *
 * A worker thread that executes tasks as thet become available on a task
 * queue.
 */
template <typename Task>
class task_worker: public std::thread
{
public:
    /**
     * @brief Type definition of the task to execute.
     */
    typedef Task task_type;

    /**
     * @brief Constructor to associate the worker with a task-queue. The thread
     * is started after successful construction.
     */
    task_worker(const std::shared_ptr<task_queue<task_type>>& queue);

    /**
     * @brief Move constructor.
     */
    task_worker(task_worker&&);

    /**
     * @brief Move assignment.
     */
    task_worker& operator=(task_worker&&);

    /**
     * @brief Destructor
     */
    virtual ~task_worker();

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
    std::shared_ptr<task_queue<task_type>> _queue;
};

template <typename Task>
task_worker<Task>::task_worker(const std::shared_ptr<task_queue<task_type>>&
        queue)
    : std::thread(&task_worker::run, this),
      _queue(queue)
{
}

template <typename Task>
task_worker<Task>::task_worker(task_worker&& rhs)
    : std::thread(std::move((std::thread&&)rhs)),
      _queue(std::move(rhs._queue))
{
}

template <typename Task>
task_worker<Task>&
task_worker<Task>::operator=(task_worker&& rhs)
{
    std::thread::operator=(std::move((std::thread&&)rhs));
    _queue = std::move(rhs._queue);
}

template <typename Task>
task_worker<Task>::~task_worker()
{
    if (joinable()) {
        join();
    }
}

template <typename Task>
void
task_worker<Task>::run()
{
    while (!_queue->synchronisation()->_terminate)
    {
        task_type tsk;
        if (_queue->wait_pop(tsk))
        {
            tsk();
        }
    }
}

/**
 * @brief Type definition of a worker for simple, self-contained tasks.
 */
typedef task_worker<task<void(void)>> simple_task_worker;

/**
 * @brief A pool of @task_worker objects that all share one task queue.
 */
template<typename Task>
class task_worker_pool
{
public:
    /**
     * Type definition of the task to execute.
     */
    typedef Task task_type;

    /**
     * Constructor, associating a queue to the pool and the number of task
     * workers.
     */
    task_worker_pool(
            const std::shared_ptr<task_queue<task_type>>& queue,
            size_t nr_workers = std::thread::hardware_concurrency());

    /** Destructor */
    virtual ~task_worker_pool();

    /**
     * @brief Start all the task workers in the pool.
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
    std::vector<std::unique_ptr<task_worker<task_type>>> _pool;
    std::shared_ptr<task_queue<task_type>> _queue;
};

template <typename Task>
task_worker_pool<Task>::task_worker_pool(
        const std::shared_ptr<task_queue<task_type>>& queue,
        size_t nr_workers)
    : _nr_workers(nr_workers), _queue(queue)
{
}

template <typename Task>
task_worker_pool<Task>::~task_worker_pool()
{
    terminate();
    wait();
}

template <typename Task>
void
task_worker_pool<Task>::initiate()
{
    // Do nothing if already started.
    if (_pool.size() > 0) {
        return;
    }

    // Ensure the synchronisation is reset
    _queue->synchronisation()->initiate();

    // Create task-worker threads
    try
    {
        for (int i = 0; i < _nr_workers; ++i)
        {
            _pool.push_back(std::unique_ptr<task_worker<task_type>>(
                            new task_worker<task_type>(_queue)));
        }
    }
    catch (...)
    {
        terminate();
        throw;
    }
}

template <typename Task>
void
task_worker_pool<Task>::terminate()
{
    _queue->synchronisation()->terminate();
}

template <typename Task>
void
task_worker_pool<Task>::wait()
{
    for (auto& worker: _pool)
    {
        if (worker->joinable())
        {
            worker->join();
        }
    }
    _pool.clear();
}

/**
 * @brief Type definition of a worker pool for simple, self-contained tasks.
 */
typedef task_worker_pool<task<void(void)>> simple_task_worker_pool;

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_TASK_H_ */
