#include "RTC.h"
#include "types.h"
#include "lib.h"
#include "i8259.h"

#define RTC_itr_num 8

//REMEMBER MAGIC NUMBERS
extern void RTC_init(){
// You will have to enable interrupt generating mode, and set the init frequency
// You will have to select registers (CMD port), send data to registers with data port

    cli();//disable_irq(8);
    
    outb(0x8b,0x70);
    char prev = inb(0x71);
    outb(0x8b,0x70);
    outb( prev | 0x40, 0x71);

    //sti();
    enable_irq(RTC_itr_num);
    return;
}

extern void RTC_handler(){
// Basic for now, likely will have to come back to later
// Read contents of Reg C - RTC will not generate another interrupt if this is not done
// Send EOI - PIC will not handle another interrupt until then
    outb(0x0C,0x70);
    inb(0x71);
    //test_interrupts();
    send_eoi(RTC_itr_num);
    return;
}