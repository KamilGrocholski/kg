#define KG_IMPL
#define KG_THREADS
#define KG_THREADS_IMPL
#define KG_LOGGER
#define KG_LOGGER_IMPL
#define KG_FLAGS
#define KG_FLAGS_IMPL
#include "kg.h"

typedef struct task_data_t {
    kg_mutex_t mutex;
    isize      value;
} task_data_t;

void* task_with_mutex(void* arg) {
    task_data_t* d = kg_cast(task_data_t*)arg;
    kg_mutex_lock(&d->mutex);
    (d->value)++;
    kg_log("task_data.value: %li", d->value);
    kg_mutex_unlock(&d->mutex);
    return null;
}

void* task_without_mutex(void* arg) {
    task_data_t* d = kg_cast(task_data_t*)arg;
    (d->value)++;
    kg_log("task_data.value: %li", d->value);
    return null;
}

i32 main(i32 argc, char* argv[]) {
    kg_allocator_t allocator = kg_allocator_default();

    kg_logger_setup();

    kg_string_t time_string = kg_time_now_as_string(&allocator);
    kg_log("%s", time_string);
    kg_string_destroy(time_string);

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
    kg_time_sleep(kg_duration_from_milliseconds(250));
    kg_log_info("b32_flag: %s", b32_flag ? "true" : "false");
    kg_log_info("u64_flag: %lu", u64_flag);
    kg_log_info("i64_flag: %li", i64_flag);

    kg_pool_t p;
    isize workers = 5;
    task_data_t task_data = {.value = 0};
    kg_mutex_create(&task_data.mutex);
    kg_pool_create(&p, &allocator, workers);

    for (isize i = 0; i < workers; i++) {
        kg_pool_add_task(&p, task_with_mutex, &task_data);
    }
    kg_pool_join(&p);
    kg_log("final value with mutex: %li", task_data.value);

    task_data.value = 0;

    for (isize i = 0; i < workers; i++) {
        kg_pool_add_task(&p, task_without_mutex, &task_data);
    }
    kg_pool_join(&p);
    kg_log("final value without mutex: %li", task_data.value);

    kg_pool_destroy(&p);
    kg_mutex_destroy(&task_data.mutex);

    kg_log("RAW");
    kg_log_trace("TRACE");
    kg_log_debug("DEBUG");
    kg_log_info("INFO");
    kg_log_warn("WARN");
    kg_log_error("ERROR");
    kg_log_fatal("FATAL");

    return 0;
}
