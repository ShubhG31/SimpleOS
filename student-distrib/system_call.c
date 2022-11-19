#include "system_call.h"
#include "file_sys_driver.h"
#include "file_sys.h"
#include "RTC.h"
#include "Terminal.h"
#include "lib.h"
#include "x86_desc.h"
#include "paging.h"
#include "i8259.h"

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

int pid, last_pid, processor_usage;
int phy_mem_loc, main_terminal, display_terminal, flag_open_three_shell;

typedef int32_t (*open_type)(uint8_t*);
typedef int32_t (*close_type)(uint32_t);
typedef int32_t (*write_type)(int32_t,void*,int32_t);
typedef int32_t (*read_type)(int32_t,void*,int32_t);

struct files_command{
    open_type open;
    close_type close;
    read_type read;
    write_type write;
};

static struct files_command RTC_commands={(open_type)&RTC_open, (close_type)&RTC_close, (read_type)&RTC_read, (write_type)&RTC_write};
static struct files_command dir_commands={(open_type)&dir_open, (close_type)&dir_close, (read_type)&dir_read, (write_type)&dir_write};
static struct files_command file_commands={(open_type)&file_open, (close_type)&file_close, (read_type)&file_read, (write_type)&file_write};
static struct files_command terminal_commands={(open_type)&terminal_open, (close_type)&terminal_close, (read_type)&terminal_read, (write_type)&terminal_write};

struct PCB_table* pcb_t;
struct PCB_table pcb_box;
struct file_descriptor fd_box;

struct terminal_t{
    int saved_esp, saved_ebp, pid;
    int send_eoi;
};
struct terminal_t terminal[3];

int get_pid(){
    return pid;
}
int get_main_terminal(){
    return main_terminal;
}
int get_display_terminal(){
    return display_terminal;
}
void fd_init(){         // need to be run after booting part
    // last_pid=-1;
    display_terminal=0;
    main_terminal=0; //for debug only                                                /////////////////////////////////////////// remember to delete
    pid=-1;
    processor_usage=0;
    flag_open_three_shell=0;
    // phy_mem_loc=8;  // start address in physical mem to store files  8MB
    return;
}

/* int system_halt(unit8_t status);
 * Inputs: status of the system
 * Return Value: 0
 * Function: halts the process of the system  */

int system_halt(uint8_t status){
    //remember to clear the paging.
    int i,saved_ebp,saved_esp,status_,old_pid;
    status_=status;
    // put_number(status);
    // puts("--------\n");
    if(status == HALT){
        status_ = HALT_error;
    }
    // clear the page that was used for now complete process
    if(pid<3){
        // fd_init();                                                          ////// something wrong here about updating the process_usage
        processor_usage^=(1<<pid);
        const uint8_t* command = (uint8_t*) "shell";
        system_execute(command);//return status;
        return -1;
    }
    // clearing process
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    for(i=2;i<7;i++) system_close(i);
    
    // update pid
    processor_usage^=(1<<pid);
    old_pid=pid;

    terminal[main_terminal].pid=pcb_t->parent_id;
    pid=pcb_t->parent_id;

    pcb_box=*pcb_t;
    pcb_box.active=0;
    pcb_box.fdt_usage=0;
    pcb_box.id=0;
    pcb_box.parent_id=0;
    saved_ebp=pcb_box.saved_ebp;
    saved_esp=pcb_box.saved_esp;

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
    // phy_mem_loc-=Program_page;
    // set_new_page(phy_mem_loc-Program_page);
    phy_mem_loc=8+4*pid;
    set_new_page(phy_mem_loc);
    
    // clear tlb
    asm volatile(
        "movl %cr3, %edx\n"
        "movl %edx, %cr3\n"
    );

    // load the parent task 
    tss.ss0 = KERNEL_DS;
    tss.esp0 = addr_8MB - size_8kb*(pid) - 4; //8mb-8kb-4 -4 is for safety

    asm volatile(
        "movl %2, %%eax;"
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        "leave;"
        "ret;"
        :
        :"r"(saved_esp), "r"(saved_ebp), "r"(status_)
    );
    return 1;
}

// uint8_t* 
/* void executeable_parse(unit8_t* command);
 * Inputs: the user command given
 * Return Value: NONE
 * Function: parses the command and arguments for the executed program */
