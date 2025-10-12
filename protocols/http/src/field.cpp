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

static std::map<method_e, std::string_view> g_method_map =
{
    { method_e::GET, "GET" },
    { method_e::HEAD, "HEAD" },
    { method_e::POST, "POST" },
    { method_e::PUT, "PUT" },
    { method_e::DELETE, "DELETE" },
    { method_e::CONNECT, "CONNECT" },
    { method_e::OPTIONS, "OPTIONS" },
    { method_e::TRACE, "TRACE" }
};

http_method::http_method()
    : _type(method_e::UNKNOWN)
{
}

http_method::http_method(method_e type)
    : _type(type)
{
}

http_method::http_method(const std::string& ext)
{
    // Find the @c ext string in the map
    auto found = std::find_if(
            g_method_map.begin(), g_method_map.end(),
            [ext](const std::map<method_e,
                  std::string_view>::value_type& item) -> bool
            {
                return item.second == ext;
            });

    // If found, use the predefined type, otherwise use the extension
    if (found != g_method_map.end()) {
        _type = found->first;
    }
    else {
        _type = method_e::EXT;
        _ext = ext;
    }
}

method_e
http_method::type() const
{
    return _type;
}

http_method::operator method_e() const
{
    return _type;
}

std::string_view
http_method::str() const
{
    if (_type == method_e::EXT) {
        return _ext;
    }
    else {
        return g_method_map[_type];
    }
}

