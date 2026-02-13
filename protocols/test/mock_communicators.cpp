#include "mock_communicators.h"

namespace mock {

/* ======================================================================
 * End
 * ====================================================================== */

end_communicator::end_communicator()
{
}

void
end_communicator::open(mock::device&& device)
{
    _device = std::move(device);
    attach(_device.signal().topic(), &end_communicator::on_signal);
}

void
end_communicator::close()
{
}

std::istream&
end_communicator::istr()
{
    return _device.istr();
}

std::ostream&
end_communicator::ostr()
{
    return _device.ostr();
}

mock::device&
end_communicator::device()
{
    return _device;
}

mud::core::event&
end_communicator::event()
{
    return _event;
}

void
end_communicator::on_signal(const mud::core::message& /* msg */)
{
    mud::core::broker::publish(received());
}

/* ======================================================================
 * Layer A
 * ====================================================================== */

layer_a_communicator::layer_a_communicator(
        mud::protocols::communicator<mock::device>& lower)
    : mud::protocols::layered_communicator<mock::device>(lower)
{
}

void
layer_a_communicator::open(mock::device&& device)
{
    lower().open(std::move(device));
}

void
layer_a_communicator::close()
{
    lower().close();
}

std::istream&
layer_a_communicator::istr()
{
    return lower().istr();
}

std::ostream&
layer_a_communicator::ostr()
{
    return lower().ostr();
}

void
layer_a_communicator::on_received(const mud::core::message& msg)
{
    // Get the layer command
    std::string cmd;
    std::getline(lower().istr(), cmd);

    // Process it
    if (cmd == "A_REQUEST") {
        // Only handled at this communicator
        lower().ostr() << "A_REPLY";
    }
    else
    if (cmd == "A_ENVELOPE") {
        // Remaining message handled at an upper communicator
        mud::core::broker::publish(received());
    }
    else {
    }
}

/* ======================================================================
 * Layer B
 * ====================================================================== */

layer_b_communicator::layer_b_communicator(
        mud::protocols::communicator<mock::device>& lower)
    : mud::protocols::layered_communicator<mock::device>(lower)
{
}

void
layer_b_communicator::open(mock::device&& device)
{
    lower().open(std::move(device));
}

void
layer_b_communicator::close()
{
    lower().close();
}

std::istream&
layer_b_communicator::istr()
{
    return lower().istr();
}

std::ostream&
layer_b_communicator::ostr()
{
    return lower().ostr();
}

void
layer_b_communicator::on_received(const mud::core::message& msg)
{
    // Get the layer command
    std::string cmd;
    std::getline(lower().istr(), cmd);

    // Process it
    if (cmd == "B_REQUEST") {
        // Only handled at this communicator
        lower().ostr() << "B_REPLY";
    }
    else
    if (cmd == "B_ENVELOPE") {
        // Remaining message handled at an upper communicator
        mud::core::broker::publish(received());
    }
    else {
    }
}

} // namespace mock

