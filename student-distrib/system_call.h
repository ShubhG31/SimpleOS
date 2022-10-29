#include "types.h"
#define exe_0 0x7f
#define exe_1 0x45
#define exe_2 0x4c
#define exe_3 0x46


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
extern int get_fd_pointer();