std::ostream&
operator<<(std::ostream& ostr, const http_method& value)
{
    ostr << value.str();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, http_method& value)
{
    std::string tok = tokenise(istr, include_none);
    value = http_method(tok);
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
    http_method value;
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

static std::map<reason_phrase_e, std::string_view> g_reason_phrase_map =
{
    { reason_phrase_e::Unknown, "Unknown" },
    { reason_phrase_e::Continue, "Continue" },
    { reason_phrase_e::SwitchingProtocols, "Switching Protocols" },
    { reason_phrase_e::OK, "OK" },
    { reason_phrase_e::Created, "Created" },
    { reason_phrase_e::Accepted, "Accepted" },
    { reason_phrase_e::NoContent, "No Content" },
    { reason_phrase_e::ResetContent, "Reset Content" },
    { reason_phrase_e::PartialContent, "Partial Content" },
    { reason_phrase_e::MultipleChoice, "Multiple Choice" },
    { reason_phrase_e::MovedPermanently, "Moved Permanently" },
    { reason_phrase_e::MovedTemporarily, "Moved Temporarily" },
    { reason_phrase_e::NotModified, "Not Modified" },
    { reason_phrase_e::UseProxy, "Use Proxy" },
    { reason_phrase_e::TemporaryRedirect, "Temporary Redirect" },
    { reason_phrase_e::BadRequest, "Bad Request" },
    { reason_phrase_e::Unauthorized, "Unauthorized" },
    { reason_phrase_e::Forbidden, "Forbidden" },
    { reason_phrase_e::NotFound, "Not Found" },
    { reason_phrase_e::InternalServerError, "Internal Server Error" },
    { reason_phrase_e::NotAcceptable, "Not Acceptable" },
    { reason_phrase_e::ProxyAuthenticationRequired, "Proxy Authentication Required" },
    { reason_phrase_e::RequestTimeout, "Request Time-out" },
    { reason_phrase_e::Conflict, "Conflict" },
    { reason_phrase_e::Gone, "Gone" },
    { reason_phrase_e::LengthRequired, "Length Required" },
    { reason_phrase_e::PreconditionFailed, "Precondition Failed" },
    { reason_phrase_e::RequestEntityTooLarge, "Request Entity Too Large" },
    { reason_phrase_e::RequestURITooLarge, "Request-URI Too Large" },
    { reason_phrase_e::UnsupportedMediaType, "Unsupported Media Type" },
    { reason_phrase_e::RequestRangeNotSatifiable, "Request range not satisfiable" },
    { reason_phrase_e::ExpectationFailed, "Expectation Failed" },
    { reason_phrase_e::NotImplemented, "Not Implemented" },
    { reason_phrase_e::BadGateway, "Bad Gateway" },
    { reason_phrase_e::ServiceUnavailable, "Service Unavailable" },
    { reason_phrase_e::GatewayTimeout, "Gateway Time-out" },
    { reason_phrase_e::HTTPVersionNotSupported, "HTTP Version not supported" }
};

http_reason_phrase::http_reason_phrase()
    : _type(reason_phrase_e::Unknown)
{
}

http_reason_phrase::http_reason_phrase(reason_phrase_e type)
    : _type(type)
{
}

http_reason_phrase::http_reason_phrase(const std::string& ext)
{
    // Find the @c ext string in the map
    auto found = std::find_if(
            g_reason_phrase_map.begin(), g_reason_phrase_map.end(),
            [ext](const std::map<reason_phrase_e,
                  std::string_view>::value_type& item) -> bool
            {
                return item.second == ext;
            });

    // If found, use the predefined type, otherwise use the extension
    if (found != g_reason_phrase_map.end()) {
        _type = found->first;
    }
    else {
        _type = reason_phrase_e::ExtensionCode;
        _ext = ext;
    }
}

reason_phrase_e
http_reason_phrase::type() const
{
    return _type;
}

http_reason_phrase::operator reason_phrase_e() const
{
    return _type;
}

std::string_view
http_reason_phrase::str() const
{
    if (_type == reason_phrase_e::ExtensionCode) {
        return _ext;
    }
    else {
        return g_reason_phrase_map[_type];
    }
}

std::ostream&
operator<<(std::ostream& ostr, const http_reason_phrase& value)
{
    ostr << value.str();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, http_reason_phrase& value)
{
    std::string tok = tokenise(istr, include_none);
    value = http_reason_phrase(tok);
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const reason_phrase& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, reason_phrase& field)
{
    std::string tok = tokenise(istr, include_all);
    field = reason_phrase(tok);
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
 * Content-Type
 * ====================================================================== */

const char _HTTP_CONTENT_TYPE[] = "Content-Type";

static std::map<content_type_e, std::string_view> g_content_type_map =
{
    { content_type_e::Unknown, "unknown/unknown" },
    { content_type_e::ApplicationForm, "application/x-www-form-urlencoded" },
    { content_type_e::ApplicationJson, "application/json" },
    { content_type_e::ApplicationOctetStream, "application/octet-stream" },
    { content_type_e::ApplicationPdf, "application/pdf" },
    { content_type_e::ApplicationPgpEncrypted, "application/pgp-encrypted" },
    { content_type_e::ApplicationPgpSignature, "application/pgp-signature" },
    { content_type_e::ApplicationPkcs7Mime, "application/pkcs7-mime" },
    { content_type_e::ApplicationPkcs7Signature, "application/pkcs7-signature" },
    { content_type_e::ApplicationXml, "application/xml" },
    { content_type_e::ApplicationZip, "application/zip" },
    { content_type_e::AudioMpeg, "audio/mpeg" },
    { content_type_e::AudioFlac, "audio/flac" },
    { content_type_e::AudioWav, "audio/wav" },
    { content_type_e::ImageJpeg, "image/jpeg" },
    { content_type_e::ImageGif, "image/gif" },
    { content_type_e::ImagePng, "image/png" },
    { content_type_e::ImageSvg, "image/svg" },
    { content_type_e::MultipartAlternative, "multipart/alternative" }, 
    { content_type_e::MultipartEncrypted,  "multipart/encrypted" },
    { content_type_e::MultipartForm_data,  "multipart/form-data" },
    { content_type_e::MultipartMixed,  "multipart/mixed" },
    { content_type_e::MultipartRelated,  "multipart/related" },
    { content_type_e::MultipartSigned,  "multipart/signed" },
    { content_type_e::TextHtml, "text/html" },
    { content_type_e::TextPlain, "text/plain" },
    { content_type_e::TextCss, "text/css" },
    { content_type_e::TextJavascript, "text/javascript" },
    { content_type_e::VideoMp4, "video/mp4" },
    { content_type_e::VideoMpeg, "video/mpeg" }
};

http_content_type::http_content_type()
    : _type(content_type_e::Unknown)
{
}

http_content_type::http_content_type(content_type_e type)
    : _type(type)
{
}

http_content_type::http_content_type(const std::string& ext)
{
    // Find the @c ext string in the map
    auto found = std::find_if(
            g_content_type_map.begin(), g_content_type_map.end(),
            [ext](const std::map<content_type_e,
                  std::string_view>::value_type& item) -> bool
            {
                return item.second == ext;
            });

    // If found, use the predefined type, otherwise use the extension
    if (found != g_content_type_map.end()) {
        _type = found->first;
    }
    else {
        _type = content_type_e::ExtensionCode;
        _ext = ext;
    }
}

content_type_e
http_content_type::type() const
{
    return _type;
}

http_content_type::operator content_type_e() const
{
    return _type;
}

std::string_view
http_content_type::str() const
{
    if (_type == content_type_e::ExtensionCode) {
        return _ext;
    }
    else {
        return g_content_type_map[_type];
    }
}

std::ostream&
operator<<(std::ostream& ostr, const http_content_type& value)
{
    ostr << value.str();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, http_content_type& value)
{
    std::string tok = tokenise(istr, include_none);
    value = http_content_type(tok);
    return istr;
}

std::ostream&
operator<<(std::ostream& ostr, const content_type& field)
{
    ostr << field.value();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, content_type& field)
{
    http_content_type value;
    istr >> value;
    field = content_type(value);
    return istr;
}

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
