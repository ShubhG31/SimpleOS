

.globl keyboard_handler
keyboard_handler:
                pushal 
                pushfl  
                call keyboard_helper 
                popfl  
                popal 
                iret 
                