#define KG_IMPL
#define KG_TESTER
#define KG_TESTER_IMPL
#include "kg.h"

void test_string_from_cstr() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_cstr(&allocator, "test");
    kgt_expect(strncmp(string, "test", 4) == 0, "nope");
    kg_string_destroy(string);
}

void test_string_from_fmt() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_fmt(&allocator, "%s 4", "test", 4);
    kgt_expect_neq_null(string);
    kgt_expect_eq(strncmp(string, "test 4", 6), 0);
    kg_string_destroy(string);
}

void test_string_append_cstr() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_cstr(&allocator, "test");
    kgt_expect_neq_null(string);
    string = kg_string_append_cstr(string, " ABCD");
    kgt_expect_neq_null(string);
    kgt_expect_eq(strncmp(string, "test ABCD", 9), 0);
    kg_string_destroy(string);
}

void test_string_append_fmt() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_cstr(&allocator, "test");
    kgt_expect_neq_null(string);
    string = kg_string_append_fmt(string, " %s %li", "ABCD", 4);
    kgt_expect_neq_null(string);
    kgt_expect_eq(strncmp(string, "test ABCD 4", 11), 0);
    kg_string_destroy(string);
}

int main() {
    kgt_t t;
    kgt_create(&t);

    kgt_test_t tests[] = {
        kgt_register(test_string_from_cstr),
        kgt_register(test_string_from_fmt),
        kgt_register(test_string_append_cstr),
        kgt_register(test_string_append_fmt),
    }; 
    isize tests_len = kg_sizeof(tests) / kg_sizeof(kgt_test_t);

    kgt_run(&t, tests, tests_len);

    kgt_destroy(&t);

    return 0;
}
