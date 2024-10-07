#ifndef _MUDLIB_XML_PROCESSING_INSTRUCTION_H_
#define _MUDLIB_XML_PROCESSING_INSTRUCTION_H_

#include <mud/core/poly_vector.h>
#include <mud/xml/attribute.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of a XML processing instruction
 */
class MUDLIB_XML_API processing_instruction : public node
{
public:
    /**
     * @brief Type definition of a @c processing_instruction pointer.
     */
    typedef std::shared_ptr<mud::xml::processing_instruction> ptr;

    /**
     * @brief Create a new @c processing_instruction instance.
     */
    static ptr create();

    /**
     * @brief Move a processing instruction.
     * @param[in] rhs The processing instruction to move from. After moving,
     * it will resemble an empty processing instruction.
     */
    processing_instruction(processing_instruction&& rhs);

    /**
     * @brief Move a processing instruction through assignment.
     * @param[in] rhs The processing instruction to move from. After moving,
     * it will resemble an empty processing instruction.
     * @return A reference to this processing instruction.
     */
    processing_instruction& operator=(processing_instruction&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~processing_instruction();

    /**
     * @brief Return the target of the processing instruction.
     */
    const std::string& target() const;

    /**
     * @brief Set the target of the processing instruction
     * @param[in] value The value to set.
     */
    void target(const std::string& value);
    void target(std::string&& value);

    /**
     * @brief Return the data of the processing instruction.
     */
    const std::string& data() const;

    /**
     * @brief Set the data of the processing instruction
     * @param[in] value The value to set.
     */
    void data(const std::string& value);
    void data(std::string&& value);

private:
    /**
     * @brief Create an empty processing instruction..
     */
    processing_instruction();

    /** The processing instruction target */
    std::string _target;

    /** The processing instruction data */
    std::string _data;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_PROCESSING_INSTRUCTION_H_ */
