.globl system_halt, system_execute, system_read, system_write, system_open, system_close, system_getargs, system_vidmap

.globl keyboard_handler
# .globl used because this function is called outside the program

Saved_eax:
        .long 0

# asembly linkage for keyboard interrupt, keyboard_helper
# will be called after the assembly linkage

keyboard_handler:
                pushal 
                pushfl  
                call keyboard_helper 
                popfl  
                popal 
                iret 

.globl RTC_handler
 
# assembly linkage for rtc handler, RTC_handle will be called
# after the assembly linkage

RTC_handler:
                pushal 
                pushfl  
                call RTC_handle 
                popfl  
                popal 
                iret   

.global system_call_handler

system_call_handler:
                sti
                pushal 
                pushfl  
                cmpl $10, %EAX   # see if command given is greater than 10
                jg error
                cmpl $1, %EAX   # see if command given is less than 1 
                jl error
                pushl %edx
                pushl %ecx
                pushl %ebx
                call *system_call_table(,%eax,4)  # jump to specified system call function 
                movl %eax, (Saved_eax)
                popl %edx
                popl %ecx
                popl %ebx
                popfl  
                popal 
                movl (Saved_eax), %eax
                iret
system_call_table: 
        .long 0,system_halt, system_execute, system_read, system_write, system_open, system_close, system_getargs, system_vidmap


error:
    popfl  
    popal 
    movl $-1, %eax
    iret

