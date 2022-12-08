#ifndef _KEYBOARD_
#define _KEYBOARD_

#include "i8259.h"
#include "lib.h"
#include "Terminal.h"
#include "system_call.h"
#include "paging.h"

#define keyboard_port 0x60 
#define keyboard_irq_num 0x01

extern char buffer[3][128];
// extern char past_entries[3][5][128];
// volatile static int newline_flag = 0; 

void keyboard_init_irq(); // intialize the keyboard
extern void keyboard_handler();

void keyboard_helper();

extern int get_enter_flag(int terminal_);

extern int prev_arrow_pressed();

extern int len_prev();

#endif // _KEYBOARD_
