

.globl keyboard_handler
keyboard_handler:
                pushal 
                pushfl  
                call keyboard_helper 
                popfl  
                popal 
                iret 



.globl RTC_handler
RTC_handler:
                pushal 
                pushfl  
                call RTC_handle 
                popfl  
                popal 
                iret   