#include "file_sys.h"
#include "file_sys_driver.h"

// #define name_length 32
// #define block_size 4096
// #define dentry_size 64
// #define file_name_off 0
// #define file_type_off 32
// #define inode_off 36
// #define length_off 0

static unsigned int boot;
static unsigned int dentry; 
static unsigned int node; 
static unsigned int data_b;
static uint32_t dir_count, inode_count, data_count;
struct dentry test;

void test_file_driver_small_file();
void test_file_driver_large_file();
void test_file_driver_executable_file();
void test_dir_driver();
void test_show_files();
void test_show_frame();
void test_file_driver_read_twice();

int32_t load_fss(unsigned int mod_start){
    boot    = mod_start;
    dentry  = mod_start+dentry_size;
    node    = mod_start+block_size;
    dir_count   = *((int*)mod_start);
    inode_count = *((int*)(mod_start+4));
    data_count  = *((int*)(mod_start+8));
    data_b  = mod_start + block_size*(1+inode_count);
    // clear();
    // printf("%u %u %u\n",dir_count,inode_count,data_b);
    // printf("finish loading file system structre\n");
    return 0;
}
int cmp_name(const uint8_t* A, const uint8_t* B){
    int i;
    for(i=0;i<name_length;i++){
        if(A[i]==0&&B[i]==0)return 1;
        if(A[i]!=B[i])return 0;
    }
    return 1;
}
int32_t read_dentry_by_name (const uint8_t* fname, struct dentry* dt){
    int i;
    if( dentry == NULL )return -1;
    (*dt).filetype=-1;
    (*dt).inode_num=-1;
    if( fname == NULL )return -1;
    for( i = 0; i < dir_count; i++ ){
        if(cmp_name((uint8_t*) (dentry + i*dentry_size + file_name_off),fname)){
            // (*dt).filetype=*((uint32_t*)(dentry + i*dentry_size + file_type_off));
            // (*dt).inode_num=*((uint32_t*)(dentry + i*dentry_size + inode_off));
            // return 0;
            return read_dentry_by_index(i,dt);
        }
    }
    return -1;
}
int32_t read_dentry_by_index (uint32_t index, struct dentry* dt){
    int i;
    if( dentry == NULL )return -1;
    (*dt).filetype=-1;
    (*dt).inode_num=-1;
    if( index < 0 || index > dir_count )return -1;
    (*dt).filetype=*((uint32_t*)(dentry + index*dentry_size + file_type_off));
    (*dt).inode_num=*((uint32_t*)(dentry + index*dentry_size + inode_off));
    for( i=0; i<32; i++){
        (*dt).filename[i]=*((uint8_t*)(dentry + index*dentry_size+i ));
    }
    return 0;
}
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length){
    int i,j,l_read,st,data_idx,inode_length;
    if( buf == NULL )return -1;
    if( inode < 0 || inode > inode_count )return -1;
    l_read=0;
    inode_length=*((uint32_t*)(node + block_size*inode + length_off));
    //printf("%d\n",inode_length);
    for( i = (1+offset / block_size); i < block_size/4; i++ ){                              // inode line-length

        st= (i == 1+offset / block_size)?(offset-offset/block_size*block_size):0;
        //printf("%d %d\n",i,st);
        data_idx=*((uint32_t*)(node + block_size*inode + i*4));
        if( data_idx<0 || data_idx>data_count )return -1;
        
        for ( j = st ; j< block_size; j++ ){                                                   // in data_block    
            buf[l_read++]=*((uint8_t*)( data_b+block_size*data_idx+j ));//data_b[ node[inode].data[i] ].d[j];
            if(l_read==length || inode_length == (i-1)*block_size+j+1 )return l_read;
        }
    }
    return -1;
}

int32_t file_sys_test_cases (){
    // test_file_driver_small_file();
    // test_file_driver_large_file();
    // test_file_driver_executable_file();
    // test_file_driver_read_twice();
    test_dir_driver();
    // test_show_files();
    // test_show_frame();
    return 0;
}

void put_number(int x){
    int bit[7],l,i;
    l=0;
    if(x==0){
        putc('0');
        return;
    }
    if(x<0)putc('-'),x=-x;
    while(x){
        bit[l++]=x%10;
        x/=10;
    }
    for( i=l-1;i>=0;i-- ){
        putc((char)(bit[i]+'0'));
    }return;
}

