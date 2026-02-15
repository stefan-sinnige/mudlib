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
