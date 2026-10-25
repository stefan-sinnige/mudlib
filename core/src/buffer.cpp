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

#include "mud/core/buffer.h"
#include <cstring>
#include <iomanip>
#include <stdexcept>

BEGIN_MUDLIB_CORE_NS

/* ======================================================================
 * Buffer view
 * ====================================================================== */

buffer_view::iterator
buffer_view::begin() {
    return iterator(_pos);
}

buffer_view::const_iterator 
buffer_view::begin() const {
    return const_iterator(_pos);
}

buffer_view::const_iterator 
buffer_view::cbegin() const {
    return const_iterator(_pos);
}

buffer_view::iterator
buffer_view::end() {
    return iterator(_pos + _sz);
}

buffer_view::const_iterator 
buffer_view::end() const {
    return const_iterator(_pos + _sz);
}

buffer_view::const_iterator 
buffer_view::cend() const {
    return const_iterator(_pos + _sz);
}

buffer_view::reverse_iterator
buffer_view::rbegin() {
    return reverse_iterator(_pos + _sz - 1);
}

buffer_view::const_reverse_iterator 
buffer_view::rbegin() const {
    return const_reverse_iterator(_pos + _sz - 1);
}

buffer_view::const_reverse_iterator 
buffer_view::crbegin() const {
    return const_reverse_iterator(_pos + _sz - 1);
}

buffer_view::reverse_iterator
buffer_view::rend() {
    return reverse_iterator(_pos - 1);
}

buffer_view::const_reverse_iterator 
buffer_view::rend() const {
    return const_reverse_iterator(_pos - 1);
}

buffer_view::const_reverse_iterator 
buffer_view::crend() const {
    return const_reverse_iterator(_pos - 1);
}

uint8_t
buffer_view::operator[](size_t n) const
{
    if (n >= _sz) {
        throw std::out_of_range("index out of bounds");
    }
    return *(_pos + n);
}

uint8_t&
buffer_view::operator[](size_t n)
{
    if (n >= _sz) {
        throw std::out_of_range("index out of bounds");
    }
    return *(_pos + n);
}

buffer_view
buffer_view::view(buffer_view::iterator first, buffer_view::iterator last) const
{
    return buffer_view(first._ptr, last._ptr - first._ptr);
}

buffer_view
buffer_view::view(buffer_view::const_iterator first,
                  buffer_view::const_iterator last) const
{
    return buffer_view(const_cast<uint8_t*>(first._ptr),
                       last._ptr - first._ptr);
}

/* ======================================================================
 * Buffer holder
 * ====================================================================== */

/**
 * @brief Buffer data holder
 * @details
 * This object holds the actual buffer buffer data, which may be shared
 * between multiple @c buffer instances. Note that the actual allocated
 * size is one byte larger than requested to ensure that an iterator end
 * pointer still refers to actual, but unused or unreachable memory.
 */
class buffer::holder
{
public:
    /**
     * @brief Create a buffer buffer holding an initial amount of bytes.
     * @param sz The size (in bytes) of the buffer data.
     */
    holder(size_t sz);

    /**
     * @brief Create a buffer buffer holding an initial buffer stream.
     * @param buf The buffer to the buffer data.
     * @param sz The size (in bytes) of the buffer data.
     * @details
     * The buffer buffer will be copied.
     */
    holder(const uint8_t* buf, size_t sz);

    /**
     * @brief Create a buffer buffer through an initialiser list.
     * @param init The initialiser list, as a container of @c uint8_t elements.
     */
    holder(std::initializer_list<uint8_t> init);

    /**
     * @brief Delete the buffer buffer.
     */
    ~holder();

    /**
     * @brief Return a pointer to the sart of the buffer.
     */
    const uint8_t* data() const { return _data; }
    uint8_t* data() { return _data; }

    /**
     * @brief Return the size of the data.
     */
    size_t size() const { return _sz; }

    /**
     * @brief Return the capacity of the data.
     */
    size_t capacity() const { return _capacity; }

    /**
     * @brief Resize.
     * @param sz The new size.
     * @details
     * Adjust the size of the buffer data. If the capacity has to increase,
     * the buffer is re-allocated and all iterators are invalidated. If the
     * size is smaller, then only the first @c sz bytes are retained.
     */
    void resize(size_t sz);

    /**
     * @brief Clear the buffer buffer.
     */
    void clear() { _sz = 0; }

