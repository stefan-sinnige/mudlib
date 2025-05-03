#include "mud/http/field.h"
#include "tokenise.h"
#include <ctime>
#include <stdexcept>

BEGIN_MUDLIB_HTTP_NS

/*
 * Case insensitive equalily on strings.
 */
bool
equal_case(const std::string& a, const std::string& b)
{
    auto a_it = a.begin();
    auto b_it = b.begin();
    while (a_it != a.end() && std::tolower(*a_it) == std::tolower(*b_it)) {
        ++a_it, ++b_it;
    }
    return a_it == a.end() && b_it == b.end();
}

/*
 * Skip input (space and commas) until we have the next CSV value. Returns
 * true if there is a CSV value ready.
 */
bool
skip_csv(std::istream& istr)
{
    int ch;
    while ((ch = istr.peek()) &&
           ((ch == SP) || (ch == SP) || (ch == HT) || (ch == CM))) {
        istr.get();
    }
    return istr.peek() != CR;
}

/* ======================================================================
 * EXTENSION
 * ====================================================================== */

void
field_ext::value(std::ostream& ostr) const
{
    ostr << *this;
}

void
field_ext::value(std::istream& istr)
{
    istr >> *this;
}

std::ostream&
operator<<(std::ostream& ostr, const field_ext& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, field_ext& field)
{
    field.value(tokenise(istr, include_all));
    return istr;
}

/* ======================================================================
 * Version
 * ====================================================================== */

const char _HTTP_VERSION[] = "__Version";

std::ostream&
operator<<(std::ostream& ostr, const version& field)
{
    switch (field.value()) {
        case version_e::HTTP10:
            ostr << "HTTP/1.0";
            break;
        case version_e::HTTP11:
            ostr << "HTTP/1.1";
            break;
        default:
            break;
    }
    return ostr;
}

std::istream&
operator>>(std::istream& istr, version& field)
{
    std::string tok = tokenise(istr, include_none);
    if (tok == "HTTP/1.0") {
        field.value(version_e::HTTP10);
    } else if (tok == "HTTP/1.1") {
        field.value(version_e::HTTP11);
    } else {
        throw std::out_of_range("Incorrectly formatted HTTP Version");
    }
    return istr;
}

/* ======================================================================
 * Method
 * ====================================================================== */

const char _HTTP_METHOD[] = "__Method";

const std::string GET = "GET";
const std::string HEAD = "HEAD";
const std::string POST = "POST";
const std::string PUT = "PUT";
const std::string DELETE = "DELETE";
const std::string CONNECT = "CONNECT";
const std::string OPTIONS = "OPTIONS";
const std::string TRACE = "TRACE";

request_method::request_method()
    : _type(method_e::UNKNOWN)
{
}

request_method::request_method(method_e type)
    : _type(type)
{
}

request_method::request_method(const std::string& ext)
{
    if (ext == GET) {
        _type = method_e::GET;
    } else if (ext == HEAD) {
        _type = method_e::HEAD;
    } else if (ext == POST) {
        _type = method_e::POST;
    } else if (ext == PUT) {
        _type = method_e::PUT;
    } else if (ext == DELETE) {
        _type = method_e::DELETE;
    } else if (ext == CONNECT) {
        _type = method_e::CONNECT;
    } else if (ext == OPTIONS) {
        _type = method_e::OPTIONS;
    } else if (ext == TRACE) {
        _type = method_e::TRACE;
    } else {
        _type = method_e::EXT;
        _ext = ext;
    }
}

method_e
request_method::type() const
{
    return _type;
}

request_method::operator method_e() const
{
    return _type;
}

const std::string&
request_method::str() const
{
    switch (_type) {
        case method_e::GET:
            return GET;
        case method_e::HEAD:
            return HEAD;
        case method_e::POST:
            return POST;
        case method_e::PUT:
            return PUT;
        case method_e::DELETE:
            return DELETE;
        case method_e::CONNECT:
            return CONNECT;
        case method_e::OPTIONS:
            return OPTIONS;
        case method_e::TRACE:
            return TRACE;
        case method_e::EXT:
            return _ext;
        default:
            return GET;
    }
}

std::ostream&
operator<<(std::ostream& ostr, const request_method& value)
{
    ostr << value.str();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, request_method& value)
{
    std::string tok = tokenise(istr, include_none);
    value = request_method(tok);
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const method& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, method& field)
{
    request_method value;
    istr >> value;
    field = method(value);
    return istr;
}

