.data					# section declaration

        # Useful offset constants for accessing members of a 
        # struct mp1_blink_struct structure
        LOCATION   = 0    
        ON_CHAR    = 2
        OFF_CHAR   = 3 
        ON_LENGTH  = 4
        OFF_LENGTH = 6
        COUNTDOWN  = 8
        STATUS     = 10
        NEXT       = 12

        STRUCT_SIZE = 16

# Pointer to head of list (initialized to NULL)
mp1_list_head:
        .long   0
		
jumptable:                             #jump table for going to appropriate dispatcher command.
        .long mp1_ioctl_add, mp1_ioctl_remove, mp1_ioctl_find, mp1_ioctl_sync


.text					# section declaration

# Export the function symbol names

.global mp1_rtc_tasklet
.global mp1_ioctl

# void mp1_poke(void);
#
# Interface: Register-based arguments (not C-style)
#    Inputs: %cl  - The byte you wish to write
#            %eax - Offset from the start of video memory that you wish
#                   to write to
#   Outputs: Text-mode video screen is written to at location %eax with
#            the byte in %cl
# Registers: Clobbers EDX
mp1_poke:
        
	movl    vmem_base_addr(,1),%edx
	movb    %cl,(%edx,%eax,1)
	ret

#Go through mp1_list_head list. First, decrement countdown. If countdown = 0
#after decrement, check status. If status = 1, location has on_char character. 
# check. if status = 0, location has off_char character. Then, tasklet should put opposite char
#out to video mem by calling mp1_poke. Then, countdown = opposite length. 
#Then, go to next list element. Once next = null, function returns.

mp1_rtc_tasklet:
#SAVE CALLEE REGS
	push 	%ebp					
	movl 	%esp, %ebp
	pushl 	%ebx
	pushl 	%esi
	pushl 	%edi
	movl 	mp1_list_head, %ebx	#EBX = LIST_HEAD
	
#LOOP START
begin_tasklet: 
	cmpl 	$0, %ebx				#IS LIST NULL? IF SO, DONE
	je 		finish_tasklet
	
	movw 	COUNTDOWN(%ebx), %si 		#SI HOLDS COUNTDOWN FOR DECREMENT
	decw 	%si							#DECREMENT COUNTDOWN
	movw 	%si, COUNTDOWN(%ebx)
	cmpw 	$0, COUNTDOWN(%ebx)			#IS COUNTDOWN = 0?
	jg 		next_struct				# check. if NOT, KEEP GOING
	cmpw 	$1, STATUS(%ebx)		#IS THE CHARACTER ON?
	je 		char_on_to_off			# check. if so, move on

#CHANGE FROM OFF TO ON
	movzwl 	LOCATION(%ebx), %eax	#EAX holds the location
	shll 	$1, %eax			#MOVE TO THE LEFT TO MULTIPLY BY 2
	movb 	ON_CHAR(%ebx), %cl 		#ONCHAR = CL
	call 	mp1_poke				#POKE TO TURN ON
	movw 	ON_LENGTH(%ebx), %cx		#CX = ONLENGTH
	movw 	%cx, COUNTDOWN(%ebx)		#COUNTDOWN = ON_LENGTH
	jmp 	status_off_to_on
	
#CHANGE FROM ON TO OFF
char_on_to_off:	
	movzwl 	LOCATION(%ebx), %eax	#EAX HOLDS LOCATION		
	shll 	$1, %eax		#SHIFT LEFT TO MULTIPLY BY TWO
	movb 	OFF_CHAR(%ebx), %cl 			#OFF CHAR = CL
	call 	mp1_poke						#POKE TO TURN OFF
	movw 	OFF_LENGTH(%ebx), %cx			#CX = OFF_LENGTH
	movw 	%cx, COUNTDOWN(%ebx)			#COUNTDOWN = OFF_LENGTH
	jmp 	status_off_to_on
	
#CHANGE STATUS TO ONE
status_off_to_on:
	cmpw 	$1, STATUS(%ebx)		#IS STATUS ONE?
	je 		status_one_to_zero			# check. if SO, CHANGE IT TO ZERO
	movw 	STATUS(%ebx), %si 		#SI = STATUS
	incw 	%si						#CHANGE TO ONE
	movw 	%si, STATUS(%ebx)		#STATUS = 1.
	jmp 	next_struct
	
#CHANGE STATUS TO ZERO
status_one_to_zero:
	movw 	STATUS(%ebx), %si		#SI = STATUS
	decw 	%si						#SI = 0
	movw 	%si, STATUS(%ebx)		#STATUS = 0
	jmp 	next_struct
	
