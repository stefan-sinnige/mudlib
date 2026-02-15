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
    std::shared_ptr<mud::core::handle> handle() const;

    /**
     * The signal event.
     */
    mud::core::event& signal();

private:
    /** The input stream. */
    std::istream& _istr;

    /** The output stream. */
    std::ostream& _ostr;

    /** The handle */
    std::shared_ptr<mud::core::handle> _handle;

    /** The signalled event */
    mud::core::event _signal;
};

device::impl::impl(std::istream& istr, std::ostream& ostr)
    : _istr(istr), _ostr(ostr)
{
    _handle = std::make_shared<mud::core::select_handle>(0xBEEF);
    _signal = mud::core::event(mud::core::uuid(), _handle,
            mud::core::event::signal_type::READING);
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

std::shared_ptr<mud::core::handle>
device::impl::handle() const
{
    return _handle;
}

mud::core::event&
device::impl::signal()
{
    return _signal;
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

std::shared_ptr<mud::core::handle>
device::handle()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->handle();
}

mud::core::event&
device::signal()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    return _impl->signal();
}

void
device::simulate_signal()
{
    if (!_impl) {
        throw mud::core::not_owner();
    }
    _impl->signal().publish();
}

void
device::impl_deleter::operator()(device::impl* ptr) const
{
    delete ptr;
}

} // namespace mock

