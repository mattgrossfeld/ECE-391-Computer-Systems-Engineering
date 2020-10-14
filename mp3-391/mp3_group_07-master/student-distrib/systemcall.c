/* System Call Initialization */
#include "systemcall.h"
#include "lib.h"
#include "file.h"
#include "blank_page.h"
#include "x86_desc.h"
#include "terminal.h"
#include "rtc.h"
#include "file.h"
#include "keyboard.h"
#include "enablepaging.h"
#include "scheduler.h"

#define magic_number 0x10000									// An arbitrary number to be used as length for read data
//format: read, write, open, close
fo_t fo_stdin = {terminal_read, no_operation, terminal_open, terminal_close};	//will not write
fo_t fo_stdout = {no_operation, terminal_write, terminal_open, terminal_close};	//will not read
fo_t fo_invalid = {no_operation, no_operation, no_operation, no_operation};
// fd 0 = stdin
// fd 1 = stdout
// user fd's start at 2
int32_t active_processes[MAX_PROCESSES] = {0, 0, 0, 0, 0, 0, 0, 0};
//terminal_t active_terminals[3];
struct terminal_t* active_terminals = (struct terminal_t*) _40MB;
int32_t halted_PID = 0x12345678; //Arbitrary


/* is process open
 * 
 * Helper function for scheduler to see if process is open.
 * Inputs: pid
 * Outputs: 1 if open, 0 if not open
 * Side Effects: None
 */
int32_t is_process_open(int32_t pid)
{
	if (active_processes[pid] == 0)
		return 0;
	return 1;
}

/* scheduler helper
 * 
 * Helper function for scheduler to see if process is on active terminal.
 * Inputs: active process
 * Outputs: 
 * Side Effects: None
 */
int32_t is_process_active(int32_t* active_p)
{
	if (*active_p == 1)
		return 1;
	return 0;
}

/* scheduler helper
 * 
 * Schedule a process
 * Inputs:  current pid, active terminal, active process
 * Outputs: 
 * Side Effects: None
 */
void scheduler_helper(int32_t current_pid, int32_t* active_t, int32_t* active_p)
{
			int32_t x = 0;
			for (x = 0; x < 3; x++)
			{
				if (active_terminals[x].child_processes[current_pid] == 1 && active_processes[current_pid] == 1 && active_terminals[x].is_active == 1)
					*active_p = 1; //process is active.
				else
					*active_p = 0;

				if (active_terminals[x].is_active == 1) //
					*active_t = x;
			}

			return;
}

//When execute fails.
/*no_operation:
* Returns -1 on execute failure
*/
int32_t no_operation()
{
	return -1;
}

