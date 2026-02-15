/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

#include "mud/html/char_reference.h"
#include "entities.h"
#include "mud/html/exception.h"
#include <stdint.h>
#include <iostream>

BEGIN_MUDLIB_HTML_NS

void
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
    std::string escaped = unescaped;
    return std::move(escaped);
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
                // Named entity. Use the entity scanner.
                int state = 1;
                while (*cptr != '\0' && state > 0) {
                    if (checks[shifts[state] + *cptr] == state) {
                        state = compressed[shifts[state] + *cptr++];
                    } else {
                        state = 0;
                    }
                }
                if (state >= 0) {
                    // Ambiguauos ampersand
                    ambiguous = true;
                } else {
                    // Output the unescaped entity
                    const char* ent = entities[-state - 1].utf8;
                    while (*ent != '\0') {
                        unescaped.push_back(*ent++);
                    }
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

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
