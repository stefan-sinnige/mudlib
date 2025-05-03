#ifndef _MUDLIB_XML_COMMENT_H_
#define _MUDLIB_XML_COMMENT_H_

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
     * @brief Type definition of a @c comment pointer.
     */
    typedef std::shared_ptr<mud::xml::comment> ptr;

    /**
     * @brief Create a new @c comment instance.
     * @param text The text to set.
     */
    static ptr create(const std::string& text);

    /**
     * @brief Move a comment.
     * @param[in] rhs The comment to move from. After moving, it will
     * resemble an empty comment.
     */
    comment(comment&& rhs) = default;

    /**
     * @brief Move a comment through assignment.
     * @param[in] rhs The comment to move from. After moving, it will
     * resemble an empty comment.
     * @return A reference to this comment.
     */
    comment& operator=(comment&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~comment() = default;

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
    /**
     * @brief Create an empty comment.
     */
    comment(const std::string& text);

    /** The comment contents */
    std::string _text;
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_COMMENT_H_ */