/* execute
 * 
 * Start a process
 * Inputs:  command
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t execute (const uint8_t* command){
//	int32_t debug = 0;

	// check for null input
	if (command == NULL)
		return -1;

	/*if(command[0] == 's' && command[1] == 'h' && command[2] == 'e'
		&& command[3] == 'l' && command[4] == 'l' && command[5] == '\0') {
		//execute("shell")
		debug = 1;
	}
*/
	if (active_processes[0] == 1 && active_processes[1] == 1 && active_processes[2] == 1 && 
		active_processes[3] == 1 && active_processes[4] == 1 && active_processes[5] == 1 &&
		active_processes[6] == 1 && active_processes[7] == 1) {
		puts("Cannot Execute: Max Processes Already Reached!\n");
		return 0;
	}

	int elf_bits = 4;											// ELF bit numbers
	uint8_t elf_val[magic_number]; 
	uint8_t arg[magic_number];
	uint8_t elf_check[4] = {0x7F, 0x45, 0x4C, 0x46};			// ELF HEADER. 4 = ELF bit numbers
	uint8_t filename[32];										// holds file name
	uint8_t entry_point[4];										// entry point of the program
	uint8_t* dest_progimg;										// program image destination
	//uint8_t* dest_pcb;										// pcb destination 
	int32_t i = 0;												// counter
	int32_t j = 0;												// counter
	int32_t k = 0;												// counter
	int32_t length = 0;											// length of buffer
	int space_flag = 0;											// holds flag for space check
	dentry_t dentry;											// dentry for file access function
	
	while (command[i] != '\0'){
		// check if length is greater than 32
		//if (i >= 32)
		//	return -1;
		// check if there is a space
		// if yes, it is the end of filename
		if (command[i] == ' ' && space_flag == 0){
			space_flag = 1;
			filename[i] = '\0';
		}
		// check if space has been detected
		if (space_flag == 1){
			// remove spaces if there are more than 1
			if (command[i] != ' '){
				elf_val[j] = command[i];
				//current_process -> arg[j] = command[i];
				j++;
			}
		}
		// space is not detected, save the filename
		else if (space_flag == 0){
			filename[i] = command[i];
		}
		i++;
		if (command[i] == '\0'){
			filename[i] = '\0';
		}
	}

	for (k = 0; k < magic_number; k++){
		arg[k] = elf_val[k];
	}
	//int32_t args = getargs(buf, j);
	// Check if program exists
	if (read_dentry_by_name(filename, &dentry) != 0){
		//active_processes[current_process->pcb_id] = 0;
		return -1;
	}	
	// If exists, check if program is an executable
	length = read_data(dentry.inode_num, 0, elf_val, magic_number);
    for (i = 0; i < elf_bits; i++) {
		// if program is not executable, return -1
		if (elf_val[i] != elf_check[i]){
			return -1;
		}
    }
	
	int32_t current_pid = get_next_pid();

	if(current_pid == -1) {
		active_processes[current_pid] = 0;
		return -1;
	}

	uint8_t curr_tid = get_curr_tid();
	int32_t check = set_activeprocess(curr_tid, current_pid);
	if (check == -1) {
		puts("Cannot Execute: Child Process For This Terminal Already Active!\n");
		return 0;
	}


/*
	if (current_pid == halted_PID && halted_PID != -1)
	{
		active_processes[current_pid] = 0;
		if (get_current_pid() != -1)
			current_pid = get_current_pid();
		halted_PID = -1;
		
	}
*/

	pcb_t * current_process = (pcb_t*)(EIGHT_MB - (EIGHT_KB*(current_pid+1))); //New PCB.
	
	
	
	current_process->pcb_id = current_pid;
	current_process->tid = curr_tid;	

	current_process->fd_array[0].is_open = 1; //Enable flags
	current_process->fd_array[1].is_open = 1; //Enable flags
	current_process->fd_array[0].file_ops = fo_stdin; //Reserve for stdin
	current_process->fd_array[1].file_ops = fo_stdout; //Reserve for stdout
	current_process->num_files = 2;

	int x;
	for (x = 2; x < MAX_FILES; x++) //Create uninitialized processes.
	{
		current_process->fd_array[x].inode = -1; //no current inode 
		current_process->fd_array[x].file_pos = 0; //Not setup
		current_process->fd_array[x].file_ops = fo_invalid; //Might cause errors. Will probably change.
		current_process->fd_array[x].is_open = 0; //Inactive
	}
	
	for (k = 0; k < j; k++){
			current_process -> arg[k] = arg[k];
	}
	
	current_process->arg[j] = '\0';

	current_process->parent_pid = active_terminals[curr_tid].last_shell_pid;

	if(filename[0] == 's' && filename[1] == 'h' && filename[2] == 'e'
		&& filename[3] == 'l' && filename[4] == 'l' && filename[5] == '\0') {

		active_terminals[curr_tid].last_shell_pid = current_pid;

		int32_t next_shell = get_next_shell();

		active_terminals[curr_tid].shells[next_shell] = current_pid;

		if(active_terminals[curr_tid].pid == -1) {
			active_terminals[curr_tid].pid = current_pid;
		}
	}

	// Set up paging
	paging_helper(current_pid);
		
	dest_progimg = (uint8_t *)0x08048000;
	memcpy(dest_progimg, elf_val, length);
	// Note the entry point of the program
	// Entry point is from bit 24 to 27 of executable 
	int y = 0;
	for (i = 24; i <= 27; i++){
		entry_point[y] = elf_val[i];
		y++;
	}

	tss.ss0 = KERNEL_DS; //Sets the kernel DS
	tss.esp0 = EIGHT_MB - (current_pid*EIGHT_KB) - 4; 

	//clear_keyboard();
	//New ESP and EBP for the PCB for the kernel-mode stack.
	asm volatile (
		"movl %%esp, %0;"
		"movl %%ebp, %1;"
		: "=r" (current_process->parent_esp), "=r" (current_process->parent_ebp)
		);

	int32_t execute_rval;

	/* IRET PERFORMS THE FOLLOWING POPS STARTING FROM TOP OF STACK:
	* EIP, CS, EFLAGS, ESP, SS */
	//Setting up the user stack and registers.
	asm volatile (
		"cli;"
		"movw $0x2B, %%ax;"
		"movw %%ax, %%ds;"
		"movw %%ax, %%es;"
		"movw %%ax, %%fs;"
		"movw %%ax, %%gs;"
		"pushl $0x2B;" 
		"pushl $0x083FFFFC;"
		"pushfl;"
		"popl %%eax;"
		"orl $0x200, %%eax;" //Sets the interrupt flag
		"pushl %%eax;"
		"pushl $0x23;"
		"movl %1, %%eax;"
		"pushl (%%eax);"
		"iret;"
		"BACK_TO_EXECUTE:;"
		"leave;"
		"ret;"
		"movl %%eax, %0;"
		: "=r" (execute_rval)
		: "r" (entry_point)
		: "%eax" /*Clobbering EAX*/
		);



	return execute_rval;													// system call execute succesfully
}


