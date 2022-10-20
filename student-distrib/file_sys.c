#include "file_sys.h"

#define name_length 32
#define block_size 4096
#define dentry_size 64
#define file_name_off 0
#define file_type_off 32
#define inode_off 36
#define length_off 0

unsigned int boot;
unsigned int dentry; 
unsigned int node; 
unsigned int data_b;
uint32_t dir_count, inode_count, data_count;
struct dentry test;

int32_t load_fss(unsigned int mod_start){
    boot    = mod_start;
    dentry  = mod_start+dentry_size;
    node    = mod_start+block_size;
    dir_count   = *((int*)mod_start);
    inode_count = *((int*)(mod_start+4));
    data_count  = *((int*)(mod_start+8));
    data_b  = mod_start + block_size*(1+inode_count);
    // clear();
    printf("%u %u %u\n",dir_count,inode_count,data_b);
    printf("finish loading file system structre\n");
    return 0;
}
int cmp_name(const uint8_t* A, const uint8_t* B){
    int i;
    for(i=0;i<name_length;i++){
        if(A[i]!=B[i])return 0;
    }
    return 1;
}
int32_t read_dentry_by_name (const uint8_t* fname, struct dentry* dt){
    int i;
    if( fname == NULL || dentry == NULL )return -1;
    for( i = 0; i < dir_count; i++ ){
        if(cmp_name((uint8_t*) (dentry + i*dentry_size + file_name_off),fname)){
            (*dt).filetype=*((uint32_t*)(dentry + i*dentry_size + file_type_off));
            (*dt).inode_num=*((uint32_t*)(dentry + i*dentry_size + inode_off));
            return 0;
        }
    }
    return -1;
}
int32_t read_dentry_by_index (uint32_t index, struct dentry* dt){
    if( dentry == NULL )return -1;
    if( index < 0 || index > dir_count )return -1;
    (*dt).filetype=*((uint32_t*)(dentry + index*dentry_size + file_type_off));
    (*dt).inode_num=*((uint32_t*)(dentry + index*dentry_size + inode_off));
    return 0;
}
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    int i,j,l_read,st,data_idx,inode_length;
    if( buf == NULL )return -1;
    if( inode < 0 || inode > inode_count )return -1;
    l_read=0;
    inode_length=*((uint32_t*)(node + block_size*inode + length_off));
    //printf("%d\n",inode_length);
    for( i = (1+offset / block_size); i <= inode_length; i++ ){                              // inode line-length

        st= (i == 1+offset / block_size)?(offset-offset/block_size*block_size):0;
        //printf("%d %d\n",i,st);
        data_idx=*((uint32_t*)(node + block_size*inode + i*4));
        if( data_idx<0 || data_idx>data_count )return -1;
        
        for ( j = st ; j< block_size; j++ ){                                                   // in data_block    
            buf[l_read++]=*((uint8_t*)( data_b+block_size*data_idx+j ));//data_b[ node[inode].data[i] ].d[j];
            if(l_read==length)return length;
        }
    }
    return 0;
}

int32_t show_ls (){
    int i,j;
    char ch[32]="verylargetextwithverylongname.tx";
    char buf[20000];
    // clear();
    // for( i=0; i<dir_count; i++){
    //     for( j=0;j<name_length;j++ ){
    //         printf("%c",*((char*)(dentry+i*dentry_size+j)));
    //     }
    //     printf("\n");
    // }
    //printf("LOOOOOOK WHAT I FOUND: %d\n",read_dentry_by_index(10,&test));
    printf("LOOOOOOK WHAT I FOUND: %d\n",read_dentry_by_name (ch,&test));
    // printf("%d\n",test.inode_num);
    clear();
    printf("LOOOOOOK WHAT I FOUND: %d\n",read_data(test.inode_num,1,buf,1000));
    for(i=0;i<1000;i++)printf("%c",buf[i]);
    printf("\n");
    return 0;
}


/*
some questions:
1. read_data, check inode is within the valid range (0~boot.inode_count)
2. why lecture call read_dentry_by_name in that way
3. do we need to initialize every entry in the file system structure
4. data block is bad, what is bad

1.  why there are two files which are not in the folder but in the directory

*/
