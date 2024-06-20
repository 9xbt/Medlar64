#include <mm/kmalloc.h>
#include <sys/sched.h>

u64 sched_tasks = 0;
task_t *idle_task = NULL;
task_t *current_task = NULL;

void sched_idle() {
    for (;;)
        asm volatile ("hlt");
}

task_t* sched_new_task(void* handler) {
    task_t* task = (task_t*)kmalloc(sizeof(task_t));

    task->pid = handler == sched_idle ? 0 : sched_tasks++;
}

void sched_init() {
    sched_new_task(sched_idle);

    printf("sched: scheduler initialized\n");
}