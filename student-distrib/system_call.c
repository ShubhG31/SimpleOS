#include "system_call.h"
#include "file_sys_driver.h"
#include "file_sys.h"
#include "RTC.h"
#include "Terminal.h"
#include "lib.h"
#include "x86_desc.h"
#include "paging.h"

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
typedef int32_t (*open_type)(uint8_t*);
typedef int32_t (*close_type)(uint32_t);
typedef int32_t (*write_type)(int32_t,void*,int32_t);
typedef int32_t (*read_type)(int32_t,void*,int32_t);
// struct file_descriptor{
//     uint32_t opt_table_pointer;
//     uint32_t inode;
//     uint32_t file_pos;
//     uint32_t flags;
// };
struct files_command{
    open_type open;
    close_type close;
    write_type write;
    read_type read;

};
// struct PCB_table{
//     int8_t id;                 // 1 byte
//     int8_t parent_id            // 1 byte
//     int32_t saved_esp;          // 4 byte
//     int32_t saved_ebp;          // 4 byte
//     int8_t active;              // 1 byte
//     int8_t fdt_usage; //00000011// 1 byte
//     struct file_descriptor fdt[8]; // 16 byte each
// };
struct files_command file_handler[4];
struct PCB_table* pcb_t;
struct PCB_table pcb_box;
struct file_descriptor fd_box;

