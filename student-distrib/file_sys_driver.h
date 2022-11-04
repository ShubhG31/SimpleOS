#ifndef _filesys_
#define _filesys_

#include "lib.h"
#include "types.h"
#include "file_sys.h"
#include "system_call.h"
#define FD_num 8


extern int dir_open(const int8_t* filename);
extern int dir_close(int32_t fd);
extern int dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int dir_write(int32_t fd, const void* buf, int32_t nbytes);

extern int file_open(const int8_t* filename);
extern int file_close(int32_t fd);
extern int file_read(int32_t fd, void* buf, int32_t nbytes);
extern int file_write(int32_t fd, const void* buf, int32_t nbytes);
extern int file_sys_init();

#endif
