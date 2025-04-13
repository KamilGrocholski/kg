#define KG_IMPL
#define KG_TESTER
#define KG_TESTER_IMPL
#define KG_THREADS
#define KG_THREADS_IMPL
#include "kg.h"

void test_mem_swap() {
    isize a = 1234;
    isize b = 2345;
    kg_mem_swap(&a, &b, kg_sizeof(isize));
    kgt_expect_eq(a, 2345);
    kgt_expect_eq(b, 1234);
}

void test_file_read_contant() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_file_content_t content = kg_file_read_content(&allocator, "./test/test.txt");
    kgt_expect_true(content.is_valid);
    kgt_expect_not_null(content.cstr);
    kgt_expect_cstr_eq(content.cstr, "test\ntest2\n");
    kg_allocator_free(&allocator, content.cstr, content.len);
}

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

void test_str_trim_prefix() {
    kg_str_t a = kg_str_create("AtestAtest");
    kg_str_t b = kg_str_create("Atest");
    kg_str_t c = kg_str_trim_prefix(a, b);
    kgt_expect_eq(c.len, 5);
    kgt_expect_mem_eq(a.ptr + 5, c.ptr, c.len);
}

void test_str_has_suffix() {
    kg_str_t a = kg_str_create("AtestAtestB");
    kg_str_t b = kg_str_create("B");
    kgt_expect_true(kg_str_has_suffix(a, b));
}

void test_str_trim_suffix() {
    kg_str_t a = kg_str_create("AtestAtest");
    kg_str_t b = kg_str_create("Atest");
    kg_str_t c = kg_str_trim_suffix(a, b);
    kgt_expect_eq(c.len, 5);
    kgt_expect_mem_eq(a.ptr, c.ptr, c.len);
}

void test_str_sub() {
    kg_str_t a = kg_str_create("AtestAtestB");
    kg_str_t sub = kg_str_substr(a, 1, 5);
    kgt_expect_eq(sub.len, 4);
    kgt_expect_mem_eq(a.ptr + 1, sub.ptr, 4);

    kg_str_t subfrom = kg_str_substr_from(a, 1);
    kgt_expect_eq(subfrom.len, 10);
    kgt_expect_mem_eq(a.ptr + 1, subfrom.ptr, 10);

    kg_str_t subto = kg_str_substr_to(a, 5);
    kgt_expect_eq(subto.len, 5);
    kgt_expect_mem_eq(a.ptr, subto.ptr, 5);
}

void test_str_chop_first_split_by() {
    kg_str_t str = kg_str_create("this=1;is=2;something=3;to=4");
    kg_str_t split_kvs = kg_str_create(";");
    kg_str_t split_kv = kg_str_create("=");

    kg_str_t kv = kg_str_chop_first_split_by(&str, split_kvs);
    kg_str_t k = kg_str_chop_first_split_by(&kv, split_kv);
    kgt_expect_true(kg_str_is_equal(k, kg_str_create("this")));
    kg_str_t v = kv;
    kgt_expect_true(kg_str_is_equal(v, kg_str_create("1")));
    kgt_expect_true(kg_str_is_equal(str, kg_str_create("is=2;something=3;to=4")));

    kv = kg_str_chop_first_split_by(&str, split_kvs);
    k = kg_str_chop_first_split_by(&kv, split_kv);
    kgt_expect_true(kg_str_is_equal(k, kg_str_create("is")));
    v = kv;
    kgt_expect_true(kg_str_is_equal(v, kg_str_create("2")));
    kgt_expect_true(kg_str_is_equal(str, kg_str_create("something=3;to=4")));

    kv = kg_str_chop_first_split_by(&str, split_kvs);
    k = kg_str_chop_first_split_by(&kv, split_kv);
    kgt_expect_true(kg_str_is_equal(k, kg_str_create("something")));
    v = kv;
    kgt_expect_true(kg_str_is_equal(v, kg_str_create("3")));
    kgt_expect_true(kg_str_is_equal(str, kg_str_create("to=4")));

    kv = kg_str_chop_first_split_by(&str, split_kvs);
    k = kg_str_chop_first_split_by(&kv, split_kv);
    kgt_expect_true(kg_str_is_equal(k, kg_str_create("to")));
    v = kv;
    kgt_expect_true(kg_str_is_equal(v, kg_str_create("4")));
    kgt_expect_true(kg_str_is_equal(str, kg_str_create("")));

    kgt_expect_true(kg_str_is_empty(str));
}

void test_duration_since() {
    kg_time_t t = kg_time_now();
    kg_duration_t d = kg_duration_since(t);
    kgt_expect_eq(kg_duration_to_milliseconds(d), 0);
}

