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

#include "mud/core/poly_vector.h"
#include "mud/test.h"
#include <cmath>
#include <memory>
#include <type_traits>

/* clang-format off */

/*
 * Base class used as the base of all the polymorphic types in the vector
 */
class base
{
public:
    virtual ~base() = default;
};

/*
 * A derived class from base that represents an integer.
 */
class derived_1: public base
{
public:
    derived_1(int i): _i(i) {}
    derived_1(const derived_1& rhs) { _i = rhs._i; }
    derived_1(derived_1&& rhs) { _i = rhs._i; rhs._i = -1; }
    int value() const { return _i; }
private:
    int _i;
};

/*
 * A derived class from base that represents a float.
 */
class derived_2: public base
{
public:
    derived_2(float f) : _f(f) {}
    derived_2(const derived_2& rhs) { _f = rhs._f; }
    derived_2(derived_2&& rhs) { _f = rhs._f; rhs._f = NAN; }
    float value() const { return _f; }
private:
    float _f;
};

typedef mud::core::poly_vector<int, base> vector_t;

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() : iter_ptr(nullptr), const_iter_ptr(nullptr),
                derived_1_object(10), derived_2_object(10.0) {
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Instances of objects */
    derived_1 derived_1_object;
    derived_2 derived_2_object;

    /* The vector */
    mud::core::poly_vector<base> vector;
    mud::core::poly_vector<base> copy;

    /* An iterator */
    mud::core::poly_vector<base>::iterator iter;
    mud::core::poly_vector<base>::const_iterator const_iter;
    mud::core::poly_vector<base>::reverse_iterator reverse_iter;
    mud::core::poly_vector<base>::const_reverse_iterator const_reverse_iter;

    /* Copies of reference to poly-vector entries */
    mud::core::poly_vector<base>::iterator_pointer iter_ptr;
    mud::core::poly_vector<base>::const_iterator_pointer const_iter_ptr;

END_CONTEXT()


FEATURE("Polymorphic Vector")
    DEFINE_GIVEN("An empty vector",
        [](context& ctx) {
        })
    DEFINE_GIVEN("A populated polymorphic vector with ten entries",
        [](context& ctx) {
            for (int i = 0; i < 10; i++) {
                if (i % 2 == 0) {
                    ctx.vector.push_back(derived_1(i));
                }
                else {
                    ctx.vector.push_back(derived_2((float)i));
                }
            }
            ASSERT(10, ctx.vector.size());
        })
    DEFINE_WHEN ("An iterator points to the first item",
        [](context& ctx) {
            ctx.iter = ctx.vector.begin();
        })
    DEFINE_WHEN ("A constant iterator points to the first item",
        [](context& ctx) {
            ctx.const_iter = ctx.vector.cbegin();
        })
    DEFINE_WHEN ("A reverse iterator points to the last item",
        [](context& ctx) {
            ctx.reverse_iter = ctx.vector.rbegin();
        })
    DEFINE_WHEN ("A constant reverse iterator points to the last item",
        [](context& ctx) {
            ctx.const_reverse_iter = ctx.vector.crbegin();
        })
    DEFINE_WHEN ("An iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.iter = ctx.vector.end();
        })
    DEFINE_WHEN ("A constant iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.const_iter = ctx.vector.cend();
        })
    DEFINE_WHEN ("A reverse iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.reverse_iter = ctx.vector.rend();
        })
    DEFINE_WHEN ("A constant reverse iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.const_reverse_iter = ctx.vector.crend();
        })
    DEFINE_WHEN ("An iterator advances to the next item",
        [](context& ctx) {
            ctx.iter++;
        })
    DEFINE_WHEN ("A constant iterator advances to the next item",
        [](context& ctx) {
            ctx.const_iter++;
        })
    DEFINE_WHEN ("A reverse iterator advances to the next item",
        [](context& ctx) {
            ctx.reverse_iter++;
        })
    DEFINE_WHEN ("A constant reverse iterator advances to the next item",
        [](context& ctx) {
            ctx.const_reverse_iter++;
        })
    DEFINE_WHEN ("An iterator retreats to the previous item",
        [](context& ctx) {
            ctx.iter--;
        })
    DEFINE_WHEN ("A constant iterator retreats to the previous item",
        [](context& ctx) {
            ctx.const_iter--;
        })
    DEFINE_WHEN ("A reverse iterator retreats to the previous item",
        [](context& ctx) {
            ctx.reverse_iter--;
        })
    DEFINE_WHEN ("A constant reverse iterator retreats to the previous item",
        [](context& ctx) {
            ctx.const_reverse_iter--;
        })
    DEFINE_WHEN ("The iterator accesses the item",
        [](context& ctx) {
            ctx.iter_ptr = &(*ctx.iter);
        })
    DEFINE_WHEN ("The constant iterator accesses the item",
        [](context& ctx) {
            ctx.const_iter_ptr = &(*ctx.const_iter);
        })
    DEFINE_WHEN ("The reverse iterator accesses the item",
        [](context& ctx) {
            ctx.iter_ptr = &(*ctx.reverse_iter);
        })
    DEFINE_WHEN ("The constant reverse iterator accesses the item",
        [](context& ctx) {
            ctx.const_iter_ptr = &(*ctx.const_reverse_iter);
        })
    DEFINE_WHEN ("An element is erased at the second position",
        [](context& ctx) {
            auto iter = ctx.vector.begin();
            ctx.iter = ctx.vector.erase(++iter);
        })
    DEFINE_THEN("The vector is empty",
        [](context& ctx) {
            ASSERT(true, ctx.vector.empty());
        })
    DEFINE_THEN("The vector is not empty",
        [](context& ctx) {
            ASSERT(false, ctx.vector.empty());
        })
    DEFINE_THEN("The vector has no entries",
        [](context& ctx) {
            ASSERT(0, ctx.vector.size());
        })
    DEFINE_THEN("The vector has one entry",
        [](context& ctx) {
            ASSERT(1, ctx.vector.size());
        })
    DEFINE_THEN("The vector has ten entries",
        [](context& ctx) {
            ASSERT(10, ctx.vector.size());
        })
    DEFINE_THEN("The vector has eleven entries",
        [](context& ctx) {
            ASSERT(11, ctx.vector.size());
        })
    DEFINE_THEN("The other vector has ten entries",
        [](context& ctx) {
            ASSERT(10, ctx.copy.size());
        })
    DEFINE_THEN("The vector has nine entries",
        [](context& ctx) {
            ASSERT(9, ctx.vector.size());
        })
    DEFINE_THEN("The vector has eight entries",
        [](context& ctx) {
            ASSERT(8, ctx.vector.size());
        })
    DEFINE_THEN("The reference points to the first item",
        [](context& ctx) {
            ASSERT(0, dynamic_cast<derived_1*>(ctx.iter_ptr)->value());
        })
    DEFINE_THEN("The constant reference points to the first item",
        [](context& ctx) {
            ASSERT(0, dynamic_cast<const derived_1*>(
                      ctx.const_iter_ptr)->value());
        })
    DEFINE_THEN("The reference points to the second item",
        [](context& ctx) {
            ASSERT(1.0, dynamic_cast<derived_2*>(ctx.iter_ptr)->value());
        })
    DEFINE_THEN("The constant reference points to the second item",
        [](context& ctx) {
            ASSERT(1.0, dynamic_cast<const derived_2*>(
                        ctx.const_iter_ptr)->value());
        })
    DEFINE_THEN("The reference points to the last item",
        [](context& ctx) {
            ASSERT(9.0, dynamic_cast<derived_2*>(ctx.iter_ptr)->value());
        })
    DEFINE_THEN("The constant reference points to the last item",
        [](context& ctx) {
            ASSERT(9.0, dynamic_cast<const derived_2*>(
                        ctx.const_iter_ptr)->value());
        })
    DEFINE_THEN("The reference points to the second last item",
        [](context& ctx) {
            ASSERT(8, dynamic_cast<derived_1*>(ctx.iter_ptr)->value());
        })
    DEFINE_THEN("The constant reference points to the second last item",
        [](context& ctx) {
            ASSERT(8, dynamic_cast<const derived_1*>(
                      ctx.const_iter_ptr)->value());
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Polymorphic vector type traits")
    GIVEN("A polymorphic vector type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::poly_vector<base>>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::poly_vector<base>>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::poly_vector<base>>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::poly_vector<base>>::value);
        })
    AND  ("The type is move-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_move_assignable<
                  mud::core::poly_vector<base>>::value);
        })

  SCENARIO("Default constructed vector has no elements")
    GIVEN("An empty vector")
    WHEN ("The vector is examined", [](context&){})
    THEN ("The vector is empty")
    AND  ("The vector has no entries")

  SCENARIO("Pushing an element in a vector")
    GIVEN("An empty vector")
    WHEN ("An element is pushed",
        [](context& ctx) {
            ctx.vector.push_back(derived_1(10));
        })
    THEN("The vector is not empty")
    AND ("The vector has one entry")
    AND ("The entry contains the pushed item",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.vector.at(0));
            ASSERT(10, entry.value());
        })

  SCENARIO("Erasing an element from a vector by position")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("An element is erased at the second position")
    THEN("The vector has nine entries")
    AND ("The entry does not contain the erased item",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.vector.at(1));
            ASSERT(2, entry.value());
        })

  SCENARIO("Erasing an element by range")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("The second to fourth elements are erased",
        [](context& ctx) {
            auto second = ctx.vector.begin();
            ++second;
            auto fourth = second;
            ++fourth, ++fourth;
            (void) ctx.vector.erase(second, fourth);
        })
    THEN("The vector has eight entries")
    AND ("The entry does not contain the erased items",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.vector.at(2));
            ASSERT(4, entry.value());
        })

  SCENARIO("Inserting an element into an ordinary position")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("An element is inserted into the second position",
        [](context& ctx) {
            ctx.iter = ctx.vector.begin();
            ctx.vector.insert(++ctx.iter, derived_1(101));
        })
    THEN("The vector has eleven entries")
    AND ("The entry contains the newly inserted items",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.vector.at(1));
            ASSERT(101, entry.value());
        })

  SCENARIO("Inserting an element at the begin position")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("An element is erased at the begin position",
        [](context& ctx) {
            ctx.iter = ctx.vector.begin();
            ctx.vector.insert(ctx.iter, derived_1(101));
        })
    THEN("The vector has eleven entries")
    AND ("The entry contains the newly inserted items",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.vector.at(0));
            ASSERT(101, entry.value());
        })

  SCENARIO("Inserting an element at the end position")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("An element is erased at the end position",
        [](context& ctx) {
            ctx.iter = ctx.vector.end();
            ctx.vector.insert(ctx.iter, derived_1(101));
        })
    THEN("The vector has eleven entries")
    AND ("The entry contains the newly inserted items",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.vector.at(10));
            ASSERT(101, entry.value());
        })

  SCENARIO("Access through iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("An iterator points to the first item")
    AND  ("The iterator accesses the item")
    THEN ("The reference points to the first item")

  SCENARIO("Access through constant iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A constant iterator points to the first item")
    AND  ("The constant iterator accesses the item")
    THEN ("The constant reference points to the first item")

  SCENARIO("Access through reverse iterator dereference operator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A reverse iterator points to the last item")
    AND  ("The reverse iterator accesses the item")
    THEN ("The reference points to the last item")
    
  SCENARIO("Access through constant reverse iterator dereference operator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A constant reverse iterator points to the last item")
    AND  ("The constant reverse iterator accesses the item")
    THEN ("The constant reference points to the last item")
    
  SCENARIO("Advancing an iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("An iterator points to the first item")
    AND  ("An iterator advances to the next item")
    AND  ("The iterator accesses the item")
    THEN ("The reference points to the second item")

  SCENARIO("Advancing a constant iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A constant iterator points to the first item")
    AND  ("A constant iterator advances to the next item")
    AND  ("The constant iterator accesses the item")
    THEN ("The constant reference points to the second item")

  SCENARIO("Advancing a reverse iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A reverse iterator points to the last item")
    AND  ("A reverse iterator advances to the next item")
    AND  ("The reverse iterator accesses the item")
    THEN ("The reference points to the second last item")

  SCENARIO("Advancing a constant reverse iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A constant reverse iterator points to the last item")
    AND  ("A constant reverse iterator advances to the next item")
    AND  ("The constant reverse iterator accesses the item")
    THEN ("The constant reference points to the second last item")

  SCENARIO("Retreating past-the-end iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("An iterator points to the past-the-end item")
    AND  ("An iterator retreats to the previous item")
    AND  ("The iterator accesses the item")
    THEN ("The reference points to the last item")

  SCENARIO("Retreating past-the-end constant iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A constant iterator points to the past-the-end item")
    AND  ("A constant iterator retreats to the previous item")
    AND  ("The constant iterator accesses the item")
    THEN ("The constant reference points to the last item")

  SCENARIO("Retreating past-the-end reverse iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A reverse iterator points to the past-the-end item")
    AND  ("A reverse iterator retreats to the previous item")
    AND  ("The reverse iterator accesses the item")
    THEN ("The reference points to the first item")

  SCENARIO("Retreating past-the-end constant reverse iterator")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("A constant reverse iterator points to the past-the-end item")
    AND  ("A constant reverse iterator retreats to the previous item")
    AND  ("The constant reverse iterator accesses the item")
    THEN ("The constant reference points to the first item")

  SCENARIO("Moving an object into a vector")
    GIVEN("An empty vector")
    WHEN ("An item is moved into the object",
        [](context& ctx) {
            ctx.vector.push_back(std::move(ctx.derived_2_object));
        })
    THEN ("The vector has one entry")
    AND  ("The object is moved into the vector",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_2&>(ctx.vector.at(0));
            ASSERT(10.0, entry.value());
        })
    AND  ("The original object is destroyed",
        [](context& ctx) {
            ASSERT(true, std::isnan(ctx.derived_2_object.value()));
        })

  SCENARIO("Moving a vector to another a vector")
    GIVEN("A populated polymorphic vector with ten entries")
    WHEN ("The vector is moved to another one",
        [](context& ctx) {
            ctx.copy = std::move(ctx.vector);
        })
    THEN ("The vector is empty") 
    AND  ("The other vector has ten entries") 
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
