# Enable Paging Function
# SOURCE CODE IS FROM OSDEV
.text
.globl enablePaging

enablePaging:
push %ebp
mov %esp, %ebp
mov %cr4, %eax
or  $0x00000010, %eax							# Set paging to 4MB
mov %eax, %cr4
mov %cr0, %eax
or $0x80000000, %eax							# Set 32nd bit to ENABLE PAGING
mov %eax, %cr0
mov %ebp, %esp
pop %ebp
ret
