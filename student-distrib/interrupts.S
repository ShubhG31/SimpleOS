.globl keyboard_handler
# .globl used because this function is called outside the program

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
