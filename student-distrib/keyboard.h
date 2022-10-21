#ifndef _KEYBOARD_
#define _KEYBOARD_

#include "i8259.h"
#include "lib.h"

#define keyboard_port 0x60 
#define keyboard_irq_num 0x01

extern char buffer[128];
static unsigned int buffer_cur_location = 0;
volatile static int newline_flag = 0; 

void keyboard_init_irq(); // intialize the keyboard
extern void keyboard_handler();

void keyboard_helper();
// #define keyboard_link(name, func)
//     .globl name
//         name:
//             pushal
//             pushfl
//             call func
//             popfl
//             popal
//             iret
// keyboard_link(keyboard_handler, keyboard_helper)

#endif // _KEYBOARD_
