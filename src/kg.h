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

#define true    1
#define false   0
#define null    NULL

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

#if defined(KG_PLATFORM_LINUX)
    #include <sys/stat.h>
#endif

kg_extern void kg_printf(const char* fmt, ...);

kg_extern void kg_assert_handler(const char* prefix, const char* condition, const char* file, isize line, const char* fmt, ...);

#define kg_assert_msg(cond, fmt, ...) if (cond) {} else {kg_assert_handler("Assertion failed", #cond, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__);}
#define kg_assert(cond)               kg_assert_msg(cond, null)
#define kg_panic(fmt, ...)            kg_assert_handler("Panic", null, __FILE__, kg_cast(isize)__LINE__, fmt, ##__VA_ARGS__)

typedef enum kg_log_level_t {
    KG_LOG_LEVEL_TRACE = 0,
    KG_LOG_LEVEL_DEBUG = 1,
    KG_LOG_LEVEL_INFO  = 2,
    KG_LOG_LEVEL_WARN  = 3,
    KG_LOG_LEVEL_ERROR = 4,
    KG_LOG_LEVEL_FATAL = 5,
    KG_LOG_LEVEL__SENTINEL,
} kg_log_level_t;

kg_extern void kg_log_handler(kg_log_level_t level, const char* file, i64 line, const char* fmt, ...);

#define kg_log_trace(fmt, ...) kg_log_handler(KG_LOG_LEVEL_TRACE, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#define kg_log_debug(fmt, ...) kg_log_handler(KG_LOG_LEVEL_DEBUG, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#define kg_log_info(fmt, ...)  kg_log_handler(KG_LOG_LEVEL_INFO,  __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#define kg_log_warn(fmt, ...)  kg_log_handler(KG_LOG_LEVEL_WARN,  __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#define kg_log_error(fmt, ...) kg_log_handler(KG_LOG_LEVEL_ERROR, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)
#define kg_log_fatal(fmt, ...) kg_log_handler(KG_LOG_LEVEL_FATAL, __FILE__, kg_cast(i64)__LINE__, fmt, ##__VA_ARGS__)

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

kg_extern kg_allocator_t kg_allocator_default(void);

#define kg_allocator_alloc_one(a, T)                    (T*)((a)->proc.alloc(a, kg_sizeof(T)))
#define kg_allocator_alloc_array(a, T, n)               (T*)((a)->proc.alloc(a, kg_sizeof(T) * n))
#define kg_allocator_alloc(a, size)                     (a)->proc.alloc(a, size)
#define kg_allocator_free(a, ptr, size)                 (a)->proc.free(a, ptr, size)
#define kg_allocator_free_all(a, size)                  (a)->proc.free_all(a, size)
#define kg_allocator_resize(a, ptr, old_size, new_size) (a)->proc.resize(a, ptr, old_size, new_size)

typedef void* (*kg_allocator_allocate_fn_t)(kg_allocator_t* a, isize size);
typedef void  (*kg_allocator_free_fn_t)    (kg_allocator_t* a, void* ptr, isize size);
typedef void  (*kg_allocator_free_all_fn_t)(kg_allocator_t* a, isize size);
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

typedef struct kg_arena_t {
    isize len;
} kg_arena_t;

typedef struct kg_string_header_t {
    isize           len;
    isize           cap;
    char*           cstr;
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
isize       kg_string_len             (kg_string_t s);
isize       kg_string_cap             (kg_string_t s);
isize       kg_string_available       (kg_string_t s);
isize       kg_string_mem_size        (kg_string_t s);
b32         kg_string_is_valid        (kg_string_t s);
b32         kg_string_is_equal        (kg_string_t s, kg_string_t other);
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
#define kg_darray_destroy(d)             do { kg_darray_header_t* h = kg_darray_header(d); kg_allocator_free(h->allocator, h, kg_darray_mem_size(d)); } while(0)

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

#ifdef KG_IMPL

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

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
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
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
void kg_allocator_default_free_all(kg_allocator_t* a, isize size) {
    kg_cast(void)a;
    kg_cast(void)size;
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
isize kg_string_len(kg_string_t s) {
    isize out_len = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_len = h->len;
    }
    return out_len;
}
isize kg_string_cap(kg_string_t s) {
    isize out_cap = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_cap = h->cap;
    }
    return out_cap;
}
isize kg_string_available(kg_string_t s) {
    isize out_available = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        if (h->cap >= h->len) {
            out_available = h->cap - h->len;
        }
    }
    return out_available;
}
kg_string_t kg_string_ensure_available(kg_string_t s, isize n) {
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
isize kg_string_mem_size(kg_string_t s) {
    isize out_mem_size = 0;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_mem_size = kg_sizeof(kg_string_header_t) + h->cap + 1;
    }
    return out_mem_size;
}
b32 kg_string_is_valid(kg_string_t s) {
    b32 out_ok = true;
    if (s) {
        kg_string_header_t* h = kg_string_header(s);
        out_ok = kg_cast(isize)strnlen(s, h->len + 1) == h->len;
    }
    return out_ok;
}
b32 kg_string_is_equal(kg_string_t s, kg_string_t other) {
    b32 out_ok = false;
    if (s && other) {
        kg_str_t s_str = (kg_str_t){.len = kg_string_len(s), .ptr = s};
        kg_str_t other_str = (kg_str_t){.len = kg_string_len(other), .ptr = other};
        out_ok = kg_str_is_equal(s_str, other_str);
    }
    return out_ok;
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
    }
    return out_str;
}
kg_inline kg_str_t kg_str_substr(const kg_str_t s, isize start_inc, isize end_exc) {
    return (kg_str_t){.len = end_exc - start_inc, .ptr = s.ptr + start_inc};
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
        out_ok = strnlen(s.ptr, s.len);
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

kg_inline void kg_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
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
                kg_mem_zero(out_content.cstr, size);
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

void kg_assert_handler(const char* prefix, const char* condition, const char* file, isize line, const char* fmt, ...) {
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_fmt(&allocator, "%s: (%s:%li) %s ", prefix, file, line, condition);
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
    exit(1);
}

void kg_log_handler(kg_log_level_t level, const char* file, i64 line, const char* fmt, ...) {
    static const char* level_cstrs[KG_LOG_LEVEL__SENTINEL] = {
        [KG_LOG_LEVEL_TRACE] = "[TRACE]: ",
        [KG_LOG_LEVEL_DEBUG] = "[DEBUG]: ",
        [KG_LOG_LEVEL_INFO]  = "[INFO]: ",
        [KG_LOG_LEVEL_WARN]  = "[WARN]: ",
        [KG_LOG_LEVEL_ERROR] = "[ERROR]: ",
        [KG_LOG_LEVEL_FATAL] = "[FATAL]: ",
    };
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t string = kg_string_from_fmt(&allocator, "%s(%s:%li) ", level_cstrs[level], file, line);
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
    if (level == KG_LOG_LEVEL_FATAL) {
        exit(1);
    }
}

#endif // KG_IMPL