/* ======================================================================
 * URI
 * ====================================================================== */

const char _HTTP_URI[] = "__URI";

std::ostream&
operator<<(std::ostream& ostr, const uri& field)
{
    mud::core::uri abspath = field.value();
    abspath.scheme("");
    ostr << abspath;
    return ostr;
}

std::istream&
operator>>(std::istream& istr, uri& field)
{
    std::string tok = tokenise(istr, include_colon);
    field.value(tok);
    return istr;
}

/* ======================================================================
 * Status Code
 * ====================================================================== */

const char _HTTP_STATUS_CODE[] = "__StatusCode";

std::ostream&
operator<<(std::ostream& ostr, const status_code& field)
{
    ostr << static_cast<std::underlying_type_t<status_code_e>>(field.value());
    return ostr;
}

std::istream&
operator>>(std::istream& istr, status_code& field)
{
    std::string tok = tokenise(istr, include_none);
    int value = std::stoi(tok);
    if (value < 100 || value > 999)
        throw std::out_of_range("Incorrectly formatted HTTP Status-Code");
    field.value(static_cast<status_code_e>(value));
    return istr;
}

/* ======================================================================
 * Reason Phrase
 * ====================================================================== */

const char _HTTP_REASON_PHRASE[] = "__ReasonPhrase";

const std::string Continue = "Continue";
const std::string SwitchingProtocols = "Switching Protocols";
const std::string OK = "OK";
const std::string Created = "Created";
const std::string Accepted = "Accepted";
const std::string NoContent = "No Content";
const std::string ResetContent = "Reset Content";
const std::string PartialContent = "Partial Content";
const std::string MultipleChoice = "Multiple Choice";
const std::string MovedPermanently = "Moved Permanently";
const std::string MovedTemporarily = "Moved Temporarily";
const std::string NotModified = "Not Modified";
const std::string UseProxy = "Use Proxy";
const std::string TemporaryRedirect = "Temporary Redirect";
const std::string BadRequest = "Bad Request";
const std::string Unauthorized = "Unauthorized";
const std::string Forbidden = "Forbidden";
const std::string NotFound = "Not Found";
const std::string InternalServerError = "Internal Server Error";
const std::string NotAcceptable = "Not Acceptable";
const std::string ProxyAuthenticationRequired = "Proxy Authentication Required";
const std::string RequestTimeout = "Request Time-out";
const std::string Conflict = "Conflict";
const std::string Gone = "Gone";
const std::string LengthRequired = "Length Required";
const std::string PreconditionFailed = "Precondition Failed";
const std::string RequestEntityTooLarge = "Request Entity Too Large";
const std::string RequestURITooLarge = "Request-URI Too Large";
const std::string UnsupportedMediaType = "Unsupported Media Type";
const std::string RequestRangeNotSatifiable = "Request range not satisfiable";
const std::string ExpectationFailed = "Expectation Failed";
const std::string NotImplemented = "Not Implemented";
const std::string BadGateway = "Bad Gateway";
const std::string ServiceUnavailable = "Service Unavailable";
const std::string GatewayTimeout = "Gateway Time-out";
const std::string HTTPVersionNotSupported = "HTTP Version not supported";

