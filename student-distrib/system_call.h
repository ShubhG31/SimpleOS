#ifndef _system_call_
#define _system_call_

#include "types.h"
#define exe_0 0x7f
#define exe_1 0x45
#define exe_2 0x4c
#define exe_3 0x46
#define HALT 37
#define HALT_error 256

#define addr_8MB 0x800000  //8*1024*1024
#define size_4MB 0x400000 
#define size_1MB 0x100000
#define size_8kb 0x2000 //8*1024
#define size_4kb 0x1000 //4*1024
#define pcb_pid_off 0
#define pcb_parid_off 1
#define pcb_saved_esp_off 2
#define pcb_saved_ebp_off 6
#define pcb_active_off 10
#define pcb_fdt_usage_off 11
#define pcb_fd_off 12
#define fd_size 16
#define PCB_size 8
#define Program_page 4
#define text_read 40
#define command_length 128
#define vidpointer 36*4*1024*1024+184*4*1024

//describes and takes in what is necessary for the file descriptor
struct file_descriptor{
    uint32_t opt_table_pointer;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
};

//describes what is necessary for the pcb table
struct PCB_table{
    int8_t id;                 // 1 byte
    int8_t parent_id;            // 1 byte
    int32_t saved_esp;          // 4 byte
    int32_t saved_ebp;          // 4 byte
    int8_t active;              // 1 byte
    int8_t fdt_usage; //00000011// 1 byte
    struct file_descriptor fdt[8]; // 16 byte each
    int8_t arg[128];
};

//the function that represents the system call handler
extern int system_call_handler();

//function that initializes the file descriptor
extern void fd_init();

//function that halts the system
extern int system_halt(uint8_t status);

//function that executes the system
extern int system_execute(const uint8_t* command);

//function for the reading of the system
extern int system_read(int32_t fd, void* buf, int32_t nbytes); 

//function for the writing of the system
extern int system_write(int32_t fd, const void* buf, int32_t nbytes); 

//function for the opening of the system
extern int system_open(const uint8_t* filename); 

//function for the closing of the system
extern int system_close(int32_t fd);

//function that gets the arguments for the system
extern int system_getargs(uint8_t* buf, int32_t nbytes); 

//function separate for video memory
extern int system_vidmap(uint8_t** screen_start);

//function that gets the pcb pointer
extern int get_pcb_pointer();

//function that checks if the file descriptor is in use
int check_fd_in_use(int32_t fd);

//function that gets the pid
extern int get_pid();

//function that prepares the IRET
extern void IRET_prepare(int);

//function for an extra label
extern void label();

extern int find_next_pid();
extern int get_display_terminal();
extern int get_main_terminal();
extern void schedule();
extern int video_map_switch();
extern int switch_terminal(int next_display_terminal);
extern void flush_tlb();
extern int get_executing_status(int terminal_);
#endif
