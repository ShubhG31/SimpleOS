#ifndef _Terminal_H
#define _Terminal_H

#include "lib.h"
#include "types.h" 

//functions written used for terminal.c read, write, open and close

uint32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);

uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

uint32_t terminal_open(const int8_t* filename);

uint32_t terminal_close(int32_t fd);

void copy_buffer(char* keyboard);

#endif
