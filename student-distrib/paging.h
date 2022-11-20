#ifndef _paging_
#define _paging_
#include "types.h"
#include "lib.h"

#define NUM_DIR (1024)
#define NUM_TABLE (1024)
#define NUM_PAGE (1024*1024*4096)
#define ENTRIES 184

/* page directory entry */
typedef union page_directory_4k {
    uint32_t val;
    struct {
        uint32_t present        : 1;
        uint32_t RW             : 1;        
        uint32_t US             : 1;
        uint32_t PWT            : 1;
        uint32_t PCD            : 1;       
        uint32_t A              : 1;
        uint32_t avl_           : 1;
        uint32_t ps             : 1;        //set to be zero for 4kb page table
        uint32_t G              : 1;
        uint32_t AVL            : 3;
        uint32_t offset_31_12   : 20;
    } __attribute__ ((packed));
} page_directory_4k_t;

typedef union page_directory_4m {
    uint32_t val;
    struct {
        uint32_t present        : 1;
        uint32_t RW             : 1;        
        uint32_t US             : 1;
        uint32_t PWT            : 1;
        uint32_t PCD            : 1;       
        uint32_t A              : 1;
        uint32_t D              : 1;
        uint32_t ps             : 1;        //set to be zero for 4kb page table
        uint32_t G              : 1;
        uint32_t avl_           : 3;
        uint32_t PAT            : 1;
        uint32_t reserved       : 9;
        uint32_t offset_31_22   : 10;
    } __attribute__ ((packed));
} page_directory_4m_t;

/* page table entry */
typedef union page_table {
    uint32_t val;
    struct {
        uint32_t present        : 1;
        uint32_t RW             : 1;
        uint32_t US             : 1;
        uint32_t PWT            : 1;
        uint32_t PCD            : 1;
        uint32_t A              : 1;
        uint32_t D              : 1;
        uint32_t PAT            : 1;
        uint32_t G              : 1;
        uint32_t AVL            : 3;
        uint32_t offset_31_12   : 20;
    } __attribute__ ((packed));
} page_table_t;


/* The Pages Table and Directory itself (declared in paging.S */
//extern page_table_t page_table[NUM_TABLE];
/* The IDT itself (declared in x86_desc.S */
//extern page_directory_t page_directory[NUM_DIR];
/* The descriptor used to load the IDTR */


page_table_t page_table[NUM_TABLE]  __attribute__((aligned(4096)));
page_directory_4k_t page_directory[NUM_DIR]  __attribute__((aligned(4096)));

extern void paging_init();
extern void LoadPagingDirectory(unsigned int*);
extern void EnablePaging();
extern int32_t set_new_page(int phy_mem_loc);
extern int32_t set_video_page();
extern int32_t set_invisible_video_page(int main_terminal);
extern int32_t map_B8_B9_table(int main_terminal);
extern int get_B8_B9_table();
#endif
