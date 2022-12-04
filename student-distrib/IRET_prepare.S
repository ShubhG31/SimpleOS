.data 
    USER_DS = 0x002B
    USER_CS = 0x0023
    Nineth_bit = 0x0200
    EIP = 0x08048018
    bottom = 0x083FFFFC

.global IRET_prepare, label

#   created a separate file for the assembly code for IRET    

IRET_prepare:
    movl 4(%esp), %edx
    pushl $USER_DS
    pushl $bottom
    pushfl
    popl %ebx
    orl $Nineth_bit, %ebx
    pushl %ebx
    pushl $USER_CS
    # movl $EIP, %ebx

    # pushl (%ebx)
    pushl %edx
    iret
    
label:
#    popl %esp
#    popl %ebp
#    leave
    ret
