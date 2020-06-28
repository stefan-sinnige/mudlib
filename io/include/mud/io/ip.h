#ifndef _MUDCOMMS_IP_H_
#define _MUDCOMMS_IP_H_

#include <string>
#include <netinet/in.h>
#include <mud/io/ns.h>

BEGIN_MUDLIB_IO_NS

/**
 * The Internet Protocol Layer (IPv4).
 */

namespace ip {

/**
 * @brief The IPv4 address representation.
 *
 * The IPv4 address is a 32-bit structure to represent a network class or a
 * host identifier.
 */
class address
{
public:
    /**
     * @brief Construct an any-address (0.0.0.0)
     */
    address();

    /**
     * @brief Address specified by 32-bits address.
     * @param addr [in] The 32-bit address (network order).
     */
    address(in_addr_t addr);

    /**
     * @brief Address specified by the dotted-decimal notation.
     * @param addr [in] The address as a dotted-decimal.
     */
    address(const std::string& str);

    /**
     * @brief Copy constructor
     */
    address(const address& rhs);

    /**
     * @brief Destructor
     */
    virtual ~address();

    /**
     * @brief Assignment operator
     */
    address& operator=(const address& rhs);

    /**
     * @brief Type conversion to a network-order address.
     */
    operator in_addr_t() const;

    /**
     * @brief Return as a dotted-decimal notation.
     * @return The dotted-decimal notation of the IP address.
     */
    std::string str() const;

private:
    /* The IP address (network-order) */
    in_addr_t m_address;
};

} // namespace ip

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDCOMMS_IP_H_ */

