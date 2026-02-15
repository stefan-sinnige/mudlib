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

#include <mud/core/json.h>
#include <cstdlib>

BEGIN_MUDLIB_CORE_NS

END_MUDLIB_CORE_NS

const std::string ERR_INVALID_ARRAY     = "invalid JSON array";
const std::string ERR_INVALID_OBJECT    = "invalid JSON object";
const std::string ERR_INVALID_TEXT      = "invalid JSON text";
const std::string ERR_NOT_A_NUMBER      = "not a number";
const std::string ERR_SYNTAX            = "syntax error";
const std::string ERR_TOO_MANY_DIGITS   = "too many digits";
const std::string ERR_UNENCLOSED_STRING = "unenclosed string";
const std::string ERR_UNKNOWN_ESCAPE    = "unknown escape sequence";

/*
 * The parsing tokens. These tokens reflect what the parser can recognise and
 * reflect a JSON value or a non-whitespace structural character. Others are
 * the end-of-input or an error in parsing.
 */
enum class Token
{
    Unknown,
    EndOfInput,
    Null,
    String,
    Boolean,
    Number,
    Comma,
    Colon,
    OpenArray,
    CloseArray,
    OpenObject,
    CloseObject
};

/*
 * Skip all white space.
 */
std::istream&
whitespace(std::istream& istr)
{
    char ch = istr.peek();
    while (ch == 0x20 || ch == 0x09 || ch == 0x0A || ch == 0x0D) {
        ch = istr.get();
        ch = istr.peek();
    }
    return istr;
}

/*
 * Parse until we have a token. A token is anything that is part of a JSON
 * grammar, except for white-spaces. Special tokens are emitted for end of
 * input or any error encountered during parsing.
 */
