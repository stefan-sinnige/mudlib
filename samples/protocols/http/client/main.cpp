#include <iostream>
#include <string>
#include <string.h>
#include <utility>
#include <vector>
#include <mud/core/uri.h>
#include <mud/event/event_loop.h>
#include <mud/http/client.h>
#include <mud/http/request.h>
#include <mud/http/response.h>

// ===========================================================================
// Main
// ===========================================================================

int
main(int argc, char** argv)
{
    // Any user-defined header fields
    std::vector<std::pair<std::string, std::string>> fields;

    // Command line processing
    while (--argc > 0 && *(++argv)[0] == '-') {
        if ((strcmp(*argv, "--help") == 0) || (strcmp(*argv, "-h") == 0)) {
            std::cout << 
R"(Usage: sample_http_client [options] URL" << std::endl
Options:
   --header FIELD:VALUE  Add a header FIELD with a VALUE
   --help, -h            Show this help)"
                      << std::endl;
            return 0;
        }
        else
        if (strcmp(*argv, "--header") == 0) {
            --argc, ++argv;
            if (argc < 2) {
                std::cerr << "Missing argument for '--header'\n";
                return 1;
            }
            const char* sep = strchr(*argv, ':');
            if (sep == nullptr) {
                std::cerr << "Missing ':' separator for '--header'\n";
                return 1;
            }
            fields.emplace_back(std::make_pair(
                std::string(*argv, sep - *argv),
                std::string(sep+1)));
        }
        else {
            std::cerr << "Unknown option '" << *argv << "'\n";
            return 1;
        }
    }

    // We should have one option left, the request itself
    if (argc != 1) {
        std::cerr << "Expected request URL" << std::endl;
        return 1;
    }
    mud::core::uri uri(*argv);

    // Create the request to send
    mud::http::request request;
    request.method(mud::http::method_e::GET);
    request.uri(uri);
    for (auto field: fields) {
        mud::http::field_ext ext(field.first);
        ext.value(field.second);
        request.field(ext);
    }

    // Create the event-loop
    std::thread event_thread = std::thread([]() {
        mud::event::event_loop::global().loop();
    });

    // Create the client to send the request and receive the response.
    mud::http::client client;
    mud::io::tcp::endpoint endpoint(uri.host(), uri.port());
    std::future<mud::http::response> response = client.request(
            endpoint, request);

    // Expect a respons within 5 seconds and print it out.
    if (std::future_status::ready !=
        response.wait_for(std::chrono::milliseconds(5000)))
    {
        std::cerr << "Unable to get a response within 5 seconds\n";
    }
    else
    {
        std::cout << response.get() << std::endl;
    }

    // Stop the event loop and exit
    mud::event::event_loop::global().terminate();
    event_thread.join();
    return 0;
}

/* vi: set ai ts=4 expandtab: */
