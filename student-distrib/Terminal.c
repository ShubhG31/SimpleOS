#include "Terminal.h"
static char line[128];
static int newline_flag_terminal = 0;


/* uint32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);
 * Inputs: fd     -- file descriptor, not used, but needed for System Call organization
           buf    -- buffer passed into copy values from 
           nbytes -- the length of the buffer, not used, but needed for System Call organization
 * Return Value: the bytes read from line buffer till next line character 
 * Function: Reads the Keyboard buffer to copy to buffer given, to echo  */
uint32_t terminal_read(int32_t fd, void *buf, int32_t nbytes){
    // checks the null of buf, if null return -1
    if(buf == NULL){
        return -1;
    }
    // waits for the newline flag to be high, if high then start commiting running code 
    while(!newline_flag_terminal);
    // Clear interrupt 
    cli();
    // initialize the counter to go through line buffer 
    int i=0;
    // check the line buffer till next line character is reached
    while(line[i]!='\n'){
        ((char*)buf)[i] = line[i];
        i++;
    }
    memset(line,0,strlen(line));
    newline_flag_terminal = 0;
    sti();
    return i;
}

/* void keyboard_init_irq();
 * Inputs: none
 * Return Value: none
 * Function: initializes the keyboard and enables the interrupt irq */
uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    if(buf == NULL){
        return -1;
    }
    int i;
    for(i = 0; i < nbytes; i++){
        putc(((char*)buf)[i]);
    }
    return i;
}

/* void keyboard_init_irq();
 * Inputs: none
 * Return Value: none
 * Function: initializes the keyboard and enables the interrupt irq */
uint32_t terminal_open(const int8_t* filename){
    if(filename == NULL){
        return -1;
    }
    return 0;
}

/* void keyboard_init_irq();
 * Inputs: none
 * Return Value: none
 * Function: initializes the keyboard and enables the interrupt irq */
uint32_t terminal_close(int32_t fd){
    return 0;
}

/* void keyboard_init_irq();
 * Inputs: none
 * Return Value: none
 * Function: initializes the keyboard and enables the interrupt irq */
void copy_buffer(char* keyboard){
    strncpy(line,keyboard,strlen(keyboard));
    newline_flag_terminal = 1;
    return;
}
