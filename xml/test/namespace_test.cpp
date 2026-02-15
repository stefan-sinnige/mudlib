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

#include "mud/test.h"
#include "mud/xml/dom.h"
#include <memory>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Construct the XML document, aware of the namespaces and posititions
     * to use. */
    void create_doc();

    /* Verify the XML document, aware of the namespaces and posititions
     * to use. */
    void verify_doc();

    /* The XML text stream */
    std::stringstream text;

    /* The document */
    mud::xml::document::ptr doc;

    /* The library namespace to use */
    mud::xml::ns::ptr lib_ns;

    /* The id namespace to use */
    mud::xml::ns::ptr id_ns;
END_CONTEXT()

void
context::create_doc()
{
    // The document
    doc = mud::xml::dom::create_document();
    auto decl = mud::xml::dom::create_declaration();

    // The root catalog element. Add the namespace attributes if they are
    // defined.
    auto catalog = lib_ns ? mud::xml::dom::create_element("catalog",
                                                          lib_ns->uri())
                          : mud::xml::dom::create_element("catalog");
    if (lib_ns) {
        std::string name = "xmlns";
        if (!lib_ns->prefix().empty()) {
            name += ":" + lib_ns->prefix();
        }
        auto ns = mud::xml::dom::create_attribute(name);
        ns->value(lib_ns->uri());
        catalog->attribute(ns);
    }
    if (id_ns) {
        std::string name = "xmlns";
        if (!id_ns->prefix().empty()) {
            name += ":" + id_ns->prefix();
        }
        auto ns = mud::xml::dom::create_attribute(name);
        ns->value(id_ns->uri());
        catalog->attribute(ns);
    }

    // Add the book structure
    auto book = lib_ns ? mud::xml::dom::create_element("book",
                                                       lib_ns->uri())
                       : mud::xml::dom::create_element("book");
    auto isbn = id_ns ? mud::xml::dom::create_attribute("isbn",
                                                        id_ns->uri())
                      : mud::xml::dom::create_attribute("isbn");
    isbn->value("978-0-14-027536-0");
    book->attribute(isbn);

    // Add the author
    auto author = lib_ns ? mud::xml::dom::create_element("author",
                                                         lib_ns->uri())
                         : mud::xml::dom::create_element("author");
    auto author_text = mud::xml::dom::create_char_data("Homer");
    author->child(author_text);

    // Add the title
    auto title = lib_ns ? mud::xml::dom::create_element("title",
                                                        lib_ns->uri())
                        : mud::xml::dom::create_element("title");
    auto title_text = mud::xml::dom::create_char_data("Illiad");
    title->child(title_text);

    // Link them up
    book->child(author);
    book->child(title);
    catalog->child(book);
    doc->child(decl);
    doc->child(catalog);
}

