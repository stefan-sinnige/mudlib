#ifndef _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATORS_H_
#define _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATORS_H_

#include <mud/protocols/communicator.h>
#include "mock_device.h"

namespace mock {

/**
 * End communicator to a particular mocked device. This does not contain
 * any messaging but is purely a wrapper for a device's input/output streams.
 */
class end_communicator:
    public mud::protocols::end_communicator<mock::device>
{
public:
    end_communicator();
    void open(mock::device&& device) override;
    void close() override;
    std::istream& istr() override;
    std::ostream& ostr() override;
    mock::device& device() override;
    const mud::core::event& event() const override;
private:
    void on_ready_read();
    mud::core::event _event;
    mock::device _device;
};

/**
 * Communicator for a A protocol on a mocked device. This protocol implements
 * the following messages:
 *   A_ENVELOPE
 *     An envelope message that contains contents for an upper communicator
 *   A_REQUEST / A_REPLY
 *     A message that is only interpreted at this communicator
 */
class layer_a_communicator:
    public mud::protocols::layered_communicator<mock::device>
{
public:
    layer_a_communicator(mud::protocols::communicator<mock::device>& lower);
    void open(mock::device&& device) override;
    void close() override;
    std::istream& istr() override;
    std::ostream& ostr() override;
    void on_receive(mock::device&) override;
};

/**
 * Communicator for a B protocol on a mocked device. This protocol implements
 * the following messages:
 *   B_ENVELOPE
 *     An envelope message that contains contents for an upper communicator
 *   B_REQUEST / B_REPLY
 *     A message that is only interpreted at this communicator
 */
class layer_b_communicator:
    public mud::protocols::layered_communicator<mock::device>
{
public:
    layer_b_communicator(mud::protocols::communicator<mock::device>& lower);
    void open(mock::device&& device) override;
    void close() override;
    std::istream& istr() override;
    std::ostream& ostr() override;
    void on_receive(mock::device&) override;
};

} // namespace mock

#endif /*  _MUDLIB_PROTOOCOLS_MOCK_COMMUNICATORS_H_ */
