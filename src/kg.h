#ifndef KG_H
#define KG_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

typedef int8_t    i8;
typedef int16_t   i16;
typedef int32_t   i32;
typedef int64_t   i64;
typedef uint8_t   u8;
typedef uint16_t  u16;
typedef uint32_t  u32;
typedef uint64_t  u64;
typedef i32       b32;
typedef float     f32;
typedef double    f64;
typedef i32       rune;
typedef size_t    usize;
typedef ptrdiff_t isize;

#define U8_MIN 0u
#define U8_MAX 0xffu
#define I8_MIN (-0x7f - 1)
#define I8_MAX 0x7f

#define U16_MIN 0u
#define U16_MAX 0xffffu
#define I16_MIN (-0x7fff - 1)
#define I16_MAX 0x7fff

#define U32_MIN 0u
#define U32_MAX 0xffffffffu
#define I32_MIN (-0x7fffffff - 1)
#define I32_MAX 0x7fffffff

#define U64_MIN 0ull
#define U64_MAX 0xffffffffffffffffull
#define I64_MIN (-0x7fffffffffffffffll - 1)
#define I64_MAX 0x7fffffffffffffffll

#define USIZE_MIX U64_MIN
#define USIZE_MAX U64_MAX

#define ISIZE_MIX I64_MIN
#define ISIZE_MAX I64_MAX

#define F32_MIN 1.17549435e-38f
#define F32_MAX 3.40282347e+38f

#define F64_MIN 2.2250738585072014e-308
#define F64_MAX 1.7976931348623157e+308

#define true  1
#define false 0
#define null  NULL

#define KG_TIME_MAX_CHARS_LEN 64
#define KG_U64_MAX_CHARS_LEN  32
#define KG_I64_MAX_CHARS_LEN  32
#define KG_B32_MAX_CHARS_LEN  16
#define KG_F64_MAX_CHARS_LEN  32

#define kg_cast(T)       (T)
#define kg_sizeof(T)     kg_cast(isize)sizeof(T)
#define kg_static_assert static_assert
#define kg_static        static
#define kg_inline        inline
#define kg_extern        extern

#define KG_RUNE_INVALID kg_cast(rune)(0xfffd)
#define KG_RUNE_MAX     kg_cast(rune)(0x0010ffff)

#define kg_min(x, y)           ((x) < (y) ? (x) : (y))
#define kg_max(x, y)           ((x) > (y) ? (x) : (y))
#define kg_clamp(x, i, j)      kg_min(kg_max((x), (i)), (j))
#define kg_abs(x)              ((x) < 0 ? -(x) : (x))
#define kg_is_within(x, i, j)  (((x) >= (i)) && ((x) <= (j)))
#define kg_is_between(x, i, j) (((x) > (i)) && ((x) < (j)))

#define kg_kibibytes(x) (            (x) * (i64)1024)
#define kg_mebibytes(x) (kg_kibibytes(x) * (i64)1024)
#define kg_gibibytes(x) (kg_mebibytes(x) * (i64)1024)
#define kg_tebibytes(x) (kg_gibibytes(x) * (i64)1024)

#define kg_kilobytes(x) (            (x) * (i64)1000)
#define kg_megabytes(x) (kg_kilobytes(x) * (i64)1000)
#define kg_gigabytes(x) (kg_megabytes(x) * (i64)1000)
#define kg_terabytes(x) (kg_gigabytes(x) * (i64)1000)

#if defined(__linux__)
    #define KG_PLATFORM_LINUX 1
#else
    #error "Unknown platform"
#endif

void kg_exit(i32 code);

void kg_printf(const char* fmt, ...);

void kg_assert_handler(const char* prefix, const char* condition, const char* file, isize line, const char* fmt, ...);

#define kg_assert_fmt(cond, fmt, ...) if (cond) {} else {kg_assert_handler("Assertion failed", #cond, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__);}
#define kg_assert(cond)               kg_assert_fmt(cond, null)
#define kg_panic(fmt, ...)            kg_assert_handler("Panic", null, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)

void* kg_mem_alloc_zero(isize size);
void* kg_mem_alloc     (isize size);
void* kg_mem_resize    (void* ptr, isize size);
void  kg_mem_free      (void* ptr);
void* kg_mem_copy      (void* dest, const void* src, isize size);
void* kg_mem_set       (void* dest, u8 byte_value, isize size);
void* kg_mem_zero      (void* dest, isize size);
i32   kg_mem_compare   (const void* a, const void* b, isize size);
void  kg_mem_swap      (void* a, void* b, isize size);
void* kg_mem_move      (void* dest, const void* src, isize size);

typedef struct kg_allocator_t kg_allocator_t;

typedef struct kg_arena_t {
    kg_allocator_t* allocator;
    isize           max_size;
    isize           allocated_size;
    void*           real_ptr;
} kg_arena_t;

b32   kg_arena_create   (kg_arena_t* a, kg_allocator_t* allocator, isize max_size);
void* kg_arena_alloc    (kg_arena_t* a, isize size);
isize kg_arena_allocated(const kg_arena_t* a);
isize kg_arena_available(const kg_arena_t* a);
isize kg_arena_mem_size (const kg_arena_t* a);
void  kg_arena_reset    (kg_arena_t* a);
void  kg_arena_destroy  (kg_arena_t* a);

kg_allocator_t kg_allocator_default (void);
kg_allocator_t kg_allocator_temp    (kg_arena_t* a);
typedef struct kg_allocator_tracking_context_t {
    const char*     name;
    kg_allocator_t* parent_allocator;
    isize           total_allocated;
    isize           total_freed;
    isize           current_allocated;
    isize           alloc_count;
    isize           free_count;
    isize           resize_count;
} kg_allocator_tracking_context_t;
void           kg_allocator_tracking_context_print(const kg_allocator_tracking_context_t ctx);
kg_allocator_t kg_allocator_tracking(kg_allocator_tracking_context_t* ctx);

void* kg_allocator_alloc   (kg_allocator_t* a, isize s);
void  kg_allocator_free    (kg_allocator_t* a, void* ptr, isize s);
void  kg_allocator_free_all(kg_allocator_t* a, b32 clear);
void* kg_allocator_resize  (kg_allocator_t* a, void* ptr, isize old_size, isize new_size);

typedef void* (*kg_allocator_allocate_fn_t)(kg_allocator_t* a, isize size);
typedef void  (*kg_allocator_free_fn_t)    (kg_allocator_t* a, void* ptr, isize size);
typedef void  (*kg_allocator_free_all_fn_t)(kg_allocator_t* a, b32 clear);
typedef void* (*kg_allocator_resize_fn_t)  (kg_allocator_t* a, void* ptr, isize old_size, isize new_size);

typedef struct kg_allocator_t {
    struct {
        kg_allocator_allocate_fn_t alloc;
        kg_allocator_free_fn_t     free;
        kg_allocator_free_all_fn_t free_all;
        kg_allocator_resize_fn_t   resize;
    } proc;
    void* context;
} kg_allocator_t;

typedef i32 (*kg_compare_fn_t)(const void* a, const void* b);

void kg_quicksort(void* src, isize lo, isize hi, isize stride, kg_compare_fn_t compare_fn);

i32   kg_cstr_compare     (const void* a, const void* b);
i32   kg_cstr_compare_n   (const void* a, const void* b, isize n);
i32   kg_cstr_compare_ci  (const void* a, const void* b);
i32   kg_cstr_compare_ci_n(const void* a, const void* b, isize n);
isize kg_cstr_len         (const char* c);
isize kg_cstr_len_n       (const char* c, isize n);

typedef struct kg_string_header_t {
    isize           len;
    isize           cap;
    kg_allocator_t* allocator;
} kg_string_header_t;

typedef struct kg_str_t kg_str_t;

#define kg_string_header(s) (kg_cast(kg_string_header_t*)(s) - 1)

typedef char* kg_string_t;

kg_string_t kg_string_create          (kg_allocator_t* a, isize cap);
kg_string_t kg_string_from_unsafe     (kg_allocator_t* a, const void* v, isize v_len);
kg_string_t kg_string_from_fmt        (kg_allocator_t* a, const char* fmt, ...);
kg_string_t kg_string_from_fmt_v      (kg_allocator_t* a, const char* fmt, va_list args);
kg_string_t kg_string_from_str        (kg_allocator_t* a, const kg_str_t s);
kg_string_t kg_string_from_str_n      (kg_allocator_t* a, const kg_str_t s, isize n);
kg_string_t kg_string_from_cstr       (kg_allocator_t* a, const char* cstr);
kg_string_t kg_string_from_cstr_n     (kg_allocator_t* a, const char* cstr, isize cstr_len);
kg_string_t kg_string_set             (kg_string_t s, const char* cstr);
kg_string_t kg_string_append          (kg_string_t s, kg_string_t other);
kg_string_t kg_string_append_unsafe   (kg_string_t s, const void* v, isize v_len);
kg_string_t kg_string_append_fmt      (kg_string_t s, const char* fmt, ...);
kg_string_t kg_string_append_fmt_v    (kg_string_t s, const char* fmt, va_list args);
kg_string_t kg_string_append_char     (kg_string_t s, char c);
kg_string_t kg_string_append_rune     (kg_string_t s, rune r);
kg_string_t kg_string_append_cstr     (kg_string_t s, const char* cstr);
kg_string_t kg_string_append_cstr_n   (kg_string_t s, const char* cstr, isize cstr_len);
kg_string_t kg_string_grow            (kg_string_t s, isize n);
kg_string_t kg_string_ensure_available(kg_string_t s, isize n);
isize       kg_string_len             (const kg_string_t s);
isize       kg_string_cap             (const kg_string_t s);
isize       kg_string_available       (const kg_string_t s);
isize       kg_string_mem_size        (const kg_string_t s);
b32         kg_string_is_valid        (const kg_string_t s);
b32         kg_string_is_equal        (const kg_string_t s, const kg_string_t other);
b32         kg_string_is_equal_cstr   (const kg_string_t s, const char* cstr);
b32         kg_string_is_empty        (const kg_string_t s);
i32         kg_string_compare         (const void* s, const void* other);
i32         kg_string_compare_n       (const void* s, const void* other, isize n);
i32         kg_string_compare_ci      (const void* s, const void* other);
i32         kg_string_compare_ci_n    (const void* s, const void* other, isize n);
u32         kg_string_utf8_len        (const kg_string_t s);
u32         kg_string_utf8_len_n      (const kg_string_t s, isize n);
void        kg_string_reset           (kg_string_t s);
void        kg_string_destroy         (kg_string_t s);

typedef struct kg_str_t {
    isize       len;
    const char* ptr;
} kg_str_t;

kg_str_t kg_str_create             (const char* cstr);
kg_str_t kg_str_create_n           (const char* cstr, isize cstr_len);
kg_str_t kg_str_create_empty       (void);
kg_str_t kg_str_create_null        (void);
kg_str_t kg_str_from_string        (const kg_string_t s);
kg_str_t kg_str_from_string_n      (const kg_string_t s, isize len);
kg_str_t kg_str_chop_first_split_by(kg_str_t* s, const kg_str_t split_by);
kg_str_t kg_str_chop_first_line    (kg_str_t* s);
kg_str_t kg_str_substr             (const kg_str_t s, isize start_inc, isize end_exc);
kg_str_t kg_str_substr_from        (const kg_str_t s, isize start_inc);
kg_str_t kg_str_substr_to          (const kg_str_t s, isize end_exc);
kg_str_t kg_str_trim_space         (const kg_str_t s);
kg_str_t kg_str_trim_space_left    (const kg_str_t s);
kg_str_t kg_str_trim_space_right   (const kg_str_t s);
kg_str_t kg_str_trim_prefix        (const kg_str_t s, const kg_str_t prefix);
kg_str_t kg_str_trim_suffix        (const kg_str_t s, const kg_str_t suffix);
b32      kg_str_is_empty           (const kg_str_t s);
b32      kg_str_is_null            (const kg_str_t s);
b32      kg_str_is_null_or_empty   (const kg_str_t s);
b32      kg_str_is_valid_cstr      (const kg_str_t s);
b32      kg_str_is_equal           (const kg_str_t s, const kg_str_t other);
b32      kg_str_contains           (const kg_str_t s, const kg_str_t needle);
b32      kg_str_has_prefix         (const kg_str_t s, const kg_str_t prefix);
b32      kg_str_has_suffix         (const kg_str_t s, const kg_str_t suffix);
isize    kg_str_index              (const kg_str_t s, const kg_str_t needle);
isize    kg_str_index_char         (const kg_str_t s, char needle);
i32      kg_str_compare            (const void* s, const void* other);
i32      kg_str_compare_n          (const void* s, const void* other, isize n);
i32      kg_str_compare_ci         (const void* s, const void* other);
i32      kg_str_compare_ci_n       (const void* s, const void* other, isize n);
u32      kg_str_utf8_len           (const kg_str_t s);
u32      kg_str_utf8_len_n         (const kg_str_t s, isize n);

b32 kg_str_to_b32(b32* b, const kg_str_t s);
b32 kg_str_to_u64(u64* u, const kg_str_t s);
b32 kg_str_to_i64(i64* i, const kg_str_t s);

isize       kg_b32_to_cstr  (char buf[KG_B32_MAX_CHARS_LEN], b32 b);
kg_string_t kg_b32_to_string(kg_allocator_t* a, b32 b);
isize       kg_u64_to_cstr  (char buf[KG_U64_MAX_CHARS_LEN], u64 u);
kg_string_t kg_u64_to_string(kg_allocator_t* a, u64 u);
isize       kg_i64_to_cstr  (char buf[KG_I64_MAX_CHARS_LEN], i64 i);
kg_string_t kg_i64_to_string(kg_allocator_t* a, i64 i);
isize       kg_f64_to_cstr  (char buf[KG_F64_MAX_CHARS_LEN], f64 f);
kg_string_t kg_f64_to_string(kg_allocator_t* a, f64 f);

char kg_char_to_lower       (char c);
char kg_char_to_upper       (char c);
b32  kg_char_is_space       (char c);
b32  kg_char_is_digit       (char c);
b32  kg_char_is_alpha       (char c);
b32  kg_char_is_alphanumeric(char c);

b32   kg_rune_is_valid   (rune r);
b32   kg_rune_is_digit   (rune r);
b32   kg_rune_is_space   (rune r);
isize kg_rune_len        (rune r);
isize kg_utf8_decode_rune(rune* r, u8* b, isize b_len);
isize kg_utf8_encode_rune(u8 b[4], rune r);

typedef struct kg_string_builder_t {
    kg_allocator_t* allocator;
    isize           cap;
    isize           len;
    char*           write_ptr;
    char*           real_ptr;
} kg_string_builder_t;

