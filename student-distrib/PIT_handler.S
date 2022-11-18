
.globl PIT_handler

PIT_handler:
    pushl %ebp
    movl %esp, %ebp
    pushal
    pushfl
    call schedule
    popfl
    popal
    leave
    iret

