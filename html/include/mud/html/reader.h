#ifndef _MUDLIB_HTML_READER_H_
#define _MUDLIB_HTML_READER_H_

#include <istream>
#include <mud/html/document.h>
#include <mud/html/ns.h>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Manipulator to output debug scanning information to standard error.
 * @note Output will only be generated if the lexical analyser has debugging
 * information enabled.
 */
std::ios_base&
scanner_debug(std::ios_base& istr);

/**
 * @brief Manipulator to output debug parsing information.
 * @note Output will only be generated if the parser generator has debugging
 * information enabled.
 */
std::ios_base&
parser_debug(std::ios_base& istr);

END_MUDLIB_HTML_NS

/**
 * @brief Extraction operator to read an HTML document from an input stream.
 * @param[in] istr The stream to read the HTML document from.
 * @param[in,out] doc The object to hold the HTML document.
 */
std::istream&
operator>>(std::istream& istr, mud::html::document& doc);

#endif /*  _MUDLIB_HTML_READER_H_ */
