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

#ifndef _MUDLIB_CORE_BUFFER_H_
#define _MUDLIB_CORE_BUFFER_H_

#include <mud/core/ns.h>
#include <mud/core/endian.h>
#include <mud/core/internal/iterator.h>
#include <concepts>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <memory>

BEGIN_MUDLIB_CORE_NS

/**
 * @brief A view inside a @c buffer.
 * @details
 * A @c buffer_view provides access to a @c buffer, or parts thereof. The view
 * can be used to access the buffer and to make limited modifications.
 */
class MUDLIB_CORE_API buffer_view
{
public:
    /** Random access iterator */
    typedef mud::core::internal::random_access::iterator
            <uint8_t, buffer_view> iterator;

    /** Random access constant iterator */
    typedef mud::core::internal::random_access::const_iterator
            <uint8_t, buffer_view> const_iterator;

    /** Random access reverse iterator */
    typedef mud::core::internal::random_access::reverse_iterator
            <uint8_t, buffer_view> reverse_iterator;

    /** Random access reverse constant iterator */
    typedef mud::core::internal::random_access::const_reverse_iterator
            <uint8_t, buffer_view> const_reverse_iterator;

    /**
     * Create an uninitialised string view.
     */
    buffer_view() = default;

    /**
     * @brief Create a view.
     * @param pos The pointer position of the start of the view.
     * @param size The number of bytes in the view from the @c pos position.
     */
    buffer_view(uint8_t* pos, size_t size)
        : _pos(pos), _sz(size)
    {}

    /**
     * Destructor.
     */
    virtual ~buffer_view() = default;

    /**
     * @brief Copy constructor.
     * @param rhs The item to copy from.
     */
    buffer_view(const buffer_view& rhs) = default;

    /**
     * @brief Move constructor.
     * @param rhs The item to move from.
     */
    buffer_view(buffer_view&& rhs) = default;

    /**
     * @brief Copy assignment.
     * @param rhs The item to copy from.
     * @return Reference to this object.
     */
    buffer_view& operator=(const buffer_view& rhs) = default;

    /**
     * @brief Move assignment.
     * @param rhs The item to move from.
     * @return Reference to this object.
     */
    buffer_view& operator=(buffer_view&& rhs) = default;

    /**
     * @brief Return the iterator to the begin of the buffer.
     */
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    /**
     * @brief Return the iterator to past the end of the buffer.
     */
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    /**
     * @brief Return the reverse iterator to the end of the buffer.
     */
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    /**
     * @brief Return the iterator to past the begin of the buffer.
     */
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    /**
     * @brief Return the pointer to the underlying data.
     * @return Pointer to the begin of the string of bytes.
     */
    uint8_t* data() { return _pos; }
    const uint8_t* data() const { return _pos; }

    /**
     * @brief Return the byte at the n-th position.
     * @param n The byte position (0 to size-1) to return, where 0 is the left
     * most position.
     * @return The byte at the position.
     */
    uint8_t operator[](size_t n) const;
    uint8_t& operator[](size_t n);

    /**
     * @brief Return the size of the byte storage.
     * @return The size of the storage.
     */
    size_t size() const { return _sz; }

