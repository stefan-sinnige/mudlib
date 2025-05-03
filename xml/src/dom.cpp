#include "mud/xml/dom.h"

BEGIN_MUDLIB_XML_NS

/* static */ xml::attribute::ptr
dom::create_attribute(const std::string& qname)
{
    return xml::attribute::create(qname);
}

/* static */ xml::attribute::ptr
dom::create_attribute(const std::string& local_name, const mud::core::uri& uri)
{
    return xml::attribute::create(local_name, uri);
}

/* static */ xml::cdata_section::ptr
dom::create_cdata_section(const std::string& text)
{
    return xml::cdata_section::create(text);
}

/* static */ xml::char_data::ptr
dom::create_char_data(const std::string& text)
{
    return xml::char_data::create(text);
}

/* static */ xml::comment::ptr
dom::create_comment(const std::string& text)
{
    return xml::comment::create(text);
}

/* static */ xml::declaration::ptr
dom::create_declaration()
{
    return xml::declaration::create();
}

/* static */ xml::document::ptr
dom::create_document()
{
    return xml::document::create();
}

/* static */ xml::element::ptr
dom::create_element(const std::string& qname)
{
    return xml::element::create(qname);
}

/* static */ xml::element::ptr
dom::create_element(const std::string& local_name, const mud::core::uri& uri)
{
    return xml::element::create(local_name, uri);
}

/* static */ xml::processing_instruction::ptr
dom::create_processing_instruction(const std::string& target,
    const std::string& data)
{
    return xml::processing_instruction::create(target, data);
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
