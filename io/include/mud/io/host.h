#ifndef _MUDLIB_IO_HOST_H_
#define _MUDLIB_IO_HOST_H_

#include <mud/io/ns.h>
#include <mud/io/ip.h>

BEGIN_MUDLIB_IO_NS

/**
 * @brief The host information.
 *
 * @details
 * The @c host class provides information related to the host system.
 */
class host
{
public:
    /**
     * @brief Return the host name.
     */
    static const std::string& hostname();
    
    /**
     * @brief Return the domain name.
     */
    static const std::string& domainname();
    
    /**
     * @brief Return the IP address associated to the host name.
     */
    static const mud::io::ip::address& address();
};

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_HOST_H_ */
