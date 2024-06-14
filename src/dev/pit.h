#pragma once

#define PIT_DAT 0x40
#define PIT_CMD 0x43
#define PIT_FREQ 1000

void pit_init();
void pit_sleep(u64 ms);