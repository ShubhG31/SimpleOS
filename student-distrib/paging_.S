.text
.globl LoadPagingDirectory, EnablePaging
.globl page_table, page_directory
.globl empty_vid_map

.align 4

# uses address of the page directory
# registers used: eax
# function: loads the page directory in the cr3 register 

LoadPagingDirectory:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%esp), %eax
    movl %eax, %cr3
    movl %ebp, %esp
    popl %ebp
    ret

# uses address of the page directory like before
# registers used: eax 
# function: enables paging, specifically setting registers and bits to enable paging. setting up something in cr4 to know that the paging process has been set up

EnablePaging:
    pushl %ebp
    movl %esp, %ebp
    movl %cr4, %eax
    orl $0x00000010, %eax       # or'ing the 2nd bit of cr4 
    movl %eax, %cr4             # cr4 tells that paging is set up
    movl %cr0, %eax
    orl $0x80000000, %eax #enabling teh bit for paging in cr4 8MB
    movl %eax, %cr0
    movl %ebp, %esp
    popl %ebp
    ret

# takes the number of entries that can go in a page table and page directory
# both tables contain 1024 entries each of 4 bytes hence .rept 1024

.align 4096
page_table:
    .rept 1024
        .long 0
    .endr
page_directory:
    .rept 1024
        .long 0
    .endr
empty_vid_map:
    .rept 1024
        .long 0
    .endr