    /**
     * @brief Insert multiple bytes at a specified location.
     * @param pos The position to insert to.
     * @param count The number of bytes to insert.
     * @param value The value of each byte.
     * @return The location of the first inserted element.
     */
    uint8_t* insert(const uint8_t* pos, size_t count, uint8_t value);

    /**
     * @brief Insert multiple bytes from a range at a specified location.
     * @param pos The position to insert to.
     * @param ptr The position of the first element to copy.
     * @param count The number of elements to copy.
     * @return The location of the first inserted element.
     */
    uint8_t* insert(const uint8_t* pos, const uint8_t* ptr, size_t count);

    /**
     * @brief Insert multiple bytes from an initialiser list at a specified
     * location.
     * @param pos The position to insert to.
     * @param init The initialiser list to insert.
     * @return The location of the first inserted element.
     */
    uint8_t* insert(const uint8_t* pos, std::initializer_list<uint8_t> init);

    /**
     * @brief Erase a bte at a specified location.
     * @param pos The position to remove to.
     * @param count The number of bytes to remove.
     * @return The location of the next element.
     */
    uint8_t* erase(const uint8_t* pos, size_t count);

private:
    /** The buffer buffer capacity */
    size_t _capacity = 0;

    /** The buffer buffer. */
    uint8_t* _data = nullptr;

    /** The buffer size in bytes */
    size_t _sz = 0;
};

buffer::holder::holder(size_t sz)
{
    resize(sz);
    
}

buffer::holder::holder(const uint8_t* buf, size_t sz)
{
    resize(sz);
    ::memcpy(_data, buf, sz);
}

buffer::holder::holder(std::initializer_list<uint8_t> init)
{
    resize(init.size());
    uint8_t* cptr = _data;
    for (uint8_t byte: init) {
        *(cptr++) = byte;
    }
}

buffer::holder::~holder()
{
    delete[] _data;
}

void
buffer::holder::resize(size_t sz)
{
    if (sz > _sz) {
        if ((sz + 1) > _capacity) {
            /* Allocate a new buffer and extend it to the next boundary */
            constexpr size_t boundary = 16;
            _capacity = sz + (boundary - sz % boundary);
            auto data = new uint8_t[_capacity];

            /* Copy old data and zero-ise the additional buffer. If this was
             * the first buffer, zero-ise the entire buffer. */
            if (_data != nullptr) {
                memcpy(data, _data, _sz);
                delete[] _data;
                memset(data + _sz, 0, _capacity - _sz - 1);
            }
            else {
                memset(data, 0, _capacity);
            }
            _data = data;
        }
    }
    _sz = sz;
}

uint8_t*
buffer::holder::insert(const uint8_t* pos, size_t count, uint8_t value)
{
    /* Bail out if nothing to do */
    if (count == 0) {
        return const_cast<uint8_t*>(pos);;
    }

    /* Resize the buffer to hold the additional data. */
    size_t offset = pos - _data;
    size_t orig_size = _sz;
    resize(_sz + count);

    /* Move data after the 'dst' to make room and fill the gap. */
    uint8_t* dst = _data + offset;
    memmove(dst + count, dst, orig_size - offset);
    memset(dst, value, count);
    return dst;
}

uint8_t*
buffer::holder::insert(const uint8_t* pos, const uint8_t* ptr, size_t count)
{
    /* Bail out if nothing to do */
    if (count == 0) {
        return const_cast<uint8_t*>(pos);;
    }

    /* Resize the buffer to hold the additional data. */
    size_t offset = pos - _data;
    size_t orig_size = _sz;
    resize(_sz + count);

    /* Move data after the 'dst' to make room and fill the gap. */
    uint8_t* dst = _data + offset;
    memmove(dst + count, dst, orig_size - offset);
    memmove(dst, ptr, count);
    return dst;
}

uint8_t*
buffer::holder::insert(const uint8_t* pos, std::initializer_list<uint8_t> init)
{
    /* Bail out if nothing to do */
    if (init.size() == 0) {
        return const_cast<uint8_t*>(pos);;
    }

    /* Resize the buffer to hold the additional data. */
    size_t offset = pos - _data;
    size_t orig_size = _sz;
    resize(_sz + init.size());

    /* Move data after the 'dst' to make room and fill the gap. */
    uint8_t* dst = _data + offset;
    memmove(dst + init.size(), dst, orig_size - offset);
    for (uint8_t value: init) {
        *dst++ = value;
    }
    return dst;
}

