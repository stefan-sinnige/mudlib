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

#include <mud/core/uri.h>
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>

BEGIN_MUDLIB_CORE_NS

// The parsing alrgorithm uses a bit-masking table for each allowable character
// that identifies its character class it belongs to, as defined in RFC 3986.
const uint8_t none = 0x00;
const uint8_t gen_delims = 0x01;
const uint8_t sub_delims = 0x02;
const uint8_t reserved = gen_delims | sub_delims;
const uint8_t unreserved = 0x04;
const uint8_t hex_digit = 0x08;
const uint8_t alpha = 0x10;
const uint8_t digit = 0x20;
const uint8_t pct_encoded = 0x40;

uint8_t table[128] = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // NUL .. BEL
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, //  BS .. SI
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // DLE .. ETB
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // CAN .. US
    0x00, 0x02, 0x00, 0x01, 0x02, 0x40, 0x02, 0x02, //  SP .. '
    0x02, 0x02, 0x02, 0x02, 0x02, 0x04, 0x04, 0x01, //   ( .. /
    0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, 0x6C, //   0 .. 7
    0x6C, 0x6C, 0x01, 0x02, 0x00, 0x02, 0x00, 0x01, //   8 .. ?
    0x01, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x14, //   @ .. G
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, //   H .. O
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, //   P .. W
    0x14, 0x14, 0x14, 0x01, 0x00, 0x01, 0x00, 0x04, //   X .. _
    0x00, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x5C, 0x14, //   ` .. g
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, //   h .. o
    0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, 0x14, //   p .. w
    0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x04, 0x00, //   x .. DEL
};

unsigned char
hex_digit_to_value(unsigned char ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return 0;
}

unsigned char
value_to_hex_digit(unsigned char ch)
{
    ch = ch & 0x0F;
    if (ch >= 0 && ch <= 9) {
        return ch + '0';
    }
    if (ch >= 10 && ch <= 15) {
        return ch - 10 + 'A';
    }
    return '0';
}

static std::string
decode(const std::string& pct_encoded)
{
    std::string pct_decoded;
    for (auto iter = pct_encoded.cbegin(); iter != pct_encoded.cend(); ++iter) {
        if (*iter != '%') {
            pct_decoded.push_back(*iter);
        }
        else {
            if ((iter + 2) != pct_encoded.cend() &&
                (table[*(iter+1)] & hex_digit) &&
                (table[*(iter+2)] & hex_digit))
            {
                unsigned char ch = (hex_digit_to_value(*(++iter)) << 4) +
                                   hex_digit_to_value(*(++iter));
                pct_decoded.push_back(ch);
            }
            else {
                pct_decoded.push_back(*iter);
            }
        }
    }
    return pct_decoded;
}

static std::string
encode(const std::string& pct_decoded)
{
    std::string pct_encoded;
    for (unsigned char ch: pct_decoded) {
        uint8_t mask = table[ch];
        if (mask & unreserved) {
            pct_encoded.push_back(ch);
        }
        else {
            pct_encoded.push_back('%');
            pct_encoded.push_back(value_to_hex_digit(ch >> 4));
            pct_encoded.push_back(value_to_hex_digit(ch & 0x0F));
        }
    }
    return pct_encoded;
}

std::map<std::string, uint16_t> normalised_ports = {
    {"ftp", 21},
    {"http", 80},
    {"https", 443},
    {"sftp", 22},
    {"ssh", 22}
};

uri::uri()
  : _port(0)
{
}

uri::uri(const std::string& str)
{
    std::stringstream sstr(str);
    sstr >> *this;
}

bool
uri::operator==(const mud::core::uri& rhs) const
{
    auto normalised_lhs = *this;
    auto normalised_rhs = rhs;
    normalised_lhs.normalise();
    normalised_rhs.normalise();
    return normalised_lhs._scheme == normalised_rhs._scheme
        && normalised_lhs._user_info == normalised_rhs._user_info
        && normalised_lhs._host == normalised_rhs._host
        && normalised_lhs._port == normalised_rhs._port
        && normalised_lhs._path == normalised_rhs._path
        && normalised_lhs._query == normalised_rhs._query
        && normalised_lhs._query_params == normalised_rhs._query_params
        && normalised_lhs._fragment == normalised_rhs._fragment;
}

