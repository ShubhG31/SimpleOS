#include "system_call.h"
#include "file_sys_driver.h"
#include "RTC.h"
#include "Terminal.h"
struct files_command{
    int *open(uint8_t*);
    int *close(uint32_t);
    int *write(int32_t,void*,int32_t);
    int *read(int32_t,void*,int32_t);
};
struct files_command file_handler[4];

void fd_init(){
    // RTC
    file_handler[0].open=&RTC_open;
    file_handler[0].close=&RTC_close;
    file_handler[0].read=&RTC_read;
    file_handler[0].write=&RTC_write;
    // file directory
    file_handler[1].open=&dir_open;
    file_handler[1].close=&dir_close;
    file_handler[1].read=&dir_read;
    file_handler[1].write=&dir_write;
    // file file
    file_handler[2].open=&file_open;
    file_handler[2].close=&file_close;
    file_handler[2].read=&file_read;
    file_handler[2].write=&file_write;
    // terminal
    file_handler[3].open=&terminal_read;
    file_handler[3].close=&terminal_close;
    file_handler[3].read=&terminal_read;
    file_handler[3].write=&terminal_write;
    return;
}
extern int system_halt(uint8_t status){

}
extern int system_execute(const uint8_t* command){

}
extern int system_read(int32_t fd, void* buf, int32_t nbytes){

}
extern int system_write(int32_t fd, const void* buf, int32_t nbytes){

}
extern int system_open(const uint8_t* filename){

} 
extern int system_close(int32_t fd){

}
extern int system_getargs(uint8_t* buf, int32_t nbytes){

} 
extern int system_vidmap(uint8_t** screen_start){

}