    /**
     * @brief Get the integral value at a certain position.
     * @tparam Type The integral type.
     * @param n The byte position of the first byte to the integral value.
     * @param from The endian system that the buffer stores the byte in.
     * @param to The endian system that the value should be converted to.
     * @return The integral value.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    Type get(size_t n, endian::endian_t from,
             endian::endian_t to = endian::endian_t::native) const
    {
        if (n + sizeof(Type) > size()) {
            throw std::range_error("index out of bounds");
        }
        return endian::convert(from, to,
                *(reinterpret_cast<const Type*>(data() + n)));
    }

    /**
     * @brief Get the integral value at a certain position.
     * @tparam Type The integral type.
     * @param pos The position of the first byte to the integral value.
     * @param from The endian system that the buffer stores the byte in.
     * @param to The endian system that the value should be converted to.
     * @return The integral value.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    Type get(const_iterator pos, endian::endian_t from,
             endian::endian_t to = endian::endian_t::native) const
    {
        return get<Type>(&*pos - data(), from, to);
    }

    /**
     * @brief Set the integral value at a certain position.
     * @tparam Type The integral type.
     * @param n The byte position of the first byte to the integral value.
     * @param value The intergal value to set.
     * @param to The endian system that the value should be converted to.
     * @param from The endian system that the @c value is provided as.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    void set(size_t n, Type value, endian::endian_t to,
             endian::endian_t from = endian::endian_t::native)
    {
        if (n + sizeof(Type) > size()) {
            throw std::range_error("index out of bounds");
        }
        *(reinterpret_cast<Type*>(data() + n)) =
                endian::convert(from, to, value);
    }

    /**
     * @brief Get the integral value at a certain position.
     * @tparam Type The integral type.
     * @param pos The position of the first byte to the integral value.
     * @param value The intergal value to set.
     * @param to The endian system that the value should be converted to.
     * @param from The endian system that the @c value is provided as.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    void set(iterator pos, Type value, endian::endian_t to,
             endian::endian_t from = endian::endian_t::native)
    {
        return set<Type>(&*pos - data(), value, to, from);
    }

    /**
     * @brief Create a sub-view of any part of the view.
     * @param first The first position
     * @param last The last position (not inclusive).
     * @return The view from the first to the last position (not inclusive).
     */
    buffer_view view(iterator first, iterator last) const;
    buffer_view view(const_iterator first, const_iterator last) const;

private:
    /* The starting position of the view. */
    uint8_t* _pos;

    /* The number of bytes of the view. */
    size_t _sz = 0;
};

/**
 * @brief Generic sequence of bytes.
 * @details
 * A generic storage for an arbitrary sequence of bytes. The context in which
 * the data is used determines its purpose. The buffer data can be accessed
 * directly, through iterators and streaming objects. The buffer data can also
 * be efficiently shared amongst other @c buffer objects through a @c
 * buffer_view, providing both read and modification access.
 */
class MUDLIB_CORE_API buffer
{
public:
    /** Random access iterator */
    typedef mud::core::internal::random_access::iterator
            <uint8_t, buffer> iterator;

    /** Random access constant iterator */
    typedef mud::core::internal::random_access::const_iterator
            <uint8_t, buffer> const_iterator;

    /** Random access reverse iterator */
    typedef mud::core::internal::random_access::reverse_iterator
            <uint8_t, buffer> reverse_iterator;

    /** Random access reverse constant iterator */
    typedef mud::core::internal::random_access::const_reverse_iterator
            <uint8_t, buffer> const_reverse_iterator;

    /**
     * @brief Constructing a sequence of data of a certain size.
     * @param sz The number of bytes to allocate.
     */
    explicit buffer(size_t sz = 0);

    /**
     * @brief Constructing a sequence of data through an initialiser.
     * @param init The initialiser list, as a container of @c uint8_t elements.
     */
    buffer(std::initializer_list<uint8_t> init);

    /**
     * @brief Construct from a string of bytes.
     * @param str The pointer to the string of bytes
     * @param sz The number of bytes.
     */
    buffer(const uint8_t* d, size_t sz);

    /**
     * @brief Copy constructor.
     * @param rhs The item to copy from.
     * @details
     * The @c rhs buffer's contents will be copied.
     */
    buffer(const buffer& rhs);

    /**
     * @brief Move constructor.
     * @param rhs The item to move from.
     */
    buffer(buffer&& rhs) = default;

    /**
     * @brief Constructor fro ma view.
     * @param view The item to copy from.
     * @details
     * The @c rhs buffer's contents will be copied.
     */
    buffer(const buffer_view& view);

    /**
     * @brief Copy assignment.
     * @param rhs The item to copy from.
     * @return Reference to this object.
     * @details
     * The @c rhs buffer's contents will be copied.
     */
    buffer& operator=(const buffer& rhs);

