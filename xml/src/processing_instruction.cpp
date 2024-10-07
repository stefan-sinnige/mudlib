#include "mud/xml/processing_instruction.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ processing_instruction::ptr
processing_instruction::create()
{
    return std::shared_ptr<xml::processing_instruction>(
            new processing_instruction());
}

processing_instruction::processing_instruction() : node(node::type_t::PI) {}

processing_instruction::~processing_instruction() {}

processing_instruction::processing_instruction(processing_instruction&& rhs)
  : node(node::type_t::PI)
{
    *this = std::move(rhs);
}

processing_instruction&
processing_instruction::operator=(processing_instruction&& rhs)
{
    _target = std::move(rhs._target);
    _data = std::move(rhs._data);
    return *this;
}

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