void test_time_date() {
    kg_time_t t = kg_time_now();
    kg_date_t d = kg_time_to_date(t);
    time_t ct = time(null);
    struct tm *lt = localtime(&ct);
    kgt_expect_eq(d.year, lt->tm_year + 1900);
    kgt_expect_eq(d.month, kg_cast(kg_month_t)(lt->tm_mon + 1));
    kgt_expect_eq(d.day, lt->tm_mday);
}

void test_time_diff() {
    kg_time_t ta = kg_time_now();
    kg_time_t tb = kg_time_add(ta, kg_duration_create(10));
    kg_duration_t dc = kg_time_diff(ta, tb);
    kgt_expect_true((kg_duration_to_milliseconds(dc) < 0));
}

void test_time_math() {
    kg_duration_t d = kg_duration_create(10);
    kg_time_t ta = kg_time_now();
    kg_time_t tb = kg_time_add(ta, d);
    kgt_expect_true(kg_time_is_before(ta, tb));
    kg_time_t tc = kg_time_sub(tb, d);
    kgt_expect_true(kg_time_is_equal(tc, ta));
}

void test_math() {
    u64 a = 10;
    a = kg_math_pow(a, 3);
    kgt_expect_eq(a, 1000);

    u64 b = 3;
    b = kg_math_pow(b, 3);
    kgt_expect_eq(b, 27);
}

void test_format() {
}

void test_parse() {
    u64 u;
    kg_str_t s = kg_str_create("1234");
    kgt_expect_true(kg_str_to_u64(&u, s));
    kgt_expect_eq(u, 1234);
    kg_str_t b = kg_str_create("-234");
    kgt_expect_false(kg_str_to_u64(&u, b));

    i64 i;
    kg_str_t c = kg_str_create("-100");
    kgt_expect_true(kg_str_to_i64(&i, c));
    kgt_expect_eq(i, -100);
    kg_str_t d = kg_str_create("234");
    kgt_expect_true(kg_str_to_i64(&i, d));
    kgt_expect_eq(i, 234);
}

void test_allocator_temp() {
    kg_allocator_t backing_allocator = kg_allocator_default();
    kg_arena_t arena;
    isize arena_max_size = 4096;
    kg_arena_create(&arena, &backing_allocator, arena_max_size);
    kgt_expect_not_null(arena.real_ptr);
    kgt_expect_eq(arena.max_size, arena_max_size);

    kg_allocator_t temp_allocator = kg_allocator_temp(&arena);
    void* mem = kg_allocator_alloc(&temp_allocator, arena_max_size + 1);
    kgt_expect_null(mem);

    mem = kg_allocator_alloc(&temp_allocator, arena_max_size);
    kgt_expect_not_null(mem);

    kg_allocator_free_all(&temp_allocator, true);
    kgt_expect_not_null(arena.real_ptr);
    kgt_expect_eq(arena.allocated_size, 0);

    kg_allocator_free_all(&temp_allocator, false);
    kgt_expect_null(arena.real_ptr);
}

void test_queue() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_queue_t q;
    kg_queue_create(&q, &allocator, kg_sizeof(char), 64);
    kgt_expect_not_null(q.real_ptr);
    for (isize i = 0; i < 10; i++) {
        char c = 'a' + i;
        kgt_expect_true(kg_queue_enqueue(&q, &c));
    }
    for (isize i = 0; i < 10; i++) {
        char c;
        kgt_expect_true(kg_queue_deque(&q, &c));
        kgt_expect_eq(c, 'a' + i);
    }
    kg_queue_destroy(&q);
    kgt_expect_null(q.real_ptr);
}

typedef struct {
    kg_mutex_t mutex;
    isize      value;
} test_pool_task_st_;

void* test_pool_task_(void* arg) {
    test_pool_task_st_* st = kg_cast(test_pool_task_st_*)arg;
    kg_mutex_lock(&st->mutex);
    st->value += 1;
    kg_mutex_unlock(&st->mutex);
    return null;
}

void test_pool() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_pool_t p;
    isize n = 10;
    isize iter = 120;
    test_pool_task_st_ st = (test_pool_task_st_){.value = 0};
    kg_mutex_create(&st.mutex);
    kg_pool_create(&p, &allocator, n);
    for (isize i = 0; i < iter; i++) {
        kg_pool_add_task(&p, test_pool_task_, &st);
    }
    kg_pool_join(&p);
    kg_mutex_destroy(&st.mutex);
    kgt_expect_eq(st.value, iter);
    kg_pool_destroy(&p);
}

void test_datetime() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_time_t t = kg_time_now();
    kg_datetime_t d = kg_time_to_datetime(t);
    kg_string_t s = kg_datetime_to_string(&allocator, d);
    kgt_expect_false(kg_string_is_empty(s));
    kg_string_destroy(s);
}

