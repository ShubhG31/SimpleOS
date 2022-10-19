#include "file_sys.h"
#define name_length 32

extern struct dentry_t dentry[63]; 
extern struct boot_block_t boot;
extern struct inode_t node[64]; 

int cmp_name(uint8_t* A, uint8_t* B){
    int i;
    for(i=0;i<name_length;i++){
        if(A[i]!=B[i])return 0;
    }
    return 1;
}
int32_t read_dentry_by_name (const uint8_t* fname, struct dentry_t* dt){
    int i;
    if( fname == NULL || dentry == NULL )return -1;
    for( i = 0; i < boot.dir_count; i++ ){
        if(cmp_name(dentry[i].filename,fname)){
            dt.filetype=dentry[i].filetype;
            dt.inode_num=dentry[i].inode_num;
            return 0;
        }
    }
    return -1;
}
int32_t read_dentry_by_index (uint32_t index, struct dentry_t* dt){
    if( dentry == NULL )return -1;
    if( index < 0 || index > dentry[i].dir_count )return -1;
    dt.filetype=dentry[index].filetype;
    dt.inode_num=dentry[index].inode_num;
    return 0;
}
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    int i,j,l_read;
    if( buf == NULL )return -1;
    if( idnoe < 0 || inode > boot.inode_count )return -1;
    l_read=0;
    for( i = 1; i <= node[inode].length; i++ ){
        for ( j = 0; j< 4096; j++ ){
            buf[l_read++]=data_b[ node[inode].data[i] ].d[j];
            if(l_read==length)return length;
        }
    }
    return 0;
}

int32_t show_ls (){


    return 0;
}


/*
some questions:
1. read_data, check inode is within the valid range (0~boot.inode_count)
2. why lecture call read_dentry_by_name in that way
3. do we need to initialize every entry in the file system structure
4. data block is bad, what is bad



*/
