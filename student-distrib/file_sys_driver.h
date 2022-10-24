#include "lib.h"
#include "types.h"
#define FD_num 8

struct file_descriptor{
    uint32_t opt_table_pointer;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
};

extern int dir_open(const int8_t* filename);
extern int dir_close(int32_t fd);
extern int dir_read(int32_t fd, void* buf, int32_t nbytes);
extern int dir_write(int32_t fd, const void* buf, int32_t nbytes);

extern int file_open(const int8_t* filename);
extern int file_close(int32_t fd);
extern int file_read(int32_t fd, void* buf, int32_t nbytes);
extern int file_write(int32_t fd, const void* buf, int32_t nbytes);
extern int file_sys_init();
