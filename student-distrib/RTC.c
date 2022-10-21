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
#define Base_f 1024

int f_now;
uint32_t flags;

/* extern void RTC_init();
 * Inputs: void
 * Return Value: void
 * Function: initializes RTC by turning on the IRQ with the default 1024 Hz rate */

void RTC_init(){
// You will have to enable interrupt generating mode, and set the init frequency
// You will have to select registers (CMD port), send data to registers with data port
    
    cli();//disable_irq(8);
    outb(REGISTER_B,INDEX_NUM);
    char prev = inb(READandWRITE);
    outb(REGISTER_B,INDEX_NUM);
    outb( prev | BITSIX, READandWRITE);
    outb(inb(INDEX_NUM) & VALUE ,INDEX_NUM);
    inb(READandWRITE);
    f=2;
    sti();
    enable_irq(RTC_itr_num);
    // 
    // enable_irq(RTC_itr_num);
    return;
}

/* extern void RTC_handle();
 * Inputs: void
 * Return Value: void
 * Function: represents the RTC handler which handles what heppens when there is an interrupt */

void RTC_handle(){
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


uint32_t RTC_open(const uint8_t* filename){
    RTC_init();
    return 0;
}
uint32_t RTC_close(int32_t fd){
    // revert virtualizing RTC
    return 0;
}
uint32_t RTC_read(int32_t fd, void* buf, int32_t nbytes){
    while(1);
    return 0;
}
uint32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes){
    int f_new;
    f_new=*((uint32_t*)buf);
    if((f_new&(f_new-1))!=0)return -1;
    return 0;
}
void sleep(int32_t t){
    int i;
    for( i=0;i<Base_f/f_now;i++ ){
        RTC_read(0,NULL,0);
    }
    return 0;
}
