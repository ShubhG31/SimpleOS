#include "system_call.h"
#include "file_sys.h"
#include "file_sys_driver.h"
#include "RTC.h"
#include "Terminal.h"

#define PCB_size 8

struct files_command{
    int *open(uint8_t*);
    int *close(uint32_t);
    int *write(int32_t,void*,int32_t);
    int *read(int32_t,void*,int32_t);
};

struct files_command file_handler[4];
struct file_descriptor F_D[PCB_size];

struct PCB_table{
    int pid;
    int parent_id
    struct file_descriptor fdt[8];
    int saved_esp;
    int saved_ebp;
    int active;
    int16_t fdt_usage;
};

int use[FD_num], head, mask;
mask = 0x80;    //mask used to check bits left to right
fdt_usage = 0;
fdt_usage = fdt_usage || mask;
mask = mask >> 1;
fdt_usage = fdt_usage || mask;
// use[0] = 1;
// use[1] = 1;

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
    file_handler[3].open=&terminal_open;
    file_handler[3].close=&terminal_close;
    file_handler[3].read=&terminal_read;
    file_handler[3].write=&terminal_write;
    return;
}

fd_init();
F_D[0].opt_table_pointer = (uint32_t)file_handler[3].read;                                       // pointer to the function?
F_D[0].inode=0;             // we have only one directory, its inode is 0
F_D[0].file_pos=0;          // start with offset at 0
F_D[0].flags=1;

F_D[1].opt_table_pointer = (uint32_t)file_handler[3].write;                                       // pointer to the function?
F_D[1].inode=0;             // we have only one directory, its inode is 0
F_D[1].file_pos=0;          // start with offset at 0
F_D[1].flags=1;


extern int system_halt(uint8_t status){

}
extern int system_execute(const uint8_t* command){

}
extern int system_read(int32_t fd, void* buf, int32_t nbytes){

}
extern int system_write(int32_t fd, const void* buf, int32_t nbytes){
    // if(fd == 0 || fd == 1){
    //     return -1;
    // }else{
    //     use[head] = 0;
    //     return 0;
    // }
}
extern int system_open(const uint8_t* filename){
    int re;
    int temp;
    temp = 0x1;
    mask = 0x80;    //mask used to check bits left to right
    struct dentry file;
    int * file_open;
    int32_t inode;                  //inode 4B
    re=read_dentry_by_name (filename,(&file));
    if(re==-1)return -1;        // reading fails, so we return -1
    if(file.filetype == 2){
        inode = file.inode_num;
    }else{
        inode = 0;
    }
    file_open = file_handler[file.filetype].open;
    for( head = 0; head < PCB_size+1 && (1 & (fdt_usage >> (head + 8))) head++);
    if(head == 8){
        putc('no space in file descriptor array');
        return -1;
    }else{
        F_D[head].opt_table_pointer = (uint32_t)file_open;                                       // pointer to the function?
        F_D[head].inode=inode;   // we have only one directory, its inode is 0
        F_D[head].file_pos=0;    // start with offset at 0
        F_D[head].flags=1;
        temp = temp >> head;
        fdt_usage = fdt_usage || temp;
        // use[head]=1;    // set this fd this in use
    }
    return head;
} 
extern int system_close(int32_t fd){
    if(fd == 0 || fd == 1){
        return -1;
    }else{
        mask = 0xDF;    //mask used to check bits left to right
        int i;
        for (i = 0; i < fd-3; i++) {
            mask = mask >> 1;
        }
        fdt_usage = fdt_usage & mask;
        // use[fd] = 0;
        return 0;
    }
}
extern int system_getargs(uint8_t* buf, int32_t nbytes){

} 
extern int system_vidmap(uint8_t** screen_start){

}
