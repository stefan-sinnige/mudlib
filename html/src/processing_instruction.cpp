#include "mud/html/processing_instruction.h"
#include <utility>

BEGIN_MUDLIB_HTML_NS

processing_instruction::processing_instruction() : node(node::type_t::PI) {}

processing_instruction::processing_instruction(const std::string& target,
                                               const std::string& data)
  : node(node::type_t::PI), _target(target), _data(data)
{}

processing_instruction::~processing_instruction() {}

processing_instruction::processing_instruction(
    const processing_instruction& rhs)
  : node(node::type_t::PI)
{
    (void)operator=(rhs);
}

processing_instruction::processing_instruction(processing_instruction&& rhs)
  : node(node::type_t::PI)
{
    *this = std::move(rhs);
}

processing_instruction&
processing_instruction::operator=(const processing_instruction& rhs)
{
    if (&rhs != this) {
        _target = rhs._target;
        _data = rhs._data;
    }
    return *this;
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

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */
