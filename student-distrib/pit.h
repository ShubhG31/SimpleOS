#include "i8259.h"
#include "lib.h"

#define FAST_HTZ 1193182
#define PIT_port 0x40
#define PIT_irq 0

void pit_initialize();
