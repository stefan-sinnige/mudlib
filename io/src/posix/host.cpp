#include "mud/io/host.h"
#include <atomic>
#include <mutex>
#include <netdb.h>
#include <unistd.h>

BEGIN_MUDLIB_IO_NS

/** The mutex to safeguard initialisation. */
static std::atomic<bool> g_initialised(false);
static std::mutex g_lock;

/** The global data fields */
std::string g_hostname;
std::string g_domainname;
mud::io::ip::address g_address;

/** The initialisation routine. The data is only retrieved once. */
void
initialise()
{
    // If already initialised, we're done.
    if (g_initialised.load()) {
        return;
    }

    // Grab the lock and test if already initialised (again).
    std::lock_guard<std::mutex> lock(g_lock);
    if (g_initialised.load()) {
        return;
    }

    // Get the host name
    char buffer[256];
    buffer[0] = '\0';
    if (gethostname(buffer, sizeof(buffer)) == 0) {
        g_hostname = buffer;
    }

    // Get the domain name
    buffer[0] = '\0';
    if (getdomainname(buffer, sizeof(buffer)) == 0) {
        g_domainname = buffer;
    }

    // Get the default IP address
    struct addrinfo* infos;
    struct addrinfo hint;
    if (getaddrinfo(g_hostname.c_str(), nullptr, nullptr, &infos) == 0) {
        if (infos != nullptr) {
            // Get the first one that is not 0.0.0.0 or 127.0.0.1
            for (auto info = infos; info != nullptr; info = info->ai_next) {
                auto addr = ((struct sockaddr_in*)info->ai_addr)->sin_addr.s_addr;
                if (addr != 0x00000000 && addr != 0x0100007F) {
                    g_address = mud::io::ip::address(addr);
                    break;
                }
            }
        }
        freeaddrinfo(infos);
    }

    // We're done now.
    g_initialised.store(true);
}

const std::string&
host::hostname()
{
    initialise();
    return g_hostname;
}

const std::string&
host::domainname()
{
    initialise();
    return g_domainname;
}
    
const mud::io::ip::address&
host::address()
{
    initialise();
    return g_address;
}

END_MUDLIB_IO_NS

