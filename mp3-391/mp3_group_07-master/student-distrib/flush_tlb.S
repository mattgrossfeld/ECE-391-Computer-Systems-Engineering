# Flush TLB
.text
.globl flush_tlb

flush_tlb:
push %ebp
mov %esp, %ebp
movl %cr3, %eax
movl %eax, %cr3
mov %ebp, %esp
pop %ebp
ret
