#include "Terminal.h"

uint32_t terminal_read(int32_t fd, void *buf, int32_t nbytes){
    if(buf == NULL){
        return -1;
    }
    while(!newline_flag_terminal);
    cli();
    int i=0;
    while(line[i]!='\n'){
        ((char*)buf)[i] = line[i];
        i++;
    }
    memset(line,0,strlen(line));
    newline_flag_terminal = 0;
    sti();
    return i;
}
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

uint32_t terminal_open(const int8_t* filename){
    return -1;
}

uint32_t terminal_close(int32_t fd){
    return-1;
}

void copy_buffer(char* keyboard){
    strncpy(line,keyboard,strlen(keyboard));
    newline_flag_terminal = 1;
    return;
}
