#define KG_IMPL
#define KG_FLAGS
#define KG_FLAGS_IMPL
#include "kg.h"

typedef struct v_t {
    kg_str_t key;
    kg_str_t value;
} v_t;

int main(i32 argc, char* argv[]) {
    kg_str_t name;
    b32 help;
    u64 uv;
    i64 iv;
    kg_flag_str(&name, "name", kg_str_create("kgapp"), "set app name");
    kg_flag_b32(&help, "help", false, "show usage");
    kg_flag_u64(&uv, "uv", 234, "uv");
    kg_flag_i64(&iv, "iv", -231, "iv");
    kg_flags_parse(argc, argv);

    if (help) {
        kg_flags_usage();
        kg_exit(0);
    }
    kg_log_info("%.*s", name.len, name.ptr);
    kg_log_info("%lu", uv);
    kg_log_info("%li", iv);

    kg_allocator_t allocator = kg_allocator_default();

    kg_file_content_t content = kg_file_read_content(&allocator, "./src/test.txt");
    kg_assert(content.is_valid);

    kg_str_t s = kg_str_create(content.cstr);
    kg_assert(s.len > 0);
    kg_str_t kv_pair_sep = kg_str_create("\n");
    kg_str_t kv_sep = kg_str_create("=");

    v_t* pairs = kg_darray_create(&allocator, v_t, 1);
    kg_assert(pairs);
    kg_assert(kg_darray_cap(pairs) == 1);
    kg_assert(kg_darray_len(pairs) == 0);
    kg_assert(kg_darray_stride(pairs) == kg_sizeof(v_t));
    kg_darray_grow(pairs, 24);
    kg_assert(pairs);
    kg_assert(kg_darray_cap(pairs) == 25);
    kg_assert(kg_darray_len(pairs) == 0);
    kg_assert(kg_darray_stride(pairs) == kg_sizeof(v_t));

    for (
        kg_str_t kv = kg_str_chop_first_split_by(&s, kv_pair_sep); 
        !kg_str_is_null_or_empty(kv); 
        kv = kg_str_chop_first_split_by(&s, kv_pair_sep)
    ) {
        kg_str_t key = kg_str_chop_first_split_by(&kv, kv_sep);
        kg_str_t value = kv;
        kg_darray_append(pairs, ((v_t){key, value}));
    }

    for (isize i = 0; i < kg_darray_len(pairs); i++) {
        v_t pair = pairs[i];
        kg_str_t k = pair.key;
        kg_str_t v = pair.value;
        kg_log_info("[%.*s]: %.*s", k.len, k.ptr, v.len, v.ptr);
    }

    kg_darray_destroy(pairs);
    kg_allocator_free(&allocator, content.cstr, content.len);

    return 0;
}
