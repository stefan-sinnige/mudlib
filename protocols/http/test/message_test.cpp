#include "mud/http/message.h"
#include "mud/http/request.h"
#include "mud/http/response.h"
#include "mud/test.h"
#include <sstream>
#include <string>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Input string stream */
    std::istringstream istr;

    /* Output string stream */
    std::ostringstream ostr;

    /* The HTTP request message */
    mud::http::request req;

    /* The HTTP response message */
    mud::http::response resp;

    /* The reference to the message (either request or response) */
    mud::http::message* msg;
END_CONTEXT()

FEATURE("HTTP/1.0 Message")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An HTTP Message", [](context&){ });
  DEFINE_WHEN ("The message is read",
      [](context& ctx) {
          ctx.istr >> *ctx.msg;
      });
  DEFINE_THEN ("The type is a request message",
      [](context& ctx) {
          ASSERT(mud::http::message::type::REQUEST, 
                 ctx.msg->type());
      })
  DEFINE_THEN ("The type is a response message",
      [](context& ctx) {
          ASSERT(mud::http::message::type::RESPONSE, 
                 ctx.msg->type());
      })
  DEFINE_THEN  ("The method is GET",
      [](context& ctx) {
          ASSERT(mud::http::method_e::GET,
                 ctx.req.method().value());
      });
  DEFINE_THEN  ("The URI is http://www.example.com/index.html",
      [](context& ctx) {
          ASSERT("http://www.example.com/index.html",
                 ctx.req.uri().value());
      });
  DEFINE_THEN ("The version is HTTP/1.0",
      [](context& ctx) {
          ASSERT(mud::http::version_e::HTTP10, 
                 ctx.msg->version().value());
      })
  DEFINE_THEN ("The Status Code is 200",
      [](context& ctx) {
          ASSERT(mud::http::status_code_e::OK,
                 ctx.resp.status_code().value());
      })
  DEFINE_THEN ("The Reason Phrase is OK",
      [](context& ctx) {
          ASSERT(mud::http::reason_phrase_e::OK,
                 ctx.resp.reason_phrase().value());
      });
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Type traits")
    GIVEN("An HTTP Message type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::http::message>::value);
        })
    THEN ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::http::message>::value);
        })
    THEN ("The type is assignable",
        [](context& ctx) {
            ASSERT(true, std::is_assignable<
                  mud::http::message,
                  mud::http::message>::value);
        })

  SCENARIO("Default request message")
    GIVEN("A default HTTP request message",
        [](context& ctx){
            ctx.msg = &ctx.req;
        })
    WHEN ("A message field is queried", [](context&){})
    THEN ("The type is a request message")
    AND  ("An out-of-range exception is thrown for a version field",
        [](context& ctx) {
            ASSERT_THROW(std::out_of_range,
                   ctx.req.field<mud::http::version>());
        })

  SCENARIO("Default response message")
    GIVEN("A default HTTP response message",
        [](context& ctx){
            ctx.msg = &ctx.resp;
        })
    WHEN ("A message field is queried", [](context&){})
    THEN ("The type is a response message")
    AND  ("An out-of-range exception is thrown for a version field",
        [](context& ctx) {
            ASSERT_THROW(std::out_of_range,
                   ctx.req.field<mud::http::version>());
        })

  SCENARIO("Reading Minimal Request")
    GIVEN("A minimal HTTP request message",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The method is GET")
     AND ("The URI is http://www.example.com/index.html")
     AND ("The version is HTTP/1.0")

  SCENARIO("Writing Minimal Request")
    GIVEN("An HTTP Message")
    WHEN ("A minimal HTTP/1.0 request is written",
        [](context& ctx) {
            ctx.req.version(mud::http::version_e::HTTP10);
            ctx.req.method(mud::http::method_e::GET);
            ctx.req.uri(mud::http::uri("http://www.example.com/index.html"));
            ctx.ostr << ctx.req;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "GET http://www.example.com/index.html HTTP/1.0\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading Minimal Response")
    GIVEN("A minimal successful HTTP response message",
        [](context& ctx) {
            ctx.msg = &ctx.resp;
            ctx.istr = std::istringstream(
                "HTTP/1.0 200 OK\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The version is HTTP/1.0")
     AND  ("The Status Code is 200")
     AND  ("The Reason Phrase is OK")

  SCENARIO("Writing Minimal Response")
    GIVEN("An HTTP Message")
    WHEN ("A minimal HTTP/1.0 Response is written",
        [](context& ctx) {
            ctx.resp.version(mud::http::version_e::HTTP10);
            ctx.resp.status_code(mud::http::status_code_e::OK);
            ctx.resp.reason_phrase(mud::http::reason_phrase_e::OK);
            ctx.ostr << ctx.resp;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "HTTP/1.0 200 OK\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading Date field")
    GIVEN("An HTTP Message with a Date field",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "Date: Mon, 03 Jan 2022 09:07:50 GMT\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The Date value is available",
        [](context& ctx) {
            mud::http::date::value_type value = ctx.req.field<mud::http::date>();
            time_t tm = std::chrono::system_clock::to_time_t(value);
            ASSERT(1641200870, tm);
        })

  SCENARIO("Reading incorrectly formatted Date field")
    GIVEN("An HTTP Message with an incorrectly formatted Date field",
        [](context& ctx) {
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "Date: Xyz, 98 Abc 2022 99:77:63 AET\r\n"
                "\r\n");
        })
    WHEN ("The message is read", [](context&ctx){})
    THEN ("An out-of-range exception is thrown for a version field",
        [](context& ctx) {
            ASSERT_THROW(std::out_of_range,
                   ctx.istr >> ctx.req);
        })

  SCENARIO("Writing Date field")
    GIVEN("An HTTP Message")
    WHEN ("A Date field is written",
        [](context& ctx) {
            ctx.req.method(mud::http::method_e::GET);
            ctx.req.uri("http://www.example.com/index.html");
            ctx.req.version(mud::http::version_e::HTTP10);
            ctx.req.field<mud::http::date>(
                    std::chrono::system_clock::from_time_t(1641200870));
            ctx.ostr << ctx.req;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "GET http://www.example.com/index.html HTTP/1.0\r\n"
                       "Date: Mon, 03 Jan 2022 09:07:50 GMT\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading extension header field")
    GIVEN("An HTTP Message with an extension header field",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "My-Custom-Field: Hello world\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The extension header field is available",
        [](context& ctx) {
            const mud::http::field_ext& fld =
                dynamic_cast<const mud::http::field_ext&>(
                    ctx.req.field_by_key("My-Custom-Field"));
            ASSERT("Hello world", fld.value());
        })

  SCENARIO("Writing extension header field")
    GIVEN("An HTTP Message")
    WHEN ("An extension header field is written",
        [](context& ctx) {
            ctx.req.method(mud::http::method_e::GET);
            ctx.req.uri("http://www.example.com/index.html");
            ctx.req.version(mud::http::version_e::HTTP10);
            mud::http::field_ext ext("My-Custom-Field");
            ext.value("Hello World");
            ctx.req.field(ext);
            ctx.ostr << ctx.req;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "GET http://www.example.com/index.html HTTP/1.0\r\n"
                       "My-Custom-Field: Hello World\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading standard and extension header fields")
    GIVEN("An HTTP Message with standard and extension fields",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "Pragma: custom=value\r\n"
                "Authorization: Basic YWxhZGRpbjpvcGVuc2VzYW1l\r\n"
                "From: any-user@example.com\r\n"
                "If-Modified-Since: Mon, 03 Jan 2022 09:07:50 GMT\r\n"
                "Referer: http://www.google.com\r\n"
                "User-Agent: Version/15.2 Safari/605.1.15\r\n"
                "Location: http://www.example.com/index.html\r\n"
                "Server: CERN/3.0 libwww/2.17\r\n"
                "WWW-Authenticate: Basic realm=Access, charset=UTF-8\r\n"
                "Allow: GET\r\n"
                "Content-Encoding: gzip\r\n"
                "Content-Type: text/html; charset=UTF-8\r\n"
                "Expires: Mon, 03 Jan 2022 09:08:50 GMT\r\n"
                "Last-Modified: Tue, 16 Nov 2022 15:29:14 GMT \r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The number of fields indicates all fields",
        [](context& ctx) {
            ASSERT(14, ctx.req.field_size());
        })

  SCENARIO("Reading Entity-Body with Content-Length")
    GIVEN("An HTTP Message with a Content-Length and Entity-Body field",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "Content-Length: 11\r\n"
                "\r\n"
                "Hello World");
        })
    WHEN ("The message is read")
    THEN ("The Entity-Body value is available",
        [](context& ctx) {
            ASSERT("Hello World", ctx.req.entity_body().value());
        })

  SCENARIO("Reading Entity-Body without Content-Length")
    GIVEN("An HTTP Message with a Content-Length and Entity-Body field",
        [](context& ctx) {
            ctx.msg = &ctx.resp;
            ctx.istr = std::istringstream(
                "HTTP/1.0 200 OK\r\n"
                "\r\n"
                "Hello World");
        })
    WHEN ("The message is read")
    THEN ("The Entity-Body value is available",
        [](context& ctx) {
            ASSERT(11, ctx.resp.entity_body().value().size());
            ASSERT("Hello World", ctx.resp.entity_body().value());
        })

  SCENARIO("Writing Entity-Body field")
    GIVEN("An HTTP Message")
    WHEN ("An Entity-Body is written",
        [](context& ctx) {
            ctx.req.method(mud::http::method_e::GET);
            ctx.req.uri("http://www.example.com/index.html");
            ctx.req.version(mud::http::version_e::HTTP10);
            ctx.req.field<mud::http::content_length>(
                    11);
            ctx.req.entity_body("Hello World");
            ctx.ostr << ctx.req;
        })
    THEN ("The Content-Length is available",
        [](context& ctx) {
            ASSERT(11, ctx.req.field<mud::http::content_length>());
        })
    AND  ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "GET http://www.example.com/index.html HTTP/1.0\r\n"
                       "Content-Length: 11\r\n"
                       "\r\n"
                       "Hello World"),
                   result);
        })

  SCENARIO("Reading multi-value header fields")
    GIVEN("An HTTP Message with a multi-value header field",
        [](context& ctx) {
            ctx.msg = &ctx.resp;
            ctx.istr = std::istringstream(
                "HTTP/1.0 200 OK\r\n"
                "Allow: GET\r\n"
                "Allow: HEAD, POST\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The duplicate fields are available",
        [](context& ctx) {
            ASSERT(1, ctx.msg->fields().size());
            auto values = ctx.msg->field<mud::http::allow>().value();
            auto iter = values.begin();
            ASSERT(true, iter != values.end());
            ASSERT(mud::http::method_e::GET, *iter);
            ++iter;
            ASSERT(true, iter != values.end());
            ASSERT(mud::http::method_e::HEAD, *iter);
            ++iter;
            ASSERT(true, iter != values.end());
            ASSERT(mud::http::method_e::POST, *iter);
            ++iter;
            ASSERT(true, iter == values.end());
        })

  SCENARIO("Writing multi-value header fields")
    GIVEN("An HTTP Message")
    WHEN ("Duplicate header fields are written",
        [](context& ctx) {
            ctx.resp.version(mud::http::version_e::HTTP10);
            ctx.resp.status_code(mud::http::status_code_e::OK);
            ctx.resp.reason_phrase(mud::http::reason_phrase_e::OK);
            std::list<mud::http::method_e> allowed = {
                mud::http::method_e::GET,
                mud::http::method_e::HEAD
            };
            ctx.resp.field<mud::http::allow>(allowed);
            ctx.ostr << ctx.resp;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "HTTP/1.0 200 OK\r\n"
                       "Allow: GET, HEAD\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading header field with non-standard camel casing")
    GIVEN("An HTTP Message with a non-standard camel cased field",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "cOnTeNt-lEnGtH: 0\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The field value is available",
        [](context& ctx) {
            int value = ctx.req.field<mud::http::content_length>();
            ASSERT(0, value);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
