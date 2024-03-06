#include "mud/http/message.h"
#include "src/message_private.h"

BEGIN_MUDLIB_HTTP_NS

std::ostream&
operator<<(std::ostream& ostr, const message& msg)
{

    if (!msg.valid()) {
        throw std::runtime_error("Invalid HTTP message");
    }

    // The request or response line
    if (msg._type == message::type_t::REQUEST) {
        msg.field<method>().serialise(ostr);
        ostr << SP;
        msg.field<uri>().serialise(ostr);
        ostr << SP;
        msg.field<version>().serialise(ostr);
        ostr << CR << LF;
    } else if (msg._type == message::type_t::RESPONSE) {
        msg.field<version>().serialise(ostr);
        ostr << SP;
        msg.field<status_code>().serialise(ostr);
        ostr << SP;
        msg.field<reason_phrase>().serialise(ostr);
        ostr << CR << LF;
    }

    // General header
    if (msg.exists<date>()) {
        ostr << "Date" << CL << SP;
        msg.field<date>().serialise(ostr);
        ostr << CR << LF;
    }

    // Request / response specific headers

    // Entiry header
    if (msg.exists<content_length>()) {
        ostr << "Content-Length" << CL << SP;
        msg.field<content_length>().serialise(ostr);
        ostr << CR << LF;
    }

    // Entity separator
    ostr << CR << LF;

    // Entity body
    if (msg.exists<entity_body>()) {
        msg.field<entity_body>().serialise(ostr);
    }

    return ostr;
}

void
version::serialise(std::ostream& ostr) const
{
    switch (_value) {
        case version::Version::HTTP10:
            ostr << "HTTP/1.0";
            break;
        default:
            break;
    }
}

void
method::serialise(std::ostream& ostr) const
{
    ostr << _value;
}

void
uri::serialise(std::ostream& ostr) const
{
    ostr << _value;
}

void
date::serialise(std::ostream& ostr) const
{
    time_t utc = std::chrono::system_clock::to_time_t(_value);
    struct tm tm;
    char buf[30];

#if defined(_WIN32)
    (void)gmtime_s(&tm, &utc);
#else
    (void)gmtime_r(&utc, &tm);
#endif
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tm);
    ostr << buf;
}

void
status_code::serialise(std::ostream& ostr) const
{
    ostr << _value;
}

void
reason_phrase::serialise(std::ostream& ostr) const
{
    ostr << _value;
}

void
content_length::serialise(std::ostream& ostr) const
{
    ostr << _value;
}

void
entity_body::serialise(std::ostream& ostr) const
{
    ostr << _value;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
