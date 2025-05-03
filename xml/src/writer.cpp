#include "mud/xml.h"

// Forward declarations

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::attribute& attribute);
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
operator<<(std::ostream& ostr, const mud::xml::attribute& attr)
{
    if (attr.ns()->resolved()) {
        ostr << "xmlns";
        if (!attr.ns()->prefix().empty()) {
            ostr << ":" << attr.ns()->prefix();
        }
        ostr << "=\"" << mud::xml::char_reference::escape(attr.value()) << "\"";
    }
    else {
        // Resolve the namespace and construct the qname to output
        auto ns = attr.resolved_ns();
        std::string qname;
        if (ns && !ns->prefix().empty()) {
            qname = ns->prefix() +  ":" + attr.local_name();
        }
        else {
            qname = attr.name();
        }
    
        ostr << qname << "=\"" << mud::xml::char_reference::escape(attr.value())
             << "\"";
    }
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::cdata_section& cdata)
{
    ostr << "<![CDATA[" << cdata.text() << "]]>";
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::xml::char_data& char_data)
{
    ostr << mud::xml::char_reference::escape(char_data.text());
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
    //     std::cout << mud::xml::indent << doc;

    // Resolve the namespace and construct the qname to output
    auto ns = element.resolved_ns();
    std::string qname;
    if (ns && !ns->prefix().empty()) {
        qname = ns->prefix() +  ":" + element.local_name();
    }
    else {
        qname = element.name();
    }
    
    // Output the element
    ostr << "<" << qname;
    for (auto attr : element.attributes()) {
        ostr << " " << *attr;
    }
    if (element.children().size() > 0) {
        ostr << ">";
        for (auto node : element.children()) {
            ostr << *node;
        }
        ostr << "</" << qname << ">";
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
operator<<(std::ostream& ostr, const mud::xml::document::ptr& doc)
{
    if (doc) {
        for (auto node : doc->children()) {
            ostr << *node;
        }
    }
    return ostr;
}

/* vi: set ai ts=4 expandtab: */
