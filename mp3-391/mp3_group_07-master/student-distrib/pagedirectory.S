# Loading page

.text
.globl loadPageDirectory

loadPageDirectory:
push %ebp
mov %esp, %ebp
mov 8(%esp), %eax
mov %eax, %cr3						# Set CR3 to be address of the page table
mov %ebp, %esp
pop %ebp
ret