std::ostream&
operator<<(std::ostream& ostr, const reason_phrase& field)
{
    switch (field.value()) {
        case reason_phrase_e::Continue:
            ostr << Continue;
            break;
        case reason_phrase_e::SwitchingProtocols:
            ostr << SwitchingProtocols;
            break;
        case reason_phrase_e::OK:
            ostr << OK;
            break;
        case reason_phrase_e::Created:
            ostr << Created;
            break;
        case reason_phrase_e::Accepted:
            ostr << Accepted;
            break;
        case reason_phrase_e::NoContent:
            ostr << NoContent;
            break;
        case reason_phrase_e::ResetContent:
            ostr << ResetContent;
            break;
        case reason_phrase_e::PartialContent:
            ostr << PartialContent;
            break;
        case reason_phrase_e::MultipleChoice:
            ostr << MultipleChoice;
            break;
        case reason_phrase_e::MovedPermanently:
            ostr << MovedPermanently;
            break;
        case reason_phrase_e::MovedTemporarily:
            ostr << MovedTemporarily;
            break;
        case reason_phrase_e::NotModified:
            ostr << NotModified;
            break;
        case reason_phrase_e::UseProxy:
            ostr << UseProxy;
            break;
        case reason_phrase_e::TemporaryRedirect:
            ostr << TemporaryRedirect;
            break;
        case reason_phrase_e::BadRequest:
            ostr << BadRequest;
            break;
        case reason_phrase_e::Unauthorized:
            ostr << Unauthorized;
            break;
        case reason_phrase_e::Forbidden:
            ostr << Forbidden;
            break;
        case reason_phrase_e::NotFound:
            ostr << NotFound;
            break;
        case reason_phrase_e::InternalServerError:
            ostr << InternalServerError;
            break;
        case reason_phrase_e::NotAcceptable:
            ostr << NotAcceptable;
            break;
        case reason_phrase_e::ProxyAuthenticationRequired:
            ostr << ProxyAuthenticationRequired;
            break;
        case reason_phrase_e::RequestTimeout:
            ostr << RequestTimeout;
            break;
        case reason_phrase_e::Conflict:
            ostr << Conflict;
            break;
        case reason_phrase_e::Gone:
            ostr << Gone;
            break;
        case reason_phrase_e::LengthRequired:
            ostr << LengthRequired;
            break;
        case reason_phrase_e::PreconditionFailed:
            ostr << PreconditionFailed;
            break;
        case reason_phrase_e::RequestEntityTooLarge:
            ostr << RequestEntityTooLarge;
            break;
        case reason_phrase_e::RequestURITooLarge:
            ostr << RequestURITooLarge;
            break;
        case reason_phrase_e::UnsupportedMediaType:
            ostr << UnsupportedMediaType;
            break;
        case reason_phrase_e::RequestRangeNotSatifiable:
            ostr << RequestRangeNotSatifiable;
            break;
        case reason_phrase_e::ExpectationFailed:
            ostr << ExpectationFailed;
            break;
        case reason_phrase_e::NotImplemented:
            ostr << NotImplemented;
            break;
        case reason_phrase_e::BadGateway:
            ostr << BadGateway;
            break;
        case reason_phrase_e::ServiceUnavailable:
            ostr << ServiceUnavailable;
            break;
        case reason_phrase_e::GatewayTimeout:
            ostr << GatewayTimeout;
            break;
        case reason_phrase_e::HTTPVersionNotSupported:
            ostr << HTTPVersionNotSupported;
            break;
        default:
            break;
    }
    return ostr;
}