#NEXT STRUCT
next_struct:
	movl 	NEXT(%ebx), %ebx
	jmp 	begin_tasklet

#DONE WITH TASKLET
finish_tasklet:
	popl 	%edi					
	popl 	%esi
	popl 	%ebx
	leave
	ret

#Jump table that chooses the function.
# check. if 0 < cmd < 3, valid cmd and jump to appropriate value. Otherwise, invalid and return -1.
mp1_ioctl:
	movl    8(%esp), %eax              #move stack pointer to register
	cmpl    $0, %eax                   #Compare eax with 0.
	jl      invalid_cmd                  # check. if cmd < 0, invalid. return -1
	cmpl    $3, %eax                   #compare eax with 3.
	jg      invalid_cmd                  # check. if cmd > 3, invalid. return -1.
	jmp     *jumptable(,%eax, 4)        #go to the jump table.

invalid_cmd:
    movl    $-1, %eax                  #return -1. EAX = return value.
    ret
	
	
#argument is a pointer to mp1_blink_struct. Allocate memory using mp1_malloc
#to copy the struct. Copy the struct to the memory (HEAP) using mp1_copy_from_user.
#countdown = on_length. status = 1. Insert struct at head of list using the mp1_list_head ptr
#Call mp1_poke to display character.
#Return 0 if successful.
# check. if mp1_malloc returns null, return -1. If mp1_copy_from_user can't copy all bytes, return
#the number of bytes not able to copy, so if it returns anything other than 0, return -1.
# check. if location not between 0 and 1999, return -1. If memory allocated and theres an error, free memory with mp1_free.
mp1_ioctl_add:
	pushl 	%ebp					#Save regs
	movl 	%esp, %ebp
	pushl 	%ebx
	pushl 	%esi
	pushl 	%edi

	movl	8(%ebp), %eax
	cmpl	$0, %eax
	je		add_fail
#MALLOC
	pushl 	$STRUCT_SIZE			#STRUCT_SIZE is how many bytes allocated
	call 	mp1_malloc				#allocate the heap
	popl 	%edx						#get rid of struct_size
	movl 	%eax, %esi				#Malloc should return the pointer to heap.
	cmpl 	$0, %eax				#Did malloc return NULL?
	je 		add_fail

#COPY STRUCT TO HEAP	
	movl 	8(%ebp), %edx 		#pointer to the struct to edx.
	pushl 	$STRUCT_SIZE		#Push the struct size to heap N
	pushl 	%edx				#push argument to stack. FROM
	pushl 	%esi				#push heap pointer to stack. TO
	call 	mp1_copy_from_user
	popl	%edx
	popl	%edx					#Pop the stack
	popl	%edx
	cmpl 	$0, %eax				#Did copy work?
	jne 	malloc_failed
	
#LOCATION BOUND CHECK
	cmpw 	$0, LOCATION(%esi)		#Check if location < 0
	jb 		malloc_failed 			# check. if its less than 0, failed
	cmpw 	$1999, LOCATION(%esi)	#Check if location > 1999
	jae 	malloc_failed 			# check. if its greater than 1999, failed.
	
#COUNTDOWN, STATUS, LENGTH CHANGE
	movw 	ON_LENGTH(%esi), %cx 	#cx = on_length
	movw 	%cx, COUNTDOWN(%esi)	#countdown = on length
	movw 	$1, STATUS(%esi)		#status = 1
	
#Placing new structure at the head of the list
	movl 	mp1_list_head, %edx 	#put in the list_head to edx.
	movl 	%edx, NEXT(%esi) 		#next = the the current list head
	movl 	%esi, mp1_list_head 	#move to the next struct
	
#UPDATE DISPLAY
	movzwl 	LOCATION(%esi), %eax 	#EAX needs to hold the location
	shll 	$1, %eax				#Shift lift to multiply by two
	movb 	ON_CHAR(%esi), %cl 		#CL is used to display character
	call 	mp1_poke
	jmp 	success
	
#ADD WORKED
success:					
	movl 	$0, %eax				#it worked, so return 0.
	popl 	%edi					
	popl 	%esi
	popl 	%ebx
	leave
	ret

#ADD FAILED
malloc_failed:
	pushl	%esi
	call	mp1_free
	addl	$4, %esp
add_fail:
	movl	$-1, %eax
	popl 	%edi					
	popl 	%esi
	popl 	%ebx
	leave
	ret

	
	
