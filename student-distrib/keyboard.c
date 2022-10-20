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
#define print_screen 0xE0
#define keycode_0 0xB
#define Ascii_0 48
#define Q 0x10
#define A 0x1E
#define Z 0x2C
#define keys 256
#define space 32
#define ascii_num_conversion 47
#define key_1 2
#define ROWS 3
#define key_9 11
#define COLUMNS 12

#define enter 0x1C
#define new_line 10 

#define backspace 0x0E
#define BS_ascii 8

#define space_keycode 0x39 

#define caps_keycode_pressed 0x3A
#define caps_keycode_released 0xBA

#define l_shift_keycode_pressed 0x2A
#define l_shift_keycode_released 0xAA

#define r_shift_keycode_pressed 0x36
#define r_shift_keycode_released 0xB6

#define backtick_keycode 0x29
#define backtick_ascii 96
#define tilde '~'

#define CTRL_keycode_pressed 0x1D
#define CTRL_keycode_released 0x9D

#define lower_a_ascii 97
#define lower_z_ascii 122
#define uppercase_conversion 32

#define backslash_keycode 0x2B
#define backslash_ascii 92
#define straight_line 124

#define apostrophe 39

#define minus_keycode 0x0C
#define equal_keycode 0x0D

static int caps_lock_flag = 0;
static int shift_flag = 0;
static int ctrl_flag = 0;

//ALL MAGIC NUMBER LABELS

static int keyboard_keycodes[keys];
static char special_num_char[10]=")!@#$%^&*(";

/* void init_keycodes();
 * Inputs: none
 * Return Value: makes sure the keycodes are initialized
 * Function: initializes the keycodes to use for all of the keyboard drivers*/

void init_keycodes(){
    int i,pt;
    char ch[3][12] =  {"qwertyuiop[]","asdfghjkl;'","zxcvbnm,./"};
    int port[3]={Q,A,Z};
    for(i=0;i<keys;i++){
        keyboard_keycodes[i]= print_screen;
    }
    for(i=key_1; i< key_9; i++){
        keyboard_keycodes[i]= ascii_num_conversion+i;
    }
    keyboard_keycodes[keycode_0] = Ascii_0;

    // clear();
    for(pt=0;pt<ROWS;pt++){
        for(i=0;i<COLUMNS;i++){
            if(ch[pt][i]==0)break;
            keyboard_keycodes[port[pt]+i]=(int)ch[pt][i];
        }
    }
    keyboard_keycodes[enter] = new_line;

    keyboard_keycodes[backspace] = BS_ascii;

    keyboard_keycodes[space_keycode] = space;

    keyboard_keycodes[backtick_keycode] = backtick_ascii;

    keyboard_keycodes[backslash_keycode] = backslash_ascii;

    keyboard_keycodes[minus_keycode] = '-';
    keyboard_keycodes[equal_keycode] = '=';

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

    // if backspace return print backspace
    if(keyboard_keycodes[scan_code] == keyboard_keycodes[backspace]){
        putc(BS_ascii);
        send_eoi(keyboard_irq_num);
        return;
    }
    // if enter is pressed, print newline and return 
    if(keyboard_keycodes[scan_code] == keyboard_keycodes[enter]){
        putc(keyboard_keycodes[enter]);
        send_eoi(keyboard_irq_num);
        return;
    }
    
    // if scan code is shift key then set flag to high 
    if(scan_code == l_shift_keycode_pressed || scan_code == r_shift_keycode_pressed){
        shift_flag = 1;
        // putc('?');
    }
    // if shift is releaased then set flag to low
    if(scan_code == l_shift_keycode_released || scan_code == r_shift_keycode_released){
        shift_flag = 0;
        // putc('!');
    }
    //if caps lock is pressed check if flag is set to high
    if(scan_code == caps_keycode_pressed){
        // if flag is high then set to low
        if(caps_lock_flag == 1){
            caps_lock_flag = 0;
        }
        // if set low then set high 
        else{
            caps_lock_flag = 1;
        }
    }
    // if ctrl is pressed then set the flag to high
    if(scan_code == CTRL_keycode_pressed){
        ctrl_flag = 1;
    }
    // 
    if(scan_code == CTRL_keycode_released){
        ctrl_flag = 0;
    }
    // if ctrl is pressed and l is pressed, clear screen and return 
    if (ctrl_flag==1 && keyboard_keycodes[scan_code]=='l'){
        clear();
        send_eoi(keyboard_irq_num); 
        return;
    }
    if(keyboard_keycodes[scan_code] != print_screen){
       if(shift_flag){
            // prints out the special characters in the number row
            if(keyboard_keycodes[scan_code]>= '0' && keyboard_keycodes[scan_code]<='9'){
                int special_character_index = keyboard_keycodes[scan_code] - 48;
                putc(special_num_char[special_character_index]);
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='`'){
                putc(tilde);
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='['){
                putc('{');
                goto end;
            }
            if(keyboard_keycodes[scan_code]==']'){
                putc('}');
                goto end;
            }
            if(keyboard_keycodes[scan_code]== backslash_ascii){
                putc(straight_line);
                goto end;
            }
            if(keyboard_keycodes[scan_code]==','){
                putc('<');
                goto end;
            }          
            if(keyboard_keycodes[scan_code]=='.'){
                putc('>');
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='/'){
                putc('?');
                goto end;
            }
            if(keyboard_keycodes[scan_code]==';'){
                putc(':');
                goto end;
            }
            if(keyboard_keycodes[scan_code]==apostrophe){
                putc('"');
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='-'){
                putc('_');
                goto end;

            }
            if(keyboard_keycodes[scan_code]=='='){
                putc('+');
                goto end;
            }
        }
        if((shift_flag ^ caps_lock_flag) && (keyboard_keycodes[scan_code]>=lower_a_ascii && keyboard_keycodes[scan_code]<=lower_z_ascii)){
                putc(keyboard_keycodes[scan_code]-uppercase_conversion);
        }
        else{
            putc(keyboard_keycodes[scan_code]);
        }
    }
    
end:
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
