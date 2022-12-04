#include "idt.h"
// #include ""

//ALL MAGIC NUMBER LABELS
#define LAST_EXCEPTION 19  // defined as the last software generated exception which is 15  
#define keyboard 0x21
#define entries 256
#define systemCall 0x80
#define dplnum 3
#define RTCint 0x28
#define keyboardInt 0x21
#define PITint 0x20
// ALL MAGIC NUMBER LABELS

// typedef void (*functions);


// function[15] = ["divide_by_zero",
//                   "single_step_int",
//                   "non_maskable_int",
//                   "breakpoint",
//                   "overflow",
//                   "bound_range_exceeds",
//                   "invalid_opcode",
//                   "coprocessor_not_avail",
//                   "double_fault",
//                   ""
//                   ""
//                   ""
// ]

// read IA-32 Intel page 153, 5.10 -- helps with present bit 

// 31_____________16_15_14_13_________8_7_____5_4_________0
// |               |   | D |           |       |          |
// |  offset 31_16 | P | P | 0 D 1 1 0 | 0 0 0 |          |
// |               |   | L |           |       |          |
// --------------------------------------------------------

/* void idt_initialization();
 * Inputs: none
 * Return Value: none just makes sure the idt is initialized
 * Function: initializes the idt with the 256 interrupt vectors making the idt have 256 entries */

void idt_initialization(){
    int i;
    for(i = 0; i<entries; i++){
        // if ( i != 0xF){
        //     idt[i].dpl = 0;
        //     idt[i].present = 1;
        // }
        // else{
        //     idt[i].present = 0;
        // }
        idt[i].reserved0 = 0;   
        idt[i].size = 1 ;       // D bit is set to 1, look above
        idt[i].reserved1 = 1 ;
        idt[i].reserved2 = 1 ;
        idt[i].reserved3 = 0 ;
        idt[i].reserved4 = 0;//0x1F;
        
        idt[i].present = 0;
        idt[i].dpl = 0;

        // interrupt gate 
        if(i == systemCall ){
            idt[i].dpl = dplnum;
            idt[i].present = 1;
            // idt[i].reserved3 = 1;
        }
        if(i == PITint || i == RTCint || i == keyboardInt || i<=LAST_EXCEPTION){
            // idt[i].dpl = 3;
            idt[i].present = 1;
            // idt[i].reserved3 = 1;
        }
        idt[i].seg_selector = KERNEL_CS; // setting the segment selector to the kernel code segment
    }
    /*
    Setting up the first 19 exceptions in IDT, these are not magic numbers
    */

    //basically sets up the memory location/address and dereferences it, giving the index as well
    //there are technically 256 interrupts but we only assign the first 20

    SET_IDT_ENTRY(idt[0], divide_by_zero);
    SET_IDT_ENTRY(idt[1], single_step_int);
    SET_IDT_ENTRY(idt[2], non_maskable_int);  
    SET_IDT_ENTRY(idt[3], breakpoint);
    SET_IDT_ENTRY(idt[4], overflow);
    SET_IDT_ENTRY(idt[5], bound_range_exceeds);
    SET_IDT_ENTRY(idt[6], invalid_opcode);
    SET_IDT_ENTRY(idt[7], coprocessor_not_avail);
    SET_IDT_ENTRY(idt[8], double_fault);
    SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun);
    SET_IDT_ENTRY(idt[10], invalid_task_state);
    SET_IDT_ENTRY(idt[11], segment_not_present);
    SET_IDT_ENTRY(idt[12], stack_segment_fault);
    SET_IDT_ENTRY(idt[13], general_protection_fault);
    SET_IDT_ENTRY(idt[14], page_fault);
    // SET_IDT_ENTRY(idt[15], handler);
    SET_IDT_ENTRY(idt[16], floating_point_exception);
    SET_IDT_ENTRY(idt[17], alignment_check );
    SET_IDT_ENTRY(idt[18], machine_check);
    SET_IDT_ENTRY(idt[19], SIMD_floating_point);
    // Setting the keyboard handler
    SET_IDT_ENTRY(idt[keyboard], keyboard_handler);
    SET_IDT_ENTRY(idt[RTCint], RTC_handler);
    SET_IDT_ENTRY(idt[PITint], PIT_handler);
    // Setting up the System Call Handler
    // idt[systemCall].reserved3= 1;
    SET_IDT_ENTRY(idt[systemCall], system_call_handler);
    lidt(idt_desc_ptr);
    return;
}

/* extern void ____();
 * Inputs: void
 * Return Value: prints what the exception is
 * Function: represents 19 different exceptions and prints it based on the exception it is and has a constant while loop looping through */

extern void divide_by_zero(){
    clear(get_display_terminal());
    printf("Exception: Division by Zero\n");
    system_halt((uint8_t)HALT_error);
   
}
// 0x1
extern void single_step_int(){
    printf("Exception: single step interrupt\n");
    system_halt((uint8_t)HALT_error);
   
}     
// 0x2
extern void non_maskable_int(){
    printf("Exception: NMI\n");
    system_halt((uint8_t)HALT_error);
   
}      
extern void breakpoint(){
    printf("Exception: breakpoint\n");
    system_halt((uint8_t)HALT_error);
   
}     
extern void overflow(){
    printf("Exception: overflow\n");
    system_halt((uint8_t)HALT_error);
   
}  
extern void bound_range_exceeds(){
    printf("Exception: bound range exceeds\n");
    system_halt((uint8_t)HALT_error);
   
}      
extern void invalid_opcode(){
    printf("Exception: Invalid Opcode\n");
    system_halt((uint8_t)HALT_error);
   
}     
extern void coprocessor_not_avail(){
    printf("Exception: Coprocessor not available\n");
    system_halt((uint8_t)HALT_error);
   
}   
extern void double_fault(){
    printf("Exception: double fault\n");
    system_halt((uint8_t)HALT_error);
   
}      
extern void coprocessor_segment_overrun(){
    printf("Exception: Coprocessor segment overrun \n");
    system_halt((uint8_t)HALT_error);
   
}     
extern void invalid_task_state(){
    printf("Exception: invalid task state \n");
    system_halt((uint8_t)HALT_error);
   
}  
extern void segment_not_present(){
    printf("Exception: segment not present\n");
    system_halt((uint8_t)HALT_error);
   
}      
extern void stack_segment_fault(){
    printf("Exception: stack segment fault\n");
    system_halt((uint8_t)HALT_error);
   
}  
extern void general_protection_fault(){
    printf("Exception: general protection fault\n");
    system_halt((uint8_t)HALT_error);
   
}
extern void page_fault(){
    // clear();
    printf("Exception: Page Fault\n");
    system_halt((uint8_t)HALT_error);
   
}
// 0xF reserved 

//0x10
extern void floating_point_exception(){
    printf("Exception: Floating Point Exception\n");
    system_halt((uint8_t)HALT_error);
}

// 0x11
void alignment_check(){
    printf("Exception: alignment Exception\n");
    system_halt((uint8_t)HALT_error);
}
//0x12
void machine_check(){
    printf("Exception: Machine Check Exception\n");
    system_halt((uint8_t)HALT_error);
}
//0x13
void SIMD_floating_point(){
    printf("Exception: SIMD Floating Point Exception\n");
    system_halt((uint8_t)HALT_error);
}
