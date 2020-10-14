#ifndef SYSTEMCALL_H
#define SYSTEMCALL_H

#include "lib.h"
#include "terminal.h"
#include "rtc.h"
#include "types.h"
#include "file.h"
#include "keyboard.h"

extern int32_t execute (const uint8_t* command);
extern int32_t halt (uint8_t status);

/*Signal systemcalls*/
#define DIV_ZERO 0
#define SEGFAULT 1
#define INTERRUPT 2
#define ALARM 3
#define USER1 4

extern int32_t set_handler (int32_t signum, void* handler_address);
extern int32_t sigreturn (void);
/* end signals */


extern int32_t no_operation();

//Helpers for scheduling
extern void scheduler_helper(int32_t current_pid, int32_t* active_t, int32_t* active_p);
extern int32_t is_process_open(int32_t pid);
extern int32_t is_process_active(int32_t* active_p);

int32_t read(int32_t fd, void * buf, int32_t nbytes);
int32_t write(int32_t fd, const void * buf, int32_t nbytes); //const buffer because write buffer will not change once sent 
int32_t open(const uint8_t * filename);
int32_t close(int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);

int32_t get_next_tid();
int32_t get_next_pid();
int32_t get_next_shell();
int32_t get_last_shell();
int32_t get_current_pid();

extern char * get_terminal_vidmem_addr();
extern int32_t get_terminal_screen_x();
extern int32_t get_terminal_screen_y();

extern void reset_terminal_x();

void initialize_terminal();
extern int32_t get_curr_tid();
int32_t set_activeprocess(int32_t curr_tid, int32_t index);
extern void set_terminalparams(int32_t next_tid, int32_t curr_tid);

#define FOUR_MB 0x400000
#define EIGHT_MB 0x800000
#define _12MB 0xC00000
#define _16MB 0x1000000
#define _20MB 0x1400000
#define _24MB 0x1800000
#define _28MB 0x1C00000
#define _32MB 0x2000000
#define _36MB 0x2400000
#define _40MB 0x2800000
#define _41MB 0x2900000
#define _42MB 0x2A00000
#define _128MB 0x08000000
#define FOUR_KB 0x1000
#define EIGHT_KB 0x2000
#define BOTTOM_OF_PAGE 0x083FFFFC // this is 128 MB - 16B // points to bottom of first 4MB page
#define PCB_BITMASK 0xFFFFE000	//32 bits total, top 19 will always be the same for a kernel stack because they are 8kb-aligned

#define FILE_BUFFER_SIZE 127
#define MAX_NBYTES 32
#define MAX_FILES 8
#define MAX_PROCESSES 8 	


typedef struct fo_t {
	int32_t (*read) (int32_t fd, void * buf, int32_t nbytes);
	int32_t (*write) (int32_t fd, const void * buf, int32_t nbytes); 
	int32_t (*open) (const uint8_t * filename);
	int32_t (*close) (int32_t fd);
} fo_t; // file operations type

typedef struct fd_t {
	fo_t file_ops; //file ops struct treated as a jump table
	uint32_t inode; //where is the inode in the file array?
	uint32_t file_pos;	//where in the file array is this particular file?
	uint32_t is_open; // 0 - not in use, 1 - in use
} fd_t; // file descriptor type

typedef struct pcb_t {
	int32_t pcb_id; //which pcb is this?
	uint32_t parent_ebp; // ebp of this pcb's parent. will be used in halt call.
	uint32_t parent_esp; // esp of parent. will be used in halt call.
	int32_t parent_pid; //pcb_id of parent
	uint32_t num_files; // number of files in file array
	fd_t fd_array[MAX_FILES]; //files currently being used by this process
	uint8_t file_name[MAX_NBYTES]; //name of current file
	uint8_t arg[128];	// holds argument of file/function
	int32_t tid; // tid of the terminal which executed this process
} pcb_t; // process control block type

typedef struct terminal_t {
	int32_t child_processes[8]; //array of the pid's of each child process for this terminal (each element corresponds to element in active_processes array and will be 0 or 1)
	int32_t pid; //pid of this terminal's parent shell (this terminal's shell's place in active_processes array)
	int32_t tid; //terminal id of this shell (this terminal's place in active_terminals array)
	int32_t cursor_x; // x pos of cursor for this terminal
	int32_t cursor_y; // y pos of cursor for this terminal
	int32_t is_active; // is this the one terminal that is currently being displayed on-screen? 0 or 1
	int32_t last_shell_pid; // the pid of the last shell that was run on this terminal
	int32_t shells[8]; // array of all shells that have been run on this terminal to keep track of hierarchy
	char keyboard_buf[buffer_limit]; // the current keyboard buf of this terminal 
	char video_buf[FOUR_KB]; // copy of the video memory for this terminal
} terminal_t; // terminal type


extern pcb_t * get_this_pcb(); 


#endif
