#ifndef _MUDLIB_HTML_PROCESSING_INSTRUCTION_H_
#define _MUDLIB_HTML_PROCESSING_INSTRUCTION_H_

#include <mud/core/poly_vector.h>
#include <mud/html/attribute.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of a HTML processing instruction
 */
class MUDLIB_HTML_API processing_instruction : public node
{
public:
    /**
     * @brief Create an empty processing instruction..
     */
    processing_instruction();

    /**
     * @brief Create an empty processing instruction..
     * @param[in] target The target.
     * @param[in] data The data for the target.
     */
    processing_instruction(const std::string& target, const std::string& data);

    /**
     * @brief Copy a processing instruction.
     * @param[in] rhs The processing instruction to copy from.
     */
    processing_instruction(const processing_instruction& rhs);

    /**
     * @brief Copy a processing instruction through assignment.
     * @param[in] rhs The processing instruction to copy from.
     * @return A reference to this processing instruction.
     */
    processing_instruction& operator=(const processing_instruction& rhs);

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
    /** The processing instruction target */
    std::string _target;

    /** The processing instruction data */
    std::string _data;
};

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_PROCESSING_INSTRUCTION_H_ */
