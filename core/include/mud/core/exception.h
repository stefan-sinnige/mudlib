#ifndef _MUDLIB_CORE_EXCEPTION_H_
#define _MUDLIB_CORE_EXCEPTION_H_

#include <exception>
#include <iostream>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief Reporting errors where the object is not an owner of a resource.
 *
 * Typically being used with @c unique_ptr objects where the object that is
 * accessing the underlying resource does not have the ownership.
 */
class not_owner: public std::exception
{
public:
    /**
     * @brief Default constructor.
     */
    not_owner();

    /**
     * @brief Destructor.
     */
    virtual ~not_owner();
};

inline
not_owner::not_owner()
{
}

inline
not_owner::~not_owner()
{
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_EXCEPTION_H_ */
