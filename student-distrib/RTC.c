#include "RTC.h"
#include "types.h"
#include "lib.h"
#include "i8259.h"

#define RTC_itr_num 8
#define REGISTER_B 0x8b
#define INDEX_NUM 0x70
#define BITSIX 0x40
#define READandWRITE 0x71
#define VALUE 0x7F
#define LOWRANGE 0x0C

/* extern void RTC_init();
 * Inputs: void
 * Return Value: void
 * Function: initializes RTC by turning on the IRQ with the default 1024 Hz rate */

extern void RTC_init(){
// You will have to enable interrupt generating mode, and set the init frequency
// You will have to select registers (CMD port), send data to registers with data port
    
    cli();//disable_irq(8);
    outb(REGISTER_B,INDEX_NUM);
    char prev = inb(READandWRITE);
    outb(REGISTER_B,INDEX_NUM);
    outb( prev | BITSIX, READandWRITE);
    outb(inb(INDEX_NUM) & VALUE ,INDEX_NUM);
    inb(READandWRITE);
    sti();
    enable_irq(RTC_itr_num);
    // 
    // enable_irq(RTC_itr_num);
    return;
}

/* extern void RTC_open();
 * Inputs: void
 * Return Value: void
 * Function: should reset the frequency to 2Hz
 */
int32_t void RTC_open(){

return 0;
}

/* extern void RTC_read();
 * Inputs: void
 * Return Value: void
 * Function: Make sure that rtc read must only return once the RTC interrupt occurs. 
   You might want to use some sort of flag here (you will not need spinlocks. Why?)
 */
extern void RTC_read(){

}

/* extern void RTC_write();
 * Inputs: void
 * Return Value: void
 * Function: must get its input parameter through a buffer 
             and not read the value directly.
 */
extern void RTC_write(){

}

/* extern void RTC_close();
 * Inputs: void
 * Return Value: void
 * Function: must get its input parameter through a buffer 
             and not read the value directly.
 */
extern void RTC_close(){

}

/* extern void RTC_handle();
 * Inputs: void
 * Return Value: void
 * Function: represents the RTC handler which handles what heppens when there is an interrupt 
 */
extern void RTC_handle(){
// Basic for now, likely will have to come back to later
// Read contents of Reg C - RTC will not generate another interrupt if this is not done
// Send EOI - PIC will not handle another interrupt until then
// test_interrupts();
    outb(LOWRANGE,INDEX_NUM);
    // outb(inb(0x70) & 0x7F ,0x70);
    inb(READandWRITE);
    // RTC test to see frequency of clock 
        // printf("Hisdjihdfihdjfhujduhn");
        // test_interrupts();
    send_eoi(RTC_itr_num);
    return;
}