    /**
     * @brief Move assignment.
     * @param rhs The item to move from.
     * @return Reference to this object.
     */
    buffer& operator=(buffer&& rhs) = default;

    /**
     * @brief Copy a buffer view.
     * @param rhs The item to copy from.
     * @return Reference to this object.
     * @details
     * The @c rhs buffer's contents will be copied.
     */
    buffer& operator=(const buffer_view& rhs);

    /**
     * @brief Clear the data
     * @details
     * Clear the data and set the size to zero.
     */
    void clear();

     /**
     * @brief Insert a byte at a specified location.
     * @param pos The position to insert to.
     * @param value The value to insert.
     * @return The location of the inserted element.
     * @note All existing iterators and views may be invalidated.
     */
    iterator insert(iterator pos, uint8_t value);
    iterator insert(const_iterator pos, uint8_t value);

    /**
     * @brief Insert multiple bytes at a specified location.
     * @param pos The position to insert to.
     * @param count The number of bytes to insert.
     * @param value The value of each byte.
     * @return The location of the first inserted element.
     * @note All existing iterators and views may be invalidated.
     */
    iterator insert(iterator pos, size_t count, uint8_t value);
    iterator insert(const_iterator pos, size_t count, uint8_t value);

    /**
     * @brief Insert a range of bytes at a specified position.
     * @param pos The position to insert to.
     * @param ptr The pointer to the bytes to insert.
     * @param count The number of bytes to insert.
     * @return The location of the first inserted element.
     * @note All existing iterators and views may be invalidated.
     */
    iterator insert(iterator pos, const uint8_t* ptr, size_t count);
    iterator insert(const_iterator pos, const uint8_t* ptr, size_t count);

    /**
     * @brief Insert multiple bytes from a range at a specified location.
     * @param pos The position to insert to.
     * @param p1 The position of the first element to copy.
     * @param p2 The position of the last element (not to be copied itself).
     * @return The location of the first inserted element.
     * @note All existing iterators and views may be invalidated.
     */
    iterator insert(iterator pos, iterator p1, iterator p2);
    iterator insert(const_iterator pos, const_iterator p1, const_iterator p2);

    /**
     * @brief Insert multiple bytes from an initialiser list at a specified
     * location.
     * @param pos The position to insert to.
     * @param init The initialiser list to insert.
     * @return The location of the first inserted element.
     * @note All existing iterators and views may be invalidated.
     */
    iterator insert(iterator pos, std::initializer_list<uint8_t> init);
    iterator insert(const_iterator pos, std::initializer_list<uint8_t> init);

    /**
     * @brief Erase a byte at a specified location.
     * @param pos The position to erase.
     * @return The position immediatetly following the erased element.
     * @note All existing iterators and views may be invalidated.
     */
    iterator erase(iterator pos);
    iterator erase(const_iterator pos);

    /**
     * @brief Erase multiple bytes from a range at a specified location.
     * @param p1 The position of the first element to erase.
     * @param p2 The position of the last element (not to be erased itself).
     * @return The position immediatetly following the erased element.
     * @note All existing iterators and views may be invalidated.
     */
    iterator erase(iterator p1, iterator p2);
    iterator erase(const_iterator p1, const_iterator p2);

    /**
     * @brief Return the size of the byte storage.
     * @return The size of the storage.
     */
    size_t size() const;

    /**
     * @brief Return the capacity of the byte storage.
     * @return The capacity of the storage.
     */
    size_t capacity() const;

    /**
     * @brief Resize the data.
     * @param sz The new size.
     * @details
     * If the size is smaller than the current size, the data at the end of
     * the data is truncated. If the size is larger than the current size, the
     * data is appended with zero-based padding.
     */
    void resize(size_t sz);
    
    /**
     * @brief Return the iterator to the begin of the buffer.
     */
    iterator begin();
    const_iterator begin() const;
    const_iterator cbegin() const;

