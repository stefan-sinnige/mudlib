#include "mud/xml.h"

// Forward declarations

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::cdata_section& cdata);
std::ostream&
operator<<(std::ostream& ostr, const mud::xml::char_data& char_data);
std::ostream&
operator<<(std::ostream& ostr, const mud::xml::comment& comment);
std::ostream&
operator<<(std::ostream& ostr, const mud::xml::declaration& decl);
std::ostream&
operator<<(std::ostream& ostr, const mud::xml::element& element);
std::ostream&
operator<<(std::ostream& ostr, const mud::xml::processing_instruction& pi);
std::ostream&
operator<<(std::ostream& ostr, const mud::xml::node& node);

// Insertion operator implementations

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::cdata_section& cdata)
{
    ostr << "<![CDATA[" << cdata.text() << "]]>";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::char_data& char_data)
{
    ostr << char_data.text();
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::comment& comment)
{
    ostr << "<!--" << comment.text() << "-->";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::declaration& decl)
{
    ostr << "<?xml version=\"" << decl.version() << "\"";
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
operator<<(std::ostream& ostr, const mud::xml::element& element)
{
    // TODO:
    // To insert indentation strings, use strings like
    //     str::string(indent*2, ' ')
    // That should be controlled by an output manipulator
    //     std::cout << mud:;xml::indent << doc;
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
operator<<(std::ostream& ostr, const mud::xml::processing_instruction& pi)
{
    ostr << "<?" << pi.target() << " " << pi.data() << "?>";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::node& node)
{
    switch (node.type()) {
        case mud::xml::node::type_t::CDATA_SECTION:
            ostr << dynamic_cast<const mud::xml::cdata_section&>(node);
            break;
        case mud::xml::node::type_t::CHAR_DATA:
            ostr << dynamic_cast<const mud::xml::char_data&>(node);
            break;
        case mud::xml::node::type_t::COMMENT:
            ostr << dynamic_cast<const mud::xml::comment&>(node);
            break;
        case mud::xml::node::type_t::DECL:
            ostr << dynamic_cast<const mud::xml::declaration&>(node);
            break;
        case mud::xml::node::type_t::ELEMENT:
            ostr << dynamic_cast<const mud::xml::element&>(node);
            break;
        case mud::xml::node::type_t::PI:
            ostr << dynamic_cast<const mud::xml::processing_instruction&>(node);
            break;
        default:
            break;
    }
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::document& doc)
{
    for (auto& node : doc.nodes()) {
        ostr << node;
    }
    return ostr;
}

/* vi: set ai ts=4 expandtab: */
