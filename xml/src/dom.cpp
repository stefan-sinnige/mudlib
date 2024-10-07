#include "mud/xml/dom.h"

BEGIN_MUDLIB_XML_NS

/* static */ xml::attribute::ptr
dom::create_attribute()
{
    return xml::attribute::create();
}

/* static */ xml::cdata_section::ptr
dom::create_cdata_section()
{
    return xml::cdata_section::create();
}

/* static */ xml::char_data::ptr
dom::create_char_data()
{
    return xml::char_data::create();
}

/* static */ xml::comment::ptr
dom::create_comment()
{
    return xml::comment::create();
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
dom::create_element()
{
    return xml::element::create();
}

/* static */ xml::processing_instruction::ptr
dom::create_processing_instruction()
{
    return xml::processing_instruction::create();
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
