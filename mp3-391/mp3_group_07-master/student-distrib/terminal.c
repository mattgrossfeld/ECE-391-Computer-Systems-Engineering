/* Terminal Initialization */
#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "terminal.h"
#include "systemcall.h"

char* terminal_buf;
/* int32_t terminal_open
 * Inputs: filename
 * Return Value: 
 * Function: */
int32_t terminal_open(const uint8_t* filename){
	if (filename == NULL) 						//Check if valid.
		return -1;
	else return 0;
}

/* int32_t terminal_close
 * Inputs: file descriptor (fd)
 * Return Value: 
 * Function: */
int32_t terminal_close(int32_t fd){
	return 0;
}

/* int32_t terminal_read
 * Inputs: buf = terminal buffer
 *  	   fd = file descriptor
 *		   nbytes = amount of data being read in bytes
 * Return Value: Data from one line that has been terminated 
 *				 by ENTER or as much as fits in the buffer
 * Function: Copies keyboard_buf to terminal_buf */
int32_t terminal_read(int32_t fd, void* buf, int32_t nbytes){
	if (buf == NULL || nbytes == 0){
		return 0;
	}
	else{
		int32_t counter = 0;
		int32_t i;
		char* keyboard_buf = (char*)_42MB; 
		terminal_buf = buf;
		int keyboard_flag;
		asm volatile (
			"cli;"
			"sti;"
		);
		init_terminal(nbytes);
		return_keyboardflag(&keyboard_flag);
		while (keyboard_flag != 1){
			return_keyboardflag(&keyboard_flag);
		}
		return_buf(keyboard_buf);
		int index = return_index();
		if (nbytes < index){
			// Check where is ENTER pressed
			while (counter <= nbytes){
				if (keyboard_buf[counter] == '\0'){
					counter++;
					break;
				}
				counter++;
			}
			// Copy keyboard buffer into terminal buffer (till ENTER is pressed
			// or the end)
			for (i = 0; i <= counter; i++){
				terminal_buf[i] = keyboard_buf[i];
			}
			clear_keyboard();
			terminal_buf[counter-1] = '\n';
			terminal_buf[counter] = '\0';
			return nbytes;
		}
		else{
			// Check where is ENTER pressed
			while (counter <= nbytes){
				if (keyboard_buf[counter] == '\0'){
					counter++;
					break;
				}
				counter++;
			}
			// Copy keyboard buffer into terminal buffer (till ENTER is pressed
			// or the end)
			for (i = 0; i <= counter; i++){
				terminal_buf[i] = keyboard_buf[i];
			}
			clear_keyboard();
			terminal_buf[counter-1] = '\n';
			terminal_buf[counter] = '\0';
			return index;
		}
	}
	
}

/* int32_t terminal_write
 * Inputs: fd = file descriptor
 *		   buf = terminal buffer
 *		   nbytes = amount of data being read in bytes
 * Return Value: Data from one line that has been terminated 
 *				 by ENTER or as much as fits in the buffer
 * Function: Copies keyboard_buf to terminal_buf */
int32_t terminal_write (int32_t fd, const void* buf, int32_t nbytes){
	//check_y();

	if (buf == NULL || nbytes == 0){
		return 0;
	}	
	int flag1;
	return_keyboardflag(&flag1);
	/*
	while (flag1 != 1){
		return_keyboardflag(&flag1);
	}
	*/
	int32_t counter = 0;
	//int index = return_index();
	terminal_buf = (char*) buf;
	//int value = 0;
	/*if (nbytes < index){
		while (counter < nbytes){
			putc(terminal_buf[counter]);
			counter++;
		}
		//change_keyboardflag(&value);
		//update_cursor();
		return nbytes;
	}
	else {*/
	pcb_t* current_process = get_this_pcb();
	int32_t current_tid = get_curr_tid();

	if(current_process->tid == current_tid) { // process we are writing is currently being displayed on screen
		while (counter < nbytes){
			putc(terminal_buf[counter]);
			counter++;
		}
	} else {
		while(counter < nbytes) {
			putc_offsite(terminal_buf[counter]);
			counter++;
		}
	}

		//change_keyboardflag(&value);
		//update_cursor();
		return nbytes;
	//}
}

/* int32_t init_terminal
 * Inputs: terminal_buffer
 *		   nbytes = amount of data being read in bytes
 * Return Value: 
 * Function: Initialize entries of terminal buffer to be \0 */
void init_terminal(int32_t nbytes){
	int32_t i;
	for (i = 0; i < nbytes; i++){
		terminal_buf[i] = '\0';
	}
}
