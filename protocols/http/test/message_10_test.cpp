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
  DEFINE_THEN  ("The URI is /index.html",
      [](context& ctx) {
          ASSERT("/index.html", ctx.req.uri().value().path());
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

  SCENARIO("Reading Minimal Request")
    GIVEN("A minimal HTTP request message",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET /index.html HTTP/1.0\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The method is GET")
     AND ("The URI is /index.html")
     AND ("The version is HTTP/1.0")

  SCENARIO("Writing Minimal Request")
    GIVEN("An HTTP Message")
    WHEN ("A minimal HTTP/1.0 request is written",
        [](context& ctx) {
            ctx.req.version(mud::http::version_e::HTTP10);
            ctx.req.method(mud::http::method_e::GET);
            ctx.req.uri("/index.html");
            ctx.ostr << ctx.req;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "GET /index.html HTTP/1.0\r\n"
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

  SCENARIO("Reading standard and extension header fields")
    GIVEN("An HTTP Message with standard and extension fields",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET /index.html HTTP/1.0\r\n"
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
                "GET /index.html HTTP/1.0\r\n"
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
    GIVEN("An HTTP Message with Entity body and without a Content-Length field",
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
            ctx.req.uri("/index.html");
            ctx.req.version(mud::http::version_e::HTTP10);
            ctx.req.field<mud::http::content_length>(11);
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
                       "GET /index.html HTTP/1.0\r\n"
                       "Content-Length: 11\r\n"
                       "\r\n"
                       "Hello World"),
                   result);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