void executeable_parse(uint8_t* command){
    // "shell"
    // puts((int8_t*)command);
    // put_number((int)strlen(command));
    // putc('\n');
    
    int start = 0, start2 = 0;
    // finds the starting postion of the command word in the command string
    while(command[start]==' '){
        start++;
    }

    int end = start, end2=strlen((int8_t*)command);
    // finds the end position of the command word in the command string
    while(command[end] != ' ' && end < strlen((int8_t*)command)){
        end++;
    }
    uint8_t temp[end-start+1];

    start2=end;
    // finds the starting position of the argument in the command string
    while(command[start2]==' '){
        start2++;
    }
    
    // finds the last position of the argument in the command string
    while(command[end2-1]==' '  && end2>start2){
        end2--;
    }

    uint8_t temp2[end2-start2+1];
    // put_number(start2);
    // putc(' ');
    // put_number(end2);putc('\n');
    temp[end-start]=0;          // string end with \0
    temp2[end2-start2]=0;       // string end with \0
    // printf("%d; start: %d, end: %d\n", end-start, start, end);
    
    // command_parsing = &command[end+1];
    // strncpy(temp,command+start,end-start);
    // command = temp;
    int length = end-start;
    int i = 0;
    while(i<length){
        temp[i]=command[start+i];
        i++;
    }
    i=start2;
    while(i<end2){
        temp2[i-start2]=command[i];
        i++;
    }
    // puts(temp2);putc('\n');

    pcb_t=(struct PCB_table*)get_pcb_pointer();
    
    strncpy((int8_t*)command, (int8_t*)temp, end-start);
    strncpy(pcb_t->arg, (int8_t*)temp2, end2-start2);
    command[end-start]=0;           // string end with \0
    pcb_t->arg[end2-start2]=0;      // string end with \0
    // puts(pcb_t->arg);
    // puts(command);
    // command = "shell";
    return;
}   

/* int system_execute(unit8_t status);
 * Inputs: the user command given
 * Return Value: certain values depending on the type of execution process it is, a fail, system call halr or system call halt
 * Function: executes a new program and process */

int system_execute(const uint8_t* command){
    // cli();
    if(pid>=5){
        puts("Too Many Programs are being run\n");
        return 0;
    }
    int re,last_last_pid;
    uint8_t buf[text_read], command_exe[command_length];
    struct dentry dt;
    
    strcpy((int8_t*)command_exe, (int8_t*)command);

    last_last_pid=last_pid;
    last_pid=pid;
    // pid++;
    pid=find_next_pid();
    // if(pid==2)pid=3;                              ///////////////////////////
    pcb_t=(struct PCB_table*)get_pcb_pointer();

    //Parse args
    executeable_parse(command_exe);
    // puts(command);putc('\n');

    //Check for executable
    re=read_dentry_by_name(command_exe,&dt);
    if(re==-1){
        pid=last_pid;
        last_pid=last_last_pid;
        return -1;
    }
    re=read_data(dt.inode_num, 0, buf, text_read);
    if(buf[0]==exe_0 && buf[1]==exe_1 && buf[2]==exe_2 && buf[3]==exe_3);
    else {
        pid=last_pid;
        last_pid=last_last_pid;
        return -1;
    }
    
    // if(flag_open_three_shell<2){
    //     // switch_terminal(pid+1);
    //     terminal[pid].send_eoi=0;
    // }
    if(pid<3) main_terminal=pid;

    terminal[main_terminal].pid=pid;
    processor_usage|=(1<<pid);

    int eip = (buf[27]<<24)|(buf[26]<<16)|(buf[25]<<8)|(buf[24]); // using bytes 27-24 of the user program to set the EIP to user program
    // puts(command);
    //Set up pagings
    phy_mem_loc=8+pid*4;
    set_new_page(phy_mem_loc);
    // phy_mem_loc+=4;
    
    // clear tlb
    asm volatile(
        "movl %cr3, %edx \n"
        "movl %edx, %cr3 \n"
    );
    // puts(command);
    //Load file into memory
    re=read_data((uint32_t)(dt.inode_num), (uint32_t)0, (uint8_t*)(0x08048000), (uint32_t)get_length(dt));//;
    
    //Create PCB
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");

    if(pid<3){
        pcb_box.parent_id=pid;
    } else {
        pcb_box.parent_id=last_pid;
    }
    pcb_box.id=pid;
    pcb_box.saved_esp=saved_esp;              // what should be saved here
    pcb_box.saved_ebp=saved_ebp;              // what should be saved here
    pcb_box.active=1;
    pcb_box.fdt_usage=3; //00000011
    
    // sets the File Descriptor table for executed function 
    fd_box.opt_table_pointer=(uint32_t)&terminal_commands;
    fd_box.inode=-1;
    fd_box.file_pos=0;
    fd_box.flags=1;         // in use or not
    pcb_box.fdt[0]=fd_box;  // sets the first entry to terminal command
    pcb_box.fdt[1]=fd_box;  // sets the second entry to terminal command 
    fd_box.opt_table_pointer=NULL;
    fd_box.inode=-1;
    fd_box.file_pos=0;
    fd_box.flags=0;
    //    puts(command);
    // entries 2-7 is set to NULL for File Descriptor table
    pcb_box.fdt[2]=fd_box;  
    pcb_box.fdt[3]=fd_box;
    pcb_box.fdt[4]=fd_box;
    pcb_box.fdt[5]=fd_box;
    pcb_box.fdt[6]=fd_box;
    pcb_box.fdt[7]=fd_box;
    strcpy(pcb_box.arg, pcb_t->arg);
    // Process Control Block is Set to current executed function
    *(pcb_t)=pcb_box;

    //Prepare for Context Switch
    tss.ss0 = KERNEL_DS;    // TSS tells the 
    tss.esp0 = 0x800000 - 0x2000*(pid) - 4; //8mb-(8kb*pid) -4 is for safety
    // DS
    // esp  calculated through the 2^20 * 132 = 138412032 to hex is 0x08400000 and -4 of that is 0x083FFFFC
    // eflags 
    // cs
    // eip 

    // sti();
    puts("wowowowowowowowowowowowoowowowowowowoowowowowowo\n");
    put_number(flag_open_three_shell);putc('\n');
    IRET_prepare(eip);          //eip address may change, may need to modify it

    return 0;
}

