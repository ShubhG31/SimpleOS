#include "Terminal.h"
static char line[128];
static int newline_flag_terminal[3] = {0,0,0};


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
    
    int curr_terminal = get_main_terminal();
    // waits for the newline flag to be high, if high then start commiting running code 
    while(!newline_flag_terminal[curr_terminal]);
    // Clear interrupt 
    
    // initialize the counter to go through line buffer 
    int i=0;
    // check the line buffer till next line character is reached
    while(line[i]!='\n'){
        ((char*)buf)[i] = line[i];
        i++;
    }
    ((char*)buf)[i] = line[i];
    i++;
    // puts(buf);
    // clear line buffer
    memset(line,0,strlen(line));
    // new line flag is set to low 
    newline_flag_terminal[curr_terminal] = 0;
    // set interupts
    sti();
    return i;
}

/* uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: fd     -- file desciptor 
           buf    -- buffer passed which is normally a char buffer 
           nbytes --  bytes that are going to be copied to the screen 
 * Return Value: bytes that have been written on the screen
 * Function: writes nbytes to the screen */
uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){
    // if buffer is null then return -1, else continue 
    cli();
    if(buf == NULL){
        return -1;
    }
    int i;
    // go through all bytes and print it on the screen
    for(i = 0; i < nbytes; i++){
        putc_user_code(((char*)buf)[i]);
    }
    sti();
    return i;
}

/* uint32_t terminal_open(const int8_t* filename);
 * Inputs: filename -- pointer to the file 
 * Return Value: if success return 0, else return -1 
 * Function: Returns if terminal open is successful or not  */
uint32_t terminal_open(const int8_t* filename){
    if(filename == NULL){
        return -1;
    }
    return 0;
}

/* uint32_t terminal_close(int32_t fd);
 * Inputs: fd -- file descriptor 
 * Return Value: returns 0 or -1 dependent on success or failure
 * Function: returns value when terminal is closed */
uint32_t terminal_close(int32_t fd){
    return 0;
}

/* void copy_buffer(char* keyboard);
 * Inputs: keyboard -- buffer given to copy over to Terminal buffer 
 * Return Value: none
 * Function: copies over a buffer given to Terminal global buffer and sets global flag for newline to high */
void copy_buffer(char* keyboard){
    // copy over buffer to line buffer for terminal
    strncpy(line,keyboard,strlen(keyboard));
    // new line flag is set to high 
    int curr_terminal=get_display_terminal();
    newline_flag_terminal[curr_terminal] = 1;
    return;
}
