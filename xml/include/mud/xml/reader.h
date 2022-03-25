#ifndef _MUDLIB_XML_READER_H_
#define _MUDLIB_XML_READER_H_

#include <istream>
#include <mud/xml/document.h>

/**
 * @brief Extraction operator to read an XML document from an input stream.
 * @param[in] istr The stream to read the XML document from.
 * @param[in,out] doc The object to hold the XML document.
 */
std::istream&
operator>>(std::istream& istr, mud::xml::document& doc);

#endif /*  _MUDLIB_XML_READER_H_ */
