#ifndef _MUDLIB_CORE_HANDLE_H_
#define _MUDLIB_CORE_HANDLE_H_

#include <memory>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
#include <mud/core/ns.h>
#if defined(WINDOWS) && defined(NATIVE)
    #include <windows.h>
#endif

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A base representaion of a handle to a resource.
 */
class MUDLIB_CORE_API handle
{
public:
    /**
     * @brief The underlying representation of the resource handle that is
     * associated to the event-loop mechanism.
     */
    enum class type_t {
        SELECT,     /*!< Handle used with @c select mechanisms */
        W32HANDLE,  /*!< Windows @c HANDLE */
        __TEST      /*!< Do not use, for testing purposes one */
    };

    /**
     * Destructor.
     */
    virtual ~handle() = default;

    /**
     * @brief Return the type of the handle.
     */
    type_t type() const;

    /**
     * @brief Return true if the handle is a valid one.
     */
    virtual bool valid() const = 0;

    /**
     * Non-copyable.
     */
    handle(const handle&) = delete;
    handle& operator=(const handle&) = delete;

protected:
    /**
     * Contstruction only permitted through derived classes.
     */
    handle(type_t type);

private:
    /**
     * The type of the handle.
     */
    type_t _type;
};

inline
handle::handle(type_t type)
    : _type(type)
{
}

inline handle::type_t
handle::type() const
{
    return _type;
}

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
template<handle::type_t Type, typename Resource, Resource Invalid>
class basic_handle: public handle
{
public:
    typedef Resource resource_type;

    /**
     * @brief Construct an invalid handle.
     * resource.
     */
    basic_handle();

    /**
     * @brief Construct a handle and associate it to an externally defined
     * resource.
     */
    basic_handle(resource_type h);

    /**
     * @brief Move constructor.
     */
    basic_handle(basic_handle&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~basic_handle();

    /**
     * @brief Return if the handle is valid.
     */
    bool valid() const override;

    /**
     * Non-copyable.
     */
    basic_handle(const basic_handle&) = delete;
    basic_handle& operator=(const basic_handle&) = delete;

private:
    /**
     * @brief Friend class to access internal representation.
     */
    template<typename R>
    friend R internal_handle(const std::unique_ptr<handle>&);

    /**
     * @brief Return the externally defined resource.
     */
    operator resource_type();

    /**
     * The underlying handle to the resource;
     */
    resource_type _handle;
};

template<handle::type_t Type, typename Resource, Resource Invalid>
basic_handle<Type, Resource, Invalid>::basic_handle()
    : handle(Type), _handle(Invalid)
{
}

template<handle::type_t Type, typename Resource, Resource Invalid>
basic_handle<Type, Resource, Invalid>::basic_handle(Resource h)
    : handle(Type), _handle(h)
{
}

template<handle::type_t Type, typename Resource, Resource Invalid>
basic_handle<Type, Resource, Invalid>::basic_handle(basic_handle&& rhs)
{
    _handle = rhs._handle;
    rhs._handle = Invalid;
}

template<handle::type_t Type, typename Resource, Resource Invalid>
basic_handle<Type, Resource, Invalid>::~basic_handle()
{
    _handle = Invalid;
}

template<handle::type_t Type, typename Resource, Resource Invalid>
basic_handle<Type, Resource, Invalid>::basic_handle::operator Resource()
{
    return _handle;
}

template<handle::type_t Type, typename Resource, Resource Invalid>
bool
basic_handle<Type, Resource, Invalid>::valid() const
{
    return (_handle != Invalid);
}

/*
 * @brief Default implementation to access internal representation
 * will throw an exception.
 */
template<typename Resource>
Resource
internal_handle(const std::unique_ptr<handle>& handle)
{
    const std::type_info& info = typeid(Resource);
    std::stringstream msg;
    msg << "Conversion of handle to '" << info.name() << "' not implemented";
    throw std::invalid_argument(msg.str());
}

/*
 * @brief Spported handles.
 */

/**
 * @brief: A handle to an @c select resource type.
 */
typedef basic_handle<handle::type_t::SELECT, int, 0> select_handle;
template<> MUDLIB_CORE_API int internal_handle<int>(const
        std::unique_ptr<handle>&);

/**
 * @brief: A handle to a windows @c HANDLE resource tpe.
 */
#ifdef WINDOWS
    typedef basic_handle<handle::type_t::W32HANDLE, HANDLE, nullptr> windows_handle;
    template<> MUDLIB_CORE_API HANDLE internal_handle<HANDLE>
    (const std::unique_ptr<handle>&);
#endif

/**
 * @brief: A handle to an @c test resource type.
 */
typedef basic_handle<handle::type_t::__TEST, int, 0> __test_handle;
template<> MUDLIB_CORE_API int internal_handle<int>(const
        std::unique_ptr<handle>&);

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_HANDLE_H_ */
