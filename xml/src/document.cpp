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

#include "mud/xml/document.h"
#include "mud/xml/exception.h"
#include <algorithm>
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ document::ptr
document::create()
{
    struct make_shared_enabler: public xml::document
    {
        make_shared_enabler()
            : xml::document()
        {}  
    };
    return std::make_shared<make_shared_enabler>();
}

document::document() : node(node::type_t::DOCUMENT)
{}

mud::xml::element::ptr
document::root() const
{
    auto found = std::find_if(_children.cbegin(), _children.cend(),
        [](const std::shared_ptr<mud::xml::node>& node) {
            return node->type() == node::type_t::ELEMENT;
        });
    if (found != _children.cend()) {
        return std::static_pointer_cast<mud::xml::element>(*found);
    } else {
        throw not_found("document has no root element");
    }
}

const mud::xml::node_seq&
document::children() const
{
    return _children;
}

void
document::children(const mud::xml::node_seq& value)
{
    _children = value;
}

void
document::children(mud::xml::node_seq&& value)
{
    _children = std::move(value);
}

void
document::child(const mud::xml::node::ptr& node)
{
    _children.push_back(node);
}

void
document::resolve()
{
    std::vector<mud::xml::ns::ptr> ns_ctx;
    resolve(root(), ns_ctx);
}

void
document::resolve(
        const mud::xml::element::ptr& element,
        const std::vector<mud::xml::ns::ptr>& parent_ctx)
{
    // Create the context for this node. This contains all the resolved XML
    // namespaces at this level. This is a union of the parent context with
    // the namespace attributes of the element.
    std::vector<mud::xml::ns::ptr> ctx = parent_ctx;
    for (auto& attr: element->attributes()) {
        if (attr->ns()->resolved()) {
            // If already defined by a parent, remove it as this one will
            // replace that definition.
            auto found = std::find_if(ctx.begin(), ctx.end(), [&](auto& ns) {
                return ns->uri() == attr->ns()->uri();
            });
            if (found != ctx.end()) {
                ctx.erase(found);
            }

            // Add the resolved namespace
            ctx.push_back(attr->ns());
        }
    }

    // Resolve any attribute
    for (auto& attr: element->attributes()) {
        // Namespace definition attributes are skipped
        if (attr->ns()->resolved()) {
            continue;
        }

        // Only resolve if there is no URI set.
        if (attr->ns()->uri().empty()) {
            auto found = std::find_if(ctx.begin(), ctx.end(), [&](auto& ns) {
                return ns->prefix() == attr->ns()->prefix();
            });
            if (found != ctx.end()) {
                attr->ns()->uri((*found)->uri());
            }
        }
    }

    // Resolve the element itself if there is no URI set.
    if (element->ns()->uri().empty()) {
        auto found = std::find_if(ctx.begin(), ctx.end(), [&](auto& ns) {
            return ns->prefix() == element->ns()->prefix();
        });
        if (found != ctx.end()) {
            element->ns()->uri((*found)->uri());
        }
    }

    // Resolve any all its children.
    for (auto& child: element->children()) {
        if (child->type() == node::type_t::ELEMENT) {
            resolve(std::static_pointer_cast<mud::xml::element>(child), ctx);
        }
    }
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
