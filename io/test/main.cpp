#include <stdio.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TextTestProgressListener.h>
#include <cppunit/ui/text/TestRunner.h>

void
help(int retval)
{
    std::cout <<
            "Command line options:\n"
            "  --test <name>   Specify the name of the test(s) to run in the format:\n"
            "                      <class>[::<method>]\n"
            "                  If a <class> is specified, then all test methods of that\n"
            "                  class will be executed.\n"
            "  --help          Show this help\n";
    exit(retval);
}

int
main(int argc, char** argv)
{
    /* Parse command line arguments */
    char* testname = nullptr;
    while (--argc && *(++argv)[0] == '-')
    {
        if (strcmp(*argv, "--test") == 0)
        {
            if (!--argc)
            {
                std::cerr << "Option --test requires a <name>\n";
                help(1);
            }
            else
            {
                testname = *(++argv);
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

    /* The event manager and test controller */
    CppUnit::TestResult controller;

    /* Add a result collector to the controller */
    CppUnit::TestResultCollector result;
    controller.addListener( &result );

    /* Add a text progress to the controller */
    //CppUnit::TextTestProgressListener progress;
    CppUnit::BriefTestProgressListener progress;
    controller.addListener( &progress );

    /* Add the test-runner and add all the test suites */
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry& registry =
            CppUnit::TestFactoryRegistry::getRegistry();
    if (testname != nullptr)
    {
        /* Find the test and add it */
        try
        {
            CppUnit::TestSuite* suite = new CppUnit::TestSuite();
            registry.addTestToSuite(suite);
            CppUnit::Test* test = suite->findTest(testname);
            runner.addTest(test);
        }
        catch (std::invalid_argument ex)
        {
            std::cerr << "Error: test '" << testname << "' "
                    << "could not be found\n";
            return 1;
        }
    }
    else
    {
        /* Add everything */
        runner.addTest( registry.makeTest() );
    }

    /* Run it */
    runner.run(controller);

    /* Print the results in a compiler compatible format */
    CppUnit::CompilerOutputter outputter( &result, std::cerr );
    outputter.write();

    /* Return 0 upon success */
    return result.wasSuccessful() ? 0 : 1;
}

/* vi: set ai ts=4 expandtab: */
