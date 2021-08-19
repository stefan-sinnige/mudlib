#include <stdio.h>
#include <mud/test.h>

int g_delay = 0;

void
help(int retval)
{
    std::cout <<
            "Command line options:\n"
            "  --test <spec>   Specify the test(s) to run:\n"
            "                      feature[#<scenario>]\n"
            "  --delay <secs>  Wait for <secs> at the end of each scenario\n"
            "  --help          Show this help\n";
    exit(retval);
}

int
main(int argc, char** argv)
{
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

    /* Run all features */
    std::pair<size_t, size_t> result = FEATURE_RUN(test);

    /* Return 0 upon success */
    return result.first == result.second ? 0 : 1;
}

/* vi: set ai ts=4 expandtab: */
