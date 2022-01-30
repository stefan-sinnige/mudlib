#include "mud/http/message.h"
#include <chrono>
#include <ctime>
#include <stdexcept>

BEGIN_MUDLIB_HTTP_NS

message::message() : _type(message::type_t::UNDETERMINED) {}

message::message(const message& rhs)
{
    (void)operator=(rhs);
}

message&
message::operator=(const message& rhs)
{
    if (&rhs != this) {
        _type = rhs._type;
        _fields = rhs._fields;
    }
    return *this;
}

void
message::clear()
{
    _type = message::type_t::UNDETERMINED;
    _fields.clear();
}

bool
message::valid() const
{
    if (_type == message::type_t::UNDETERMINED)
        return false;
    return true;
}

message::type_t
message::type() const
{
    return _type;
}

void
message::type(message::type_t value)
{
    _type = value;
}

size_t
message::field_size() const
{
    return _fields.size();
}

/* ======================================================================
 * Version
 * ====================================================================== */

const message::field_t version::field = message::field_t::VERSION;

version&
version::operator=(const version& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

/* ======================================================================
 * Method
 * ====================================================================== */

const message::field_t method::field = message::field_t::METHOD;

const std::string method::GET = "GET";
const std::string method::HEAD = "HEAD";
const std::string method::POST = "POST";

method&
method::operator=(const method& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

/* ======================================================================
 * URI
 * ====================================================================== */

const message::field_t uri::field = message::field_t::URI;

uri&
uri::operator=(const uri& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

/* ======================================================================
 * Date
 * ====================================================================== */

const message::field_t date::field = message::field_t::DATE;

date&
date::operator=(const date& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

/* ======================================================================
 * Status Code
 * ====================================================================== */

const message::field_t status_code::field = message::field_t::STATUS_CODE;

status_code&
status_code::operator=(const status_code& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

/* ======================================================================
 * Reason Phrase
 * ====================================================================== */

const message::field_t reason_phrase::field = message::field_t::REASON_PHRASE;

const std::string reason_phrase::OK = "OK";
const std::string reason_phrase::Created = "Created";
const std::string reason_phrase::Accepted = "Accepted";
const std::string reason_phrase::NoContent = "No Content";
const std::string reason_phrase::MovedPermanently = "Moved Permanently";
const std::string reason_phrase::MovedTemporarily = "Moved Temporarily";
const std::string reason_phrase::NotModified = "Not Modified";
const std::string reason_phrase::BadRequest = "Bad Request";
const std::string reason_phrase::Unauthorized = "Unauthorized";
const std::string reason_phrase::Forbidden = "Forbidden";
const std::string reason_phrase::NotFound = "Not Found";
const std::string reason_phrase::InternalServerError = "Internal Server Error";
const std::string reason_phrase::NotImplemented = "Not Implemented";
const std::string reason_phrase::BadGateway = "Bad Gateway";
const std::string reason_phrase::ServiceUnavailable = "Service Unavailable";

reason_phrase&
reason_phrase::operator=(const reason_phrase& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

/* ======================================================================
 * Content-Length
 * ====================================================================== */

const message::field_t content_length::field = message::field_t::CONTENT_LENGTH;

content_length&
content_length::operator=(const content_length& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

/* ======================================================================
 * Entity Body
 * ====================================================================== */

const message::field_t entity_body::field = message::field_t::ENTITY_BODY;

entity_body&
entity_body::operator=(const entity_body& rhs)
{
    if (&rhs != this) {
        _value = rhs._value;
    }
    return *this;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