/* read
 * 
 * Read an item
 * Inputs:  file descriptor, buffer to write, number of bytes
 * Outputs: bytes read on success
 * Side Effects: None
 */
int32_t read(int32_t fd, void * buf, int32_t nbytes) {

	int32_t read_data;

	pcb_t * current_process = get_this_pcb(); // get current process

	if(buf == NULL || nbytes < 0) { // ensure buf and nbytes are valid
		return -1;
	}

	if(fd < 0 || fd >= MAX_FILES) { // ensure fd is valid
		//fd = 2;
		return -1;
	}

	if(current_process->fd_array[fd].is_open == 0) { // make sure we are reading an open file
		return -1;
	}

	read_data = current_process->fd_array[fd].file_ops.read(fd, buf, nbytes); // return output of relevant read function

	return read_data;
}

/* write
 * 
 * Write an item
 * Inputs:  file descriptor, buffer to write, number of bytes
 * Outputs: bytes written on success
 * Side Effects: None
 */
int32_t write(int32_t fd, const void * buf, int32_t nbytes) {

	int32_t write_data;

	pcb_t * current_process = get_this_pcb(); 

	if(buf == NULL || nbytes < 0) { // check valid params
		return -1;
	}

	if(fd < 0 || fd >= MAX_FILES) { // check valid fd
		return -1;
	}

	if(current_process->fd_array[fd].is_open == 0) { // make sure we are writing an open file
		return -1;
	}

	//check_y();

	write_data = current_process->fd_array[fd].file_ops.write(fd, buf, nbytes);	// return output of relevant write function

	//check_y();

	return write_data;
}

