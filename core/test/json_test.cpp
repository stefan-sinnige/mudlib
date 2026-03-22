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

#include "mud/core/json.h"
#include "mud/test.h"
#include <sstream>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() = default;

    /* Destructor after each scenario */
    ~context() = default;

    /* The json value */
    mud::core::json json;

    /* A string */
    std::string str;
END_CONTEXT()

FEATURE("JSON")
    DEFINE_WHEN("The json value is examined", [](context&){})
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("JSON type traits")
    GIVEN("A json type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::json>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::json>::value);
        })
    AND  ("The type is  copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::json>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::json>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::json>::value);
        })

  SCENARIO("JSON can hold a null value")
    GIVEN("A json that is assigned a null value",
        [](context& ctx){
            ctx.json = nullptr;
        })
    WHEN ("The json value is examined")
    THEN ("The value is null", 
        [](context& ctx){
            ASSERT(mud::core::json::type_t::null, ctx.json.type());
        })

  SCENARIO("JSON can hold a string value")
    GIVEN("A json that is assigned a string value",
        [](context& ctx){
            ctx.json = "Hello World";
        })
    WHEN ("The json value is examined")
    THEN ("The value is equal to the assigned string value", 
        [](context& ctx){
            ASSERT(mud::core::json::type_t::string, ctx.json.type());
            ASSERT(std::string("Hello World"), ctx.json.string());
        })

  SCENARIO("JSON can hold an integer value")
    GIVEN("A json that is assigned an integer value",
        [](context& ctx){
            ctx.json = 1001;
        })
    WHEN ("The json value is examined")
    THEN ("The value is equal to the assigned integer value", 
        [](context& ctx){
            ASSERT(mud::core::json::type_t::integer, ctx.json.type());
            ASSERT(1001, ctx.json.integer());
        })

  SCENARIO("JSON can hold a decimal value")
    GIVEN("A json that is assigned a decimal value",
        [](context& ctx){
            ctx.json = 3.14159265358979323846;
        })
    WHEN ("The json value is examined")
    THEN ("The value is equal to the assigned decimal value", 
        [](context& ctx){
            ASSERT(mud::core::json::type_t::decimal, ctx.json.type());
            ASSERT(3.14159265358979323846, ctx.json.decimal());
        })

  SCENARIO("JSON can hold a boolean value")
    GIVEN("A json that is assigned a boolean value",
        [](context& ctx){
            ctx.json = true;
        })
    WHEN ("The json value is examined")
    THEN ("The value is equal to the assigned boolean value", 
        [](context& ctx){
            ASSERT(mud::core::json::type_t::boolean, ctx.json.type());
            ASSERT(true, ctx.json.boolean());
        })

  SCENARIO("JSON can hold an array value")
    GIVEN("A json that is assigned an array value",
        [](context& ctx){
            std::vector<mud::core::json> arr;
            for (int i = 0; i < 10; ++i) {
                arr.push_back(i);
            }
            arr.push_back("Hello World");
            arr.push_back(false);
            ctx.json = arr;
        })
    WHEN ("The json value is examined")
    THEN ("The value is equal to the assigned array value", 
        [](context& ctx){
            ASSERT(mud::core::json::type_t::array, ctx.json.type());
            const auto& arr = ctx.json.array();
            for (int i = 0; i < 10; ++i) {
                ASSERT(i, arr[i].integer());
            }
            ASSERT(std::string("Hello World"), arr[10].string());
            ASSERT(false, arr[11].boolean());
        })

  SCENARIO("JSON can hold an object value")
    GIVEN("A json that is assigned an object value",
        [](context& ctx){
            std::map<std::string, mud::core::json> obj;
            obj["first"] = "Hello World";
            obj["second"] = 1001;
            obj["third"] = false;
            ctx.json = obj;
        })
    WHEN ("The json value is examined")
    THEN ("The value is equal to the assigned object value", 
        [](context& ctx){
            ASSERT(mud::core::json::type_t::object, ctx.json.type());
            const auto& obj = ctx.json.object();
            ASSERT(3, obj.size());
            ASSERT(std::string("Hello World"),
                   obj.find("first")->second.string());
            ASSERT(1001,
                   obj.find("second")->second.integer());
            ASSERT(false,
                   obj.find("third")->second.boolean());
        })

  SCENARIO("Various basic JSON texts can be read")
    GIVEN("A stream containing a JSON text",
        [](context& ctx) {
            ctx.str = ctx.sample<std::string>("text");
        })
    WHEN ("The text is parsed",
        [](context& ctx) {
            std::stringstream sstr(ctx.str);
            sstr >> ctx.json;
        })
    THEN ("The JSON value is correctly formed",
        [](context& ctx) {
            if (ctx.json.type() == mud::core::json::type_t::decimal) {
                /* Precision might be lost, only look at a number of decimals */
                std::stringstream expected;
                std::stringstream result;
                expected << std::setprecision(15)
                         << ctx.sample<mud::core::json>("value").decimal();
                result   << std::setprecision(15)
                         << ctx.json.decimal();
                ASSERT(expected.str(), result.str());

            }
            else {
                auto expected = ctx.sample<mud::core::json>("value");
                ASSERT(true, expected == ctx.json);
            }
         })
    SAMPLES(std::string, mud::core::json)
         HEADINGS("text", "value")
         SAMPLE("null",                   nullptr)
         SAMPLE("1001",                   1001)
         SAMPLE("\"Hello World\"",        "Hello World")
         SAMPLE("3.14159265358979323846", 3.14159265358979323846);
         SAMPLE("false",                  false);
    END_SAMPLES()

  SCENARIO("JSON integer value can be read at interoperable boundaries")
    GIVEN("A stream containing a JSON text",
        [](context& ctx) {
            ctx.str = ctx.sample<std::string>("text");
        })
    WHEN ("The text is parsed",
        [](context& ctx) {
            std::stringstream sstr(ctx.str);
            sstr >> ctx.json;
        })
    THEN ("The JSON value is correctly formed",
        [](context& ctx) {
            auto expected = ctx.sample<mud::core::json>("value");
            ASSERT(true, expected == ctx.json);
         })
    SAMPLES(std::string, mud::core::json)
         HEADINGS("text", "value")
         SAMPLE("-9007199254740993", -9007199254740993LL) // -(2^53)+1
         SAMPLE("9007199254740991",  9007199254740991LL)  // (2^53)-1
    END_SAMPLES()

  SCENARIO("An empty JSON array can be read")
    GIVEN("A stream containing an empty JSON array",
        [](context& ctx) {
            ctx.str = "[]";
        })
    WHEN ("The text is parsed",
        [](context& ctx) {
            std::stringstream sstr(ctx.str);
            sstr >> ctx.json;
        })
    THEN ("The JSON value is correctly formed",
        [](context& ctx) {
            mud::core::json::array_t arr = ctx.json.array();
            ASSERT(0, arr.size());
        })

  SCENARIO("A JSON array of different types can be read")
    GIVEN("A stream containing a JSON array",
        [](context& ctx) {
            ctx.str = "[1001,false,\"Hello World\",3.14159]";
        })
    WHEN ("The text is parsed",
        [](context& ctx) {
            std::stringstream sstr(ctx.str);
            sstr >> ctx.json;
        })
    THEN ("The JSON value is correctly formed",
        [](context& ctx) {
            mud::core::json::array_t arr = ctx.json.array();
            ASSERT(4, arr.size());
            ASSERT(true, arr[0] == 1001);
            ASSERT(true, arr[1] == false);
            ASSERT(true, arr[2] == "Hello World");
            /* Precision might be lost, only look at a number of decimals, so
             * we cannot truly rely on the following amongst platforms
             *    ASSERT(true, arr[3] == 3.14159);
             */
            std::stringstream expected;
            std::stringstream result;
            expected << std::setprecision(6) << 3.14159;
            result   << std::setprecision(6) << arr[3].decimal();
            ASSERT(expected.str(), result.str());
        })

  SCENARIO("An empty JSON object can be read")
    GIVEN("A stream containing an empty JSON object",
        [](context& ctx) {
            ctx.str = "{}";
        })
    WHEN ("The text is parsed",
        [](context& ctx) {
            std::stringstream sstr(ctx.str);
            sstr >> ctx.json;
        })
    THEN ("The JSON value is correctly formed",
        [](context& ctx) {
            mud::core::json::object_t obj = ctx.json.object();
            ASSERT(0, obj.size());
         })

  SCENARIO("A JSON object can be read")
    GIVEN("A stream containing a JSON object",
        [](context& ctx) {
            ctx.str = "{\"boolean\":false,"
                       "\"decimal\":3.14159,"
                       "\"integer\":1001,"
                       "\"string\":\"Hello World\"}";
        })
    WHEN ("The text is parsed",
        [](context& ctx) {
            std::stringstream sstr(ctx.str);
            sstr >> ctx.json;
        })
    THEN ("The JSON value is correctly formed",
        [](context& ctx) {
            mud::core::json::object_t obj = ctx.json.object();
            ASSERT(4, obj.size());
            ASSERT(true, obj["integer"] == 1001);
            ASSERT(true, obj["boolean"] == false);
            ASSERT(true, obj["string"] == "Hello World");
            /* Precision might be lost, only look at a number of decimals, so
             * we cannot truly rely on the following amongst platforms
             *    ASSERT(true, obj["decimal"] == 3.14159);
             */
            std::stringstream expected;
            std::stringstream result;
            expected << std::setprecision(6) << 3.14159;
            result   << std::setprecision(6) << obj["decimal"].decimal();
            ASSERT(expected.str(), result.str());
         })

  SCENARIO("Various basic JSON values can be written")
    GIVEN("A json value",
         [](context& ctx) {
            ctx.json = ctx.sample<mud::core::json>("value");
         })
    WHEN ("The text is written",
         [](context& ctx) {
            std::stringstream sstr;
            sstr << ctx.json;
            ctx.str = sstr.str();
         })
    THEN ("The text is correctly formed",
         [](context& ctx) {
            auto expected = ctx.sample<std::string>("text");
            ASSERT(expected, ctx.str);
         })
    SAMPLES(mud::core::json, std::string)
         HEADINGS("value", "text")
         SAMPLE(nullptr,                "null")
         SAMPLE(1001,                   "1001")
         SAMPLE("Hello World",          "\"Hello World\"")
         SAMPLE(3.14159265358979323846, "3.14159")
         SAMPLE(false,                  "false")
    END_SAMPLES()

  SCENARIO("JSON integer value can be written at interoperable boundaries")
    GIVEN("A json value",
         [](context& ctx) {
            ctx.json = ctx.sample<mud::core::json>("value");
         })
    WHEN ("The text is written",
         [](context& ctx) {
            std::stringstream sstr;
            sstr << ctx.json;
            ctx.str = sstr.str();
         })
    THEN ("The text is correctly formed",
         [](context& ctx) {
            auto expected = ctx.sample<std::string>("text");
            ASSERT(expected, ctx.str);
         })
    SAMPLES(mud::core::json, std::string)
         HEADINGS("value", "text")
         SAMPLE(-9007199254740993LL, "-9007199254740993") // -(2^53)+1
         SAMPLE(9007199254740991LL,  "9007199254740991")  // (2^53)-1
    END_SAMPLES()

  SCENARIO("A JSON array of different types can be written")
    GIVEN("A json value representing an array",
         [](context& ctx) {
            mud::core::json::array_t arr;
            arr.push_back(1001);
            arr.push_back(false);
            arr.push_back("Hello World");
            arr.push_back(3.14159);
            ctx.json = arr;
         })
    WHEN ("The text is written",
         [](context& ctx) {
            std::stringstream sstr;
            sstr << ctx.json;
            ctx.str = sstr.str();
         })
    THEN ("The test is correctly formed",
         [](context& ctx) {
            ASSERT("[1001,false,\"Hello World\",3.14159]", ctx.str);
         })

  SCENARIO("A JSON object can be written")
    GIVEN("A json value representing an object",
         [](context& ctx) {
            mud::core::json::object_t obj;
            obj["integer"] = 1001;
            obj["boolean"] = false;
            obj["string"] = "Hello World";
            obj["decimal"] = 3.14159;
            ctx.json = obj;
         })
    WHEN ("The text is written",
         [](context& ctx) {
            std::stringstream sstr;
            sstr << ctx.json;
            ctx.str = sstr.str();
         })
    THEN ("The test is correctly formed",
         [](context& ctx) {
            ASSERT("{\"boolean\":false,"
                   "\"decimal\":3.14159,"
                   "\"integer\":1001,"
                   "\"string\":\"Hello World\"}", ctx.str);
         })

  SCENARIO("Parsing string encodings")
    GIVEN("A stream containing a JSON text",
        [](context& ctx){
        })
    WHEN ("The text is parsed",
        [](context& ctx){
        })
    THEN ("The value is equal to the assigned object value", 
        [](context& ctx){
        })
    SAMPLES(std::string, mud::core::json)
        HEADINGS("text", "value")
    END_SAMPLES()

  SCENARIO("Writing string encodings")
    GIVEN("A json value",
        [](context& ctx){
        })
    WHEN ("The text is parsed",
        [](context& ctx){
        })
    THEN ("The JSON value is correctly formed",
        [](context& ctx){
        })
    SAMPLES(mud::core::json, std::string)
        HEADINGS("value", "text")
    END_SAMPLES()

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