void fd_init(){         // need to be run after booting part
    last_pid=-1;
    pid=0;
    processor_usage=0;
    phy_mem_loc=8;  // start address in physical mem to store files  8MB
    // RTC
    file_handler[0].open = (open_type)&RTC_open;
    file_handler[0].close = (close_type)&RTC_close;
    file_handler[0].read = (read_type)&RTC_read;
    file_handler[0].write = (write_type)&RTC_write;
    // file directory
    file_handler[1].open = (open_type)&dir_open;
    file_handler[1].close = (close_type)&dir_close;
    file_handler[1].read = (read_type)&dir_read;
    file_handler[1].write = (write_type)&dir_write;
    // file file
    file_handler[2].open = (open_type)&file_open;
    file_handler[2].close = (close_type)&file_close;
    file_handler[2].read = (read_type)&file_read;
    file_handler[2].write = (write_type)&file_write;
    // terminal
    file_handler[3].open = (open_type)&terminal_read;
    file_handler[3].close = (close_type)&terminal_close;
    file_handler[3].read = (read_type)&terminal_read;
    file_handler[3].write = (write_type)&terminal_write;
    return;
}
int system_halt(uint8_t status){
    //remeber to clear the paging.

    // clear the page that was used for now complete process

    // clear tlb
    asm volatile(
        "movl %cr3, %edx\n"
        "movl %edx, %cr3\n"
    );

    // load the parent task 

    return 1;
}
int system_execute(const uint8_t* command){
    int re;
    char buf[40];
    struct dentry dt;

    last_pid=pid;
    char *buffer = 0x08048000 + (0x8<<20)*pid ;
    pid++;
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    //Parse args

    //Check for executable
    re=read_dentry_by_name(command,&dt);
    if(re==-1)return -1;
    re=read_data(dt.inode_num, 0, buf, 40);
    if(buf[0]==exe_0 && buf[1]==exe_1 && buf[2]==exe_2 && buf[3]==exe_3);
    else return -1;

    //Set up paging

    int eip = (buf[27]<<24)|(buf[26]<<16)|(buf[25]<<8)|(buf[24]);

    // page fault *****
    set_new_page(phy_mem_loc);
    phy_mem_loc+=4;
    
    // clear tlb
    asm volatile(
        "movl %cr3, %edx \n"
        "movl %edx, %cr3 \n"
    );
    //Load file into memory
    re=read_data((uint32_t)(dt.inode_num), (uint32_t)0, (uint8_t*)(0x08048000), (uint32_t)5605);//(uint32_t)get_length(dt));
    int fde;
    fde=file_open(command);
    // fd=file_open("hello");
    // fd=file_open("fish");       //dont exist
    // puts("return value of file_open:");
    // put_number(fd);
    // putc('\n');
    if(fde==-1){
        puts("\n Cannot find the file \n");
        return -1;
    }
    // char buf[60000];  
    // char *buffer = 0x08048000 + (0x8<<20)*pid ;
    re=file_read(fde,(void*)buffer,60000);

    
    //Create PCB
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    pcb_box.parent_id=last_pid;
    pcb_box.id=pid;
    pcb_box.saved_esp=saved_esp;              // what should be saved here
    pcb_box.saved_ebp=saved_ebp;              // what should be saved here
    pcb_box.active=1;
    pcb_box.fdt_usage=3; //00000011
    
    fd_box.opt_table_pointer=(uint32_t)&file_handler[3];
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
    // *((int32_t*)(pcb_t))=pcb_box;
    *(pcb_t)=pcb_box;
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
    // IRET_prepare(EIP);

    // // clear tlb
    // asm volatile(
    //     "movl %cr3, %edx \n"
    //     "movl %edx, %cr3 \n"
    // );

    // DS
    // esp  calculated through the 2^20 * 132 = 138412032 to hex is 0x08400000 and -4 of that is 0x083FFFFC
    // eflags 
    // cs
    // eip 
    // asm volatile ("pushl $0x002B\n" 
    //     "pushl $0x083FFFFC\n"
    //     "pushfl\n"
    //     "popl %ebx\n"
    //     "orl $0x0200, %ebx\n"
    //     "pushl %ebx\n"
    //     "pushl $0x0023\n"
    //     "pushl $0x08048040\n"      
    //     "iret \n"
    // );
    IRET_prepare(eip);
      // "pushl 0x800000\n"  
    //return;
    // system_halt();
    return 1;
}
int system_read(int32_t fd, void* buf, int32_t nbytes){
    int re;
    if(check_fd_in_use(fd)==0)return -1;
    // pcb_t=get_pcb_pointer();
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    pcb_box=*(pcb_t);
    fd_box=pcb_t->fdt[fd];
    // re=(((read_type)(((struct files_command*)fd_box)->opt_table_pointer))->read)(&fd_box,buf,nbytes);
    re = (read_type)(((struct files_command*)(fd_box.opt_table_pointer))->read)((int32_t)&fd_box,(void*)buf,(int32_t)nbytes);
    if(re!=-1){
        fd_box.file_pos+=re;
        *((struct file_descriptor*)(pcb_t+pcb_fd_off+fd*fd_size))=fd_box;
    }
    return re;
}
int system_write(int32_t fd, const void* buf, int32_t nbytes){
    int re;
    if(check_fd_in_use(fd)==0)return -1;
    // pcb_t=get_pcb_pointer();
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    pcb_box=*pcb_t;
    fd_box=pcb_t->fdt[fd];
    // re=*(*(struct files_command*)fd_box.opt_table_pointer->write)(&fd_box,buf,nbytes);
    re = (write_type)(((struct files_command*)fd_box.opt_table_pointer)->write)((int32_t)&fd_box,(void*)buf,(int32_t)nbytes);
    return re;
}
int system_open(const uint8_t* filename){
    int re, fd,mask ;
    mask = 0x80;    //mask used to check bits left to right
    struct dentry file;
    int * file_open;
    int32_t inode;                  //inode 4B
    if(filename==NULL||strlen((int8_t)filename)>name_length)return -1;
    re=read_dentry_by_name (filename,(&file));
    if(re==-1)return -1;        // reading fails, so we return -1
    
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;

    for( fd = 2; fd <= PCB_size; fd++){
        if(((1<<fd)&pcb_box.fdt_usage)==0)break;
        puts("no space in file descriptor array");
        if(fd==PCB_size)return -1;
    }

    fd_box=pcb_t->fdt[fd];
    
    if(file.filetype == 2){
        inode = file.inode_num;
    }else{
        inode = 0;
    }
    
    file_open = file_handler[file.filetype].open;
    
    fd_box.opt_table_pointer = (uint32_t)file_open;        // pointer to the function?
    fd_box.inode = inode;   // we have only one directory, its inode is 0
    fd_box.file_pos = 0;    // start with offset at 0
    fd_box.flags = 1;
    pcb_box.fdt[fd]=fd_box;
    pcb_box.fdt_usage = pcb_box.fdt_usage | (1<<fd);
    // *((int32_t*)(pcb_t))=pcb_box;
    *pcb_t = pcb_box;

    (file_handler[file.filetype].open)(filename);
    // use[head]=1;    // set this fd this in use
    return fd;
} 
int system_close(int32_t fd){
    if(fd==0||fd==1)return -1;
    if(check_fd_in_use(fd)==0)return -1;
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;
    fd_box=pcb_t->fdt[fd];
    fd_box.opt_table_pointer=NULL;
    fd_box.inode=-1;
    fd_box.file_pos=0;
    fd_box.flags=0;
    *((struct file_descriptor*)(pcb_t+pcb_fd_off+fd*fd_size))=fd_box;
    pcb_box.fdt_usage^=(1<<fd);
    return 0;
}
int system_getargs(uint8_t* buf, int32_t nbytes){
    return 1;
} 
int system_vidmap(uint8_t** screen_start){
    return 1;
}
int check_fd_in_use(int32_t fd){
    pcb_t=get_pcb_pointer();
    pcb_box=*pcb_t;
    if(((1<<fd)&pcb_box.fdt_usage)!=0)return 1;
    return 0;
}
int get_pcb_pointer(){
    return addr_8MB-size_8kb*pid;
}
