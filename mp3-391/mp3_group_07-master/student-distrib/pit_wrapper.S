# Taken from OSDEV 
# PIT Wrapper to fix triple fault error 
# and handle ISR routines. Compiler does not understand
# what is going on (registers and stacks need to be preserved 
# between asm statements, therefore optimizer will likely 
# corrupt function. Therefore, we need a wrapper to handle 
# interrupt. 

# PIT Wrapper Function
.text
.globl   pit_wrapper
.align   4
 
pit_wrapper:
    pushal
    call interrupt_timer
    popal
    iret