bool
uri::operator!=(const mud::core::uri& rhs) const
{
    return ! (*this == rhs);
}

void
uri::scheme(const std::string& value)
{
    // Scheme is case-insensitive
    _scheme = value;
    std::transform(_scheme.begin(), _scheme.end(), _scheme.begin(),
        [](unsigned char c) { return std::tolower(c); });
}

void
uri::host(const std::string& value)
{
    // Host is case-insensitive
    _host = value;
    std::transform(_host.begin(), _host.end(), _host.begin(),
        [](unsigned char c) { return std::tolower(c); });
}

void
uri::path(const std::filesystem::path& value)
{
    // Normalise the path
    _path = value.lexically_normal();
}

void
uri::query(const std::string& value)
{
    _query = value;
}

void
uri::query_params(const query_params_t& value)
{
    _query_params = value;
}

std::string
uri::authority() const
{
    std::stringstream auth;
    if (!_user_info.empty()) {
        auth << _user_info << '@';
    }
    auth <<  _host;
    if (_port != 0) {
        auth << ':' << _port;
    }
    return auth.str();
}

void
uri::clear()
{
    _scheme.clear();
    _user_info.clear();
    _host.clear();
    _port = 0;
    _path.clear();
    _query.clear();
    _query_params.clear();
    _fragment.clear();
}

bool
uri::empty() const
{
    return _scheme.empty() &&
           _user_info.empty() &&
           _host.empty() &&
           _port == 0 &&
           _path.empty() &&
           _query.empty() &&
           _query_params.empty() &&
           _fragment.empty();
}

void
uri::normalise()
{
    syntax_normalisation();
    case_normalisation();
    percent_encoding_normalisation();
    path_segment_normalisation();
    scheme_normalisation();
    protocol_normalisation();
}

void
uri::syntax_normalisation()
{
}

void
uri::case_normalisation()
{
    // The proper case is already applied when each field it set.
}

void
uri::percent_encoding_normalisation()
{
    // Only needed when streamed to an output stream. The implementation is
    // using uppercase hex digits.
}

void
uri::path_segment_normalisation()
{
    // Paths are already lexcially normalised when set.

    // Absolute URI's always have a path starting with '/'
    if (absolute()) {
        if (_path.empty() || _path.native()[0] != '/') {
            _path = std::filesystem::path("/") / _path;
        }
    }
}

void
uri::scheme_normalisation()
{
    // If a port has not been set, use the scheme associated port number.
    if (_port == 0) {
        auto norm_port = normalised_ports.find(_scheme);
        if ((norm_port != normalised_ports.end())) {
            _port = norm_port->second;
        }
    }
}

void
uri::protocol_normalisation()
{
}