void test_date() {
    kg_allocator_t allocator = kg_allocator_default();
    kg_time_t t = kg_time_now();
    kg_date_t d = kg_time_to_date(t);
    kg_string_t s = kg_date_to_string(&allocator, d);
    kgt_expect_false(kg_string_is_empty(s));
    kg_string_destroy(s);
}

void test_quicksort() {
    isize len = 7;
    isize values[7]      = {7,4,1,2,3,5,6};
    isize values_copy[7] = {7,4,1,2,3,5,6};
    isize expected[7]    = {1,2,3,4,5,6,7};
    kg_quicksort(values, 0, 0, kg_sizeof(isize), kg_mem_compare);
    for (isize i = 0; i < len; i++) {
        kgt_expect_eq(values_copy[i], values[i]);
    }
    kg_quicksort(values, 0, len, kg_sizeof(isize), kg_mem_compare);
    for (isize i = 0; i < len; i++) {
        kgt_expect_eq(expected[i], values[i]);
    }
}

void test_string_builder() {
    kg_allocator_t a = kg_allocator_default();
    kg_string_builder_t b;
    isize init_cap = 1;
    kgt_expect_true(kg_string_builder_create(&b, &a, init_cap));
    kgt_expect_eq(kg_string_builder_cap(&b), init_cap);

    char c = 'c';
    kgt_expect_true(kg_string_builder_write_char(&b, c));
    kgt_expect_mem_eq(b.write_ptr, &c, 1);

    kg_string_t f = kg_string_builder_to_string(&b, &a);
    kgt_expect_true(kg_string_is_equal_cstr(f, "c"));
    kg_string_destroy(f);

    kg_str_t str = kg_str_create("str");
    kgt_expect_true(kg_string_builder_write_str(&b, str));
    kgt_expect_mem_eq(b.write_ptr, str.ptr, str.len);

    kg_string_t f2 = kg_string_builder_to_string(&b, &a);
    kgt_expect_true(kg_string_is_equal_cstr(f2, "cstr"));
    kg_string_destroy(f2);

    kg_str_t strn = kg_str_create("str");
    kgt_expect_true(kg_string_builder_write_str_n(&b, strn, 2));
    kgt_expect_mem_eq(b.write_ptr, strn.ptr, 2);

    kg_string_t f3 = kg_string_builder_to_string(&b, &a);
    kgt_expect_true(kg_string_is_equal_cstr(f3, "cstrst"));
    kg_string_destroy(f3);

    const char* cstr = "cstr";
    kgt_expect_true(kg_string_builder_write_cstr(&b, cstr));
    kgt_expect_mem_eq(b.write_ptr, cstr, 4);

    kg_string_t f4 = kg_string_builder_to_string(&b, &a);
    kgt_expect_true(kg_string_is_equal_cstr(f4, "cstrstcstr"));
    kg_string_destroy(f4);

    const char* cstrn = "cstr";
    kgt_expect_true(kg_string_builder_write_cstr_n(&b, cstrn, 2));
    kgt_expect_mem_eq(b.write_ptr, cstrn, 2);

    kg_string_t f5 = kg_string_builder_to_string(&b, &a);
    kgt_expect_true(kg_string_is_equal_cstr(f5, "cstrstcstrcs"));
    kg_string_destroy(f5);

    kgt_expect_true(kg_string_builder_reset(&b));
    kgt_expect_eq(kg_string_builder_len(&b), 0);

    kg_string_builder_destroy(&b);
}

int main() {
    kgt_t t;
    kgt_create(&t);

    kgt_test_t tests[] = {
        kgt_register(test_mem_swap),
        kgt_register(test_file_read_contant),
        kgt_register(test_string_from_cstr),
        kgt_register(test_string_from_fmt),
        kgt_register(test_string_append_cstr),
        kgt_register(test_string_append_fmt),
        kgt_register(test_str_is_equal),
        kgt_register(test_str_index),
        kgt_register(test_str_contains),
        kgt_register(test_str_has_prefix),
        kgt_register(test_str_trim_prefix),
        kgt_register(test_str_has_suffix),
        kgt_register(test_str_trim_suffix),
        kgt_register(test_str_sub),
        kgt_register(test_str_chop_first_split_by),
        kgt_register(test_duration_since),
        kgt_register(test_time_date),
        kgt_register(test_time_diff),
        kgt_register(test_time_math),
        kgt_register(test_math),
        kgt_register(test_format),
        kgt_register(test_parse),
        kgt_register(test_allocator_temp),
        kgt_register(test_queue),
        kgt_register(test_pool),
        kgt_register(test_datetime),
        kgt_register(test_date),
        kgt_register(test_quicksort),
        kgt_register(test_string_builder),
    }; 
    isize tests_len = kg_sizeof(tests) / kg_sizeof(kgt_test_t);

    kgt_run(&t, tests, tests_len);

    kgt_destroy(&t);

    return 0;
}
