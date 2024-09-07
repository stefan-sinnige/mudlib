#include <mud/core/exception.h>
#include "mock_device.h"

namespace mock {

class device::impl
{
public:
    /**
     * Constructor.
     */
    impl(std::istream& istr, std::ostream& ostr);

    /**
     * Destructor.
     */
    ~impl();

    /**
     * The input stream.
     */
    std::istream& istr();

    /**
     * The output stream.
     */
    std::ostream& ostr();

    /**
     * The handle.
     */
    const std::unique_ptr<mud::core::handle>& handle() const;

    /**
     * Set the on-receive callback handler.
     */
    void on_ready_read_cb(device::on_ready_read_func func);

    /**
     * Return the on-receive callback handler.
     */
    device::on_ready_read_func on_ready_read_cb() const;

private:
    /** The input stream. */
    std::istream& _istr;

    /** The output stream. */
    std::ostream& _ostr;

    /** The on-receive callback. */
    device::on_ready_read_func _on_ready_read_cb;

    /** The handle */
    std::unique_ptr<mud::core::handle> _handle;
};

device::impl::impl(std::istream& istr, std::ostream& ostr)
    : _istr(istr), _ostr(ostr), _on_ready_read_cb(nullptr)
{
    _handle = std::unique_ptr<mud::core::handle>(new mud::core::select_handle(0xBEEF));
}

device::impl::~impl()
{
}

std::istream&
device::impl::istr()
{
    return _istr;
}

std::ostream&
device::impl::ostr()
{
    return _ostr;
}

const std::unique_ptr<mud::core::handle>&
device::impl::handle() const
{
    return _handle;
}

void
device::impl::on_ready_read_cb(device::on_ready_read_func func)
{
    _on_ready_read_cb = func;
}

device::on_ready_read_func
device::impl::on_ready_read_cb() const
{
    return _on_ready_read_cb;
}

device::device()
{
}

device::device(std::istream& istr, std::ostream& ostr)
{
    _impl = std::unique_ptr<impl, impl_deleter>(new impl(istr, ostr));
}

device::device(device&& other)
{
    std::swap(_impl, other._impl);
}

device::~device()
{
}

device& device::operator=(device&& other)
{
    std::swap(_impl, other._impl);
    return *this;
}

std::istream&
device::istr()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->istr();
}

std::ostream&
device::ostr()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->ostr();
}

const std::unique_ptr<mud::core::handle>&
device::handle() const
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->handle();
}

void
device::on_ready_read_cb(device::on_ready_read_func func)
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    _impl->on_ready_read_cb(func);
}

device::on_ready_read_func
device::on_ready_read_cb() const
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->on_ready_read_cb();
}

void
device::signal_read()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    auto func = _impl->on_ready_read_cb();
    if (func) {
        func();
    }
}

void
device::impl_deleter::operator()(device::impl* ptr) const
{
    delete ptr;
}

} // namespace mock