uint8_t*
buffer::holder::erase(const uint8_t* pos, size_t count)
{
    /* Bail out if nothing to do */
    if (count == 0) {
        return const_cast<uint8_t*>(pos);;
    }

    /* Move the data to remove the bytes */
    size_t offset = pos - _data;
    uint8_t* dst = (uint8_t*)pos;
    memmove(dst, dst + count, _sz - offset - count);
    _sz -= count;
    return dst;
}

/* ======================================================================
 * Buffer
 * ====================================================================== */

buffer::buffer(size_t sz)
    : _holder(std::make_shared<buffer::holder>(sz))
{
}

buffer::buffer(std::initializer_list<uint8_t> init)
    : _holder(std::make_shared<buffer::holder>(init))
{
}

buffer::buffer(const uint8_t* buf, size_t sz)
    : _holder(std::make_shared<buffer::holder>(buf, sz))
{
}

buffer::buffer(const buffer& rhs)
    : _holder(std::make_shared<buffer::holder>(rhs.data(), rhs.size()))
{
}

buffer::buffer(const buffer_view& view)
    : _holder(std::make_shared<buffer::holder>(view.data(), view.size()))
{
}

buffer&
buffer::operator=(const buffer& rhs)
{
    if (&rhs == this) {
        return *this;
    }
    _holder = std::make_shared<buffer::holder>(rhs.data(), rhs.size());
    return *this;
}

buffer&
buffer::operator=(const buffer_view& rhs)
{
    _holder = std::make_shared<buffer::holder>(rhs.data(), rhs.size());
    return *this;
}

void
buffer::clear()
{
    _holder->clear();
}

buffer::iterator
buffer::insert(buffer::iterator pos, uint8_t value)
{
    return _holder->insert(pos._ptr, 1, value);
}

buffer::iterator
buffer::insert(buffer::const_iterator pos, uint8_t value)
{
    return _holder->insert(pos._ptr, 1, value);
}

buffer::iterator
buffer::insert(buffer::iterator pos, size_t count, uint8_t value)
{
    return _holder->insert(pos._ptr, count, value);
}

buffer::iterator
buffer::insert(buffer::const_iterator pos, size_t count, uint8_t value)
{
    return _holder->insert(pos._ptr, count, value);
}

buffer::iterator
buffer::insert(buffer::iterator pos, buffer::iterator first,
               buffer::iterator last)
{
    return _holder->insert(pos._ptr, first._ptr, last._ptr - first._ptr);
}

buffer::iterator
buffer::insert(buffer::const_iterator pos, buffer::const_iterator first,
               buffer::const_iterator last)
{
    return _holder->insert(pos._ptr, first._ptr, last._ptr - first._ptr);
}

buffer::iterator
buffer::insert(buffer::iterator pos, const uint8_t* ptr, size_t count)
{
    return _holder->insert(pos._ptr, ptr, count);
}

buffer::iterator
buffer::insert(buffer::const_iterator pos, const uint8_t* ptr, size_t count)
{
    return _holder->insert(pos._ptr, ptr, count);
}

buffer::iterator
buffer::insert(buffer::iterator pos, std::initializer_list<uint8_t> init)
{
    return _holder->insert(pos._ptr, init);
}

buffer::iterator
buffer::insert(buffer::const_iterator pos, std::initializer_list<uint8_t> init)
{
    return _holder->insert(pos._ptr, init);
}

buffer::iterator
buffer::erase(buffer::iterator pos)
{
    return _holder->erase(pos._ptr, 1);
}

buffer::iterator
buffer::erase(buffer::const_iterator pos)
{
    return _holder->erase(pos._ptr, 1);
}

buffer::iterator
buffer::erase(buffer::iterator first, buffer::iterator last)
{
    return _holder->erase(first._ptr, last._ptr - first._ptr);
}

buffer::iterator
buffer::erase(buffer::const_iterator first, buffer::const_iterator last)
{
    return _holder->erase(first._ptr, last._ptr - first._ptr);
}

size_t
buffer::size() const
{
    return _holder->size();
}

size_t
buffer::capacity() const
{
    return _holder->capacity();
}

void
buffer::resize(size_t sz)
{
    _holder->resize(sz);
}

