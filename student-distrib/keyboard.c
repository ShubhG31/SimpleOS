#include "keyboard.h"


//  void keyboard_handler(){
//     asm volatile(" \n\
//                 pushal \n\
//                 pushfl  \n\
//                 call keyboard_helper \n\
//                 popfl  \n\
//                 popal \n\
//                 iret \n\
//                 "   
//     );
// }
#define keycode_0 0xB
#define Ascii_0 48
static int keyboard_keycodes[249];


void init_keycodes(){
    int i,pt;
    char ch[3][10] =  {"qwertyuiop","asdfghjkl","zxcvbnm"};
    int port[3]={0x10,0x1E,0x2C};
    for(i=0;i<249;i++){
        keyboard_keycodes[i]= 39;
    }
    for(i=2; i< 11; i++){
        keyboard_keycodes[i]= 47+i;
    }
    keyboard_keycodes[keycode_0] = Ascii_0;

    // clear();
    for(pt=0;pt<3;pt++){
        for(i=0;i<10;i++){
            if(ch[pt][i]==0)break;
            keyboard_keycodes[port[pt]+i]=(int)ch[pt][i];
        }
    }
}
void keyboard_helper(){
    int port = keyboard_port;
    int scan_code = inb(port);
    // send_eoi(End);
    // clear();
    // printf("Hellooooo");
    if(keyboard_keycodes[scan_code] != 39){
    putc(keyboard_keycodes[scan_code]);
    }
    //clear();
    // putc('a');
    

    send_eoi(keyboard_irq_num);
    return;
}

void keyboard_init_irq(){
    init_keycodes();
    enable_irq(keyboard_irq_num);
    return;
}
