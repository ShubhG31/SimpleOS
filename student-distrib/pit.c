#include "pit.h"


void pit_initialize(){

    cli();

    uint32_t freq = FAST_HTZ / 100000;
    outb(0x36, PIT_port);
    outb(freq&0xFF, PIT_port);
    outb(freq >> 8, PIT_port);
    sti();

    enable_irq(PIT_irq);
    // send_eoi(0);
    return;

}


