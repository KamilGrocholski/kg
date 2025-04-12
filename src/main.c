#define KG_IMPL
/*#define KG_THREADS*/
/*#define KG_THREADS_IMPL*/
#define KG_LOGGER
#define KG_LOGGER_IMPL
#define KG_FORMATTER
#define KG_FORMATTER_IMPL
#include "kg.h"

i32 kg_fmt_str(const char* fmt, va_list arg) {
    kg_cast(void)fmt;
    isize len = va_arg(arg, isize);
    const char* ptr = va_arg(arg, const char*);
    for (isize i = 0; i < len; i++) {
        putchar(ptr[i]);
    }
    return 0;
}

i32 kg_fmt_bool(const char* fmt, va_list arg) {
    kg_cast(void)fmt;
    b32 bool = va_arg(arg, b32);
    const char* lit = bool ? "true" : "false";
    while(*lit) {
        putchar(*lit++);
    }
    return 0;
}

i32 kg_fmt_cstr(const char* fmt, va_list arg) {
    kg_cast(void)fmt;
    const char* cstr = va_arg(arg, const char*);
    while(*cstr) {
        putchar(*cstr++);
    }
    return 0;
}

i32 kg_fmt_i64(const char* fmt, va_list arg) {
    kg_cast(void)fmt;
    u64 i = va_arg(arg, u64);
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t s = kg_i64_to_string(&allocator, i);
    for(isize i = 0; i < kg_string_len(s); i++) {
        putchar(s[i]);
    }
    return 0;
}

i32 kg_fmt_u64(const char* fmt, va_list arg) {
    kg_cast(void)fmt;
    u64 u = va_arg(arg, u64);
    kg_allocator_t allocator = kg_allocator_default();
    kg_string_t s = kg_u64_to_string(&allocator, u);
    for(isize i = 0; i < kg_string_len(s); i++) {
        putchar(s[i]);
    }
    return 0;
}

i32 kg_fmt_char(const char* fmt, va_list arg) {
    kg_cast(void)fmt;
    char c = va_arg(arg, int);
    putchar(c);
    return 0;
}

i32 kg_fmt_rune(const char* fmt, va_list arg) {
    kg_cast(void)fmt;
    rune r = va_arg(arg, rune);
    return 0;
}

i32 main() {
    kg_logger_create();

    kg_log("siema");
    kg_fmt_register("str",  kg_fmt_str);
    kg_fmt_register("bool", kg_fmt_bool);
    kg_fmt_register("cstr", kg_fmt_cstr);
    kg_fmt_register("i64",  kg_fmt_i64);
    kg_fmt_register("u64",  kg_fmt_u64);
    kg_fmt_register("char", kg_fmt_char);
    kg_fmt_register("rune", kg_fmt_rune);

    kg_str_t str = kg_str_create("to jestem ja ze str");

    kg_fmt("siema {rune} {char} {str} {bool} {cstr} {i64} {u64}\n", "ś", 'o', str, true, "okej", -2, 4);

    return 0;
}
