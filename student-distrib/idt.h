#ifndef _IDT_H_
#define _IDT_H_


#include "types.h"
#include "x86_desc.h"
#include "multiboot.h"
#include "lib.h"
#include "keyboard.h"
#include "RTC.h"
#include "system_call.h"

// these declare the 19 exceptions that are used in idt.c

extern void idt_initialization();
extern void PIT_handler();

extern void divide_by_zero();               // 0x0
extern void single_step_int();              // 0x1      
extern void non_maskable_int();             // 0x2      
extern void breakpoint();                   // 0x3      
extern void overflow();                     // 0x4      
extern void bound_range_exceeds();          // 0x5      
extern void invalid_opcode();               // 0x6      
extern void coprocessor_not_avail();        // 0x7      
extern void double_fault();                 // 0x8      
extern void coprocessor_segment_overrun();  // 0x9      
extern void invalid_task_state();           // 0xA      
extern void segment_not_present();          // 0xB      
extern void stack_segment_fault();          // 0xC   
extern void general_protection_fault();     // 0xD
extern void page_fault();                   // 0xE
                                            // 0xF reserved 
extern void floating_point_exception();     // 0x10 

void alignment_check();                     // 0x11
void machine_check();                       //0x12
void SIMD_floating_point();                 //0x13

#endif // _IDT_H





