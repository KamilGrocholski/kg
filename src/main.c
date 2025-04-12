#define KG_IMPL
/*#define KG_THREADS*/
/*#define KG_THREADS_IMPL*/
/*#define KG_LOGGER*/
/*#define KG_LOGGER_IMPL*/
#include "kg.h"

i32 main() {
    kg_str_t str = kg_str_create("siema");

    kg_allocator_t allocator = kg_allocator_default();

    return 0;
}