std::istream&
operator>>(std::istream& istr, reason_phrase& field)
{
    std::string tok = tokenise(istr, include_all);
    if (equal_case(tok, Continue)) {
        field.value(reason_phrase_e::Continue);
    } else if (equal_case(tok, SwitchingProtocols)) {
        field.value(reason_phrase_e::SwitchingProtocols);
    } else if (equal_case(tok, OK)) {
        field.value(reason_phrase_e::OK);
    } else if (equal_case(tok, Created)) {
        field.value(reason_phrase_e::Created);
    } else if (equal_case(tok, Accepted)) {
        field.value(reason_phrase_e::Accepted);
    } else if (equal_case(tok, NoContent)) {
        field.value(reason_phrase_e::NoContent);
    } else if (equal_case(tok, ResetContent)) {
        field.value(reason_phrase_e::ResetContent);
    } else if (equal_case(tok, PartialContent)) {
        field.value(reason_phrase_e::PartialContent);
    } else if (equal_case(tok, MultipleChoice)) {
        field.value(reason_phrase_e::MultipleChoice);
    } else if (equal_case(tok, MovedPermanently)) {
        field.value(reason_phrase_e::MovedPermanently);
    } else if (equal_case(tok, MovedTemporarily)) {
        field.value(reason_phrase_e::MovedTemporarily);
    } else if (equal_case(tok, NotModified)) {
        field.value(reason_phrase_e::NotModified);
    } else if (equal_case(tok, UseProxy)) {
        field.value(reason_phrase_e::UseProxy);
    } else if (equal_case(tok, TemporaryRedirect)) {
        field.value(reason_phrase_e::TemporaryRedirect);
    } else if (equal_case(tok, BadRequest)) {
        field.value(reason_phrase_e::BadRequest);
    } else if (equal_case(tok, Unauthorized)) {
        field.value(reason_phrase_e::Unauthorized);
    } else if (equal_case(tok, Forbidden)) {
        field.value(reason_phrase_e::Forbidden);
    } else if (equal_case(tok, NotFound)) {
        field.value(reason_phrase_e::NotFound);
    } else if (equal_case(tok, InternalServerError)) {
        field.value(reason_phrase_e::InternalServerError);
    } else if (equal_case(tok, NotAcceptable)) {
        field.value(reason_phrase_e::NotAcceptable);
    } else if (equal_case(tok, ProxyAuthenticationRequired)) {
        field.value(reason_phrase_e::ProxyAuthenticationRequired);
    } else if (equal_case(tok, RequestTimeout)) {
        field.value(reason_phrase_e::RequestTimeout);
    } else if (equal_case(tok, Conflict)) {
        field.value(reason_phrase_e::Conflict);
    } else if (equal_case(tok, Gone)) {
        field.value(reason_phrase_e::Gone);
    } else if (equal_case(tok, LengthRequired)) {
        field.value(reason_phrase_e::LengthRequired);
    } else if (equal_case(tok, PreconditionFailed)) {
        field.value(reason_phrase_e::PreconditionFailed);
    } else if (equal_case(tok, RequestEntityTooLarge)) {
        field.value(reason_phrase_e::RequestEntityTooLarge);
    } else if (equal_case(tok, RequestURITooLarge)) {
        field.value(reason_phrase_e::RequestURITooLarge);
    } else if (equal_case(tok, UnsupportedMediaType)) {
        field.value(reason_phrase_e::UnsupportedMediaType);
    } else if (equal_case(tok, RequestRangeNotSatifiable)) {
        field.value(reason_phrase_e::RequestRangeNotSatifiable);
    } else if (equal_case(tok, ExpectationFailed)) {
        field.value(reason_phrase_e::ExpectationFailed);
    } else if (equal_case(tok, NotImplemented)) {
        field.value(reason_phrase_e::NotImplemented);
    } else if (equal_case(tok, BadGateway)) {
        field.value(reason_phrase_e::BadGateway);
    } else if (equal_case(tok, ServiceUnavailable)) {
        field.value(reason_phrase_e::ServiceUnavailable);
    } else if (equal_case(tok, GatewayTimeout)) {
        field.value(reason_phrase_e::GatewayTimeout);
    } else if (equal_case(tok, HTTPVersionNotSupported)) {
        field.value(reason_phrase_e::HTTPVersionNotSupported);
    } else {
        field.value(reason_phrase_e::ExtensionCode);
    }
    return istr;
}

/* ======================================================================
 * Entity Body
 * ====================================================================== */

const char _HTTP_ENTITY_BODY[] = "__EntityBody";

std::ostream&
operator<<(std::ostream& ostr, const entity_body& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, entity_body& field)
{
    field.value("TBD");
    return istr;
}

/* ======================================================================
 * Allow
 * ====================================================================== */

const char _HTTP_ALLOW[] = "Allow";

field_factory::registrar<_HTTP_ALLOW, allow> allow_registrar;

/* ======================================================================
 * Connection
 * ====================================================================== */

const char _HTTP_CONNECTION[] = "Connection";

const std::string Close = "close";
const std::string KeepAlive = "keep-alive";

std::ostream&
operator<<(std::ostream& ostr, const connection& field)
{
    switch (field.value()) {
        case connection_e::Close:
            ostr << Close;
            break;
        case connection_e::KeepAlive:
            ostr << KeepAlive;
            break;
        default:
            break;
    }
    return ostr;
}

std::istream&
operator>>(std::istream& istr, connection& field)
{
    std::string tok = tokenise(istr, include_all);
    if (equal_case(tok, Close)) {
        field.value(connection_e::Close);
    } else if (equal_case(tok, KeepAlive)) {
        field.value(connection_e::KeepAlive);
    } else {
        throw std::out_of_range("Incorrectly formatted HTTP Connection");
    }
    return istr;
}

field_factory::registrar<_HTTP_CONNECTION, connection> connection_registrar;

/* ======================================================================
 * Content-Length
 * ====================================================================== */

const char _HTTP_CONTENT_LENGTH[] = "Content-Length";

std::ostream&
operator<<(std::ostream& ostr, const content_length& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, content_length& field)
{
    std::string tok = tokenise(istr, include_none);
    int value = std::stoi(tok);
    if (value < 0) {
        throw std::out_of_range(
            "Incorrectly formatted HTTP Content-Length field");
    }
    field.value(value);
    return istr;
}