void test_dir_driver(){
    int fd,i,j,re;
    char buf[32];
    clear();
    fd=dir_open(".");
    puts("return value of dir_open:");
    put_number(fd);
    putc('\n');
    // puts("open finish\n");
    for( i = 1; i < dir_count; i++ ){
        put_number(i);
        putc(':');
        re=dir_read(fd,buf,0);
        puts("  file_name: ");
        for( j=0;j<re;j++ )putc(buf[j]);
        putc('\n');
    }
    re=dir_write(fd,NULL,0);
    puts("return value of dir_write:");
    put_number(re);
    putc('\n');
    fd=dir_close(fd);
    puts("return value of dir_close:");
    put_number(fd);
    putc('\n');
    return;
}
void test_file_driver(){
    int fd,i,re;
    char buf[50000];
    // char ch[32]={"frame1.txt"};
    // test_show_frame();
    clear();
    // fd=file_open((uint8_t*)ch);
    // fd=file_open("verylargetextwithverylongname.tx");
    // fd=file_open("frame1.txt");
    // fd=file_open("ls");
    // fd=file_open("frame2.txt"); //dont exist
    fd=file_open("fish");       //dont exist
    puts("return value of file_open:");
    put_number(fd);
    putc('\n');
    if(fd==-1)puts("\n Cannot find the file \n");
    // printf("fd: %d /// open finish\n",fd);

    re=file_read(fd,(void*)buf,60000);
    // re=file_read(fd,(void*)buf,600);
    puts("read finish: read_length: ");
    put_number(re);
    putc('\n');
    puts("---------------------------------------------------------------\n");
    for(i=0;i<re;i++){
        if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
        // if(buf[i]!='\0' && buf[i]!=' ' && buf[i]!='\n')
            putc(buf[i]);
    }
    puts("\n---------------------------------------------------------------\n");
    // puts("i finish reading the fisrt part\n");
    // re=file_read(fd,(void*)buf,100);
    // //printf("read finish: read_length: %d\n",re);
    // puts("read finish: read_length: ");
    // put_number(re);
    // putc('\n');
    // puts("---------------------------------------------------------------\n");
    // for(i=0;i<re;i++){
    //     if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
    //     // if(buf[i]!='\0' && buf[i]!=' ' && buf[i]!='\n')
    //         putc(buf[i]);
    // }
    // putc('\n');
    // puts("---------------------------------------------------------------\n");
    re=file_write(fd,NULL,0);
    puts("return value of file_write:");
    put_number(re);
    putc('\n');
    fd=file_close(fd);
    puts("return value of file_close:");
    put_number(fd);
    putc('\n');

    return;
}
void test_file_driver_small_file(){
    int fd,i,re;
    char buf[50000];
    clear();
    // fd=file_open("frame1.txt");
    fd=file_open("frame0.txt");
    // fd=file_open("frame2.txt");
    puts("return value of file_open:");
    put_number(fd);
    putc('\n');
    if(fd==-1)puts("\n Cannot find the file \n");
    // printf("fd: %d /// open finish\n",fd);

    re=file_read(fd,(void*)buf,60000);
    // re=file_read(fd,(void*)buf,600);
    puts("read finish: read_length: ");
    put_number(re);
    putc('\n');
    puts("---------------------------------------------------------------\n");
    for(i=0;i<re;i++){
        if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
        // if(buf[i]!='\0' && buf[i]!=' ' && buf[i]!='\n')
            putc(buf[i]);
    }
    puts("\n---------------------------------------------------------------\n");
    re=file_write(fd,NULL,0);
    puts("return value of file_write:");
    put_number(re);
    putc('\n');
    fd=file_close(fd);
    puts("return value of file_close:");
    put_number(fd);
    putc('\n');

    return;
}
void test_file_driver_large_file(){
    int fd,i,re;
    char buf[50000];
    clear();
    fd=file_open("verylargetextwithverylongname.tx");
    // fd=file_open("hello");
    // fd=file_open("fish");       //dont exist
    puts("return value of file_open:");
    put_number(fd);
    putc('\n');
    if(fd==-1)puts("\n Cannot find the file \n");

    re=file_read(fd,(void*)buf,60000);
    // re=file_read(fd,(void*)buf,600);
    puts("read finish: read_length: ");
    put_number(re);
    putc('\n');
    puts("---------------------------------------------------------------\n");
    for(i=0;i<re;i++){
        if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
        // if(buf[i]!='\0' && buf[i]!=' ' && buf[i]!='\n')
            putc(buf[i]);
    }
    puts("\n---------------------------------------------------------------\n");
    re=file_write(fd,NULL,0);
    puts("return value of file_write:");
    put_number(re);
    putc('\n');
    fd=file_close(fd);
    puts("return value of file_close:");
    put_number(fd);
    putc('\n');

    return;
}
void test_file_driver_executable_file(){
    int fd,i,re;
    char buf[50000];
    clear();
    // fd=file_open("ls");
    fd=file_open("grep");
    puts("return value of file_open:");
    put_number(fd);
    putc('\n');
    if(fd==-1)puts("\n Cannot find the file \n");
    // printf("fd: %d /// open finish\n",fd);

    re=file_read(fd,(void*)buf,60000);
    // re=file_read(fd,(void*)buf,600);
    puts("read finish: read_length: ");
    put_number(re);
    putc('\n');
    puts("---------------------------------------------------------------\n");
    for(i=0;i<re;i++){
        if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
        // if(buf[i]!='\0' && buf[i]!=' ' && buf[i]!='\n')
            putc(buf[i]);
    }
    puts("\n---------------------------------------------------------------\n");
    re=file_write(fd,NULL,0);
    puts("return value of file_write:");
    put_number(re);
    putc('\n');
    fd=file_close(fd);
    puts("return value of file_close:");
    put_number(fd);
    putc('\n');

    return;
}
void test_file_driver_read_twice(){
    int fd,i,re;
    char buf[50000];
    clear();
    fd=file_open("verylargetextwithverylongname.tx");
    puts("return value of file_open:");
    put_number(fd);
    putc('\n');
    if(fd==-1)puts("\n Cannot find the file \n");

    re=file_read(fd,(void*)buf,100);
    puts("read finish: read_length: ");
    put_number(re);
    putc('\n');
    puts("---------------------------------------------------------------\n");
    for(i=0;i<re;i++){
        if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
        // if(buf[i]!='\0' && buf[i]!=' ' && buf[i]!='\n')
            putc(buf[i]);
    }
    puts("\n---------------------------------------------------------------\n");
    puts("i finish reading the fisrt part\n");
    re=file_read(fd,(void*)buf,100);
    //printf("read finish: read_length: %d\n",re);
    puts("read finish: read_length: ");
    put_number(re);
    putc('\n');
    puts("---------------------------------------------------------------\n");
    for(i=0;i<re;i++){
        if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
        // if(buf[i]!='\0' && buf[i]!=' ' && buf[i]!='\n')
            putc(buf[i]);
    }
    putc('\n');
    puts("---------------------------------------------------------------\n");
    re=file_write(fd,NULL,0);
    puts("return value of file_write:");
    put_number(re);
    putc('\n');
    fd=file_close(fd);
    puts("return value of file_close:");
    put_number(fd);
    putc('\n');

    return;
}
void test_show_files(){
    int i,j;
    clear();
    for( i=0; i<dir_count; i++){
        for( j=0;j<name_length;j++ ){
            // printf("%c",*((char*)(dentry+i*dentry_size+j)));
            putc(*((char*)(dentry+i*dentry_size+j)));
        }
        putc('\n');
    }
    // printf("LOOOOOOK WHAT I FOUND: %d\n",read_dentry_by_index(10,&test));
    return;
}
void test_show_frame(){
    int i,re;
    // char ch[32]="verylargetextwithverylongname.tx";
    char ch[32]="ls";
    char buf[20000];
    clear();
    re=read_dentry_by_name ((uint8_t*)ch,(&test));
    puts("open_file: ");
    put_number(re);
    putc('\n');
    // printf("LOOOOOOK WHAT I FOUND: %d\n",read_dentry_by_name ((uint8_t*)ch,(&test)));
    puts("index: ");
    put_number(test.inode_num);
    putc('\n');
    // printf("%d\n",test.inode_num);
    re=read_data(test.inode_num,0,(void*)buf,6000);
    for(i=0;i<re;i++){
        if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
        // if(buf[i]!='\0' && buf[i]!=' ')
            putc(buf[i]);
    }
    putc('\n');
    // printf("\nfinish first part\n\n");
    // re=read_data(test.inode_num,0,(void*)buf,600);
    // for(i=0;i<re;i++){
    //     if((buf[i]>31 && buf[i]<127)||(buf[i]=='\n'))
    //     // if(buf[i]!='\0' && buf[i]!=' ')
    //         putc(buf[i]);
    //     if(re%80==0)putc('\n');
    // }
    // putc('\n');
    // printf("\n");
    // printf("%d %d %d\n",buf[1],buf[2],buf[3]);
    // putc(buf[1]);
    // putc(buf[2]);
    // putc(buf[3]);
    // putc(buf[5340]);
    // printf("\ndone\n");
    return;
}

int32_t get_length(struct dentry dt){
    return *((uint32_t*)(node + block_size*(dt.inode_num) + length_off));
}

int32_t get_dir_number(){
    return dir_count;
}
/*
some questions:
1. read_data, check inode is within the valid range (0~boot.inode_count)
2. why lecture call read_dentry_by_name in that way
3. do we need to initialize every entry in the file system structure
4. data block is bad, what is bad

1.  why there are two files which are not in the folder but in the directory
2.  file descriptor create our own struct array (PCB) ?
3.  check the printing - how?
4.  file_sys read dir - read what
*/
