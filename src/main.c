#define KG_IMPL
#define KG_THREADS
#define KG_THREADS_IMPL
#define KG_LOGGER
#define KG_LOGGER_IMPL
#include "kg.h"

i32 main() {
    kg_logger_create();

    kg_str_t name = kg_str_create("okej");
    isize index = kg_str_index(name, kg_str_create("okej"));
    isize indexc = kg_str_index_char(name, 'e');
    kg_log("str: %li", index);
    kg_log("char:%li", indexc);
    kg_log("v2:%s", "okeja");

    return 0;
}