    /**
     * @brief Return the iterator to past the end of the buffer.
     */
    iterator end();
    const_iterator end() const;
    const_iterator cend() const;

    /**
     * @brief Return the reverse iterator to the end of the buffer.
     */
    reverse_iterator rbegin();
    const_reverse_iterator rbegin() const;
    const_reverse_iterator crbegin() const;

    /**
     * @brief Return the iterator to past the begin of the buffer.
     */
    reverse_iterator rend();
    const_reverse_iterator rend() const;
    const_reverse_iterator crend() const;

    /**
     * @brief Return the pointer to the underlying data.
     * @return Pointer to the begin of the string of bytes.
     */
    uint8_t* data();
    const uint8_t* data() const;

    /**
     * @brief Return the byte at the n-th position.
     * @param n The byte position (0 to size-1) to return, where 0 is the left
     * most position.
     * @return The byte at the position.
     */
    uint8_t operator[](size_t n) const;
    uint8_t& operator[](size_t n);

    /**
     * @brief Get the integral value at a certain position.
     * @tparam Type The integral type.
     * @param n The byte position of the first byte to the integral value.
     * @param from The endian system that the buffer stores the byte in.
     * @param to The endian system that the value should be converted to.
     * @return The integral value.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    Type get(size_t n, endian::endian_t from,
             endian::endian_t to = endian::endian_t::native) const
    {
        if (n + sizeof(Type) > size()) {
            throw std::range_error("index out of bounds");
        }
        return endian::convert(from, to,
                *(reinterpret_cast<const Type*>(data() + n)));
    }

    /**
     * @brief Get the integral value at a certain position.
     * @tparam Type The integral type.
     * @param pos The position of the first byte to the integral value.
     * @param from The endian system that the buffer stores the byte in.
     * @param to The endian system that the value should be converted to.
     * @return The integral value.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    Type get(const_iterator pos, endian::endian_t from,
             endian::endian_t to = endian::endian_t::native) const
    {
        return get<Type>(&*pos - data(), from, to);
    }

    /**
     * @brief Set the integral value at a certain position.
     * @tparam Type The integral type.
     * @param n The byte position of the first byte to the integral value.
     * @param value The intergal value to set.
     * @param to The endian system that the value should be converted to.
     * @param from The endian system that the @c value is provided as.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    void set(size_t n, Type value, endian::endian_t to,
             endian::endian_t from = endian::endian_t::native)
    {
        if (n + sizeof(Type) > size()) {
            throw std::range_error("index out of bounds");
        }
        *(reinterpret_cast<Type*>(data() + n)) =
                endian::convert(from, to, value);
    }

    /**
     * @brief Get the integral value at a certain position.
     * @tparam Type The integral type.
     * @param pos The position of the first byte to the integral value.
     * @param value The intergal value to set.
     * @param to The endian system that the value should be converted to.
     * @param from The endian system that the @c value is provided as.
     * @throw std::out_of_range The position does not have enough space
     * available to construct the integral value.
     */
    template<std::integral Type>
    void set(iterator pos, Type value, endian::endian_t to,
             endian::endian_t from = endian::endian_t::native)
    {
        return set<Type>(&*pos - data(), value, to, from);
    }

    /**
     * @brief Create a view of the entire buffer.
     * @return The view of the entire buffer.
     */
    buffer_view view() const;

    /**
     * @brief Create a view of any part of the buffer.
     * @param first The first position
     * @param last The last position (not inclusive).
     * @return The view from the first to the last position (not inclusive).
     */
    buffer_view view(iterator first, iterator last) const;
    buffer_view view(const_iterator first, const_iterator last) const;

    /**
     * @brief Bit-wise OR assignment on equal sized blocks.
     * @param rhs The data to OR with.
     * @return The reference to this object.
     */
    buffer& operator|=(const buffer& rhs);

    /**
     * @brief Bit-wise AND assignment on equal sized blocks.
     * @param rhs The data to AND with.
     * @return The reference to this object.
     */
    buffer& operator&=(const buffer& rhs);

