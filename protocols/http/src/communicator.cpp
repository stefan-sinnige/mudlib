#include "mud/http/communicator.h"

BEGIN_MUDLIB_HTTP_NS

namespace communicator {

server::server(communicator<mud::io::tcp::socket>& lower)
    : mud::protocols::layered_communicator<mud::io::tcp::socket>(lower)
{
}

client::client(communicator<mud::io::tcp::socket>& lower)   
    : mud::protocols::layered_communicator<mud::io::tcp::socket>(lower)
{
}

}; // namespace communicator

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
