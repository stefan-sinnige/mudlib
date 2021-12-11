#include <stdio.h>
#include <future>
#include <mud/test.h>
#include <mud/core/task.h>

/* The delay to be applied to each test-run in order to visually verify the
 * affect of the test-cases. The delay is specified in milliseconds. */
int g_delay = 100;

/* The task queue for the test-cases to run the application object on the
 * main thread.  */
mud::core::simple_task_queue g_app_queue;

void
help(int retval)
{
    std::cout <<
            "Command line options:\n"
            "  --test <spec>    Specify the test(s) to run:\n"
            "                       feature[#<scenario>]\n"
            "  --delay <msecs>  Wait for <msecs> at the end of each scenario\n"
            "  --help           Show this help\n";
    exit(retval);
}

int
main(int argc, char** argv)
{
    /* Flag to indicate to keep on running the app-queue run-loop. */
    bool running = true;

    /* Parse command line arguments */
    std::string test;
    while (--argc && *(++argv)[0] == '-')
    {
        if (strcmp(*argv, "--test") == 0)
        {
            if (!--argc)
            {
                std::cerr << "Option --test requires a test specification\n";
                help(1);
            }
            else
            {
                test = *(++argv);
            }
        }
        else if (strcmp(*argv, "--delay") == 0)
        {
            if (!--argc)
            {
                std::cerr << "Option --delay requires a test specification\n";
                help(1);
            }
            else
            {
                g_delay = atoi(*(++argv));
            }
        }
        else if (strcmp(*argv, "--help") == 0 || strcmp(*argv, "-h") == 0)
        {
            help(0);
        }
        else
        {
            std::cerr << "Unrecognised command line option(s)\n";
            help(1);
        }
    }
    if (argc)
    {
        std::cerr << "Unrecognised command line option(s)\n";
        help(1);
    }

    /* The application needs to be run on a main thread. This is not so much an
     * issue with Win32 / X11 as these UI threads can run on any thread, but
     * Cocoa in particular requires the UI actions on the main thread.
     *
     * We can therefore not run the test cases on the main thread as that would
     * be assigned to run the application's main run-loop. The test cases will
     * need to be executed in its own thread instead and use a task-queue to
     * the main thread to control the UI applicaition from the test cases. */

    /* Run all features on a separate thread. */
    std::future<std::pair<size_t, size_t>> result;
    result = std::async(std::launch::async, [&]() {
        /* Run the tests */
        std::pair<size_t, size_t> test_result = FEATURE_RUN(test);

        /* instruct the queue to terminate */
        g_app_queue.synchronisation()->terminate();

        return test_result;
    });

    /* Continue to run the task loop. We stop running until the test-cases
     * have run. */
    while (!g_app_queue.synchronisation()->_terminate)
    {
        mud::core::simple_task tsk;
        if (g_app_queue.wait_pop(tsk))
        {
            tsk();
        }
    }

    /* Return 0 upon success */
    auto test_result = result.get();
    return test_result.first == test_result.second ? 0 : 1;
}

/* vi: set ai ts=4 expandtab: */