    /**
     * @brief Bit-wise XOR assignment on equal sized blocks.
     * @param rhs The data to XOR with.
     * @return The reference to this object.
     */
    buffer& operator^=(const buffer& rhs);

    /**
     * @brief Bit-wise left-shift assignment.
     * @param pos The amount of bits to shift.
     * @return The reference to this object.
     */
    buffer& operator<<=(size_t pos);

    /**
     * @brief Bit-wise right-shift assignment.
     * @param pos The amount of bits to shift.
     * @return The reference to this object.
     */
    buffer& operator>>=(size_t pos);

    /**
     * @brief Equality operator.
     * @param rhs The data to compare against.
     * @return @c true if the data is equal.
     */
    bool operator==(const buffer&) const;

    /**
     * @brief In-equality operator.
     * @param rhs The data to compare against.
     * @return @c true if the data is not equal..
     */
    bool operator!=(const buffer&) const;

private:
    /**
     * The object holding the buffer data.
     */
    class holder;
    std::shared_ptr<holder> _holder;
 
    /** Friend operations */
    friend std::ostream& operator<<(std::ostream& ostr, const buffer& data);
    friend std::istream& operator>>(std::istream& istr, buffer& data);
    friend buffer operator|(const buffer& lhs, const buffer& rhs);
    friend buffer operator&(const buffer& lhs, const buffer& rhs);
    friend buffer operator^(const buffer& lhs, const buffer& rhs);
    friend buffer operator<<(const buffer& lhs, size_t pos);
    friend buffer operator>>(const buffer& lhs, size_t pos);
};

/**
 * @brief Write buffer data as hexadecimal digits.
 * @param ostr The output stream to write to.
 * @param data The data value to output.
 * @return The reference to @c ostr;
 */
std::ostream& operator<<(std::ostream& ostr, const buffer& data);

/**
 * @brief Write buffer view data as hexadecimal digits.
 * @param ostr The output stream to write to.
 * @param data The data value to output.
 * @return The reference to @c ostr;
 */
std::ostream& operator<<(std::ostream& ostr, const buffer_view& data);

/**
 * @brief Read hexadecimal digits to buffer data.
 * @param istr The input stream to write to.
 * @param data The data value to hold the data.
 * @return The reference to @c istr;
 */
std::istream& operator>>(std::istream& istr, const buffer& data);

/**
 * @brief Bitwise OR on two equal sized data blocks.
 * @param lhs The first data block
 * @param rhs The second data block
 * @return The result of the OR of @c lhs with @c rhs.
 * @details
 * Note that @c lhs should be smaller or equal to @c rhs, the result is always
 * the same size as @c lhs.
 */
buffer operator|(const buffer& lhs, const buffer& rhs);

/**
 * @brief Bitwise AND on two equal sized data blocks.
 * @param lhs The first data block
 * @param rhs The second data block
 * @return The result of the AND of @c lhs with @c rhs.
 * @details
 * Note that @c lhs should be smaller or equal to @c rhs, the result is always
 * the same size as @c lhs.
 */
buffer operator&(const buffer& lhs, const buffer& rhs);

/**
 * @brief Bitwise XOR on two equal sized data blocks.
 * @param lhs The first data block
 * @param rhs The second data block
 * @return The result of the XOR of @c lhs with @c rhs.
 * @details
 * Note that @c lhs should be smaller or equal to @c rhs, the result is always
 * the same size as @c lhs.
 */
buffer operator^(const buffer& lhs, const buffer& rhs);

/**
 * @brief Bitwise shift left.
 * @param lhs The data to shift.
 * @param pos The amount of bits to shift.
 * @return The shifted result.
 */
buffer operator<<(const buffer& lhs, size_t pos);

/**
 * @brief Bitwise shift right.
 * @param lhs The data to shift.
 * @param pos The amount of bits to shift.
 * @return The shifted result.
 */
buffer operator>>(const buffer& lhs, size_t pos);

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */

#endif /* _MUDLIB_CORE_BUFFER_H_ */
