# Taken from OSDEV 
# Keyboard Wrapper to fix triple fault error 
# and handle ISR routines. Compiler does not understand
# what is going on (registers and stacks need to be preserved 
# between asm statements, therefore optimizer will likely 
# corrupt function. Therefore, we need a wrapper to handle 
# interrupt. 

# Keyboard Wrapper Function
.text
.globl   keyboard_wrapper
.align   4
 
keyboard_wrapper:
    pushal
    call interrupt_keyboard					# Calls keyboard interrupt handler
    popal
    iret
