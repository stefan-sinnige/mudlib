#include "mud/http/field.h"
#include "tokenise.h"
#include <ctime>
#include <stdexcept>

BEGIN_MUDLIB_HTTP_NS

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

std::ostream&
operator<<(std::ostream& ostr, const method& field)
{
    switch (field.value()) {
        case method_e::GET:
            ostr << GET;
            break;
        case method_e::HEAD:
            ostr << HEAD;
            break;
        case method_e::POST:
            ostr << POST;
            break;
        default:
            break;
    }
    return ostr;
}

std::istream&
operator>>(std::istream& istr, method& field)
{
    std::string tok = tokenise(istr, include_none);
    if (tok == GET) {
        field.value(method_e::GET);
    } else if (tok == HEAD) {
        field.value(method_e::HEAD);
    } else if (tok == POST) {
        field.value( method_e::POST);
    } else {
        throw std::out_of_range("Incorrectly formatted HTTP Method");
    }
    return istr;
}

/* ======================================================================
 * URI
 * ====================================================================== */

const char _HTTP_URI[] = "__URI";

std::ostream&
operator<<(std::ostream& ostr, const uri& field)
{
    ostr << field.value();
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

const std::string OK = "OK";
const std::string Created = "Created";
const std::string Accepted = "Accepted";
const std::string NoContent = "No Content";
const std::string MovedPermanently = "Moved Permanently";
const std::string MovedTemporarily = "Moved Temporarily";
const std::string NotModified = "Not Modified";
const std::string BadRequest = "Bad Request";
const std::string Unauthorized = "Unauthorized";
const std::string Forbidden = "Forbidden";
const std::string NotFound = "Not Found";
const std::string InternalServerError = "Internal Server Error";
const std::string NotImplemented = "Not Implemented";
const std::string BadGateway = "Bad Gateway";
const std::string ServiceUnavailable = "Service Unavailable";

std::ostream&
operator<<(std::ostream& ostr, const reason_phrase& field)
{
    switch (field.value()) {
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
        case reason_phrase_e::MovedPermanently:
            ostr << MovedPermanently;
            break;
        case reason_phrase_e::MovedTemporarily:
            ostr << MovedTemporarily;
            break;
        case reason_phrase_e::NotModified:
            ostr << NotModified;
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
        case reason_phrase_e::NotImplemented:
            ostr << NotImplemented;
            break;
        case reason_phrase_e::BadGateway:
            ostr << BadGateway;
            break;
        case reason_phrase_e::ServiceUnavailable:
            ostr << ServiceUnavailable;
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
    if (tok == OK) {
        field.value(reason_phrase_e::OK);
    } else if (tok == Created) {
        field.value(reason_phrase_e::Created);
    } else if (tok == Accepted) {
        field.value(reason_phrase_e::Accepted);
    } else if (tok == NoContent) {
        field.value(reason_phrase_e::NoContent);
    } else if (tok == MovedPermanently) {
        field.value(reason_phrase_e::MovedPermanently);
    } else if (tok == MovedTemporarily) {
        field.value(reason_phrase_e::MovedTemporarily);
    } else if (tok == NotModified) {
        field.value(reason_phrase_e::NotModified);
    } else if (tok == BadRequest) {
        field.value(reason_phrase_e::BadRequest);
    } else if (tok == Unauthorized) {
        field.value(reason_phrase_e::Unauthorized);
    } else if (tok == Forbidden) {
        field.value(reason_phrase_e::Forbidden);
    } else if (tok == NotFound) {
        field.value(reason_phrase_e::NotFound);
    } else if (tok == InternalServerError) {
        field.value(reason_phrase_e::InternalServerError);
    } else if (tok == NotImplemented) {
        field.value(reason_phrase_e::NotImplemented);
    } else if (tok == BadGateway) {
        field.value(reason_phrase_e::BadGateway);
    } else if (tok == ServiceUnavailable) {
        field.value(reason_phrase_e::ServiceUnavailable);
    } else {
        throw std::out_of_range("Incorrectly formatted HTTP Version");
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

std::ostream&
operator<<(std::ostream& ostr, const allow& field)
{
    switch (field.value()) {
        case method_e::GET:
            ostr << GET;
            break;
        case method_e::HEAD:
            ostr << HEAD;
            break;
        case method_e::POST:
            ostr << POST;
            break;
        default:
            break;
    }
    return ostr;
}

std::istream&
operator>>(std::istream& istr, allow& field)
{
    std::string tok = tokenise(istr, include_none);
    if (tok == GET) {
        field.value(method_e::GET);
    } else if (tok == HEAD) {
        field.value(method_e::HEAD);
    } else if (tok == POST) {
        field.value( method_e::POST);
    } else {
        throw std::out_of_range("Incorrectly formatted HTTP Allow field");
    }
    return istr;
}

field_factory::registrar<_HTTP_ALLOW, allow> allow_registrar;

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
        throw std::out_of_range("Incorrectly formatted HTTP Content-Length field");
    }
    field.value(value);
    return istr;
}

field_factory::registrar<_HTTP_CONTENT_LENGTH, content_length> content_length_registrar;

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
 * Field Factory
 * ====================================================================== */

field_factory&
field_factory::instance()
{
    static field_factory _instance;
    return _instance;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
