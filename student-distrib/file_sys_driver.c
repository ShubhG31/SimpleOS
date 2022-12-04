#include "file_sys_driver.h"

#define PCB_size 8 
#define dir_count 63
struct dentry dt_dir,dt_file;
int dir_p;
int head;

/*
 * file_sys_init
 *   DESCRIPTION: initialize the condition that each file descriptor are used or not
 *   INPUTS: none
 *   OUTPUTS: initialize every usage condition to be zero
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int file_sys_init(){
    return 0;
}
/* ---------------------------- DIR ----------------------------*/

/*
 * dir_open
 *   DESCRIPTION: open_directory, store open condition into PCB
 *   INPUTS: filename: the dirctory we need to open
 *   OUTPUTS: none
 *   RETURN VALUE: return the file descriptor number, we assume all to be zero for CP2
 *                 if reading fails, we return -1
 *   SIDE EFFECTS: initialzie FD info, and set the flag whose FD contains info to be used
 */
// File open() initialize any temporary structures, return 0
int dir_open(const int8_t* filename){
    int re;
    if(filename==NULL||strlen(filename)>name_length)return -1;
    re=read_dentry_by_name ((uint8_t*)filename,(&dt_dir));
    if(re==-1)return -1;        // reading fails, so we return -1
    dir_p=-1;        // store the index of file that we have already read
    return dt_dir.inode_num;
}

/*
 * dir_close
 *   DESCRIPTION: close_directory, revert whar we store in PCB
 *   INPUTS: fd: index of file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for seccess, -1 for failure
 *   SIDE EFFECTS: none
 */
// File close() undo what you did in the open function, return 0
int dir_close(int32_t fd){
    if(fd<0||fd>7)return -1;
    dir_p=0;
    return 0;
}

/*
 * dir_read
 *   DESCRIPTION: read the files' name under this file directory
 *   INPUTS: fd: index of file descriptor
 *           buf: store the data we read into buf array
 *           nbytes: we need to read nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: length of data that we read for seccess, -1 for failure
 *   SIDE EFFECTS: none
 */
// File read() reads count bytes of data from file into buf
int dir_read(int32_t fd, void* buf, int32_t nbytes){
    int re,j,l_read;
    l_read=0;
    for( dir_p=dir_p+1; dir_p<get_dir_number(); dir_p++ ){  // continue to read file_name from last one
        re=read_dentry_by_index (dir_p, &dt_dir);       // get the dentry of next file
        // puts("file_type:");
        // put_number(dt_dir.filetype);
        // puts("  length:");
        // put_number(get_length(dt_dir));
        // puts("  ");                                      // for test
        for( j=0;j<name_length;j++ ){
            // printf("%c",dt_dir.filename[j]);
            *((uint8_t*)(buf+l_read))=dt_dir.filename[j];       // store the name of file's info
            l_read++;
            // buf[l_read++]=dt_dir.filename[j];
        }
        // printf("\n");
        return l_read;  // return the number of bytes in name
    }
    return 0;
    // return -1;
}

/*
 * dir_write
 *   DESCRIPTION: write info into directory
 *   INPUTS: fd: index of file descriptor
 *           buf: store the data we write into buf array
 *           nbytes: we need to write nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: length of data that we write for seccess, -1 for failure
 *   SIDE EFFECTS: none
 */
// File write() should do nothing, return -1
int dir_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
    //return 0;
}

/* ---------------------------- FILE ----------------------------*/
/*
 * file_open
 *   DESCRIPTION: open_file, store open condition into PCB
 *   INPUTS: filename: the file we need to open
 *   OUTPUTS: none
 *   RETURN VALUE: return the file descriptor number, we assume all to be zero for CP2
 *                 if reading fails, we return -1
 *   SIDE EFFECTS: initialzie FD info, and set the flag whose FD contains info to be used
 */
// File open() initialize any temporary structures, return 0
int file_open(const int8_t* filename){
    int re=0;
    if(filename==NULL||strlen(filename)>name_length)return -1;
    re=read_dentry_by_name ((uint8_t*)filename,(&dt_file));
    if(re==-1)return -1;
    return 0;//dt_file.inode_num;
    // return head;
}

/*
 * file_close
 *   DESCRIPTION: close_close, revert whar we store in PCB
 *   INPUTS: fd: index of file descriptor
 *   OUTPUTS: none
 *   RETURN VALUE: 0 for seccess, -1 for failure
 *   SIDE EFFECTS: none
 */
// File close() undo what you did in the open function, return 0
int file_close(int32_t fd){
    // if(fd<=1)return -1;          // might need this for the latter checkpoint
    if(fd<0)return -1;
    return 0;
}

/*
 * file_read
 *   DESCRIPTION: read the files' data
 *   INPUTS: fd: index of file descriptor
 *           buf: store the data we read into buf array
 *           nbytes: we need to read nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: length of data that we read for seccess, -1 for failure
 *   SIDE EFFECTS: none
 */
// File read() reads count bytes of data from file into buf
int file_read(int32_t fd, void* buf, int32_t nbytes){
    // puts(" iiiiii fucking finish everything before here\n");
    int re;
    struct PCB_table* pcb_;
    struct file_descriptor FD;
    if(fd<0)return -1;
    pcb_ = (struct PCB_table*)get_pcb_pointer();
    FD = pcb_->fdt[fd];

    re=read_data ( FD.inode, FD.file_pos, (uint8_t*)buf, nbytes);   // read data stored in inode, starting at file_pos
    // if( re==-1 )return -1;
    // if( re==0 ) return 0;
//    FD.file_pos+=re;
//    pcb_ = (struct PCB_table*)get_pcb_pointer();    // reload      
//    pcb_->fdt[fd]=FD;                                          
    // printf("I got the file read\n");
    return re;
}


/*
 * file_write
 *   DESCRIPTION: write info into files
 *   INPUTS: fd: index of file descriptor
 *           buf: store the data we write into buf array
 *           nbytes: we need to write nbytes
 *   OUTPUTS: none
 *   RETURN VALUE: length of data that we write for seccess, -1 for failure
 *   SIDE EFFECTS: none
 */
// File write() should do nothing, return -1
int file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
    //return 0;
}
