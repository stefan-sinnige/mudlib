#include "mud/core/handle.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* clang-format off */

CONTEXT()
    /* Constructor initialised for each scenario run */
    context() {
        resource = 0;
    }

    /* Destructor after each scenario */
    ~context() {
    }

    /* Operating system resource */
    int resource;

    /* The (integer) handles */
    std::unique_ptr<mud::core::handle> handle;
    std::unique_ptr<mud::core::handle> other;
END_CONTEXT()

FEATURE("Handle")
      DEFINE_GIVEN("An initialised operating system resource type",
        [](context& ctx) {
            ctx.resource = 10;
        })
      DEFINE_GIVEN("An initialised operating system resource handle",
        [](context& ctx) {
            ctx.resource = 10;
            ctx.handle = std::unique_ptr<mud::core::handle>(
                new mud::core::select_handle(ctx.resource));
        })
      DEFINE_WHEN ("A handle is assigned",
        [](context& ctx) {
            ctx.handle = std::unique_ptr<mud::core::handle>(
                new mud::core::select_handle(ctx.resource));
        })
     DEFINE_THEN ("The handle is non-existent",
        [](context& ctx) {
            ASSERT(nullptr, ctx.handle.get());
        })
     DEFINE_THEN ("The handle is valid",
        [](context& ctx) {
            ASSERT(true, ctx.handle->valid());
        })
     DEFINE_THEN ("The handle is invalid",
        [](context& ctx) {
            ASSERT(false, ctx.handle->valid());
        })
     DEFINE_THEN ("The other handle is valid",
        [](context& ctx) {
            ASSERT(true, ctx.other->valid());
        })
     DEFINE_THEN ("The other handle is invalid",
        [](context& ctx) {
            ASSERT(false, ctx.other->valid());
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Handle type traits")
    GIVEN("A handle type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::core::handle>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::handle>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::handle>::value);
        })
    AND  ("The type is not move-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_move_constructible<
                  mud::core::handle>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::core::handle>::value);
        })

  SCENARIO("Integer handle type traits")
    GIVEN("An integer handle type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is default constructible",
        [](context& ctx) {
            ASSERT(true, std::is_default_constructible<
                  mud::core::select_handle>::value);
        })
    AND  ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::core::select_handle>::value);
        })
    AND  ("The type is not copy-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_copy_assignable<
                  mud::core::select_handle>::value);
        })
    AND  ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::core::select_handle>::value);
        })
    AND  ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::core::select_handle>::value);
        })

  SCENARIO("Default constructed handle")
    GIVEN("An default constructed integer handle",
        [](context& ctx){
            ctx.handle = std::unique_ptr<mud::core::handle>(
                new mud::core::select_handle());
        })
    WHEN ("The handle is examined", [](context&){})
    THEN ("The handle is invalid")

  SCENARIO("Assignment to an operating system resource")
    GIVEN("An initialised operating system resource type")
    WHEN ("A handle is assigned")
    THEN ("A handle becomes the owner of the resource",
        [](context& ctx) {
            ASSERT(ctx.resource, mud::core::internal_handle<int>(ctx.handle));
        })
    AND ("The handle is valid")

  SCENARIO("Moving a unique handle to another instance")
    GIVEN("An initialised operating system resource handle")
    WHEN ("A handle is assigned to another instance",
        [](context& ctx) {
            ctx.other = std::move(ctx.handle);
        })
    THEN ("The handle is non-existent")
    AND  ("The other handle is valid")

END_FEATURE()

/* clang-format on */

/* vi: set ai ts=4 expandtab: */
