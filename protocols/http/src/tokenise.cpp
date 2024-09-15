#include "tokenise.h"

BEGIN_MUDLIB_HTTP_NS

std::string
tokenise(std::istream& istr, const token_manip& manip)
{
    /* The standard table of allowable characters. Additional characters
     * that an be governed by the token manipulators are *not* included. */
    static uint8_t table[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // NUL .. SI
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // DLE .. US
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, //  SP .. /
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, //   0 .. ?
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //   @ .. O
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //   P .. _
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //   ` .. o
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0  //   p .. DEL
    };

    /* Read all characters that are part of a token */
    int ch;
    char buf[128];
    int n = 0;
    std::string str;
    while ((ch = istr.get()) != std::char_traits<char>::eof()) {
        if ((ch > 0 && ch < 127 && table[ch] == 1) ||
            (manip.space && ch == SP) || (manip.colon && ch == CL) ||
            (manip.comma && ch == CM)) 
        {
            buf[n++] = ch;
            if (n >= sizeof(buf)) {
                str.append(buf, n);
                n = 0;
            }
        } else {
            break;
        }
    }
    buf[n] = '\0';
    str.append(buf, n);
    istr.unget();
    return str;
}

/*
 * Expect a specific character to be read next from an input stream. Throws
 * a 'malformed HTTP message' error if the next character does not match.
 */
std::istream&
expect(std::istream& istr, int expected)
{
    int ch = istr.get();
    if (ch != expected) {
        throw std::runtime_error("Malformed HTTP message");
    }
    return istr;
}

/*
 * Skip whitespace according to its type.
 */
std::istream&
whitespace(std::istream& istr, ws_t type)
{
    bool found = false; 
    char ch = istr.peek();
    while (ch == ' ' || ch == '\t') {
        found = true;
        istr.get();
        ch = istr.peek();
    }
    if (type == RWS && !found) {
        throw std::runtime_error("Malformed HTTP message");
    }
    return istr;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