void
context::verify_doc()
{
    // The root catalog element
    auto catalog = doc->root();
    {
        ASSERT("catalog", catalog->local_name());
        if (lib_ns) {
            ASSERT(lib_ns->prefix(), catalog->ns()->prefix());
            ASSERT(lib_ns->uri(), catalog->ns()->uri());
            bool found_ns = false;
            for (auto attr: catalog->attributes()) {
                if (attr->ns()->resolved() &&
                   (attr->ns()->uri() == catalog->ns()->uri()))
                {
                    found_ns = true;
                    break;
                }
            }
            ASSERT(true, found_ns);
        }
        else {
            ASSERT("", catalog->ns()->prefix());
            ASSERT(true, catalog->ns()->uri().empty());
        }
    }

    // The book element
    ASSERT(1, catalog->children().size());
    auto book = std::dynamic_pointer_cast<mud::xml::element>(
        *catalog->children().begin());
    {
        ASSERT("book", book->local_name());
        if (lib_ns) {
            ASSERT(lib_ns->prefix(), book->ns()->prefix());
            ASSERT(lib_ns->uri(), book->ns()->uri());
        }
        else {
            ASSERT("", book->ns()->prefix());
            ASSERT(true, book->ns()->uri().empty());
        }

        auto found = std::find_if(book->attributes().begin(),
                book->attributes().end(), [](auto& attr) {
            return attr->local_name() == "isbn";
        });
        ASSERT(true, (found != book->attributes().end()));
        auto isbn = *found;
        ASSERT("978-0-14-027536-0", isbn->value());
        if (id_ns) {
            ASSERT(id_ns->prefix(), isbn->ns()->prefix());
            ASSERT(id_ns->uri(), isbn->ns()->uri());
        }
        else {
            if (lib_ns && lib_ns->prefix().empty()) {
                ASSERT(lib_ns->prefix(), isbn->ns()->prefix());
                ASSERT(lib_ns->uri(), isbn->ns()->uri());
            }
            else {
                ASSERT("", isbn->ns()->prefix());
                ASSERT(true, isbn->ns()->uri().empty());
            }
        }
    }

    // The author element
    ASSERT(2, book->children().size());
    auto author = std::dynamic_pointer_cast<mud::xml::element>(
            book->children()[0]);
    {
        ASSERT("author", author->local_name());
        if (lib_ns) {
            ASSERT(lib_ns->prefix(), author->ns()->prefix());
            ASSERT(lib_ns->uri(), author->ns()->uri());
        }
        else {
            ASSERT("", author->ns()->prefix());
            ASSERT(true, author->ns()->uri().empty());
        }

        ASSERT(1, author->children().size());
        auto text = std::dynamic_pointer_cast<mud::xml::char_data>(
                author->children()[0]);
        ASSERT("Homer", text->text());
    }

    // The title element
    ASSERT(2, book->children().size());
    auto title = std::dynamic_pointer_cast<mud::xml::element>(
            book->children()[1]);
    {
        ASSERT("title", title->local_name());
        if (lib_ns) {
            ASSERT(lib_ns->prefix(), title->ns()->prefix());
            ASSERT(lib_ns->uri(), title->ns()->uri());
        }
        else {
            ASSERT("", title->ns()->prefix());
            ASSERT(true, title->ns()->uri().empty());
        }

        ASSERT(1, title->children().size());
        auto text = std::dynamic_pointer_cast<mud::xml::char_data>(
                title->children()[0]);
        ASSERT("Illiad", text->text());
    }
}

