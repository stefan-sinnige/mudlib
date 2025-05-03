#include "mud/http/chunk.h"
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

FEATURE("HTTP/1.1 Message")

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
  DEFINE_THEN ("The version is HTTP/1.1",
      [](context& ctx) {
          ASSERT(mud::http::version_e::HTTP11, 
                 ctx.msg->version().value());
      })
  DEFINE_THEN ("The Host field is 127.0.0.1",
      [](context& ctx) {
          ASSERT("127.0.0.1",
                 ctx.msg->field<mud::http::host>().value());
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
                "GET /index.html HTTP/1.1\r\n"
                "Host: 127.0.0.1\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The method is GET")
     AND ("The URI is /index.html")
     AND ("The version is HTTP/1.1")
     AND ("The Host field is 127.0.0.1")

  SCENARIO("Writing Minimal Request")
    GIVEN("An HTTP Message")
    WHEN ("A minimal HTTP/1.1 request is written",
        [](context& ctx) {
            ctx.req.version(mud::http::version_e::HTTP11);
            ctx.req.method(mud::http::method_e::GET);
            ctx.req.uri("/index.html");
            ctx.req.field<mud::http::host>("127.0.0.1");
            ctx.ostr << ctx.req;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "GET /index.html HTTP/1.1\r\n"
                       "Host: 127.0.0.1\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading Minimal Response")
    GIVEN("A minimal successful HTTP response message",
        [](context& ctx) {
            ctx.msg = &ctx.resp;
            ctx.istr = std::istringstream(
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 0\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The version is HTTP/1.1")
     AND  ("The Status Code is 200")
     AND  ("The Reason Phrase is OK")

  SCENARIO("Writing Minimal Response")
    GIVEN("An HTTP Message")
    WHEN ("A minimal HTTP/1.1 Response is written",
        [](context& ctx) {
            ctx.resp.version(mud::http::version_e::HTTP11);
            ctx.resp.status_code(mud::http::status_code_e::OK);
            ctx.resp.reason_phrase(mud::http::reason_phrase_e::OK);
            ctx.resp.field<mud::http::content_length>(0);
            ctx.ostr << ctx.resp;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                       "HTTP/1.1 200 OK\r\n"
                       "Content-Length: 0\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading Entity-Body with Content-Length")
    GIVEN("An HTTP Message with a Content-Length and Entity-Body field",
        [](context& ctx) {
            ctx.msg = &ctx.req;
            ctx.istr = std::istringstream(
                "GET /index.html HTTP/1.1\r\n"
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
                "HTTP/1.1 200 OK\r\n"
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
            ctx.req.version(mud::http::version_e::HTTP11);
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
                       "GET /index.html HTTP/1.1\r\n"
                       "Content-Length: 11\r\n"
                       "\r\n"
                       "Hello World"),
                   result);
        })

  SCENARIO("Reading a chunked Entity-Body")
    GIVEN("An HTTP Message with chunked Transfer-Encoding",
        [](context& ctx) {
            ctx.msg = &ctx.resp;
            ctx.istr = std::istringstream(
                "HTTP/1.1 200 OK\r\n"
                "Transfer-Encoding: chunked\r\n"
                "\r\n"
                "5\r\nHello\r\n"
                "8\r\n World. \r\n"
                "23\r\nIt has been good\r\nto see you again.\r\n"
                "0\r\n\r\n");
        })
    WHEN ("The message is read")
    THEN ("The Entity-Body value is available",
        [](context& ctx) {
            ASSERT("Hello World. It has been good\r\nto see you again.", ctx.msg->entity_body().value());
        })

  SCENARIO("Writing a chunked Entity-Body")
    GIVEN("An HTTP Message")
    WHEN ("A chunked Entity Body is written",
        [](context& ctx) {
            ctx.resp.version(mud::http::version_e::HTTP11);
            ctx.resp.status_code(mud::http::status_code_e::OK);
            ctx.resp.reason_phrase(mud::http::reason_phrase_e::OK);
            ctx.resp.field<mud::http::transfer_encoding>(
                mud::http::transfer_coding_e::CHUNKED);
            ctx.ostr << ctx.resp
                << mud::http::chunk("Hello")
                << mud::http::chunk(" World. ")
                << mud::http::chunk("It has been good\r\nto see you again.")
                << mud::http::chunk();
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(std::string(
                        "HTTP/1.1 200 OK\r\n"
                        "Transfer-Encoding: chunked\r\n"
                        "\r\n"
                        "5\r\nHello\r\n"
                        "8\r\n World. \r\n"
                        "23\r\nIt has been good\r\nto see you again.\r\n"
                        "0\r\n\r\n"),
                    result);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
