#include "mud/xml/node.h"
#include "mud/xml/cdata_section.h"
#include "mud/xml/char_data.h"
#include "mud/xml/comment.h"
#include "mud/xml/declaration.h"
#include "mud/xml/element.h"
#include "mud/xml/processing_instruction.h"

BEGIN_MUDLIB_XML_NS

const mud::xml::node_seq&
node::children() const
{
    static mud::xml::node_seq  empty;
    return empty;
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