#int location as parameter.
#Go through mp1_list_head list. Find elements whose location = arg.
# check. if element exists, remove it from linked list, free its memory by calling mp1_free,
#and return 0. If no element matches the location arg, return -1.
mp1_ioctl_remove:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	pushl	%esi
	pushl	%edi
	
	movl 	mp1_list_head, %eax  	#eax = list head
    movl	8(%ebp), %ebx			#ebx = argument (location of struct we want to remove)
	cmpl 	$0,%eax					#is the list head null?
    je 		remove_fail				#list head null, then failed to remove.
    movzwl 	(%eax), %edx   			#%edx <- location
    cmpl 	%edx, %ebx    			# compare head's location and arg's
    je 		next_struct_remove		# check. if they're the same, remove the next struct.
    pushl 	%ebx					#push the arg.
    call 	finder_helper			#find the arg
    popl	%ebx        			#pop arg from the stack.
    cmpl 	$0, %eax				#did we find it?
    je 		remove_fail				# check. if return = 0, we didn't find it.
    movl 	NEXT(%eax), %ecx		#ecx holds the next struct if we found it.
    movl 	NEXT(%ecx), %edx		#next,next  location is now the current location.
    movl 	%edx, NEXT(%eax)		#the next, next location is now the next one.
    movl 	%ecx, %eax				#next location is now the current location
    jmp 	remove_free

#remove the head struct
next_struct_remove:
    movl	NEXT(%eax), %edx			#edx = next struct
    movl 	%edx, mp1_list_head			#list_head starts at next struct

remove_free:
    pushl 	%eax						#push the struct to stack.
    call 	mp1_free					#free the stack
    popl 	%edi       					#deallocate the stack.
	jmp		remove_success

remove_success:
	movl	$0, %eax		#return 0.
	popl	%edi
	popl	%esi
	popl	%ebx
	leave
    ret
	
remove_fail:
    movl	$-1, %eax		#return -1.
    popl	%edi
	popl	%esi
	popl	%ebx
	leave
	ret
	
#Pointer to mp1_blink_struct, like add function does. Input param = location.
# check. if pointer = valid (goes to a struct), search mp1_list_head list for an element
#that matches the location. Then, copy entire element (a blink struct) to the
#user-level structure pointed to by param passed in (use mp1_copy_to_user).
# check. if no matching location in the list, return -1. Otherwise, return 0.
mp1_ioctl_find:
#SAVE CALLEE REGS
	pushl 	%ebp				
	movl 	%esp, %ebp
	pushl 	%ebx
	pushl 	%esi
	pushl 	%edi

#BEGIN FIND	
find_start:	
	movl 	8(%ebp), %esi 		#ARG (location) in ECX
	cmpl 	$0, %esi 			#is the location NULL?
	je 		find_fail			# check. if so, nothing to remove and we're done. Otherwise, check the list
	
	movw 	(%esi), %si 			#location specifically in cx now.
	movl 	mp1_list_head, %edx 	#edx holds start of the list.
	cmpl 	$0, %edx				# check. if list_head is null, find failed.
	je 		find_fail
	movw	(%edx), %dx
	cmpw	%si, %dx 			#is the arg = current struct?
	je 		find_success		# check. if yes, we found it!
	movzwl 	%si, %esi 			#move CX back to ecx, but zero extend
	movl 	%esi, %ebx				#push to stack.
	pushl	%ebx
	call 	finder_helper
	popl 	%ebx
	cmpl 	$0, %eax 			#Did we find it?
	je 		find_fail			# check. if finder_helper returns 1, we failed.
	movl 	NEXT(%eax), %edx 	#list moves on.
	
find_success:
	movl 	8(%ebp), %esi
	pushl	$STRUCT_SIZE		#struct size. N.
	pushl	%edx				#pointer to arg. FROM
	pushl 	%esi				#pointer to heap.	TO.
	call	mp1_copy_to_user
	popl	%ebx
	popl	%ebx
	popl	%ebx
	cmpl 	$0, %eax
	jne		find_fail
	movl	$0, %eax			#return 0
	popl 	%edi
	popl	%esi
	popl	%ebx
	leave
	ret
	
find_fail:
	movl	$-1, %eax			#return -1
	popl 	%edi
	popl	%esi
	popl	%ebx
	leave
	ret
	
#FINDER HELPER FUNCTION	
finder_helper:
    movl 	mp1_list_head, %ebx			#ebx = mp1_list_head
    cmpl 	$0, %ebx					#check if null. if so, failed.
    je 		finder_helper_fail
	cmpl	$0, NEXT(%ebx)				#check if next one is null. if so, failed.
	je		finder_helper_fail
    movl 	NEXT(%ebx), %ecx			#ecx holds next value otherwise.
