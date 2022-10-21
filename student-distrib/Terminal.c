#include "Terminal.h"

uint32_t terminal_read(int32_t fd, void *buf, int32_t nbytes){
    // if(screen_x==79){
    //     screen_y++;
    // }
    // if(keyboard_keycodes[scan_code] != space){
    //     putc(keyboard_keycodes[scan_code]);
    // }
    // sti();
    
    // while(!newline_flag);

    // cli();
    strncpy(buf, buffer,128);
    memset(buffer,0,strlen(buffer));
    // printf("%s",line);
    // printf("%s,buffer)
    return strlen;
}
uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes){

}

uint32_t terminal_open(const int8_t* filename){

}

uint32_t terminal_close(int32_t fd){

}
