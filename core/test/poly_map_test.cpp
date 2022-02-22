#include "mud/core/poly_map.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* clang-format off */

/*
 * Base class used as the base of all the polymorphic types in the map
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
    float value() const { return _f; }
private:
    float _f;
};

typedef mud::core::poly_map<int, base> map_t;

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() : ref_ptr(nullptr), const_ref_ptr(nullptr){
    }

    /* Destructor after each scenario */
    ~context() {
        delete ref_ptr;
        delete const_ref_ptr;
    }

    /* The map */
    mud::core::poly_map<int, base> map;

    /* An iterator */
    mud::core::poly_map<int, base>::iterator iter;
    mud::core::poly_map<int, base>::const_iterator const_iter;
    mud::core::poly_map<int, base>::reverse_iterator reverse_iter;
    mud::core::poly_map<int, base>::const_reverse_iterator const_reverse_iter;

    /* Copies of reference to poly-map entries */
    mud::core::poly_map<int, base>::iterator_reference* ref_ptr;
    mud::core::poly_map<int, base>::const_iterator_reference* const_ref_ptr;

END_CONTEXT()


FEATURE("Polymorphic Map")
    DEFINE_GIVEN("An empty map",
        [](context& ctx) {
        })
    DEFINE_GIVEN("A populated polymorphic map with ten entries",
        [](context& ctx) {
            for (int i = 0; i < 10; i+=2) {
                ctx.map.insert(i, derived_1(i));
            }
            for (int i = 1; i < 11; i+=2) {
                ctx.map.insert(i, derived_2((float)i));
            }
            ASSERT(10, ctx.map.size());
        })
    DEFINE_WHEN ("An iterator points to the first item",
        [](context& ctx) {
            ctx.iter = ctx.map.begin();
        })
    DEFINE_WHEN ("A constant iterator points to the first item",
        [](context& ctx) {
            ctx.const_iter = ctx.map.cbegin();
        })
    DEFINE_WHEN ("A reverse iterator points to the last item",
        [](context& ctx) {
            ctx.reverse_iter = ctx.map.rbegin();
        })
    DEFINE_WHEN ("A constant reverse iterator points to the last item",
        [](context& ctx) {
            ctx.const_reverse_iter = ctx.map.crbegin();
        })
    DEFINE_WHEN ("An iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.iter = ctx.map.end();
        })
    DEFINE_WHEN ("A constant iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.const_iter = ctx.map.cend();
        })
    DEFINE_WHEN ("A reverse iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.reverse_iter = ctx.map.rend();
        })
    DEFINE_WHEN ("A constant reverse iterator points to the past-the-end item",
        [](context& ctx) {
            ctx.const_reverse_iter = ctx.map.crend();
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
            ctx.ref_ptr = new map_t::iterator_reference(
                    ctx.iter->first,
                    ctx.iter->second);
        })
    DEFINE_WHEN ("The constant iterator accesses the item",
        [](context& ctx) {
            ctx.const_ref_ptr = new map_t::const_iterator_reference(
                    ctx.const_iter->first,
                    ctx.const_iter->second);
        })
    DEFINE_WHEN ("The reverse iterator accesses the item",
        [](context& ctx) {
            ctx.ref_ptr = new map_t::iterator_reference(
                    ctx.reverse_iter->first,
                    ctx.reverse_iter->second);
        })
    DEFINE_WHEN ("The constant reverse iterator accesses the item",
        [](context& ctx) {
            ctx.const_ref_ptr = new map_t::const_iterator_reference(
                    ctx.const_reverse_iter->first,
                    ctx.const_reverse_iter->second);
        })
    DEFINE_THEN("The map is empty",
        [](context& ctx) {
            ASSERT(true, ctx.map.empty());
        })
    DEFINE_THEN("The map is not empty",
        [](context& ctx) {
            ASSERT(false, ctx.map.empty());
        })
    DEFINE_THEN("The map has no entries",
        [](context& ctx) {
            ASSERT(0, ctx.map.size());
        })
    DEFINE_THEN("The map has one entry",
        [](context& ctx) {
            ASSERT(1, ctx.map.size());
        })
    DEFINE_THEN("The map has ten entries",
        [](context& ctx) {
            ASSERT(10, ctx.map.size());
        })
    DEFINE_THEN("The map has nine entries",
        [](context& ctx) {
            ASSERT(9, ctx.map.size());
        })
    DEFINE_THEN("The map has eight entries",
        [](context& ctx) {
            ASSERT(8, ctx.map.size());
        })
    DEFINE_THEN("The reference points to the first item",
        [](context& ctx) {
            ASSERT(0, ctx.ref_ptr->first);
            ASSERT(0, dynamic_cast<derived_1&>(
                      ctx.ref_ptr->second).value());
        })
    DEFINE_THEN("The constant reference points to the first item",
        [](context& ctx) {
            ASSERT(0, ctx.const_ref_ptr->first);
            ASSERT(0, dynamic_cast<const derived_1&>(
                      ctx.const_ref_ptr->second).value());
        })
    DEFINE_THEN("The reference points to the second item",
        [](context& ctx) {
            ASSERT(1, ctx.ref_ptr->first);
            ASSERT(1.0, dynamic_cast<derived_2&>(
                      ctx.ref_ptr->second).value());
        })
    DEFINE_THEN("The constant reference points to the second item",
        [](context& ctx) {
            ASSERT(1, ctx.const_ref_ptr->first);
            ASSERT(1.0, dynamic_cast<const derived_2&>(
                      ctx.const_ref_ptr->second).value());
        })
    DEFINE_THEN("The reference points to the last item",
        [](context& ctx) {
            ASSERT(9, ctx.ref_ptr->first);
            ASSERT(9.0, dynamic_cast<derived_2&>(
                        ctx.ref_ptr->second).value());
        })
    DEFINE_THEN("The constant reference points to the last item",
        [](context& ctx) {
            ASSERT(9, ctx.const_ref_ptr->first);
            ASSERT(9.0, dynamic_cast<const derived_2&>(
                        ctx.const_ref_ptr->second).value());
        })
    DEFINE_THEN("The reference points to the second last item",
        [](context& ctx) {
            ASSERT(8, ctx.ref_ptr->first);
            ASSERT(8, dynamic_cast<derived_1&>(
                        ctx.ref_ptr->second).value());
        })
    DEFINE_THEN("The constant reference points to the second last item",
        [](context& ctx) {
            ASSERT(8, ctx.const_ref_ptr->first);
            ASSERT(8, dynamic_cast<const derived_1&>(
                        ctx.const_ref_ptr->second).value());
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Polymorphic map type traits")
    GIVEN("A polymorphic map type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::poly_map<int, base>>::value);
        })
    AND  ("The type is copy-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_copy_constructible<
                  mud::core::poly_map<int, base>>::value);
        })
    AND  ("The type is copy-assignable",
        [](context& ctx) {
            ASSERT(true, std::is_copy_assignable<
                  mud::core::poly_map<int, base>>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::core::poly_map<int, base>>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::core::poly_map<int, base>>::value);
        })

  SCENARIO("Default constructed map has no elements")
    GIVEN("An empty map")
    WHEN ("The map is examined", [](context&){})
    THEN ("The map is empty")
    AND  ("The map has no entries")

  SCENARIO("Inserting an element in a map")
    GIVEN("An empty map")
    WHEN ("An element is inserted",
        [](context& ctx) {
            ctx.map.insert(10, derived_1(10));
        })
    THEN("The map is not empty")
    AND ("The map has one entry")
    AND ("The entry contains the inserted item",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.map.at(10));
            ASSERT(10, entry.value());
        })

  SCENARIO("Inserting an element with the same key overwrites previous entry")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("An element is inserted with an pre-existing key",
        [](context& ctx) {
            ctx.map.insert(2, derived_1(200));
        })
    THEN("The map has ten entries")
    AND ("The entry contains the updated item",
        [](context& ctx) {
            auto& entry = dynamic_cast<derived_1&>(ctx.map.at(2));
            ASSERT(200, entry.value());
        })

  SCENARIO("Erasing an element from a map by key")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("An element is erased for a key that does exist in the map",
        [](context& ctx) {
            ASSERT(1, ctx.map.erase(2));
        })
    THEN("The map has nine entries")
    AND ("The entry does not contain the erased item",
        [](context& ctx) {
            ASSERT_THROW(std::out_of_range, ctx.map.at(2));
        })

  SCENARIO("Erasing an element from a map by key that does not exist")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("An element is erased for a key that does not exist in the map",
        [](context& ctx) {
            ASSERT(0, ctx.map.erase(20));
        })
    THEN("The map has ten entries")

  SCENARIO("Erasing an element by position")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("The second element is erased",
        [](context& ctx) {
            auto iter = ctx.map.begin();
            ++iter;
            (void) ctx.map.erase(iter);
        })
    THEN("The map has nine entries")
    AND ("The entry does not contain the erased item",
        [](context& ctx) {
            ASSERT_THROW(std::out_of_range, ctx.map.at(1));
        })

  SCENARIO("Erasing an element by range")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("The second to fourth elements are erased",
        [](context& ctx) {
            auto second = ctx.map.begin();
            ++second;
            auto fourth = second;
            ++fourth, ++fourth;
            (void) ctx.map.erase(second, fourth);
        })
    THEN("The map has eight entries")
    AND ("The entry does not contain the erased items",
        [](context& ctx) {
            ASSERT_THROW(std::out_of_range, ctx.map.at(1));
            ASSERT_THROW(std::out_of_range, ctx.map.at(2));
        })

  SCENARIO("Access through iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("An iterator points to the first item")
    AND  ("The iterator accesses the item")
    THEN ("The reference points to the first item")

  SCENARIO("Access through constant iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A constant iterator points to the first item")
    AND  ("The constant iterator accesses the item")
    THEN ("The constant reference points to the first item")

  SCENARIO("Access through reverse iterator dereference operator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A reverse iterator points to the last item")
    AND  ("The reverse iterator accesses the item")
    THEN ("The reference points to the last item")
    
  SCENARIO("Access through constant reverse iterator dereference operator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A constant reverse iterator points to the last item")
    AND  ("The constant reverse iterator accesses the item")
    THEN ("The constant reference points to the last item")
    
  SCENARIO("Advancing an iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("An iterator points to the first item")
    AND  ("An iterator advances to the next item")
    AND  ("The iterator accesses the item")
    THEN ("The reference points to the second item")

  SCENARIO("Advancing a constant iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A constant iterator points to the first item")
    AND  ("A constant iterator advances to the next item")
    AND  ("The constant iterator accesses the item")
    THEN ("The constant reference points to the second item")

  SCENARIO("Advancing a reverse iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A reverse iterator points to the last item")
    AND  ("A reverse iterator advances to the next item")
    AND  ("The reverse iterator accesses the item")
    THEN ("The reference points to the second last item")

  SCENARIO("Advancing a constant reverse iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A constant reverse iterator points to the last item")
    AND  ("A constant reverse iterator advances to the next item")
    AND  ("The constant reverse iterator accesses the item")
    THEN ("The constant reference points to the second last item")

  SCENARIO("Retreating past-the-end iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("An iterator points to the past-the-end item")
    AND  ("An iterator retreats to the previous item")
    AND  ("The iterator accesses the item")
    THEN ("The reference points to the last item")

  SCENARIO("Retreating past-the-end constant iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A constant iterator points to the past-the-end item")
    AND  ("A constant iterator retreats to the previous item")
    AND  ("The constant iterator accesses the item")
    THEN ("The constant reference points to the last item")

  SCENARIO("Retreating past-the-end reverse iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A reverse iterator points to the past-the-end item")
    AND  ("A reverse iterator retreats to the previous item")
    AND  ("The reverse iterator accesses the item")
    THEN ("The reference points to the first item")

  SCENARIO("Retreating past-the-end constant reverse iterator")
    GIVEN("A populated polymorphic map with ten entries")
    WHEN ("A constant reverse iterator points to the past-the-end item")
    AND  ("A constant reverse iterator retreats to the previous item")
    AND  ("The constant reverse iterator accesses the item")
    THEN ("The constant reference points to the first item")
END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
