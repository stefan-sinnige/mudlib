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

#ifndef _MUDLIB_CORE_TEST_MECHANISM_H_
#define _MUDLIB_CORE_TEST_MECHANISM_H_

#include <list>
#include <map>
#include <mud/core/handle.h>
#include <mud/core/event_mechanism.h>
#include <mud/core/message.h>

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
            const std::shared_ptr<mud::core::task_queue<void(void)>>& queue,
            const std::shared_ptr<mud::core::timer_dispatcher>& timers);

    /* Destructor */
    ~test_mechanism();

    /* Add an event */
    void add(mud::core::event&& event) override;

    /* Remove and event */
    void remove(const mud::core::event& event) override;

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
