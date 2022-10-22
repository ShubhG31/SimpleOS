#include "file_sys_driver.h"
#include "file_sys.h"

#define PCB_size 8
#define dir_count 63

struct dentry dt_dir,dt_file;
static struct file_descriptor FD[PCB_size];
int use[8],dir_p,head;

int file_sys_init(){
    int i;
    for( i=0;i<PCB_size;i++ )use[i]=0;
    use[0]=use[1]=0;                                            // might need to change in future checkpoint, no need to change starting iterate point
                                                                // because we will set the use[i]=1 to avoid using it
    return 0;
}
/* ---------------------------- DIR ----------------------------*/
// File open() initialize any temporary structures, return 0
int dir_open(const int8_t* filename){
    int re;
    re=read_dentry_by_name ((uint8_t*)filename,(&dt_dir));
    if(re==-1)return -1;
    for( head = 0; head < PCB_size && use[head] == 1; head++ );
    FD[head].opt_table_pointer=0;                                       // pointer to the function?
    FD[head].inode=0;
    FD[head].file_pos=0;
    FD[head].flags=1;
    use[head]=1;
    dir_p=0;
    return head;
}

// File close() undo what you did in the open function, return 0
int dir_close(int32_t fd){
    if(fd<0)return -1;
    if(use[fd]==0)return 0;
    FD[fd].opt_table_pointer=0;
    FD[fd].inode=-1;
    FD[fd].file_pos=0;
    FD[fd].flags=0;
    use[fd]=0;
    return 0;
}

// File read() reads count bytes of data from file into buf
int dir_read(int32_t fd, void* buf, int32_t nbytes){
    int re,j,l_read;
    if(fd<0)return -1;
    if(use[fd]==0)return -1;
    l_read=0;
    for( dir_p=dir_p+1; dir_p<get_dir_number(); dir_p++ ){
        re=read_dentry_by_index (dir_p, &dt_dir);
        // printf("file_type: %d   length: %d //",dt_dir.filetype, get_length(dt_dir));
        puts("file_type:");
        put_number(dt_dir.filetype);
        puts("    length:");
        put_number(get_length(dt_dir));
        puts(" ");
        for( j=0;j<name_length;j++ ){
            // printf("%c",dt_dir.filename[j]);
            *((uint8_t*)(buf+l_read))=dt_dir.filename[j];
            l_read++;
            // buf[l_read++]=dt_dir.filename[j];
        }
        // printf("\n");
        return l_read;
    }
    return -1;
}

// File write() should do nothing, return -1
int dir_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
    //return 0;
}

/* ---------------------------- FILE ----------------------------*/
// File open() initialize any temporary structures, return 0
int file_open(const int8_t* filename){
    int re=0;
    re=read_dentry_by_name ((uint8_t*)filename,(&dt_file));
    if(re==-1)return -1;
    for( head = 0; head < PCB_size && use[head] == 1; head++ );
    FD[head].opt_table_pointer=0;                                        // pointer to the function?
    FD[head].inode=dt_file.inode_num;
    FD[head].file_pos=0;
    FD[head].flags=1;
    use[head]=1;
    // printf("I got the file open %d\n",FD[head].inode);
    return head;
}

// File close() undo what you did in the open function, return 0
int file_close(int32_t fd){
    // if(fd<=1)return -1;          // might need this for the latter checkpoint
    if(fd<0)return -1;
    if(use[fd]==0)return -1;
    FD[fd].opt_table_pointer=0;
    FD[fd].inode=-1;
    FD[fd].file_pos=0;
    FD[fd].flags=0;
    use[fd]=0;
    return 0;
}

// File read() reads count bytes of data from file into buf
int file_read(int32_t fd, void* buf, int32_t nbytes){
    int re;
    // if(fd<=1)return -1;          // might need this for the latter checkpoint
    if(fd<0)return -1;
    if(use[fd]==0)return -1;
    re=read_data ( FD[fd].inode, FD[fd].file_pos, (uint8_t*)buf, nbytes);
    // printf("\nI got the file read? %d\n",re);
    if( re==0 || re==-1 )return -1;
    FD[fd].file_pos+=nbytes;
    // printf("I got the file read\n");
    return re;
}

// File write() should do nothing, return -1
int file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
    //return 0;
}
