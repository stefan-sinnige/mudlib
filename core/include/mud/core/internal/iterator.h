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

#ifndef _MUDLIB_CORE_INTERNAL_ITERATOR_H_
#define _MUDLIB_CORE_INTERNAL_ITERATOR_H_

#include <mud/core/ns.h>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>

BEGIN_MUDLIB_CORE_NS

namespace internal {

namespace random_access {

/**
 * @brief Random access iterator.
 */
template<typename ValueType, typename ClassType>
class iterator
{
public:
    /**
     * Member type definitions.
     */
    typedef std::random_access_iterator_tag iterator_category;
    typedef ValueType value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;

    /**
     * @brief Create an uninitialised iterator.
     */
    iterator() = default;

    /**
     * @brief Destruct the iterator.
     */
    virtual ~iterator() = default;

    /**
     * Return a reference to the object pointed to by the iterator.
     */
    reference operator*() {
        return *_ptr;
    }

    /**
     * Return a pointer to the object pointed to by the iterator.
     */
    pointer operator->() {
        return _ptr;
    }

    /**
     * Increase the iterator to the next element.
     */
    iterator& operator++() {
        ++_ptr;
        return *this;
    }
    iterator operator++(int) {
        auto tmp = _ptr;
        ++_ptr;
        return tmp;
    }

    /**
     * @brief Increase the reverse_iterator a number of element.
     * @param n The number of elements to advance.
     * @return The iterator itself.
     */
    iterator& operator+=(difference_type n) {
        _ptr += n;
        return *this;
    }

    /**
     * Decrease the iterator to the previous element.
     */
    iterator& operator--() {
        --_ptr;
        return *this;
    }
    iterator operator--(int) {
        auto tmp = _ptr;
        --_ptr;
        return tmp;
    }

    /**
     * @brief Decrease the reverse_iterator a number of element.
     * @param n The number of elements to recede.
     * @return The iterator itself.
     */
    iterator& operator-=(difference_type n) {
        _ptr -= n;
        return *this;
    }

    /**
     * Comparison between iterators.
     * @param lhs The first iterator.
     * @param lhs The second iterator.
     */
    friend auto operator<=>(const iterator& lhs, const iterator& rhs) = default;

    /**
     * @brief Increase the iterator a number of element.
     * @param n The number of elements to advance.
     * @param iter The iterator to increase to
     * @return A new iterator pointing to the increased position.
     */
    friend iterator operator+(
            difference_type n,
            const iterator& iter)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Increase the iterator a number of element.
     * @param iter The iterator to increase to
     * @param n The number of elements to advance.
     * @return A new iterator pointing to the increased position.
     */
    friend iterator operator+(
            const iterator& iter,
            difference_type n)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param n The number of elements to recede.
     * @param iter The iterator to decrease from
     * @return A new iterator pointing to the decreased position.
     */
    friend iterator operator-(
            difference_type n,
            const iterator& iter)
    {
        return iter._ptr - n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param iter The iterator to decrease from
     * @param n The number of elements to recede.
     * @return A new iterator pointing to the decreased position.
     */
    friend iterator operator-(
            const iterator& iter,
            difference_type n)
    {
        return iter._ptr - n;
    }

    /**
     * @brief The difference between two iterators.
     * @parm first The first iterator.
     * @parm second The second iterator.
     * @return The difference between the iterator positions.
     */
    friend difference_type operator-(
            const iterator& first,
            const iterator& second)
    {
        return first._ptr - second._ptr;
    }

private:
    /** 
     * @brief Construct an iterator pointing to an element in the buffer.
     * @param pos The position.
     */
    iterator(pointer pos) : _ptr(pos) {}

    /* The pointer to the element */
    pointer _ptr = nullptr;

    /* Friend */
    friend ClassType;
};

/**
 * @brief Random access iterator.
 */
template<typename ValueType, typename ClassType>
class const_iterator
{
public:
    /**
     * Member type definitions.
     */
    typedef std::random_access_iterator_tag iterator_category;
    typedef const uint8_t value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;

    /**
     * @brief Create an uninitialised iterator.
     */
    const_iterator() = default;

    /**
     * @brief Destruct the iterator.
     */
    virtual ~const_iterator() = default;

    /**
     * Return a reference to the object pointed to by the iterator.
     */
    reference operator*()
    {
        return *_ptr;
    }

    /**
     * Return a pointer to the object pointed to by the iterator.
     */
    pointer operator->()
    {
        return _ptr;
    }

    /**
     * Increase the iterator to the next element.
     */
    const_iterator& operator++()
    {
        ++_ptr;
        return *this;
    }
    const_iterator operator++(int)
    {
        auto tmp = _ptr;
        ++_ptr;
        return tmp;
    }

    /**
     * @brief Increase the reverse_iterator a number of element.
     * @param n The number of elements to advance.
     * @return The iterator advanced
     */
    const_iterator& operator+=(difference_type n) {
        _ptr += n;
        return *this;
    }

