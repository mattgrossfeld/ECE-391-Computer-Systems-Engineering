/* Keyboard Initialization */
#include "keyboard.h"
#include "i8259.h"
#include "lib.h"
#include "systemcall.h"
#include "blank_page.h"
#include "terminal.h"

char scancode[0x84];						// holds the corresponding scancodes for keyboard
char capital[0x84];							// holds the capital of scancodes for keyboard
int flag = -1;								// capital letter flag
int shift_flag = 0;							// shift key
int ctrl_flag = 0;							// ctrl key
int alt_flag = 0;							// alt key
int stop_x = 0;								// keep track of screen_x when enter is pressed
int stop_y = 0;								// keep track of screen_y when enter is pressed
	
char keyboard_buf[buffer_limit];						// text buffer
int index = 0;								// index of text buffer

/* FOR TESTING */
char terminal_buf[buffer_limit];			// terminal buffer
int keyboard_flag = 0;

/* Terminal Flags */
int terminal2_flag = 0;						// Terminal 2 has never been ran before
int terminal3_flag = 0;						// Terminal 3 has never been ran before

/* void init_capital
 * Description: initialize capital letters and shift symbols
 * Inputs:              
 * Return Value: Capital letters or shift symbols
 * Function: */
void init_capital(){
	//capital[0x01] = ' ';					// ESC
	capital[0x02] = '!';
	capital[0x03] = '@';					
	capital[0x04] = '#';				
	capital[0x05] = '$';					
	capital[0x06] = '%';
	capital[0x07] = '^';
	capital[0x08] = '&';
	capital[0x09] = '*';
	capital[0x0A] = '(';
	capital[0x0B] = ')';
	capital[0x0C] = '_';
	capital[0x0D] = '+';
	//capital[0x0E] = ' ';					// BACKSPACE
	//capital[0x0F] = '	';					// TAB
	capital[0x10] = 'Q';					
	capital[0x11] = 'W';
	capital[0x12] = 'E';
	capital[0x13] = 'R';					
	capital[0x14] = 'T';
	capital[0x15] = 'Y';
	capital[0x16] = 'U';
	capital[0x17] = 'I';
	capital[0x18] = 'O';
	capital[0x19] = 'P';
	capital[0x1A] = '{';
	capital[0x1B] = '}';
	//capital[0x1C] = ' ';					// ENTER
	//capital[0x1D] = ' ';					// CTRL
	capital[0x1E] = 'A';					
	capital[0x1F] = 'S';
	capital[0x20] = 'D';
	capital[0x21] = 'F';
	capital[0x22] = 'G';
	capital[0x23] = 'H';
	capital[0x24] = 'J';
	capital[0x25] = 'K';
	capital[0x26] = 'L';
	capital[0x27] = ':';
	capital[0x28] = '"';
	capital[0x29] = '~';
	//capital[0x2A] = ' ';					// SHIFT
	capital[0x2B] = '|';					
	capital[0x2C] = 'Z';
	capital[0x2D] = 'X';
	capital[0x2E] = 'C';
	capital[0x2F] = 'V';
	capital[0x30] = 'B';  				
	capital[0x31] = 'N';
	capital[0x32] = 'M';
	capital[0x33] = '<';					
	capital[0x34] = '>';					
	capital[0x35] = '?';				
	//capital[0x36] = ' ';					// SHIFT
	capital[0x37] = '*';
	//capital[0x38] = ' ';					// ALT					
	capital[0x39] = ' ';					// SPACE
	//capital[0x3A] = ' ';					// CAPS LOCK
	// keypad
	capital[0x47] = '7';
	capital[0x48] = '8';
	capital[0x49] = '9';
	capital[0x4A] = '-';				
	capital[0x4B] = '4';		
	capital[0x4C] = '5';
	capital[0x4D] = '6';
	capital[0x4E] = '+';
	capital[0x4F] = '1';				
	capital[0x50] = '2';	
	capital[0x51] = '3';
	capital[0x52] = '0';
	capital[0x53] = '.';		
}
/* void init_scancode
 * Description: initialize scancode values to output correct keys
 * Inputs:              
 * Return Value: 
 * Function: */