std::istream&
operator>>(std::istream& istr, uri& value)
{
    char ch;
    std::string buffer;

    // Empty the value
    value.clear();

    // Scheme
    while((ch = istr.get()) != std::char_traits<char>::eof()) {
        if (ch < 0 || ch > 127) {
            // Out of character range
            throw std::runtime_error("Malformed URI");
        }
        uint8_t mask = table[ch];
        if ((mask & alpha) || (mask & digit) | (ch == '+') || (ch == '-') ||
            (ch == '.'))
        {
            buffer.push_back(ch);
        }
        else {
            break;
        }
    }
    if (ch == ':') {
        value.scheme(decode(buffer));
        buffer.clear();
        ch = istr.get();
    }

    // Authority (user-info, host and port) - is only possible after two slashes
    if (buffer.empty() && (ch == '/') && istr.peek() == '/') {
        (void) istr.get();

        std::string authority;
        while((ch = istr.get()) != std::char_traits<char>::eof()) {
            if (ch < 0 || ch > 127) {
                // Out of character range
                throw std::runtime_error("Malformed URI");
            }
            uint8_t mask = table[ch];
            if ((mask & unreserved) || (ch == '@') || (ch == ':')) {
                authority.push_back(ch);
            }
            else {
                break;
            }
        }

        // User-info
        auto first = authority.find_first_of('@');
        if (first != std::string::npos) {
            if (first != authority.find_last_of('@')) {
                // Only expect a single '@' in the authority
                throw std::runtime_error("Malformed URI");
            }
            value.user_info(decode(authority.substr(0, first)));
            authority.erase(0, first+1);
        }

        // Host
        first = authority.find_first_of(':');
        if (first != std::string::npos) {
            if (first != authority.find_last_of(':')) {
                // Only expect a single ':' in the host/port
                throw std::runtime_error("Malformed URI");
            }
            value.host(decode(authority.substr(0, first)));
            authority.erase(0, first+1);
        }
        else {
            value.host(decode(authority));
            authority.clear();
        }

        // Port
        if (!authority.empty()) {
            uint16_t port = 0;
            for (auto ch: authority) {
                uint8_t mask = table[ch];
                if (mask & digit) {
                    port = port * 10 + (ch - '0');
                }
                else {
                    throw std::runtime_error("Malformed URI");
                }
            }
            value.port(port);
        }
    }

    // Path
    while (ch != std::char_traits<char>::eof()) {
        uint8_t mask = table[ch];
        if ((mask & unreserved) || (mask & pct_encoded) || (mask & sub_delims)
            || (ch == '/') || (ch == ':') || (ch == '@'))
        {
            buffer.push_back(ch);
            ch = istr.get();
        }
        else {
            break;
        }
    }
    value.path(decode(buffer));

    // query
    if (ch == '?') {
        // Get the query as a single string
        std::string query;
        while((ch = istr.get()) != std::char_traits<char>::eof()) {
            uint8_t mask = table[ch];
            if ((mask & unreserved) || (mask & pct_encoded) ||
                (mask & sub_delims) || (ch == ':') || (ch == '@') ||
                (ch == '/') || (ch == '?'))
            {
                query.push_back(ch);
            }
            else {
                break;
            }
        }
        value.query(decode(query));

        // Get the query as parameters
        std::string::size_type start = 0;
        std::string::size_type end;
        uri::query_params_t query_params;
        while ((end = query.find_first_of('=', start)) != std::string::npos) {
            auto key =  decode(query.substr(start, end - start));
            start = end + 1;
            std::string::size_type end = query.find_first_of('&', start);
            auto value = decode(query.substr(start, end - start));
            if (end != std::string::npos) {
                start = end + 1;
            }
            query_params.push_back(std::make_pair(
                decode(key), decode(value)));
        }
        value.query_params(query_params);
    }

    // fragment
    if (ch == '#') {
        std::string fragment;
        while((ch = istr.get()) != std::char_traits<char>::eof()) {
            uint8_t mask = table[ch];
            if ((mask & unreserved) || (mask & pct_encoded) ||
                (mask & sub_delims) || (ch == ':') || (ch == '@') ||
                (ch == '/') || (ch == '?'))
            {
                fragment.push_back(ch);
            }
            else {
                break;
            }
       }
        value.fragment(decode(fragment));
    }

    // Normalise it.
    value.normalise();

    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const uri& _value)
{
    // Output a normalised URI
    uri value = _value;
    value.normalise();

    if (value.absolute()) {
        // The scheme and ":"
        ostr << value.scheme() << ":";

        // If there is a host name: "//" [user_info "@"] host [":" port]
        if (value.host().size()) {
            ostr << "//";
            if (value.user_info().size()) {
                ostr << encode(value.user_info()) << "@";
            }
            ostr << encode(value.host());
            auto norm_port = normalised_ports.find(value.scheme());
            if ((norm_port != normalised_ports.end()) &&
                (value.port() != norm_port->second) &&
                (value.port() != 0))
            {
                ostr << ":" << value.port();
            }
        }
    }

    // The path (either absolute or relative)
    ostr << value.path().native();

    // The optional query parameters: "?" query
    //   Where query is either query parameters: key "=" value "&"
    //   Or the single query string
    // This allows for query parameters to be properly encoded while their
    // separators ("=" and "&" are iutput as-is).
    if (value.query_params().size()) {
        ostr << "?";
        for (auto iter = value.query_params().cbegin();
             iter != value.query_params().cend();
             ++iter)
        {
            if (iter != value.query_params().cbegin()) {
                ostr << "&";
            }
            ostr << encode(iter->first) << "=" << encode(iter->second);
        }
    }
    else 
    if (value.query().size()) {
        ostr << "?" << value.query();
    }

    // The optional fragment: "#" fragment
    if (value.fragment().size()) {
        ostr << "#" << encode(value.fragment());
    }
    return ostr;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
