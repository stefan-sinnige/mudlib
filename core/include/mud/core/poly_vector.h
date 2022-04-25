#ifndef _MUDLIB_CORE_POLY_VECTOR_H_
#define _MUDLIB_CORE_POLY_VECTOR_H_

#include <mud/core/ns.h>
#include <vector>

BEGIN_MUDLIB_CORE_NS

/*
 * @brief Vector of polymorphic types.
 *
 * A vector that is able to hold polymorphic instances of a certain base type.
 */
template<class Type, class Allocator = std::allocator<std::shared_ptr<Type>>>
class poly_vector : private std::vector<std::shared_ptr<Type>, Allocator>
{
public:
    /**
     * Template parameter type definitions
     */
    typedef Type value_type;
    typedef Allocator allocator_type;

private:
    /**
     * Implementattion specific type definitions.
     */
    typedef std::shared_ptr<Type> storage_type;
    typedef std::vector<storage_type, allocator_type> impl_type;

public:
    /**
     * Member type definitions.
     */
    typedef typename impl_type::size_type size_type;
    typedef typename impl_type::difference_type difference_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
    typedef value_type& iterator_reference;
    typedef const value_type& const_iterator_reference;
    typedef value_type* iterator_pointer;
    typedef const value_type* const_iterator_pointer;

    /**
     * @brief A bidirectional iterator accessing the elements in a forward
     * direction and returning references to the elements.
     */
    class iterator
      : private std::vector<std::shared_ptr<Type>, Allocator>::iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Type value_type;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef typename std::vector<std::shared_ptr<Type>, Allocator>::iterator
            impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef value_type& reference;
        typedef value_type* pointer;

        /**
         * Construct an iterator.
         */
        iterator() = default;

        /**
         * Destruct the iterator.
         */
        virtual ~iterator() = default;

