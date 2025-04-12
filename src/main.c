#define KG_IMPL
#define KG_THREADS
#define KG_THREADS_IMPL
#include "kg.h"

typedef struct st {
    kg_mutex_t mu;
    isize      c;
} st;

void* some_work(void* arg) {
    st* s = kg_cast(st*)arg;
    s->c++;
    kg_log_info("threadid=%li, value=%li", kg_thread_id(), s->c);
    kg_time_sleep(1 * KG_MILLISECOND);
    return 0;
}

int main() {
    kg_allocator_t allocator = kg_allocator_default();

    kg_time_t start = kg_time_now();

    kg_pool_t p;
    kg_pool_create(&p, &allocator, 4);

    for (isize i = 0; i < 140; i++) {
        st s = {
            .c = 20
        };
        kg_mutex_create(&s.mu);
        kg_pool_add_task(&p, some_work, &s);
    }

    kg_pool_wait(&p);
    kg_duration_t diff = kg_duration_since(start);
    kg_log_info("time: %lims", kg_duration_to_milliseconds(diff));

    kg_pool_destroy(&p);

    return 0;
}