/* open
 * 
 * Open an item
 * Inputs:  filename
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t open(const uint8_t * filename) {

	dentry_t current_dentry;

	if(filename == NULL) {
		return -1;
	}

	//pcb_t * current_process = (pcb_t *)(EIGHT_MB - (EIGHT_KB * current_pid); //need to subtract (Eight_KB * Number_of_open_processes) from eight mb
	pcb_t * current_process = get_this_pcb();

	if(read_dentry_by_name(filename, &current_dentry) == -1) {
		return -1;
	}

	int32_t fd;
	for(fd = 2; fd < MAX_FILES; fd++) {							//start at i = 2 becaue stdin and stdout will be fd 0 and 1
		if(current_process->fd_array[fd].is_open == 0) {
			current_process->fd_array[fd].is_open = 1;
			break;
		}
	}

	if(current_dentry.file_type == 0) { //rtc
		if(rtc_open(filename) == -1) { //ensure there are no errors opening
			return -1;
		}

		current_process->num_files++;
		current_process->fd_array[fd].file_ops.read = &rtc_read;
		current_process->fd_array[fd].file_ops.write = &rtc_write;
		current_process->fd_array[fd].file_ops.open = &rtc_open;
		current_process->fd_array[fd].file_ops.close = &rtc_close;
		current_process->fd_array[fd].inode = 0;
		current_process->fd_array[fd].file_pos = 0;
		current_process->fd_array[fd].is_open = 1;

	} else if(current_dentry.file_type == 1) { //directory
		if(open_directory(filename) == -1) { //ensure there are no errors opening
			return -1;
		}

		current_process->num_files++;
		current_process->fd_array[fd].file_ops.read = &read_directory;
		current_process->fd_array[fd].file_ops.write = &write_directory;
		current_process->fd_array[fd].file_ops.open = &open_directory;
		current_process->fd_array[fd].file_ops.close = &close_directory;
		current_process->fd_array[fd].inode = 0;
		current_process->fd_array[fd].file_pos = 0;
		current_process->fd_array[fd].is_open = 1;

	} else if(current_dentry.file_type == 2) { //regular file
		if(open_file(filename) == -1) {	//ensure there are no errors opening
			return -1;
		}

		current_process->num_files++;
		current_process->fd_array[fd].file_ops.read = &read_file;
		current_process->fd_array[fd].file_ops.write = &write_file;
		current_process->fd_array[fd].file_ops.open = &open_file;
		current_process->fd_array[fd].file_ops.close = &close_file;
		current_process->fd_array[fd].inode = current_dentry.inode_num;
		current_process->fd_array[fd].file_pos = 0;
		current_process->fd_array[fd].is_open = 1;

	} else { // error if not an rtc, directory, or file
		return -1;
	}

	return fd;
}

/* close
 * 
 * Close an item
 * Inputs:  
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t close(int32_t fd) {

	int32_t close_data;

	pcb_t * current_process = get_this_pcb();

	if(fd < 2 || fd >= MAX_FILES) {		//don't want to close stdin or stdout
		return -1;
	}

	if(current_process->fd_array[fd].is_open == 0) { // file is not open therefore cannot be closed
		//active_processes[current_process->pcb_id] = 0;
		return -1;
	}

	current_process->fd_array[fd].is_open = 0; // mark file as closed

	close_data = current_process->fd_array[fd].file_ops.close(fd); // return output of relevant close function

	if(close_data == -1) {
		return -1;
	} else {
		return 0;
	}

}

/* get next pid
 * 
 * Gets next available pid
 * Inputs:  
 * Outputs: pid number
 * Side Effects: None
 */
int32_t get_next_pid(){
	int pid;

	for(pid = 0; pid < MAX_PROCESSES; pid++) {
		if(!active_processes[pid]) {
			active_processes[pid] = 1;
			return pid;
		}
	}

	return -1; //if no processes are available
}

/* get next shell
 * 
 * Gets next available shell for terminal
 * Inputs:  
 * Outputs: place in array for next shell
 * Side Effects: None
 */
int32_t get_next_shell(){
	int pid;
	int32_t current_tid = get_curr_tid();

	for(pid = 0; pid < MAX_PROCESSES; pid++) {
		if(active_terminals[current_tid].shells[pid] == -1) {
			//active_terminals[current_tid].shells[pid] = 1;
			return pid;
		}
	}

	return -1; //if no processes are available
}

