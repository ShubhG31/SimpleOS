#include "types.h"


extern int system_call_handler();

extern int system_halt(uint8_t status);
extern int system_execute(const uint8_t* command); 
extern int system_read(int32_t fd, void* buf, int32_t nbytes); 
extern int system_write(int32_t fd, const void* buf, int32_t nbytes); 
extern int system_open(const uint8_t* filename); 
extern int system_close(int32_t fd);
extern int system_getargs(uint8_t* buf, int32_t nbytes); 
extern int system_vidmap(uint8_t** screen_start);