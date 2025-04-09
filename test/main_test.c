#define KG_IMPL
#define KG_TESTER
#define KG_TESTER_IMPL
#include "kg.h"

void test_string_from_cstr() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_cstr(&allocator, "test");
    kgt_expect_cstr_eq(string, "test");
    kg_string_destroy(string);
}

void test_string_from_fmt() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_fmt(&allocator, "%s 4", "test", 4);
    kgt_expect_not_null(string);
    kgt_expect_cstr_eq(string, "test 4");
    kg_string_destroy(string);
}

void test_string_append_cstr() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_cstr(&allocator, "test");
    kgt_expect_not_null(string);
    string = kg_string_append_cstr(string, " ABCD");
    kgt_expect_not_null(string);
    kgt_expect_cstr_eq(string, "test ABCD");
    kg_string_destroy(string);
}

void test_string_append_fmt() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_cstr(&allocator, "test");
    kgt_expect_not_null(string);
    string = kg_string_append_fmt(string, " %s %li", "ABCD", 4);
    kgt_expect_not_null(string);
    kgt_expect_cstr_eq(string, "test ABCD 4");
    kg_string_destroy(string);
}

void test_str_is_equal() {
    kg_str_t a = kg_str_create("testA");
    kg_str_t b = kg_str_create("testA");
    kg_str_t c = kg_str_create("testB");
    kgt_expect_true(kg_str_is_equal(a, b));
    kgt_expect_false(kg_str_is_equal(a, c));
}

void test_str_index() {
    kg_str_t a = kg_str_create("testAtest");
    kg_str_t b = kg_str_create("A");
    kgt_expect_eq(kg_str_index(a, b), 4);
}

void test_str_contains() {
    kg_str_t a = kg_str_create("testAtest");
    kg_str_t b = kg_str_create("A");
    kgt_expect_true(kg_str_contains(a, b));
}

void test_str_has_prefix() {
    kg_str_t a = kg_str_create("AtestAtest");
    kg_str_t b = kg_str_create("A");
    kgt_expect_true(kg_str_has_prefix(a, b));
}

void test_str_has_suffix() {
    kg_str_t a = kg_str_create("AtestAtestB");
    kg_str_t b = kg_str_create("B");
    kgt_expect_true(kg_str_has_suffix(a, b));
}

int main() {
    kgt_t t;
    kgt_create(&t);

    kgt_test_t tests[] = {
        kgt_register(test_string_from_cstr),
        kgt_register(test_string_from_fmt),
        kgt_register(test_string_append_cstr),
        kgt_register(test_string_append_fmt),
        kgt_register(test_str_is_equal),
        kgt_register(test_str_index),
        kgt_register(test_str_contains),
        kgt_register(test_str_has_prefix),
        kgt_register(test_str_has_suffix),
    }; 
    isize tests_len = kg_sizeof(tests) / kg_sizeof(kgt_test_t);

    kgt_run(&t, tests, tests_len);

    kgt_destroy(&t);

    return 0;
}
