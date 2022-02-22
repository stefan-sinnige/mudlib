#ifndef _MUDLIB_XML_WRITER_H_
#define _MUDLIB_XML_WRITER_H_

#include <mud/xml/document.h>
#include <mud/xml/exception.h>
#include <mud/xml/ns.h>
#include <ostream>

BEGIN_MUDLIB_XML_NS

END_MUDLIB_XML_NS

/**
 * @brief Insertion operator to output an XML document to an output stream.
 * @param[in] istr The stream to output the XML document to.
 * @param[in,out] doc The XML document to output.
 */
std::ostream&
operator<<(std::ostream& istr, const mud::xml::document& doc);

#endif /*  _MUDLIB_XML_WRITER_H_ */
