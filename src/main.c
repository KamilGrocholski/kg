#define KG_IMPL
#define KG_THREADS
#define KG_THREADS_IMPL
#include "kg.h"

typedef struct st {
    kg_mutex_t mu;
    isize      c;
} st;

void some_work(void* arg) {
    st* s = kg_cast(st*)arg;
    kg_log_info("%li", s->c);
}

int main() {
    kg_allocator_t allocator = kg_allocator_default();

    st s = {
        .c = 20
    };
    kg_mutex_create(&s.mu);

    kg_pool_t p;
    kg_pool_create(&p, &allocator, 4);

    for (isize i = 0; i < 140; i++) {
        kg_pool_add_task(&p, some_work, &s);
    }

    kg_pool_run(&p);

    kg_pool_wait(&p);

    kg_mutex_destroy(&s.mu);

    kg_pool_destroy(&p);

    return 0;
}