b32         kg_string_builder_create          (kg_string_builder_t* b, kg_allocator_t* a, isize cap);
b32         kg_string_builder_write_unsafe    (kg_string_builder_t* b, const void* v, isize n);
b32         kg_string_builder_write_u64       (kg_string_builder_t* b, u64 u);
b32         kg_string_builder_write_i64       (kg_string_builder_t* b, i64 i);
b32         kg_string_builder_write_f64       (kg_string_builder_t* b, f64 f);
b32         kg_string_builder_write_char      (kg_string_builder_t* b, char c);
b32         kg_string_builder_write_rune      (kg_string_builder_t* b, rune r);
b32         kg_string_builder_write_fmt       (kg_string_builder_t* b, const char* fmt, ...);
b32         kg_string_builder_write_fmt_v     (kg_string_builder_t* b, const char* fmt, va_list args);
b32         kg_string_builder_write_cstr      (kg_string_builder_t* b, const char* c);
b32         kg_string_builder_write_cstr_n    (kg_string_builder_t* b, const char* c, isize n);
b32         kg_string_builder_write_str       (kg_string_builder_t* b, const kg_str_t s);
b32         kg_string_builder_write_str_n     (kg_string_builder_t* b, const kg_str_t s, isize n);
b32         kg_string_builder_write_string    (kg_string_builder_t* b, const kg_string_t s);
b32         kg_string_builder_write_string_n  (kg_string_builder_t* b, const kg_string_t s, isize n);
b32         kg_string_builder_grow            (kg_string_builder_t* b, isize n);
b32         kg_string_builder_grow_formula    (kg_string_builder_t* b, isize n);
b32         kg_string_builder_ensure_available(kg_string_builder_t* b, isize n);
kg_string_t kg_string_builder_to_string       (const kg_string_builder_t* b, kg_allocator_t* a);
isize       kg_string_builder_mem_size        (const kg_string_builder_t* b);
isize       kg_string_builder_cap             (const kg_string_builder_t* b);
isize       kg_string_builder_len             (const kg_string_builder_t* b);
isize       kg_string_builder_available       (const kg_string_builder_t* b);
void        kg_string_builder_reset           (kg_string_builder_t* b);
void        kg_string_builder_destroy         (kg_string_builder_t* b);

typedef struct kg_darray_base_t {
    isize           len;
    isize           cap;
    isize           stride;
    kg_allocator_t* allocator;
} kg_darray_base_t;

void* kg_darray_create2_          (kg_allocator_t* a, isize stride, isize cap, kg_darray_base_t* out_b);
void* kg_darray_grow2_            (kg_darray_base_t* b, isize n, void** out_ptr);
void* kg_darray_grow_formula2_    (kg_darray_base_t* b, isize n, void** out_ptr);
void* kg_darray_ensure_available2_(kg_darray_base_t* b, isize n, void** out_ptr);

#define KG_DARRAY_TYPEDEF(T, name) \
    typedef struct kg_darray_##name##_t { \
        kg_darray_base_t base; \
        T*               ptr; \
    } kg_darray_##name##_t; \
    kg_static kg_inline kg_darray_##name##_t kg_darray_##name##_create(kg_allocator_t* a, isize cap) { \
        kg_darray_##name##_t out = {0}; \
        out.ptr = kg_cast(T*)kg_darray_create2_(a, kg_sizeof(T), cap, &out.base); \
        return out; \
    } \
    kg_static kg_inline b32 kg_darray_##name##_append(kg_darray_##name##_t* d, T v) { \
        if (kg_darray_ensure_available2_(&d->base, 1, kg_cast(void**)&d->ptr)) { \
            d->ptr[d->base.len] = v; \
            d->base.len++; \
            return true; \
        } \
        return false; \
    } \
    kg_static kg_inline isize kg_darray_##name##_len(const kg_darray_##name##_t* d) { \
        return d ? d->base.len : 0; \
    } \
    kg_static kg_inline isize kg_darray_##name##_cap(const kg_darray_##name##_t* d) { \
        return d ? d->base.cap : 0; \
    } \
    kg_static kg_inline isize kg_darray_##name##_stride(const kg_darray_##name##_t* d) { \
        return d ? d->base.stride : 0; \
    } \
    kg_static kg_inline isize kg_darray_##name##_available(const kg_darray_##name##_t* d) { \
        return d ? d->base.cap - d->base.len : 0; \
    } \
    kg_static kg_inline b32 kg_darray_##name##_grow(kg_darray_##name##_t* d, isize n) { \
        return null != kg_darray_grow2_(&d->base, n, kg_cast(void**)&d->ptr); \
    } \
    kg_static kg_inline b32 kg_darray_##name##_grow_formula(kg_darray_##name##_t* d, isize n) { \
        return null != kg_darray_grow_formula2_(&d->base, n, kg_cast(void**)&d->ptr); \
    } \
    kg_static kg_inline b32 kg_darray_##name##_pop(kg_darray_##name##_t* d) { \
        if (d->base.len > 0) { \
            d->base.len--; \
            return true; \
        } \
        return false; \
    } \
    kg_static kg_inline b32 kg_darray_##name##_swap_remove(kg_darray_##name##_t* d, isize i) { \
        if (kg_is_within(i, 0, kg_darray_##name##_len(d) - 1)) { \
            kg_mem_swap(d->ptr + i, d->ptr + kg_darray_##name##_len(d), d->base.stride); \
            d->base.len--; \
            return true; \
        } \
        return false; \
    } \
    kg_static kg_inline isize kg_darray_##name##_mem_size(const kg_darray_##name##_t* d) { \
        return kg_darray_##name##_cap(d) * kg_darray_##name##_stride(d); \
    } \
    kg_static kg_inline void kg_darray_##name##_destroy(kg_darray_##name##_t* d) { \
        if (d) { \
            kg_allocator_free(d->base.allocator, d->ptr, kg_darray_##name##_mem_size(d)); \
            kg_mem_zero(d, kg_sizeof(kg_darray_##name##_t)); \
        } \
    }

KG_DARRAY_TYPEDEF(b32, b32)
KG_DARRAY_TYPEDEF(i8, i8)
KG_DARRAY_TYPEDEF(u8, u8)
KG_DARRAY_TYPEDEF(i16, i16)
KG_DARRAY_TYPEDEF(u16, u16)
KG_DARRAY_TYPEDEF(i32, i32)
KG_DARRAY_TYPEDEF(u32, u32)
KG_DARRAY_TYPEDEF(i64, i64)
KG_DARRAY_TYPEDEF(u64, u64)
KG_DARRAY_TYPEDEF(f32, f32)
KG_DARRAY_TYPEDEF(f64, f64)
KG_DARRAY_TYPEDEF(rune, rune)
KG_DARRAY_TYPEDEF(isize, isize)
KG_DARRAY_TYPEDEF(usize, usize)
KG_DARRAY_TYPEDEF(kg_string_t, string)
KG_DARRAY_TYPEDEF(kg_str_t, str)
KG_DARRAY_TYPEDEF(char, char)
KG_DARRAY_TYPEDEF(const char*, cstr)
KG_DARRAY_TYPEDEF(const void*, void)

typedef struct kg_darray_header_t {
    isize           len;
    isize           cap;
    isize           stride;
    kg_allocator_t* allocator;
} kg_darray_header_t;

void*   kg_darray_create_                (kg_allocator_t* a, isize stride, isize cap);
void*   kg_darray_grow_                  (void* d, isize n);
#define kg_darray_header(d)              (kg_cast(kg_darray_header_t*)d - 1)
#define kg_darray_create(a, T, cap)      kg_cast(T*)kg_darray_create_(a, kg_sizeof(T), cap)
#define kg_darray_pop(d)                 do { kg_darray_header_t* h = kg_darray_header(d); if (h->len > 0) { h->len--; } } while(0)
#define kg_darray_swap_remove(d, i)      do { \
    kg_darray_header_t* h = kg_darray_header(d); \
    if (0 <= i && i < h->len) { \
        kg_mem_swap(&d[h->len - 1], &d[i], h->stride); \
        h->len--; \
    } \
} while(0)
#define kg_darray_append(d, item)        do { \
    kg_darray_ensure_available(d, 1); \
    (d)[kg_darray_header(d)->len++] = (item); \
} while(0);
#define kg_darray_len(d)                 (d ? kg_darray_header(d)->len : 0)
#define kg_darray_cap(d)                 (d ? kg_darray_header(d)->cap : 0)
#define kg_darray_stride(d)              (d ? kg_darray_header(d)->stride : 0)
#define kg_darray_allocator(d)           (d ? kg_darray_header(d)->allocator : null)
#define kg_darray_grow(d, n)             do { (d) = kg_darray_grow_(d, n); } while(0)
#define kg_darray_grow_formula(d, n)     do { (d) = kg_darray_grow_(d, kg_darray_cap(d) + n); } while(0)
#define kg_darray_available(d)           (kg_darray_cap(d) > kg_darray_len(d) ? kg_darray_cap(d) - kg_darray_len(d) : 0)
#define kg_darray_ensure_available(d, n) do { \
    if (kg_darray_available(d) < n) { \
        kg_darray_grow_formula(d, n); \
    } \
} while(0)
#define kg_darray_mem_size(d)            (kg_sizeof(kg_darray_header_t) + kg_darray_cap(d) * kg_darray_stride(d))
#define kg_darray_destroy(d)             do { \
    kg_darray_header_t* h = kg_darray_header(d); \
    kg_allocator_free(h->allocator, h, kg_darray_mem_size(d)); \
} while(0)

/*typedef struct kg_map_kv_t kg_map_kv_t;*/
/**/
/*typedef struct kg_map_base_t {*/
/*    kg_allocator_t* allocator;*/
/*    kg_map_kv_t*    buckets;*/
/*    isize           kv_count;*/
/*    isize           key_size;*/
/*    isize           value_size;*/
/*} kg_map_base_t;*/
/**/
/*#define KG_MAP_TYPEDEF(K, T, name) \*/
/*    typedef struct kg_map_##name##_t { \*/
/*    } kg_map_##name##_t; \*/
/*    b32 kg_map_##name##_create(kg_map_##name##_t* m, kg_allocator_t* a, isize buckets_len) { \*/
/*        b32 ok; \*/
/*        if (m && a && buckets > 0) { \*/
/*            m->kv_count = 0; \*/
/*            m->allocator = a; \*/
/*            m->buckets = kg_allocator_alloc(a, kg_sizeof(kg_map_kv_t) * buckets); \*/
/*            if (m->buckets) { \*/
/*                ok = true; \*/
/*            } \*/
/*        } \*/
/*        return ok; \*/
/*    }*/

typedef struct kg_queue_t {
    kg_allocator_t* allocator;
    isize           len;
    isize           cap;
    isize           stride;
    void*           real_ptr;
} kg_queue_t;

b32    kg_queue_create          (kg_queue_t* q, kg_allocator_t* allocator, isize stride, isize cap);
b32    kg_queue_peek            (const kg_queue_t* q, void* o);
b32    kg_queue_enqueue         (kg_queue_t* q, const void* o);
b32    kg_queue_deque           (kg_queue_t* q, void* o);
b32    kg_queue_grow            (kg_queue_t* q, isize n);
b32    kg_queue_ensure_available(kg_queue_t* q, isize n);
b32    kg_queue_is_empty        (const kg_queue_t* q);
isize  kg_queue_available       (const kg_queue_t* q);
isize  kg_queue_len             (const kg_queue_t* q);
isize  kg_queue_cap             (const kg_queue_t* q);
isize  kg_queue_mem_size        (const kg_queue_t* q);
void   kg_queue_destroy         (kg_queue_t* q);

typedef enum kg_file_mode_t {
    KG_FILE_MODE_READ  = 0x1,
    KG_FILE_MODE_WRITE = 0x2,
} kg_file_mode_t;

typedef struct kg_file_t {
    void* handle;
    b32   is_valid;
} kg_file_t;

typedef struct kg_file_content_t {
    char*           cstr;
    isize           len;
    kg_allocator_t* allocator;
    b32             is_valid;
} kg_file_content_t;

b32               kg_file_open           (kg_file_t* f, const char* filename, kg_file_mode_t mode, b32 binary);
isize             kg_file_size           (kg_file_t* f);
kg_file_content_t kg_file_content_read   (kg_allocator_t* a, const char* filename);
void              kg_file_content_destroy(kg_file_content_t* fc);
b32               kg_file_close          (kg_file_t* f);

typedef struct kg_time_t {
    struct timespec wall;      // CLOCK_REALTIME
    struct timespec monotonic; // CLOCK_MONOTONIC
} kg_time_t;

typedef struct kg_duration_t {
    time_t sec;
    long   nsec;
} kg_duration_t;

typedef enum kg_month_t {
    KG_MONTH_JANUARY   = 1, 
    KG_MONTH_FEBRUARY, 
    KG_MONTH_MARCH, 
    KG_MONTH_APRIL, 
    KG_MONTH_MAY, 
    KG_MONTH_JUNE, 
    KG_MONTH_JULY, 
    KG_MONTH_AUGUST, 
    KG_MONTH_SEPTEMBER, 
    KG_MONTH_OCTOBER, 
    KG_MONTH_NOVEMBER, 
    KG_MONTH_DECEMBER, 
} kg_month_t;

typedef enum kg_weekday_t {
    KG_WEEKDAY_SUNDAY    = 0,
    KG_WEEKDAY_MONDAY,
    KG_WEEKDAY_TUESDAY,
    KG_WEEKDAY_WEDNESDAY,
    KG_WEEKDAY_THURSDAY,
    KG_WEEKDAY_FRIDAY,
    KG_WEEKDAY_SATURDAY,
} kg_weekday_t;

/*const u64 KG_NANOSECOND  = 1;*/
/*const u64 KG_MICROSECOND = 1000 * KG_NANOSECOND;*/
/*const u64 KG_MILLISECOND = 1000 * KG_MICROSECOND;*/
/*const u64 KG_SECOND      = 1000 * KG_MILLISECOND;*/
/*const u64 KG_MINUTE      = 60 * KG_SECOND;*/
/*const u64 KG_HOUR        = 60 * KG_MINUTE;*/

void          kg_time_sleep        (const kg_duration_t d);
kg_time_t     kg_time_now          (void);
kg_time_t     kg_time_add          (const kg_time_t t, const kg_duration_t d);
kg_time_t     kg_time_sub          (const kg_time_t t, const kg_duration_t d);
b32           kg_time_is_equal     (const kg_time_t t, const kg_time_t o);
b32           kg_time_is_after     (const kg_time_t t, const kg_time_t o);
b32           kg_time_is_before    (const kg_time_t t, const kg_time_t o);
kg_duration_t kg_time_diff         (const kg_time_t t, const kg_time_t o);
kg_duration_t kg_time_since        (const kg_time_t t);
/*i32           kg_time_to_year      (const kg_time_t t);*/
/*kg_month_t    kg_time_to_month     (const kg_time_t t);*/
/*kg_weekday_t  kg_time_to_day       (const kg_time_t t);*/
kg_string_t   kg_time_to_string    (const kg_time_t t, kg_allocator_t* a);
isize         kg_time_to_cstr      (const kg_time_t t, char* b);
kg_string_t   kg_time_now_as_string(kg_allocator_t* a);

kg_duration_t kg_duration_create           (i64 seconds, i64 nanoseconds);
kg_duration_t kg_duration_from_milliseconds(i64 milliseconds);
i64           kg_duration_to_milliseconds  (const kg_duration_t d);
f64           kg_duration_to_hours         (const kg_duration_t d);
i64           kg_duration_to_seconds       (const kg_duration_t d);
kg_string_t   kg_duration_to_string        (const kg_duration_t d, kg_allocator_t* a);
isize         kg_duration_to_cstr          (const kg_duration_t d, char* b);

f64 kg_math_pow     (f64 base, f64 exponent);
i32 kg_isize_compare(const void* a, const void* b);
i32 kg_u64_compare  (const void* a, const void* b);
i32 kg_i64_compare  (const void* a, const void* b);

#ifdef KG_IMPL

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>

kg_inline void* kg_mem_alloc_zero(isize size) {
    return calloc(1, size);
}
kg_inline void* kg_mem_alloc(isize size) {
    return malloc(size);
}
kg_inline void* kg_mem_resize(void* ptr, isize size) {
    return realloc(ptr, size);
}
kg_inline void kg_mem_free(void* ptr) {
    free(ptr);
}
kg_inline void* kg_mem_copy(void* dest, const void* src, isize size) {
    return memcpy(dest, src, size);
}
kg_inline void* kg_mem_set(void* dest, u8 byte_value, isize size) {
    return memset(dest, byte_value, size);
}
kg_inline void* kg_mem_zero(void* dest, isize size) {
    return memset(dest, 0, size);
}
kg_inline i32 kg_mem_compare(const void* a, const void* b, isize size) {
    return kg_cast(i32)memcmp(a, b, size);
}
kg_inline void kg_mem_swap(void* a, void* b, isize size) {
    u8 temp[size];
    kg_mem_copy(temp, a, size);
    kg_mem_copy(a, b, size);
    kg_mem_copy(b, temp, size);
}
kg_inline void* kg_mem_move(void* dest, const void* src, isize size) {
    return memmove(dest, src, size);
}

