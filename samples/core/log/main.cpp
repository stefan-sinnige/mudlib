#include <mud/core/log.h>
#include <future>

// ===========================================================================
// Main
// ===========================================================================

int
main(int argc, char** argv)
{
    mud::core::log::target_t targets = mud::core::log::target_t::none;
    mud::core::log::severity_t severity = mud::core::log::severity_t::info;
    mud::core::log::format_t format = mud::core::log::format_t::none;
    std::filesystem::path file_path;
    int nr_threads = 0;
    std::vector<std::future<void>> threads;

    // Command line processing
    while (--argc > 0 && *(++argv)[0] == '-') {
        if ((strcmp(*argv, "--help") == 0) || (strcmp(*argv, "-h") == 0)) {
            std::cout << 
R"(Usage: sample_log [options]" << std::endl
Options:
  TARGETS AND SEVERITY:
     --console             Enable console logging
     --file <PATH>         Enable file logging to PATH
     --severity <SEVERITY> The minimum severity to log. This can be
                               TRACE, DEBUG, INFO, WARN, ERROR or FATAL

  FORMAT:
     --format-timestamp    Add timestamp logging
     --format-severity     Add severity logging
     --format-thread       Add thread logging

  OTHER
     --threads NR          Start logging from NR threads
     --help, -h            Show this help)"
                      << std::endl;
            return 0;
        }
        else
        if (strcmp(*argv, "--console") == 0) {
            targets = targets | mud::core::log::target_t::console;
        }
        else
        if (strcmp(*argv, "--file") == 0) {
            --argc, ++argv;
            if (argc < 1) {
                std::cerr << "Error: missing argument for '--file'\n";
                return 1;
            }
            targets = targets | mud::core::log::target_t::file;
            file_path = *argv;
        }
        else
        if (strcmp(*argv, "--severity") == 0) {
            --argc, ++argv;
            if (argc < 1) {
                std::cerr << "Error: missing argument for '--severity'\n";
                return 1;
            }
            if (strcmp(*argv, "TRACE") == 0) {
                severity = mud::core::log::severity_t::trace;
            }
            else
            if (strcmp(*argv, "DEBUG") == 0) {
                severity = mud::core::log::severity_t::debug;
            }
            else
            if (strcmp(*argv, "INFO") == 0) {
                severity = mud::core::log::severity_t::info;
            }
            else
            if (strcmp(*argv, "WARN") == 0) {
                severity = mud::core::log::severity_t::warn;
            }
            else
            if (strcmp(*argv, "ERROR") == 0) {
                severity = mud::core::log::severity_t::error;
            }
            else
            if (strcmp(*argv, "FATAL") == 0) {
                severity = mud::core::log::severity_t::fatal;
            }
            else {
                std::cerr << "Error: unknown severity '" << *argv << "'\n";
                return 1;
            }
        }
        else
        if (strcmp(*argv, "--format-timestamp") == 0) {
            format = format | mud::core::log::format_t::timestamp;
        }
        else
        if (strcmp(*argv, "--format-severity") == 0) {
            format = format | mud::core::log::format_t::severity;
        }
        else
        if (strcmp(*argv, "--format-thread") == 0) {
            format = format | mud::core::log::format_t::thread;
        }
        else
        if (strcmp(*argv, "--threads") == 0) {
            --argc, ++argv;
            if (argc < 1) {
                std::cerr << "Error: missing argument for '--threads'\n";
                return 1;
            }
            nr_threads = atoi(*argv);
        }
        else {
            std::cerr << "Error: unknown option '" << *argv << "'\n";
            return 1;
        }
    }

    // Setup the application logging options
    mud::core::log::targets(targets, file_path);
    mud::core::log::severity(severity);
    mud::core::log::formats(format);

    // Start a number of threads that perform logging
    for (int i = 0; i< nr_threads; ++i) {
        threads.push_back(std::async(std::launch::async, []{
            mud::core::log l;
            l.trace() << "This is a trace message" << std::endl;
            l.debug() << "This is a debug message" << std::endl;
            l.info() << "This is an informational message" << std::endl;
            l.warn() << "This is a warning message" << std::endl;
            l.error() << "This is an error message" << std::endl;
            l.fatal() << "This is a fatal error message" << std::endl;
        }));
    }

    // Always log from the main thread.
    mud::core::log l;
    l.trace() << "This is a trace message" << std::endl;
    l.debug() << "This is a debug message" << std::endl;
    l.info() << "This is an informational message" << std::endl;
    l.warn() << "This is a warning message" << std::endl;
    l.error() << "This is an error message" << std::endl;
    l.fatal() << "This is a fatal error message" << std::endl;

    // Wait for all the threads
    for (auto& f: threads) {
        f.wait();
    }

    return 0;
}

/* vi: set ai ts=4 expandtab: */