        /**
         * Return a reference to the object pointed to by the iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return *ref;
        }

        /**
         * Return a pointer to the object pointed to by the iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            return ref.get();
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
        iterator(impl_type impl) : _iter(impl) {}

        /**
         * The implementation dependent iterator.
         */
        impl_type _iter;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_vector;
    };

    /**
     * @brief A bidirectional iterator accessing the elements in a forward
     * direction and returning constant references to the elements.
     */
    class const_iterator
      : private std::vector<std::shared_ptr<Type>, Allocator>::const_iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Type value_type;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef typename std::vector<std::shared_ptr<Type>,
                                     Allocator>::const_iterator impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef const value_type& reference;
        typedef const value_type* pointer;

        /**
         * Construct an iterator.
         */
        const_iterator() = default;

        /**
         * Construct a constant iterator from a non-constant iterator.
         */
        const_iterator(iterator rhs) : _iter(rhs._iter) {}

        /**
         * Destruct the iterator.
         */
        virtual ~const_iterator() = default;

        /**
         * Return a reference to the object pointed to by the iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return *ref;
        }

        /**
         * Return a pointer to the object pointed to by the iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            return ref.get();
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
        const_iterator(impl_type impl) : _iter(impl) {}

        /**
         * The implementation dependent iterator.
         */
        impl_type _iter;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_vector;
    };

    /**
     * @brief A bidirectional iterator accessing the elements in a reverse
     * direction and returning references to the elements.
     */
    class reverse_iterator
      : private std::vector<std::shared_ptr<Type>, Allocator>::reverse_iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Type value_type;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef typename std::vector<std::shared_ptr<Type>,
                                     Allocator>::reverse_iterator impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef value_type& reference;
        typedef value_type* pointer;

        /**
         * Construct an reverse iterator.
         */
        reverse_iterator() = default;

        /**
         * Destruct the iterator.
         */
        virtual ~reverse_iterator() = default;

        /**
         * Return a reference to the object pointed to by the reverse iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return *ref;
        }

        /**
         * Return a pointer to the object pointed to by the reverse iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            return ref.get();
            ;
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
        reverse_iterator(impl_type impl) : _iter(impl) {}

        /**
         * The implementation dependent reverse_iterator.
         */
        impl_type _iter;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_vector;
    };

    /**
     * @brief A bidirectional iterator accessing the elements in a reverse
     * direction and returning constant references to the elements.
     */
    class const_reverse_iterator
      : private std::vector<std::shared_ptr<Type>,
                            Allocator>::const_reverse_iterator
    {
    public:
        /**
         * Template parameter type definitions.
         */
        typedef Type value_type;
        typedef Allocator allocator_type;

    private:
        /**
         * Implementation specific iterator type definition.
         */
        typedef
            typename std::vector<std::shared_ptr<Type>,
                                 Allocator>::const_reverse_iterator impl_type;

    public:
        /**
         * Member type definitions.
         */
        typedef typename impl_type::difference_type difference_type;
        typedef std::bidirectional_iterator_tag iterator_category;
        typedef const value_type& reference;
        typedef const value_type* pointer;

        /**
         * Construct an reverse iterator.
         */
        const_reverse_iterator() = default;

        /**
         * Construct a constant iterator from a non-constant iterator.
         */
        const_reverse_iterator(reverse_iterator rhs) : _iter(rhs._iter) {}

        /**
         * Destruct the iterator.
         */
        virtual ~const_reverse_iterator() = default;

        /**
         * Return a reference to the object pointed to by the reverse iterator.
         */
        reference operator*()
        {
            auto ref = *_iter;
            return *ref;
        }

        /**
         * Return a pointer to the object pointed to by the reverse iterator.
         */
        pointer operator->()
        {
            auto ref = *_iter;
            return ref.get();
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
        const_reverse_iterator(impl_type impl) : _iter(impl) {}

        /**
         * The implementation dependent reverse_iterator.
         */
        impl_type _iter;

        /** Allow the enclosing class access to the provate constructor. */
        friend class poly_vector;
    };

    /**
     * @brief Constructs a vector.
     */
    poly_vector() = default;

    /**
     * @brief Constructs a new vector by copying the contents from another one.
     * @param[in] rhs The vector to copy the contents from.
     *
     * Note that the copied items are not deep-copied, but their underlying
     * smart-pointers are copied instead. This in effect shares the same object
     * between two vectors.
     */
    poly_vector(const poly_vector& rhs) : impl_type(rhs){};

    /**
     * @brief Constructs a new vector by moving the contents from another one.
     * @param[in] rhs The vector to move the contents from.
     */
    poly_vector(poly_vector&& rhs) : impl_type(std::move(rhs)){};

    /**
     * @brief Assign the contents of another polymorphic vector.
     * @param[in] rhs The vector to copy the contents from.
     *
     * Note that the copied items are not deep-copied, but their underlying
     * smart-pointers are copied instead. This in effect shares the same object
     * between two vectors.
     */
    poly_vector& operator=(const poly_vector& rhs)
    {
        (void)impl_type::operator=(rhs);
        return *this;
    }

    /**
     * @brief Assign the contents of another polymorphic vector.
     * @param[in] rhs The vector to move the contents from.
     */
    poly_vector& operator=(poly_vector&& rhs)
    {
        (void)impl_type::operator=(std::move(rhs));
        return *this;
    }

    /**
     * @brief Destructs the vector.
     */
    virtual ~poly_vector() = default;

    /**
     * @brief Return the number of elements in the vector.
     */
    size_t size() const { return impl_type::size(); }

    /**
     * @brief Checks whether the vector is empty.
     */
    bool empty() const { return impl_type::empty(); }

    /**
     * Return a reference to the item at a position.
     * @param[in] pos The position to query (zero-based);
     * @throw std::out_of_range is the position is out of bounds.
     */
    reference at(size_type pos)
    {
        typename impl_type::reference ref = impl_type::at(pos);
        return *ref;
    }

    /**
     * Return a constant reference to the item at a position.
     * @param[in] pos The position to query (zero-based);
     * @throw std::out_of_range is the position is out of bounds.
     */
    const_reference at(size_type pos) const
    {
        typename impl_type::const_reference ref = impl_type::at(pos);
        return *ref;
    }

    /**
     * Return a reference to the item at a position.
     * @param[in] pos The position to query (zero-based);
     * @throw std::out_of_range is the position is out of bounds.
     */
    reference operator[](size_type pos)
    {
        typename impl_type::reference ref = impl_type::operator[](pos);
        return *ref;
    }

    /**
     * Return a constant reference to the item at a position.
     * @param[in] pos The position to query (zero-based);
     * @throw std::out_of_range is the position is out of bounds.
     */
    const_reference operator[](size_type pos) const
    {
        typename impl_type::const_reference ref = impl_type::operator[](pos);
        return *ref;
    }

    /**
     * Return a reference to the first item.
     * @throw std::out_of_range is the position is out of bounds.
     */
    reference front()
    {
        typename impl_type::reference ref = impl_type::front();
        return *ref;
    }

    /**
     * Return a constant reference to the last item.
     * @throw std::out_of_range is the position is out of bounds.
     */
    const_reference front() const
    {
        typename impl_type::const_reference ref = impl_type::front();
        return *ref;
    }

    /**
     * Return a reference to the last item.
     * @throw std::out_of_range is the position is out of bounds.
     */
    reference back()
    {
        typename impl_type::reference ref = impl_type::back();
        return *ref;
    }

    /**
     * Return a constant reference to the last item.
     * @throw std::out_of_range is the position is out of bounds.
     */
    const_reference back() const
    {
        typename impl_type::const_reference ref = impl_type::back();
        return *ref;
    }

    /**
     * @brief Clear the contents.
     */
    void clear() { return impl_type::clear(); }

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
     * @brief Append a new element to th end of the container.
     * @param[in] value The value to copy into the container.
     */
    template<class PolyType>
    void push_back(const PolyType& value)
    {
        impl_type::push_back(std::make_shared<PolyType>(value));
    }

    /**
     * @brief Append a new element to the end of the container.
     * @param[in] value The value to move into the container.
     */
    template<class PolyType>
    void push_back(PolyType&& value)
    {
        /* Use forwarding references to correctly deduce as this is also
         * invoked when used as a reference. Then we can deduce the correct
         * _push_back helper to invoke. */
        __push_back<typename std::decay<PolyType>::type>(
            std::forward<PolyType>(value));
    }

    /**
     * @brief Insert a new element at a certain position in the container.
     * @param[in] pos The position before to insert the element at.
     * @param[in] value The value to copy into the container.
     * @return The iterator pointing to the inserted item.
     */
    template<class PolyType>
    iterator insert(const_iterator pos, const PolyType& value)
    {
        auto new_pos =
            impl_type::insert(pos._iter, std::make_shared<PolyType>(value));
        return iterator(new_pos);
    }

    /**
     * @brief Inset a new element at a certain position in the container.
     * @param[in] pos The position before to insert the element at.
     * @param[in] value The value to move into the container.
     * @return The iterator pointing to the inserted item.
     */
    template<class PolyType>
    iterator insert(const_iterator pos, PolyType&& value)
    {
        /* Use forwarding references to correctly deduce as this is also
         * invoked when used as a reference. Then we can deduce the correct
         * __insert helper to invoke. */
        return __insert<typename std::decay<PolyType>::type>(
            pos._iter, std::forward<PolyType>(value));
    }

    /**
     * @brief Remove the last element of the container.
     */
    void pop_back() { *(impl_type::pop_back()); }

    /**
     * Return an iterator to the start of the vector.
     */
    iterator begin() { return iterator(impl_type::begin()); }
    const_iterator begin() const { return const_iterator(impl_type::cbegin()); }

    /**
     * Return an iterator to the element following the end of the vector.
     */
    iterator end() { return iterator(impl_type::end()); }
    const_iterator end() const { return const_iterator(impl_type::cend()); }

    /**
     * Return an iterator to the start of the vector.
     */
    const_iterator cbegin() const
    {
        return const_iterator(impl_type::cbegin());
    }

    /**
     * Return an iterator to the element following the end of the vector.
     */
    const_iterator cend() const { return const_iterator(impl_type::cend()); }

    /**
     * Return a reverse iterator to the start of the vector.
     */
    reverse_iterator rbegin() { return reverse_iterator(impl_type::rbegin()); }

    /**
     * Return a reverse iterator to the element following the end of the vector.
     */
    reverse_iterator rend() { return reverse_iterator(impl_type::rend()); }

    /**
     * Return a reveerse iterator to the start of the vector.
     */
    const_reverse_iterator crbegin() const
    {
        return const_reverse_iterator(impl_type::crbegin());
    }

    /**
     * Return a reverse iterator to the element following the end of the vector.
     */
    const_reverse_iterator crend() const
    {
        return const_reverse_iterator(impl_type::crend());
    }

private:
    /**
     * Helper functions for the  @c push_back(PolyType&&)
     * function which re-routes to the correct push-back (either as lvalue
     * or rvalue with move semantics).
     */
    template<class PolyType>
    void __push_back(const PolyType& value)
    {
        impl_type::push_back(std::make_shared<PolyType>(value));
    }
    template<class PolyType>
    void __push_back(PolyType&& value)
    {
        impl_type::push_back(std::make_shared<PolyType>(std::move(value)));
    }

    /**
     * Helper functions for the  @c insert(PolyType&&)
     * function which re-routes to the correct insert (either as lvalue
     * or rvalue with move semantics).
     */
    template<class PolyType>
    iterator __insert(const_iterator pos, const PolyType& value)
    {
        auto new_pos =
            impl_type::insert(pos._iter, std::make_shared<PolyType>(value));
        return iterator(new_pos);
    }

    template<class PolyType>
    iterator __insert(const_iterator pos, PolyType&& value)
    {
        auto new_pos = impl_type::insert(
            pos._iter, std::make_shared<PolyType>(std::move(value)));
        return iterator(new_pos);
    }
};

END_MUDLIB_CORE_NS

/* vi: set expandtab ai ts=4: */

#endif /*  _MUDLIB_CORE_POLY_VECTOR_H_ */
