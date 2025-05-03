#ifndef _MUDLIB_XML_NAMESPACE_H_
#define _MUDLIB_XML_NAMESPACE_H_

#include <list>
#include <string>
#include <mud/xml/ns.h>
#include <mud/core/uri.h>

BEGIN_MUDLIB_XML_NS

/**
 * @brief Representation of an XML namespace.
 */
class MUDLIB_XML_API ns
{
public:
    /**
     * @brief Type definition of an @c ns pointer.
     */
    typedef std::shared_ptr<mud::xml::ns> ptr;

    /**
     * @brief Create a new @c ns instance.
     */
    static ptr create();

    /**
     * @brief Move a namespace.
     * @param[in] rhs The namespace to move from. After moving, it will
     * resemble an empty namespace.
     */
    ns(ns&& rhs) = default;

    /**
     * @brief Move a namespace through assignment.
     * @param[in] rhs The namespace to move from. After moving, it will
     * resemble an empty namespace.
     * @return A reference to this namespace.
     */
    ns& operator=(ns&& rhs) = default;

    /**
     * @brief Destructor.
     */
    virtual ~ns() = default;

    /**
     * @brief Set the namespace URI.
     * @param[in] value The value to set.
     *
     * @details
     * The @c URI is a namespace specification that provides the @c element
     * or @c attribute with a unique identification to resolve ambiguity.
     */
    void uri(const mud::core::uri& value);
    void uri(mud::core::uri&& value);

    /**
     * @brief Return the namespace URI.
     *
     * @details
     * The @c URI is a namespace specification that provides the @c element
     * or @c attribute with a unique identification to resolve ambiguity.
     */
    const mud::core::uri& uri() const;

    /**
     * @brief Set the namespace prefix.
     * @param[in] value The value to set.
     *
     * @details
     * The @c prefix is used as part of a qualified name for an element and
     * attribute. If the prefix is empty, the namespace is defined as the
     * default namesapce.
     */
    void prefix(const std::string& value);
    void prefix(std::string&& value);

    /**
     * @brief Return the namespace prefix.
     *
     * @details
     * The @c prefix is used as part of a qualified name for an element and
     * attribute. If the prefix is empty, the namespace is defined as the
     * default namesapce.
     */
    const std::string& prefix() const;

    /**
     * @brief Flag to indicate if this is a resolved namespace within the
     * document context.
     */
    bool resolved() const;

private:
    /**
     * @brief Create an empty namespace.
     */
    ns();

    /**
     * @brief Set the resolved status of the namespace.
     */
    void resolved(bool value);

    /** The qualified name */
    std::string _prefix;

    /** The namespace URI. */
    mud::core::uri _uri;

    /** The resolved flag. */
    bool _resolved;

    /** Element and attributes can alter the resolved state. */
    friend class element;
    friend class attribute;
};

/**
 * @brief The list of namespaces.
 *
 * @details
 * The list of namespace instances is a list of pointers to namespaces. This
 * concept would allow us to easily and efficiently share namespace definitions.
 */
class MUDLIB_XML_API ns_list: public  std::list<mud::xml::ns::ptr>
{
};

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_NAMESPACE_H_ */
