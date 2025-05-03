#include "mud/soap/message.h"
#include "mud/test.h"
#include <sstream>
#include <string>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* The text stream */
    std::stringstream text;

    /* A SOAP message. */
    mud::soap::message msg;
END_CONTEXT()

FEATURE("Writer")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_WHEN("The message is written",
      [](context& ctx){
            ctx.text << ctx.msg;
      });
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Writing an empty message")
    GIVEN("An empty SOAP  message", [](context&){})
    WHEN ("The message is written")
    THEN ("The text represents the SOAP message contents",
      [](context& ctx) {
        ASSERT(
          "<?xml version=\"1.0\"?>"
          "<env:Envelope "
              "env:encodingStyle=\"https://www.w3.org/2003/05/soap-encoding\" "
              "xmlns:env=\"http://www.w3.org/2003/05/soap-envelope\">"
            "<env:Header/>"
            "<env:Body/>"
          "</env:Envelope>",
          ctx.text.str());
      })

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
