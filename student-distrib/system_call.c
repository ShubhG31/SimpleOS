#include "system_call.h"
#include "file_sys_driver.h"
#include "file_sys.h"
#include "RTC.h"
#include "Terminal.h"
#include "lib.h"

#define addr_8MB 0x800000  //8*1024*1024
#define size_8kb 0x2000 //8*1024
#define pcb_pid_off 0
#define pcb_parid_off 1
#define pcb_saved_esp_off 2
#define pcb_saved_ebp_off 6
#define pcb_active_off 10
#define pcb_fdt_usage_off 11
#define pcb_fd_off 12
#define fd_size 16
#define PCB_size 8

int pid,last_pid,processor_usage;
int phy_mem_loc;

struct file_descriptor{
    uint32_t opt_table_pointer;
    uint32_t inode;
    uint32_t file_pos;
    uint32_t flags;
};
struct files_command{
    int32_t *open(uint8_t*);
    int32_t *close(uint32_t);
    int32_t *write(int32_t,void*,int32_t);
    int32_t *read(int32_t,void*,int32_t);
};
struct PCB_table{
    int8_t id;                 // 1 byte
    int8_t parent_id            // 1 byte
    int32_t saved_esp;          // 4 byte
    int32_t saved_ebp;          // 4 byte
    int8_t active;              // 1 byte
    int8_t fdt_usage; //00000011// 1 byte
    struct file_descriptor fdt[8]; // 16 byte each
};
struct files_command file_handler[4]=;
struct PCB_table* pcb_t;
struct PCB_table pcb_box;
struct file_descriptor fd_box;

