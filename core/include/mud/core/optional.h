#ifndef _MUDLIB_CORE_OPTIONAL_H_
#define _MUDLIB_CORE_OPTIONAL_H_

#include <mud/core/ns.h>
#include <stdexcept>

BEGIN_MUDLIB_CORE_NS

/*
 * @brief Class template that managed an optional reference to an object.
 */
template<class Type>
class optional_ref
{
public:
    /**
     * Construct an optional reference object that does not have a value.
     */
    optional_ref() : _object(nullptr) {}

    /**
     * Construct an optional reference object that has a reference to an object.
     * @param[in] obj The reference to the object to retain.
     */
    optional_ref(Type& obj) : _object(&obj) {}

    /**
     * Copy constructor.
     */
    optional_ref(const optional_ref& rhs) : _object(rhs._object) {}

    /**
     * Move constructor.
     * @param[in] rhs The object to move from. After the operation, @c rhs will
     * not contain a reference to any object.
     */
    optional_ref(optional_ref&& rhs) : _object(rhs._object) { rhs._object = nullptr; }

    /**
     * Destructor.
     */
    virtual ~optional_ref() {}

    /**
     * Copy assignment.
     */
    optional_ref& operator=(const optional_ref& rhs) {
        _object = rhs._object;
        return *this;
    }

    /**
     * Move assignment, moving any reference to this instance.
     * @param[in] rhs The object to move from. After the operation, @c rhs will
     * not contain a reference to any object.
     */
    optional_ref& operator=(optional_ref&& rhs) {
        _object = rhs._object;
        rhs._object = nullptr;
        return *this;
    }

    /**
     * Return true if the optional reference contains a value.
     */
    operator bool() const { return _object != nullptr; }
    bool has_value() const { return _object != nullptr; }

    /**
     * Return the reference to the object. If there is no value, an @c
     * out_of_range exception is thrown.
     */
    Type& operator*() const { 
        if (!_object) {
            throw std::out_of_range("Optional reference has no valid object");
        }
        return *_object;
    }

private:
    /** The link to an object, or @c nullptr if there is no object. */
    Type* _object;
};

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_OPTIONAL_H_ */
