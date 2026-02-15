/*
 * ++ start-license-description ++
 *
 * Copyright (c) 2026 Stefan Sinnige.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * ++ end-license-description ++
 */

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