    /**
     * Decrease the iterator to the previous element.
     */
    const_iterator& operator--()
    {
        --_ptr;
        return *this;
    }
    const_iterator operator--(int)
    {
        auto tmp = _ptr;
        --_ptr;
        return tmp;
    }

    /**
     * @brief Decrease the reverse_iterator a number of element.
     * @param n The number of elements to recede.
     * @return The iterator receded
     */
    const_iterator& operator-=(difference_type n) {
        _ptr -= n;
        return *this;
    }

    /**
     * Comparison between iterators.
     * @param lhs The first iterator.
     * @param lhs The second iterator.
     */
    friend auto operator<=>(const const_iterator& lhs,
                            const const_iterator& rhs) = default;

    /**
     * @brief Increase the iterator a number of element.
     * @param n The number of elements to advance.
     * @param iter The iterator to increase to
     * @return A new iterator pointing to the increased position.
     */
    friend const_iterator operator+(
            difference_type n,
            const const_iterator& iter)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Increase the iterator a number of element.
     * @param iter The iterator to increase to
     * @param n The number of elements to advance.
     * @return A new iterator pointing to the increased position.
     */
    friend const_iterator operator+(
            const const_iterator& iter,
            difference_type n)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param n The number of elements to recede.
     * @param iter The iterator to decrease from
     * @return A new iterator pointing to the decreased position.
     */
    friend const_iterator operator-(
            difference_type n,
            const const_iterator& iter)
    {
        return iter._ptr - n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param iter The iterator to decrease from
     * @param n The number of elements to recede.
     * @return A new iterator pointing to the decreased position.
     */
    friend const_iterator operator-(
            const const_iterator& iter,
            difference_type n)
    {
        return iter._ptr - n;
    }

    /**
     * @brief The difference between two iterators.
     * @parm first The first iterator.
     * @parm second The second iterator.
     * @return The difference between the iterator positions.
     */
    friend difference_type operator-(
            const const_iterator& first,
            const const_iterator& second)
    {
        return first._ptr - second._ptr;
    }

 private:
    /** 
     * @brief Construct an iterator pointing to an element in the buffer.
     * @param pos The position.
     */
    const_iterator(pointer pos) : _ptr(pos) {}

    /* The pointer to the element */
    pointer _ptr = nullptr;

    /* Friend */
    friend ClassType;
};

/**
 * @brief Reverse random access iterator.
 */
template<typename ValueType, typename ClassType>
class reverse_iterator
{
public:
    /**
     * Member type definitions.
     */
    typedef std::random_access_iterator_tag iterator_category;
    typedef ValueType value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;

    /**
     * @brief Create an uninitialised reverse_iterator.
     */
    reverse_iterator() = default;

    /**
     * @brief Destruct the reverse_iterator.
     */
    virtual ~reverse_iterator() = default;

    /**
     * Return a reference to the object pointed to by the reverse_iterator.
     */
    reference operator*() {
        return *_ptr;
    }

    /**
     * Return a pointer to the object pointed to by the reverse_iterator.
     */
    pointer operator->() {
        return _ptr;
    }

    /**
     * Increase the reverse_iterator to the next element.
     */
    reverse_iterator& operator++() {
        --_ptr;
        return *this;
    }
    reverse_iterator operator++(int) {
        auto tmp = _ptr;
        --_ptr;
        return tmp;
    }

    /**
     * @brief Increase the reverse_iterator a number of element.
     * @param n The number of elements to advance.
     * @return The iterator advanced
     */
    reverse_iterator& operator+=(difference_type n) {
        _ptr -= n;
        return *this;
    }

    /**
     * Decrease the reverse iterator to the previous element.
     */
    reverse_iterator& operator--() {
        ++_ptr;
        return *this;
    }
    reverse_iterator operator--(int) {
        auto tmp = _ptr;
        ++_ptr;
        return tmp;
    }

    /**
     * @brief Decrease the reverse_iterator a number of element.
     * @param n The number of elements to recede.
     * @return The iterator receded
     */
    reverse_iterator& operator-=(difference_type n) {
        _ptr += n;
        return *this;
    }

    /**
     * Comparison between iterators.
     * @param lhs The first iterator.
     * @param lhs The second iterator.
     */
    friend auto operator<=>(const reverse_iterator& lhs,
                            const reverse_iterator& rhs) = default;