void fd_init(){         // need to be run after booting part
    last_pid=-1;
    pid=0;
    processor_usage=0;
    phy_mem_loc=8;  // start address in physical mem to store files  8MB
    // RTC
    file_handler[0].open = &RTC_open;
    file_handler[0].close = &RTC_close;
    file_handler[0].read = &RTC_read;
    file_handler[0].write = &RTC_write;
    // file directory
    file_handler[1].open = &dir_open;
    file_handler[1].close = &dir_close;
    file_handler[1].read = &dir_read;
    file_handler[1].write = &dir_write;
    // file file
    file_handler[2].open = &file_open;
    file_handler[2].close = &file_close;
    file_handler[2].read = &file_read;
    file_handler[2].write = &file_write;
    // terminal
    file_handler[3].open = &terminal_read;
    file_handler[3].close = &terminal_close;
    file_handler[3].read = &terminal_read;
    file_handler[3].write = &terminal_write;
    return;
}
extern int system_halt(uint8_t status){
    //remeber to clear the paging.

    // clear the page that was used for now complete process

    // clear tlb
    asm volatile(
        "movl %cr3, %edx"
        "movl %edx, %cr3"
    )

    // load the parent task 


}
extern int system_execute(const uint8_t* command){
    int re;
    char buf[4];
    struct dentry dt;

    last_pid=pid;
    pid++;
    pcb_t=get_pcb_pointer();
    //Parse args

    //Check for executable
    re=read_dentry_by_name(command,&dt);
    if(re==-1)return -1;
    re=read_data(dt.inode_num, 0, buf, 4);
    if(buf[0]==exe_0 && buf[1]==exe_1 && buf[2]==exe_2 && buf[3]==exe_3);
    else return -1;

    //Set up paging
    set_new_page(phy_mem_loc);
    phy_mem_loc+=4;

    //Losd file into memory
    re=read_data(dt.inode_num, 0, (uint8_t*)(0x08048000), get_length(dt));
    
    //Create PCB
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    pcb_box.parent_id=last_pid;
    pcb_box.id=pid;
    pcb_box.saved_esp=saved_esp;              // what should be saved here
    pcb_box.saved_ebp=saved_ebp;              // what should be saved here
    pcb_box.active=1;
    pcb_box.fdt_usage=3; //00000011
    
    fd_box.opt_table_pointer=&file_handler[3];
    fd_box.inode=-1;
    fd_box.file_pos=0;
    fd_box.flags=1;         // in use or not
    pcb_box.fdt[0]=fd_box;
    pcb_box.fdt[1]=fd_box;
    fd_box.opt_table_pointer=NULL;
    fd_box.inode=-1;
    fd_box.file_pos=0;
    fd_box.flags=0;
    pcb_box.fdt[2]=fd_box;
    pcb_box.fdt[3]=fd_box;
    pcb_box.fdt[4]=fd_box;
    pcb_box.fdt[5]=fd_box;
    pcb_box.fdt[6]=fd_box;
    pcb_box.fdt[7]=fd_box;
    *((int32_t*)(pcb_t))=pcb_box;
    // *((int32_t*)(pcb_t+pcb_parid_off))=pid;
    // *((int32_t*)(pcb_t+pcb_pid_off))=pid++;
    // *((int32_t*)(pcb_t+pcb_saved_esp_off))=;       // what should be saved here 
    // *((int32_t*)(pcb_t+pcb_saved_ebp_off))=;       // what should be saved here 
    // *((int32_t*)(pcb_t+pcb_active_off))=1;
    // *((int32_t*)(pcb_t+pcb_fdt_usage_off))=3;       //00000011
    // *((struct file_descriptor*)(pcb_t+pcb_fd_off))=
    // // *((int32_t*)(pcb_t+))

    //Prepare for Context Switch
    tss.ss0 = KERNEL_DS;
    tss.esp0 = 0x800000 - 0x2000*pid - 4; //8mb-8kb 

    //Push IRET context to kernel stack

    //IRET
    IRET_prepare(EIP);
    
    //return;
    // system_halt();
}
extern int system_read(int32_t fd, void* buf, int32_t nbytes){
    int re;
    if(check_fd_in_use(fd)==0)return -1;
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;
    fd_box=pcb_t.fdt[fd];
    re=*(*(struct files_command*)(fd_box.opt_table_pointer).read)(&fd_box,buf,nbytes);
    if(re!=-1){
        fd_box.file_pos+=re;
        *((struct file_descriptor*)(pcb_t+pcb_fd_off+fd*fd_size))=fd_box;
    }
    return re;
}
extern int system_write(int32_t fd, const void* buf, int32_t nbytes){
    int re;
    if(check_fd_in_use(fd)==0)return -1;
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;
    fd_box=pcb_t.fdt[fd];
    re=*(*(struct files_command*)(fd_box.opt_table_pointer).write)(&fd_box,buf,nbytes);
    return re;
}
extern int system_open(const uint8_t* filename){
    int re, fd ;
    mask = 0x80;    //mask used to check bits left to right
    struct dentry file;
    int * file_open;
    int32_t inode;                  //inode 4B
    if(filename==NULL||strlen(filename)>name_length)return -1;
    re=read_dentry_by_name (filename,(&file));
    if(re==-1)return -1;        // reading fails, so we return -1
    
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;

    for( fd = 2; fd <= PCB_size; fd++){
        if(((1<<fd)&pcb_box.fdt_usage)==0)break;
        puts('no space in file descriptor array');
        if(fd==PCB_size)return -1;
    }

    fd_box=pcb_t.fdt[fd];
    
    if(file.filetype == 2){
        inode = file.inode_num;
    }else{
        inode = 0;
    }
    
    file_open = file_handler[file.filetype];
    
    fd_box.opt_table_pointer = (uint32_t)file_open;        // pointer to the function?
    fd_box.inode = inode;   // we have only one directory, its inode is 0
    fd_box.file_pos = 0;    // start with offset at 0
    fd_box.flags = 1;
    pcb_box.fdt[fd]=fd_box;
    pcb_box.fdt_usage = pcb_box.fdt_usage | (1<<fd);
    *((int32_t*)(pcb_t))=pcb_box;

    *(file_handler[file.filetype].open)(filename);
    // use[head]=1;    // set this fd this in use
    return fd;
} 
extern int system_close(int32_t fd){
    if(fd==0||fd==1)return -1;
    if(check_fd_in_use(fd)==0)return -1;
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;
    fd_box=pcb_t.fdt[fd];
    fd_box.opt_table_pointer=NULL;
    fd_box.inode=-1;
    fd_box.file_pos=0;
    fd_box.flags=0;
    *((struct file_descriptor*)(pcb_t+pcb_fd_off+fd*fd_size))=fd_box;
    pcb_box.fdt_usage^=(1<<fd);
    return 0;
}
extern int system_getargs(uint8_t* buf, int32_t nbytes){

} 
extern int system_vidmap(uint8_t** screen_start){

}
int check_fd_in_use(int32_t fd){
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;
    if(((1<<fd)&pcb_box.fdt_usage)!=0)return 1;
    return 0;
}
extern int get_pcb_pointer(){
    return addr_8MB-size_8kb*pid;
}
