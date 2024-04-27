#include <iostream>
#include <string>
#include "mud/http/request.h"
#include "mud/http/response.h"

BEGIN_MUDLIB_HTTP_NS

/*
 * Special characters with specific meaning in the HTTP protocol.
 */
const char SP = 32;
const char CR = 13;
const char LF = 10;
const char CL = 58;
const char HT =  9;
const char DQ = 34;

/*
 * Token manipulation options. Extends the allowable characters when
 * tokenising the input stream.
 */
struct token_manip
{
    bool space; /* Include a space to be part of the token. */
    bool colon; /* Include a colon to be part of the token. */
};
constexpr token_manip include_none = { 0, 0 };
constexpr token_manip include_space = { 1, 0 };
constexpr token_manip include_colon = { 0, 1 };
constexpr token_manip include_all = { 1, 1 };

/*
 * Return the next HTTP message token. Additional token manipulation
 * options can be provided.
 */
std::string
tokenise(std::istream& istr, const token_manip& manip);

/*
 * Expect a specific character to be read next from an input stream. Throws
 * a 'malformed HTTP message' error if the next character does not match.
 */
std::istream&
expect(std::istream& istr, int expected);

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
