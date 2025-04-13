#pragma once

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>

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

#define U64_MAX_CHARS_LEN      21
#define I64_MAX_CHARS_LEN      21
#define DATE_MAX_CHARS_LEN     21
#define DATETIME_MAX_CHARS_LEN 41
#define B32_MAX_CHARS_LEN      6
#define F64_MAX_CHARS_LEN      25

#define kg_bit(x) (1<<(x))

#define kg_mask_set(var, set, mask) do { \
    if (set) (var) |=  (mask); \
    else     (var) &= ~(mask); \
} while(0)

#define kg_cast(T)       (T)
#define kg_sizeof(T)     kg_cast(isize)sizeof(T)
#define kg_static_assert static_assert
#define kg_static        static
#define kg_inline        inline
#define kg_extern        extern

#if defined(__linux__)
    #define KG_PLATFORM_LINUX 1
#else
    #error "Unknown platform"
#endif

#define kg_exit(code) exit(code)

void kg_printf(const char* fmt, ...);

void kg_assert_handler(const char* prefix, const char* condition, const char* file, isize line, const char* fmt, ...);

#define kg_assert_fmt(cond, fmt, ...) if (cond) {} else {kg_assert_handler("Assertion failed", #cond, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__);}
#define kg_assert(cond)               kg_assert_fmt(cond, null)
#define kg_panic(fmt, ...)            kg_assert_handler("Panic", null, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)

void* kg_mem_alloc  (isize size);
void* kg_mem_resize (void* ptr, isize size);
void  kg_mem_free   (void* ptr);
void* kg_mem_copy   (void* dest, const void* src, isize size);
void* kg_mem_set    (void* dest, u8 byte_value, isize size);
void* kg_mem_zero   (void* dest, isize size);
i32   kg_mem_compare(const void* a, const void* b, isize size);
void  kg_mem_swap   (void* a, void* b, isize size);
void* kg_mem_move   (void* dest, const void* src, isize size);

typedef struct kg_allocator_t kg_allocator_t;

typedef struct kg_arena_t {
    kg_allocator_t* allocator;
    void*           real_ptr;
    isize           max_size;
    isize           allocated_size;
} kg_arena_t;

b32   kg_arena_create   (kg_arena_t* a, kg_allocator_t* allocator, isize max_size);
isize kg_arena_allocated(const kg_arena_t* a);
isize kg_arena_available(const kg_arena_t* a);
isize kg_arena_mem_size (const kg_arena_t* a);
void  kg_arena_reset    (kg_arena_t* a);
void  kg_arena_destroy  (kg_arena_t* a);

kg_allocator_t kg_allocator_default(void);
kg_allocator_t kg_allocator_temp   (kg_arena_t* a);

#define kg_allocator_alloc_one(a, T)                    (T*)((a)->proc.alloc(a, kg_sizeof(T)))
#define kg_allocator_alloc_array(a, T, n)               (T*)((a)->proc.alloc(a, kg_sizeof(T) * n))
#define kg_allocator_alloc(a, size)                     (a)->proc.alloc(a, size)
#define kg_allocator_free(a, ptr, size)                 (a)->proc.free(a, ptr, size)
#define kg_allocator_free_all(a, clear)                 (a)->proc.free_all(a, clear)
#define kg_allocator_resize(a, ptr, old_size, new_size) (a)->proc.resize(a, ptr, old_size, new_size)

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

typedef i32 (*kg_compare_fn_t)(const void* a, const void* b, isize size);

void kg_quicksort(void* src, isize lo, isize hi, isize stride, kg_compare_fn_t compare_fn);

typedef struct kg_string_header_t {
    isize           len;
    isize           cap;
    kg_allocator_t* allocator;
} kg_string_header_t;

#define kg_string_header(s) (kg_cast(kg_string_header_t*)(s) - 1)

typedef char* kg_string_t;

kg_string_t kg_string_create          (kg_allocator_t* a, isize cap);
kg_string_t kg_string_from_fmt        (kg_allocator_t* a, const char* fmt, ...);
kg_string_t kg_string_from_fmt_v      (kg_allocator_t* a, const char* fmt, va_list args);
kg_string_t kg_string_from_cstr       (kg_allocator_t* a, const char* cstr);
kg_string_t kg_string_from_cstr_n     (kg_allocator_t* a, const char* cstr, isize cstr_len);
kg_string_t kg_string_append          (kg_string_t s, kg_string_t other);
kg_string_t kg_string_append_fmt      (kg_string_t s, const char* fmt, ...);
kg_string_t kg_string_append_fmt_v    (kg_string_t s, const char* fmt, va_list args);
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
void        kg_string_destroy         (kg_string_t s);

typedef struct kg_str_t {
    isize       len;
    const char* ptr;
} kg_str_t;

#define kg_str_create_empty() ((kg_str_t){.len = 0, .ptr = ""})
#define kg_str_create_null()  ((kg_str_t){.len = 0, .ptr = null})

kg_str_t kg_str_create             (const char* cstr);
kg_str_t kg_str_create_n           (const char* cstr, isize cstr_len);
kg_str_t kg_str_from_string        (const kg_string_t s);
kg_str_t kg_str_from_string_n      (const kg_string_t s, isize len);
kg_str_t kg_str_chop_first_split_by(kg_str_t* s, const kg_str_t split_by);
kg_str_t kg_str_substr             (const kg_str_t s, isize start_inc, isize end_exc);
kg_str_t kg_str_substr_from        (const kg_str_t s, isize start_inc);
kg_str_t kg_str_substr_to          (const kg_str_t s, isize end_exc);
kg_str_t kg_str_trim_space         (const kg_str_t s);
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