/* get last shell
 * 
 * Gets last shell run for terminal
 * Inputs:  
 * Outputs: place of last run shell in shells array
 * Side Effects: None
 */
int32_t get_last_shell(){
	int pid;
	int32_t current_tid = get_curr_tid();

	for(pid = (MAX_PROCESSES - 1); pid >= 0; pid--) {
		if(active_terminals[current_tid].shells[pid] != -1) {
			//active_terminals[current_tid].shells[pid] = 0;
			return pid;
		}
	}

	return -1; //if no processes are available
}

/* get next tid
 * 
 * Gets next available terminal id
 * Inputs:  
 * Outputs: tid number 
 * Side Effects: None
 */
int32_t get_next_tid(){
	int tid;

	for(tid = 0; tid < 3; tid++) {
		if(!active_terminals[tid].is_active) {
			active_terminals[tid].is_active = 1;
			return tid;
		}
	}

	return -1; //if no processes are available
}

/* get_curr_tid
 * 
 * Gets current terminal id
 * Inputs:  
 * Outputs: tid number 
 * Side Effects: None
 */
int32_t get_curr_tid(){
	int32_t tid;

	for(tid = 0; tid < 3; tid++) {
		if(active_terminals[tid].is_active) {
			return tid;
		}
	}

	return -1; //if no processes are available
}


int32_t get_current_pid() {
	int32_t pid;
	for (pid = MAX_PROCESSES - 1; pid > -1; pid--)
	{
		if (active_processes[pid] == 1)
			return pid;
	}
	return -1;
}

/* get this pcb
 * 
 * gets pcb for current process
 * Inputs:  
 * Outputs: pointer to pcb
 * Side Effects: None
 */
pcb_t * get_this_pcb() {
	uint32_t ESP;

	asm ("movl %%esp, %0"
        : "=r" (ESP)
        );

	return (pcb_t*)(ESP & PCB_BITMASK); // this will return the mem address of the pcb for the current process
}

/* get this terminal
 * 
 * gets pointer to active terminal's vid mem
 * Inputs:  
 * Outputs: pointer to terminal struct's vid mem
 * Side Effects: None
 */
char * get_terminal_vidmem_addr() {
	int32_t current_tid = get_curr_tid();
	char* addr = (char *)(&active_terminals[current_tid].video_buf);
	
	return addr;
}

/* get terminal screen x
 * 
 * gets current x coordinate of terminal's cursor
 * Inputs:  
 * Outputs: terminal's cursor_x member
 * Side Effects: None
 */
int32_t get_terminal_screen_x() {

	int32_t current_tid = get_curr_tid();
	int32_t cursor_x = active_terminals[current_tid].cursor_x;

	return cursor_x;
}

/* get terminal screen y
 * 
 * gets current y coordinate of terminal's cursor
 * Inputs:  
 * Outputs: terminal's cursor_y member
 * Side Effects: None
 */
int32_t get_terminal_screen_y() {

	int32_t current_tid = get_curr_tid();
	int32_t cursor_y = active_terminals[current_tid].cursor_y;

	return cursor_y;
}

/* reset terminal screen 
 * 
 * sets current x coordinate of terminal's cursor to 0
 * Inputs:  
 * Outputs:
 * Side Effects: None
 */
void reset_terminal_x() {
	int32_t current_tid = get_curr_tid();
	active_terminals[current_tid].cursor_x = 0;

	return;
}