field_factory::registrar<_HTTP_CONTENT_LENGTH, content_length>
    content_length_registrar;

/* ======================================================================
 * Date
 * ====================================================================== */

const char _HTTP_DATE[] = "Date";

std::ostream&
operator<<(std::ostream& ostr, const date& field)
{
    time_t utc = std::chrono::system_clock::to_time_t(field.value());
    struct tm tm;
    char buf[30];

#if defined(_WIN32)
    (void)gmtime_s(&tm, &utc);
#else
    (void)gmtime_r(&utc, &tm);
#endif
    strftime(buf, sizeof(buf), "%a, %d %b %Y %H:%M:%S GMT", &tm);
    ostr << buf;
    return ostr;
}

std::istream&
operator>>(std::istream& istr, date& field)
{
    std::string tok = tokenise(istr, include_all);

    // Only support RFC 822 with GMT timezone.
    struct tm tm = {};
    if (::strptime(tok.c_str(), "%a, %d %b %Y %H:%M:%S GMT", &tm) == nullptr) {
        throw std::out_of_range("Incorrectly formatted HTTP Date field");
    }

    // The 'struct tm' is supposed to be localtime. Convert it to time_t which
    // is supposed to be UTC by adding the GMT offset/
    tm.tm_isdst = 0;
    time_t tmp = ::mktime(&tm);
    time_t utc = tmp + localtime(&tmp)->tm_gmtoff;
    field.value(std::chrono::system_clock::from_time_t(utc));
    return istr;
}

field_factory::registrar<_HTTP_DATE, date> date_registrar;

/* ======================================================================
 * Host
 * ====================================================================== */

const char _HTTP_HOST[] = "Host";

std::ostream&
operator<<(std::ostream& ostr, const host& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, host& field)
{
    field.value(tokenise(istr, include_all));
    return istr;
}

field_factory::registrar<_HTTP_HOST, host> host_registrar;

/* ======================================================================
 * Location
 * ====================================================================== */

const char _HTTP_LOCATION[] = "Location";

std::ostream&
operator<<(std::ostream& ostr, const location& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, location& field)
{
    field.value(tokenise(istr, include_all));
    return istr;
}

field_factory::registrar<_HTTP_LOCATION, location> location_registrar;

/* ======================================================================
 * Transfer-Encoding
 * ====================================================================== */

const char _HTTP_TRANSFER_ENCODING[] = "Transfer-Encoding";

const std::string Chunked = "chunked";
const std::string Compress = "compress";
const std::string Deflate = "deflate";
const std::string Gzip = "gzip";
const std::string X_Gzip = "x-gzip";

std::ostream&
operator<<(std::ostream& ostr, const transfer_coding_e& value)
{
    switch (value) {
        case transfer_coding_e::CHUNKED:
            ostr << Chunked;
            break;
        case transfer_coding_e::COMPRESS:
            ostr << Compress;
            break;
        case transfer_coding_e::DEFLATE:
            ostr << Deflate;
            break;
        case transfer_coding_e::GZIP:
            ostr << Gzip;
            break;
        default:
            break;
    }
    return ostr;
}

std::istream&
operator>>(std::istream& istr, transfer_coding_e& value)
{
    std::string tok = tokenise(istr, include_none);
    if (equal_case(tok, Chunked)) {
        value = transfer_coding_e::CHUNKED;
    } else if (equal_case(tok, Compress)) {
        value = transfer_coding_e::COMPRESS;
    } else if (equal_case(tok, Deflate)) {
        value = transfer_coding_e::DEFLATE;
    } else if (equal_case(tok, Gzip)) {
        value = transfer_coding_e::GZIP;
    } else if (equal_case(tok, X_Gzip)) {
        value = transfer_coding_e::GZIP;
    } else {
        throw std::out_of_range("Incorrectly formatted HTTP Transfer-Encoding");
    }
    return istr;
}

field_factory::registrar<_HTTP_TRANSFER_ENCODING, transfer_encoding>
    transfer_encoding_registrar;

/* ======================================================================
 * Field Factory
 * ====================================================================== */

bool
field_factory::less_case::operator()(const char* a, const char* b) const
{
    while (*a != 0 && std::tolower(*a) == std::tolower(*b)) {
        ++a, ++b;
    }
    return std::tolower(*a) < std::tolower(*b);
}

field_factory&
field_factory::instance()
{
    static field_factory _instance;
    return _instance;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
