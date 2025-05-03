#ifndef _MUDLIB_XML_CHAR_REFERENCE_H_
#define _MUDLIB_XML_CHAR_REFERENCE_H_

#include <mud/xml/ns.h>
#include <string>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Utility class to escape or unescape a string.
 */
class MUDLIB_XML_API char_reference
{
public:
    /**
     * @brief Escape a string by replacing specific characters with its
     * escaped character reference representation.
     * @param[in] unescaped The unescaped string.
     * @return The escaped form.
     */
    static std::string escape(const std::string& unescaped);

    /**
     * @brief Unescape a string by replacing specific character references with
     * its unescaped reference representation.
     * @param[in] escaped The unescaped string.
     * @return The unescaped form.
     * @throw mud::html::exception when the escaped string contains unknown
     * character references.
     */
    static std::string unescape(const std::string& escaped);
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_CHAR_REFERENCE_H_ */
