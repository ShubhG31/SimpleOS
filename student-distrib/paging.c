#include "paging.h"
#include "types.h"
#include "lib.h"

/* The Pages Table and Directory itself (declared in paging.c */

/* initialization */
/* extern void paging_init();
 * Inputs: void
 * Return Value: the complete initialization of paging including the page table and directory
 * Function: initializes paging, page table and directory, in order to be used within virtual memory compared to the typical physical memory */

extern void paging_init(){
    int i;
    // printf("function start\n");
    // intializing the page directory
    for( i = 0; i < NUM_DIR; i++ ){
        page_directory[i].present=0;
        page_directory[i].RW=1; //changed
        page_directory[i].US=0;
        page_directory[i].PWT=0; 
        page_directory[i].PCD=1;        // Tells the program that it is not Video Memory when set to 1  
        page_directory[i].A=0;
        page_directory[i].avl_=0;
        page_directory[i].ps=0;
        page_directory[i].G=1;          // When 1, tells that set to kernel pages
        page_directory[i].AVL=0;
        page_directory[i].offset_31_12=0;//*(page_table[i*1024]);
    }
    // printf("before the inits table loop\n"); 
    page_directory[0].present=1;        // page table is present
    page_directory[0].RW=1; // changed 
    page_directory[0].US=1;
    page_directory[0].PWT=0; //
    page_directory[0].PCD=1;            // Tells the program that it is not Video Memory when set to 1  
    page_directory[0].A=0;
    page_directory[0].avl_=0;
    page_directory[0].ps=0;
    page_directory[0].G=1;               // When 1, tells that set to kernel pages
    page_directory[0].AVL=0;
    page_directory[0].offset_31_12=((uint32_t)(page_table))>>12; // sets the address that points to page table in physical memory
    
    page_directory[1].present=1;         // page table is present
    page_directory[1].RW=1; // changed 
    page_directory[1].US=0; // changed 
    page_directory[1].PWT=0; //
    page_directory[1].PCD=0;
    page_directory[1].A=0;
    page_directory[1].avl_=0;
    page_directory[1].ps=1;               // When 1, tells us that the pages are 4MB 
    page_directory[1].G=1;                // Tells the program that it is not Video Memory when set to 1
    page_directory[1].AVL=0;
    page_directory[1].offset_31_12=1<<10; // Reserves the 21 to 12 bits for a big 4MB page 

//    printf("before the table loop\n");
    // initializing page table 
    for( i = 0; i < NUM_TABLE; i++){
        page_table[i].present=0;
        page_table[i].RW=0;
        page_table[i].US=0;
        page_table[i].PWT=0;
        page_table[i].PCD=1;               // Tells the program that it is not Video Memory when set to 1  
        page_table[i].A=0;
        page_table[i].D=0;
        page_table[i].PAT=0;
        page_table[i].G=1;                 // Tells the program that it is not Video Memory when set to 1
        page_table[i].AVL=0;
        page_table[i].offset_31_22=0;   //
    }
    // clear();
    // printf("before the wacky loop\n");
    // Sets videomem page
    for( i = ENTRIES; i <= ENTRIES ; i++){
        page_table[i].present=1;
        page_table[i].RW=1; // changed 
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

/* Inputs: void
 * Return Value: initialization of paging including the page directory as shown in documents
 * Function: initializing the page table with the right status of 0 or 1 */

int32_t set_new_page(int phy_mem_loc){
    page_directory[32].present=1;         // page table is present
    page_directory[32].RW=1;  // changed 
    page_directory[32].US = 1; //changed
    page_directory[32].PWT=0; //
    page_directory[32].PCD=0;
    page_directory[32].A=0;
    page_directory[32].avl_=0;
    page_directory[32].ps=1;               // When 1, tells us that the pages are 4MB 
    page_directory[32].G=1;                // Tells the program that it is not Video Memory when set to 1
    page_directory[32].AVL=0;
    page_directory[32].offset_31_12=(phy_mem_loc/4)<<10;
    return 1; 
}


int32_t set_video_page(){
    page_directory[36].present=1;         // page table is present
    page_directory[36].RW=1;  // changed 
    page_directory[36].US = 0; //changed
    page_directory[36].PWT=0; //
    page_directory[36].PCD=0;
    page_directory[36].A=0;
    page_directory[36].avl_=0;
    page_directory[36].ps=1;               // When 1, tells us that the pages are 4MB 
    page_directory[36].G=0;                // Tells the program that it is not Video Memory when set to 1
    page_directory[36].AVL=0;
    page_directory[36].offset_31_12=0xB8;
    return 36<<12; 
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
