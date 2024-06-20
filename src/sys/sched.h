#pragma once

#include <types.h>
#include <sys/idt.h>

typedef struct task {
    regs registers;
    struct task *next;

    u64 pid;
} task_t;

void sched_init();