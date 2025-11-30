#ifndef _MUDLIB_CORE_TEST_MECHANISM_H_
#define _MUDLIB_CORE_TEST_MECHANISM_H_

#include <list>
#include <map>
#include <mud/core/handle.h>
#include <mud/core/event_mechanism.h>

/*
 * Simulation of simple OS handle with a state to indicate if there is
 * something to read or not.
 */
enum class os_handle_signal
{
    NONE,
    READY_TO_READ
};

/*
 * A test resource
 */
class test_resource
{
public:
    /* Constructor, creating a UDP socket. */
    test_resource();

    /* Destructor */
    ~test_resource();

    /* Write a character */
    void write(char ch);

    /* Read a character */
    char read();

    /* Return the handle */
    const std::shared_ptr<mud::core::handle>& handle() const;

private:
    /* The character buffer */
    char _ch;

    /* The handle. */
    std::shared_ptr<mud::core::handle> _handle;
};

/*
 * A Test mechanism for the test-resources. The main loop running on the
 * separate thread is implementing a simple polling mechanism.
 */
class test_mechanism : public mud::core::event_mechanism
{
public:
    /* Type definition to the handler type */
    typedef std::function<void(void)> event_handler;

    /* Constructor */
    test_mechanism(
            const std::shared_ptr<mud::core::simple_task_queue>& queue,
            const std::shared_ptr<mud::core::timer_dispatcher>& timers);

    /* Destructor */
    ~test_mechanism();

    /* Register handler */
    void register_handler(const mud::core::event& event) override;

    /* Deregister handler */
    void deregister_handler(const mud::core::event& event) override;

    /* Initiate */
    std::shared_future<void> initiate() override;

    /* Terminate */
    void terminate() override;

private:
    /* Thread function */
    void loop();

    std::list<mud::core::event> _events;
    std::mutex _lock;
    std::atomic_bool _running;
    std::thread _thread;
    std::promise<void> _promise;
    std::shared_future<void> _future;
};

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_TEST_MECHANISM_H_ */
