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

#include "mud/io/interface.h"
#include <errno.h>
#include <ifaddrs.h>
#include <net/if.h>
#include <netinet/in.h>
#include <functional>

BEGIN_MUDLIB_IO_NS

ip::address_configuration::address_configuration()
{
}

bool
ip::address_configuration::addressable(const ip::address& addr) const
{
    // Get the network ID of the interface
    uint32_t interface_network_id = (uint32_t)_destination & (uint32_t)_netmask;

    // Get the network ID of the address with the interface's netmask
    uint32_t address_network_id = (uint32_t)addr & (uint32_t)_netmask;

    // The address if part of this interface's network if the network IDs match
    return interface_network_id == address_network_id;
}

ip::interface::interface(const std::string& name)
    : _name(name)
{
}

/* static */ std::vector<ip::interface>
ip::interface::interfaces()
{
    std::vector<ip::interface> ifs;

    // Get a new list of all interface addresses. This also cleans up the list
    // when the function returns.
    struct ifaddrs_auto {
        // Quey all interfaces.
        ifaddrs_auto() {
            if (getifaddrs(&addr) != 0) {
                addr = nullptr;
            }
        }

        // Cleanup
        ~ifaddrs_auto() {
            if (addr != nullptr) {
                freeifaddrs(addrs);
            }
        }

        // List of all addresses.
        struct ifaddrs* addrs;

        // The current address to examine.
        struct ifaddrs* addr;
    } ifaddrs;

    // Iterate over all addresses and process them. This will return one address
    // at a time. Make sure to combine the addresses to each interface.
    while (ifaddrs.addr != nullptr) {
        // Only consider IP ethernet interfaces
        auto sin = (struct sockaddr_in*)ifaddrs.addr->ifa_addr;
        if (sin == nullptr || sin->sin_family != AF_INET) {
            // Move to the next address
            ifaddrs.addr = ifaddrs.addr->ifa_next;
            continue;
        }

        // Get an existing interface from the vector, or add a new one if none
        // could be found.
        auto ifp = std::find_if(ifs.begin(), ifs.end(), [&](auto& item) {
            return item.name() == ifaddrs.addr->ifa_name;
        });
        if (ifp == ifs.end()) {
            ifp = ifs.insert(ifs.end(), ip::interface(ifaddrs.addr->ifa_name));
        }

        // Get the address and netmask configuration
        address_configuration cfg;
        cfg.destination(sin->sin_addr.s_addr);
        sin = (struct sockaddr_in*)ifaddrs.addr->ifa_netmask;
        cfg.netmask(sin->sin_addr.s_addr);

        // Get the optional broadcast
        if (ifaddrs.addr->ifa_flags & IFF_BROADCAST) {
            sin = (struct sockaddr_in*)ifaddrs.addr->ifa_dstaddr;
            if (sin != nullptr) {
                cfg.broadcast(sin->sin_addr.s_addr);
            }
        }

        // Get the status
        cfg.loopback(ifaddrs.addr->ifa_flags & IFF_LOOPBACK);
        cfg.up(ifaddrs.addr->ifa_flags & IFF_UP);
        cfg.running(ifaddrs.addr->ifa_flags & IFF_RUNNING);
        cfg.multicast(ifaddrs.addr->ifa_flags & IFF_MULTICAST);

        // Save the address configuration entry to the interface and move to
        // the next address.
        ifp->push_back(cfg);
        ifaddrs.addr = ifaddrs.addr->ifa_next;
    }

    return ifs;
}

END_MUDLIB_IO_NS

