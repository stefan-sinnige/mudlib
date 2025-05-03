#ifndef _MUDLIB_IO_INTERFACE_H_
#define _MUDLIB_IO_INTERFACE_H_

#include <mud/io/ns.h>
#include <mud/io/ip.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_IO_NS

/**
 * The network interfaces.
 */

namespace ip {

    /**
     * @brief An IP network interface address configruation.
     *
     * @details
     * The IP network address is associated to a device and defines its IP based
     * configuration. This includes items like the address, netmask and state.
     * A single device may have multiple interfaces assigned.
     */
    class address_configuration
    {
    public:
        /**
         * @brief Create an empty (and invalid) configuration.
         */
        address_configuration();

        /**
         * @brief Copy an address configuration.
         * @param other The configuration to copy from.
         */
        address_configuration(const address_configuration& other) = default;

        /**
         * @brief Move an address configuration.
         * @param other The configuration to move from.
         */
        address_configuration(address_configuration&& other) = default;

        /**
         * @brief Copy an address configuration.
         * @param other The configuration to copy from.
         */
        address_configuration& operator=(const address_configuration& other)
                = default;

        /**
         * @brief Move an address configuration.
         * @param other The configuration to move from.
         */
        address_configuration& operator=(address_configuration&& other)
                = default;

        /**
         * @brief Return the IP destination address.
         *
         * @details
         * The destination address is the assigned address to an interface which
         * is used to connect to as point-to-point communication.
         */
        const address& destination() const { return _destination; }

        /**
         * @brief Set the IP destination address.
         * @param value The destination address to set.
         *
         * @details
         * The destination address is the assigned address to an interface which
         * is used to connect to as point-to-point communication.
         */
        void destination(const address& value) { _destination = value; }

        /**
         * @brief Return the IP netmask address.
         *
         * @details
         * The netmask address is the assigned address mask that is used by the
         * IP routing mechanism to route traffic to the address that passes the
         * netmask.
         */
        const address& netmask() const { return _netmask; }

        /**
         * @brief Set the IP netmask address.
         * @param value The netmask address to set.
         *
         * @details
         * The netmask address is the assigned address mask that is used by the
         * IP routing mechanism to route traffic to the address that passes the
         * netmask.
         */
        void netmask(const address& value) { _netmask = value; }

        /**
         * @brief Return the IP broadcast address.
         *
         * @details
         * The broadcast address is the assigned address that is used by the
         * IP routing mechanism to route traffic to all devices on that network
         * segment.
         */
        const address& broadcast() const { return _broadcast; }

        /**
         * @brief Set the IP netmask address.
         * @param value The netmask address to set.
         *
         * @details
         * The netmask address is the assigned address mask that is used by the
         * IP routing mechanism to route traffic to the address that passes the
         * netmask.
         */
        void broadcast(const address& value) { _broadcast = value; }

        /**
         * @brief Return true if the interface is a loopback device.
         */
        bool loopback() const { return _loopback; }

        /**
         * @brief Set the loopback status of the interface.
         * @param value Set to @true if the interface is a loopback device.
         */
        void loopback(bool value) { _loopback = value; }

        /**
         * @brief Return true if the interface is up.
         */
        bool up() const { return _up; }

        /**
         * @brief Set the connectivity status of the interface.
         * @param value Set to @true if the interface is up.
         */
        void up(bool value) { _up = value; }

        /**
         * @brief Return true if the interface is running.
         */
        bool running() const { return _running; }

        /**
         * @brief Set the running status of the interface.
         * @param value Set to @true if the interface is running.
         */
        void running(bool value) { _running = value; }

        /**
         * @brief Return true if the interface supports multicast.
         */
        bool multicast() const { return _multicast; }

        /**
         * @brief Set the multicast support status of the interface.
         * @param value Set to @true if the interface supports multicast.
         */
        void multicast(bool value) { _multicast = value; }

        /**
         * @brief Check if the address is addressable through this interface's
         * network.
         *
         * @details
         * Returns @c true if the @c addr is part of the same network as this
         * interface configuration. In other words, the address when masked
         * by the @c netmask produces the same network id as the @c destination
         * address masked by the @c netmask.
         */
        bool addressable(const address& addr) const;

    private:
        /** The destination address */
        address _destination;

        /** The netmask address */
        address _netmask;

        /** The broadcast address */
        address _broadcast;

        /** Flag if the interface is a loopback device */
        bool _loopback;

        /** Flag if the interface is up */
        bool _up;

        /** Flag if the interface is running */
        bool _running;

        /** Flag if the interface supports multicast */
        bool _multicast;
    };


    /**
     * @brief An IP network interface.
     *
     * @details
     * The IP network interface is a physical or virtual interface of the local
     * system and may contain multiple IP addresses. A system typically would
     * have a number of device, including the loopback device.
     */
    class interface
    {
    public:
        /**
         * @brief Create an IP network interface by its name.
         * @param name The name if the device.
         *
         * @details
         * Commonly used names for UNIX based systems are @c lo for loopback
         * devices; @c eth0 or @c en0 for physical network devices and @c
         * bridge0 for bridged devices. The name is highly associated to the
         * operating system and should not be relied upon literaaly, other than
         * a means of grouping addresses to an interface.
         */
        interface(const std::string& name);

        /**
         * @brief Return the interface name.
         */
        const std::string& name() const { return _name; }

        /**
         * @brief Return the list of addresses assigned to the interface.
         */
        const std::vector<address_configuration>& addresses() const {
            return _addresses;
        }

        /**
         * @brief Add an address configuration.
         * @param config The configuration to add.
         */
        void push_back(const address_configuration& config) {
            _addresses.push_back(config);
        }

        /**
         * @brief Return all address of the local system.
         */
        static std::vector<interface> interfaces();

    private:
        /** The name of the device */
        std::string _name;

        /** The list of addresses */
        std::vector<address_configuration> _addresses;
    };

} // namespace ip

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_INTERFACE_H_ */
