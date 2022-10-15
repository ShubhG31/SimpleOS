#include "keyboard.h"


extern void keyboard_handler(){
    asm volatile("
                pushal 
                pushfl  
                call keyboard_helper  
                popfl  
                popal 
                iret 
                "   
    )
}

extern void keyboard_helper(){
    int port = keyboard_port;
    asm volatile("
            inb port
            
    
    
    "

    )
}
