#ifndef _MUDLIB_HTML_WRITER_H_
#define _MUDLIB_HTML_WRITER_H_

#include <mud/html/document.h>
#include <mud/html/ns.h>
#include <ostream>

/**
 * @brief Insertion operator to output an HTML document to an output stream.
 * @param[in] istr The stream to output the HTML document to.
 * @param[in,out] doc The HTML document to output.
 */
std::ostream&
operator<<(std::ostream& istr, const mud::html::document& doc);

#endif /*  _MUDLIB_HTML_WRITER_H_ */
