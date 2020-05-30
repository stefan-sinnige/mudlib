#ifndef _MUDLIB_IO_KERNEL_HANDLE_H_
#define _MUDLIB_IO_KERNEL_HANDLE_H_

#include <memory>
#include <mud/io/ns.h>
#include <mud/core/handle.h>

BEGIN_MUDLIB_IO_NS

/**
 * @brief A handle to an operating system (kernel) resource.
 *
 * A handle is a reference to an operating system resource. Depending on the
 * underlying operating system, this can be a multitude of types of resources,
 * including files, sockets, pipes and timers. When a handle is associated
 * to a resource (either created directly through a specific object of the
 * handle inheritance tree, or indirectly by assiging an externally acquired
 * resource to an instance of a handle), the handle will own the resource and
 * shall be released only when the handle is destructed.
 *
 * As a resource is associated to a single handle, the handle object is often
 * used in combination with a @c std::unique_ptr to ensure unique access.
 *
 * Once a resource is assigned to a @c kernel_handle, the @c kernel_handle
 * becomes the owner of that resource. This means that a @c kernel_handle
 * cannot be copied. For this purpose, the @c kernel_handle class (and all its
 * inherited classes) are not copyable.
 */
typedef mud::core::basic_handle<int> kernel_handle;

END_MUDLIB_IO_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_IO_KERNEL_HANDLE_H_ */
