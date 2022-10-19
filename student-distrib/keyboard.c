#include "keyboard.h"


//  void keyboard_handler(){
//     asm volatile(" \n
//                 pushal \n
//                 pushfl  \n
//                 call keyboard_helper \n
//                 popfl  \n
//                 popal \n
//                 iret \n
//                 "   
//     );
// }

// ALL MAGIC NUMBER LABELS
#define keycode_0 0xB
#define Ascii_0 48
#define Q 0x10
#define A 0x1E
#define Z 0x2C
#define keys 249
#define space 32
#define range 47
#define low 2
#define low1 3
#define high 11
#define high1 10

#define enter 0x1C
#define new_line 10 

#define backspace 0x0E
#define BS_ascii 8

//ALL MAGIC NUMBER LABELS

static int keyboard_keycodes[249];

/* void init_keycodes();
 * Inputs: none
 * Return Value: makes sure the keycodes are initialized
 * Function: initializes the keycodes to use for all of the keyboard drivers*/

void init_keycodes(){
    int i,pt;
    char ch[3][10] =  {"qwertyuiop","asdfghjkl","zxcvbnm"};
    int port[3]={Q,A,Z};
    for(i=0;i<keys;i++){
        keyboard_keycodes[i]= space;
    }
    for(i=low; i< high; i++){
        keyboard_keycodes[i]= range+i;
    }
    keyboard_keycodes[keycode_0] = Ascii_0;

    // clear();
    for(pt=0;pt<low1;pt++){
        for(i=0;i<high1;i++){
            if(ch[pt][i]==0)break;
            keyboard_keycodes[port[pt]+i]=(int)ch[pt][i];
        }
    }
    keyboard_keycodes[enter] = new_line;

    keyboard_keycodes[backspace] = BS_ascii;
}

/* void keyboard_helper();
 * Inputs: none
 * Return Value: writes a particular keycode given a scan code
 * Function: overall sees which key uses the particular scan code set and writes it to the screen. it then sends an end of interrupt because the function is over */

void keyboard_helper(){
    int port = keyboard_port;
    int scan_code = inb(port);
    // send_eoi(End);
    // clear();
    // printf("Hellooooo");
    if(keyboard_keycodes[scan_code] != space){
    putc(keyboard_keycodes[scan_code]);
    }
    //clear();
    // putc('a');
    

    send_eoi(keyboard_irq_num);
    return;
}

/* void keyboard_init_irq();
 * Inputs: none
 * Return Value: none
 * Function: initializes the keyboard and enables the interrupt irq */

void keyboard_init_irq(){
    init_keycodes();
    enable_irq(keyboard_irq_num);
    return;
}
