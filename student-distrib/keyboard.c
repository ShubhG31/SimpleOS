#include "keyboard.h"


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

#define tab_keycode 0x0F
#define tab_ascii 9

#define alt_pressed 0x38
#define alt_pressed2 0x36
#define alt_released 0xB8
#define alt_released2 0xB6

#define f1_pressed 0x3B
#define f2_pressed 0x3C
#define f3_pressed 0x3D

#define max_characters 127

static int caps_lock_flag = 0;
static int shift_flag = 0;
static int ctrl_flag = 0;
static int alt_flag = 0;
static int alt1_flag = 0, alt2_flag = 0;
int enter_flags[3]={0,0,0};
//ALL MAGIC NUMBER LABELS

static int keyboard_keycodes[keys];
static char special_num_char[10]=")!@#$%^&*(";
char buffer[3][128] = {{0},{0},{0}};
static unsigned int buffer_cur_location[3] = {0};

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
    cli();                                              // question here idk whether we need this
    int port = keyboard_port;
    int scan_code = inb(port);
    int B8_B9_backup;

    int curr_terminal = get_display_terminal();     // everything we type is put into the display terminal buffer

    // if alt is pressed 
    if(scan_code == alt_pressed) alt1_flag = 1;

    if(scan_code == alt_pressed2) alt2_flag = 1;

    // this is to deal with the edge case that i press alt1 and dont release and press alt2
    // and then release alt2, the alt2_flag will never be raised (seemingly)
    if(scan_code == alt_pressed || scan_code == alt_pressed2){
        send_eoi(keyboard_irq_num);
        sti();
        return;
    }

    if(scan_code == alt_released) alt1_flag = 0;

    if(scan_code == alt_released2) alt2_flag = 0;

    if(scan_code == alt_released || scan_code == alt_released2){
        send_eoi(keyboard_irq_num);
        sti();
        return;
    }

    alt_flag = alt1_flag | alt2_flag;

    if(alt_flag == 1 && scan_code == f1_pressed){
        switch_terminal(1);
        send_eoi(keyboard_irq_num);
        sti();
        return;
    }
    if(alt_flag == 1 && scan_code == f2_pressed){
        switch_terminal(2);
        send_eoi(keyboard_irq_num);
        sti();
        return;
    }
    if(alt_flag == 1 && scan_code == f3_pressed){
        switch_terminal(3);
        send_eoi(keyboard_irq_num);
        sti();
        return;
    }
    
    if(get_executing_status(curr_terminal)==1){ // when we get into the user code it is 1, we ban the keyboard to type anything instead of using terminal read
        send_eoi(keyboard_irq_num);
        sti();
        return;
    }

    // if locaton is 0 and if pressed key is backspace  
    if( buffer_cur_location[curr_terminal] == 0 && keyboard_keycodes[scan_code] == keyboard_keycodes[backspace]){
        // return end of interupt 
        send_eoi(keyboard_irq_num);
        sti();
        return;
    }

    B8_B9_backup=get_B8_B9_table();
    map_B8_B9_table(0xB8);
    flush_tlb();

    // if buffer location is greater than 0 and if backspace is pressed
    if( buffer_cur_location[curr_terminal] > 0 && keyboard_keycodes[scan_code] == keyboard_keycodes[backspace]){
            putc(BS_ascii); // print backspace 
            // set current location of buffer current index to 0
            buffer[curr_terminal][buffer_cur_location[curr_terminal]] = 0;
            // decrement the location since backspace is pressed
            buffer_cur_location[curr_terminal]--;
            // send end of interrupt signal
            goto end;
            // send_eoi(keyboard_irq_num);
            // return;
    }
    // if enter is pressed, print newline and return 
    if( keyboard_keycodes[scan_code] == keyboard_keycodes[enter]){

        // if the display one is not running terminal, we should not let the enter pressed
        // if(get_main_terminal()!=get_display_terminal()){
        //     enter_flags[get_display_terminal()]=1;
        //     goto end;
        // }

        // put new line into the buffer 
        buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '\n';
        // buffer location is now set to 0 
        buffer_cur_location[curr_terminal] = 0;
        // print out new line
        putc(keyboard_keycodes[enter]);
        // clear buffer 
        copy_buffer(buffer[curr_terminal]);
        // clear keyboard buffer 
        memset((void*)buffer[curr_terminal],0,strlen((int8_t*)buffer));
        // send end of interupt signal
        goto end;
        // send_eoi(keyboard_irq_num);
        // return;
    }
    
    // if scan code is shift key then set flag to high 
    if(scan_code == l_shift_keycode_pressed || scan_code == r_shift_keycode_pressed){
        // shift flag is set to high
        shift_flag = 1;
    }
    // if shift is releaased then set flag to low
    if(scan_code == l_shift_keycode_released || scan_code == r_shift_keycode_released){
        // shift flag is set to low 
        shift_flag = 0;
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
        // clear screen
        clear(get_display_terminal());
        // send end of interupt signal
        // send_eoi(keyboard_irq_num); 
        // return;
        goto end;
    }

    // if buffer location is less than allowed value of 127 and pressed key is allowed then execute
    if(buffer_cur_location[curr_terminal] < max_characters && keyboard_keycodes[scan_code] != print_screen){
       if(shift_flag){
            // prints out the special characters in the number row
            if(keyboard_keycodes[scan_code]>= '0' && keyboard_keycodes[scan_code]<='9'){
                int special_character_index = keyboard_keycodes[scan_code] - 48; // 48 is the conversion to index to index of the special characters array
                putc(special_num_char[special_character_index]);
                buffer[curr_terminal][buffer_cur_location[curr_terminal]]=special_num_char[special_character_index];
                // buffer location is incremented 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='`'){
                putc(tilde);
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = tilde;
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='['){
                putc('{');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '{';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]==']'){
                putc('}');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '}';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]== backslash_ascii){
                putc(straight_line);
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = straight_line;
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]==','){
                putc('<');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '<';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }          
            if(keyboard_keycodes[scan_code]=='.'){
                putc('>');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '>';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='/'){
                putc('?');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '?';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]==';'){
                putc(':');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = ':';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]==apostrophe){
                putc('"');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '"';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
            if(keyboard_keycodes[scan_code]=='-'){
                putc('_');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '_';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;

            }
            if(keyboard_keycodes[scan_code]=='='){
                putc('+');
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = '+';
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
                goto end;
            }
        }
        // if flags are high and inside the ascii values of a and z, then print out uppercase 
        if((shift_flag ^ caps_lock_flag) && (keyboard_keycodes[scan_code]>=lower_a_ascii && keyboard_keycodes[scan_code]<=lower_z_ascii)){
                putc(keyboard_keycodes[scan_code]-uppercase_conversion);
                buffer[curr_terminal][buffer_cur_location[curr_terminal]] = keyboard_keycodes[scan_code]-uppercase_conversion;
                // increment the buffer current index 
                buffer_cur_location[curr_terminal]+=1;
        }
        else{
            putc(keyboard_keycodes[scan_code]);
            buffer[curr_terminal][buffer_cur_location[curr_terminal]] = keyboard_keycodes[scan_code];
            // increment the buffer current index 
            buffer_cur_location[curr_terminal]+=1;
        }
    }
    
end:
    map_B8_B9_table( B8_B9_backup );
    flush_tlb();
    send_eoi(keyboard_irq_num);
    sti();
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

int get_enter_flag(int terminal_){
    return enter_flags[terminal_];
}
