#include "system_call.h"
#include "file_sys_driver.h"
#include "file_sys.h"
#include "RTC.h"
#include "Terminal.h"
#include "lib.h"

int pid,processor_usage;
int phy_mem_loc;

struct files_command{
    int *open(uint8_t*);
    int *close(uint32_t);
    int *write(int32_t,void*,int32_t);
    int *read(int32_t,void*,int32_t);
};
struct PCB_table{
    int pid;
    int parent_id
    struct file_descriptor fdt[8];
    int saved_esp;
    int saved_ebp;
    int active;
    int fdt_usage;
};
struct files_command file_handler[4];

void fd_init(){
    pid=0;
    processor_usage=0;
    phy_mem_loc=8;  // start address in physical mem to store files  8kb
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
    int re;
    char buf[100000];
    struct dentry dt;
    struct PCB_table pcb_t;
    //Parse args

    //Check for executable
    re=read_dentry_by_name(command,dt);
    if(re==-1)return -1;
    re=read_data(dt.inode_num, 0, buf, 4);
    if(buf[0]==exe_0 && buf[1]==exe_1 && buf[2]==exe_2 && buf[3]==exe_3);
    else return -1;

    //Set up paging
    set_new_page(phy_mem_loc);
    phy_mem_loc+=4;

    //Losd file into memory
    re=read_data(dt.inode_num, 0, 0x08048000, get_length(dt));
    
    //Create PCB
    pcb_t.parent_id=pid;
    pcb_t.pid=pid++;
    // pcb_t.fdt[8]         // initialize the stdin and stdout
    pcb_t.saved_esp=;       // what should be saved here 
    pcb_t.saved_ebp=;       // what should be saved here
    pcb_t.active=1;
    pcb_t.fdt_usgae=3;
    
    //Prepare for Context Switch
    //Push IRET context to kernel stack
    //IRET
    //return;
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
