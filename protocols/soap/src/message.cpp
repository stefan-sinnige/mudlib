#include "mud/soap/message.h"
#include <mud/xml/dom.h>
#include <memory>

BEGIN_MUDLIB_SOAP_NS

const mud::core::uri ENV_NS("http://www.w3.org/2003/05/soap-envelope");
const mud::core::uri ENC_NS("https://www.w3.org/2003/05/soap-encoding");

message::message()
{
    _xml = mud::xml::dom::create_document();
    auto decl = mud::xml::dom::create_declaration();
    auto env = mud::xml::dom::create_element("Envelope", ENV_NS);
    auto env_ns = mud::xml::dom::create_attribute("xmlns:env");
    env_ns->value(ENV_NS);
    env->attribute(env_ns);
    auto enc = mud::xml::dom::create_attribute("encodingStyle", ENV_NS);
    enc->value(ENC_NS);
    env->attribute(enc);
    auto header = mud::xml::dom::create_element("Header", ENV_NS);
    auto body = mud::xml::dom::create_element("Body", ENV_NS);
    env->child(header);
    env->child(body);
    _xml->child(decl);
    _xml->child(env);
}

mud::xml::document::ptr
message::xml() const
{
    return _xml;
}

END_MUDLIB_SOAP_NS

std::ostream&
operator<<(std::ostream& ostr, const mud::soap::message& msg)
{
    ostr << msg.xml();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, mud::soap::message& msg)
{
    return istr;
}

/* vi: set ai ts=4 expandtab: */