kg_inline void* kg_allocator_alloc(kg_allocator_t* a, isize s) {
    return a->proc.alloc(a, s);
}
kg_inline void kg_allocator_free(kg_allocator_t* a, void* ptr, isize s) {
    a->proc.free(a, ptr, s);
}
kg_inline void kg_allocator_free_all(kg_allocator_t* a, b32 clear) {
    a->proc.free_all(a, clear);
}
kg_inline void* kg_allocator_resize(kg_allocator_t* a, void* ptr, isize old_size, isize new_size) {
    return a->proc.resize(a, ptr, old_size, new_size);
}

void* kg_allocator_default_alloc(kg_allocator_t* a, isize size) {
    kg_cast(void)a;
    return kg_mem_alloc_zero(size);
}
void kg_allocator_default_free(kg_allocator_t* a, void* ptr, isize size) {
    kg_cast(void)a;
    kg_cast(void)size;
    kg_mem_free(ptr);
}
void kg_allocator_default_free_all(kg_allocator_t* a, b32 clear) {
    kg_cast(void)a;
    kg_cast(void)clear;
}
void* kg_allocator_default_resize(kg_allocator_t* a, void* ptr, isize old_size, isize new_size) {
    kg_cast(void)a;
    kg_cast(void)old_size;
    return kg_mem_resize(ptr, new_size);
}
kg_inline kg_allocator_t kg_allocator_default(void) {
    return (kg_allocator_t){
        .proc = {
            .alloc    = kg_allocator_default_alloc,
            .free     = kg_allocator_default_free,
            .free_all = kg_allocator_default_free_all,
            .resize   = kg_allocator_default_resize,
        },
        .context = null,
    };
}

void* kg_allocator_tracking_alloc(kg_allocator_t* a, isize size) {
    kg_allocator_tracking_context_t* ctx = kg_cast(kg_allocator_tracking_context_t*)a->context;
    void* out_ptr = kg_allocator_alloc(ctx->parent_allocator, size);
    if (out_ptr) {
        ctx->total_allocated += size;
        ctx->alloc_count++;
        ctx->current_allocated = ctx->total_allocated - ctx->total_freed;
        kg_printf("[allocator](%s) alloc %lliB at %p\n", ctx->name, size, out_ptr);
    }
    return out_ptr;
}
void kg_allocator_tracking_free(kg_allocator_t* a, void* ptr, isize size) {
    kg_allocator_tracking_context_t* ctx = kg_cast(kg_allocator_tracking_context_t*)a->context;
    if (ptr) {
        ctx->total_freed += size;
        ctx->current_allocated = ctx->total_allocated - ctx->total_freed;
        ctx->free_count++;
        kg_allocator_free(ctx->parent_allocator, ptr, size);
        kg_printf("[allocator](%s) free %lliB at %p\n", ctx->name, size, ptr);
    }
}
void kg_allocator_tracking_free_all(kg_allocator_t* a, b32 clear) {
    kg_allocator_tracking_context_t* ctx = kg_cast(kg_allocator_tracking_context_t*)a->context;
    ctx->total_freed = ctx->total_allocated;
    ctx->current_allocated = 0;
    kg_allocator_free_all(ctx->parent_allocator, clear);
    kg_printf("[allocator](%s) Free all (clear=%s)\n", ctx->name, clear ? "true" : "false");
}
void* kg_allocator_tracking_resize(kg_allocator_t* a, void* ptr, isize old_size, isize new_size) {
    kg_allocator_tracking_context_t* ctx = kg_cast(kg_allocator_tracking_context_t*)a->context;
    void* out_ptr = null;
    if (ptr) {
        out_ptr = kg_allocator_resize(ctx->parent_allocator, ptr, old_size, new_size);
        if (out_ptr) {
            ctx->resize_count++;
            ctx->total_allocated += new_size;
            ctx->total_freed += old_size;
            ctx->current_allocated = ctx->total_allocated - ctx->total_freed;
            kg_printf("[allocator](%s) resize from %lliB to %lliB, ptr %p -> %p\n", ctx->name, old_size, new_size, ptr, out_ptr);
        }
    }
    return out_ptr;
}
void kg_allocator_tracking_context_print(const kg_allocator_tracking_context_t ctx) {
    kg_printf("[allocator](%s)\n\tparent_allocator:  %p\n\talloc_count:       %lli\n\tfree_count:        %lli\n\tresize_count:      %lli\n\ttotal_allocated:   %lliB\n\ttotal_freed:       %lliB\n\tcurrent_allocated: %lliB\n", 
              ctx.name, 
              ctx.parent_allocator,
              ctx.alloc_count, 
              ctx.free_count, 
              ctx.resize_count, 
              ctx.total_allocated, 
              ctx.total_freed, 
              ctx.current_allocated);
}
kg_inline kg_allocator_t kg_allocator_tracking(kg_allocator_tracking_context_t* ctx) {
    kg_assert(ctx);
    kg_assert(ctx->parent_allocator);
    kg_assert(ctx->name);
    return (kg_allocator_t){
        .proc = {
            .alloc    = kg_allocator_tracking_alloc,
            .free     = kg_allocator_tracking_free,
            .free_all = kg_allocator_tracking_free_all,
            .resize   = kg_allocator_tracking_resize,
        },
        .context = ctx,
    };
}

b32 kg_arena_create(kg_arena_t* a, kg_allocator_t* allocator, isize max_size) {
    b32 out_ok = false;
    kg_arena_t arena = {
        .allocator      = allocator,
        .real_ptr       = kg_allocator_alloc(allocator, max_size),
        .max_size       = max_size,
        .allocated_size = 0,
    };
    if (arena.real_ptr) {
        *a = arena;
        out_ok = true;
    }
    return out_ok;
}
void* kg_arena_alloc(kg_arena_t* a, isize size) {
    void* out = null;
    if (a && size > 0) {
        isize available = kg_arena_available(a);
        if (available >= size) {
            out = kg_cast(u8*)a->real_ptr + a->allocated_size;
            if (out) {
                a->allocated_size += size;
            }
        }
    }
    return out;
}
kg_inline isize kg_arena_allocated(const kg_arena_t* a) {
    return a ? a->allocated_size : 0;
}
kg_inline isize kg_arena_available(const kg_arena_t* a) {
    isize out = 0;
    if (a && a->max_size > a->allocated_size) {
        out = a->max_size - a->allocated_size;
    }
    return out;
}
kg_inline isize kg_arena_mem_size(const kg_arena_t* a) {
    return a ? a->max_size : 0;
}
void kg_arena_reset(kg_arena_t* a) {
    if (a) {
        kg_mem_zero(a->real_ptr, kg_arena_mem_size(a));
        a->allocated_size = 0;
    }
}
void kg_arena_destroy(kg_arena_t* a) {
    if (a) {
        kg_allocator_free(a->allocator, a->real_ptr, kg_arena_mem_size(a));
        kg_mem_zero(a, kg_sizeof(kg_arena_t));
    }
}

void* kg_allocator_temp_alloc(kg_allocator_t* a, isize size) {
    kg_arena_t* arena = kg_cast(kg_arena_t*)a->context;
    return kg_arena_alloc(arena, size);
}
void kg_allocator_temp_free(kg_allocator_t* a, void* ptr, isize size) {
    kg_cast(void)a;
    kg_cast(void)ptr;
    kg_cast(void)size;
}
void kg_allocator_temp_free_all(kg_allocator_t* a, b32 clear) {
    kg_arena_t* arena = kg_cast(kg_arena_t*)a->context;
    if (clear) {
        kg_arena_reset(arena);
    } else {
        kg_arena_destroy(arena);
    }
}
void* kg_allocator_temp_resize(kg_allocator_t* a, void* ptr, isize old_size, isize new_size) {
    kg_cast(void)old_size;
    void* out_mem = null;
    kg_arena_t* arena = kg_cast(kg_arena_t*)a->context;
    out_mem = kg_arena_alloc(arena, new_size);
    if (out_mem) {
        kg_mem_copy(out_mem, ptr, old_size);
    }
    return out_mem;
}
kg_inline kg_allocator_t kg_allocator_temp(kg_arena_t* a) {
    return (kg_allocator_t){
        .proc = {
            .alloc    = kg_allocator_temp_alloc,
            .free     = kg_allocator_temp_free,
            .free_all = kg_allocator_temp_free_all,
            .resize   = kg_allocator_temp_resize,
        },
        .context = a,
    };
}

void kg_quicksort(void* src, isize start_inc, isize end_exc, isize stride, kg_compare_fn_t compare_fn) {
    if (src == null || stride == 0 || compare_fn == null || start_inc >= end_exc) {
        return;
    }
    u8* casted_src = kg_cast(u8*)src;
    isize i = start_inc;
    isize j = end_exc - 1;
    u8* p = casted_src + ((start_inc + (end_exc - start_inc) / 2) * stride);
    while(i <= j) {
        while(compare_fn(casted_src + (i * stride), p) < 0) {i++;}
        while(compare_fn(casted_src + (j * stride), p) > 0) {j--;}
        if (i <= j) {
            kg_mem_swap(casted_src + (i * stride), casted_src + (j * stride), stride);
            i++;
            j--;
        }
    }
    if (start_inc < j) {
        kg_quicksort(src, start_inc, j + 1, stride, compare_fn);
    }
    if (i < end_exc) {
        kg_quicksort(src, i - 1, end_exc, stride, compare_fn);
    }
}

kg_inline i32 kg_cstr_compare(const void* a, const void* b) {
    return strncmp(a, b, ISIZE_MAX);
}
kg_inline i32 kg_cstr_compare_n(const void* a, const void* b, isize n) {
    return strncmp(a, b, n);
}
kg_inline i32 kg_cstr_compare_ci(const void* a, const void* b) {
    return kg_cstr_compare_ci_n(a, b, ISIZE_MAX);
}
kg_inline i32 kg_cstr_compare_ci_n(const void* a, const void* b, isize n) {
    const char* cstr_a = kg_cast(const char*)a;
    const char* cstr_b = kg_cast(const char*)b;
    i32 out = 0;
    isize i = 0;
    if (n > 0) {
        while(i++ < n && *cstr_a && *cstr_b) {
            out = kg_char_to_lower(*cstr_a++) - kg_char_to_lower(*cstr_b++);
            if (out != 0) {
                break;
            }
        }
    }
    return out;
}
kg_inline isize kg_cstr_len(const char* c) {
    return kg_cast(isize)strnlen(c, ISIZE_MAX);
}
kg_inline isize kg_cstr_len_n(const char* c, isize n) {
    return kg_cast(isize)strnlen(c, n);
}

