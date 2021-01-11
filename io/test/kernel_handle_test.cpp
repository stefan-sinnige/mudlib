#include "mud/io/kernel_handle.h"
#include "mud/test.h"
#include <memory>
#include <type_traits>

/* *INDENT-OFF* */

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

    /* Kernel handles */
    std::unique_ptr<mud::io::kernel_handle> handle;
END_CONTEXT()

FEATURE("Kernel handle")
      DEFINE_GIVEN("An initialised operating system resource type",
        [](context& ctx) {
            ctx.resource = 10;
        })
      DEFINE_WHEN ("A kernel handle is assigned",
        [](context& ctx) {
            ctx.handle = std::unique_ptr<mud::io::kernel_handle>(
                new mud::io::kernel_handle(ctx.resource));
        })
     DEFINE_THEN ("The kernel handle is valid",
        [](context& ctx) {
            ASSERT(true, ctx.handle->valid());
        })
     DEFINE_THEN ("The kernel handle is invalid",
        [](context& ctx) {
            ASSERT(false, ctx.handle->valid());
        })
  END_DEFINES()

  /*
   * The scenarios 
   */

  SCENARIO("Type traits")
    GIVEN("A kernel_handle type", [](context&){})
    WHEN ("The type traits are examined", [](context&){})
    THEN ("The type is not default constructible",
        [](context& ctx) {
            ASSERT(false, std::is_default_constructible<
                  mud::io::kernel_handle>::value);
        })
    THEN ("The type is not copy-constructible",
        [](context& ctx) {
            ASSERT(false, std::is_copy_constructible<
                  mud::io::kernel_handle>::value);
        })
    THEN ("The type is not assignable",
        [](context& ctx) {
            ASSERT(false, std::is_assignable<
                  mud::io::kernel_handle,
                  mud::io::kernel_handle>::value);
        })
    THEN ("The type is move-constructible",
        [](context& ctx) {
            ASSERT(true, std::is_move_constructible<
                  mud::io::kernel_handle>::value);
        })
    THEN ("The type is not move-assignable",
        [](context& ctx) {
            ASSERT(false, std::is_move_assignable<
                  mud::io::kernel_handle>::value);
        })

  SCENARIO("Assignment to an operating system resource")
    GIVEN("An initialised operating system resource type")
    WHEN ("A kernel handle is assigned")
    THEN ("A kernel handle becomes the owner of the resource",
        [](context& ctx) {
            ASSERT(ctx.resource, (int)*ctx.handle);
        })
    AND ("The kernel handle is valid")

END_FEATURE()

/* *INDENT-ON* */

/* vi: set ai ts=4 expandtab: */