/* int system_read(int32_t fd, void* buf, int32_t nbytes);
 * Inputs: fd which is the file descriptor, buf which is the buffer, nbytes which is the number of bytes
 * Return Value: re
 * Function: reads data from the given process or system */

int system_read(int32_t fd, void* buf, int32_t nbytes){
    int re;
    re=0;
    if(fd<0||fd>7)return -1;
    if(fd==1)return -1; //stdout does not handle terminal read
    if(check_fd_in_use(fd)==0)return -1;
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    fd_box=pcb_t->fdt[fd];
    if( pid<3 && terminal[pid].send_eoi==0 ) {
        terminal[pid].send_eoi=1;
        send_eoi(0);
    }
    re = ((((struct files_command*)(fd_box.opt_table_pointer))->read)((int32_t)fd,(void*)buf,(int32_t)nbytes));
    if(re!=-1){
        // fd_box.file_pos+=re;
        pcb_t=(struct PCB_table*)get_pcb_pointer();
        pcb_t->fdt[fd].file_pos+=re;
    }
    return re;
}

/* int system_write(int32_t fd, const void* buf, int32_t nbytes);
 * Inputs: fd which is the file descriptor, buf which is the buffer, nbytes which is the number of bytes
 * Return Value: null
 * Function: writes data to the given process or system */

int system_write(int32_t fd, const void* buf, int32_t nbytes){
    // puts(" i fucking finish everything before here\n");
    int re;
    if(fd<0||fd>7)return -1;
    if(fd==0)return -1; //stdin does not handle terminal write
    if(check_fd_in_use(fd)==0)return -1;
    // pcb_t=get_pcb_pointer();
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    fd_box=pcb_t->fdt[fd];
    // re=*(*(struct files_command*)fd_box.opt_table_pointer->write)(&fd_box,buf,nbytes);
    re = ((((struct files_command*)fd_box.opt_table_pointer)->write)((int32_t)fd,(void*)buf,(int32_t)nbytes));
    return re;
}

/* int system_open(const unit8_t* filename);
 * Inputs: filename which is the name of the file
 * Return Value: fd which is the file descriptor
 * Function: opens the system and displays access */

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
            // puts("no space in file descriptor array");
            return -1;
        }
    }
    
    if (file.filetype == 0) re=((RTC_commands.open)((uint8_t*)filename));
    else if (file.filetype == 1) re=((dir_commands.open)((uint8_t*)filename));
    else if (file.filetype == 2) re=((file_commands.open)((uint8_t*)filename));
    if(re==-1)return -1;

    fd_box=pcb_t->fdt[fd];
    
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

