.text
.globl LoadPagingDirectory, EnablePaging
.globl page_table, page_directory

.align 4
LoadPagingDirectory:
    pushl %ebp
    movl %esp, %ebp
    movl 8(%esp), %eax
    movl %eax, %cr3
    movl %ebp, %esp
    popl %ebp
    ret

EnablePaging:
    pushl %ebp
    movl %esp, %ebp
    movl %cr4, %eax
    orl $0x00000010, %eax
    movl %eax, %cr4
    movl %cr0, %eax
    orl $0x80000000, %eax
    movl %eax, %cr0
    movl %ebp, %esp
    popl %ebp
    ret

.align 4096
page_table:
    .rept 1024
        .long 0
    .endr
page_directory:
    .rept 1024
        .long 0
    .endr
