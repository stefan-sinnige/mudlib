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

#include "mud/protocols/communicator.h"
#include "mud/test.h"
#include "mock_communicators.h"
#include "mock_device.h"
#include <sstream>
#include <string>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context()
        : device(istr, ostr),
          end_comms(nullptr),
          a_comms(nullptr),
          b_comms(nullptr)
    {
    }

    /* Destructor after each scenario */
    ~context() {
        delete end_comms;
        delete a_comms;
        delete b_comms;
    }

    /* Input device string stream */
    std::stringstream istr;

    /* Output device string stream */
    std::stringstream ostr;

    /* Mock device */
    mock::device device;

    /* Mock end communicator */
    mock::end_communicator* end_comms;

    /* Mock layer A communicator */
    mock::layer_a_communicator* a_comms;

    /* Mock layer B communicator */
    mock::layer_b_communicator* b_comms;
END_CONTEXT()

FEATURE("Communication Protocol Layering")

  /*
   * The predefined Gherkin steps.
   */

  DEFINE_GIVEN("A single layered protocol",
      [](context& ctx){
          ctx.end_comms = new mock::end_communicator();
          ctx.a_comms = new mock::layer_a_communicator(*ctx.end_comms);
          ctx.end_comms->open(std::move(ctx.device));
      });
  DEFINE_GIVEN("A multi layered protocol",
      [](context& ctx){
          ctx.end_comms = new mock::end_communicator();
          ctx.a_comms = new mock::layer_a_communicator(*ctx.end_comms);
          ctx.b_comms = new mock::layer_b_communicator(*ctx.a_comms);
          ctx.end_comms->open(std::move(ctx.device));
      });
  DEFINE_WHEN("A message for layer A is sent",
      [](context& ctx){
        ctx.istr << "A_REQUEST" << std::endl;
        ctx.end_comms->device().simulate_signal();
      });
  DEFINE_WHEN("A message for layer B is sent",
      [](context& ctx){
        ctx.istr << "B_REQUEST" << std::endl;
        ctx.end_comms->device().simulate_signal();
      });
  DEFINE_WHEN("An enveloped message for layer B is sent",
      [](context& ctx){
        ctx.istr << "A_ENVELOPE" << std::endl
                 << "B_REQUEST" << std::endl;
        ctx.end_comms->device().simulate_signal();
      });
  DEFINE_THEN("A reply from layer A is received",
      [](context& ctx){
        std::string reply;
        std::getline(ctx.ostr, reply);
        ASSERT("A_REPLY", reply);
      });
  DEFINE_THEN("A reply from layer B is received",
      [](context& ctx){
        std::string reply;
        std::getline(ctx.ostr, reply);
        ASSERT("B_REPLY", reply);
      });
  DEFINE_THEN("A reply from layer A is not received",
      [](context& ctx){
      })
  DEFINE_THEN("A reply from layer B is not received",
      [](context& ctx){
      })
  END_DEFINES()

  /*
   * The scenarios.
   */

  SCENARIO("Single layer communication")
    GIVEN("A single layered protocol")
     WHEN("A message for layer A is sent")
     THEN("A reply from layer A is received")

  SCENARIO("Multi layer communication to upper layer")
    GIVEN("A multi layered protocol")
     WHEN("An enveloped message for layer B is sent")
     THEN("A reply from layer A is not received")
      AND("A reply from layer B is received")

  SCENARIO("Multi layer communication to middle layer")
    GIVEN("A multi layered protocol")
     WHEN("A message for layer A is sent")
     THEN("A reply from layer A is received")
      AND("A reply from layer B is not received")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
