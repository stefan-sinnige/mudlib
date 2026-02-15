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

#ifndef _MUDLIB_CORE_POLY_MAP_H_
#define _MUDLIB_CORE_POLY_MAP_H_

#include <map>
#include <memory>
#include <mud/core/ns.h>

BEGIN_MUDLIB_CORE_NS

/*
 * @brief Map of polymorphic types.
 *
 * A map that is able to hold polymorphic instances of a certain base type.
 */
template<class Key, class Type, class Compare = std::less<Key>,
         class Allocator =
             std::allocator<std::pair<const Key, std::shared_ptr<Type>>>>
class poly_map
  : private std::map<Key, std::shared_ptr<Type>, Compare, Allocator>
{
public:
    /**
     * Template parameter type definitions
     */
    typedef Key key_type;
    typedef Type value_type;
    typedef Compare key_compare;
    typedef Allocator allocator_type;

private:
    /**
     * Implementattion specific type definitions.
     */
    typedef std::shared_ptr<Type> storage_type;
    typedef std::map<key_type, storage_type, key_compare, allocator_type>
        impl_type;

public:
    /**
     * Member type definitions.
     */
    typedef typename impl_type::size_type size_type;
    typedef typename impl_type::difference_type difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef std::pair<key_type, value_type&> iterator_reference;
    typedef std::pair<key_type, const value_type&> const_iterator_reference;

    /**
     * @brief A bidirectional iterator accessing the elements in a forward
     * direction and returning references to the elements.
     */
    class iterator
      : private std::map<Key, std::shared_ptr<Type>, Compare,
                         Allocator>::iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Key key_type;
        typedef Type value_type;
        typedef Compare key_compare;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef typename std::map<Key, std::shared_ptr<Type>, Compare,
                                  Allocator>::iterator impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef std::pair<key_type, value_type&> reference;
        typedef std::pair<key_type, value_type&>* pointer;

        /**
         * Construct an iterator.
         */
        iterator() : _ptr(nullptr) {}

        /**
         * Destruct the iterator.
         */
        virtual ~iterator() { delete _ptr; }

        /**
         * Return a reference to the object pointed to by the iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return std::pair<key_type, value_type&>(ref.first, *(ref.second));
        }

        /**
         * Return a pointer to the object pointed to by the iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            _ptr =
                new std::pair<key_type, value_type&>(ref.first, *(ref.second));
            return _ptr;
        }

        /**
         * Return true if the two iterators reference the same element.
         * @param[in] lhs The first iterator.
         * @param[in] lhs The second iterator.
         */
        friend bool operator==(const iterator& lhs, const iterator& rhs)
        {
            return lhs._iter == rhs._iter;
        }

        /**
         * Return true if the two iterators do no reference the same element.
         * @param[in] lhs The first iterator.
         * @param[in] lhs The second iterator.
         */
        friend bool operator!=(const iterator& lhs, const iterator& rhs)
        {
            return lhs._iter != rhs._iter;
        }

        /**
         * Increase the iterator to the next element.
         */
        iterator& operator++()
        {
            ++_iter;
            return *this;
        }
        iterator operator++(int)
        {
            iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        /**
         * Decrease the iterator to the previous element.
         */
        iterator& operator--()
        {
            --_iter;
            return *this;
        }
        iterator operator--(int)
        {
            iterator tmp(*this);
            --(*this);
            return tmp;
        }

    private:
        /**
         * Construct an iterator and initialise it with the implementation
         * dependent iterator.
         */
        iterator(impl_type impl) : _iter(impl), _ptr(nullptr) {}

        /**
         * The implementation dependent iterator.
         */
        impl_type _iter;

        /**
         * Reference to the current object (used by operator-> exclusively).
         */
        pointer _ptr;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_map;
    };

    /**
     * @brief A bidirectional iterator accessing the elements in a forward
     * direction and returning constant references to the elements.
     */
    class const_iterator
      : private std::map<Key, std::shared_ptr<Type>, Compare,
                         Allocator>::const_iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Key key_type;
        typedef Type value_type;
        typedef Compare key_compare;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef typename std::map<Key, std::shared_ptr<Type>, Compare,
                                  Allocator>::const_iterator impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef std::pair<key_type, const value_type&> reference;
        typedef std::pair<key_type, const value_type&>* pointer;

        /**
         * Construct an iterator.
         */
        const_iterator() : _ptr(nullptr) {}

        /**
         * Construct a constant iterator from a non-constant iterator.
         */
        const_iterator(iterator rhs) : _iter(rhs._iter), _ptr(nullptr) {}

        /**
         * Destruct the iterator.
         */
        virtual ~const_iterator() { delete _ptr; }

        /**
         * Return a reference to the object pointed to by the iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return std::pair<key_type, const value_type&>(ref.first,
                                                          *(ref.second));
        }

        /**
         * Return a pointer to the object pointed to by the iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            _ptr = new std::pair<key_type, const value_type&>(ref.first,
                                                              *(ref.second));
            return _ptr;
        }

        /**
         * Return true if the two iterators reference the same element.
         * @param[in] lhs The first iterator.
         * @param[in] lhs The second iterator.
         */
        friend bool operator==(const const_iterator& lhs,
                               const const_iterator& rhs)
        {
            return lhs._iter == rhs._iter;
        }

        /**
         * Return true if the two iterators do no reference the same element.
         * @param[in] lhs The first iterator.
         * @param[in] lhs The second iterator.
         */
        friend bool operator!=(const const_iterator& lhs,
                               const const_iterator& rhs)
        {
            return lhs._iter != rhs._iter;
        }

        /**
         * Increase the iterator to the next element.
         */
        const_iterator& operator++()
        {
            ++_iter;
            return *this;
        }
        const_iterator operator++(int)
        {
            const_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        /**
         * Decrease the iterator to the previous element.
         */
        const_iterator& operator--()
        {
            --_iter;
            return *this;
        }
        const_iterator operator--(int)
        {
            const_iterator tmp(*this);
            --(*this);
            return tmp;
        }

    private:
        /**
         * Construct an iterator and initialise it with the implementation
         * dependent iterator.
         */
        const_iterator(impl_type impl) : _iter(impl), _ptr(nullptr) {}

        /**
         * The implementation dependent iterator.
         */
        impl_type _iter;

        /**
         * Reference to the current object (used by operator-> exclusively).
         */
        pointer _ptr;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_map;
    };

    /**
     * @brief A bidirectional iterator accessing the elements in a reverse
     * direction and returning references to the elements.
     */
    class reverse_iterator
      : private std::map<Key, std::shared_ptr<Type>, Compare,
                         Allocator>::reverse_iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Key key_type;
        typedef Type value_type;
        typedef Compare key_compare;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef typename std::map<Key, std::shared_ptr<Type>, Compare,
                                  Allocator>::reverse_iterator impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef std::pair<key_type, value_type&> reference;
        typedef std::pair<key_type, value_type&>* pointer;

        /**
         * Construct an reverse iterator.
         */
        reverse_iterator() : _ptr(nullptr) {}

        /**
         * Destruct the iterator.
         */
        virtual ~reverse_iterator() { delete _ptr; }

        /**
         * Return a reference to the object pointed to by the reverse iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return std::pair<key_type, value_type&>(ref.first, *(ref.second));
        }

        /**
         * Return a pointer to the object pointed to by the reverse iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            _ptr =
                new std::pair<key_type, value_type&>(ref.first, *(ref.second));
            return _ptr;
        }

        /**
         * Return true if the two reverse iterators reference the same element.
         * @param[in] lhs The first reverse iterator.
         * @param[in] lhs The second reverse iterator.
         */
        friend bool operator==(const reverse_iterator& lhs,
                               const reverse_iterator& rhs)
        {
            return lhs._iter == rhs._iter;
        }

        /**
         * Return true if the two reverse iterators do no reference the same
         * element.
         * @param[in] lhs The first reverse iterator.
         * @param[in] lhs The second reverse iterator.
         */
        friend bool operator!=(const reverse_iterator& lhs,
                               const reverse_iterator& rhs)
        {
            return lhs._iter != rhs._iter;
        }

        /**
         * Increase the reverse iterator to the next element.
         */
        reverse_iterator& operator++()
        {
            ++_iter;
            return *this;
        }
        reverse_iterator operator++(int)
        {
            reverse_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        /**
         * Decrease the reverse iterator to the previous element.
         */
        reverse_iterator& operator--()
        {
            --_iter;
            return *this;
        }
        reverse_iterator operator--(int)
        {
            reverse_iterator tmp(*this);
            --(*this);
            return tmp;
        }

    private:
        /**
         * Construct a reverse iterator and initialise it with the
         * implementation dependent reverse iterator.
         */
        reverse_iterator(impl_type impl) : _iter(impl), _ptr(nullptr) {}

        /**
         * The implementation dependent reverse_iterator.
         */
        impl_type _iter;

        /**
         * Reference to the current object (used by operator-> exclusively).
         */
        pointer _ptr;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_map;
    };

    /**
     * @brief A bidirectional iterator accessing the elements in a reverse
     * direction and returning constant references to the elements.
     */
    class const_reverse_iterator
      : private std::map<Key, std::shared_ptr<Type>, Compare,
                         Allocator>::const_reverse_iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Key key_type;
        typedef Type value_type;
        typedef Compare key_compare;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef typename std::map<Key, std::shared_ptr<Type>, Compare,
                                  Allocator>::const_reverse_iterator impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef std::pair<key_type, const value_type&> reference;
        typedef std::pair<key_type, const value_type&>* pointer;

        /**
         * Construct an reverse iterator.
         */
        const_reverse_iterator() : _ptr(nullptr) {}

        /**
         * Construct a constant iterator from a non-constant iterator.
         */
        const_reverse_iterator(reverse_iterator rhs)
          : _iter(rhs._iter), _ptr(nullptr)
        {}

        /**
         * Destruct the iterator.
         */
        virtual ~const_reverse_iterator() { delete _ptr; }

        /**
         * Return a reference to the object pointed to by the reverse iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return std::pair<key_type, const value_type&>(ref.first,
                                                          *(ref.second));
        }

        /**
         * Return a pointer to the object pointed to by the reverse iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            _ptr = new std::pair<key_type, const value_type&>(ref.first,
                                                              *(ref.second));
            return _ptr;
        }

        /**
         * Return true if the two reverse iterators reference the same element.
         * @param[in] lhs The first reverse iterator.
         * @param[in] lhs The second reverse iterator.
         */
        friend bool operator==(const const_reverse_iterator& lhs,
                               const const_reverse_iterator& rhs)
        {
            return lhs._iter == rhs._iter;
        }

        /**
         * Return true if the two reverse iterators do no reference the same
         * element.
         * @param[in] lhs The first reverse iterator.
         * @param[in] lhs The second reverse iterator.
         */
        friend bool operator!=(const const_reverse_iterator& lhs,
                               const const_reverse_iterator& rhs)
        {
            return lhs._iter != rhs._iter;
        }

        /**
         * Increase the reverse_iterator to the next element.
         */
        const_reverse_iterator& operator++()
        {
            ++_iter;
            return *this;
        }
        const_reverse_iterator operator++(int)
        {
            const_reverse_iterator tmp(*this);
            ++(*this);
            return tmp;
        }

        /**
         * Decrease the reverse_iterator to the previous element.
         */
        const_reverse_iterator& operator--()
        {
            --_iter;
            return *this;
        }
        const_reverse_iterator operator--(int)
        {
            const_reverse_iterator tmp(*this);
            --(*this);
            return tmp;
        }

    private:
        /**
         * Construct an reverse iterator and initialise it with the
         * implementation dependent reverse iterator.
         */
        const_reverse_iterator(impl_type impl) : _iter(impl), _ptr(nullptr) {}

        /**
         * The implementation dependent reverse_iterator.
         */
        impl_type _iter;

        /**
         * Reference to the current object (used by operator-> exclusively).
         */
        pointer _ptr;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_map;
    };

    /**
     * @brief Constructs a map.
     */
    poly_map() = default;

    /**
     * @brief Constructs a new map by copying the contents from another one.
     * @param[in] rhs The map to copy the contents from.
     */
    poly_map(const poly_map& rhs) : impl_type(rhs){};

    /**
     * @brief Assign the contents of another polymorphic map.
     * @param[in] rhs The map to copy the contents from.
     */
    poly_map& operator=(const poly_map& rhs)
    {
        (void)impl_type::operator=(rhs);
        return *this;
    }

    /**
     * @brief Destructs the map.
     */
    virtual ~poly_map() = default;

    /**
     * Non-moveable.
     */
    poly_map(poly_map&&) = delete;
    poly_map& operator=(poly_map&&) = delete;

    /**
     * @brief Return the number of elements in the map.
     */
    size_t size() const { return impl_type::size(); }

    /**
     * @brief Checks whether the map is empty.
     */
    bool empty() const { return impl_type::empty(); }

    /**
     * @brief Clear the contents.
     */
    void clear() { return impl_type::clear(); }

    /**
     * @brief Insert an element in the map that is indexed by a key. If the
     * key already exists, the mapped element is destructed and overwritten.
     * @tparam[in] PolyType The type of the value.
     * @param[in] key The key to find the value.
     * @param[in] value The value to insert.
     * @return The reference to the new value.
     * @throws std::bad_type_id Value is not of type @c PolyType
     */
    template<class PolyType>
    PolyType& insert(const key_type& key, const PolyType& value)
    {
        typename impl_type::iterator found = impl_type::find(key);
        if (found != impl_type::end()) {
            impl_type::erase(found);
        }
        auto res = impl_type::emplace(key, std::make_shared<PolyType>(value));
        if (!res.second) {
            throw std::bad_alloc();
        }
        return dynamic_cast<PolyType&>(*(res.first->second));
    }

    /**
     * @brief Insert an element in the map that is indexed by a key. If the
     * key already exists, the mapped element is destructed and overwritten.
     * @tparam[in] PolyType The type of the value.
     * @param[in] key The key to find the value.
     * @param[in] value The value to insert.
     * @return The reference to the new value.
     * @throws std::bad_type_id Value is not of type @c PolyType
     */
    /*
    template<class PolyType>
    PolyType& insert(const key_type& key, PolyType&& value)
    {
        typename impl_type::iterator found = impl_type::find(key);
        if (found != impl_type::end()) {
            impl_type::erase(found);
        }
        auto res = impl_type::emplace(
            key, std::make_shared<PolyType>(std::move(value)));
        if (!res.second) {
            throw std::bad_alloc();
        }
        return dynamic_cast<PolyType&>(*(res.first->second));
    }
    */

    /**
     * @brief Erase an element with matching key.
     * @param[in] key The key value of element to remove.
     * @returns The number of items removed (0 or 1).
     */
    size_type erase(const key_type& key) { return impl_type::erase(key); }

    /**
     * @brief Erase an element as a certain position.
     * @param[in] pos The position to erase.
     * @returns Iterator to the next position.
     */
    iterator erase(iterator pos)
    {
        auto new_pos = impl_type::erase(pos._iter);
        return iterator(new_pos);
    }

    /**
     * @brief Erase an element as a certain position.
     * @param[in] pos The position to erase.
     * @returns Iterator to the next position.
     */
    iterator erase(const_iterator pos)
    {
        auto new_pos = impl_type::erase(pos._iter);
        return iterator(new_pos);
    }

    /**
     * @brief Erase a range of elements.
     * @param[in] first The position of the first element to erase.
     * @param[in] last The position of the last element to erase.
     * @returns Iterator to the next position.
     */
    iterator erase(const_iterator first, const_iterator last)
    {
        auto new_pos = impl_type::erase(first._iter, last._iter);
        return iterator(new_pos);
    }

    /**
     * @brief Returns a reference to the value that is indexed by the key.
     * @param[in] key The key to find the value.
     * @return The reference to the value.
     * @throws std::out_of_range Key is not present in the map
     */
    reference at(const key_type& key) { return *(impl_type::at(key)); }

    /**
     * @brief Returns a reference to the value that is indexed by the key.
     * @param[in] key The key to find the value.
     * @return The reference to the value.
     * @throws std::out_of_range Key is not present in the map
     */
    const_reference at(const key_type& key) const
    {
        return *(impl_type::at(key));
    }

    /**
     * @brief Find an element with a matching key.
     * @returns Iterator to the matching element, or a past-the-end iterator
     * if an element with matching key is not found.
     */
    iterator find(const key_type& key)
    {
        return iterator(impl_type::find(key));
    }

    /**
     * @brief Find an element with a matching key.
     * @returns Iterator to the matching element, or a past-the-end iterator
     * if an element with matching key is not found.
     */
    const_iterator find(const key_type& key) const
    {
        return const_iterator(impl_type::find(key));
    }

    /**
     * Return an iterator to the start of the map.
     */
    iterator begin() { return iterator(impl_type::begin()); }
    const_iterator begin() const { return const_iterator(impl_type::cbegin()); }

    /**
     * Return an iterator to the element following the end of the map.
     */
    iterator end() { return iterator(impl_type::end()); }
    const_iterator end() const { return const_iterator(impl_type::cend()); }

    /**
     * Return an iterator to the start of the map.
     */
    const_iterator cbegin() const
    {
        return const_iterator(impl_type::cbegin());
    }

    /**
     * Return an iterator to the element following the end of the map.
     */
    const_iterator cend() const { return const_iterator(impl_type::cend()); }

    /**
     * Return a reverse iterator to the start of the map.
     */
    reverse_iterator rbegin() { return reverse_iterator(impl_type::rbegin()); }

    /**
     * Return a reverse iterator to the element following the end of the map.
     */
    reverse_iterator rend() { return reverse_iterator(impl_type::rend()); }

    /**
     * Return a reveerse iterator to the start of the map.
     */
    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(impl_type::crbegin());
    }

    /**
     * Return a reverse iterator to the element following the end of the map.
     */
    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(impl_type::crend());
    }
};

END_MUDLIB_CORE_NS

/* vi: set expandtab ai ts=4: */

#endif /*  _MUDLIB_CORE_POLY_MAP_H_ */
