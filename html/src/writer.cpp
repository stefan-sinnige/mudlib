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

#include "mud/html.h"

// Forward declarations

std::ostream&
operator<<(std::ostream& ostr, const mud::html::cdata_section& cdata);
std::ostream&
operator<<(std::ostream& ostr, const mud::html::char_data& char_data);
std::ostream&
operator<<(std::ostream& ostr, const mud::html::comment& comment);
std::ostream&
operator<<(std::ostream& ostr, const mud::html::declaration& decl);
std::ostream&
operator<<(std::ostream& ostr, const mud::html::element& element);
std::ostream&
operator<<(std::ostream& ostr, const mud::html::processing_instruction& pi);
std::ostream&
operator<<(std::ostream& ostr, const mud::html::node& node);

// Insertion operator implementations

std::ostream&
operator<<(std::ostream& ostr, const mud::html::cdata_section& cdata)
{
    ostr << "<![CDATA[" << cdata.text() << "]]>";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::html::char_data& char_data)
{
    ostr << char_data.text();
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::html::comment& comment)
{
    ostr << "<!--" << comment.text() << "-->";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::html::declaration& decl)
{
    ostr << "<?html version=\"" << decl.version() << "\"";
    if (decl.encoding().size() > 0) {
        ostr << " encoding=\"" << decl.encoding() << "\"";
    }
    if (decl.standalone()) {
        ostr << " standalone=\"yes\"";
    }
    ostr << "?>";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::html::element& element)
{
    // TODO:
    // To insert indentation strings, use strings like
    //     str::string(indent*2, ' ')
    // That should be controlled by an output manipulator
    //     std::cout << mud:;html::indent << doc;
    ostr << "<" << element.name();
    for (auto& attr : element.attributes()) {
        ostr << " " << attr.name() << "=\"" << attr.value() << "\"";
    }
    if (element.nodes().size() > 0) {
        ostr << ">";
        for (auto& node : element.nodes()) {
            ostr << node;
        }
        ostr << "</" << element.name() << ">";
    } else {
        ostr << "/>";
    }

    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::html::processing_instruction& pi)
{
    ostr << "<?" << pi.target() << " " << pi.data() << "?>";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::html::node& node)
{
    switch (node.type()) {
        case mud::html::node::type_t::CDATA_SECTION:
            ostr << dynamic_cast<const mud::html::cdata_section&>(node);
            break;
        case mud::html::node::type_t::CHAR_DATA:
            ostr << dynamic_cast<const mud::html::char_data&>(node);
            break;
        case mud::html::node::type_t::COMMENT:
            ostr << dynamic_cast<const mud::html::comment&>(node);
            break;
        case mud::html::node::type_t::DECL:
            ostr << dynamic_cast<const mud::html::declaration&>(node);
            break;
        case mud::html::node::type_t::ELEMENT:
            ostr << dynamic_cast<const mud::html::element&>(node);
            break;
        case mud::html::node::type_t::PI:
            ostr << dynamic_cast<const mud::html::processing_instruction&>(node);
            break;
        default:
            break;
    }
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::html::document& doc)
{
    for (auto& node : doc.nodes()) {
        ostr << node;
    }
    return ostr;
}

/* vi: set ai ts=4 expandtab: */
