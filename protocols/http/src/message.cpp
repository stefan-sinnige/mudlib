#include "mud/http/message.h"
#include "mud/http/chunk.h"
#include "tokenise.h"
#include <stdexcept>

BEGIN_MUDLIB_HTTP_NS

message::message(enum message::type type)
  : _type(type), _version(version_e::HTTP11)
{
}

void
message::clear()
{
    _fields.clear();
}

bool
message::valid() const
{
    return true;
}

enum message::type
message::type() const
{
    return _type;
}

const base_field&
message::field_by_key(const std::string& key) const
{
    auto iter = find(key.c_str());
    if (iter == _fields.end()) {
        throw std::out_of_range("field not found");
    }
    return *iter;
}

bool
message::exists(const std::string& key) const
{
    return find(key.c_str()) != _fields.end();
}

size_t
message::field_size() const
{
    return _fields.size();
}

std::ostream&
operator<<(std::ostream& ostr, const message& msg)
{
    if (!msg.valid()) {
        throw std::runtime_error("Invalid HTTP message");
    }

    // The request or response line
    if (msg.type() == message::type::REQUEST) {
        const auto& req = dynamic_cast<const request&>(msg);
        ostr << req.method() << SP << req.uri() << SP << req.version() << CR
             << LF;
    } else if (msg.type() == message::type::RESPONSE) {
        const auto& resp = dynamic_cast<const response&>(msg);
        ostr << resp.version() << SP << resp.status_code() << SP
             << resp.reason_phrase() << CR << LF;
    }

    // Any header. At this stage there is no distinction made between a
    // request or response specific header - all are exported.
    for (const auto& field : msg.fields()) {
        ostr << field.key() << CL << SP;
        field.value(ostr);
        ostr << CR << LF;
    }

    // Entity separator
    ostr << CR << LF;

    // Entity body
    ostr << msg.entity_body();
    return ostr;
}

std::istream&
operator>>(std::istream& istr, message& msg)
{
    // Tokenisation manipulators
    static const token_manip include_none = { 0, 0 };
    static const token_manip include_space = { 1, 0 };
    static const token_manip include_colon = { 0, 1 };
    static const token_manip include_all = { 1, 1 };

    // Ensure to start with a fresh message
    msg.clear();

    // Request or response line
    if (msg.type() == message::type::REQUEST) {
        // Request-Line = Method SP Request-URI SP HTTP-Version CRLF
        auto& req = dynamic_cast<request&>(msg);
        istr >> req.method();
        expect(istr, SP);
        istr >> req.uri();
        expect(istr, SP);
        istr >> req.version();
        expect(istr, CR);
        expect(istr, LF);
    } else {
        // Status-Line = HTTP-Version SP Status-Code SP Reason-Phrase CRLF
        auto& resp = dynamic_cast<response&>(msg);
        istr >> resp.version();
        expect(istr, SP);
        istr >> resp.status_code();
        expect(istr, SP);
        istr >> resp.reason_phrase();
        expect(istr, CR);
        expect(istr, LF);
    }

    // Any header. At this stage there is no distinction made between a
    // request or response specific header - all are accepted.
    while (istr.peek() != CR) {
        std::string field_name = tokenise(istr, include_none);
        expect(istr, CL);
        expect(istr, SP);
        auto field =
            field_factory::instance().create(field_name.c_str(), msg.fields());
        if (field) {
            // Known field inserted into the message's fields map.
            (*field).value(istr);
        } else {
            // Unknown field - add as a generic extension field.
            field_ext ext(field_name);
            ext.value(istr);
            base_field::field id =
                (base_field::field)(((int)ext.type()) - msg.fields().size());
            msg.fields().push_back(ext);
        }
        expect(istr, CR);
        expect(istr, LF);
    }

    // The entity separator
    expect(istr, CR);
    expect(istr, LF);

    // The entity body can be read in the following ways:
    //    * Transfer-Encoding
    //      * If chunked, read till the last chunks
    //        * This needs to be changed for streaming data where each chunk
    //          woulld need to trigger a read event callback.
    //      * Other encoding are yet unsupported
    //    * Content-Length
    //      * Read till the designated length
    //    * No Content-Length or Transfer-Encoding
    //      * If the message is a request, assume it does not contain a body
    //      * Read till connection is closed (HTTP 1.0 only)
    if (msg.exists<http::transfer_encoding>()) {
        // Only support chunked encoding
        const auto& encodings = msg.field<http::transfer_encoding>().value();
        if (std::find_if(encodings.begin(), encodings.end(),
                         [](const auto& enc) {
                             return enc == transfer_coding_e::CHUNKED;
                         }) == encodings.end()) {
            throw std::out_of_range("Only chunked Transfer-Encoding supported");
        }

        // Read all the chunks and append them to the body
        std::string body;
        while (true) {
            chunk chunk_data;
            istr >> chunk_data;
            if (chunk_data.size() == 0) {
                break;
            }
            body.append(chunk_data.data());
        }
        msg.entity_body(body);
    } else if (msg.exists<http::content_length>()) {
        // Read the exact size as indicated by the content-length.
        int content_length = msg.field<http::content_length>().value();
        std::string body;
        body.resize(content_length);
        istr.read(&body[0], content_length);
        msg.entity_body(body);
    } else if (msg.type() == message::type::RESPONSE &&
               msg.version() == version_e::HTTP10) {
        // Read until the end, but only for a Response Message (RFC 9110 - 8.6)
        // and read it in blocks for best performance.
        const size_t BLOCKSIZE = 1024;
        static char buf[BLOCKSIZE];
        std::string body;
        do {
            istr.read(buf, BLOCKSIZE);
            body.append(buf, istr.gcount());
        } while (!istr.eof());
        msg.entity_body(body);
    } else if (msg.type() == message::type::REQUEST) {
        // Assume empty request body
    } else {
        // Some cases allow no Transfer-Encoding / Content-Length.
        throw std::out_of_range("Unable to determine HTTP Entity-Body length");
    }
    return istr;
}

END_MUDLIB_HTTP_NS

/* vi: set ai ts=4 expandtab: */
