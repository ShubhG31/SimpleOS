#include "file_sys_driver.h"
#include "file_sys.h"

#define PCB_size 8
#define dir_count 63

struct dentry dt_dir,dt_file;
static struct file_descriptor FD[PCB_size];
int use[FD_num],dir_p,head;

/*
 * file_sys_init
 *   DESCRIPTION: initialize the condition that each file descriptor are used or not
 *   INPUTS: none
 *   OUTPUTS: initialize every usage condition to be zero
 *   RETURN VALUE: none
 *   SIDE EFFECTS: none
 */
int file_sys_init(){
    int i;
    for( i=0;i<PCB_size;i++ )use[i]=0;
    use[0]=use[1]=0;                                            // might need to change in future checkpoint, no need to change starting iterate point
                                                                // because we will set the use[i]=1 to avoid using it
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
    for( head = 0; head < PCB_size && use[head] == 1; head++ );         // this is for future CP, ignore now
    FD[head].opt_table_pointer=0;                                       // pointer to the function?
    FD[head].inode=0;   // we have only one directory, its inode is 0
    FD[head].file_pos=0;    // start with offset at 0
    FD[head].flags=1;
    use[head]=1;    // set this fd this in use
    dir_p=0;        // store the index of file that we have already read
    return head;
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
    if(fd<0)return -1;
    if(use[fd]==0)return 0; //sanity check
    FD[fd].opt_table_pointer=0;
    FD[fd].inode=-1;    // set inode to be impossible one
    FD[fd].file_pos=0;  // start with 0 position
    FD[fd].flags=0;
    use[fd]=0;
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
    if(fd<0)return -1;
    if(use[fd]==0)return -1;
    l_read=0;
    for( dir_p=dir_p+1; dir_p<get_dir_number(); dir_p++ ){  // continue to read file_name from last one
        re=read_dentry_by_index (dir_p, &dt_dir);       // get the dentry of next file
        // printf("file_type: %d   length: %d //",dt_dir.filetype, get_length(dt_dir));
        // puts("file_type:");
        // put_number(dt_dir.filetype);
        // puts("    length:");
        // put_number(get_length(dt_dir));
        // puts(" ");                                      // for test
        for( j=0;j<name_length;j++ ){
            // printf("%c",dt_dir.filename[j]);
            *((uint8_t*)(buf+l_read))=dt_dir.filename[j];       // store the name of file's info
            l_read++;
            // buf[l_read++]=dt_dir.filename[j];
        }
        // printf("\n");
        return l_read;  // return the number of bytes in name
    }
    return -1;
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
    for( head = 0; head < PCB_size && use[head] == 1; head++ );
    FD[head].opt_table_pointer=0;                                        // pointer to the function?
    FD[head].inode=dt_file.inode_num;   // store the inode_num of the file
    FD[head].file_pos=0;        // starting position is 0
    FD[head].flags=1;
    use[head]=1;
    // printf("I got the file open %d\n",FD[head].inode);
    return head;
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
    if(use[fd]==0)return -1;
    FD[fd].opt_table_pointer=0;
    FD[fd].inode=-1;        // revert inode to a impossible index
    FD[fd].file_pos=0;
    FD[fd].flags=0;
    use[fd]=0;
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
    int re;
    // if(fd<=1)return -1;          // might need this for the latter checkpoint
    if(fd<0)return -1;
    if(use[fd]==0)return -1;
    re=read_data ( FD[fd].inode, FD[fd].file_pos, (uint8_t*)buf, nbytes);   // read data stored in inode, starting at file_pos
    // printf("\nI got the file read? %d\n",re);
    if( re==0 || re==-1 )return -1;
    FD[fd].file_pos+=nbytes;        // update the offset
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
