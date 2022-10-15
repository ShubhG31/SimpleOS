#include "idt.h"
// #include ""

#define LAST_EXCEPTION 0xF  // defined as the last software generated exception which is 15  

// typedef void (*functions);


// function[15] = ["divide_by_zero",
//                   "single_step_int",
//                   "non_maskable_int",
//                   "breakpoint",
//                   "overflow"
//                   "bound_range_exceeds"
//                   "invalid_opcode"
//                   "coprocessor_not_avail"
//                   "double_fault"
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

void idt_initialization(){
    int i;
    for(i = 0; i<256; i++){
        if (i <= LAST_EXCEPTION && i != 0xF){
            idt[i].dpl = 0;
            idt[i].present = 1;
        }
        else{
            idt[i].present = 0;
        }
        idt[i].reserved0 = 0;   
        idt[i].size = 1 ;       // D bit is set to 1, look above
        idt[i].reserved1 = 1 ;
        idt[i].reserved2 = 1 ;
        idt[i].reserved3 = 0 ;
        idt[i].reserved4 &= 0x1F;
        if(i == 0x80){
            idt[i].dpl = 3;
            idt[i].present = 1;
            // idt[i].reserved3 = 1;
        }
        idt[i].seg_selector = KERNEL_CS; // setting the segment selector to the kernel code segment
    }
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

    // Setting the keyboard handler
    SET_IDT_ENTRY(idt[0x21], keyboard_handler);
    lidt(idt_desc_ptr);
    return;
}
extern void divide_by_zero(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}
// 0x1
extern void single_step_int(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}     
// 0x2
extern void non_maskable_int(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}      
extern void breakpoint(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}     
extern void overflow(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}  
extern void bound_range_exceeds(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}      
extern void invalid_opcode(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}     
extern void coprocessor_not_avail(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}   
extern void double_fault(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}      
extern void coprocessor_segment_overrun(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}     
extern void invalid_task_state(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}  
extern void segment_not_present(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}      
extern void stack_segment_fault(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}  
extern void general_protection_fault(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}
extern void page_fault(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}
// 0xF reserved 

//0x10
extern void floating_point_exception(){
    printf("Exception: Division by Zero\n");
    while(1);
   
}
// int main(){
// SET_IDT_ENTRY(idt[0], divide_by_zero);
// SET_IDT_ENTRY(idt[1], single_step_int);
// SET_IDT_ENTRY(idt[2], non_maskable_int);  
// SET_IDT_ENTRY(idt[3], breakpoint);
// SET_IDT_ENTRY(idt[4], overflow);
// SET_IDT_ENTRY(idt[5], bound_range_exceeds);
// SET_IDT_ENTRY(idt[6], invalid_opcode);
// SET_IDT_ENTRY(idt[7], coprocessor_not_avail);
// SET_IDT_ENTRY(idt[8], double_fault);
// SET_IDT_ENTRY(idt[9], coprocessor_segment_overrun);
// SET_IDT_ENTRY(idt[10], invalid_task_state);
// SET_IDT_ENTRY(idt[11], segment_not_present);
// SET_IDT_ENTRY(idt[12], stack_segment_fault);
// SET_IDT_ENTRY(idt[13], general_protection_fault);
// SET_IDT_ENTRY(idt[14], page_fault);
// // SET_IDT_ENTRY(idt[15], handler);
// SET_IDT_ENTRY(idt[16], floating_point_exception);
// lidt(idt_desc_ptr);
// return 0;
// }