/* int system_close(int32_t fd);
 * Inputs: fd which is the file descriptor
 * Return Value: 0
 * Function: closes the file descriptor that was given in the input */

int system_close(int32_t fd){
    int re;
    if(fd<0||fd>7)return -1;
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

/* int system_getargs(unit8_t* buf, int32_t nbytes);
 * Inputs: buf which is the buffer and nbytes which is the number of bytes
 * Return Value: null
 * Function: gets the given argument and writes it into the buffer */

int system_getargs(uint8_t* buf, int32_t nbytes){
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    //checking if buffer is null
    if(buf == NULL) return -1;
    int len=strlen((int8_t*)pcb_t->arg);
    if(len==0||len>nbytes)return -1;
    //copying the argument into PCB
    strcpy((int8_t*)buf,pcb_t->arg);
    return 0;
} 

/* int system_vidmap(uint32_t** screen_start);
 * Inputs: buf which is the buffer
 * Return Value: null
 * Function: puts in the address that is pointing to virtual memory in the system */

int system_vidmap(uint8_t** screen_start){
    if(screen_start == NULL)return -1;
    if((int)screen_start>=0x400000 && (int)screen_start<0x800000)return -1;     // if accessing the memory location between 4MB-8MB return -1
    
    // *screen_start = (uint8_t*)set_video_page();
    *screen_start = (uint8_t*)(36*4*1024*1024+184*4*1024);

    put_number(main_terminal);putc('\n');
    put_number(display_terminal);putc('\n');

    // if the new user video is not displaying, reset the pointer
    // if( main_terminal != display_terminal ){
    //     set_invisible_video_page(main_terminal);
    //     map_B8_B9_table( ((8+main_terminal)*size_4MB+184*size_4kb)/size_4kb );
    // }

    // puts("Success vidmap\n");
    return 0;
}

void strncpy_(int dest, int source, uint32_t nbytes){
    // strncpy((int8_t*)dest, (int8_t*)source, nbytes);
    memcpy((int8_t*)dest, (int8_t*)source, nbytes);
    return;
}

int switch_terminal(int next_display_terminal){
    cli();
    next_display_terminal-=1;
    flush_tlb();
    // if next_display_terminal
    if( next_display_terminal == display_terminal )goto done_switch_terminal;
    
    if( next_display_terminal == main_terminal && display_terminal != main_terminal ){
        map_B8_B9_table(0xB8);
        flush_tlb();
        strncpy_( (0xBC + 2*display_terminal)*size_4kb, (0xB8)*size_4kb, size_8kb );
        strncpy_( (0xB8)*size_4kb, vidpointer, size_8kb );
        set_video_page();
        goto done_switch_terminal;
    }

    if( next_display_terminal != main_terminal && display_terminal == main_terminal ){
        set_invisible_video_page(main_terminal);
        flush_tlb();
        // map_B8_B9_table(0xB8);           // no need because this one is already displaying
        strncpy_( vidpointer, (0xB8)*size_4kb, size_8kb );
        strncpy_( (0xB8)*size_4kb, (0xBC + 2*next_display_terminal)*size_4kb, size_8kb);        
        map_B8_B9_table( ((8+main_terminal)*size_4MB+184*size_4kb)/size_4kb );
        goto done_switch_terminal;
    }

    if( next_display_terminal != main_terminal && display_terminal != main_terminal ){
        map_B8_B9_table(0xB8);
        flush_tlb();
        strncpy_( (0xBC + 2*display_terminal)*size_4kb, (0xB8)*size_4kb, size_8kb );
        strncpy_( (0xB8)*size_4kb, (0xBC + 2*next_display_terminal)*size_4kb, size_8kb );
        map_B8_B9_table( ((8+main_terminal)*size_4MB+184*size_4kb)/size_4kb );     
        goto done_switch_terminal;
    }
done_switch_terminal:
    display_terminal=next_display_terminal;
    flush_tlb();
    puts("finish switch terminal\n");
    sti();
    return 0;
}

void schedule(){
    int next_main_terminal = (main_terminal+1)%3;
    int next_pid = terminal[next_main_terminal].pid;
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");

    // store and update main_terminal and pid
    terminal[main_terminal].pid=pid;
    terminal[main_terminal].saved_ebp=saved_ebp;
    terminal[main_terminal].saved_esp=saved_esp;

    if(flag_open_three_shell!=3){
        // switch_terminal(flag_open_three_shell);
        terminal[flag_open_three_shell].send_eoi=0;

        set_video_page();
        strncpy_( (0xBC + 2*main_terminal)*size_4kb, (0xB8)*size_4kb, size_8kb );
        strncpy_( (0xB8)*size_4kb, (0xBC + 2*next_main_terminal)*size_4kb, size_8kb );
        display_terminal=next_main_terminal;
        main_terminal=next_main_terminal;
        
        flag_open_three_shell++;
        const uint8_t* command = (uint8_t*) "shell";
        system_execute(command);//return status;
        return;
    }
    else{
        send_eoi(0);
        return;
    }
    // update esp ebp
    asm volatile(
        "movl %0, %%esp;"
        "movl %1, %%ebp;"
        :
        :"r"(terminal[next_main_terminal].saved_esp), "r"(terminal[next_main_terminal].saved_ebp)
    );

    // switch video map
    flush_tlb();
    if( next_main_terminal == main_terminal )goto finish_schedule_terminal;
    if( next_main_terminal == display_terminal && main_terminal != display_terminal ){
        map_B8_B9_table(0xB8);
        strncpy_( (0xBC + 2*main_terminal)*size_4kb, vidpointer, size_8kb );
        set_video_page();
        goto finish_schedule_terminal;
    }
    if( next_main_terminal != display_terminal && main_terminal == display_terminal){
        strncpy_( (0xBC + 2*main_terminal)*size_4kb, vidpointer, size_8kb );
        set_invisible_video_page(next_main_terminal);
        strncpy_( vidpointer, (0xBC + 2*next_main_terminal)*size_4kb, size_8kb );
        map_B8_B9_table( ((8+next_main_terminal)*size_4MB+184*size_4kb)/size_4kb );
        goto finish_schedule_terminal;
    }
    if (next_main_terminal != display_terminal && main_terminal != display_terminal){
        strncpy_( (0xBC + 2*main_terminal)*size_4kb, vidpointer, size_8kb );
        set_invisible_video_page(next_main_terminal);
        strncpy_( vidpointer, (0xBC + 2*next_main_terminal)*size_4kb, size_8kb );
        map_B8_B9_table( ((8+next_main_terminal)*size_4MB+184*size_4kb)/size_4kb );
        goto finish_schedule_terminal;
    }

finish_schedule_terminal:

    // update main_terminal and pid
    pid = next_pid;
    main_terminal = next_main_terminal;

    // update tss
    tss.ss0 = KERNEL_DS;    // TSS tells the 
    tss.esp0 = 0x800000 - 0x2000*(next_pid) - 4; //8mb-(8kb*pid) -4 is for safety

    // switch user code
    phy_mem_loc = 8 + next_pid * 4;
    set_new_page(phy_mem_loc);

    // flush TLB
    flush_tlb();

    // call iret back to user code
    // asm volatile(
    //     "iret;"
    // );
    send_eoi(0);
    return;
}

/* int check_fd_in_use(int32_t fd);
 * Inputs: fd which is the file descriptor
 * Return Value: null
 * Function: checks if the file descriptor is in use and passes that result in */

int check_fd_in_use(int32_t fd){
    pcb_t=(struct PCB_table*)get_pcb_pointer();
    pcb_box=*pcb_t;
    if(((1<<fd)&pcb_box.fdt_usage)!=0)return 1;
    return 0;
}

/* int get_pcb_pointer();
 * Inputs: null
 * Return Value: the pcb pointer
 * Function: returns the pcb pointer using the pid in the system */

int get_pcb_pointer(){
    return addr_8MB-size_8kb*(pid+1);       // pid starts at 0 (0 -> move 1*8kb, 1-> move 2*8kb)
}

int find_next_pid(){
    int i;
    for(i=0;i<6;i++){
        if((processor_usage&(1<<i))==0)return i;
    }
    return -1;
}
void flush_tlb(){
    asm volatile(
        "movl %cr3, %edx \n"
        "movl %edx, %cr3 \n"
    );
    return;
}