void init_scancode(){
	//scancode[0x01] = ' ';					// ESC
	scancode[0x02] = '1';
	scancode[0x03] = '2';					
	scancode[0x04] = '3';				
	scancode[0x05] = '4';					
	scancode[0x06] = '5';
	scancode[0x07] = '6';
	scancode[0x08] = '7';
	scancode[0x09] = '8';
	scancode[0x0A] = '9';
	scancode[0x0B] = '0';
	scancode[0x0C] = '-';
	scancode[0x0D] = '=';
	//scancode[0x0E] = ' ';					// BACKSPACE
	//scancode[0x0F] = '	';					// TAB
	scancode[0x10] = 'q';					
	scancode[0x11] = 'w';
	scancode[0x12] = 'e';
	scancode[0x13] = 'r';					
	scancode[0x14] = 't';
	scancode[0x15] = 'y';
	scancode[0x16] = 'u';
	scancode[0x17] = 'i';
	scancode[0x18] = 'o';
	scancode[0x19] = 'p';
	scancode[0x1A] = '[';
	scancode[0x1B] = ']';
	//scancode[0x1C] = ' ';					// ENTER
	//scancode[0x1D] = ' ';					// CTRL
	scancode[0x1E] = 'a';					
	scancode[0x1F] = 's';
	scancode[0x20] = 'd';
	scancode[0x21] = 'f';
	scancode[0x22] = 'g';
	scancode[0x23] = 'h';
	scancode[0x24] = 'j';
	scancode[0x25] = 'k';
	scancode[0x26] = 'l';
	scancode[0x27] = ';';
	scancode[0x28] = '\'';
	scancode[0x29] = '`';
	//scancode[0x2A] = ' ';					// SHIFT
	scancode[0x2B] = '\\';					
	scancode[0x2C] = 'z';
	scancode[0x2D] = 'x';
	scancode[0x2E] = 'c';
	scancode[0x2F] = 'v';
	scancode[0x30] = 'b';  				
	scancode[0x31] = 'n';
	scancode[0x32] = 'm';
	scancode[0x33] = ',';					
	scancode[0x34] = '.';					
	scancode[0x35] = '/';				
	//scancode[0x36] = ' ';					// SHIFT
	scancode[0x37] = '*';
	//scancode[0x38] = ' ';					// ALT					
	scancode[0x39] = ' ';					// SPACE
	//scancode[0x3A] = ' ';					// CAPS LOCK

	scancode[0x47] = '7';
	scancode[0x48] = '8';
	scancode[0x49] = '9';
	scancode[0x4A] = '-';				
	scancode[0x4B] = '4';		
	scancode[0x4C] = '5';
	scancode[0x4D] = '6';
	scancode[0x4E] = '+';
	scancode[0x4F] = '1';				
	scancode[0x50] = '2';	
	scancode[0x51] = '3';
	scancode[0x52] = '0';
	scancode[0x53] = '.';				
}

/* char echochar
 * Description: Helper function to convert scancode to ASCII
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
void echochar() {
	/*if (index == buffer_limit){
		keyboard_flag = 1;
	}*/
	// Get input from keyboard
	int keyboard = inb(0x60);
	// Initialize scancode
	init_scancode();
	// Initialize capital letters and shift symbols
	init_capital();
	int i;

	// Setting and Resetting Control Flag
	if (keyboard == 0x1D){
		ctrl_flag = 1;
		send_eoi(1);
		return;
	}
	else if (keyboard == 0x9D){
		ctrl_flag = 0;
		send_eoi(1);
		return;
	}
	// Setting and Resetting ALT Flag
	else if (keyboard == 0x38){
		alt_flag = 1;
		send_eoi(1);
		return;
	}
	else if (keyboard == 0xB8){
		alt_flag = 0;
		send_eoi(1);
		return;
	}
	// When enter is pressed, offset stop_y by 1 and reset stop_x
	if (keyboard == 0x1C){
		//check_y();
		set_index();
		keyboard_flag = 1;
		if (index < buffer_limit){
			keyboard_buf[index] = capital[keyboard];
			if (output_y() == (NUM_ROWS - 1))
				vertical_scrolling_down();
			else {
				input_y(1);
				reset_x();
			}
			// Record where enter is pressed
			stop_x = output_x();
			stop_y = output_y();
			index++;
			send_eoi(1);
			return;
		}
	}
	
	// When backspace is pressed, remove character
	if (keyboard == 0x0E && (index > 0)){
		int store = backspace_helper(stop_x, stop_y);
		if (store != 1){
			index--;
			keyboard_buf[index] = '\0';
		}
		send_eoi(1);
		return;
	}
	
	// When shift is pressed, reverse capital letter flag
	// and set shift flag
	if (keyboard== 0x36 || keyboard == 0x2A){
		shift_flag = 1;
		send_eoi(1); 
		return;
	}
	// When shift is released, reverse capital letter flag
	// and reset shift flag
	if (keyboard == 0xAA || keyboard == 0xB6){
		shift_flag = 0;
		send_eoi(1); 
		return;
	}
	
	// Check if CAPS LOCK is pressed
	if (keyboard == 0x3A){
		flag = (-flag);
		send_eoi(1); 
		return;
	}
	
	// Clear screen when CTRL-L is pressed
	if (keyboard == 0x26 && ctrl_flag == 1){
		clear_screen();
		index = 0;
		send_eoi(1);
		return;
	}
	
	// CTRL+C to halt program
	if (keyboard == 0x2E && ctrl_flag == 1){
		ctrlc_y();
		halt(0);
	}
	
	/*
	*
	*
	*
	*
	*
	*
	*
	*				ALT F1 / ALT F2 / ALT F3
	*				FUNCTIONALITY
	*
	*
	*
	*
	*
	*
	*
	*
	*
	*
	*/
