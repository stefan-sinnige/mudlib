#ifndef _MUDLIB_HTML_COMMENT_H_
#define _MUDLIB_HTML_COMMENT_H_

#include <mud/core/poly_vector.h>
#include <mud/html/attribute.h>
#include <mud/html/node.h>
#include <mud/html/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_HTML_NS

/**
 * @brief Representation of a HTML comment
 */
class MUDLIB_HTML_API comment : public node
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

END_MUDLIB_HTML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_HTML_COMMENT_H_ */