kg_string_t kg_string_create(kg_allocator_t* a, isize cap) {
    kg_string_t out_string = null;
    isize mem_size = kg_sizeof(kg_string_header_t) + cap + 1;
    kg_string_header_t* h = kg_cast(kg_string_header_t*)a->proc.alloc(a, mem_size);
    if (h) {
        kg_mem_zero(h, mem_size);
        h->allocator = a;
        h->cap = cap;
        out_string = kg_cast(kg_string_t)(h + 1);
    }
    return out_string;
}
kg_string_t kg_string_from_unsafe(kg_allocator_t* a, const void* v, isize v_len) {
    kg_string_t out_string = kg_string_create(a, v_len);
    if (out_string) {
        kg_mem_copy(out_string, v, v_len);
        kg_string_header_t* h = kg_string_header(out_string);
        h->len = v_len;
        out_string[v_len] = '\0';
    }
    return out_string;
}
kg_string_t kg_string_from_fmt(kg_allocator_t* a, const char* fmt, ...) {
    kg_string_t out_string = null;
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        out_string = kg_string_from_fmt_v(a, fmt, args);
        va_end(args);
    }
    return out_string;
}
kg_string_t kg_string_from_fmt_v(kg_allocator_t* a, const char* fmt, va_list args) {
    kg_string_t out_string = null;
    if (fmt) {
        va_list args_copy;
        va_copy(args_copy, args);
        isize length_check = vsnprintf(0, 0, fmt, args_copy);
        va_end(args_copy);
        if (length_check >= 0) {
            isize length = kg_cast(isize)length_check;
            out_string = kg_string_create(a, length);
            if (out_string) {
                kg_string_header_t* h = kg_string_header(out_string);
                vsnprintf(out_string + h->len, length + 1, fmt, args);
                h->len = length;
                out_string[h->len] = '\0';
            }
        }
    }
    return out_string;
}
kg_inline kg_string_t kg_string_from_str(kg_allocator_t* a, const kg_str_t s) {
    return kg_string_from_cstr_n(a, s.ptr, s.len);
}
kg_inline kg_string_t kg_string_from_str_n(kg_allocator_t* a, const kg_str_t s, isize n) {
    return kg_string_from_cstr_n(a, s.ptr, n > s.len ? s.len : n);
}
kg_inline kg_string_t kg_string_from_cstr(kg_allocator_t* a, const char* cstr) {
    return kg_string_from_cstr_n(a, cstr, kg_cstr_len(cstr));
}
kg_string_t kg_string_from_cstr_n(kg_allocator_t* a, const char* cstr, isize cstr_len) {
    kg_string_t out_string = null;
    isize cap = cstr_len;
    isize mem_size = kg_sizeof(kg_string_header_t) + cap + 1;
    kg_string_header_t* h = kg_cast(kg_string_header_t*)a->proc.alloc(a, mem_size);
    if (h) {
        kg_mem_zero(h, mem_size);
        h->allocator = a;
        h->cap = cap;
        h->len = cstr_len;
        out_string = kg_cast(kg_string_t)(h + 1);
        kg_mem_copy(out_string, cstr, cstr_len);
        out_string[h->len] = '\0';
    }
    return out_string;
}
kg_string_t kg_string_set(kg_string_t s, const char* cstr) {
    kg_string_t out = s;
    isize cstr_len = kg_cstr_len(cstr);
    if (cstr_len >= 0) {
        kg_string_header_t* h = kg_string_header(s);
        out = kg_string_ensure_available(s, cstr_len);
        if (out) {
            kg_mem_copy(out, cstr, cstr_len);
            h->len = cstr_len;
            out[h->len] = '\0';
        }
    }
    return out;
}
kg_string_t kg_string_append(kg_string_t s, kg_string_t other) {
    kg_string_t out_string = s;
    kg_string_header_t* s_h = kg_string_header(s);
    kg_string_header_t* other_h = kg_string_header(other);
    out_string = kg_string_ensure_available(s, other_h->len);
    if (out_string) {
        s_h->len += other_h->len;
        kg_mem_copy(out_string, other, other_h->len);
        out_string[s_h->len] = '\0';
    }
    return out_string;
}
kg_string_t kg_string_append_unsafe(kg_string_t s, const void* v, isize v_len) {
    kg_string_t out_string = null;
    kg_string_header_t* s_h = kg_string_header(s);
    out_string = kg_string_ensure_available(s, v_len);
    if (out_string) {
        s_h->len += v_len;
        kg_mem_copy(out_string, v, v_len);
        out_string[s_h->len] = '\0';
    }
    return out_string;
}
kg_string_t kg_string_append_fmt(kg_string_t s, const char* fmt, ...) {
    kg_string_t out_string = s;
    if (fmt) {
        va_list args;
        va_start(args, fmt);
        out_string = kg_string_append_fmt_v(out_string, fmt, args);
        va_end(args);
    }
    return out_string;
}
kg_string_t kg_string_append_fmt_v(kg_string_t s, const char* fmt, va_list args) {
    kg_string_t out_string = s;
    if (fmt) {
        va_list args_copy;
        va_copy(args_copy, args);
        isize length_check = vsnprintf(0, 0, fmt, args_copy);
        va_end(args_copy);
        if (length_check >= 0) {
            isize length = kg_cast(isize)length_check;
            isize new_length = kg_string_len(out_string) + length;
            out_string = kg_string_ensure_available(out_string, new_length);
            if (out_string) {
                kg_string_header_t* h = kg_string_header(out_string);
                vsnprintf(out_string + h->len, length + 1, fmt, args);
                h->len = new_length;
                out_string[h->len] = '\0';
            }
        }
    }
    return out_string;
}
kg_inline kg_string_t kg_string_append_char(kg_string_t s, char c) {
    return kg_string_append_unsafe(s, &c, 1);
}
kg_inline kg_string_t kg_string_append_rune(kg_string_t s, rune r) {
    kg_string_t out = s;
    u8 buf[8] = {0};
    isize len = kg_utf8_encode_rune(buf, r);
    out = kg_string_append_unsafe(s, buf, len);
    return out;
}
kg_string_t kg_string_append_cstr(kg_string_t s, const char* cstr) {
    return kg_string_append_cstr_n(s, cstr, kg_cstr_len(cstr));
}
kg_string_t kg_string_append_cstr_n(kg_string_t s, const char* cstr, isize cstr_len) {
    kg_string_t out_string = null;
    if (cstr && cstr_len > 0) {
        out_string = kg_string_ensure_available(s, cstr_len);
        if (out_string) {
            kg_string_header_t* out_string_h = kg_string_header(out_string);
            kg_mem_copy(out_string + out_string_h->len, cstr, cstr_len);
            out_string_h->len += cstr_len;
            out_string[out_string_h->len] = '\0';
        }
    }
    return out_string;
}
kg_string_t kg_string_grow(kg_string_t s, isize n) {
    kg_string_t out_string = null;
    kg_string_header_t* h = kg_string_header(s);
    isize old_mem_size = kg_string_mem_size(s);
    isize new_mem_size = old_mem_size + n + 1;
    kg_string_header_t* new_h = kg_cast(kg_string_header_t*)h->allocator->proc.resize(h->allocator, h, old_mem_size, new_mem_size);
    if (new_h) {
        new_h->cap += n;
        out_string = kg_cast(kg_string_t)(new_h + 1);
    }
    return out_string;
}
kg_inline isize kg_string_len(const kg_string_t s) {
    isize out_len = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_len = h->len;
    }
    return out_len;
}
kg_inline isize kg_string_cap(const kg_string_t s) {
    isize out_cap = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_cap = h->cap;
    }
    return out_cap;
}
kg_inline isize kg_string_available(const kg_string_t s) {
    isize out_available = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        if (h->cap >= h->len) {
            out_available = h->cap - h->len;
        }
    }
    return out_available;
}
kg_inline kg_string_t kg_string_ensure_available(kg_string_t s, isize n) {
    kg_string_t out_string = null;
    if (s) {
        isize available = kg_string_available(s);
        if (available < n) {
            out_string = kg_string_grow(s, n);
        } else {
            out_string = s;
        }
    }
    return out_string;
}
kg_inline isize kg_string_mem_size(const kg_string_t s) {
    isize out_mem_size = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_mem_size = kg_sizeof(kg_string_header_t) + h->cap + 1;
    }
    return out_mem_size;
}
kg_inline b32 kg_string_is_valid(const kg_string_t s) {
    b32 out_ok = false;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_ok = kg_cstr_len_n(s, h->len + 1) == h->len;
    }
    return out_ok;
}
kg_inline b32 kg_string_is_equal(const kg_string_t s, const kg_string_t other) {
    return kg_string_compare(s, other) == 0;
}
kg_inline b32 kg_string_is_equal_cstr(const kg_string_t s, const char* cstr) {
    return kg_cstr_compare(s, cstr) == 0;
}
kg_inline b32 kg_string_is_empty(const kg_string_t s) {
    return kg_string_len(s) == 0;
}
kg_inline i32 kg_string_compare(const void* s, const void* other) {
    isize s_len = kg_string_len(kg_cast(kg_string_t)s);
    isize other_len = kg_string_len(kg_cast(kg_string_t)other);
    i32 out = s_len - other_len;
    if (out == 0) {
        out = kg_cstr_compare_n(s, other, s_len);
    }
    return out;
}
kg_inline i32 kg_string_compare_n(const void* s, const void* other, isize n) {
    i32 out;
    isize s_len = kg_string_len(kg_cast(kg_string_t)s);
    isize other_len = kg_string_len(kg_cast(kg_string_t)other);
    if (s_len >= n && other_len >= n) {
        out = kg_cstr_compare_n(s, other, n);
    } else {
        out = s_len - other_len;
    }
    return out;
}
kg_inline i32 kg_string_compare_ci(const void* s, const void* other) {
    isize s_len = kg_string_len(kg_cast(kg_string_t)s);
    isize other_len = kg_string_len(kg_cast(kg_string_t)other);
    i32 out = s_len - other_len;
    if (out == 0) {
        out = kg_cstr_compare_ci_n(s, other, s_len);
    }
    return out;
}
kg_inline i32 kg_string_compare_ci_n(const void* s, const void* other, isize n) {
    i32 out;
    isize s_len = kg_string_len(kg_cast(kg_string_t)s);
    isize other_len = kg_string_len(kg_cast(kg_string_t)other);
    if (s_len >= n && other_len >= n) {
        out = kg_cstr_compare_ci_n(s, other, n);
    } else {
        out = s_len - other_len;
    }
    return out;
}
kg_inline u32 kg_string_utf8_len(const kg_string_t s) {
    return kg_str_utf8_len(kg_str_from_string(s));
}
kg_inline u32 kg_string_utf8_len_n(const kg_string_t s, isize n) {
    return kg_str_utf8_len_n(kg_str_from_string(s), n);
}
kg_inline void kg_string_reset(kg_string_t s) {
    kg_string_header_t* h = kg_string_header(s);
    h->len = 0;
    s[0] = '\0';
}
void kg_string_destroy(kg_string_t s) {
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        isize mem_size = kg_string_mem_size(s);
        h->allocator->proc.free(h->allocator, h, mem_size);
    }
}

kg_inline kg_inline kg_str_t kg_str_create(const char* cstr) {
    kg_str_t out_str = (kg_str_t){0};
    if (cstr) {
        out_str.len = kg_cstr_len(cstr);
        out_str.ptr = cstr;
    }
    return out_str;
}
kg_inline kg_str_t kg_str_create_n(const char* cstr, isize len) {
    kg_str_t out_str = (kg_str_t){0};
    if (cstr && len > 0) {
        out_str.len = len;
        out_str.ptr = cstr;
    }
    return out_str;
}
kg_inline kg_str_t kg_str_create_empty(void) {return (kg_str_t){.len=0,.ptr=""};}
kg_inline kg_str_t kg_str_create_null(void) {return (kg_str_t){.len=0,.ptr=null};}
kg_inline kg_str_t kg_str_from_string(const kg_string_t s) {
    return (kg_str_t){.len = kg_string_len(s), .ptr = s};
}
kg_inline kg_str_t kg_str_from_string_n(const kg_string_t s, isize len) {
    isize string_len = kg_string_len(s);
    return (kg_str_t){.len = len > string_len ? string_len : len, .ptr = s};
}
kg_str_t kg_str_chop_first_split_by(kg_str_t* s, const kg_str_t split_by) {
    kg_str_t out_str = kg_str_create_empty();
    isize index = kg_str_index(*s, split_by);
    if (index >= 0) {
        out_str = kg_str_substr(*s, 0, index);
        *s = (kg_str_t){.ptr = s->ptr + index + split_by.len, .len = s->len - index - 1};
    } else {
        out_str = *s;
        *s = kg_str_create_empty();
    }
    return out_str;
}
kg_str_t kg_str_chop_first_line(kg_str_t* s) {
    kg_str_t out_str = kg_str_create_empty();
    isize index = kg_str_index_char(*s, '\n');
    if (index >= 0) {
        out_str = kg_str_substr(*s, 0, index);
        *s = (kg_str_t){.ptr = s->ptr + index + 1, .len = s->len - index - 1};
    } else {
        out_str = *s;
        *s = kg_str_create_empty();
    }
    return out_str;
}
kg_inline kg_str_t kg_str_substr(const kg_str_t s, isize start_inc, isize end_exc) {
    kg_str_t out;
    if (end_exc > 0 && start_inc <= end_exc) {
        if (end_exc > s.len) {
            end_exc = s.len;
        }
        if (start_inc < 0) {
            start_inc = 0;
        }
        out = (kg_str_t){.len = end_exc - start_inc, .ptr = s.ptr + start_inc};
    } else {
        out = kg_str_create_empty();
    }
    return out;
}
kg_inline kg_str_t kg_str_substr_from(const kg_str_t s, isize start_inc) {
    return kg_str_substr(s, start_inc, s.len);
}
kg_inline kg_str_t kg_str_substr_to(const kg_str_t s, isize end_exc) {
    return kg_str_substr(s, 0, end_exc);
}
kg_inline kg_str_t kg_str_trim_space(const kg_str_t s) {
    return kg_str_trim_space_left(kg_str_trim_space_right(s));
}
kg_inline kg_str_t kg_str_trim_space_left(const kg_str_t s) {
    kg_str_t out = s;
    if (out.ptr && out.len > 0) {
        isize n = 0;
        while(n < s.len && kg_char_is_space(out.ptr[n])) {
            n++;
        }
        out.ptr += n;
        out.len -= n;
    }
    return out;
}
kg_inline kg_str_t kg_str_trim_space_right(const kg_str_t s) {
    kg_str_t out = s;
    if (out.ptr && out.len > 0) {
        isize n = 0;
        while(n < s.len && kg_char_is_space(out.ptr[s.len - n - 1])) {
            n++;
        }
        out.len -= n;
    }
    return out;
}
kg_str_t kg_str_trim_prefix(const kg_str_t s, const kg_str_t prefix) {
    kg_str_t out;
    if (kg_str_has_prefix(s, prefix)) {
        out = (kg_str_t){
            .len = s.len - prefix.len,
            .ptr = s.ptr + prefix.len,
        };
    } else {
        out = kg_str_create_empty();
    }
    return out;
}
kg_str_t kg_str_trim_suffix(const kg_str_t s, const kg_str_t suffix) {
    kg_str_t out;
    if (kg_str_has_suffix(s, suffix)) {
        out = (kg_str_t){
            .len = s.len - suffix.len,
            .ptr = s.ptr,
        };
    } else {
        out = kg_str_create_empty();
    }
    return out;
}
kg_inline b32 kg_str_is_empty(const kg_str_t s) {
    return s.ptr && s.len == 0;
}
kg_inline b32 kg_str_is_null(const kg_str_t s) {
    return s.ptr == null;
}
kg_inline b32 kg_str_is_null_or_empty(const kg_str_t s) {
    return s.ptr == null || s.len == 0;
}
kg_inline b32 kg_str_is_valid_cstr(const kg_str_t s) {
    b32 out_ok = false;
    if (s.ptr) {
        out_ok = kg_cstr_len_n(s.ptr, s.len) == s.len;
    }
    return out_ok;
}
kg_inline b32 kg_str_is_equal(const kg_str_t s, const kg_str_t other) {
    return kg_str_compare(&s, &other) == 0;
}
kg_inline b32 kg_str_contains(const kg_str_t s, const kg_str_t needle) {
    return kg_str_index(s, needle) >= 0;
}
kg_inline b32 kg_str_has_prefix(const kg_str_t s, const kg_str_t prefix) {
    b32 out;
    if (s.len >= prefix.len) {
        out = kg_cstr_compare_n(s.ptr, prefix.ptr, prefix.len) == 0;
    } else {
        out = false;
    }
    return out;
}
kg_inline b32 kg_str_has_suffix(const kg_str_t s, const kg_str_t suffix) {
    b32 out;
    if (s.len >= suffix.len) {
        out = kg_cstr_compare_n(s.ptr + s.len - suffix.len, suffix.ptr, suffix.len) == 0;
    } else {
        out = false;
    }
    return out;
}
kg_inline u32 kg_str_utf8_len(const kg_str_t s) {
    return kg_str_utf8_len_n(s, U32_MAX);
}
u32 kg_str_utf8_len_n(const kg_str_t s, isize n) {
    u32 out_len = 0;
    for (isize i = 0; out_len < n; i++, out_len++) {
        rune r = kg_cast(rune)s.ptr[i];
        if (r == 0) {
            break;
        }
        if (r >= 0 && r < (1<<7)-1) {
        } else if ((r & 0xe0) == 0xc0) {
            i++;
        } else if ((r & 0xf0) == 0xe0) {
            i+=2;
        } else if ((r & 0xf8) == 0xf0) {
            i+=3;
        } else {
            out_len = 0;
            break;
        }
    }
    return out_len;
}
isize kg_str_index(const kg_str_t s, const kg_str_t needle) {
    isize out_index = -1;
    if (s.len >= needle.len) {
        for (isize i = 0; i < s.len - needle.len + 1; i++) {
            if (kg_cstr_compare_n(s.ptr + i, needle.ptr, needle.len) == 0) {
                out_index = i;
                break;
            }
        }
    }
    return out_index;
}
isize kg_str_index_char(const kg_str_t s, char needle) {
    isize out_index = -1;
    for (isize i = 0; i < s.len; i++) {
        if (s.ptr[i] == needle) {
            out_index = i;
            break;
        }
    }
    return out_index;
}
kg_inline i32 kg_str_compare(const void* s, const void* other) {
    kg_str_t* str_s = kg_cast(kg_str_t*)s;
    kg_str_t* str_other = kg_cast(kg_str_t*)other;
    i32 out = str_s->len - str_other->len;
    if (out == 0) {
        out = kg_cstr_compare_n(str_s->ptr, str_other->ptr, str_s->len);
    }
    return out;
}
kg_inline i32 kg_str_compare_n(const void* s, const void* other, isize n) {
    kg_str_t* str_s = kg_cast(kg_str_t*)s;
    kg_str_t* str_other = kg_cast(kg_str_t*)other;
    i32 out;
    if (str_s->len >= n && str_other->len >= n) {
        out = kg_cstr_compare_n(str_s->ptr, str_other->ptr, n);
    } else {
        out = str_s->len - str_other->len;
    }
    return out;
}
kg_inline i32 kg_str_compare_ci(const void* s, const void* other) {
    kg_str_t* str_s = kg_cast(kg_str_t*)s;
    kg_str_t* str_other = kg_cast(kg_str_t*)other;
    i32 out = str_s->len - str_other->len;
    if (out == 0) {
        out = kg_cstr_compare_ci_n(str_s->ptr, str_other->ptr, str_s->len);
    }
    return out;
}
kg_inline i32 kg_str_compare_ci_n(const void* s, const void* other, isize n) {
    kg_str_t* str_s = kg_cast(kg_str_t*)s;
    kg_str_t* str_other = kg_cast(kg_str_t*)other;
    i32 out;
    if (str_s->len >= n && str_other->len >= n) {
        out = kg_cstr_compare_ci_n(str_s->ptr, str_other->ptr, n);
    } else {
        out = str_s->len - str_other->len;
    }
    return out;
}

