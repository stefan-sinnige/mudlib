#include "mud/xml/attribute.h"
#include "mud/xml/element.h"
#include <utility>

BEGIN_MUDLIB_XML_NS

/* static */ attribute::ptr
attribute::create(const std::string& qname)
{
    struct make_shared_enabler: public xml::attribute
    {
        make_shared_enabler(const std::string& qname)
            : xml::attribute(qname)
        {}
    };
    return std::make_shared<make_shared_enabler>(qname);
}

/* static */ attribute::ptr
attribute::create(const std::string& local_name, const mud::core::uri& uri)
{
    struct make_shared_enabler: public xml::attribute
    {
        make_shared_enabler(const std::string& local_name,
                            const mud::core::uri& uri)
            : xml::attribute(local_name, uri)
        {}
    };
    return std::make_shared<make_shared_enabler>(local_name, uri);
}

attribute::attribute(const std::string& qname)
    : node(node::type_t::ATTRIBUTE)
    , _name(qname)
    , _ns(mud::xml::ns::create())
{
    // Split the qualified name into a prefix and local-name
    std::string prefix, local_name;
    auto pos = _name.find_first_of(':');
    if (pos == std::string::npos) {
        local_name = qname;
    }
    else {
        prefix = qname.substr(0, pos);
        local_name = qname.substr(pos+1);
    }

    // Check if this is a namespace attribute or an ordinary one.
    if (prefix == "xmlns" || (prefix.empty() && local_name == "xmlns")) {
        // This attribute is a resolved namespace definition. The value should
        // hold the namespace URI. The attribute does not have a local or qname.
        if (!prefix.empty()) {
            _ns->prefix(local_name);
        }
        _ns->resolved(true);
        _local_name.clear();
        _name.clear();
    }
    else {
        // This attribute is an ordinary definition.
        _ns->prefix(prefix);
        _local_name = local_name;
    }
}

attribute::attribute(const std::string& local_name, const mud::core::uri& uri)
    : node(node::type_t::ATTRIBUTE)
    , _local_name(local_name)
    , _name(local_name)
    , _ns(mud::xml::ns::create())
{
    _ns->uri(uri);
}

const std::string&
attribute::name() const
{
    return _name;
}

const std::string&
attribute::value() const
{
    return _value;
}

void
attribute::value(const std::string& value)
{
    _value = value;

    // If the attribute is a resolved namespace, the attribute value is the
    // namespace URI.
    if (_ns->resolved()) {
        mud::core::uri uri(_value);
        _ns->uri(uri);
    }
}

void
attribute::value(std::string&& value)
{
    _value = std::move(value);

    // If the attribute is a resolved namespace, the attribute value is the
    // namespace URI.
    if (_ns->resolved()) {
        mud::core::uri uri(_value);
        _ns->uri(uri);
    }
}

const std::string&
attribute::prefix() const
{
    return _ns->prefix();
}

const std::string&
attribute::local_name() const
{
    return _local_name;
}

const mud::xml::ns::ptr&
attribute::ns() const
{
    return _ns;
}

mud::xml::ns::ptr
attribute::resolved_ns() const
{
    auto ancestor = _parent.lock();
    if (!ancestor) {
        return nullptr;
    }
    return ancestor->get_shared<mud::xml::element>()->resolved_ns(_ns->uri());
}

std::size_t
attribute_hash::operator()(
    const mud::xml::attribute::ptr& key) const
{
    if (!key) {
        // Non-existent attribute.
        return 0;
    }
    else {
        // For namespace attribute hash the URI, otherwise hash the attribute
        // name.
        if (key->ns()->resolved()) {
            std::stringstream sstr;
            sstr << key->ns()->uri();
            return std::hash<std::string>()(sstr.str());
        }
        else {
            return std::hash<std::string>()(key->name());
        }
    }
}
 
std::size_t
attribute_equal_to::operator()(
    const mud::xml::attribute::ptr& key1,
    const mud::xml::attribute::ptr& key2) const
{
    if (key1 == key2) {
        // Same underlying attribute object.
        return true;
    }
    else {
        if (!key2 || !key2) {
            // One of the object is non-existent.
            return false;
        }
        // Equality on the attribute name
        return std::equal_to<std::string>()(key1->name(), key2->name());
    }
}

END_MUDLIB_XML_NS

/* vi: set ai ts=4 expandtab: */
