#include "lib.h"
#include "types.h"

#define name_length 32
#define block_size 4096
#define dentry_size 64
#define file_name_off 0
#define file_type_off 32
#define inode_off 36
#define length_off 0

struct dentry{
    uint8_t filename[32];
    int32_t filetype;
    int32_t inode_num;
    int8_t reserved[24];
};//dentry_t;      // 64B

struct boot_block{
    int32_t dir_count;
    int32_t inode_count;
    int32_t data_count;
    int8_t  reserved[52];
};//boot_block_t;  // 64B

struct inode{
    int32_t length;
    int32_t data[1023];
};//inode_t;       // 4KB

struct data_block{
    int8_t d[4096];
};//data_block_t;

extern int32_t read_dentry_by_name (const uint8_t* fname, struct dentry* dentry);  
extern int32_t read_dentry_by_index (uint32_t index, struct dentry* dentry);
extern int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);
extern int32_t load_fss();
extern int32_t show_ls();
extern int32_t get_length();
extern int32_t get_dir_number();