// ALT F1	
	if (keyboard == 0x3B && alt_flag == 1){
		send_eoi(1);
		int32_t curr_tid = get_curr_tid();
		set_terminalparams(0, curr_tid);
		
		// TAKE SEND_EOI AND RETURN OUT
		// HALT clears the flag and send_eoi
		
		return;
	}
// ALT F2	
	if (keyboard == 0x3C && alt_flag == 1){
		send_eoi(1);
		int32_t curr_tid = get_curr_tid();
		set_terminalparams(1, curr_tid);
		if (terminal2_flag == 0){
			terminal2_flag = 1;
			execute((uint8_t*)"shell");
		}
		// TAKE SEND_EOI AND RETURN OUT
		// HALT clears the flag and send_eoi
		//printf("Process 2");
		return;
	}
// ALT F3	
	if (keyboard == 0x3D && alt_flag == 1){
		send_eoi(1);
		int32_t curr_tid = get_curr_tid();
		set_terminalparams(2, curr_tid);
		if (terminal3_flag == 0){
			terminal3_flag = 1;
			execute((uint8_t*)"shell");
		}
		// TAKE SEND_EOI AND RETURN OUT
		// HALT clears the flag and send_eoi
		//printf("Process 3");
		
		return;
	}
	
	// Only detect key presses (Key releases are larger than 53)
	if (keyboard < 0x53){
		// ONLY CAPS LOCK
		if (flag == 1 && shift_flag == 0){
			// Print capital letters
			if ( (keyboard >= 0x10 && keyboard <= 0x19) || (keyboard >= 0x1E && keyboard <= 0x26) || (keyboard >= 0x2C && keyboard <= 0x32) ){
				if (index < buffer_limit){
					if ((keyboard > 0x01 && keyboard < 0x36 && keyboard != 0x0E && keyboard != 0x1C && keyboard != 0x1D && keyboard != 0x2A) || keyboard == 0x39){
						// account for TAB
						if (keyboard == 0x0F){
								i = 0;
								while (i < 4){
									keyboard_buf[index] = capital[0x39];
									index++;
									printf("%c", capital[0x39]);
									i++;
								}
						}
						else{
							keyboard_buf[index] = capital[keyboard];
							index++;
							printf("%c", capital[keyboard]);
						}
					}
				}
			}
				
			else{
				if (index < buffer_limit){
					if ((keyboard > 0x01 && keyboard < 0x36 && keyboard != 0x0E && keyboard != 0x1C && keyboard != 0x1D && keyboard != 0x2A) || keyboard == 0x39){
						// account for TAB
						if (keyboard == 0x0F){
								i = 0;
								while (i < 4){
									keyboard_buf[index] = scancode[0x39];
									index++;
									printf("%c", scancode[0x39]);
									i++;
								}
						}
						else{
							keyboard_buf[index] = scancode[keyboard];
							index++;
							printf("%c", scancode[keyboard]);
						}
					}
				}
			}			
		}
		// SHIFT AND CAPS LOCK
		else if (flag == 1 && shift_flag == 1){
			// Print small letters
			if ( (keyboard >= 0x10 && keyboard <= 0x19) || (keyboard >= 0x1E && keyboard <= 0x26) || (keyboard >= 0x2C && keyboard <= 0x32) ){
				if (index < buffer_limit){
					if ((keyboard > 0x01 && keyboard < 0x36 && keyboard != 0x0E && keyboard != 0x1C && keyboard != 0x1D && keyboard != 0x2A) || keyboard == 0x39){
						// account for TAB
						if (keyboard == 0x0F){
								i = 0;
								while (i < 4){
									keyboard_buf[index] = scancode[0x39];
									index++;
									printf("%c", scancode[0x39]);
									i++;
								}
						}
						else{
							keyboard_buf[index] = scancode[keyboard];
							index++;
							printf("%c", scancode[keyboard]);
						}
					}
				}
			}
				
			else {
				if (index < buffer_limit){
					if ((keyboard > 0x01 && keyboard < 0x36 && keyboard != 0x0E && keyboard != 0x1C && keyboard != 0x1D && keyboard != 0x2A) || keyboard == 0x39){
						// account for TAB
						if (keyboard == 0x0F){
								i = 0;
								while (i < 4){
									keyboard_buf[index] = capital[0x39];
									index++;
									printf("%c", capital[0x39]);
									i++;
								}
						}
						else{
							keyboard_buf[index] = capital[keyboard];
							index++;
							printf("%c", capital[keyboard]);
						}
					}
				}
			}			
				
		}
		// ONLY SHIFT
		else if (flag == -1 && shift_flag == 1){
			if (index < buffer_limit){
				if ((keyboard > 0x01 && keyboard < 0x36 && keyboard != 0x0E && keyboard != 0x1C && keyboard != 0x1D && keyboard != 0x2A) || keyboard == 0x39){
					// account for TAB
					if (keyboard == 0x0F){
						i = 0;
						while (i < 4){
							keyboard_buf[index] = capital[0x39];
							index++;
							printf("%c", capital[0x39]);
							i++;
						}
					}
					else{
						keyboard_buf[index] = capital[keyboard];
						printf("%c", capital[keyboard]);
						index++;
					}
				}
			}
		}
		// NONE
		else if (flag == -1 && shift_flag == 0){
			if (index < buffer_limit){
				if ((keyboard > 0x01 && keyboard < 0x36 && keyboard != 0x0E && keyboard != 0x1C && keyboard != 0x1D && keyboard != 0x2A) || keyboard == 0x39){
					// account for TAB
					if (keyboard == 0x0F){
						i = 0;
						while (i < 4){
							keyboard_buf[index] = scancode[0x39];
							index++;
							printf("%c", scancode[0x39]);
							i++;
						}
					}
					else{
						keyboard_buf[index] = scancode[keyboard];
						index++;
						printf("%c", scancode[keyboard]);
					}
				}
			}
		}
	}
	//update_cursor();
	send_eoi(1); 
	return;
}

