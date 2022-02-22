#ifndef _MUDLIB_XML_COMMENT_H_
#define _MUDLIB_XML_COMMENT_H_

#include <mud/core/poly_vector.h>
#include <mud/xml/attribute.h>
#include <mud/xml/node.h>
#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of a XML comment
 */
class MUDLIB_XML_API comment : public node
{
public:
    /**
     * @brief Create an empty comment.
     */
    comment();

    /**
     * @brief Create an comment.
     * @param[in] data The comment.
     */
    comment(const std::string& data);

    /**
     * @brief Copy a comment.
     * @param[in] rhs The comment to copy from.
     */
    comment(const comment& rhs);

    /**
     * @brief Copy a comment through assignment.
     * @param[in] rhs The comment to copy from.
     * @return A reference to this comment.
     */
    comment& operator=(const comment& rhs);

    /**
     * @brief Move a comment.
     * @param[in] rhs The comment to move from. After moving, it will
     * resemble an empty comment.
     */
    comment(comment&& rhs);

    /**
     * @brief Move a comment through assignment.
     * @param[in] rhs The comment to move from. After moving, it will
     * resemble an empty comment.
     * @return A reference to this comment.
     */
    comment& operator=(comment&& rhs);

    /**
     * @brief Destructor.
     */
    virtual ~comment();

    /**
     * @brief Return the contents of the comment.
     */
    const std::string& text() const;

    /**
     * @brief Set the contents of the comment.
     * @param[in] value The value to set.
     */
    void text(const std::string& value);
    void text(std::string&& value);

private:
    /** The comment contents */
    std::string _text;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_COMMENT_H_ */
