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

#include "mud/core/utf.h"
#include "mud/test.h"
#include <sstream>
#include <string>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() = default;

    /* Destructor after each scenario */
    ~context() = default;

    /* A Code point */
    mud::core::codepoint cp;

    /* A UTF-8 value */
    mud::core::utf8 utf_8;

    /* A UTF-16 value */
    mud::core::utf16 utf_16;

    /* A UTF-32 value */
    mud::core::utf32 utf_32;

    /* A string */
    std::string str;

    /* A string stream */
    std::stringstream sstr;
END_CONTEXT()

/* Sample UTF strings (binary). These are interchangeable and represent the
 * same phrase. */
const std::string utf8 =
        "\x10\x07\x20\x07\x1d\x07\x20\x00\x10\x07\x20\x07\x1d\x07\x20\x00"
        "\x20\x07\x21\x07\x22\x07\x10\x07\x20\x00\x2b\x07\x12\x07\x29\x07"
        "\x2c\x07\x22\x07\x1d\x07";
const std::string utf16le =
        "\x10\x07\x20\x07\x1d\x07\x20\x00\x10\x07\x20\x07\x1d\x07\x20\x00"
        "\x20\x07\x21\x07\x22\x07\x10\x07\x20\x00\x2b\x07\x12\x07\x29\x07"
        "\x2c\x07\x22\x07\x1d\x07";
const std::string utf16be =
        "\x07\x10\x07\x20\x07\x1d\x00\x20\x07\x10\x07\x20\x07\x1d\x00\x20"
        "\x07\x20\x07\x21\x07\x22\x07\x10\x00\x20\x07\x2b\x07\x12\x07\x29"
        "\x07\x2c\x07\x22\x07\x1d";
const std::string utf16bom =
        "\xfe\xff\x07\x10\x07\x20\x07\x1d\x00\x20\x07\x10\x07\x20\x07\x1d"
        "\x00\x20\x07\x20\x07\x21\x07\x22\x07\x10\x00\x20\x07\x2b\x07\x12"
        "\x07\x29\x07\x2c\x07\x22\x07\x1d";
const std::string utf32le =
        "\x10\x07\x00\x00\x20\x07\x00\x00\x1d\x07\x00\x00\x20\x00\x00\x00"
        "\x10\x07\x00\x00\x20\x07\x00\x00\x1d\x07\x00\x00\x20\x00\x00\x00"
        "\x20\x07\x00\x00\x21\x07\x00\x00\x22\x07\x00\x00\x10\x07\x00\x00"
        "\x20\x00\x00\x00\x2b\x07\x00\x00\x12\x07\x00\x00\x29\x07\x00\x00"
        "\x2c\x07\x00\x00\x22\x07\x00\x00\x1d\x07\x00\x00";
const std::string utf32be =
        "\x00\x00\x07\x10\x00\x00\x07\x20\x00\x00\x07\x1d\x00\x00\x00\x20"
        "\x00\x00\x07\x10\x00\x00\x07\x20\x00\x00\x07\x1d\x00\x00\x00\x20"
        "\x00\x00\x07\x20\x00\x00\x07\x21\x00\x00\x07\x22\x00\x00\x07\x10"
        "\x00\x00\x00\x20\x00\x00\x07\x2b\x00\x00\x07\x12\x00\x00\x07\x29"
        "\x00\x00\x07\x2c\x00\x00\x07\x22\x00\x00\x07\x1d";
const std::string utf32bom =
        "\x00\x00\xfe\xff\x00\x00\x07\x10\x00\x00\x07\x20\x00\x00\x07\x1d"
        "\x00\x00\x00\x20\x00\x00\x07\x10\x00\x00\x07\x20\x00\x00\x07\x1d"
        "\x00\x00\x00\x20\x00\x00\x07\x20\x00\x00\x07\x21\x00\x00\x07\x22"
        "\x00\x00\x07\x10\x00\x00\x00\x20\x00\x00\x07\x2b\x00\x00\x07\x12"
        "\x00\x00\x07\x29\x00\x00\x07\x2c\x00\x00\x07\x22\x00\x00\x07\x1d";

