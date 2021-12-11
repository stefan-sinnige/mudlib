#ifndef _MUDLIB_CORE_HANDLE_H_
#define _MUDLIB_CORE_HANDLE_H_

#include <atomic>
#include <memory>
#include <mud/core/ns.h>
#include <sstream>
#include <stdexcept>
#include <typeinfo>
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
    enum class type_t
    {
        NONE,        /*!< Handle to represent an unknown type. */
        ATOMIC_BOOL, /*!< Handle to represent a boolean state */
        SELECT,      /*!< Handle used with @c select mechanisms */
        W32HANDLE,   /*!< Windows @c HANDLE */
        W32WND,      /*!< Windows @c HWND */
        X11,         /*!< X11 handle */
        COCOA,       /*!< MacOS-X Cocoa Event handle */
        __TEST       /*!< Do not use, for testing purposes one */
    };

    /**
     * @brief A self-signalling resource using the handle's @c resource_type.
     *
     * A self-signalling resource is a resource that is built using the same
     * resource_type and can be used to send and catch triggers.
     */
    class MUDLIB_CORE_API signal
    {
    public:
        /**
         * @brief Constructor.
         *
         * Creating a new self-signalling resource.
         */
        signal() = default;

        /**
         * @brief Destructor.
         *
         * Remove the self-signalling resource and free any underlying
         * implementation dependent resources.
         */
        virtual ~signal() = default;

        /**
         * @brief The handle associated to the resource.
         */
        virtual const std::unique_ptr<mud::core::handle>& handle() const = 0;

        /**
         * @brief Signal the resources.
         *
         * Signal the resource such that it can be captured by any thread
         * waiting for it.
         */
        virtual void trigger() = 0;

        /**
         * @brief Capture the signal's trigger.
         *
         * Capture the signal and return true if it has been caught.
         */
        virtual bool capture() = 0;

        /**
         * Non-copyable.
         */
        signal(const signal&) = delete;
        signal& operator=(const signal&) = delete;

        /**
         * Non-moveable.
         */
        signal(signal&&) = delete;
        signal& operator=(signal&&) = delete;
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

inline handle::handle(type_t type) : _type(type) {}

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
class basic_handle : public handle
{
public:
    typedef Resource resource_type;

    class MUDLIB_CORE_API signal : public handle::signal
    {
    public:
        /**
         * @brief Constructor.
         *
         * Creating a new self-signalling resource.
         */
        signal();

        /**
         * @brief Destructor.
         *
         * Remove the self-signalling resource and free any underlying
         * implementation dependent resources.
         */
        virtual ~signal();

        /**
         * @brief The handle associated to the resource.
         */
        virtual const std::unique_ptr<mud::core::handle>& handle()
            const override;

        /**
         * @brief Signal the resources.
         *
         * Signal the resource such that it can be captured by any thread
         * waiting for it.
         */
        virtual void trigger() override;

        /**
         * @brief Capture the signal's trigger.
         *
         * Capture the signal and return true if it has been caught.
         */
        virtual bool capture() override;

    private:
        /**
         * Platform specific implementation.
         */
        class impl;
        struct impl_deleter
        {
            void operator()(impl*) const;
        };
        std::unique_ptr<impl, impl_deleter> _impl;
    };

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
{}

template<handle::type_t Type, typename Resource, Resource Invalid>
basic_handle<Type, Resource, Invalid>::basic_handle(Resource h)
  : handle(Type), _handle(h)
{}

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

/**
 * @brief A generic template to define a unique handle to an atomic resource.
 *
 * A handle is an abstract reference to a unique resource. In case of an
 * atomic resource, it is not associated to a particular operating system
 * resource, but rather to a unique resource state.
 *
 * As a resource is associated to a single handle, the handle object is often
 * used in combination with a @c std::unique_ptr to ensure unique access.
 *
 * An atomic resource handle can generally not be waited (until C++20).
 */
template<handle::type_t Type, typename AtomicType>
class atomic_handle : public handle
{
public:
    typedef AtomicType atomic_type;

    class signal : public handle::signal
    {
    public:
        /**
         * @brief Constructor.
         *
         * Creating a new self-signalling resource.
         */
        signal();

        /**
         * @brief Destructor.
         *
         * Remove the self-signalling resource and free any underlying
         * implementation dependent resources.
         */
        virtual ~signal();

        /**
         * @brief The handle associated to the resource.
         */
        virtual const std::unique_ptr<mud::core::handle>& handle()
            const override;

        /**
         * @brief Signal the resources.
         *
         * Signal the resource such that it can be captured by any thread
         * expecting that value.
         *
         * For atomic handles, the trigger value will be @c true.
         */
        virtual void trigger() override;

        /**
         * @brief Capture the signal's trigger.
         *
         * Capture the signal and return true if it has been caught with the
         * expected value. If it has been caught, replace it with a new value..
         *
         * For atomic handles, the trigger expected value will be @c true,
         * and the new value witlll be @c false.
         */
        virtual bool capture() override;

        /**
         * @brief Signal the resources.
         *
         * Signal the resource such that it can be captured by any thread
         * expecting that value.
         *
         * @param value [in] The value to set
         */
        virtual void trigger(atomic_type value);

        /**
         * @brief Capture the signal's trigger.
         *
         * Capture the signal and return true if it has been caught with the
         * expected value. If it has been caught, replace it with a new value..
         *
         * @param expected [in] The value to capture
         * @param value [in] The value to set if it has been caught
         */
        virtual bool capture(atomic_type expected, atomic_type value);

    private:
        /**
         * The unique atomic signal resource
         */
        std::unique_ptr<mud::core::handle> _impl;
    };

    /**
     * @brief Construct an invalid handle.
     * resource.
     */
    atomic_handle();

    /**
     * @brief Construct a handle and associate it to an externally defined
     * resource.
     */
    atomic_handle(atomic_type h);

    /**
     * @brief Move constructor.
     */
    atomic_handle(atomic_handle&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~atomic_handle();

    /**
     * @brief Return if the handle is valid.
     */
    bool valid() const override;

    /**
     * Non-copyable.
     */
    atomic_handle(const atomic_handle&) = delete;
    atomic_handle& operator=(const atomic_handle&) = delete;

private:
    /**
     * @brief Friend class to access internal representation.
     */
    template<typename R>
    friend R internal_handle(const std::unique_ptr<handle>&);

    /**
     * @brief Return the externally defined resource.
     */
    operator atomic_type();

    /*
     * The validity of the handle.
     */
    bool _valid;

    /**
     * The underlying handle to the resource;
     */
    std::atomic<atomic_type> _handle;
};

template<handle::type_t Type, typename AtomicType>
atomic_handle<Type, AtomicType>::atomic_handle() : handle(Type), _valid(false)
{}

template<handle::type_t Type, typename AtomicType>
atomic_handle<Type, AtomicType>::atomic_handle(AtomicType h)
  : handle(Type), _handle(h), _valid(true)
{}

template<handle::type_t Type, typename AtomicType>
atomic_handle<Type, AtomicType>::atomic_handle(atomic_handle&& rhs)
{
    atomic_type value = rhs.load();
    _handle.store(value);
    _valid = rhs._valid;
    rhs._valid = false;
}

template<handle::type_t Type, typename AtomicType>
atomic_handle<Type, AtomicType>::~atomic_handle()
{
    _valid = false;
}

template<handle::type_t Type, typename AtomicType>
atomic_handle<Type, AtomicType>::atomic_handle::operator AtomicType()
{
    return _handle;
}

template<handle::type_t Type, typename AtomicType>
bool
atomic_handle<Type, AtomicType>::valid() const
{
    return _valid;
}

template<handle::type_t Type, typename AtomicType>
atomic_handle<Type, AtomicType>::signal::signal()
{
    _impl = std::unique_ptr<mud::core::handle>(
        new mud::core::atomic_handle<Type, AtomicType>(false));
}

template<handle::type_t Type, typename AtomicType>
atomic_handle<Type, AtomicType>::signal::~signal()
{}

template<handle::type_t Type, typename AtomicType>
const std::unique_ptr<mud::core::handle>&
atomic_handle<Type, AtomicType>::signal::handle() const
{
    return _impl;
}

template<handle::type_t Type, typename AtomicType>
void
atomic_handle<Type, AtomicType>::signal::trigger()
{
    trigger(true);
}

template<handle::type_t Type, typename AtomicType>
void
atomic_handle<Type, AtomicType>::signal::trigger(AtomicType value)
{
    auto ptr =
        static_cast<mud::core::atomic_handle<Type, AtomicType>*>(_impl.get());
    ptr->_handle.store(value);
}

template<handle::type_t Type, typename AtomicType>
bool
atomic_handle<Type, AtomicType>::signal::capture()
{
    return capture(true, false);
}

template<handle::type_t Type, typename AtomicType>
bool
atomic_handle<Type, AtomicType>::signal::capture(AtomicType expected,
                                                 AtomicType value)
{
    AtomicType data = expected;
    auto ptr =
        static_cast<mud::core::atomic_handle<Type, AtomicType>*>(_impl.get());
    return ptr->_handle.compare_exchange_strong(data, value);
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
 * @brief Supported system handles.
 */

/**
 * @brief: A handle to an atomic boolean state type.
 */
typedef atomic_handle<handle::type_t::ATOMIC_BOOL, bool> atomic_bool_handle;
template<>
MUDLIB_CORE_API bool
internal_handle<bool>(const std::unique_ptr<handle>&);

/**
 * @brief: A handle to an @c select resource type.
 */
typedef basic_handle<handle::type_t::SELECT, int, 0> select_handle;
template<>
MUDLIB_CORE_API int
internal_handle<int>(const std::unique_ptr<handle>&);

#ifdef WINDOWS
/**
 * @brief: A handle to a windows @c HANDLE resource type.
 */
typedef basic_handle<handle::type_t::W32HANDLE, HANDLE, nullptr> windows_handle;
template<>
MUDLIB_CORE_API HANDLE
internal_handle<HANDLE>(const std::unique_ptr<handle>&);

/**
 * @brief: A handle to a windows @c HHWND resource type.
 */
typedef basic_handle<handle::type_t::W32WND, HWND, nullptr> win32_handle;
template<>
MUDLIB_CORE_API HWND
internal_handle<HWND>(const std::unique_ptr<handle>&);
#endif

#ifdef DARWIN
/**
 * @brief: A dummy handle for a MacOSX UI loop wake-up trigger.
 */
typedef mud::core::basic_handle<mud::core::handle::type_t::COCOA, void*,
                                nullptr>
    cocoa_handle;
#endif

/**
 * @brief: A handle to an @c test resource type.
 */
typedef basic_handle<handle::type_t::__TEST, int, 0> __test_handle;
template<>
MUDLIB_CORE_API int
internal_handle<int>(const std::unique_ptr<handle>&);

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_HANDLE_H_ */
