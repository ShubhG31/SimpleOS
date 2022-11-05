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
    read_type read;
    write_type write;
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
static struct files_command RTC_commands={(open_type)&RTC_open, (close_type)&RTC_close, (read_type)&RTC_read, (write_type)&RTC_write};
static struct files_command dir_commands={(open_type)&dir_open, (close_type)&dir_close, (read_type)&dir_read, (write_type)&dir_write};
static struct files_command file_commands={(open_type)&file_open, (close_type)&file_close, (read_type)&file_read, (write_type)&file_write};
static struct files_command terminal_commands={(open_type)&terminal_open, (close_type)&terminal_close, (read_type)&terminal_read, (write_type)&terminal_write};

struct PCB_table* pcb_t;
struct PCB_table pcb_box;
struct file_descriptor fd_box;

int get_pid(){
    return pid;
}
void fd_init(){         // need to be run after booting part
    last_pid=-1;
    pid=-1;
    processor_usage=0;
    phy_mem_loc=8;  // start address in physical mem to store files  8MB
    return;
}
int system_halt(uint8_t status){
    //remember to clear the paging.
    int i,saved_ebp,saved_esp;
    // clear the page that was used for now complete process
    if(pid==0)return -1;
    // clearing process
    pcb_t=(struct PCB_table*)get_pcb_pointer();

    
    // update pid
    pid=pcb_t->parent_id;

    pcb_box=*pcb_t;
    pcb_box.active=0;
    pcb_box.fdt_usage=0;
    pcb_box.id=0;
    pcb_box.parent_id=0;
    saved_ebp=pcb_box.saved_ebp;
    saved_esp=pcb_box.saved_esp;
    // put_number(saved_ebp);putc('\n');
    // put_number(saved_esp);putc('\n');
    // while(1);
    pcb_box.saved_ebp=0;
    pcb_box.saved_esp=0;
    for(i=0;i<7;i++){
        (pcb_box.fdt[i]).opt_table_pointer=NULL;
        (pcb_box.fdt[i]).inode=0;
        (pcb_box.fdt[i]).file_pos=0;
        (pcb_box.fdt[i]).flags=0;
    }
    *pcb_t=pcb_box;
    // clearing paging (remapping)
    phy_mem_loc-=4;
    set_new_page(phy_mem_loc-4);
    
    
    // clear tlb
    asm volatile(
        "movl %cr3, %edx\n"
        "movl %edx, %cr3\n"
    );

    // load the parent task 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = 0x800000 - 0x2000*(pid) - 4; //8mb-8kb 

    // puts(" i fucking finish everything before here\n");
    // restore_esp_ebp(saved_ebp,saved_esp);
    // while(1);
    // label();
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "leave;"
        "ret;"
        :
        :"r"(saved_esp), "r"(saved_ebp)
    );
    // register uint32_t saved_ebp_ asm("ebp");
    // register uint32_t saved_esp_ asm("esp");
    // put_number(saved_ebp_);putc('\n');
    // put_number(saved_esp_);putc('\n');
    // asm volatile(
    //     "jmp label\n"
    // );
    // while(1);
    return 1;
}
int system_execute(const uint8_t* command){
    int re;
    uint8_t buf[40];
    struct dentry dt;

    // puts("we are running:");
    // int i;
    // for(i=0;i<10;i++)putc(*(command+i));putc('\n');
    // puts((int8_t*)command);
    // putc('\n');
    // if(*((uint8_t*)command)!='s')while(1);

    //Parse args

    //Check for executable
    re=read_dentry_by_name(command,&dt);

    if(re==-1)return -1;
    re=read_data(dt.inode_num, 0, buf, 40);
    if(buf[0]==exe_0 && buf[1]==exe_1 && buf[2]==exe_2 && buf[3]==exe_3);
    else return -1;

    last_pid=pid;
    // char *buffer = 0x08048000 + (0x8<<20)*pid ; //
    // char *buffer = (char*)0x08048000; //
    pid++;
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    
    //Set up paging

    int eip = (buf[27]<<24)|(buf[26]<<16)|(buf[25]<<8)|(buf[24]);

    set_new_page(phy_mem_loc);
    phy_mem_loc+=4;
    
    // clear tlb
    asm volatile(
        "movl %cr3, %edx \n"
        "movl %edx, %cr3 \n"
    );
    //Load file into memory
    re=read_data((uint32_t)(dt.inode_num), (uint32_t)0, (uint8_t*)(0x08048000), (uint32_t)get_length(dt));//;
    
    //Create PCB
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");
    // puts("fuckit:\n");
    // put_number(saved_ebp);putc('\n');
    // put_number(saved_esp);putc('\n');
    pcb_box.parent_id=last_pid;
    pcb_box.id=pid;
    pcb_box.saved_esp=saved_esp;              // what should be saved here
    pcb_box.saved_ebp=saved_ebp;              // what should be saved here
    pcb_box.active=1;
    pcb_box.fdt_usage=3; //00000011
    
    fd_box.opt_table_pointer=(uint32_t)&terminal_commands;
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

    
    // re=read_data((uint32_t)(dt.inode_num), (uint32_t)0, (uint8_t*)(0x08048000), (uint32_t)5605);//(uint32_t)get_length(dt));
    // int fde;
    // fde=file_open(command);
    // fd=file_open("hello");
    // fd=file_open("fish");       //dont exist
    // puts("return value of file_open:");
    // put_number(fd);
    // putc('\n');
    // if(fde==-1){
    //     puts("\n Cannot find the file \n");
    //     return -1;
    // }
    // char buf[60000];  
    // char *buffer = 0x08048000 + (0x8<<20)*pid ;
    // re=file_read(fde,(void*)buffer,60000);

    //Prepare for Context Switch
    tss.ss0 = KERNEL_DS;
    tss.esp0 = 0x800000 - 0x2000*(pid) - 4; //8mb-8kb 

    /* everythinint i;
    char file_name[32]="frame0.txt";
    char buff[100];
    put_number((int)(&file_commands));
    putc('\n');
    re=system_open((uint8_t*)file_name);
    put_number(re);
    putc('\n');
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    puts("inode: ");
    put_number(pcb_t->fdt[re].inode);
    putc('\n');
    puts("fdt_usage: ");
    put_number(pcb_t->fdt_usage);
    putc('\n');
    puts("command: ");
    put_number(pcb_t->fdt[re].opt_table_pointer);
    putc('\n');

    re=system_read(re,(void*)buff,99);
    put_number(re);
    putc('\n');
    for(i=0;i<re;i++)putc(buff[i]);
    putc('\n');
    puts(" i fucking finish everything before here\n");

    re=system_write(re,(void*)buff,99);
    put_number(re);
    putc('\n');

    re=system_close(2);
    put_number(re);
    putc('\n');
    puts("inode: ");
    put_number(pcb_t->fdt[2].inode);
    putc('\n');
    puts("fdt_usage: ");
    put_number(pcb_t->fdt_usage);
    putc('\n');
    puts("command: ");
    put_number(pcb_t->fdt[2].opt_table_pointer);
    putc('\n');
    while(1);*/
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
    IRET_prepare(eip);          //eip address may change, may need to modify it
      // "pushl 0x800000\n"  
    puts("fuckfuckfuckfuckfuckfuckfuckfuckfuckfuck\n");
    while(1);
    return 0;
}
int system_read(int32_t fd, void* buf, int32_t nbytes){
    int re;
    re=0;
    if(fd==1)return -1; //stdout does not handle terminal read
    if(check_fd_in_use(fd)==0)return -1;
    // pcb_t=get_pcb_pointer();
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    fd_box=pcb_t->fdt[fd];

    // put_number(re);
    // putc('\n');
    re = ((((struct files_command*)(fd_box.opt_table_pointer))->read)((int32_t)fd,(void*)buf,(int32_t)nbytes));
    // put_number(re);
    // putc('\n');
    // puts(" i fucking finish everything before here\n");
    // int i;
    // put_number(re);
    // puts("FFFF:");
    // for(i=0;i<re;i++){
    //     putc(*(uint8_t*)(buf+i));
    // }putc('\n');
    if(re!=-1){
        fd_box.file_pos+=re;
        pcb_t->fdt[fd].file_pos+=re;
    }
    return re;
}
int system_write(int32_t fd, const void* buf, int32_t nbytes){
    // puts(" i fucking finish everything before here\n");
    int re;
    if(fd==0)return -1; //stdin does not handle terminal write
    if(check_fd_in_use(fd)==0)return -1;
    // pcb_t=get_pcb_pointer();
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    fd_box=pcb_t->fdt[fd];
    // re=*(*(struct files_command*)fd_box.opt_table_pointer->write)(&fd_box,buf,nbytes);
    re = ((((struct files_command*)fd_box.opt_table_pointer)->write)((int32_t)fd,(void*)buf,(int32_t)nbytes));
    return re;
}
int system_open(const uint8_t* filename){
    int re, fd ;
    struct dentry file;
    // int32_t inode;                  //inode 4B
    if(filename==NULL||strlen((int8_t*)filename)>name_length)return -1;
    re=read_dentry_by_name (filename,(&file));
    if(re==-1)return -1;        // reading fails, so we return -1
    
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    pcb_box=*pcb_t;

    for( fd = 2; fd <= PCB_size; fd++){
        if(((1<<fd)&pcb_box.fdt_usage)==0)break;
        if(fd==PCB_size){
            puts("no space in file descriptor array");
            return -1;
        }
    }

    
    if (file.filetype == 0) re=((RTC_commands.open)((uint8_t*)filename));
    else if (file.filetype == 1) re=((dir_commands.open)((uint8_t*)filename));
    else if (file.filetype == 2) re=((file_commands.open)((uint8_t*)filename));
    if(re==-1)return -1;

    fd_box=pcb_t->fdt[fd];
    
    // if(file.filetype == 2){
    //     inode = file.inode_num;
    // }else{
    //     inode = 0;
    // }
    if (file.filetype == 0) fd_box.opt_table_pointer = (uint32_t) &RTC_commands;
    else if (file.filetype == 1) fd_box.opt_table_pointer = (uint32_t) &dir_commands;
    else if (file.filetype == 2) fd_box.opt_table_pointer = (uint32_t) &file_commands;

    fd_box.inode =  file.inode_num;   // we have only one directory, its inode is 0
    fd_box.file_pos = 0;    // start with offset at 0
    fd_box.flags = 1;
    pcb_box.fdt[fd]=fd_box;
    pcb_box.fdt_usage = pcb_box.fdt_usage | (1<<fd);
    // *((int32_t*)(pcb_t))=pcb_box;
    *pcb_t = pcb_box;

    return fd;
} 
int system_close(int32_t fd){
    int re;
    if(fd==0||fd==1)return -1;
    if(check_fd_in_use(fd)==0)return -1;
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    pcb_box=*pcb_t;
    pcb_box.fdt[fd].opt_table_pointer=NULL;
    pcb_box.fdt[fd].inode=-1;
    pcb_box.fdt[fd].file_pos=0;
    pcb_box.fdt[fd].flags=0;
    pcb_box.fdt_usage^=(1<<fd);
    *(pcb_t)=pcb_box;
    re = ((((struct files_command*)fd_box.opt_table_pointer)->close)((int32_t)fd));
    return 0;
}
int system_getargs(uint8_t* buf, int32_t nbytes){
    return 1;
} 
int system_vidmap(uint8_t** screen_start){
    return 1;
}
int check_fd_in_use(int32_t fd){
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    pcb_box=*pcb_t;
    if(((1<<fd)&pcb_box.fdt_usage)!=0)return 1;
    return 0;
}
int get_pcb_pointer(){
    return addr_8MB-size_8kb*(pid+1);       // pid starts at 0 (0 -> move 1*8kb, 1-> move 2*8kb)
}