/* halt
 * 
 * End a process
 * Inputs:  status
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t halt (uint8_t status){
	cli();

	int8_t exiting_main_shell = 0;
	int32_t current_tid = get_curr_tid();

	pcb_t* current_process = get_this_pcb();

	if(current_process->pcb_id == active_terminals[current_tid].last_shell_pid) {// exiting a shell that is not the main shell
		// must update last_shell_pid
		int32_t current_shell = get_last_shell();
		active_terminals[current_tid].shells[current_shell] = -1;
		int32_t prev_shell = get_last_shell();
		active_terminals[current_tid].last_shell_pid = active_terminals[current_tid].shells[prev_shell];
	}

	if(current_process->pcb_id == active_terminals[current_tid].pid) {
		exiting_main_shell = 1;
	}
	//if (current_process->pcb_id == 0)
	//	current_process->parent_pid = -1;

	uint32_t halt_pid = current_process->pcb_id;
	halted_PID = halt_pid;
	
	active_processes[halt_pid] = 0;

	//uint8_t curr_tid = get_curr_tid();
	active_terminals[current_process->tid].child_processes[halt_pid] = 0;

	int x;
	for (x = 2; x < MAX_FILES; x++)
	{
		if (current_process->fd_array[x].is_open == 1)
		{
			current_process->fd_array[x].is_open = 0;
			current_process->fd_array[x].file_ops = fo_invalid;
			close(x);
		}
	}

	clear_keyboard();

	if(exiting_main_shell) {
		execute((uint8_t*)"shell");
	}

	pcb_t* parent_process = (pcb_t*) (EIGHT_MB - (current_process->parent_pid + 1)*EIGHT_KB);
	
	if (parent_process->pcb_id == 0)
		parent_process->parent_pid = -1; //Fixing the parent_PID

	int32_t parentPage = parent_process->pcb_id;
	if (parentPage == -1)
		return -1;
	paging_helper(parentPage);
	tss.esp0 = EIGHT_MB - (parent_process->pcb_id*EIGHT_KB) - 4; 
	//tss.esp0 = current_process->parent_esp;
	asm volatile (
		"movl %0, %%ebx;"
		"movl %1, %%ebp;"
		"movl %2, %%esp;"
		"xorl %%eax, %%eax;"
		"movb %%bl, %%al;"
		"leave;"
		"ret;"
		:
		: "r" ((uint32_t)status), "r" (current_process->parent_ebp), "r" (current_process->parent_esp)
		: "%eax"
		);
	// Restore parent paging
/*	paging_helper(current_pid - 1);
	pcb_t* current_pcb = get_this_pcb();
	for (fd = 0; fd < MAX_FILES; fd++){
		if(current_pcb -> fd_array[fd].is_open == present)
			close(fd);
	}
*/
	/*
				CALL PAGING HELPER AND FIGURE OUT WHICH USER
				LEVEL PROGRAM USING PCB
	paging_helper(index);
	*/
	return 0;

}


/* getargs
 * 
 * Get arguments
 * Inputs:  buf, nbytes
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
	pcb_t* current_process = get_this_pcb();
	if (current_process->arg[0] == '\0')
		return -1;
	int32_t i = 0;
	int32_t length = strlen((int8_t*)current_process->arg);
	for (i = 0; i < nbytes; i++){
		if (current_process->arg[i] == '\0')
			break;
		buf[i] = current_process->arg[i];
	}
	buf[length] = '\0';
	return 0;
}

/* vidmap
 * 
 * Map video memory
 * Inputs:  screen_start pointer
 * Outputs: 0 on success
 * Side Effects: None
 */
int32_t vidmap (uint8_t** screen_start){
	if (screen_start == NULL || (*screen_start < (uint8_t)_128MB) || *screen_start > ((uint8_t)(_128MB + FOUR_MB)))
		return -1;
	*screen_start = vidmap_helper();
	return 0;
}

/* intiialize_terminal
 * 
 * Initialize all values in terminal struct for this terminal
 * Inputs:  tid to initialize
 * Outputs: 
 * Side Effects: None
 */
void initialize_terminal(int32_t tid) {
	int32_t i;
	for(i = 0; i < MAX_PROCESSES; i++) {
		active_terminals[tid].child_processes[i] = 0;
	}

	for(i = 0; i < MAX_PROCESSES; i++) {
		active_terminals[tid].shells[i] = -1;
	}

	active_terminals[tid].pid = -1;
	active_terminals[tid].tid = tid;
	active_terminals[tid].cursor_x = 0;
	active_terminals[tid].cursor_y = 0;
	active_terminals[tid].last_shell_pid = -1;
	
	if(tid == 0) {
		active_terminals[tid].is_active = 1;
	} else {
		active_terminals[tid].is_active = 0;
	}
}