Token
tokenise(std::istream& istr, mud::core::json& value)
{
    char buf[128];
    int n = 0;

    /* Skip all white space */
    whitespace(istr);
   
    /* Check for end of input. */
    int ch = istr.get();
    if (ch == std::char_traits<char>::eof()) {
        return Token::EndOfInput;
    }

    /* Check for single character matches */
    switch(ch) {
        case 0x2C:
            return Token::Comma;
        case 0x3A:
            return Token::Colon;
        case 0x5B:
            return Token::OpenArray;
        case 0x5D:
            return Token::CloseArray;
        case 0x7B:
            return Token::OpenObject;
        case 0x7D:
            return Token::CloseObject;
    }

    /* Check for a string. */
    if (ch == 0x22) {
        std::string str;
        while (1) {
            /* Get next character */
            if ((ch = istr.get()) == std::char_traits<char>::eof()) {
                throw std::runtime_error(ERR_UNENCLOSED_STRING);
            }

            /* Check for end of string */
            if (ch == 0x22) {
                str.append(buf, n);
                value = str;
                return Token::String;
            }

            /* Check for escape. */
            if (ch == 0x5C) {
                ch = istr.get();
                if (ch == std::char_traits<char>::eof()) {
                    throw std::runtime_error(ERR_UNENCLOSED_STRING);
                }
                switch (ch) {
                    case 0x22:
                        ch = '"';
                        break;
                    case 0x5C:
                        ch = '\\';
                        break;
                    case 0x2F:
                        ch = '/';
                        break;
                    case 0x62:
                        ch = '\b';
                        break;
                    case 0x6E:
                        ch = '\f';
                        break;
                    case 0x72:
                        ch = '\t';
                        break;
                    case 0x74:
                        break;
                    default:
                        throw std::runtime_error(ERR_UNKNOWN_ESCAPE);
                }
                if (ch == 0x74) {
                    /* TODO: handle \UXXXX */
                }
            }

            /* Store the single (transcoded) character as part of a string */
            buf[n++] = ch;
            if (n > sizeof(buf)) {
                str.append(buf, n);
                n = 0;
            }
        }
    }

    /* Check 'null' */
    if (ch == 'n') {
        bool ok = true;
        ok = ok && ((ch = istr.get()) == 'u');
        ok = ok && ((ch = istr.get()) == 'l');
        ok = ok && ((ch = istr.get()) == 'l');
        int lookahead = istr.peek();
        ok = ok && ((lookahead == std::char_traits<char>::eof()) ||
                    lookahead == 0x20 || lookahead == 0x09 ||
                    lookahead == 0x0A || lookahead == 0x0D ||
                    lookahead == 0x2C || lookahead == 0x5D ||
                    lookahead == 0x7D);
        if (ok) {
            value = nullptr;
            return Token::Null;
        }
    }

    /* Check 'false' */
    if (ch == 'f') {
        bool ok = true;
        ok = ok && ((ch = istr.get()) == 'a');
        ok = ok && ((ch = istr.get()) == 'l');
        ok = ok && ((ch = istr.get()) == 's');
        ok = ok && ((ch = istr.get()) == 'e');
        int lookahead = istr.peek();
        ok = ok && ((lookahead == std::char_traits<char>::eof()) ||
                    lookahead == 0x20 || lookahead == 0x09 ||
                    lookahead == 0x0A || lookahead == 0x0D ||
                    lookahead == 0x2C || lookahead == 0x5D ||
                    lookahead == 0x7D);
        if (ok) {
            value = false;
            return Token::Boolean;
        }
    }

    /* Check 'true' */
    if (ch == 't') {
        bool ok = true;
        ok = ok && ((ch = istr.get()) == 'r');
        ok = ok && ((ch = istr.get()) == 'u');
        ok = ok && ((ch = istr.get()) == 'e');
        int lookahead = istr.peek();
        ok = ok && ((lookahead == std::char_traits<char>::eof()) ||
                    lookahead == 0x20 || lookahead == 0x09 ||
                    lookahead == 0x0A || lookahead == 0x0D ||
                    lookahead == 0x2C || lookahead == 0x5D ||
                    lookahead == 0x7D);
        if (ok) {
            value = true;
            return Token::Boolean;
        }
    }

    /* Check a number */
    if (ch == '-' || (ch >= '0' && ch <= '9')) {
        /* Keep track of the numeric cmponent */
        enum class component_t {
            integer,
            fraction,
            exponent
        } comp = component_t::integer;

        /* Parse the number */
        buf[n++] = ch;
        while (true) {
            /* Check if it is too big */
            if (n >= sizeof(buf)) {
                    throw std::out_of_range(ERR_TOO_MANY_DIGITS);
            }

            /* Parse a valid number */
            ch = istr.get();
            if (ch >= '0' && ch <= '9') {
                /* Valid in any component */
            }
            else if (ch == '.') {
                if (comp == component_t::integer) {
                    comp = component_t::fraction;
                }
                else {
                    throw std::runtime_error(ERR_NOT_A_NUMBER);
                }
            }
            else if (ch == 0x65 || ch == 0x45) {
                if (comp == component_t::integer ||
                    comp == component_t::fraction)
                {
                    comp = component_t::exponent;
                }
                else {
                    throw std::runtime_error(ERR_NOT_A_NUMBER);
                }
            }
            else
            if (ch == '-' || ch == '+') {
                if (comp != component_t::exponent) {
                    throw std::runtime_error(ERR_NOT_A_NUMBER);
                }
            }
            else {
                if (ch == std::char_traits<char>::eof() ||
                    ch == 0x20 || ch == 0x09 ||
                    ch == 0x0A || ch == 0x0D ||
                    ch == 0x2C || ch == 0x5D ||
                    ch == 0x7D)
                {
                    istr.unget();
                    break;
                }
                else {
                    throw std::runtime_error(ERR_NOT_A_NUMBER);
                }
            }
            buf[n++] = ch;

        }
        buf[n] = '\0';

        /* If we are still in the integer component state, then we have an
         * integer, otherwise we'll have a decimal number */
        if (comp == component_t::integer) {
            value = (int64_t)strtoll(buf, nullptr, 10);
        }
        else {
            value = std::strtold(buf, nullptr);
        }
        return Token::Number;
    }

    throw std::runtime_error(ERR_SYNTAX);
};

/*
 * Return the next token without consuming it.
 * The next token can be an educated guess and does not indicate that it will
 * be successfully parsed. Only the bare minimum is read to establish the guess.
 * Note that any white-space will be consumed.
 */
Token
lookahead(std::istream& istr)
{
    int ch;

    /* Skip all white space */
    whitespace(istr);

    /* Peek the next non-white space character. A single character should be
     * enough to determine the (start-of) a known token. */
    ch = istr.peek();
    switch (ch) {
        case std::char_traits<char>::eof():
            return Token::EndOfInput;
        case 0x2C:
            return Token::Comma;
        case 0x3A:
            return Token::Colon;
        case 0x5B:
            return Token::OpenArray;
        case 0x5D:
            return Token::CloseArray;
        case 0x7B:
            return Token::OpenObject;
        case 0x7D:
            return Token::CloseObject;
        case 0x22:
            return Token::String;
        case 'n':
            return Token::Null;
        case 't':
        case 'f':
            return Token::Boolean;
    }
    if (ch == '-' || (ch >= '0' || ch <= '9')) {
        return Token::Number;
    }
    return Token::Unknown;
}

