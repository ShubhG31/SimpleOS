#define keyboard_port 0x60 


extern void keyboard_handler();

extern void keyboard_helper();
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