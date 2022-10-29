.global IRET_prepare

IRET_prepare:
    pushl   %ebp
    movl    %ebp, %esp
    pushl   $USER_DS
    pushl   $0x8400000
    pushfl
    popl    %eax
    orl     $0x0200, %eax
    pushl   %eax

    pushl   $USER_CS
    pushl   4(%ebp)
    iret
