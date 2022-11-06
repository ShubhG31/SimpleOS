#ifndef _system_call_
#define _system_call_

#include "types.h"
#define exe_0 0x7f
#define exe_1 0x45
#define exe_2 0x4c
#define exe_3 0x46
#define HALT 37
#define HALT_error 256

struct file_descriptor{
    uint32_t opt_table_pointer;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
};
struct PCB_table{
    int8_t id;                 // 1 byte
    int8_t parent_id;            // 1 byte
    int32_t saved_esp;          // 4 byte
    int32_t saved_ebp;          // 4 byte
    int8_t active;              // 1 byte
    int8_t fdt_usage; //00000011// 1 byte
    struct file_descriptor fdt[8]; // 16 byte each
};
extern int system_call_handler();

extern void fd_init();
extern int system_halt(uint8_t status);
extern int system_execute(const uint8_t* command); 
extern int system_read(int32_t fd, void* buf, int32_t nbytes); 
extern int system_write(int32_t fd, const void* buf, int32_t nbytes); 
extern int system_open(const uint8_t* filename); 
extern int system_close(int32_t fd);
extern int system_getargs(uint8_t* buf, int32_t nbytes); 
extern int system_vidmap(uint8_t** screen_start);
extern int get_pcb_pointer();
int check_fd_in_use(int32_t fd);
extern int get_pid();
extern void IRET_prepare(int);
extern void label();

#endif
