#include "RTC.h"
#include "types.h"
#include "lib.h"
#include "system_call.h"
#include "i8259.h"

#define RTC_itr_num 8
#define REGISTER_A 0x8a
#define REGISTER_B 0x8b
#define REGISTER_C 0x0C
#define INDEX_NUM 0x70
#define BITSIX 0x40
#define READandWRITE 0x71
#define VALUE 0x7F
#define MAX_FREQ 1024
#define MIN_FREQ 2
#define OPEN_RTC_RATE 0x0F
#define RTC_RATE_MASK 0x0F
#define TOP_4BITS 0xF0
#define PASS 1
#define FAIL 0
static int8_t flag[3] = {0,0,0};
static int idx;
static volatile uint16_t Hz_rate[3];
static uint16_t Hz_counter[3];
static uint32_t buffer_rate = 0;

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
    Hz_rate[0] = Hz_rate[1] = Hz_rate[2] = MAX_FREQ;
    Hz_counter[0] = Hz_counter[1] = Hz_counter[2] = MAX_FREQ/Hz_rate[get_main_terminal()];
    sti();
    enable_irq(RTC_itr_num);
    // 
    // enable_irq(RTC_itr_num);
    return;
}


/* extern void RTC_open();
 * Inputs: const uint8_t* filename
 * Return Value: 0
 * Function: should reset the frequency to 2Hz. Input not used
 Works need better test
 */
int32_t RTC_open(const uint8_t* filename){
    idx=get_main_terminal();
    Hz_rate[idx] = MIN_FREQ;
    Hz_counter[idx] = MAX_FREQ/Hz_rate[idx];
    // printf("1");
    return 0;
}

/* extern void RTC_read();
 * Inputs: int32_t fd, void* buf, int32_t nbytes
 * Return Value: 0 if successful
 * Function: Make sure that rtc read must only return once the RTC interrupt 
   occurs. You might want to use some sort of flag here 
   (you will not need spinlocks. Why?) inputs not used
 */
int32_t RTC_read(int32_t fd, void* buf, int32_t nbytes){
    flag[get_main_terminal()] = 0;
    while(!flag[get_main_terminal()]){
    //    printf("flag = 0\n");
    }
    // printf("flag = 0\n");
    return 0;
}

/* extern void RTC_write();
 * Inputs: int32_t fd, const void* buf, int32_t nbytes
 * Return Value: -1 if failed and 0 if successfule
 * Function: must get its input parameter through a buffer 
             and not read the value directly.
 */
int32_t RTC_write(int32_t fd, const void* buf, int32_t nbytes){
    if(buf == NULL){
        // printf("NULL\n");
        return -1;
    }
    buffer_rate =*((int*)buf);
    //printf("buffer_rate = %d\n",buffer_rate);
    //if statement check if buf int is between 0-1024 and power of 2
    // 1024Hz is maximum frequency written in OSDev
    if((buffer_rate > 1024 || !((buffer_rate != 0) && ((buffer_rate & (buffer_rate - 1)) == 0)))){
        return -1;//invalid input
    }else{
        idx=get_main_terminal();
        Hz_rate[idx] = buffer_rate;
        // Hz_counter = MAX_FREQ/Hz_rate;
        return 4;//number of bytes changed
    }
//return 4;
}

/* extern void RTC_close();
 * Inputs: int32_t fd
 * Return Value: 0
 * Function: Don't need to do anything for RTC
 */
int32_t RTC_close(int32_t fd){

return 0;
}

/*
int32_t Changing_RTC_rate(int8_t rate){
    rate &= RTC_RATE_MASK;			// rate must be above 2 and not over 15
    cli();
    outb(REGISTER_A, INDEX_NUM);     // set index to register A, disable NMI
    char prev = inb(READandWRITE);	// get initial value of register A
    outb(REGISTER_A, INDEX_NUM);		// reset index to A
    outb((prev & TOP_4BITS) | rate, READandWRITE)
    outb(inb(INDEX_NUM) & VALUE ,INDEX_NUM);
    inb(READandWRITE);
    sti();
    enable_irq(RTC_itr_num);
    return 0;
}
*/

/* extern void RTC_handle();
 * Inputs: void
 * Return Value: void
 * Function: represents the RTC handler which handles what heppens when there is an interrupt 
 */
extern void RTC_handle(){
// Basic for now, likely will have to come back to later
// Read contents of Reg C - RTC will not generate another interrupt if this is not done
// Send EOI - PIC will not handle another interrupt until then
    outb(REGISTER_C, INDEX_NUM);
    inb(READandWRITE);
    cli();
    for(idx=0;idx<3;idx++){     // 3: we have 3 terminals to control
        Hz_counter[idx]--;
        if(Hz_counter[idx] <= 0){ 
            flag[idx] = 1;
            Hz_counter[idx] = MAX_FREQ/Hz_rate[idx];
        }
    }
    sti();
    send_eoi(RTC_itr_num);
    return;
}

