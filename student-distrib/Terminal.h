#ifndef _Terminal_H
#define _Terminal_H

#include "lib.h"
#include "types.h"
#include "keyboard.h"

static char line[128];

uint32_t terminal_read(int32_t fd, void *buf, int32_t nbytes);

uint32_t terminal_write(int32_t fd, const void* buf, int32_t nbytes);

uint32_t terminal_open(const int8_t* filename);

uint32_t terminal_close(int32_t fd);


#endif