FEATURE("UTF")
    DEFINE_GIVEN("An empty codepoint", [](context& ctx){})
    DEFINE_GIVEN("An empty UTF-8 character", [](context& ctx){})
    DEFINE_GIVEN("An empty UTF-16 character", [](context& ctx){})
    DEFINE_GIVEN("An empty UTF-32 character", [](context& ctx){})
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("UTF codepoint traits")
    GIVEN("A codepoint type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::codepoint>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::codepoint>::value);
        })
    AND  ("The type is  copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::codepoint>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::codepoint>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::codepoint>::value);
        })

  SCENARIO("UTF-8 type traits")
    GIVEN("A utf-8 type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::utf8>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::utf8>::value);
        })
    AND  ("The type is  copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::utf8>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::utf8>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::utf8>::value);
        })

  SCENARIO("UTF-16 type traits")
    GIVEN("A utf-16 type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::utf16>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::utf16>::value);
        })
    AND  ("The type is  copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::utf16>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::utf16>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::utf16>::value);
        })

  SCENARIO("UTF-32 type traits")
    GIVEN("A utf-32 type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::utf32>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::utf32>::value);
        })
    AND  ("The type is  copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::utf32>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::utf32>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::utf32>::value);
        })

  SCENARIO("A codepoint can be created from an integer value")
    GIVEN("An empty codepoint")
    WHEN ("The codepoint is assigned a value", [](context& ctx) {
            ctx.cp = ctx.sample<uint32_t>("value");
        })
    THEN ("The codepoint value is set correctly", [](context& ctx) {
            ASSERT(ctx.sample<mud::core::codepoint>("codepoint"), ctx.cp);
        })
    SAMPLES(uint32_t, mud::core::codepoint)
        HEADINGS("value", "codepoint")
        SAMPLE(   0x0000,   0x0000 )  // Smallest BMP - plane 0
        SAMPLE(   0xFFFF,   0xFFFF )  // Largest  BMP - plane 0
        SAMPLE(  0x10000,  0x10000 )  // Smallest Plane 1
        SAMPLE(  0x1FFFF,  0x1FFFF )  // Largest  Plane 1
        SAMPLE( 0x100000, 0x100000 )  // Smallest Plane 16
        SAMPLE( 0x10FFFF, 0x10FFFF )  // Largest  Plane 16
        SAMPLE( 0x110000,   0x0000 )  // Out of range defaults to NULL
    END_SAMPLES()

  SCENARIO("A codepoint can be parsed from a string")
    GIVEN("An empty codepoint")
    WHEN ("The codepoint is assigned a string value", [](context& ctx) {
            ctx.cp = ctx.sample<std::string>("value");
        })
    THEN ("The codepoint value is set correctly", [](context& ctx) {
            ASSERT(ctx.sample<mud::core::codepoint>("codepoint"), ctx.cp);
        })
    SAMPLES(std::string, mud::core::codepoint)
        HEADINGS("value", "codepoint")
        SAMPLE( "U+0000",     0x0000 )  // Smallest BMP - plane 0
        SAMPLE( "U+FFFF",     0xFFFF )  // Largest  BMP - plane 0
        SAMPLE( "U+10000",   0x10000 )  // Smallest Plane 1
        SAMPLE( "U+1FFFF",   0x1FFFF )  // Largest  Plane 1
        SAMPLE( "U+100000", 0x100000 )  // Smallest Plane 16
        SAMPLE( "U+10FFFF", 0x10FFFF )  // Largest  Plane 16
        SAMPLE( "U+110000",   0x0000 )  // Out of range defaults to NULL
        SAMPLE( "U+FFF",      0x0000 )  // Out of range defaults to NULL
        SAMPLE( "U+102G",     0x0000 )  // Non hexadecimal
        SAMPLE( "Sample",     0x0000 )  // Random string
        SAMPLE( "",           0x0000 )  // Empty string
    END_SAMPLES()

  SCENARIO("A codepoint can be written to an output stream")
    GIVEN("An code with a defined value", [](context& ctx) {
            ctx.cp = ctx.sample<mud::core::codepoint>("codepoint");
        })
    WHEN ("The codepoint is written to an output stream", [](context& ctx) {
            std::stringstream sstr;
            sstr << ctx.cp;
            ctx.str = sstr.str();
        })
    THEN ("The codepoint value is printed correctly", [](context& ctx) {
            ASSERT(ctx.sample<std::string>("string"), ctx.str);
        })
    SAMPLES(mud::core::codepoint, std::string)
        HEADINGS("codepoint", "string")
        SAMPLE(   0x0000,   "U+0000" )  // Smallest BMP - plane 0
        SAMPLE(   0xFFFF,   "U+FFFF" )  // Largest  BMP - plane 0
        SAMPLE(  0x10000,  "U+10000" )  // Smallest Plane 1
        SAMPLE(  0x1FFFF,  "U+1FFFF" )  // Largest  Plane 1
        SAMPLE( 0x100000, "U+100000" )  // Smallest Plane 16
        SAMPLE( 0x10FFFF, "U+10FFFF" )  // Largest  Plane 16
    END_SAMPLES()

  SCENARIO("A codepoint can be read from an input stream")
    GIVEN("An input stream containing a codepoint notation", [](context& ctx) {
            ctx.sstr = std::stringstream(ctx.sample<std::string>("string"));
        })
    WHEN ("The codepoint is read from the input stream", [](context& ctx) {
            ctx.sstr >> ctx.cp;
        })
    THEN ("The codepoint value matches the  input", [](context& ctx) {
            ASSERT(ctx.sample<mud::core::codepoint>("codepoint"), ctx.cp);
        })
    SAMPLES(std::string, mud::core::codepoint)
        HEADINGS("string", "codepoint")
        SAMPLE( "U+0000",     0x0000 )  // Smallest BMP - plane 0
        SAMPLE( "U+FFFF",     0xFFFF )  // Largest  BMP - plane 0
        SAMPLE( "U+10000",   0x10000 )  // Smallest Plane 1
        SAMPLE( "U+1FFFF",   0x1FFFF )  // Largest  Plane 1
        SAMPLE( "U+100000", 0x100000 )  // Smallest Plane 16
        SAMPLE( "U+10FFFF", 0x10FFFF )  // Largest  Plane 16
        SAMPLE( "U+11FFFF",  0x11FFF )  // Scale down if larger than 0x10FFFF
    END_SAMPLES()

  SCENARIO("A (multi-byte) UTF-8 character can be created from a code-point")
    GIVEN("An empty UTF-8 character")
    WHEN ("The character is assigned a value", [](context& ctx) {
            ctx.cp = ctx.sample<std::string>("codepoint");
            ctx.utf_8 = ctx.cp;
        })
    THEN ("The character value is set correctly", [](context& ctx) {
            ASSERT(ctx.sample<mud::core::utf8::data_type>("encoding"),
                   ctx.utf_8.data());
        })
    SAMPLES(std::string, mud::core::utf8::data_type)
        HEADINGS("codepoint", "encoding")
        SAMPLE(  "U+0000", { 0x00 } )                   // NULL
        SAMPLE(  "U+0041", { 0x41 } )                   // ASCII 'A'
        SAMPLE(  "U+00FC", { 0xC3, 0xBC } )             // Latin-1 suplement 'ü'
        SAMPLE(  "U+03C4", { 0xCF, 0x84 } )             // Greek and coptic 'τ'
        SAMPLE(  "U+222B", { 0xE2, 0x88, 0xAB } )       // Mathematical '∫'
        SAMPLE(  "U+AB99", { 0xEA, 0xAE, 0x99 } )       // Cherokee 'ꮙ'
        SAMPLE( "U+10698", { 0xF0, 0x90, 0x9A, 0x98 } ) // Linear A '𐚘'
        SAMPLE( "U+1FA02", { 0xF0, 0x9F, 0xA8, 0x82 } ) // Chess Rook '🨂''
        SAMPLE( "U+E0051", { 0xF3, 0xA0, 0x81, 0x91 } ) // Tag letter Q '''
    END_SAMPLES()

  SCENARIO("A (multi-word) UTF-16 character can be created from a code-point")
    GIVEN("An empty UTF-16 character")
    WHEN ("The character is assigned a value", [](context& ctx) {
            ctx.cp = ctx.sample<std::string>("codepoint");
            ctx.utf_16 = ctx.cp;
        })
    THEN ("The character value is set correctly", [](context& ctx) {
            ASSERT(ctx.sample<mud::core::utf16::data_type>("encoding"),
                   ctx.utf_16.data());
        })
    SAMPLES(std::string, mud::core::utf16::data_type)
        HEADINGS("codepoint", "encoding")
        SAMPLE(  "U+0000", { 0x0000 } )         // NULL
        SAMPLE(  "U+0041", { 0x0041 } )         // ASCII 'A'
        SAMPLE(  "U+00FC", { 0x00FC } )         // Latin-1 suplement 'ü'
        SAMPLE(  "U+03C4", { 0x03C4 } )         // Greek and coptic 'τ'
        SAMPLE(  "U+222B", { 0x222B } )         // Mathematical '∫'
        SAMPLE(  "U+AB99", { 0xAB99 } )         // Cherokee 'ꮙ'
        SAMPLE( "U+10698", { 0xD801, 0xDE98 } ) // Linear A '𐚘'
        SAMPLE( "U+1FA02", { 0xD83E, 0xDE02 } ) // Chess Rook '🨂''
        SAMPLE( "U+E0051", { 0xDB40, 0xDC51 } ) // Tag letter Q '''
    END_SAMPLES()

  SCENARIO("A UTF-32 character can be created from a code-point")
    GIVEN("An empty UTF-32 character")
    WHEN ("The character is assigned a value", [](context& ctx) {
            ctx.cp = ctx.sample<std::string>("codepoint");
            ctx.utf_32 = ctx.cp;
        })
    THEN ("The character value is set correctly", [](context& ctx) {
            ASSERT(ctx.sample<mud::core::utf32::data_type>("encoding"),
                   ctx.utf_32.data());
        })
    SAMPLES(std::string, mud::core::utf32::data_type)
        HEADINGS("codepoint", "encoding")
        SAMPLE(  "U+0000", 0x00000000 ) // NULL
        SAMPLE(  "U+0041", 0x00000041 ) // ASCII 'A'
        SAMPLE(  "U+00FC", 0x000000FC ) // Latin-1 suplement 'ü'
        SAMPLE(  "U+03C4", 0x000003C4 ) // Greek and coptic 'τ'
        SAMPLE(  "U+222B", 0x0000222B ) // Mathematical '∫'
        SAMPLE(  "U+AB99", 0x0000AB99 ) // Cherokee 'ꮙ'
        SAMPLE( "U+10698", 0x00010698 ) // Linear A '𐚘'
        SAMPLE( "U+1FA02", 0x0001FA02 ) // Chess Rook '🨂''
        SAMPLE( "U+E0051", 0x000E0051 ) // Tag letter Q '''
    END_SAMPLES()

