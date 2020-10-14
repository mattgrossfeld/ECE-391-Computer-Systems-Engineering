# Taken from OSDEV 

# System Call Wrapper Function
.text
.globl   systemcall_wrapper
 
systemcall_wrapper:
	pushl %ecx
	pushl %edx
	pushl %ebx
	pushl %esi
	pushl %edi
	pushfl
	cmpl $10, %eax
    ja error									# return error if invalid
    cmpl $1, %eax
    jb error
	pushl %edx
	pushl %ecx
	pushl %ebx									# check if call number is greater than 4
	call *jumptable(,%eax,4)					# jump to core functions
	addl $12, %esp
	cmpl $-1, %eax
	je error
	jmp done
	
		
error:
	movl $-1, %eax
	popfl
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	iret
				
done:
	popfl
	popl %edi
	popl %esi
	popl %ebx
	popl %edx
	popl %ecx
	iret	
	
jumptable:
		.long error, halt, execute, read, write, open, close, getargs, vidmap, set_handler, sigreturn
