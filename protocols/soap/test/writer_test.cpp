/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

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
