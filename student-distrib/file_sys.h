#include "lib.h"
#include "types.h"

struct dentry{
    uint8_t filename[32];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[24];
}dentry_t;      // 64B

struct boot_block{
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t  reserved[52];
}boot_block_t;  // 64B

struct inode{
    int32_t length;
    int32_t data[1023];
}inode_t;       // 4KB

struct data_block{
    int8_t d[4096];
}data_block_t;

extern int32_t read_dentry_by_name (const uint8_t* fname, dentry* dentry);  
extern int32_t read_dentry_by_index (uint32_t index, dentry* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern int32_t show_ls();

extern struct dentry_t dentry[63]; 
extern struct boot_block_t boot;
extern struct inode_t node[64]; 
extern struct data_block_t data_b[4096]

// q1: what is dentry

