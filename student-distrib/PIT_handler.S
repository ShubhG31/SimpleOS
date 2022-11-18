
.globl PIT_handler

PIT_handler:
    pushl %ebp
    movl %esp, %ebp
    pushal
    pushfl
    call schdule
    popfl
    popal
    leave
    iret