#define KG_VALID_BOOL_STRS_MAP_LEN 5
kg_static struct {kg_str_t str; b32 bool;} KG_VALID_BOOL_STRS_MAP[KG_VALID_BOOL_STRS_MAP_LEN] = {
    {{.len = 4, .ptr = "true"},  true},
    {{.len = 2, .ptr = "ok"},    true},
    {{.len = 3, .ptr = "yes"},   true},
    {{.len = 5, .ptr = "false"}, false},
    {{.len = 2, .ptr = "no"},    false},
};
b32 kg_str_to_b32(b32* b, const kg_str_t s) {
    b32 out_ok = false;
    for (isize i = 0; i < KG_VALID_BOOL_STRS_MAP_LEN; i++) {
        if (kg_str_is_equal(s, KG_VALID_BOOL_STRS_MAP[i].str)) {
            *b = KG_VALID_BOOL_STRS_MAP[i].bool;
            out_ok = true;
        }
    }
    return out_ok;
}
b32 kg_str_to_u64(u64* u, const kg_str_t s) {
    b32 out_ok = true;
    u64 value = 0;
    for (isize i = 0; i < s.len; i++) {
        char c = s.ptr[i];
        if (c < '0' || c > '9') {
            out_ok = false;
            break;
        }
        value = value * 10 + (c - '0');
    }
    *u = value;
    return out_ok;
}
b32 kg_str_to_i64(i64* i, const kg_str_t s) {
    b32 out_ok = true;
    i64 value = 0;
    char c1 = s.ptr[0];
    if (c1 == '-') {
        out_ok = kg_str_to_u64(kg_cast(u64*)(&value), (kg_str_t){.len = s.len - 1, .ptr = s.ptr + 1});
        value = -value;
    } else {
        out_ok = kg_str_to_u64(kg_cast(u64*)(&value), s);
    }
    *i = value;
    return out_ok;
}
kg_string_t kg_b32_to_string(kg_allocator_t* a, b32 b) {
    return kg_string_from_cstr(a, b ? "true" : "false");
}
isize kg_b32_to_cstr(char buf[KG_B32_MAX_CHARS_LEN], b32 b) {
    isize len = sprintf(buf, "%s", b ? "true" : "false");
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
isize kg_u64_to_cstr(char buf[KG_U64_MAX_CHARS_LEN], u64 u) {
    isize len = sprintf(buf, "%lu", u);
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
kg_string_t kg_u64_to_string(kg_allocator_t* a, u64 u) {
    kg_string_t out = null;
    char buf[KG_U64_MAX_CHARS_LEN] = {0};
    if (kg_u64_to_cstr(buf, u) > 0) {
        out = kg_string_from_cstr(a, buf);
    }
    return out;
}
isize kg_i64_to_cstr(char buf[KG_I64_MAX_CHARS_LEN], i64 i) {
    isize len = sprintf(buf, "%li", i);
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
kg_string_t kg_i64_to_string(kg_allocator_t* a, i64 i) {
    kg_string_t out = null;
    char buf[KG_I64_MAX_CHARS_LEN] = {0};
    if (kg_i64_to_cstr(buf, i) > 0) {
        out = kg_string_from_cstr(a, buf);
    }
    return out;
}
isize kg_f64_to_cstr(char buf[KG_F64_MAX_CHARS_LEN], f64 f) {
    isize len = sprintf(buf, "%lf", f);
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
kg_string_t kg_f64_to_string(kg_allocator_t* a, f64 f) {
    kg_string_t out = null;
    char buf[KG_F64_MAX_CHARS_LEN] = {0};
    if (kg_i64_to_cstr(buf, f) > 0) {
        out = kg_string_from_cstr(a, buf);
    }
    return out;
}
kg_inline char kg_char_to_lower(char c) {
    if (c >= 'A' && c <= 'Z') {
        return 'a' + (c - 'A');
    }
    return c;
}
kg_inline char kg_char_to_upper(char c) {
    if (c >= 'a' && c <= 'z') {
        return 'A' + (c - 'a');
    }
    return c;
}
kg_inline b32 kg_char_is_space(char c) {
    switch(c) {
        case ' ':
        case '\t':
        case '\n':
        case '\r':
        case '\f':
        case '\v':
            return true;
    }
    return false;
}
kg_inline b32 kg_char_is_digit(char c) {
    return c >= '0' && c <= '9';
}
kg_inline b32 kg_char_is_alpha(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}
kg_inline b32 kg_char_is_alphanumeric(char c) {
    return kg_char_is_digit(c) || kg_char_is_alpha(c);
}

kg_static const rune KG_RUNE_SURROGATE_MIN = 0xd800;
kg_static const rune KG_RUNE_SURROGATE_MAX = 0xdfff;
kg_static const rune KG_RUNE_1_MAX         = (1 << 7 ) - 1;
kg_static const rune KG_RUNE_2_MAX         = (1 << 11) - 1;
kg_static const rune KG_RUNE_3_MAX         = (1 << 16) - 1;
kg_static kg_inline b32 kg_utf8_is_cont(u8 b) {
    return kg_is_within(b, 0x80, 0xbf);
}
isize kg_utf8_decode_rune(rune* r, u8* b, isize b_len) {
    kg_printf("%s - understand and replace it with this -> https://go.dev/src/unicode/utf8/utf8.go\n", __func__);
    const u8 maskx = 0x3f;
    isize bytes = 0;
    rune c = KG_RUNE_INVALID;
    if (b_len <= 0 || b == null) {
        c = KG_RUNE_INVALID;
        bytes = 0;
    } else if ((b[0] >> 7) == 0) {
        if (b_len >= 1) {
            c = kg_cast(rune)b[0];
            bytes = 1;
        }
    } else if ((b[0] & 0xe0) == 0xc0) {
        if (b_len >= 2 && kg_utf8_is_cont(b[1])) {
            c = ((kg_cast(rune)b[0] & 0x1f) << 6)
                | (kg_cast(rune)b[1] & maskx);
            if (c < 0x80) {
                c = KG_RUNE_INVALID;
                bytes = 0;
            } else {
                bytes = 2;
            }
        }
    } else if ((b[0] & 0xf0) == 0xe0) {
        if (b_len >= 3 && kg_utf8_is_cont(b[1]) && kg_utf8_is_cont(b[2])) {
            c = ((kg_cast(rune)b[0] & 0x0f) << 12)
                | ((kg_cast(rune)b[1] & maskx) << 6)
                | (kg_cast(rune)b[2] & maskx);
            if (c < 0x800) {
                c = KG_RUNE_INVALID;
                bytes = 0;
            } else if (c >= 0xd800 && c <= 0xdfff) {
                c = KG_RUNE_INVALID;
                bytes = 0;
            } else {
                bytes = 3;
            }
        }
    } else if ((b[0] & 0xf8) == 0xf0) {
        if (b_len >= 4 && kg_utf8_is_cont(b[1]) && kg_utf8_is_cont(b[2]) && kg_utf8_is_cont(b[3])) {
            c = ((kg_cast(rune)b[0] & 0x07) << 18)
                | ((kg_cast(rune)b[1] & maskx) << 12)
                | ((kg_cast(rune)b[2] & maskx) << 6)
                | (kg_cast(rune)b[3] & maskx);
            if (c < 0x10000 || c > 0x10ffff) {
                c = KG_RUNE_INVALID;
                bytes = 0;
            } else {
                bytes = 4;
            }
        }
    } else {
        c = KG_RUNE_INVALID;
    }
    if (!kg_rune_is_valid(c)) {
        bytes = 0;
        c = KG_RUNE_INVALID;
    }
    *r = c;
    return bytes;
}
isize kg_utf8_encode_rune(u8 b[4], rune r) {
    kg_printf("%s - understand and replace it with this -> https://go.dev/src/unicode/utf8/utf8.go\n", __func__);
    u32 i = kg_cast(u32)r;
    kg_static const u8 maskx = 0x3f;
    isize bytes;
    if (i <= 0x7f) {
        b[0] = kg_cast(u8)r;
        bytes = 1;
    } else if (i <= KG_RUNE_2_MAX) {
        b[0] = 0xc0 | (kg_cast(u8)(r >> 6));
        b[1] = 0x80 | (kg_cast(u8)(r)       & maskx);
        bytes = 2;
    } else if (!kg_rune_is_valid(i)) {
        r = KG_RUNE_INVALID;
        b[0] = 0xe0 | (kg_cast(u8)(r >> 12));
        b[1] = 0x80 | (kg_cast(u8)(r >> 6)  & maskx);
        b[2] = 0x80 | (kg_cast(u8)(r)       & maskx);
        bytes = 3;
    } else if (i <= KG_RUNE_3_MAX) {
        b[0] = 0xe0 | (kg_cast(u8)(r >> 12));
        b[1] = 0x80 | (kg_cast(u8)(r >> 6)  & maskx);
        b[2] = 0x80 | (kg_cast(u8)(r)       & maskx);
        bytes = 3;
    } else if (i <= KG_RUNE_MAX) {
        b[0] = 0xf0 | (kg_cast(u8)(r >> 18));
        b[1] = 0x80 | (kg_cast(u8)(r >> 12) & maskx);
        b[2] = 0x80 | (kg_cast(u8)(r >> 6)  & maskx);
        b[3] = 0x80 | (kg_cast(u8)(r)       & maskx);
        bytes = 4;
    } else {
        bytes = 0;
    }
    return bytes;
}
kg_inline b32 kg_rune_is_valid(rune r) {
    if (0 <= r && r < KG_RUNE_SURROGATE_MIN) return true;
    else if (KG_RUNE_SURROGATE_MAX < r && r <= KG_RUNE_MAX) return true;
    return false;
}
kg_inline b32 kg_rune_is_digit(rune r) {
    return kg_is_within(r, 0x0030, 0x0039);
}
kg_inline b32 kg_rune_is_space(rune r) {
    switch(r) {
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x20:
            return true;
        case 0xa0:  
        case 0x1680:
        case 0x2000:
        case 0x2001:
        case 0x2002:
        case 0x2003:
        case 0x2004:
        case 0x2005:
        case 0x2006:
        case 0x2007:
        case 0x2008:
        case 0x2009:
        case 0x200a:
        case 0x2028:
        case 0x2029:
        case 0x202f:
        case 0x205f:
        case 0x3000:
            return true;
        default:
            return false;
    }
}
isize kg_rune_len(rune r) {
    isize out;
    if (r < 0) out = -1;
    else if (r <= KG_RUNE_1_MAX) out = 1;
    else if (r <= KG_RUNE_2_MAX) out = 2;
    else if (KG_RUNE_SURROGATE_MIN <= r && r <= KG_RUNE_SURROGATE_MAX) out = -1;
    else if (r <= KG_RUNE_3_MAX) out = 3;
    else if (r <= KG_RUNE_MAX) out = 4;
    else out = -1;
    return out;
}


b32 kg_string_builder_create(kg_string_builder_t* b, kg_allocator_t* a, isize cap) {
    b32 out_ok = false;
    *b = (kg_string_builder_t){
        .allocator = a,
        .cap       = cap,
        .real_ptr  = kg_allocator_alloc(a, cap),
    };
    if (b->real_ptr) {
        b->write_ptr = b->real_ptr;
        out_ok = true;
    }
    return out_ok;
}
kg_inline b32 kg_string_builder_write_unsafe(kg_string_builder_t* b, const void* v, isize n) {
    b32 out_ok = false;
    if (kg_string_builder_ensure_available(b, n)) {
        kg_mem_copy(b->write_ptr, v, n);
        b->len += n;
        b->write_ptr += n;
        out_ok = true;
    }
    return out_ok;
}
kg_inline b32 kg_string_builder_write_u64(kg_string_builder_t* b, u64 u) {
    b32 out_ok = false;
    char buf[KG_U64_MAX_CHARS_LEN] = {0};
    isize len = kg_u64_to_cstr(buf, u);
    out_ok = kg_string_builder_write_unsafe(b, buf, len);
    return out_ok;
}
kg_inline b32 kg_string_builder_write_i64(kg_string_builder_t* b, i64 i) {
    b32 out_ok = false;
    char buf[KG_I64_MAX_CHARS_LEN] = {0};
    isize len = kg_i64_to_cstr(buf, i);
    out_ok = kg_string_builder_write_unsafe(b, buf, len);
    return out_ok;
}
kg_inline b32 kg_string_builder_write_f64(kg_string_builder_t* b, f64 f) {
    b32 out_ok = false;
    char buf[KG_F64_MAX_CHARS_LEN] = {0};
    isize len = kg_f64_to_cstr(buf, f);
    out_ok = kg_string_builder_write_unsafe(b, buf, len);
    return out_ok;
}
kg_inline b32 kg_string_builder_write_char(kg_string_builder_t* b, char c) {
    b32 out_ok = false;
    if (kg_string_builder_ensure_available(b, 1)) {
        *b->write_ptr = c;
        b->write_ptr++;
        b->len++;
        out_ok = true;
    }
    return out_ok;
}
kg_inline b32 kg_string_builder_write_rune(kg_string_builder_t* b, rune r) {
    b32 out_ok = false;
    u8 buf[8] = {0};
    isize len = kg_utf8_encode_rune(buf, r);
    out_ok = kg_string_builder_write_unsafe(b, buf, len);
    return out_ok;
}
b32 kg_string_builder_write_fmt(kg_string_builder_t* b, const char* fmt, ...) {
    b32 out_ok = false;
    if (fmt && b) {
        va_list args;
        va_start(args, fmt);
        out_ok = kg_string_builder_write_fmt_v(b, fmt, args);
        va_end(args);
    }
    return out_ok;
}
b32 kg_string_builder_write_fmt_v(kg_string_builder_t* b, const char* fmt, va_list args) {
    b32 out_ok = false;
    if (fmt) {
        va_list args_copy;
        va_copy(args_copy, args);
        isize length_check = vsnprintf(0, 0, fmt, args_copy);
        va_end(args_copy);
        if (length_check >= 0) {
            isize length = kg_cast(isize)length_check;
            isize new_length = kg_string_builder_len(b) + length;
            if (kg_string_builder_ensure_available(b, new_length)) {
                vsnprintf(b->write_ptr, length + 1, fmt, args);
                b->len = new_length;
                b->write_ptr += length;
                out_ok = true;
            }
        }
    }
    return out_ok;
}
kg_inline b32 kg_string_builder_write_cstr(kg_string_builder_t* b, const char* c) {
    b32 out_ok = false;
    isize len = kg_cstr_len(c);
    out_ok = kg_string_builder_write_unsafe(b, c, len);
    return out_ok;
}
kg_inline b32 kg_string_builder_write_cstr_n(kg_string_builder_t* b, const char* c, isize n) {    
    return kg_string_builder_write_unsafe(b, c, n);
}
kg_inline b32 kg_string_builder_write_str(kg_string_builder_t* b, const kg_str_t s) {
    return kg_string_builder_write_unsafe(b, s.ptr, s.len);
}
kg_inline b32 kg_string_builder_write_str_n(kg_string_builder_t* b, const kg_str_t s, isize n) {
    return kg_string_builder_write_unsafe(b, s.ptr, n > s.len ? s.len : n);
}
kg_inline b32 kg_string_builder_write_string(kg_string_builder_t* b, const kg_string_t s) {
    isize len = kg_string_len(s);
    return kg_string_builder_write_unsafe(b, s, len);
}
kg_inline b32 kg_string_builder_write_string_n(kg_string_builder_t* b, const kg_string_t s, isize n) {
    isize len = kg_string_len(s);
    return kg_string_builder_write_unsafe(b, s, n > len ? len : n);
}
kg_inline b32 kg_string_builder_grow(kg_string_builder_t* b, isize n) {
    b32 out_ok = false;
    isize old_mem_size = kg_string_builder_mem_size(b);
    isize new_mem_size = old_mem_size + n;
    char* new_real_ptr = kg_allocator_resize(b->allocator, b->real_ptr, old_mem_size, new_mem_size);
    if (new_real_ptr) {
        b->real_ptr = new_real_ptr;
        b->write_ptr = b->real_ptr + b->len;
        b->cap += n;
        out_ok = true;
    }
    return out_ok;
}
kg_inline isize kg_string_builder_cap(const kg_string_builder_t* b) {
    return b ? b->cap : 0;
}
kg_inline isize kg_string_builder_len(const kg_string_builder_t* b) {
    return b ? b->len : 0;
}
kg_inline isize kg_string_builder_available(const kg_string_builder_t* b) {
    b32 out = 0;
    if (b && b->cap > b->len) {
        out = b->cap - b->len;
    }
    return out;
}
kg_inline b32 kg_string_builder_ensure_available(kg_string_builder_t* b, isize n) {
    b32 out_ok = true;
    if (kg_string_builder_available(b) < n) {
        out_ok = kg_string_builder_grow_formula(b, n);
    }
    return out_ok;
}

kg_inline void kg_string_builder_reset(kg_string_builder_t* b) {
    b->write_ptr = b->real_ptr;
    b->len = 0;
}
kg_string_t kg_string_builder_to_string(const kg_string_builder_t* b, kg_allocator_t* a) {
    return kg_string_from_cstr_n(a, b->real_ptr, b->len);
}
kg_inline b32 kg_string_builder_grow_formula(kg_string_builder_t* b, isize n) {
    return kg_string_builder_grow(b, kg_max(b->cap * 2, b->cap + n));
}
kg_inline isize kg_string_builder_mem_size(const kg_string_builder_t* b) {
    return b ? b->cap : 0;
}
void kg_string_builder_destroy(kg_string_builder_t* b) {
    if (b) {
        kg_allocator_free(b->allocator, b->real_ptr, kg_string_builder_mem_size(b));
        kg_mem_zero(b, kg_sizeof(kg_string_builder_t));
    }
}

void* kg_darray_create2_(kg_allocator_t* a, isize stride, isize cap, kg_darray_base_t* out_b) {
    void* out_ptr = kg_allocator_alloc(a, cap * stride);
    if (out_ptr) {
        out_b->allocator = a;
        out_b->stride    = stride;
        out_b->cap       = cap;
        out_b->len       = 0;
    }
    return out_ptr;
}
void* kg_darray_grow2_(kg_darray_base_t* b, isize n, void** out_ptr) {
    void* out_new_ptr = null;
    isize old_mem_size = b->cap * b->stride;
    isize new_mem_size = old_mem_size + n * b->stride;
    out_new_ptr = kg_allocator_resize(b->allocator, *out_ptr, old_mem_size, new_mem_size);
    if (out_new_ptr) {
        *out_ptr = out_new_ptr;
        b->cap += n;
    }
    return out_new_ptr;
}
void* kg_darray_grow_formula2_(kg_darray_base_t* b, isize n, void** out_ptr) {
    return kg_darray_grow2_(b, b->cap + n, out_ptr);
}
void* kg_darray_ensure_available2_(kg_darray_base_t* b, isize n, void** out_ptr) {
    if (b->len + n > b->cap) {
        if (!kg_darray_grow_formula2_(b, n, out_ptr)) {
            return null;
        }
    }
    return *out_ptr;
}

void* kg_darray_create_(kg_allocator_t* allocator, isize stride, isize cap) {
    void* out_darray = null;
    isize mem_size = kg_sizeof(kg_darray_header_t) + cap * stride;
    kg_darray_header_t* h = kg_cast(kg_darray_header_t*)kg_allocator_alloc(allocator, mem_size);
    if (h) {
        h->len = 0;
        h->cap = cap;
        h->stride = stride;
        h->allocator = allocator;
        out_darray = kg_cast(void*)(h + 1);
    }
    return out_darray;
}
void* kg_darray_grow_(void* d, isize n) {
    kg_darray_header_t* old_h = kg_darray_header(d);
    isize old_mem_size = kg_darray_mem_size(d);
    isize new_mem_size = old_mem_size + old_h->stride * n;
    kg_darray_header_t* new_h = kg_allocator_resize(old_h->allocator, old_h, old_mem_size, new_mem_size);
    if (new_h) {
        new_h->cap += n;
        return kg_cast(void*)(new_h + 1);
    }
    return null;
}

b32 kg_queue_create(kg_queue_t* q, kg_allocator_t* allocator, isize stride, isize cap) {
    b32 out_ok = false;
    *q = (kg_queue_t){
        .allocator = allocator, 
        .len       = 0,
        .cap       = cap,
        .stride    = stride,
        .real_ptr  = kg_allocator_alloc(allocator, stride * cap),
    };
    if (q->real_ptr) {
        out_ok = true;
    }
    return out_ok;
}
b32 kg_queue_peek(const kg_queue_t* q, void* o) {
    b32 out_ok = false;
    if (q->len > 0) {
        kg_mem_copy(o, q->real_ptr, q->stride);
        out_ok = true;
    }
    return out_ok;
}
b32 kg_queue_enqueue(kg_queue_t* q, const void* o) {
    b32 out_ok = false;
    if (o && kg_queue_ensure_available(q, 1)) {
        if (kg_mem_copy(kg_cast(u8*)q->real_ptr + (q->len * q->stride), o, q->stride) != null) {
            q->len++;
            out_ok = true;
        }
    }
    return out_ok;
}
b32 kg_queue_deque(kg_queue_t* q, void* o) {
    b32 out_ok = false;
    if (o && q->len > 0) {
        out_ok = kg_mem_copy(o, q->real_ptr, q->stride) != null;
        if (out_ok) {
            kg_mem_move(q->real_ptr, kg_cast(u8*)q->real_ptr + q->stride, (q->len - 1) * q->stride);
            q->len--;
        } 
    }
    return out_ok;
}
b32 kg_queue_grow(kg_queue_t* q, isize n) {
    b32 out_ok = false;
    if (n > 0) {
        isize old_mem_size = kg_queue_mem_size(q);
        isize new_mem_size = kg_queue_mem_size(q) + q->stride * n;
        void* new_real_ptr = kg_allocator_resize(q->allocator, q->real_ptr, old_mem_size, new_mem_size);
        if (new_real_ptr) {
            q->cap += n;
            q->real_ptr = new_real_ptr;
            out_ok = true;
        }
    }
    return out_ok;
}
kg_inline b32 kg_queue_ensure_available(kg_queue_t* q, isize n) {
    b32 out_ok = true;
    if (kg_queue_available(q) < n) {
        out_ok = kg_queue_grow(q, n);
    }
    return out_ok;
}
kg_inline b32 kg_queue_is_empty(const kg_queue_t* q) {
    return q ? q->len <= 0 : true;
}
kg_inline isize kg_queue_available(const kg_queue_t* q) {
    isize out = 0;
    if (q && q->cap > q->len) {
        out = q->cap - q->len;
    }
    return out;
}
kg_inline isize kg_queue_len(const kg_queue_t* q) {
    return q ? q->len : 0;
}
kg_inline isize kg_queue_cap(const kg_queue_t* q) {
    return q ? q->cap : 0;
}
kg_inline isize kg_queue_mem_size(const kg_queue_t* q) {
    isize out = 0;
    if (q) {
        out = q->cap * q->stride;
    }
    return out;
}
void kg_queue_destroy(kg_queue_t* q) {
    if (q) {
        kg_allocator_free(q->allocator, q->real_ptr, kg_queue_mem_size(q));
        kg_mem_zero(q, kg_sizeof(kg_queue_t));
    }
}

kg_inline void kg_exit(i32 code) {
    exit(code);
}

kg_inline void kg_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

b32 kg_file_open(kg_file_t* f, const char* filename, kg_file_mode_t mode, b32 binary) {
    b32 out_ok = true;
    const char* mode_str;
    if ((mode & KG_FILE_MODE_READ) != 0 && (mode & KG_FILE_MODE_WRITE) != 0) {
        mode_str = binary ? "w+b" : "w+";
    } else if ((mode & KG_FILE_MODE_READ) != 0 && (mode & KG_FILE_MODE_WRITE) == 0) {
        mode_str = binary ? "rb" : "r";
    } else if ((mode & KG_FILE_MODE_READ) == 0 && (mode & KG_FILE_MODE_WRITE) != 0) {
        mode_str = binary ? "wb" : "w";
    } else {
        out_ok = false;
    }
    if (out_ok) {
        FILE* file = fopen(filename, mode_str);
        if (!file) {
            out_ok = false;
        } else {
            f->handle = file;
            f->is_valid = true;
        }
    }
    return out_ok;
}
isize kg_file_size(kg_file_t* f) {
    isize out_size = -1;
    if (f->handle) {
        fseek(kg_cast(FILE*)f->handle, 0, SEEK_END);
        out_size = ftell(kg_cast(FILE*)f->handle);
        rewind(kg_cast(FILE*)f->handle);
    }
    return out_size;
}
kg_file_content_t kg_file_content_read(kg_allocator_t* a, const char* filename) {
    kg_file_content_t out_content = {
        .cstr      = null,
        .len       = 0,
        .allocator = a,
        .is_valid  = false,
    };
    kg_file_t f = {0};
    if (kg_file_open(&f, filename, KG_FILE_MODE_READ, false)) {
        isize size = kg_file_size(&f);
        if (size >= 0) {
            out_content.cstr = kg_cast(char*)kg_allocator_alloc(a, size + 1);
            if (out_content.cstr) {
                kg_mem_zero(out_content.cstr, size + 1);
                isize bytes_read = fread(out_content.cstr, 1, size, kg_cast(FILE*)f.handle);
                out_content.len = bytes_read;
                out_content.cstr[out_content.len] = '\0';
                out_content.is_valid = true;
            }
        }
        kg_file_close(&f);
    }
    return out_content;
}
void kg_file_content_destroy(kg_file_content_t* fc) {
    if (fc) {
        kg_allocator_free(fc->allocator, fc->cstr, fc->len);
        kg_mem_zero(fc, kg_sizeof(kg_file_content_t));
    }
}
b32 kg_file_close(kg_file_t* f) {
    b32 out_ok = false;
    if (f->handle) {
        fclose(kg_cast(FILE*)f->handle);
        f->handle = 0;
        f->is_valid = false;
        out_ok = true;
    }
    return out_ok;
}

kg_static void kg_time_ltimespec_normalize(struct timespec* ts) {
    const long REPLACE_THIS_NSEC_PER_SEC = 1e9L;
    if (ts->tv_nsec >= REPLACE_THIS_NSEC_PER_SEC) {
        ts->tv_sec += ts->tv_nsec / REPLACE_THIS_NSEC_PER_SEC;
        ts->tv_nsec = ts->tv_nsec % REPLACE_THIS_NSEC_PER_SEC;
    } else if (ts->tv_nsec < 0) {
        long sec_adjust = (-ts->tv_nsec + REPLACE_THIS_NSEC_PER_SEC - 1) / REPLACE_THIS_NSEC_PER_SEC;
        ts->tv_sec -= sec_adjust;
        ts->tv_nsec += sec_adjust * REPLACE_THIS_NSEC_PER_SEC;
    }
}
void kg_time_sleep(const kg_duration_t d) {
    struct timespec ts = {
        .tv_sec  = d.sec,
        .tv_nsec = d.nsec,
    };
    kg_assert_fmt(nanosleep(&ts, null) == 0, "not my fault");
}
kg_inline kg_time_t kg_time_now(void) {
    kg_time_t out = (kg_time_t){0};
    kg_assert_fmt(clock_gettime(CLOCK_REALTIME, &out.wall) == 0, "not my fault");
    kg_assert_fmt(clock_gettime(CLOCK_MONOTONIC, &out.monotonic) == 0, "not my fault");
    return out;
}
kg_inline kg_time_t kg_time_add(const kg_time_t t, const kg_duration_t d) {
    kg_time_t out = (kg_time_t){
        .wall = {
            .tv_sec  = t.wall.tv_sec + d.sec,
            .tv_nsec = t.wall.tv_nsec + d.nsec,
        },
        .monotonic = {
            .tv_sec  = t.monotonic.tv_sec + d.sec,
            .tv_nsec = t.monotonic.tv_nsec + d.nsec,
        },
    };
    kg_time_ltimespec_normalize(&out.wall);
    kg_time_ltimespec_normalize(&out.monotonic);
    return out;
}
kg_inline kg_time_t kg_time_sub(const kg_time_t t, const kg_duration_t d) {
    kg_time_t out = (kg_time_t){
        .wall = {
            .tv_sec  = t.wall.tv_sec - d.sec,
            .tv_nsec = t.wall.tv_nsec - d.nsec,
        },
        .monotonic = {
            .tv_sec  = t.monotonic.tv_sec - d.sec,
            .tv_nsec = t.monotonic.tv_nsec - d.nsec,
        },
    };
    return out;
}
kg_inline b32 kg_time_is_equal(const kg_time_t t, const kg_time_t o) {
    return t.monotonic.tv_sec == o.monotonic.tv_sec && t.monotonic.tv_nsec == o.monotonic.tv_nsec;
}
kg_inline b32 kg_time_is_after(const kg_time_t t, const kg_time_t o) {
    b32 out;
    if (t.monotonic.tv_sec > o.monotonic.tv_sec) {
        out = true;
    } else if (t.monotonic.tv_sec < o.monotonic.tv_sec) {
        out = false;
    } else {
        out = t.monotonic.tv_nsec > o.monotonic.tv_nsec;
    }
    return out;
}
kg_inline b32 kg_time_is_before(const kg_time_t t, const kg_time_t o) {
    b32 out;
    if (t.monotonic.tv_sec < o.monotonic.tv_sec) {
        out = true;
    } else if (t.monotonic.tv_sec > o.monotonic.tv_sec) {
        out = false;
    } else {
        out = t.monotonic.tv_nsec < o.monotonic.tv_nsec;
    }
    return out;
}
kg_inline kg_duration_t kg_time_diff(const kg_time_t t, const kg_time_t other) {
    kg_duration_t out;
    if (kg_time_is_after(t, other)) {
        out = (kg_duration_t){
            .sec = t.monotonic.tv_sec - other.monotonic.tv_sec,
            .nsec = t.monotonic.tv_nsec - other.monotonic.tv_nsec,
        };
    } else {
        out = (kg_duration_t){
            .sec = other.monotonic.tv_sec - t.monotonic.tv_sec,
            .nsec = other.monotonic.tv_nsec - t.monotonic.tv_nsec,
        };
    }
    if (t.monotonic.tv_nsec < other.monotonic.tv_nsec) {
        out.sec--;
        out.nsec += 1e9L;
    }
    return out;
}
kg_inline kg_duration_t kg_time_since(const kg_time_t t) {
    return kg_time_diff(kg_time_now(), t);
}
kg_inline kg_duration_t kg_duration_since(const kg_time_t t) {
    kg_time_t now = kg_time_now();
    kg_duration_t out = kg_time_diff(now, t);
    return out;
}
kg_string_t kg_time_to_string(const kg_time_t t, kg_allocator_t* a) {
    kg_string_t out = kg_string_create(a, KG_TIME_MAX_CHARS_LEN);
    kg_assert(kg_time_to_cstr(t, out) < KG_TIME_MAX_CHARS_LEN);
    return out;
}
kg_string_t kg_time_now_as_string(kg_allocator_t* a) {
    kg_string_t out = kg_string_create(a, KG_TIME_MAX_CHARS_LEN);
    kg_time_t now = kg_time_now();
    kg_assert(kg_time_to_cstr(now, out) < KG_TIME_MAX_CHARS_LEN);
    return out;
}
// replace this with buffer + buffer size
isize kg_time_to_cstr(const kg_time_t t, char* b) {
    isize out = 0;
    struct tm tm;
    if (null == localtime_r(&t.wall.tv_sec, &tm)) {
        out = 0;
    } else {
        out = strftime(b, KG_TIME_MAX_CHARS_LEN, "%Y-%m-%d %H:%M:%S", &tm);
        isize len = kg_cstr_len(b);
        out += snprintf(b + len, KG_TIME_MAX_CHARS_LEN - len, ".%09ld", t.wall.tv_nsec);
    }
    return out;
}
kg_inline kg_duration_t kg_duration_create(i64 seconds, i64 nanoseconds) {
    return (kg_duration_t){
        .sec  = seconds,
        .nsec = nanoseconds,
    };
}
kg_inline kg_duration_t kg_duration_from_milliseconds(i64 milliseconds) {
    return (kg_duration_t){
        .sec  = milliseconds / 1000,
        .nsec = (milliseconds % 1000) * 1e6L,
    };
}
kg_inline i64 kg_duration_to_milliseconds(const kg_duration_t d) {
    return d.sec * 1000 + d.nsec / 1e6L;
}
f64 kg_duration_to_hours(const kg_duration_t d) {
    return kg_cast(f64)d.sec / (60 * 60);
}
i64 kg_duration_to_seconds(const kg_duration_t d) {
    return kg_cast(i64)d.sec;
}
kg_string_t kg_duration_to_string(const kg_duration_t d, kg_allocator_t* a) {
    return kg_string_from_fmt(a, "%llu sec %lli nsec", d.sec, d.nsec);
}
isize kg_duration_to_cstr(const kg_duration_t d, char* b) {
    kg_panic("kg_duration_to_cstr - not implemented");
    kg_cast(void)d;
    kg_cast(void)b;
    return 0;
}

kg_inline f64 kg_math_pow(f64 base, f64 exponent) {
    f64 out = 1.0;
    for (isize i = 0; i < exponent; i++) {
        out *= base;
    }
    return out;
}
kg_inline i32 kg_isize_compare(const void* a, const void* b) {
    return (*kg_cast(isize*)a) - (*kg_cast(isize*)b);
}
kg_inline i32 kg_u64_compare(const void* a, const void* b) {
    return (*kg_cast(u64*)a) - (*kg_cast(u64*)b);
}
kg_inline i32 kg_i64_compare(const void* a, const void* b) {
    return (*kg_cast(i64*)a) - (*kg_cast(i64*)b);
}

void kg_assert_handler(const char* prefix, const char* condition, const char* file, isize line, const char* fmt, ...) {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_fmt(&allocator, "[%s] %s:%li %s | ", prefix, file, line, condition);
    if (string) {
        va_list args;
        va_start(args, fmt);
        string = kg_string_append_fmt_v(string, fmt, args);
        va_end(args);
    }
    if (string) {
        kg_printf("%s\n", string);
        kg_string_destroy(string);
    }
    kg_exit(1);
}

#endif // KG_IMPL

#ifdef KG_THREADS

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>

typedef struct kg_sema_t {
    sem_t unix_handle;
} kg_sema_t;

b32  kg_sema_create (kg_sema_t* s);
b32  kg_sema_post   (kg_sema_t* s, isize count);
b32  kg_sema_wait   (kg_sema_t* s);
void kg_sema_destroy(kg_sema_t* s);

typedef struct kg_mutex_t {
    pthread_mutex_t     pthread_mutex;
	pthread_mutexattr_t pthread_mutexattr;
} kg_mutex_t;

b32  kg_mutex_create  (kg_mutex_t* m);
b32  kg_mutex_lock    (kg_mutex_t* m);
b32  kg_mutex_try_lock(kg_mutex_t* m);
b32  kg_mutex_unlock  (kg_mutex_t* m);
void kg_mutex_destroy (kg_mutex_t* m);

typedef struct kg_cond_t {
    pthread_cond_t pthread_cond;
} kg_cond_t;

b32  kg_cond_create   (kg_cond_t* c);
b32  kg_cond_broadcast(kg_cond_t* c);
b32  kg_cond_signal   (kg_cond_t* c);
b32  kg_cond_wait     (kg_cond_t* c, kg_mutex_t* m);
void kg_cond_destroy  (kg_cond_t* c);

typedef struct kg_thread_t kg_thread_t;

typedef void* (*kg_thread_fn_t)(void* arg);

typedef struct kg_thread_t {
    pthread_t posix_handle;

    isize          stack_size;
    b32 volatile   is_running;
} kg_thread_t;

b32  kg_thread_create (kg_thread_t* t, kg_thread_fn_t fn, void* user_data);
b32  kg_thread_join   (kg_thread_t* t);
b32  kg_thread_detach (kg_thread_t* t);
i32  kg_thread_id     (void);
void kg_thread_destroy(kg_thread_t* t);

typedef struct kg_task_t {
    kg_thread_fn_t fn;
    void*          arg;
} kg_task_t;

typedef struct kg_pool_t {
    kg_allocator_t* allocator;
    kg_queue_t      task_queue;
    kg_mutex_t      work_mutex;
    kg_cond_t       work_cond;
    kg_cond_t       working_cond;
    kg_thread_t*    workers;
    isize           working_count;
    isize           workers_n;
    b32             stop;
} kg_pool_t;

b32  kg_pool_create  (kg_pool_t* p, kg_allocator_t* a, isize n);
b32  kg_pool_add_task(kg_pool_t* p, kg_thread_fn_t fn, void* arg);
b32  kg_pool_join    (kg_pool_t* p);
void kg_pool_destroy (kg_pool_t* p);

#endif // KG_THREADS

#ifdef KG_THREADS_IMPL

b32 kg_sema_create(kg_sema_t* s) {
    return s ? sem_init(&s->unix_handle, 0, 0) == 0 : false;
}
b32 kg_sema_post(kg_sema_t* s, isize count) {
    b32 out_ok = false;
    if (s) {
        while(count-- > 0) { sem_post(&s->unix_handle); }
        out_ok = true;
    }
    return out_ok;
}
b32 kg_sema_wait(kg_sema_t* s) {
    b32 out_ok = true;
    int i; do { i = sem_wait(&s->unix_handle); } while(i == -1 && errno == EINTR);
    return out_ok;
}
void kg_sema_destroy(kg_sema_t* s) {
    if (s) {
        sem_destroy(&s->unix_handle);
    }
}

b32 kg_mutex_create(kg_mutex_t* m) {
    b32 out_ok = true;
    *m = (kg_mutex_t){0};
    out_ok = pthread_mutexattr_init(&m->pthread_mutexattr) == 0;
    if (out_ok) {
	    out_ok = pthread_mutexattr_settype(&m->pthread_mutexattr, PTHREAD_MUTEX_RECURSIVE) == 0;
        if (out_ok) {
            out_ok = pthread_mutex_init(&m->pthread_mutex, &m->pthread_mutexattr) == 0;
        }
    }
    return out_ok;
}
b32 kg_mutex_lock(kg_mutex_t* m) {
    return m ? pthread_mutex_lock(&m->pthread_mutex) == 0 : false;
}
b32 kg_mutex_try_lock(kg_mutex_t* m) {
    return m ? pthread_mutex_trylock(&m->pthread_mutex) == 0 : false;
}
b32 kg_mutex_unlock(kg_mutex_t* m) {
    return m ? pthread_mutex_unlock(&m->pthread_mutex) == 0 : false;
}
void kg_mutex_destroy(kg_mutex_t* m) {
    if (m) {
        pthread_mutex_destroy(&m->pthread_mutex);
    }
}

b32 kg_cond_create(kg_cond_t* c) {
    b32 out_ok = true;
    pthread_condattr_t attr;
    out_ok = pthread_condattr_init(&attr) == 0;
    if (out_ok) {
        out_ok = pthread_cond_init(&c->pthread_cond, &attr) == 0;
        if (out_ok) {
            out_ok = pthread_condattr_destroy(&attr) == 0;
        }
    }
    return out_ok;
}
b32 kg_cond_broadcast(kg_cond_t* c) {
    b32 out_ok = true;
    out_ok = pthread_cond_broadcast(&c->pthread_cond) == 0;
    return out_ok;
}
b32 kg_cond_signal(kg_cond_t* c) {
    b32 out_ok = true;
    out_ok = pthread_cond_signal(&c->pthread_cond) == 0;
    return out_ok;
}
b32 kg_cond_wait(kg_cond_t* c, kg_mutex_t* m) {
    b32 out_ok = true;
    out_ok = pthread_cond_wait(&c->pthread_cond, &m->pthread_mutex) == 0;
    return out_ok;
}
void kg_cond_destroy(kg_cond_t* c) {
    if (c) {
        pthread_cond_destroy(&c->pthread_cond);
        kg_mem_zero(c, kg_sizeof(kg_cond_t));
    }
}

b32 kg_thread_create(kg_thread_t* t, kg_thread_fn_t fn, void* arg) {
    b32 out_ok = false;
    kg_mem_zero(t, kg_sizeof(kg_thread_t));
    t->is_running = true;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    out_ok = pthread_create(&t->posix_handle, &attr, fn, arg) == 0;
    pthread_attr_destroy(&attr);
    return out_ok;
}
b32 kg_thread_join(kg_thread_t* t) {
    b32 out_ok = false;
    if (t && t->is_running) {
        out_ok = pthread_join(t->posix_handle, null) == 0;
        kg_mem_zero(t, kg_sizeof(kg_thread_t));
    }
    return out_ok;
}
b32 kg_thread_detach(kg_thread_t* t) {
    b32 out_ok = false;
    out_ok = pthread_detach(t->posix_handle) == 0;
    return out_ok;
}
i32 kg_thread_id(void) {
    return pthread_self();
}
void kg_thread_destroy(kg_thread_t* t) {
    if (t) {
        if (t->is_running) {
            kg_thread_join(t);
        }
        kg_mem_zero(t, kg_sizeof(kg_thread_t));
    } 
}

void* kg_pool_loop_(void* arg) {
    kg_pool_t* p = kg_cast(kg_pool_t*)arg;
    kg_task_t task = {0};
    while (true) {
        kg_mutex_lock(&p->work_mutex);
        while (kg_queue_is_empty(&p->task_queue) && !p->stop) {
            kg_cond_wait(&p->work_cond, &p->work_mutex);
        }
        if (p->stop) {
            kg_mutex_unlock(&p->work_mutex);
            break;
        }
        b32 has_task = kg_queue_deque(&p->task_queue, &task);
        p->working_count++;
        kg_mutex_unlock(&p->work_mutex);
        if (has_task && task.fn) {
            task.fn(task.arg);
        }
        kg_mutex_lock(&p->work_mutex);
        p->working_count--;
        if (!p->stop && p->working_count == 0 && kg_queue_is_empty(&p->task_queue)) {
            kg_cond_signal(&p->working_cond);
        }
        kg_mutex_unlock(&p->work_mutex);
    }
    return null;
}
b32 kg_pool_create(kg_pool_t* p, kg_allocator_t* a, isize n) {
    kg_static isize initial_queue_cap = 16;
    b32 out_ok = false;
    if (p && a && n > 0) {
        *p = (kg_pool_t){
            .allocator = a,
            .workers_n = n,
        };
        p->workers = kg_allocator_alloc(p->allocator, kg_sizeof(kg_thread_t) * p->workers_n);
        if (p->workers) {
            if (!kg_mutex_create(&p->work_mutex)) goto cleanup;
            if (!kg_cond_create(&p->work_cond)) goto cleanup;
            if (!kg_cond_create(&p->working_cond)) goto cleanup;
            if (!kg_queue_create(&p->task_queue, p->allocator, kg_sizeof(kg_task_t), initial_queue_cap)) goto cleanup;
            for (isize i = 0; i < p->workers_n; i++) {
                if (!kg_thread_create(&p->workers[i], kg_pool_loop_, p)) goto cleanup;
            }
            out_ok = true;

cleanup:
    if (!out_ok) {
        kg_pool_destroy(p);
    }
        }
    }

    return out_ok;
}
b32 kg_pool_add_task(kg_pool_t* p, kg_thread_fn_t fn, void* arg) {
    b32 out_ok = false;
    if (p && fn) {
        kg_mutex_lock(&p->work_mutex);
        kg_task_t task = (kg_task_t){
            .fn = fn,
            .arg = arg,
        };
        out_ok = kg_queue_enqueue(&p->task_queue, &task);
        kg_cond_signal(&p->work_cond);
        kg_mutex_unlock(&p->work_mutex);
    }
    return out_ok;
}
b32 kg_pool_join(kg_pool_t* p) {
    b32 out_ok = true;
    kg_mutex_lock(&p->work_mutex);
    while (true) {
        if (!kg_queue_is_empty(&p->task_queue) || p->working_count > 0) {
            kg_cond_wait(&p->working_cond, &p->work_mutex);
        } else {
            break;
        }
    }
    kg_mutex_unlock(&p->work_mutex);
    return out_ok;
}
void kg_pool_destroy(kg_pool_t* p) {
    if (p) {
        kg_mutex_lock(&p->work_mutex);
        p->stop = true;
        kg_cond_broadcast(&p->work_cond);
        kg_mutex_unlock(&p->work_mutex);
        kg_pool_join(p);
        kg_mutex_destroy(&p->work_mutex);
        kg_cond_destroy(&p->work_cond);
        kg_cond_destroy(&p->working_cond);
        for (isize i = 0; i < p->workers_n; i++) {
            kg_thread_destroy(&p->workers[i]);
        }
        kg_queue_destroy(&p->task_queue);
        if (p->workers) {
            kg_allocator_free(p->allocator, p->workers, kg_sizeof(kg_thread_t) * p->workers_n);
        }
        kg_mem_zero(p, kg_sizeof(kg_pool_t));
    }
}

#endif // KG_THREADS_IMPL

#ifdef KG_FLAGS

void kg_flag_str   (kg_str_t* holder, const char* name, const kg_str_t default_value, const char* usage);
void kg_flag_b32   (b32* holder, const char* name, b32 default_value, const char* usage);
void kg_flag_u64   (u64* holder, const char* name, u64 default_value, const char* usage);
void kg_flag_i64   (i64* holder, const char* name, i64 default_value, const char* usage);
void kg_flags_parse(i32 argc, char* argv[]);
void kg_flags_usage(void);

#endif // KG_FLAGS

#ifdef KG_FLAGS_IMPL

typedef enum kg_flag_kind_t {
    KG_FLAG_KIND_STR = 0,
    KG_FLAG_KIND_B32,
    KG_FLAG_KIND_U64,
    KG_FLAG_KIND_I64,
    KG_FLAG_KIND__SENTINEL,
} kg_flag_kind_t;

typedef struct kg_flag_t {
    kg_flag_kind_t    kind;
    kg_str_t          raw_value;
    const char*       name;
    const kg_str_t    name_str;
    const char*       usage;
    const kg_str_t    usage_str;
    union {
        kg_str_t* holder_str;
        b32*      holder_b32;
        u64*      holder_u64;
        i64*      holder_i64;
    };
    union {
        const kg_str_t default_str;
        b32            default_b32;
        u64            default_u64;
        i64            default_i64;
    };
} kg_flag_t;

#define KG_FLAGS_MAX_LEN 128

kg_static kg_flag_t kg_flags[KG_FLAGS_MAX_LEN];
kg_static isize     kg_flags_len = 0;

const char* kg_flag_kind_to_cstr(kg_flag_kind_t k) {
    kg_static const char* kind_cstrs[] = {
        [KG_FLAG_KIND_STR] = "str",
        [KG_FLAG_KIND_B32] = "b32",
        [KG_FLAG_KIND_U64] = "u64",
        [KG_FLAG_KIND_I64] = "i64",
    };
    return kind_cstrs[k];
}
void kg_flag_register_handler(kg_flag_t f) {
    kg_assert(kg_flags_len <= KG_FLAGS_MAX_LEN);
    kg_mem_copy(kg_flags + kg_flags_len, &f, kg_sizeof(kg_flag_t));
    kg_flags_len++;
}
void kg_flag_parse_str(kg_flag_t* f, kg_str_t raw_value) {
    f->raw_value = raw_value;
    kg_assert(!kg_str_is_null(raw_value));
    *f->holder_str = raw_value;
}
void kg_flag_str(kg_str_t* holder, const char* name, const kg_str_t default_value, const char* usage) {
    *holder = default_value;
    kg_assert(holder);
    kg_assert(name);
    kg_assert(!kg_str_is_null(default_value));
    kg_assert(usage);
    kg_flag_t f = {
        .kind        = KG_FLAG_KIND_STR,
        .name        = name,
        .name_str    = kg_str_create(name),
        .usage       = usage,
        .usage_str   = kg_str_create(usage),
        .default_str = default_value,
        .holder_str  = holder,
    };
    kg_flag_register_handler(f);
}
void kg_flag_parse_b32(kg_flag_t* f, kg_str_t raw_value) {
    f->raw_value = raw_value;
    kg_assert_fmt(kg_str_to_b32(f->holder_b32, raw_value), "invalid str representation of b32")
}
void kg_flag_b32(b32* holder, const char* name, b32 default_value, const char* usage) {
    *holder = default_value;
    kg_assert(holder);
    kg_assert(name);
    kg_assert(usage);
    kg_flag_t f = {
        .kind        = KG_FLAG_KIND_B32,
        .name        = name,
        .name_str    = kg_str_create(name),
        .usage       = usage,
        .usage_str   = kg_str_create(usage),
        .default_b32 = default_value,
        .holder_b32  = holder,
    };
    kg_flag_register_handler(f);
}
void kg_flag_parse_i64(kg_flag_t* f, kg_str_t raw_value) {
    f->raw_value = raw_value;
    kg_assert_fmt(kg_str_to_i64(f->holder_i64, raw_value), "invalid str representation of i64")
}
void kg_flag_i64(i64* holder, const char* name, i64 default_value, const char* usage) {
    *holder = default_value;
    kg_assert(holder);
    kg_assert(name);
    kg_assert(usage);
    kg_flag_t f = {
        .kind        = KG_FLAG_KIND_I64,
        .name        = name,
        .name_str    = kg_str_create(name),
        .usage       = usage,
        .usage_str   = kg_str_create(usage),
        .default_i64 = default_value,
        .holder_i64  = holder,
    };
    kg_flag_register_handler(f);
}
void kg_flag_parse_u64(kg_flag_t* f, kg_str_t raw_value) {
    f->raw_value = raw_value;
    kg_assert_fmt(kg_str_to_u64(f->holder_u64, raw_value), "invalid str representation of u64")
}
void kg_flag_u64(u64* holder, const char* name, u64 default_value, const char* usage) {
    *holder = default_value;
    kg_assert(holder);
    kg_assert(name);
    kg_assert(usage);
    kg_flag_t f = {
        .kind        = KG_FLAG_KIND_U64,
        .name        = name,
        .name_str    = kg_str_create(name),
        .usage       = usage,
        .usage_str   = kg_str_create(usage),
        .default_u64 = default_value,
        .holder_u64  = holder,
    };
    kg_flag_register_handler(f);
}
void kg_flags_parse(i32 argc, char* argv[]) {
    typedef struct kg_arg_meta_t {
        kg_str_t name;
        kg_str_t value;
    }  kg_arg_meta_t;
    kg_arg_meta_t arg_metas[KG_FLAGS_MAX_LEN];
    isize arg_metas_len = 0;
    kg_str_t arg_split = kg_str_create("=");
    kg_str_t arg_prefix = kg_str_create("--");
    for (i32 i = 1; i < argc; i++) {
        char* arg = argv[i];
        kg_str_t arg_str = kg_str_create(arg);
        if (kg_str_has_prefix(arg_str, arg_prefix)) {
            arg_str = kg_str_substr(arg_str, arg_prefix.len, arg_str.len);
            kg_str_t name = kg_str_chop_first_split_by(&arg_str, arg_split);
            kg_str_t value = arg_str;
            kg_assert(arg_metas_len < KG_FLAGS_MAX_LEN);
            arg_metas[arg_metas_len++] = (kg_arg_meta_t){
                .name  = name,
                .value = value,
            };
        }
    }
    kg_flag_t* flag;
    for (i32 i = 0; i < kg_flags_len; i++) {
        flag = &kg_flags[i];
        kg_arg_meta_t arg_meta;
        for (isize j = 0; j < arg_metas_len; j++) {
            arg_meta = arg_metas[j];
            if (kg_str_is_equal(flag->name_str, arg_meta.name)) {
                switch(flag->kind) {
                    case KG_FLAG_KIND_STR:
                        kg_flag_parse_str(flag, arg_meta.value);
                        break;
                    case KG_FLAG_KIND_B32:
                        kg_flag_parse_b32(flag, arg_meta.value);
                        break;
                    case KG_FLAG_KIND_U64:
                        kg_flag_parse_u64(flag, arg_meta.value);
                        break;
                    case KG_FLAG_KIND_I64:
                        kg_flag_parse_i64(flag, arg_meta.value);
                        break;
                    default:
                        kg_panic("KG_FLAGS - unknown flag kind '%d'", flag->kind);
                }
                break;
            }
        }
    }
}
void kg_flags_usage(void) {
    for (isize i = 0; i < kg_flags_len; i++) {
        kg_flag_t f = kg_flags[i];
        switch (f.kind) {
            case KG_FLAG_KIND_STR:
                kg_printf("--%s\n\tkind:    %s\n\tusage:   %s\n\tdefault: '%.*s'\n", f.name, kg_flag_kind_to_cstr(f.kind), f.usage, f.default_str.len, f.default_str.ptr);
                break;
            case KG_FLAG_KIND_B32:
                kg_printf("--%s\n\tkind:    %s\n\tusage:   %s\n\tdefault: %s\n", f.name, kg_flag_kind_to_cstr(f.kind), f.usage, f.default_b32 ? "true" : "false");
                break;
            case KG_FLAG_KIND_I64:
                kg_printf("--%s\n\tkind:    %s\n\tusage:   %s\n\tdefault: %lli\n", f.name, kg_flag_kind_to_cstr(f.kind), f.usage, f.default_str.len, f.default_i64);
                break;
            case KG_FLAG_KIND_U64:
                kg_printf("--%s\n\tkind:    %s\n\tusage:   %s\n\tdefault: %llu\n", f.name, kg_flag_kind_to_cstr(f.kind), f.usage, f.default_str.len, f.default_u64);
                break;
            default:
                kg_panic("KG_FLAGS - unknown flag kind '%d'", f.kind);
        }
    }
}

#endif // KG_FLAGS_IMPL

#ifdef KG_LOGGER

typedef enum kg_log_level_t {
    KG_LOG_LEVEL_RAW   = 0,
    KG_LOG_LEVEL_TRACE = 1,
    KG_LOG_LEVEL_DEBUG = 2,
    KG_LOG_LEVEL_INFO  = 3,
    KG_LOG_LEVEL_WARN  = 4,
    KG_LOG_LEVEL_ERROR = 5,
    KG_LOG_LEVEL_FATAL = 6,
    KG_LOG_LEVEL__SENTINEL,
} kg_log_level_t;

#define KG_LOG_RAW   1
#define KG_LOG_TRACE 1
#define KG_LOG_DEBUG 1
#define KG_LOG_INFO  1
#define KG_LOG_WARN  1
#define KG_LOG_ERROR 1
#define KG_LOG_FATAL 1
#define KG_LOG_NONE  0

typedef struct kg_logger_context_t {
#ifdef KG_THREADS
    kg_mutex_t mutex;
#endif
} kg_logger_context_t;

void kg_logger_setup(void);
void kg_log_handler (kg_log_level_t level, const char* file, isize line, const char* fmt, ...);

#if KG_LOG_NONE != 1 && KG_LOG_RAW == 1
    #define kg_log(fmt, ...) kg_log_handler(KG_LOG_LEVEL_RAW, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_TRACE == 1
    #define kg_log_trace(fmt, ...) kg_log_handler(KG_LOG_LEVEL_TRACE, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_trace(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_DEBUG == 1
    #define kg_log_debug(fmt, ...) kg_log_handler(KG_LOG_LEVEL_DEBUG, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_debug(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_INFO == 1
    #define kg_log_info(fmt, ...) kg_log_handler(KG_LOG_LEVEL_INFO, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_info(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_WARN == 1
    #define kg_log_warn(fmt, ...) kg_log_handler(KG_LOG_LEVEL_WARN, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_warn(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_ERROR == 1
    #define kg_log_error(fmt, ...) kg_log_handler(KG_LOG_LEVEL_ERROR, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_error(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_FATAL == 1
    #define kg_log_fatal(fmt, ...) kg_log_handler(KG_LOG_LEVEL_FATAL, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_fatal(fmt, ...)
#endif

#ifdef KG_LOGGER_IMPL

#ifdef KG_THREADS
kg_static kg_logger_context_t kg_logger_context;
#endif
void kg_logger_setup(void) {
#ifdef KG_THREADS
    kg_mutex_create(&kg_logger_context.mutex);
#endif
}
void kg_log_handler(kg_log_level_t level, const char* file, isize line, const char* fmt, ...) {
#ifdef KG_THREADS
    kg_mutex_lock(&kg_logger_context.mutex);
#endif
    kg_cast(void)file;
    kg_cast(void)line;
    kg_static const char* level_cstrs[KG_LOG_LEVEL__SENTINEL] = {
        [KG_LOG_LEVEL_RAW]   = "",
        [KG_LOG_LEVEL_TRACE] = "[trace]",
        [KG_LOG_LEVEL_DEBUG] = "[debug]",
        [KG_LOG_LEVEL_INFO]  = "[info ]",
        [KG_LOG_LEVEL_WARN]  = "[warn ]",
        [KG_LOG_LEVEL_ERROR] = "[error]",
        [KG_LOG_LEVEL_FATAL] = "[fatal]",
    };
    static const char* level_colors[] = {
        [KG_LOG_LEVEL_RAW]   = "",
        [KG_LOG_LEVEL_TRACE] = "\x1b[94m",
        [KG_LOG_LEVEL_DEBUG] = "\x1b[36m",
        [KG_LOG_LEVEL_INFO]  = "\x1b[32m",
        [KG_LOG_LEVEL_WARN]  = "\x1b[33m",
        [KG_LOG_LEVEL_ERROR] = "\x1b[31m",
        [KG_LOG_LEVEL_FATAL] = "\x1b[35m",
    };
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_builder_t sb;
    if (kg_string_builder_create(&sb, &allocator, 128)) {
        if (level != KG_LOG_LEVEL_RAW) {
            kg_time_t time = kg_time_now();
            char time_cstr[KG_TIME_MAX_CHARS_LEN] = {0};
            if (kg_time_to_cstr(time, time_cstr) > 0) {
                kg_string_builder_write_fmt(&sb, "\x1b[90m%s\x1b[0m %s%s \x1b[0m", 
                                            time_cstr, 
                                            level_colors[level],
                                            level_cstrs[level]);
            }
        } 
        va_list args;
        va_start(args, fmt);
        kg_string_builder_write_fmt_v(&sb, fmt, args);
        va_end(args);
        kg_string_t final_msg = kg_string_builder_to_string(&sb, &allocator);
        if (final_msg) {
            kg_printf("%s\n", final_msg);
        }
        kg_string_destroy(final_msg);
        kg_string_builder_destroy(&sb);
    }
    if (level == KG_LOG_LEVEL_FATAL) {
        kg_exit(1);
    }
#ifdef KG_THREADS
    kg_mutex_unlock(&kg_logger_context.mutex);
#endif
}

#endif // KG_LOGGER_IMPL

#endif // LG_LOGGER

#ifdef KG_TESTER

typedef struct kgt_t {
    isize _;
} kgt_t;

typedef void (*kgt_fn_t)(kgt_t* t);

typedef struct kgt_test_t {
    kgt_fn_t fn;
} kgt_test_t;

void       kgt_create  (kgt_t* t);
kgt_test_t kgt_register(kgt_fn_t fn);
void       kgt_run     (kgt_t* t, kgt_test_t* tests, isize tests_len);
void       kgt_destroy (kgt_t* t);

void kgt_expect_handler(const char* cond, const char* msg, const char* file, isize line, const char* fn_name);

#define kgt_expect(cond, msg)          if (cond) {} else { kgt_expect_handler(#cond, msg, __FILE__, kg_cast(isize)__LINE__, __func__); }
#define kgt_expect_null(a)             kgt_expect(a == null, "expected null")
#define kgt_expect_not_null(a)         kgt_expect(a != null, "expected not null")
#define kgt_expect_eq(a, b)            kgt_expect(a == b, "expected eq")
#define kgt_expect_neq(a, b)           kgt_expect(a != b, "expected neq")
#define kgt_expect_lt(a, b)            kgt_expect(a < b, "expected lt")
#define kgt_expect_lte(a, b)           kgt_expect(a <= b, "expected lte")
#define kgt_expect_gt(a, b)            kgt_expect(a > b, "expected gt")
#define kgt_expect_gte(a, b)           kgt_expect(a >= b, "expected gte")
#define kgt_expect_ptr_eq(a, b)        kgt_expect(a == b, "expected ptr eq")
#define kgt_expect_ptr_neq(a, b)       kgt_expect(a != b, "expected ptr neq")
#define kgt_expect_true(a)             kgt_expect(a == true, "expected true")
#define kgt_expect_false(a)            kgt_expect(a == false, "expected false")
#define kgt_expect_mem_eq(a, b, s)     kgt_expect(kg_mem_compare(a, b, s) == 0, "expected memory eq")
#define kgt_expect_mem_neq(a, c, s)    kgt_expect(kg_mem_compare(a, b, s) != 0, "expected memory neq")
#define kgt_expect_cstr_eq(a, b)       kgt_expect(kg_cstr_compare(a, b) == 0, "expected cstr eq")
#define kgt_expect_cstr_neq(a, c)      kgt_expect(kg_cstr_compare(a, b) != 0, "expected cstr neq")
#define kgt_expect_cstr_n_eq(a, b, n)  kgt_expect(kg_cstr_compare_n(a, b, n) == 0, "expected cstr n eq")
#define kgt_expect_cstr_n_neq(a, c, n) kgt_expect(kg_cstr_compare_n(a, b, n) != 0, "expected cstr n neq")

#endif // KG_TESTER

#ifdef KG_TESTER_IMPL

void kgt_expect_handler(const char* cond, const char* msg, const char* file, isize line, const char* fn_name) {
    kg_printf("\x1b[31m[test_failed]\x1b[0m %s (%s:%li) %s %s\n", fn_name, file, line, msg, cond);
    /*kg_exit(1);*/
}

void kgt_create(kgt_t* t) {
    kg_mem_zero(t, kg_sizeof(kgt_t));
}
kgt_test_t kgt_register(kgt_fn_t fn) {
    kgt_test_t out = (kgt_test_t){
        .fn = fn,
    };
    return out;
}
void kgt_run(kgt_t* t, kgt_test_t* tests, isize tests_len) {
    for (isize i = 0; i < tests_len; i++) {
        kgt_test_t test = tests[i];
        test.fn(t);
    }
}
void kgt_destroy(kgt_t* t) {
    if (t) {
        kg_mem_zero(t, kg_sizeof(kgt_t));
    }
}

#endif // KG_TESTER_IMPL

#endif // KG_H
