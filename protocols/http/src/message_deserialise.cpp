#include "mud/http/message.h"
#include "src/message_private.h"

BEGIN_MUDLIB_HTTP_NS

/*
 * Token manipulation options. Extends the allowable characters when
 * tokenising the input stream.
 */
struct token_manip
{
    bool space; /* Include a space to be part of the token. */
    bool colon; /* Include a colon to be part of the token. */
};

/*
 * Return the next HTTP message token. Additional token manipulation
 * options can be provided.
 */
std::string
tokenise(std::istream& istr, const token_manip& manip)
{
    /* The standard table of allowable characters. Additional characters
     * that an be governed by the token manipulators are *not* included. */
    static uint8_t table[128] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // NUL .. SI
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // DLE .. US
        0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, //  SP .. /
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
    while ((ch = istr.get()) != EOF) {
        if ((ch > 0 && ch < 127 && table[ch] == 1) ||
            (manip.space && ch == SP) || (manip.colon && ch == CL)) {
            buf[n++] = ch;
            if (n > sizeof(buf)) {
                str.append(buf, n);
                n = 0;
            }
        } else {
            buf[n] = '\0';
            break;
        }
    }
    str.append(buf, n);
    istr.unget();
    return str;
}

/*
 * Expect a specific character to be read next from an input stream. Throws
 * a 'malformed HTTP message' error if the next character does not match.
 */
void
expect(std::istream& istr, int expected)
{
    int ch = istr.get();
    if (ch != expected) {
        throw std::runtime_error("Malformed HTTP message");
    }
}

std::istream&
operator>>(std::istream& istr, message& msg)
{
    std::string tok;

    // Tokenisation manipulators
    static const token_manip include_none = { 0, 0 };
    static const token_manip include_space = { 1, 0 };
    static const token_manip include_colon = { 0, 1 };
    static const token_manip include_all = { 1, 1 };

    // Ensure to start with a fresh message
    msg.clear();

    // Request or response line
    tok = tokenise(istr, include_none);
    if (tok.substr(0, 4) == "HTTP") {
        // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
        msg.parse_field<http::version>(tok);
        expect(istr, SP);
        msg.parse_field<http::status_code>(tokenise(istr, include_none));
        expect(istr, SP);
        msg.parse_field<http::reason_phrase>(tokenise(istr, include_all));
        expect(istr, CR);
        expect(istr, LF);
    } else {
        // Request-Line = Method SP Request-URI SP HTTP-Version CRLF
        msg.parse_field<http::method>(tok);
        expect(istr, SP);
        msg.parse_field<http::uri>(tokenise(istr, include_colon));
        expect(istr, SP);
        msg.parse_field<http::version>(tokenise(istr, include_none));
        expect(istr, CR);
        expect(istr, LF);
    }

    // Any header. At this stage there is no distinction made between a
    // request or response specific header - all are accepted.
    while (istr.peek() != CR) {
        tok = tokenise(istr, include_none);
        if (tok == "Date") {
            expect(istr, CL);
            expect(istr, SP);
            msg.parse_field<http::date>(tokenise(istr, include_all));
        } else if (tok == "Content-Length") {
            expect(istr, CL);
            expect(istr, SP);
            msg.parse_field<http::content_length>(tokenise(istr, include_none));
        } else if (tok == "Pragma" || tok == "Authorization" || tok == "From" ||
                   tok == "If-Modified-Since" || tok == "Referer" ||
                   tok == "User-Agent" || tok == "Location" ||
                   tok == "Server" || tok == "WWW-Authenticate" ||
                   tok == "Allow" || tok == "Content-Encoding" ||
                   tok == "Content-Type" || tok == "Expires" ||
                   tok == "Last-Modified") {
            // Consume, but ignore
            expect(istr, CL);
            expect(istr, SP);
            (void)tokenise(istr, include_all);
        } else {
            // Extension Header
            expect(istr, CL);
            expect(istr, SP);
            (void)tokenise(istr, include_all);
        }
        expect(istr, CR);
        expect(istr, LF);
    }

    // The entity seprator
    expect(istr, CR);
    expect(istr, LF);

    // The entity body
    if (msg.exists<http::content_length>()) {
        int content_length = msg.field<http::content_length>();
        std::string body;
        body.resize(content_length);
        istr.read(&body[0], content_length);
        msg.parse_field<http::entity_body>(body);
    }
    return istr;
}

void
version::parse(const std::string& tok)
{
    if (tok == "HTTP/1.0") {
        _value = version::Version::HTTP10;
    } else {
        throw std::out_of_range("Incorrectly formatted HTTP Version");
    }
}

void
method::parse(const std::string& tok)
{
    _value = tok;
}

void
uri::parse(const std::string& tok)
{
    _value = tok;
}

void
date::parse(const std::string& tok)
{
    // Only support RFC 822 with GMT timezone.
    struct tm tm = {};
    if (::strptime(tok.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &tm) == nullptr) {
        throw std::out_of_range("Incorrectly formatted HTTP Date");
    }

    // The 'struct tm' is supposed to be localtime. Convert it to time_t which
    // is supposed to be UTC by adding the GMT offset/
    tm.tm_isdst = 0;
    time_t tmp = ::mktime(&tm);
    time_t utc = tmp + localtime(&tmp)->tm_gmtoff;
    _value = std::chrono::system_clock::from_time_t(utc);
}

void
status_code::parse(const std::string& tok)
{
    int value = std::stoi(tok);
    if (value < 100 || value > 999)
        throw std::out_of_range("Incorrectly formatted HTTP Status-Code");
    _value = value;
}

void
reason_phrase::parse(const std::string& tok)
{
    _value = tok;
}

void
content_length::parse(const std::string& tok)
{
    int value = std::stoi(tok);
    if (value < 0) {
        throw std::out_of_range("Incorrectly formatted HTTP Status-Code");
    }
    _value = value;
}

void
entity_body::parse(const std::string& tok)
{
    _value = tok;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
