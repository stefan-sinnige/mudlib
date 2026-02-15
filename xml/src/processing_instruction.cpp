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

#include "mud/xml/processing_instruction.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ processing_instruction::ptr
processing_instruction::create(const std::string& target,
        const std::string& data)
{
    struct make_shared_enabler: public xml::processing_instruction
    {
        make_shared_enabler(const std::string& target, const std::string& data)
            : xml::processing_instruction(target, data)
        {}  
    };
    return std::make_shared<make_shared_enabler>(target, data);
}

processing_instruction::processing_instruction(const std::string& target,
        const std::string& data)
    : node(node::type_t::PI)
    , _target(target)
    , _data(data)
{}

const std::string&
processing_instruction::target() const
{
    return _target;
}

void
processing_instruction::target(const std::string& value)
{
    _target = value;
}

void
processing_instruction::target(std::string&& value)
{
    _target = std::move(value);
}

const std::string&
processing_instruction::data() const
{
    return _data;
}

void
processing_instruction::data(const std::string& value)
{
    _data = value;
}

void
processing_instruction::data(std::string&& value)
{
    _data = std::move(value);
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