/*
 * Expect the next token to be of some value. This is only to be designed for
 * structural tokens, not tokens that represent a JSON value.
 * Throws an exception if the next token does not match the expectation.
 */
void
expect(std::istream& istr, Token expected)
{
    mud::core::json dummy;
    auto token = tokenise(istr, dummy);
    if (token != expected) {
        throw std::runtime_error(ERR_SYNTAX);
    }
}

std::istream&
operator>>(std::istream& istr, mud::core::json& value)
{
    /*
     * Read the next token and process it:
     *    - Basic single value are returned as-is
     *    - Array opening bracket create an array with embedded values, until a
     *      matching closing bracket is found. This involves recursion.
     *    - Object opening bracket create an object with key-value pairs, until
     *      a matching closing bracket is found. This involves recursion.
     */
    auto token = tokenise(istr, value);
    if (token == Token::Null || token == Token::String ||
        token == Token::Boolean || token == Token::Number)
    {
        // Basic single value
    }
    else
    if (token == Token::OpenArray) {
        // Read all elements until the next token is a closing array
        mud::core::json::array_t arr;
        mud::core::json element;
        Token lookahead_token;
        while ((lookahead_token = lookahead(istr)) != Token::CloseArray) {
            // If this is not the first element, expect a comma
            if (arr.size() > 0) {
                if (lookahead_token != Token::Comma) {
                    throw std::runtime_error(ERR_INVALID_ARRAY);
                }
                expect(istr, Token::Comma);
            }

            // Get the element. Note that this can be another complex value.
            istr >> element;
            arr.push_back(element);
        }

        // Expect a closing array
        expect(istr, Token::CloseArray);
        value = arr;
    }
    else
    if (token == Token::OpenObject) {
        // Read all key-value elements until the next token is a closing object
        mud::core::json::object_t obj;
        Token lookahead_token = lookahead(istr); 
        while ((lookahead_token = lookahead(istr)) != Token::CloseObject) {
            // If this is not the first element, expect a comma
            if (obj.size() > 0) {
                if (lookahead_token != Token::Comma) {
                    throw std::runtime_error(ERR_INVALID_OBJECT);
                }
                expect(istr, Token::Comma);
            }

            // Get the key-value pair, separated by a colon. Note that the
            // key is always a string, but the value can be another complex
            // JSON structure.
            mud::core::json key;
            mud::core::json val;
            token = tokenise(istr, key);
            if (token != Token::String) {
                throw std::runtime_error(ERR_INVALID_OBJECT);
            }
            expect(istr, Token::Colon);
            istr >> val;
            obj[key.string()] = val;
        }

        // Expect a closing object
        expect(istr, Token::CloseObject);
        value = obj;
    }
    else {
        throw std::runtime_error(ERR_INVALID_TEXT);
    }
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const mud::core::json& value)
{
    switch (value.type())
    {
        case mud::core::json::type_t::null:
            ostr << "null";
            break;
        case mud::core::json::type_t::string:
            ostr << "\"" << value.string() << "\"";
            break;
        case mud::core::json::type_t::integer:
            ostr << value.integer();
            break;
        case mud::core::json::type_t::decimal:
            ostr << value.decimal();
            break;
        case mud::core::json::type_t::boolean:
            ostr << (value.boolean() ? "true" : "false");
            break;
        case mud::core::json::type_t::array: {
            bool comma = false;
            ostr << "[";
            for( const auto& element: value.array()) {
                if (comma) ostr << ',';
                ostr << element;
                comma = true;
            }
            ostr << "]";
            break; }
        case mud::core::json::type_t::object: {
            bool comma = false;
            ostr << "{";
            for( const auto& [key, value]: value.object()) {
                if (comma) ostr << ',';
                ostr << "\"" << key << "\":";
                ostr << value;
                comma = true;
            }
            ostr << "}";
            break; }
    }
    return ostr;
}

/* vi: set ai ts=4 expandtab: */
