#include "keyboard.h"


 void keyboard_handler(){
    asm volatile(" \n\
                pushal \n\
                pushfl  \n\
                call keyboard_helper \n\
                popfl  \n\
                popal \n\
                iret \n\
                "   
    );
}

void keyboard_helper(){
    int port = keyboard_port;
    char read = inb(port);
    // send_eoi(End);
    clear();
    // putc('a');
    printf('Hellooooo');

    send_eoi(keyboard_irq_num);
    return;
}

void keyboard_init_irq(){
    enable_irq(keyboard_irq_num);
    return;
}