#if 0
  SCENARIO("A UTF-8 stream can be read to a string")
    GIVEN("A UTF-8 character stream", [](context&ctx){
            ctx.sstr = std::stringstream(utf8);
        })
    WHEN ("The stream is read as a UTF-8 string", [](context& ctx) {
            ctx.sstr >> ctx.utf_8_str;
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF-8 stream with a BOM can be read to a string")
    GIVEN("A UTF-8 character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF-8 string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF-8 string can be written to a stream")
    GIVEN("A UTF-8 string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF-8 string can be written to a stream with a BOM")
    GIVEN("A UTF-8 string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16BE stream can be read to a string")
    GIVEN("A UTF16BE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF16BE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16BE stream with a BOM can be read to a string")
    GIVEN("A UTF16BE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF16BE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16BE string can be written to a stream")
    GIVEN("A UTF16BE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16BE string can be written to a stream with a BOM")
    GIVEN("A UTF16BE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16LE stream can be read to a string")
    GIVEN("A UTF16LE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF16LE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16LE stream with a BOM can be read to a string")
    GIVEN("A UTF16LE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF16LE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16LE string can be written to a stream")
    GIVEN("A UTF16LE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF16LE string can be written to a stream with a BOM")
    GIVEN("A UTF16LE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32BE stream can be read to a string")
    GIVEN("A UTF32BE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF32BE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32BE stream with a BOM can be read to a string")
    GIVEN("A UTF32BE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF32BE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32BE string can be written to a stream")
    GIVEN("A UTF32BE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32BE string can be written to a stream with a BOM")
    GIVEN("A UTF32BE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32LE stream can be read to a string")
    GIVEN("A UTF32LE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF32LE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32LE stream with a BOM can be read to a string")
    GIVEN("A UTF32LE character stream", [](context&ctx){
        })
    WHEN ("The stream is read as a UTF32LE string", [](context& ctx) {
        })
    THEN ("The string contains the contents of the stream", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32LE string can be written to a stream")
    GIVEN("A UTF32LE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })

  SCENARIO("A UTF32LE string can be written to a stream with a BOM")
    GIVEN("A UTF32LE string", [](context&ctx){
        })
    WHEN ("The stream is written to a stream", [](context& ctx) {
        })
    THEN ("The stream contains the contents of the string", [](context& ctx) {
            ASSERT(true, false);
        })
#endif 

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
