#define KG_IMPL
#define KG_THREADS
#define KG_THREADS_IMPL
#define KG_LOGGER
#define KG_LOGGER_IMPL
#define KG_FLAGS
#define KG_FLAGS_IMPL
#include "kg.h"

i32 main(i32 argc, char* argv[]) {
    kg_logger_create();

    b32 help_flag;
    kg_str_t str_flag;
    b32 b32_flag;
    u64 u64_flag;
    i64 i64_flag;

    kg_flag_b32(&help_flag, "help", true, "show flags usage");
    kg_flag_str(&str_flag, "str", kg_str_create("default_str"), "set str");
    kg_flag_b32(&b32_flag, "b32", false, "set b32");
    kg_flag_u64(&u64_flag, "u64", 123, "set u64");
    kg_flag_i64(&i64_flag, "i64", -123, "set i64");

    kg_flags_parse(argc, argv);

    if (help_flag) {
        kg_flags_usage();
        /*kg_exit(0);*/
    }

    kg_log_info("str_flag: %.*s", str_flag.len, str_flag.ptr);
    kg_time_sleep(kg_duration_from_milliseconds(1000));
    kg_log_info("b32_flag: %s", b32_flag ? "true" : "false");
    kg_log_info("u64_flag: %lu", u64_flag);
    kg_log_info("i64_flag: %li", i64_flag);

    return 0;
}
