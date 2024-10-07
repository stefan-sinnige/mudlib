#ifndef _MUDLIB_XML_NODE_H_
#define _MUDLIB_XML_NODE_H_

#include <mud/xml/ns.h>
#include <string>
#include <vector>

BEGIN_MUDLIB_XML_NS

/*
 * Forward declarations
 */
class cdata_section;
class char_data;
class comment;
class declaration;
class element;
class processing_instruction;
class node_seq;

/**
 * @brief Abstract representation of any node of an XML tree.
 *
 * @details
 * The XML node is any information item that can be added to the XML document
 * tree. These nodes can only be constructed as a @c shared_ptr and can only
 * be manipulated through the @c shared_ptr. This is accomplished by providing
 * a @c create function in each XML node (@c element, @c attribute, @c char_data
 * etc) and is the only means of creating an object of such a type.
 *
 * This concept would allow us to easily share XML nodes by their @c shared_ptr
 * in lists of children, references to a parent node and as part of XML XPath
 * query results.
 */
class MUDLIB_XML_API node : public std::enable_shared_from_this<node>
{
public:
    /**
     * @brief Type definition of a @c node pointer.
     */
    typedef std::shared_ptr<mud::xml::node> ptr;

    /**
     * @brief Return the @ shared_ptr associated to this instance.
     */
    ptr get_shared() {
        return shared_from_this();
    }

    /**
     * @brief Return the @ shared_ptr associated to this instance.
     */
    template<typename Item>
    typename Item::ptr get_shared() {
        return std::dynamic_pointer_cast<Item>(shared_from_this());
    }

    /**
     * The type of nodes.
     */
    enum class type_t
    {
        DOCUMENT,
        CDATA_SECTION,
        CHAR_DATA,
        COMMENT,
        DECL,
        ELEMENT,
        PI,
        ATTRIBUTE
    };

    /**
     * @brief Move a node.
     * @param[in] rhs The node to move from.
     */
    node(node&& rhs) { _type = rhs._type; }

    /**
     * @brief Copy a node through assignment.
     * @param[in] rhs The node to copy from.
     * @return A reference to this node.
     */
    node& operator=(node&& rhs)
    {
        _type = rhs._type;
        return *this;
    }

    /**
     * @brief Destruct a node.
     */
    virtual ~node() = default;

    /**
     * @brief Return the type of this node.
     */
    type_t type() const { return _type; }

    /**
     * @brief Get the parent of the node.
     */
    ptr parent() const {
        return std::static_pointer_cast<node>(_parent.lock());
    }

    /**
     * @brief Get the children of the node.
     *
     * @details
     * Return the children of a node. Some node types do not have the concept of
     * children. These will return an empty sequence.
     */
    virtual const mud::xml::node_seq& children() const;

protected:
    /**
     * @brief Construct a node of a certain type.
     *
     * @details
     * An XML node cannot be publicly constructed, but only through its
     * derived XML information item type.
     *
     * @param[in] type The type to assign to the node.
     */
    node(type_t type) : _type(type) {}

    /**
     * Set the parent of the node.
     */
    void parent(ptr parent) {
        _parent = parent;
    }

private:
    /** The node type. */
    type_t _type;

    /** The reference to the parent item. Note that this is a non-owning
     * weak pointer. If this were a shared_ptr, then adding a node as a child
     * of a parent in its children list, would create a circular destruction
     * dependency and the object would not be deleted. */
    std::weak_ptr<xml::node> _parent;
};

/**
 * @brief The list of node instances.
 *
 * @details
 * The list of node instances is an ordered list of pointers to nodes. This
 * concept would allow us to easily and efficiently share nodes (for example
 * in an XML document and an XPath result query).
 */
class MUDLIB_XML_API node_seq: public  std::vector<mud::xml::node::ptr>
{
};


END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_XML_NODE_H_ */
