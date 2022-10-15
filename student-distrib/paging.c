#include "paging.h"
#include "types.h"

/* The Pages Table and Directory itself (declared in paging.c */

/* initialization */
extern void paging_init(){
    int i;
    // printf("function start\n");
    for( i = 0; i < NUM_DIR; i++ ){
        page_directory[i].present=0;
        page_directory[i].RW=0;
        page_directory[i].PWT=0; //
        page_directory[i].PCD=1;
        page_directory[i].A=0;
        page_directory[i].avl_=0;
        page_directory[i].ps=0;
        page_directory[i].G=1;
        page_directory[i].AVL=0;
        page_directory[i].offset_31_12=0;//*(page_table[i*1024]);
    }
    // printf("before the inits table loop\n"); 
    page_directory[0].present=1;
    page_directory[0].RW=0;
    page_directory[0].PWT=0; //
    page_directory[0].PCD=1;
    page_directory[0].A=0;
    page_directory[0].avl_=0;
    page_directory[0].ps=0;
    page_directory[0].G=1;
    page_directory[0].AVL=0;
    page_directory[0].offset_31_12=((uint32_t)(page_table))>>12;
    
    page_directory[1].present=1;
    page_directory[1].RW=0;
    page_directory[1].PWT=0; //
    page_directory[1].PCD=0;
    page_directory[1].A=0;
    page_directory[1].avl_=0;
    page_directory[1].ps=1;
    page_directory[1].G=1;
    page_directory[1].AVL=0;
    page_directory[1].offset_31_12=1<<10;

//    printf("before the table loop\n"); 
    for( i = 0; i < NUM_TABLE; i++){
        page_table[i].present=0;
        page_table[i].RW=0;
        page_table[i].US=0;
        page_table[i].PWT=0;
        page_table[i].PCD=1;
        page_table[i].A=0;
        page_table[i].D=0;
        page_table[i].PAT=0;
        page_table[i].G=1;
        page_table[i].AVL=0;
        page_table[i].offset_31_22=0;   //
    }
    // clear();
    // printf("before the wacky loop\n");
    for( i = 184; i <= 184 ; i++){
        page_table[i].present=1;
        page_table[i].RW=0;
        page_table[i].US=0;
        page_table[i].PWT=0;
        page_table[i].PCD=0;
        page_table[i].A=0;
        page_table[i].D=0;
        page_table[i].PAT=0;
        page_table[i].G=1;
        page_table[i].AVL=0;
        page_table[i].offset_31_22=i;   //
    }
    // printf("after the wacky loop\n");
    LoadPagingDirectory((unsigned int*)page_directory);
    // printf("-----------------------------\n");
    EnablePaging();
    // printf("finish enable ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    return;
}
/*
extern void LoadPagingDirectory(unsigned int* cr){
    int box;
    if(cr==NULL)return;
    asm ("movl %%eax, %%cr3"
        :"=r" (box)
        :"r"(cr)
        :"%eax"
    );
    return;
}

extern void EnablePaging(){
    asm ("movl %%cr4, %%eax"
        "orl $0x00000010, %%eax"
        "movl %%eax, %%cr4"
        "movl %%cr0, %%eax"
        "orl $0x80000000, %%eax"
        "movl %%eax, %%cr0"
    );
    return;
}
*/