#include "mud/xml/char_reference.h"
#include "mud/xml/exception.h"
#include <stdint.h>
#include <string.h>

BEGIN_MUDLIB_XML_NS

static void
insert_codepoint(std::string& unescaped, uint32_t codepoint)
{
    // Convert UTF-32 to UTF-8
    if ((codepoint & 0xFFFFFF80) == 0x00000000) {
        unescaped.push_back(codepoint & 0x0000007F);
    } else if ((codepoint & 0xFFFFF800) == 0x00000000) {
        unescaped.push_back(0xC0 | ((codepoint >> 6) & 0x0000001F));
        unescaped.push_back(0x80 | ((codepoint >> 0) & 0x0000003F));
    } else if ((codepoint & 0xFFFF0000) == 0x00000000) {
        unescaped.push_back(0xE0 | ((codepoint >> 12) & 0x0000000F));
        unescaped.push_back(0x80 | ((codepoint >> 6) & 0x0000003F));
        unescaped.push_back(0x80 | ((codepoint >> 0) & 0x0000003F));
    } else if ((codepoint & 0xFF700000) == 0x00000000) {
        unescaped.push_back(0xF0 | ((codepoint >> 18) & 0x00000003));
        unescaped.push_back(0x80 | ((codepoint >> 12) & 0x0000003F));
        unescaped.push_back(0x80 | ((codepoint >> 6) & 0x0000003F));
        unescaped.push_back(0x80 | ((codepoint >> 0) & 0x0000003F));
    } else {
        throw std::exception();
    }
}

std::string
char_reference::escape(const std::string& unescaped)
{
    std::string escaped;
    for (char ch: unescaped) {
        switch (ch) {
            case '<':
                escaped.append("&lt;");
                break;
            case '>':
                escaped.append("&gt;");
                break;
            case '&':
                escaped.append("&amp;");
                break;
            case '\'':
                escaped.append("&apos;");
                break;
            case '"':
                escaped.append("&quot;");
                break;
            default:
                escaped.push_back(ch);
                break;
        }
    }
    return escaped;
}

std::string
char_reference::unescape(const std::string& escaped)
{
    std::string unescaped;
    unescaped.reserve(escaped.size());
    const char* cptr = escaped.data();
    while (*cptr != '\0') {
        if (*cptr == '&') {
            bool ambiguous = false;
            const char* start = cptr;

            // Escape character
            if (*(cptr + 1) == '#') {
                if (*(cptr + 2) == 'x' || *(cptr + 2) == 'X') {
                    // Hexadecimal escape
                    cptr += 3;
                    uint32_t codepoint = 0;
                    while ((*cptr >= '0' && *cptr <= '9') ||
                           (*cptr >= 'A' && *cptr <= 'F') ||
                           (*cptr >= 'a' && *cptr <= 'f')) {
                        codepoint = codepoint * 16;
                        if (*cptr <= '9') {
                            codepoint += (*cptr - '0');
                        } else if (*cptr <= 'F') {
                            codepoint += (*cptr - 'A' + 10);
                        } else if (*cptr <= 'f') {
                            codepoint += (*cptr - 'a' + 10);
                        }
                        ++cptr;
                    }
                    if (*cptr == ';') {
                        ++cptr;
                        insert_codepoint(unescaped, codepoint);
                    } else {
                        // Ambiguauos ampersand
                        ambiguous = true;
                    }
                } else {
                    // Decimal escape
                    cptr += 2;
                    uint32_t codepoint = 0;
                    while (*cptr >= '0' && *cptr <= '9') {
                        codepoint = codepoint * 10 + (*cptr - '0');
                        ++cptr;
                    }
                    if (*cptr == ';') {
                        ++cptr;
                        insert_codepoint(unescaped, codepoint);
                    } else {
                        // Ambiguauos ampersand
                        ambiguous = true;
                    }
                }
            } else {
                // Named entity. Only the predefined entities are supported.
                ++cptr;
                if (strncmp(cptr, "lt;", 3) == 0) {
                    unescaped.push_back('<');
                    cptr += 3;
                }
                else
                if (strncmp(cptr, "gt;", 3) == 0) {
                    unescaped.push_back('>');
                    cptr += 3;
                }
                else
                if (strncmp(cptr, "amp;", 4) == 0) {
                    unescaped.push_back('&');
                    cptr += 4;
                }
                else
                if (strncmp(cptr, "apos;", 5) == 0) {
                    unescaped.push_back('\'');
                    cptr += 5;
                }
                else
                if (strncmp(cptr, "quot;", 5) == 0) {
                    unescaped.push_back('"');
                    cptr += 5;
                }
                else {
                    ambiguous = true;
                }
            }

            // If ambiguous, just copy the buffer as-is
            if (ambiguous) {
                while (start != cptr) {
                    unescaped.push_back(*start++);
                }
            }
        } else {
            // No escaped character
            unescaped.push_back(*cptr++);
        }
    }
    return std::move(unescaped);
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