b32 kg_str_to_b32(b32* b, const kg_str_t s);
b32 kg_str_to_u64(u64* u, const kg_str_t s);
b32 kg_str_to_i64(i64* i, const kg_str_t s);

isize       kg_b32_to_cstr  (char buf[B32_MAX_CHARS_LEN], b32 b);
kg_string_t kg_b32_to_string(kg_allocator_t* a, b32 b);
isize       kg_u64_to_cstr  (char buf[U64_MAX_CHARS_LEN], u64 u);
kg_string_t kg_u64_to_string(kg_allocator_t* a, u64 u);
isize       kg_i64_to_cstr  (char buf[I64_MAX_CHARS_LEN], i64 i);
kg_string_t kg_i64_to_string(kg_allocator_t* a, i64 i);
isize       kg_f64_to_cstr  (char buf[F64_MAX_CHARS_LEN], f64 f);
kg_string_t kg_f64_to_string(kg_allocator_t* a, f64 f);

char kg_char_to_lower(char c);
char kg_char_to_upper(char c);
b32  kg_char_is_space(char c);

typedef struct kg_string_builder_t {
    kg_allocator_t* allocator;
    isize           cap;
    isize           len;
    char*           write_ptr;
    char*           real_ptr;
} kg_string_builder_t;

b32         kg_string_builder_create          (kg_string_builder_t* b, kg_allocator_t* a, isize cap);
b32         kg_string_builder_write_unsafe    (kg_string_builder_t* b, const char* cstr, isize n);
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
b32         kg_string_builder_cap             (const kg_string_builder_t* b);
b32         kg_string_builder_len             (const kg_string_builder_t* b);
b32         kg_string_builder_available       (const kg_string_builder_t* b);
b32         kg_string_builder_ensure_available(kg_string_builder_t* b, isize n);
b32         kg_string_builder_reset           (kg_string_builder_t* b);
kg_string_t kg_string_builder_to_string       (const kg_string_builder_t* b, kg_allocator_t* a);
isize       kg_string_builder_grow_formula    (const kg_string_builder_t* b, isize n);
isize       kg_string_builder_mem_size        (const kg_string_builder_t* b);
void        kg_string_builder_destroy         (kg_string_builder_t* b);

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
#define kg_darray_append(d, item)        do { \
    kg_darray_ensure_available(d, 1); \
    (d)[kg_darray_header(d)->len++] = (item); \
} while(0);
#define kg_darray_len(d)                 (d ? kg_darray_header(d)->len : 0)
#define kg_darray_cap(d)                 (d ? kg_darray_header(d)->cap : 0)
#define kg_darray_stride(d)              (d ? kg_darray_header(d)->stride : 0)
#define kg_darray_allocator(d)           (d ? kg_darray_header(d)->allocator : null)
#define kg_darray_grow(d, n)             do { (d) = kg_darray_grow_(d, n); } while(0)
#define kg_darray_grow_formula(n, min)   kg_cast(isize)((n * 2) + min)
#define kg_darray_available(d)           (kg_darray_cap(d) > kg_darray_len(d) ? kg_darray_cap(d) - kg_darray_len(d) : 0)
#define kg_darray_ensure_available(d, n) do { \
    if (kg_darray_available(d) < n) { \
        kg_darray_grow(d, kg_darray_grow_formula(kg_darray_cap(d), n)); \
    } \
} while(0)
#define kg_darray_mem_size(d)            (kg_sizeof(kg_darray_header_t) + kg_darray_cap(d) * kg_darray_stride(d))
#define kg_darray_destroy(d)             do { \
    kg_darray_header_t* h = kg_darray_header(d); \
    kg_allocator_free(h->allocator, h, kg_darray_mem_size(d)); \
} while(0)

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

void              kg_file_create      (kg_file_t* f);
b32               kg_file_open        (kg_file_t* f, const char* filename, kg_file_mode_t mode, b32 binary);
isize             kg_file_size        (kg_file_t* f);
kg_file_content_t kg_file_read_content(kg_allocator_t* a, const char* filename);
b32               kg_file_close       (kg_file_t* f);

typedef struct kg_time_t {
    i64 milliseconds;
} kg_time_t;

typedef struct kg_duration_t {
    i64 milliseconds;
    // u64 seconds;
    // u64 nanoseconds;
} kg_duration_t;

typedef enum kg_month_t {
    January   = 1, 
    February, 
    March, 
    April, 
    May, 
    June, 
    July, 
    August, 
    September, 
    October, 
    November, 
    December, 
} kg_month_t;

typedef struct kg_date_t {
    i32        year;
    kg_month_t month;
    i32        day;
} kg_date_t;

typedef struct kg_datetime_t {
    i32        year;
    kg_month_t month;
    u8         day;
    u8         hour;
    u8         minute;
    u8         second;
} kg_datetime_t;

const u64 KG_NANOSECOND  = 1;
const u64 KG_MICROSECOND = 1000 * KG_NANOSECOND;
const u64 KG_MILLISECOND = 1000 * KG_MICROSECOND;
const u64 KG_SECOND      = 1000 * KG_MILLISECOND;
const u64 KG_MINUTE      = 60 * KG_SECOND;
const u64 KG_HOUR        = 60 * KG_MINUTE;