finder_helper_loop:
    cmpl 	$0, %ecx					#check if its null in the loop.
    je 		finder_helper_fail
    movzwl 	(%ecx), %eax				#move the struct to eax.
	movl	4(%esp), %edx				#move the argument (location we're looking for) to edx
    cmpl 	%edx, %eax  		#do the locations match?
    je 		finder_success		# check. if so, return it.
    movl 	%ecx, %ebx			#ebx = struct
    movl 	NEXT(%ecx), %ecx	#next struct is now the current struct.
    jmp 	finder_helper_loop

finder_success:
    movl 	%ebx, %eax				#return the struct.
    ret

finder_helper_fail:
    movl 	$0, %eax				#return 0.
    ret
#END FINDER HELPER FUNCTION
	
	


#arg is actually 2 two-byte unsigned short integers within one argument.
#first integer on upper 16 bits of arg. second integer is stored in lower 16 bits.
#Extract them from the arg.
#first_int = location of first blinking char. second_int = location of second blinking
#char that become synced with first. Search mp1_list_head list for elements with
#locations that match the two ints. Then, copy on_length, off_length, countdown, status
#from first element to second element. Then, call mp1_poke to udpate display.
#Return 0 for success. Return -1 for failure.
mp1_ioctl_sync:	
sync_start:
	movl	4(%esp), %eax		#arg = %eax
	movzwl	%ax, %ebx			#ebx holds second location.
	shrl	$16, %eax			#shift right to get the next 16 bits.
	movzwl	%ax, %ecx			#ecx holds the first location.
	movl	mp1_list_head, %edx			#edx points to list head
	cmpl	$0, %edx					#is list head null?
	je		sync_fail					# check. if yes, we've finished sync.
	movzwl	(%edx), %edx
	cmpl	%edx, %eax					#is the first location = current struct?
	jne		sync_find_one				# check. if not,start finding the first one
	movl	mp1_list_head, %eax			#eax now points to struct
	jmp		sync_find_helper				#find the second one.
	

#Find the first location
sync_find_one:
	pushl	%ebx					#second location
	pushl	%ecx					#first location
	call	finder_helper			#call the finder helper
	popl	%ebx					#pop the stack.
	popl	%ebx
	cmpl	$0,	%eax				#check if finder_helper returned 1 (not found)
	je		sync_fail				#since it failed, we're done.
	movl	NEXT(%eax), %ecx		#move the next struct to ecx, the first location

sync_find_helper:
	movl	mp1_list_head, %esi
	movzwl	(%esi), %eax
	cmpl	%eax, %ebx				#is the list head the second location?
	jne		sync_find_two			# check. if list head not second location, find it.
	movl	mp1_list_head, %ebx		#make ebx the list head. 
	jmp		sync_helper				#go to helper function
	
sync_find_two:
	pushl	%ebx					#push second location
	call	finder_helper			#find it
	popl	%esi					#pop the stack.
	cmpl	$0, %eax				#did it find the second location?
	je		sync_fail				# check. if not, we're done.
	movl	NEXT(%eax), %ebx		#move the next location to ebx.

sync_helper:
	movw 	ON_LENGTH(%ecx), %di	#di = on_length of location 1
	movw	%di, ON_LENGTH(%ebx)	#on_length2 = on_length1
    movw 	OFF_LENGTH(%ecx), %di	#di = off_length1
	movw	%di, OFF_LENGTH(%ebx)	#off_length2 = off_length1
    movw 	COUNTDOWN(%ecx), %di	#di = countdown1
	movw	%di, COUNTDOWN(%ebx)	#countdown2 = countdown1
    movw 	STATUS(%ecx), %di		#di = status1
	movw	%di, STATUS(%ebx)		#status2 = status1
	
    movzwl 	(%ebx), %eax
    shll 	$1, %eax					#multiply the location.
    cmpl 	$1999, %eax        			#is location > 1999?
    ja 		sync_fail					# check. if location out of bounds, failed.
    cmpw 	$0, STATUS(%ebx)			#Check if char is on or off
    je 		turn_off					#Turn off the character if status = 0.
    movb 	ON_CHAR(%ebx), %cl			#turn on the character
    jmp 	sync_complete				#successfully complete the sync.
turn_off:
    movb 	OFF_CHAR(%ebx), %cl			#Turn off the character

sync_complete:
    call 	mp1_poke
    movl 	$0, %eax
    ret
	
sync_fail:
	movl	$-1, %eax
	ret
	
.end