/* void decrement_screen
 * Inputs:
 * Return Value: 
 * Function: Updates stop_x and stop_y correctly */
void decrement_screen(){
	if (stop_x != 0)
		stop_x--;
	stop_y--;
}

/* void return_buf
 * Inputs: keyboard_buf pointer
 * Return Value: 
 * Function: returns keyboard buffer */
void return_buf(char* buf){
	int i;
	for (i = 0; i < buffer_limit; i++){
		buf[i] = keyboard_buf[i];
		if (buf[i] == '\0')
			return;
	}
	return;
}

/* void return_index
 * Inputs: 
 * Return Value: Index of keyboard buffer
 * Function: returns keyboard buffer index*/
int return_index(){
	return index;
}

/* void return_keyboardflag
 * Inputs: 
 * Return Value: Keyboard flag
 * Function: Updates keyboard flag in terminal.c so that 
 *			 terminal read is called when enter is pressed */
void return_keyboardflag(int* b){
	*b = keyboard_flag;
	return;
}

/* void clear_keyboard
 * Description: Clear keyboard buffer, send eoi(for CTRL C)
 *				and reset keyboard_flag
 * Inputs:              
 * Return Value: 
 * Function: Clear keyboard buffer*/
void clear_keyboard(){
	int i;
	keyboard_flag = 0;
	index = 0;
	for (i = 0; i < buffer_limit; i++)
		keyboard_buf[i] = '\0';
	send_eoi(1);
	return;
}

/* void set_buf
 * Description: set keyboard buffer
 * Inputs:              
 * Return Value: 
 * Function: 
 */
void set_buf(char * buf){
	int i;
	for (i = 0; i < buffer_limit; i++)
		keyboard_buf[i] = buf[i];
	
	return;
}

/* void set_index
 * Description: set keyboard index
 * Inputs:              
 * Return Value: 
 * Function: 
 */
void set_index(){
	int i = 0;
	index = 0;
	for (i = 0; i < buffer_limit; i++){
		if (keyboard_buf[i] != '\0'){
			index++;
		}
		else return;
	}
}