void          kg_time_sleep      (const kg_duration_t d);
kg_time_t     kg_time_now        (void);
kg_time_t     kg_time_add        (const kg_time_t t, const kg_duration_t d);
kg_time_t     kg_time_sub        (const kg_time_t t, const kg_duration_t d);
b32           kg_time_is_equal   (const kg_time_t t, const kg_time_t o);
b32           kg_time_is_after   (const kg_time_t t, const kg_time_t o);
b32           kg_time_is_before  (const kg_time_t t, const kg_time_t o);
kg_duration_t kg_time_diff       (const kg_time_t t, const kg_time_t other);
kg_date_t     kg_time_to_date    (const kg_time_t t);
kg_datetime_t kg_time_to_datetime(const kg_time_t t);
i32           kg_time_to_year    (const kg_time_t t);
kg_month_t    kg_time_to_month   (const kg_time_t t);
i32           kg_time_to_day     (const kg_time_t t);

kg_duration_t kg_duration_create           (i64 milliseconds);
kg_duration_t kg_duration_from_milliseconds(i64 milliseconds);
kg_duration_t kg_duration_since            (const kg_time_t t);
i64           kg_duration_to_milliseconds  (const kg_duration_t d);

kg_string_t kg_date_to_string    (kg_allocator_t* a, const kg_date_t d);
isize       kg_date_to_cstr      (char b[DATE_MAX_CHARS_LEN], const kg_date_t d);
kg_string_t kg_datetime_to_string(kg_allocator_t* a, const kg_datetime_t d);
isize       kg_datetime_to_cstr  (char b[DATETIME_MAX_CHARS_LEN], const kg_datetime_t d);

f64 kg_math_pow(f64 base, f64 exponent);

#ifdef KG_IMPL

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>

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
    static const isize small_size = 256;
    if (size <= small_size) {
        u8 temp[size];
        kg_mem_copy(temp, a, size);
        kg_mem_copy(a, b, size);
        kg_mem_copy(b, temp, size);
    } else {
        u8* temp = kg_mem_alloc(size);
        if (temp) {
            kg_mem_copy(temp, a, size);
            kg_mem_copy(a, b, size);
            kg_mem_copy(b, temp, size);
            kg_mem_free(temp);
        }
    }
}
kg_inline void* kg_mem_move(void* dest, const void* src, isize size) {
    return memmove(dest, src, size);
}

