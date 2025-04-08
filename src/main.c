#define KG_IMPL
#include "kg.h"

int main() {
    kg_allocator_t allocator = kg_allocator_default();

    isize* n = kg_darray_create(&allocator, isize, 64);
    for (isize i = 0; i < 10; i++) {
        kg_darray_append(n, i);
    }
    kg_assert(kg_darray_len(n) == 10);
    for (isize i = 0; i < kg_darray_len(n); i++) {
        kg_printf("%li\n", n[i]);
    }

    kg_darray_destroy(n);

    kg_string_t s = kg_file_read_string(&allocator, "./src/main.c");
    kg_log_info("%s", s);
    kg_string_destroy(s);

    return 0;
}