buffer::iterator
buffer::begin() {
    return iterator(_holder->data());
}

buffer::const_iterator 
buffer::begin() const {
    return const_iterator(_holder->data());
}

buffer::const_iterator 
buffer::cbegin() const {
    return const_iterator(_holder->data());
}

buffer::iterator
buffer::end() {
    return iterator(_holder->data() + _holder->size());
}

buffer::const_iterator 
buffer::end() const {
    return const_iterator(_holder->data() + _holder->size());
}

buffer::const_iterator 
buffer::cend() const {
    return const_iterator(_holder->data() + _holder->size());
}

buffer::reverse_iterator
buffer::rbegin() {
    return reverse_iterator(_holder->data() + _holder->size() - 1);
}

buffer::const_reverse_iterator 
buffer::rbegin() const {
    return const_reverse_iterator(_holder->data() + _holder->size() - 1);
}

buffer::const_reverse_iterator 
buffer::crbegin() const {
    return const_reverse_iterator(_holder->data() + _holder->size() - 1);
}

buffer::reverse_iterator
buffer::rend() {
    return reverse_iterator(_holder->data() - 1);
}

buffer::const_reverse_iterator 
buffer::rend() const {
    return const_reverse_iterator(_holder->data() - 1);
}

buffer::const_reverse_iterator 
buffer::crend() const {
    return const_reverse_iterator(_holder->data() - 1);
}

uint8_t*
buffer::data() {
    return _holder->data();
}

const uint8_t*
buffer::data() const {
    return _holder->data();
}

uint8_t
buffer::operator[](size_t n) const
{
    if (n >= _holder->size()) {
        throw std::out_of_range("index out of bounds");
    }
    return *(_holder->data() + n);
}

uint8_t&
buffer::operator[](size_t n)
{
    if (n >= _holder->size()) {
        throw std::out_of_range("index out of bounds");
    }
    return *(_holder->data() + n);
}

buffer_view
buffer::view() const
{
    return view(cbegin(), cend());
}

buffer_view
buffer::view(buffer::iterator first, buffer::iterator last) const
{
    return buffer_view(first._ptr, last._ptr - first._ptr);
}

buffer_view
buffer::view(buffer::const_iterator first, buffer::const_iterator last) const
{
    return buffer_view(const_cast<uint8_t*>(first._ptr),
                       last._ptr - first._ptr);
}

buffer&
buffer::operator|=(const buffer& rhs)
{
    if (size() > rhs.size()) {
        throw std::out_of_range("right operand too small");
    }
    auto sptr = rhs.data();
    auto dptr = data();
    auto eptr = data() + size();
    while (dptr < eptr) {
        *dptr++ |= *sptr++;
    }
    return *this;
}

buffer&
buffer::operator&=(const buffer& rhs)
{
    if (size() > rhs.size()) {
        throw std::out_of_range("right operand too small");
    }
    auto sptr = rhs.data();
    auto dptr = data();
    auto eptr = data() + size();
    while (dptr < eptr) {
        *dptr++ &= *sptr++;
    }
    return *this;
}

buffer&
buffer::operator^=(const buffer& rhs)
{
    if (size() > rhs.size()) {
        throw std::out_of_range("right operand too small");
    }
    auto sptr = rhs.data();
    auto dptr = data();
    auto eptr = data() + size();
    while (dptr < eptr) {
        *dptr++ ^= *sptr++;
    }
    return *this;
}

buffer&
buffer::operator<<=(size_t pos)
{
    // Shifting more than the size leaves a zero'ed buffer.
    if (pos >= size()*8) {
        std::fill(begin(), end(), 0);
        return *this;
    }

    // Calculate the number of bytes and remaining bits to shift.
    size_t bytes = pos / 8;
    size_t bits = pos % 8;

    // Shift whole bytes first
    if (bytes != 0) {
        for (int index = 0; index < (size() - bytes); ++index) {
            (*this)[index] = (*this)[index + bytes];
        }
        for (int index = size() - bytes; index < size(); ++index) {
            (*this)[index] = 0;
        }
    }   
    
    // Shift the remaining bits, and carry-over a mask of the MSB bits from next
    // byte.
    for (int index = 0; index < size(); ++index) {
        (*this)[index] <<= bits;
        if (index < (size() - 1)) {
            (*this)[index] |= (*this)[index+1] >> (8-bits);
        }
    }
    return *this;
}

