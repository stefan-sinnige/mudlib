#include "mud/xml/processing_instruction.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ processing_instruction::ptr
processing_instruction::create(const std::string& target,
        const std::string& data)
{
    return std::shared_ptr<xml::processing_instruction>(
            new processing_instruction(target, data));
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
