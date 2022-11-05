.global IRET_prepare, label

IRET_prepare:
    pushl %ebp
    movl %esp, %ebp
    pushl   %ebp
    movl    %ebp, %esp
    movl   8(%ebp),%ecx
#     pushl   $USER_DS
#     pushl   $0x8400000
#     pushfl
#     popl    %eax
#     orl     $0x0200, %eax
#     pushl   %eax

#     pushl   $USER_CS
#     pushl   4(%ebp)
    #sti
    pushl $0x002B
    pushl $0x083FFFFC
    pushfl
    popl %ebx
    orl $0x0200, %ebx
    pushl %ebx
    pushl $0x0023
    # pushl %ecx
    # pushl $0x080482e8
    pushl (0x08048018)
    iret
    
label:

    leave
    ret