buffer&
buffer::operator>>=(size_t pos)
{
    // Shifting more than the size leaves a zero'ed buffer.
    if (pos >= size()*8) {
        std::fill(begin(), end(), 0);
        return *this;
    }

    // Calculate the number of bytes and remaining bits to shift.
    size_t bytes = pos / 8;
    size_t bits = pos % 8;

    // Shift whole bytes first
    if (bytes != 0) {
        for (int index = (size() - 1); index >= bytes; --index) {
            (*this)[index] = (*this)[index - bytes];
        }
        for (int index = 0; index < bytes; ++index) {
            (*this)[index] = 0;
        }
    }   
    
    // Shift the remaining bits, and carry-over a mask of the LSB bits from the
    // previous byte.
    for (int index = (size() - 1); index >= 0; --index) {
        (*this)[index] >>= bits;
        if (index > 0) {
            (*this)[index] |= (*this)[index-1] << (8-bits);
        }
    }
    return *this;
}

bool
buffer::operator==(const buffer& rhs) const
{
    if (size() == rhs.size()) {
        for (size_t i = 0; i < size(); ++i) {
            if ((*this)[i] != rhs[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}


bool
buffer::operator!=(const buffer& rhs) const
{
    return ! operator==(rhs);
}

std::ostream&
operator<<(std::ostream& ostr, const buffer& d)
{
    auto cptr = d.data();
    auto eptr = d.data() + d.size();
    auto saved_flags = ostr.flags();
    ostr << std::hex << std::setfill('0');
    while (cptr < eptr) {
        ostr << std::setw(2) << (int)*cptr;
        ++cptr;
    }
    ostr.flags(saved_flags);
    return ostr;
}

std::ostream&
operator<<(std::ostream& ostr, const buffer_view& d)
{
    auto cptr = d.data();
    auto eptr = d.data() + d.size();
    auto saved_flags = ostr.flags();
    ostr << std::hex << std::setfill('0');
    while (cptr < eptr) {
        ostr << std::setw(2) << (int)*cptr;
        ++cptr;
    }
    ostr.flags(saved_flags);
    return ostr;
}

std::istream&
operator>>(std::istream& istr, buffer& d)
{
    /* Define a simple hex-digit to a nibble */
    auto hex2nibble = [](char d) -> int {
        if (d >= '0' && d <= '9') {
            return d - '0';
        }
        else 
        if (d >= 'a' && d <= 'f') {
            return d - 'a' + 10;
        }
        else
        if (d >= 'A' && d <= 'F') {
            return d - 'A' + 10;
        }
        return -1;
    };

    /* Work on 16-bytes temporary storage, only add a byte if two nibbles have
     * been read and processed correctly. */
    uint8_t tmp[16];
    size_t idx = 0;
    while (true) {
        /* Read next two characters from the stream */
        int dh = istr.get();
        if (dh == std::istream::traits_type::eof()) {
            break;
        }
        int dl = istr.get();
        if (dl == std::istream::traits_type::eof()) {
            istr.putback(dh);
            break;
        }

        /* Check if they are hexadecimal digits */
        int nh = hex2nibble(dh);
        int nl = hex2nibble(dl);
        if (nh == -1 || nl == -1) {
            istr.putback(dl);
            istr.putback(dh);
            break;
        }

        /* Append it */
        tmp[idx++] = (uint8_t)(nh << 4 | nl);
        if (idx == 16) {
            d.insert(d.cend(), tmp, idx);
            idx = 0;
        }
    }
    d.insert(d.cend(), tmp, idx);
    return istr;
}

buffer
operator|(const buffer& lhs, const buffer& rhs)
{
    buffer result = lhs;
    result |= rhs;
    return result;
}

buffer
operator&(const buffer& lhs, const buffer& rhs)
{
    buffer result = lhs;
    result &= rhs;
    return result;
}

buffer
operator^(const buffer& lhs, const buffer& rhs)
{
    buffer result = lhs;
    result ^= rhs;
    return result;
}

buffer
operator<<(const buffer& lhs, size_t pos)
{
    buffer result = lhs;
    result <<= pos;
    return result;
}

buffer
operator>>(const buffer& lhs, size_t pos)
{
    buffer result = lhs;
    result >>= pos;
    return result;
}

END_MUDLIB_CORE_NS

/* vi: set ai ts=4 expandtab: */