void* kg_allocator_default_alloc(kg_allocator_t* a, isize size) {
    kg_cast(void)a;
    return kg_mem_alloc(size);
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

b32 kg_arena_create(kg_arena_t* a, kg_allocator_t* allocator, isize max_size) {
    b32 out_ok = false;
    kg_arena_t arena = {
        .allocator      = allocator,
        .real_ptr       = allocator->proc.alloc(allocator, max_size),
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
            out = a->allocator->proc.alloc(a->allocator, size);
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
        a->allocator->proc.free(a->allocator, a->real_ptr, kg_arena_mem_size(a));
        *a = (kg_arena_t){0};
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
    kg_cast(void)ptr;
    kg_cast(void)old_size;
    void* out_mem = null;
    kg_arena_t* arena = kg_cast(kg_arena_t*)a->context;
    out_mem = kg_arena_alloc(arena, new_size);
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
        while(compare_fn(casted_src + (i * stride), p, stride) < 0) {i++;}
        while(compare_fn(casted_src + (j * stride), p, stride) > 0) {j--;}
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
kg_string_t kg_string_from_cstr(kg_allocator_t* a, const char* cstr) {
    return kg_string_from_cstr_n(a, cstr, strnlen(cstr, ISIZE_MAX));
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
kg_string_t kg_string_append(kg_string_t s, kg_string_t other) {
    kg_string_t out_string = null;
    if (s && other) {
        kg_string_header_t* s_h = kg_string_header(s);
        kg_string_header_t* other_h = kg_string_header(other);
        out_string = kg_string_ensure_available(s, other_h->len);
        if (out_string) {
            s_h->len += other_h->len;
            kg_mem_copy(out_string, other, other_h->len);
            out_string[s_h->len] = '\0';
        }
    }
    return out_string;
}
kg_string_t kg_string_append_fmt(kg_string_t s, const char* fmt, ...) {
    kg_string_t out_string = s;
    if (fmt && s) {
        va_list args;
        va_start(args, fmt);
        out_string = kg_string_append_fmt_v(out_string, fmt, args);
        va_end(args);
    }
    return out_string;
}
kg_string_t kg_string_append_fmt_v(kg_string_t s, const char* fmt, va_list args) {
    kg_string_t out_string = s;
    if (fmt && s) {
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
kg_string_t kg_string_append_cstr(kg_string_t s, const char* cstr) {
    return kg_string_append_cstr_n(s, cstr, strnlen(cstr, ISIZE_MAX));
}
kg_string_t kg_string_append_cstr_n(kg_string_t s, const char* cstr, isize cstr_len) {
    kg_string_t out_string = null;
    if (s && cstr && cstr_len > 0) {
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
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        isize old_mem_size = kg_string_mem_size(s);
        isize new_mem_size = old_mem_size + n + 1;
        kg_string_header_t* new_h = kg_cast(kg_string_header_t*)h->allocator->proc.resize(h->allocator, h, old_mem_size, new_mem_size);
        if (new_h) {
            new_h->cap += n;
            out_string = kg_cast(kg_string_t)(new_h + 1);
        }
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
        out_ok = kg_cast(isize)strnlen(s, h->len + 1) == h->len;
    }
    return out_ok;
}
kg_inline b32 kg_string_is_equal(const kg_string_t s, const kg_string_t other) {
    b32 out_ok = false;
    if (s && other) {
        kg_str_t s_str = (kg_str_t){.len = kg_string_len(s), .ptr = s};
        kg_str_t other_str = (kg_str_t){.len = kg_string_len(other), .ptr = other};
        out_ok = kg_str_is_equal(s_str, other_str);
    }
    return out_ok;
}
kg_inline b32 kg_string_is_equal_cstr(const kg_string_t s, const char* cstr) {
    b32 out_ok = false;
    if (s && cstr) {
        kg_str_t s_str = (kg_str_t){.len = kg_string_len(s), .ptr = s};
        kg_str_t cstr_str = kg_str_create(cstr);
        out_ok = kg_str_is_equal(s_str, cstr_str);
    }
    return out_ok;
}
kg_inline b32 kg_string_is_empty(const kg_string_t s) {
    return kg_string_len(s) == 0;
}
void kg_string_destroy(kg_string_t s) {
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        isize mem_size = kg_string_mem_size(s);
        h->allocator->proc.free(h->allocator, h, mem_size);
    }
}

kg_inline kg_str_t kg_str_create(const char* cstr) {
    kg_str_t out_str = (kg_str_t){0};
    if (cstr) {
        out_str.len = strnlen(cstr, ISIZE_MAX);
        out_str.ptr = cstr;
    }
    return out_str;
}
kg_str_t kg_str_create_n(const char* cstr, isize len) {
    kg_str_t out_str = (kg_str_t){0};
    if (cstr && len > 0) {
        out_str.len = len;
        out_str.ptr = cstr;
    }
    return out_str;
}
kg_str_t kg_str_from_string(const kg_string_t s) {
    return (kg_str_t){.len = kg_string_len(s), .ptr = s};
}
kg_str_t kg_str_from_string_n(const kg_string_t s, isize len) {
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
kg_str_t kg_str_substr_from(const kg_str_t s, isize start_inc) {
    return kg_str_substr(s, start_inc, s.len);
}
kg_str_t kg_str_substr_to(const kg_str_t s, isize end_exc) {
    return kg_str_substr(s, 0, end_exc);
}
kg_str_t kg_str_trim_space(const kg_str_t s) {
    kg_str_t out = s;
    if (out.ptr && out.len > 0) {
        while(out.ptr) {
        }
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
        out_ok = kg_cast(isize)strnlen(s.ptr, s.len) == s.len;
    }
    return out_ok;
}
kg_inline b32 kg_str_is_equal(const kg_str_t s, const kg_str_t other) {
    b32 out_ok = false;
    if (s.len == other.len) {
        out_ok = strncmp(s.ptr, other.ptr, s.len) == 0;
    } 
    return out_ok;
}
kg_inline b32 kg_str_contains(const kg_str_t s, const kg_str_t needle) {
    return kg_str_index(s, needle) >= 0;
}
kg_inline b32 kg_str_has_prefix(const kg_str_t s, const kg_str_t prefix) {
    b32 out_ok = false;
    if (s.len >= prefix.len) {
        out_ok = strncmp(s.ptr, prefix.ptr, prefix.len) == 0;
    }
    return out_ok;
}
kg_inline b32 kg_str_has_suffix(const kg_str_t s, const kg_str_t suffix) {
    b32 out_ok = false;
    if (s.len >= suffix.len) {
        out_ok = strncmp(s.ptr + s.len - suffix.len, suffix.ptr, suffix.len) == 0;
    }
    return out_ok;
}
isize kg_str_index(const kg_str_t s, const kg_str_t needle) {
    isize out_index = -1;
    if (s.len >= needle.len) {
        for (isize i = 0; i < s.len - needle.len + 1; i++) {
            if (strncmp(s.ptr + i, needle.ptr, needle.len) == 0) {
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

#define KG_VALID_BOOL_STRS_MAP_LEN 5
static struct {kg_str_t str; b32 bool;} KG_VALID_BOOL_STRS_MAP[KG_VALID_BOOL_STRS_MAP_LEN] = {
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
isize kg_b32_to_cstr(char buf[B32_MAX_CHARS_LEN], b32 b) {
    isize len = sprintf(buf, "%s", b ? "true" : "false");
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
isize kg_u64_to_cstr(char buf[U64_MAX_CHARS_LEN], u64 u) {
    isize len = sprintf(buf, "%lu", u);
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
kg_string_t kg_u64_to_string(kg_allocator_t* a, u64 u) {
    kg_string_t out = null;
    char buf[U64_MAX_CHARS_LEN] = {0};
    if (kg_u64_to_cstr(buf, u) > 0) {
        out = kg_string_from_cstr(a, buf);
    }
    return out;
}
isize kg_i64_to_cstr(char buf[I64_MAX_CHARS_LEN], i64 i) {
    isize len = sprintf(buf, "%li", i);
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
kg_string_t kg_i64_to_string(kg_allocator_t* a, i64 i) {
    kg_string_t out = null;
    char buf[I64_MAX_CHARS_LEN] = {0};
    if (kg_i64_to_cstr(buf, i) > 0) {
        out = kg_string_from_cstr(a, buf);
    }
    return out;
}
isize kg_f64_to_cstr(char buf[F64_MAX_CHARS_LEN], f64 f) {
    isize len = sprintf(buf, "%lf", f);
    if (len > 0) {
        buf[len] = '\0';
    }
    return len;
}
kg_string_t kg_f64_to_string(kg_allocator_t* a, f64 f) {
    kg_string_t out = null;
    char buf[F64_MAX_CHARS_LEN] = {0};
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

b32 kg_string_builder_create(kg_string_builder_t* b, kg_allocator_t* a, isize cap) {
    b32 out_ok = false;
    *b = (kg_string_builder_t){
        .allocator = a,
        .cap       = cap,
        .real_ptr  = kg_allocator_alloc(a, cap),
    };
    if (b->real_ptr) {
        out_ok = true;
    }
    return out_ok;
}
kg_inline b32 kg_string_builder_write_unsafe(kg_string_builder_t* b, const char* cstr, isize n) {
    b32 out_ok = false;
    if (kg_string_builder_ensure_available(b, n)) {
        kg_mem_copy(b->write_ptr, cstr, n);
        b->len += n;
        b->write_ptr += n;
        out_ok = true;
    }
    return out_ok;
}
kg_inline b32 kg_string_builder_write_u64(kg_string_builder_t* b, u64 u) {
    b32 out_ok = false;
    char buf[U64_MAX_CHARS_LEN] = {0};
    isize len = kg_u64_to_cstr(buf, u);
    out_ok = kg_string_builder_write_unsafe(b, buf, len);
    return out_ok;
}
kg_inline b32 kg_string_builder_write_i64(kg_string_builder_t* b, i64 i) {
    b32 out_ok = false;
    char buf[I64_MAX_CHARS_LEN] = {0};
    isize len = kg_i64_to_cstr(buf, i);
    out_ok = kg_string_builder_write_unsafe(b, buf, len);
    return out_ok;
}
kg_inline b32 kg_string_builder_write_f64(kg_string_builder_t* b, f64 f) {
    b32 out_ok = false;
    char buf[F64_MAX_CHARS_LEN] = {0};
    isize len = kg_f64_to_cstr(buf, f);
    out_ok = kg_string_builder_write_unsafe(b, buf, len);
    return out_ok;
}
kg_inline b32 kg_string_builder_write_char(kg_string_builder_t* b, char c) {
    b32 out_ok = false;
    if (kg_string_builder_ensure_available(b, 1)) {
        *b->write_ptr = c;
        b->len++;
        out_ok = true;
    }
    return out_ok;
}
kg_inline b32 kg_string_builder_write_rune(kg_string_builder_t* b, rune r) {
    kg_cast(void)b;
    kg_cast(void)r;
    kg_panic("not implemented");
    return true;
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
    if (fmt && b) {
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
    isize len = kg_cast(isize)strnlen(c, ISIZE_MAX);
    if (len > 0) {
        out_ok = kg_string_builder_write_unsafe(b, c, len);
    }
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
kg_inline b32 kg_string_builder_cap(const kg_string_builder_t* b) {
    return b ? b->cap : 0;
}
kg_inline b32 kg_string_builder_len(const kg_string_builder_t* b) {
    return b ? b->len : 0;
}
kg_inline b32 kg_string_builder_available(const kg_string_builder_t* b) {
    b32 out = 0;
    if (b && b->cap < b->len) {
        out = b->cap - b->len;
    }
    return out;
}
kg_inline b32 kg_string_builder_ensure_available(kg_string_builder_t* b, isize n) {
    b32 out_ok = true;
    if (kg_string_builder_available(b) < n) {
        isize new_cap = kg_string_builder_grow_formula(b, n);
        out_ok = kg_string_builder_grow(b, new_cap);
    }
    return out_ok;
}

b32 kg_string_builder_reset(kg_string_builder_t* b) {
    b32 out_ok = false;
    if (b) {
        b->write_ptr = b->real_ptr;
        b->len = 0;
        out_ok = true;
    }
    return out_ok;
}
kg_string_t kg_string_builder_to_string(const kg_string_builder_t* b, kg_allocator_t* a) {
    return kg_string_from_cstr_n(a, b->real_ptr, b->len);
}
kg_inline isize kg_string_builder_grow_formula(const kg_string_builder_t* b, isize n) {
    return b->cap * 2 + n;
}
kg_inline isize kg_string_builder_mem_size(const kg_string_builder_t* b) {
    return b ? b->cap : 0;
}
void kg_string_builder_destroy(kg_string_builder_t* b) {
    if (b) {
        kg_allocator_free(b->allocator, b->real_ptr, kg_string_builder_mem_size(b));
    }
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
    if (kg_queue_ensure_available(q, 1)) {
        if (kg_mem_copy(kg_cast(u8*)q->real_ptr + (q->len * q->stride), o, q->stride) != null) {
            out_ok = true;
            q->len++;
        }
    }
    return out_ok;
}
b32 kg_queue_deque(kg_queue_t* q, void* o) {
    b32 out_ok = false;
    if (q->len > 0) {
        out_ok = kg_mem_copy(o, q->real_ptr, q->stride) != null;
        if (out_ok) {
            out_ok = kg_mem_move(q->real_ptr, kg_cast(u8*)q->real_ptr + q->stride, (q->len - 1) * q->stride) != null;
            if (out_ok) {
                q->len--;
            }
        } 
    }
    return out_ok;
}
b32 kg_queue_grow(kg_queue_t* q, isize n) {
    b32 out_ok = false;
    isize old_mem_size = kg_queue_mem_size(q);
    isize new_mem_size = kg_queue_mem_size(q) + q->stride * n;
    void* new_real_ptr = kg_allocator_resize(q->allocator, q->real_ptr, old_mem_size, new_mem_size);
    if (new_real_ptr) {
        q->cap += n;
        q->real_ptr = new_real_ptr;
        out_ok = true;
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
    kg_allocator_free(q->allocator, q->real_ptr, kg_queue_mem_size(q));
    *q = (kg_queue_t){0};
}

kg_inline void kg_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);
}

void kg_file_create(kg_file_t* f) {
    *f = (kg_file_t){
        .handle   = 0,
        .is_valid = false,
    };
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
kg_file_content_t kg_file_read_content(kg_allocator_t* a, const char* filename) {
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
            out_content.cstr = kg_allocator_alloc_array(a, char, size + 1);
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

void kg_time_sleep(const kg_duration_t d) {
    i64 milliseconds = kg_duration_to_milliseconds(d);
    struct timespec ts = {
        .tv_sec  = milliseconds / 1000,
        .tv_nsec = milliseconds % 1000,
    };
    nanosleep(&ts, null);
}

kg_time_t kg_time_now(void) {
    struct timeval tv;
    gettimeofday(&tv, null);
    kg_time_t out = {
        .milliseconds = kg_cast(i64)(tv.tv_sec * 1000 + tv.tv_usec / 1000),
    };
    return out;
}
kg_inline kg_time_t kg_time_add(const kg_time_t t, const kg_duration_t d) {
    kg_time_t out_time = (kg_time_t){.milliseconds = t.milliseconds + d.milliseconds};
    return out_time;
}
kg_inline kg_time_t kg_time_sub(const kg_time_t t, const kg_duration_t d) {
    kg_time_t out_time = (kg_time_t){.milliseconds = t.milliseconds - d.milliseconds};
    return out_time;
}
kg_inline b32 kg_time_is_equal(const kg_time_t t, const kg_time_t o) {
    return t.milliseconds == o.milliseconds;
}
kg_inline b32 kg_time_is_after(const kg_time_t t, const kg_time_t o) {
    return t.milliseconds > o.milliseconds;
}
kg_inline b32 kg_time_is_before(const kg_time_t t, const kg_time_t o) {
    return t.milliseconds < o.milliseconds;
}
kg_inline kg_duration_t kg_time_diff(const kg_time_t t, const kg_time_t other) {
    kg_duration_t out = { 
        .milliseconds = t.milliseconds - other.milliseconds,
    };
    return out;
}
kg_inline kg_date_t kg_time_to_date(const kg_time_t t) {
    i64 seconds = t.milliseconds / 1000;
    struct tm *lt = localtime(&seconds);
    kg_date_t out = {
        .year  = lt->tm_year + 1900,
        .month = lt->tm_mon + 1,
        .day   = lt->tm_mday,
    };
    return out;
}
kg_inline kg_datetime_t kg_time_to_datetime(const kg_time_t t) {
    i64 seconds = t.milliseconds / 1000;
    struct tm *lt = localtime(&seconds);
    kg_datetime_t out = {
        .year    = lt->tm_year + 1900,
        .month   = lt->tm_mon + 1,
        .day     = lt->tm_mday,
        .hour    = lt->tm_hour,
        .minute  = lt->tm_min,
        .second  = lt->tm_sec,
    };
    return out;
}
kg_inline i32 kg_time_to_year(const kg_time_t t) {
    i64 seconds = t.milliseconds / 1000;
    struct tm *lt = localtime(&seconds);
    return lt->tm_year + 1900;
}
kg_inline kg_month_t kg_time_to_month(const kg_time_t t) {
    i64 seconds = t.milliseconds / 1000;
    struct tm *lt = localtime(&seconds);
    return lt->tm_mon + 1;
}
kg_inline i32 kg_time_to_day(const kg_time_t t) {
    i64 seconds = t.milliseconds / 1000;
    struct tm *lt = localtime(&seconds);
    return lt->tm_mday;
}
kg_inline kg_duration_t kg_duration_create(i64 milliseconds) {
    return (kg_duration_t){milliseconds};
}
kg_inline kg_duration_t kg_duration_from_milliseconds(i64 milliseconds) {
    return (kg_duration_t){milliseconds};
}
kg_inline kg_duration_t kg_duration_since(const kg_time_t t) {
    kg_duration_t out = { 
        .milliseconds = kg_time_now().milliseconds - t.milliseconds,
    };
    return out;
}
kg_inline i64 kg_duration_to_milliseconds(const kg_duration_t d) {
    return d.milliseconds;
}
kg_string_t kg_date_to_string(kg_allocator_t* a, const kg_date_t d) {
    kg_string_t out = null;
    char buf[DATE_MAX_CHARS_LEN] = {0};
    if (kg_date_to_cstr(buf, d) > 0) {
        out = kg_string_from_cstr(a, buf);
    }
    return out;
}
isize kg_date_to_cstr(char b[DATE_MAX_CHARS_LEN], const kg_date_t d) {
    i32 len = sprintf(b, "%d-%02d-%02d", d.year, d.month, d.day);
    if (len > 0) {
        b[len] = '\0';
    }
    return len;
}
kg_string_t kg_datetime_to_string(kg_allocator_t* a, const kg_datetime_t d) {
    kg_string_t out = null;
    char buf[DATETIME_MAX_CHARS_LEN] = {0};
    if (kg_datetime_to_cstr(buf, d) > 0) {
        out = kg_string_from_cstr(a, buf);
    }
    return out;
}
isize kg_datetime_to_cstr(char b[DATETIME_MAX_CHARS_LEN], const kg_datetime_t d) {
    isize len = sprintf(b, "%d-%02d-%02d %02d:%02d:%02d", d.year, d.month, d.day, d.hour, d.minute, d.second);
    if (len > 0) {
        b[len] = '\0';
    }
    return len;
}

kg_inline f64 kg_math_pow(f64 base, f64 exponent) {
    f64 out = 1.0;
    for (isize i = 0; i < exponent; i++) {
        out *= base;
    }
    return out;
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
    return sem_init(&s->unix_handle, 0, 0) == 0;
}
b32 kg_sema_post(kg_sema_t* s, isize count) {
    b32 out_ok = true;
    while(count-- > 0) { sem_post(&s->unix_handle); }
    return out_ok;
}
b32 kg_sema_wait(kg_sema_t* s) {
    b32 out_ok = true;
    int i; do { i = sem_wait(&s->unix_handle); } while(i == -1 && errno == EINTR);
    return out_ok;
}
void kg_sema_destroy(kg_sema_t* s) {
    sem_destroy(&s->unix_handle);
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
    return pthread_mutex_lock(&m->pthread_mutex) == 0;
}
b32 kg_mutex_try_lock(kg_mutex_t* m) {
    return pthread_mutex_trylock(&m->pthread_mutex) == 0;
}
b32 kg_mutex_unlock(kg_mutex_t* m) {
    return pthread_mutex_unlock(&m->pthread_mutex) == 0;
}
void kg_mutex_destroy(kg_mutex_t* m) {
    pthread_mutex_destroy(&m->pthread_mutex);
}

b32 kg_cond_create(kg_cond_t* c) {
    b32 out_ok = true;
    pthread_condattr_t attr;
    out_ok = pthread_condattr_init(&attr) == 0;
    if (out_ok) {
        out_ok = pthread_cond_init(&c->pthread_cond, &attr) == 0;
        if (out_ok) {
            out_ok = pthread_condattr_destroy(&attr);
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
    pthread_cond_destroy(&c->pthread_cond);
}

b32 kg_thread_create(kg_thread_t* t, kg_thread_fn_t fn, void* arg) {
    b32 out_ok = false;
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
    if (t->is_running) {
        out_ok = pthread_join(t->posix_handle, null) == 0;
        t->posix_handle = 0;
        t->is_running = false;
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
    if (t->is_running) {
        kg_thread_join(t);
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
    static isize initial_queue_cap = 16;
    b32 out_ok = false;
    *p = (kg_pool_t){
        .allocator = a,
        .workers_n = n,
    };
    p->workers = kg_allocator_alloc_array(p->allocator, kg_thread_t, p->workers_n);
    if (p->workers) {
        kg_mutex_create(&p->work_mutex);
        kg_cond_create(&p->work_cond);
        kg_cond_create(&p->working_cond);
        kg_queue_create(&p->task_queue, p->allocator, kg_sizeof(kg_task_t), initial_queue_cap);
        for (isize i = 0; i < p->workers_n; i++) {
            kg_thread_create(&p->workers[i], kg_pool_loop_, p);
        }
        out_ok = true;
    }
    return out_ok;
}
b32 kg_pool_add_task(kg_pool_t* p, kg_thread_fn_t fn, void* arg) {
    b32 out_ok = true;
    kg_mutex_lock(&p->work_mutex);
    kg_task_t task = (kg_task_t){
        .fn = fn,
        .arg = arg,
    };
    out_ok = kg_queue_enqueue(&p->task_queue, &task);
    kg_cond_signal(&p->work_cond);
    kg_mutex_unlock(&p->work_mutex);
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
    kg_allocator_free(p->allocator, p->workers, kg_sizeof(kg_thread_t) * p->workers_n);
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

static kg_flag_t kg_flags[KG_FLAGS_MAX_LEN];
static isize     kg_flags_len = 0;

const char* kg_flag_kind_to_cstr(kg_flag_kind_t k) {
    static const char* kind_cstrs[] = {
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
                        kg_panic("KG_FLAGS: unknown flag kind");
                }
                break;
            }
        }
    }
}
void kg_flags_usage(void) {
    for (isize i = 0; i < kg_flags_len; i++) {
        kg_flag_t f = kg_flags[i];
        kg_printf("--%s\n\tkind: %s\n\tusage: %s\n", f.name, kg_flag_kind_to_cstr(f.kind), f.usage);
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

typedef struct kg_logger_internals_t {
#ifdef KG_THREADS
    kg_mutex_t mutex;
#endif
    isize _;
} kg_logger_internals_t;

void kg_logger_create(void);
void kg_log_handler  (kg_log_level_t level, const char* file, i64 line, const char* fmt, ...);

#if KG_LOG_NONE != 1 && KG_LOG_RAW == 1
    #define kg_log(fmt, ...) kg_log_handler(KG_LOG_LEVEL_RAW, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_TRACE == 1
    #define kg_log_trace(fmt, ...) kg_log_handler(KG_LOG_LEVEL_TRACE, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_trace(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_DEBUG == 1
    #define kg_log_debug(fmt, ...) kg_log_handler(KG_LOG_LEVEL_DEBUG, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_debug(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_INFO == 1
    #define kg_log_info(fmt, ...) kg_log_handler(KG_LOG_LEVEL_INFO, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_info(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_WARN == 1
    #define kg_log_warn(fmt, ...) kg_log_handler(KG_LOG_LEVEL_WARN, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_warn(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_ERROR == 1
    #define kg_log_error(fmt, ...) kg_log_handler(KG_LOG_LEVEL_ERROR, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_error(fmt, ...)
#endif
#if KG_LOG_NONE != 1 && KG_LOG_FATAL == 1
    #define kg_log_fatal(fmt, ...) kg_log_handler(KG_LOG_LEVEL_FATAL, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#else   
    #define kg_log_fatal(fmt, ...)
#endif

#ifdef KG_LOGGER_IMPL

#ifdef KG_THREADS
static kg_logger_internals_t kg_logger_internals;
#endif
void kg_logger_create(void) {
#ifdef KG_THREADS
    kg_mutex_create(&kg_logger_internals.mutex);
#endif
}
void kg_log_handler(kg_log_level_t level, const char* file, i64 line, const char* fmt, ...) {
#ifdef KG_THREADS
    kg_mutex_lock(&kg_logger_internals.mutex);
#endif
    kg_cast(void)file;
    kg_cast(void)line;
    static const char* level_cstrs[KG_LOG_LEVEL__SENTINEL] = {
        [KG_LOG_LEVEL_RAW]   = "",
        [KG_LOG_LEVEL_TRACE] = "[trace]",
        [KG_LOG_LEVEL_DEBUG] = "[debug]",
        [KG_LOG_LEVEL_INFO]  = "[info ]",
        [KG_LOG_LEVEL_WARN]  = "[warn ]",
        [KG_LOG_LEVEL_ERROR] = "[error]",
        [KG_LOG_LEVEL_FATAL] = "[fatal]",
    };
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_builder_t sb;
    if (kg_string_builder_create(&sb, &allocator, 128)) {
        if (level != KG_LOG_LEVEL_RAW) {
            kg_time_t time = kg_time_now();
            kg_datetime_t datetime = kg_time_to_datetime(time);
            char datetime_cstr[DATETIME_MAX_CHARS_LEN] = {0};
            if (kg_datetime_to_cstr(datetime_cstr, datetime) > 0) {
                kg_string_builder_write_fmt(&sb, "%s %s ", datetime_cstr, level_cstrs[level]);
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
    kg_mutex_unlock(&kg_logger_internals.mutex);
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

void kgt_expect_handler(const char* cond, const char* msg, const char* file, isize line);

#define kgt_expect(cond, msg)          if (cond) {} else { kgt_expect_handler(#cond, msg, __FILE__, kg_cast(isize)__LINE__); }
#define kgt_expect_null(a)             kgt_expect(a == null, "expected null")
#define kgt_expect_not_null(a)         kgt_expect(a != null, "expected not null")
#define kgt_expect_eq(a, b)            kgt_expect(a == b, "expected eq")
#define kgt_expect_neq(a, b)           kgt_expect(a != b, "expected neq")
#define kgt_expect_ptr_eq(a, b)        kgt_expect(a == b, "expected ptr eq")
#define kgt_expect_ptr_neq(a, b)       kgt_expect(a != b, "expected ptr neq")
#define kgt_expect_true(a)             kgt_expect(a == true, "expected true")
#define kgt_expect_false(a)            kgt_expect(a == false, "expected false")
#define kgt_expect_mem_eq(a, b, s)     kgt_expect(kg_mem_compare(a, b, s) == 0, "expected memory eq")
#define kgt_expect_mem_neq(a, c, s)    kgt_expect(kg_mem_compare(a, b, s) != 0, "expected memory neq")
#define kgt_expect_cstr_eq(a, b)       kgt_expect(strcmp(a, b) == 0, "expected cstr eq")
#define kgt_expect_cstr_neq(a, c)      kgt_expect(strcmp(a, b) != 0, "expected cstr neq")
#define kgt_expect_cstr_n_eq(a, b, n)  kgt_expect(strncmp(a, b, n) == 0, "expected cstr n eq")
#define kgt_expect_cstr_n_neq(a, c, n) kgt_expect(strncmp(a, b, n) != 0, "expected cstr n neq")

#endif // KG_TESTER

#ifdef KG_TESTER_IMPL

void kgt_expect_handler(const char* cond, const char* msg, const char* file, isize line) {
    kg_printf("Test failed: (%s:%li) %s %s\n", file, line, msg, cond);
    /*kg_exit(1);*/
}

void kgt_create(kgt_t* t) {
    *t = (kgt_t){0};
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
    *t = (kgt_t){0};
}

#endif // KG_TESTER_IMPL
