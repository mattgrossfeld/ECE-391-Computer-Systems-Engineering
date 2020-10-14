# Taken from OSDEV 
# RTC Wrapper to fix triple fault error 
# and handle ISR routines. Compiler does not understand
# what is going on (registers and stacks need to be preserved 
# between asm statements, therefore optimizer will likely 
# corrupt function. Therefore, we need a wrapper to handle 
# interrupt. 

# RTC Wrapper Function
.text
.globl   rtc_wrapper
.align   4
 
rtc_wrapper:
    pushal
    call interrupt_rtc
    popal
    iret
