#ifndef _MUDLIB_CORE_HANDLE_H_
#define _MUDLIB_CORE_HANDLE_H_

#include <memory>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A generic template to define a unique handle to a resource.
 *
 * A handle is an abstract reference to a unique resource. This can be a kernel
 * resource, like a reference to a file, a socket or a pipe. But can also
 * refer to a resource on any other level, like a reference to a window on a
 * graphics display. The @basic_handle only provides a very basic concept of
 * uniqueness of a particular type.
 *
 * As a resource is associated to a single handle, the handle object is often
 * used in combination with a @c std::unique_ptr to ensure unique access.
 */
template<typename Type>
class basic_handle
{
public:
    typedef Type handle_type;

    /**
     * @brief Construct a handle and associate it to an externally defined
     * resource.
     */
    basic_handle(handle_type h);

    /**
     * @brief Move constructor.
     */
    basic_handle(basic_handle&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~basic_handle();

    /**
     * @brief Return the externally defined resource.
     */
    operator handle_type();

    /**
     * @brief Return if the handle is valid.
     */
    bool valid();

    /**
     * Non default constructable.
     */
    basic_handle() = delete;

    /**
     * Non-copyable.
     */
    basic_handle(const basic_handle&) = delete;
    basic_handle& operator=(const basic_handle&) = delete;

private:
    /**
     * The underlying handle to the resource;
     */
    handle_type _handle;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_HANDLE_H_ */