/* set_activeprocess
 * 
 * Sets child processes of the active terminal
 * Inputs:  curr_tid
 * Outputs: 
 * Side Effects: None
 */
int32_t set_activeprocess(int32_t curr_tid, int32_t index) {
		
	if (active_terminals[curr_tid].child_processes[index] == 0) {
		active_terminals[curr_tid].child_processes[index] = 1;
		return 0;
	}
	
	return -1;
}

/* set_terminalparams
 * 
 * Sets the screen_x and screen_y for the curr_tid and sets the
 * struct parameters for the next_tid
 * Inputs:  next_tid and curr_tid
 * Outputs: 
 * Side Effects: None
 */
void set_terminalparams(int32_t next_tid, int32_t curr_tid) {
	
	
	//paging_helper(next_tid);
	//put vid mem / paging switch here	
	//char* kb_buf = (char*)_42MB;
	return_buf((char*)&active_terminals[curr_tid].keyboard_buf);
	//clear_keyboard();
	
	active_terminals[curr_tid].cursor_x = output_x();
	active_terminals[curr_tid].cursor_y = output_y();	
	/*int i = 0;
	for (i = 0; i < buffer_limit; i++){
		active_terminals[curr_tid].keyboard_buf[i] = kb_buf[i];
	}
	*/
	change_x(active_terminals[next_tid].cursor_x);
	change_y(active_terminals[next_tid].cursor_y);
	clear_keyboard();
	set_buf(active_terminals[next_tid].keyboard_buf);

	if (next_tid == 0){
		active_terminals[0].is_active = 1;
		active_terminals[1].is_active = 0;
		active_terminals[2].is_active = 0;	
	}
	else if (next_tid == 1){
		active_terminals[0].is_active = 0;
		active_terminals[1].is_active = 1;
		active_terminals[2].is_active = 0;	
	}
	else if (next_tid == 2){
		active_terminals[0].is_active = 0;
		active_terminals[1].is_active = 0;
		active_terminals[2].is_active = 1;	
	}
	// memcpy params: (void* dest, const void* src, uint32_t n)
	// uint32_t* curr_addr = (uint32_t*)(_40MB + (curr_tid * 4276));
	// save whatever in video memory of the current terminal to curr_addr (4096 amount of chars)
	memcpy(&active_terminals[curr_tid].video_buf, (uint32_t*)VID_MEM, FOUR_KB);
	// replaces whatever in video memory with address (next terminal)
	memcpy((uint32_t*)VID_MEM, &active_terminals[next_tid].video_buf, FOUR_KB);
	// Updates index of the keyboard buffer properly
	set_index();
	// Updates cursor after switching terminal
	update_cursor();
}

/* set_handler
* inputs: 
* signum - the signal that is sent. 
* handler_address - the address of the signal handler
* outputs: 0 on success. -1 on fail.
* details: sets up the proper signal handler for execution.
*/
int32_t set_handler (int32_t signum, void* handler_address)
{
	if (signum < 0 || signum > 4 || handler_address == NULL)
		return -1;
	return 0;
}

/* sigreturn
* inputs: none
* outputs: returns hardware context eax.
* details: copies hardware context.
*
*/
int32_t sigreturn (void)
{

	return -1;

}
/*

	struct termainal_info {
		char vid_mem[4096];
		char keyboard[128];
	};
	
	struct termainal_info *  term0 = (struct terminal_info) _40MB;
	struct termainal_info * term1 = (struct terminal_info) _44MB;

struct teminal_info * all_terminals = (struct terminal_info) _40MB;
all_terminals[0].keyboard = fdgfg;
all_terminals[2].vidmem = ;

*/