    /**
     * @brief Increase the iterator a number of element.
     * @param n The number of elements to advance.
     * @param iter The iterator to increase to
     * @return A new iterator pointing to the increased position.
     */
    friend reverse_iterator operator+(
            difference_type n,
            const reverse_iterator& iter)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Increase the iterator a number of element.
     * @param iter The iterator to increase to
     * @param n The number of elements to advance.
     * @return A new iterator pointing to the increased position.
     */
    friend reverse_iterator operator+(
            const reverse_iterator& iter,
            difference_type n)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param n The number of elements to recede.
     * @param iter The iterator to decrease from
     * @return A new iterator pointing to the decreased position.
     */
    friend reverse_iterator operator-(
            difference_type n,
            const reverse_iterator& iter)
    {
        return iter._ptr - n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param iter The iterator to decrease from
     * @param n The number of elements to recede.
     * @return A new iterator pointing to the decreased position.
     */
    friend reverse_iterator operator-(
            const reverse_iterator& iter,
            difference_type n)
    {
        return iter._ptr - n;
    }

    /**
     * @brief The difference between two iterators.
     * @parm first The first iterator.
     * @parm second The second iterator.
     * @return The difference between the iterator positions.
     */
    friend difference_type operator-(
            const reverse_iterator& first,
            const reverse_iterator& second)
    {
        return first._ptr - second._ptr;
    }

private:
    /** 
     * @brief Construct an reverse iterator pointing to an element in the
     * buffer.
     * @param pos The position.
     */
    reverse_iterator(pointer pos) : _ptr(pos) {}

    /* The pointer to the element */
    pointer _ptr = nullptr;

    /* Friend */
    friend ClassType;
};

/**
 * @brief Reverse random access iterator.
 */
template<typename ValueType, typename ClassType>
class const_reverse_iterator
{
public:
    /**
     * Member type definitions.
     */
    typedef std::random_access_iterator_tag iterator_category;
    typedef const ValueType value_type;
    typedef value_type* pointer;
    typedef value_type& reference;
    typedef std::ptrdiff_t difference_type;

    /**
     * @brief Create an uninitialised reverse_iterator.
     */
    const_reverse_iterator() = default;

    /**
     * @brief Destruct the reverse_iterator.
     */
    virtual ~const_reverse_iterator() = default;

    /**
     * Return a reference to the object pointed to by the reverse_iterator.
     */
    reference operator*()
    {
        return *_ptr;
    }

    /**
     * Return a pointer to the object pointed to by the reverse_iterator.
     */
    pointer operator->()
    {
        return _ptr;
    }

    /**
     * Increase the reverse_iterator to the next element.
     */
    const_reverse_iterator& operator++()
    {
        --_ptr;
        return *this;
    }
    const_reverse_iterator operator++(int)
    {
        auto tmp = _ptr;
        --_ptr;
        return tmp;
    }

    /**
     * @brief Increase the reverse_iterator a number of element.
     * @param n The number of elements to advance.
     * @return The iterator advanced
     */
    const_reverse_iterator& operator+=(difference_type n) {
        _ptr -= n;
        return *this;
    }

    /**
     * Decrease the reverse_iterator to the previous element.
     */
    const_reverse_iterator& operator--()
    {
        ++_ptr;
        return *this;
    }
    const_reverse_iterator operator--(int)
    {
        auto tmp = _ptr;
        ++_ptr;
        return tmp;
    }

    /**
     * @brief Decrease the reverse_iterator a number of element.
     * @param n The number of elements to recede.
     * @return The iterator receded
     */
    const_reverse_iterator& operator-=(difference_type n) {
        _ptr += n;
        return *this;
    }

    /**
     * Comparison between iterators.
     * @param lhs The first iterator.
     * @param lhs The second iterator.
     */
    friend auto operator<=>(const const_reverse_iterator& lhs,
                            const const_reverse_iterator& rhs) = default;

    /**
     * @brief Increase the iterator a number of element.
     * @param n The number of elements to advance.
     * @param iter The iterator to increase to
     * @return A new iterator pointing to the increased position.
     */
    friend const_reverse_iterator operator+(
            difference_type n,
            const const_reverse_iterator& iter)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Increase the iterator a number of element.
     * @param iter The iterator to increase to
     * @param n The number of elements to advance.
     * @return A new iterator pointing to the increased position.
     */
    friend const_reverse_iterator operator+(
            const const_reverse_iterator& iter,
            difference_type n)
    {
        return iter._ptr + n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param n The number of elements to recede.
     * @param iter The iterator to decrease from
     * @return A new iterator pointing to the decreased position.
     */
    friend const_reverse_iterator operator-(
            difference_type n,
            const const_reverse_iterator& iter)
    {
        return iter._ptr - n;
    }

    /**
     * @brief Decrease the iterator a number of element.
     * @param iter The iterator to decrease from
     * @param n The number of elements to recede.
     * @return A new iterator pointing to the decreased position.
     */
    friend const_reverse_iterator operator-(
            const const_reverse_iterator& iter,
            difference_type n)
    {
        return iter._ptr - n;
    }

    /**
     * @brief The difference between two iterators.
     * @parm first The first iterator.
     * @parm second The second iterator.
     * @return The difference between the iterator positions.
     */
    friend difference_type operator-(
            const const_reverse_iterator& first,
            const const_reverse_iterator& second)
    {
        return first._ptr - second._ptr;
    }

 private:
    /** 
     * @brief Construct a reverse iterator pointing to an element in the
     * buffer.
     * @param pos The position.
     */
    const_reverse_iterator(pointer pos) : _ptr(pos) {}

    /* The pointer to the element */
    pointer _ptr = nullptr;

    /* Friend */
    friend ClassType;
};

} /* random_access */

} /* internal */

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_INTERNAL_ITERATOR_H_ */
