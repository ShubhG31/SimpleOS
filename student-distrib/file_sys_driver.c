#include "file_sys_driver.h"
#include "file_sys.h"

struct dentry dt_dir,dt_file;

/* ---------------------------- DIR ----------------------------*/
// File open() initialize any temporary structures, return 0
int dir_open(const uint8_t* filename){
    //read_dentry_by_name (const uint8_t* fname, struct dentry_t* dt){
    read_dentry_by_name (filename,(&dt_dir));
    return 0;
}

// File close() undo what you did in the open function, return 0
int dir_close(int32_t fd){
    return 0;
}

// File read() reads count bytes of data from file into buf
int dir_read(int32_t fd, void* buf, int32_t nbytes){
    int re,inode;
    inode=1;
    re=read_data ( inode, 0, buf, nbytes);
    if( re==0 || re==-1 )return -1;
    return 0;
}

// File write() should do nothing, return -1
int dir_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
    //return 0;
}

/* ---------------------------- FILE ----------------------------*/
// File open() initialize any temporary structures, return 0
int file_open(const uint8_t* filename){
    read_dentry_by_name(filename,&dt_file);
    return 0;
}

// File close() undo what you did in the open function, return 0
int file_close(int32_t fd){

    return 0;
}

// File read() reads count bytes of data from file into buf
int file_read(int32_t fd, void* buf, int32_t nbytes){
    int re,inode;
    inode=1;
    re=read_data ( inode, 0, buf, nbytes);
    if( re==0 || re==-1 )return -1;

    return 0;
}

// File write() should do nothing, return -1
int file_write(int32_t fd, const void* buf, int32_t nbytes){
    return -1;
    //return 0;
}
