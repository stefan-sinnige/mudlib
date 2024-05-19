#include "mud/http/chunk.h"
#include "tokenise.h"
#include <stdexcept>

BEGIN_MUDLIB_HTTP_NS

std::ostream&
operator<<(std::ostream& ostr, const chunk& body)
{
    ostr << std::hex << body.size() << std::dec;
    ostr << CR << LF;
    ostr << body.data();
    ostr << CR << LF;
    return ostr;
}

std::istream&
operator>>(std::istream& istr, chunk& body)
{
    // Read the hexadecimal size. Support up to 32-bits (which is ridiculous).
    uint32_t size;
    istr >> std::hex >> size >> std::dec;
    expect(istr, CR);
    expect(istr, LF);

    // Read the data
    if (size > 0) {
        std::string data;
        data.resize(size);
        istr.read(&data[0], size);
        body = chunk(data);
    } else {
        body = chunk();
    }
    expect(istr, CR);
    expect(istr, LF);
    return istr;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
