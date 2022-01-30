#include "mud/http/message.h"
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

    /* The HTTP message */
    mud::http::message msg;
END_CONTEXT()

FEATURE("HTTP/1.0 Message")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("An HTTP Message", [](context&){ });
  DEFINE_WHEN ("The message is read",
      [](context& ctx) {
          ctx.istr >> ctx.msg;
      });
  DEFINE_THEN ("The type is undetermined",
      [](context& ctx) {
          ASSERT(mud::http::message::type_t::UNDETERMINED, 
                 ctx.msg.type());
      })
  DEFINE_THEN  ("The method is GET",
      [](context& ctx) {
          ASSERT(mud::http::method::GET,
                 ctx.msg.field<mud::http::method>().value());
      });
  DEFINE_THEN  ("The URI is http://www.example.com/index.html",
      [](context& ctx) {
          ASSERT("http://www.example.com/index.html",
                 ctx.msg.field<mud::http::uri>().value());
      });
  DEFINE_THEN ("The version is HTTP/1.0",
      [](context& ctx) {
          ASSERT(mud::http::version::Version::HTTP10, 
                 ctx.msg.field<mud::http::version>().value());
      })
  DEFINE_THEN ("The Status Code is 200",
      [](context& ctx) {
          ASSERT(mud::http::status_code::StatusCode::OK,
                 ctx.msg.field<mud::http::status_code>().value());
      })
  DEFINE_THEN ("The Reason Phrase is OK",
      [](context& ctx) {
          ASSERT(mud::http::reason_phrase::OK,
                 ctx.msg.field<mud::http::reason_phrase>().value());
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

  SCENARIO("Default message is undetermined")
    GIVEN("A default HTTP message", [](context&){})
    WHEN ("A message field is queried", [](context&){})
    THEN ("The type is undetermined")
    AND  ("An out-of-range exception is thrown for a version field",
        [](context& ctx) {
            ASSERT_THROW(std::out_of_range,
                   ctx.msg.field<mud::http::version>());
        })

  SCENARIO("Reading Minimal Request")
    GIVEN("A minimal HTTP request message",
        [](context& ctx) {
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
            ctx.msg.type(mud::http::message::type_t::REQUEST);
            ctx.msg.field<mud::http::version>(
                    mud::http::version::Version::HTTP10);
            ctx.msg.field<mud::http::method>(
                    mud::http::method::GET);
            ctx.msg.field<mud::http::uri>(
                    "http://www.example.com/index.html");
            ctx.ostr << ctx.msg;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(50, result.size());
            ASSERT(std::string(
                       "GET http://www.example.com/index.html HTTP/1.0\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading Minimal Response")
    GIVEN("A minimal successful HTTP response message",
        [](context& ctx) {
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
            ctx.msg.type(mud::http::message::type_t::RESPONSE);
            ctx.msg.field<mud::http::version>(
                    mud::http::version::Version::HTTP10);
            ctx.msg.field<mud::http::status_code>(
                    mud::http::status_code::StatusCode::OK);
            ctx.msg.field<mud::http::reason_phrase>(
                    mud::http::reason_phrase::OK);
            ctx.ostr << ctx.msg;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(19, result.size());
            ASSERT(std::string(
                       "HTTP/1.0 200 OK\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading Date field")
    GIVEN("An HTTP Message with a Date field",
        [](context& ctx) {
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "Date: Mon, 03 Jan 2022 09:07:50 GMT\r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The Date value is available",
        [](context& ctx) {
            mud::http::date::type_t value = ctx.msg.field<mud::http::date>();
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
                   ctx.istr >> ctx.msg);
        })

  SCENARIO("Writing Date field")
    GIVEN("An HTTP Message")
    WHEN ("A Date field is written",
        [](context& ctx) {
            ctx.msg.type(mud::http::message::type_t::REQUEST);
            ctx.msg.field<mud::http::method>(
                    mud::http::method::GET);
            ctx.msg.field<mud::http::uri>(
                    "http://www.example.com/index.html");
            ctx.msg.field<mud::http::version>(
                    mud::http::version::Version::HTTP10);
            ctx.msg.field<mud::http::date>(
                    std::chrono::system_clock::from_time_t(1641200870));
            ctx.ostr << ctx.msg;
        })
    THEN ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(87, result.size());
            ASSERT(std::string(
                       "GET http://www.example.com/index.html HTTP/1.0\r\n"
                       "Date: Mon, 03 Jan 2022 09:07:50 GMT\r\n"
                       "\r\n"),
                   result);
        })

  SCENARIO("Reading fields that are ignored")
    GIVEN("An HTTP Message with fields that are ignored",
        [](context& ctx) {
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
                "Allow: GET, POST\r\n"
                "Content-Encoding: gzip\r\n"
                "Content-Type: text/html; charset=UTF-8\r\n"
                "Expires: Mon, 03 Jan 2022 09:08:50 GMT\r\n"
                "Last-Modified: Tue, 16 Nov 2022 15:29:14 GMT \r\n"
                "\r\n");
        })
    WHEN ("The message is read")
    THEN ("The number of fields indicates that these are ignored",
        [](context& ctx) {
            ASSERT(3, ctx.msg.field_size());
        })

  SCENARIO("Reading Entity-Body")
    GIVEN("An HTTP Message with a Content-Length and Entity-Body field",
        [](context& ctx) {
            ctx.istr = std::istringstream(
                "GET http://www.example.com/index.html HTTP/1.0\r\n"
                "Content-Length: 11\r\n"
                "\r\n"
                "Hello World");
        })
    WHEN ("The message is read")
    THEN ("The Entity-Body value is available",
        [](context& ctx) {
            ASSERT("Hello World",
                 ctx.msg.field<mud::http::entity_body>().value());
        })

  SCENARIO("Writing Entity-Body field")
    GIVEN("An HTTP Message")
    WHEN ("An Entity-Body is written",
        [](context& ctx) {
            ctx.msg.type(mud::http::message::type_t::REQUEST);
            ctx.msg.field<mud::http::method>(
                    mud::http::method::GET);
            ctx.msg.field<mud::http::uri>(
                    "http://www.example.com/index.html");
            ctx.msg.field<mud::http::version>(
                    mud::http::version::Version::HTTP10);
            ctx.msg.field<mud::http::content_length>(
                    11);
            ctx.msg.field<mud::http::entity_body>(
                    "Hello World");
            ctx.ostr << ctx.msg;
        })
    THEN ("The Content-Length is available",
        [](context& ctx) {
            ASSERT(11, ctx.msg.field<mud::http::content_length>());
        })
    AND  ("The message is formatted correctly",
        [](context& ctx) {
            std::string result = ctx.ostr.str();
            ASSERT(81, result.size());
            ASSERT(std::string(
                       "GET http://www.example.com/index.html HTTP/1.0\r\n"
                       "Content-Length: 11\r\n"
                       "\r\n"
                       "Hello World"),
                   result);
        })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