FEATURE("Namespace")
    DEFINE_WHEN("The document is written",
        [](context& ctx) {
            ctx.text << ctx.doc;
        })
    DEFINE_WHEN("The text is read",
        [](context& ctx) {
            ctx.text >> ctx.doc;
        })
    DEFINE_THEN("The document represents the text",
        [](context& ctx) {
            ctx.verify_doc();
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Writing a document with no namespaces")
    GIVEN("A structured XML document",
        [](context& ctx) {
            ctx.create_doc();
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(
                "<?xml version=\"1.0\"?>"
                "<catalog>"
                  "<book isbn=\"978-0-14-027536-0\">"
                    "<author>Homer</author>"
                    "<title>Illiad</title>"
                  "</book>"
                "</catalog>",
                ctx.text.str());
        })

  SCENARIO("Writing a document with default namespace in root element")
    GIVEN("A structured XML document",
        [](context& ctx) {
            mud::core::uri("http://library.example.org/");
            ctx.lib_ns = mud::xml::ns::create();
            ctx.lib_ns->uri(mud::core::uri("http://library.example.org/"));
            ctx.create_doc();
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(
                "<?xml version=\"1.0\"?>"
                "<catalog xmlns=\"http://library.example.org/\">"
                  "<book isbn=\"978-0-14-027536-0\">"
                    "<author>Homer</author>"
                    "<title>Illiad</title>"
                  "</book>"
                "</catalog>",
                ctx.text.str());
        })

  SCENARIO("Writing a document with a defined namespace in root element")
    GIVEN("A structured XML document",
        [](context& ctx) {
            mud::core::uri("http://library.example.org/");
            ctx.lib_ns = mud::xml::ns::create();
            ctx.lib_ns->prefix("lib");
            ctx.lib_ns->uri(mud::core::uri("http://library.example.org/"));
            ctx.create_doc();
            ctx.doc = mud::xml::dom::create_document();
            ctx.create_doc();
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(
                "<?xml version=\"1.0\"?>"
                "<lib:catalog xmlns:lib=\"http://library.example.org/\">"
                  "<lib:book isbn=\"978-0-14-027536-0\">"
                    "<lib:author>Homer</lib:author>"
                    "<lib:title>Illiad</lib:title>"
                  "</lib:book>"
                "</lib:catalog>",
                ctx.text.str());
        })

  SCENARIO("Writing a document with default and defined namespaces in root element")
    GIVEN("A structured XML document",
        [](context& ctx) {
            mud::core::uri("http://library.example.org/");
            ctx.lib_ns = mud::xml::ns::create();
            ctx.lib_ns->uri(mud::core::uri("http://library.example.org/"));
            mud::core::uri("http://library.example.org/");
            ctx.id_ns = mud::xml::ns::create();
            ctx.id_ns->prefix("id");
            ctx.id_ns->uri(mud::core::uri("http://library.example.org/isbn"));
            ctx.create_doc();
        })
    WHEN ("The document is written")
    THEN ("The text represents the document contents",
        [](context& ctx) {
            ASSERT(
                "<?xml version=\"1.0\"?>"
                "<catalog xmlns:id=\"http://library.example.org/isbn\" "
                         "xmlns=\"http://library.example.org/\">"
                  "<book id:isbn=\"978-0-14-027536-0\">"
                    "<author>Homer</author>"
                    "<title>Illiad</title>"
                  "</book>"
                "</catalog>",
                ctx.text.str());
        })

  SCENARIO("Reading a document with no namespaces")
    GIVEN("A structured XML document",
        [](context& ctx) {
            ctx.text <<
                "<?xml version=\"1.0\"?>"
                "<catalog>"
                  "<book isbn=\"978-0-14-027536-0\">"
                    "<author>Homer</author>"
                    "<title>Illiad</title>"
                  "</book>"
                "</catalog>";
        })
    WHEN ("The text is read")
    THEN ("The document represents the text")

  SCENARIO("Reading a document with default namespace in root element")
    GIVEN("A structured XML document",
        [](context& ctx) {
            mud::core::uri("http://library.example.org/");
            ctx.lib_ns = mud::xml::ns::create();
            ctx.lib_ns->uri(mud::core::uri("http://library.example.org/"));
            ctx.text <<
                "<?xml version=\"1.0\"?>"
                "<catalog xmlns=\"http://library.example.org/\">"
                  "<book isbn=\"978-0-14-027536-0\">"
                    "<author>Homer</author>"
                    "<title>Illiad</title>"
                  "</book>"
                "</catalog>";
        })
    WHEN ("The text is read")
    THEN ("The document represents the text")

  SCENARIO("Reading a document with a defined namespace in root element")
    GIVEN("A structured XML document",
        [](context& ctx) {
            mud::core::uri("http://library.example.org/");
            ctx.lib_ns = mud::xml::ns::create();
            ctx.lib_ns->prefix("lib");
            ctx.lib_ns->uri(mud::core::uri("http://library.example.org/"));
            ctx.text <<
                "<?xml version=\"1.0\"?>"
                "<lib:catalog xmlns:lib=\"http://library.example.org/\">"
                  "<lib:book isbn=\"978-0-14-027536-0\">"
                    "<lib:author>Homer</lib:author>"
                    "<lib:title>Illiad</lib:title>"
                  "</lib:book>"
                "</lib:catalog>";
        })
    WHEN ("The text is read")
    THEN ("The document represents the text")

  SCENARIO("Reading a document with default and defined namespaces in root element")
    GIVEN("A structured XML document",
        [](context& ctx) {
            mud::core::uri("http://library.example.org/");
            ctx.lib_ns = mud::xml::ns::create();
            ctx.lib_ns->uri(mud::core::uri("http://library.example.org/"));
            mud::core::uri("http://library.example.org/");
            ctx.id_ns = mud::xml::ns::create();
            ctx.id_ns->prefix("id");
            ctx.id_ns->uri(mud::core::uri("http://library.example.org/isbn"));
            ctx.text <<
                "<?xml version=\"1.0\"?>"
                "<catalog xmlns:id=\"http://library.example.org/isbn\" "
                         "xmlns=\"http://library.example.org/\">"
                  "<book id:isbn=\"978-0-14-027536-0\">"
                    "<author>Homer</author>"
                    "<title>Illiad</title>"
                  "</book>"
                "</catalog>";
        })
    WHEN ("The text is read")
    THEN ("The document represents the text")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
