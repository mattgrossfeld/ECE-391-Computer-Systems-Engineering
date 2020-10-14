/* List of Interrupts */
#include "interrupts.h"
#include "keyboard.h"
#include "rtc.h"
#include "scheduler.h"

/* void interrupt_timer
 * Description: Called when Programmable Interrupt Timer Interrupt happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
void interrupt_timer() {
	pit_handler();
 }
 
/* void interrupt_keyboard
 * Description: Called when Keyboard Interrupt happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
void interrupt_keyboard() {
	 echochar();							// Call echochar function to echo character to screen
 }
 
/* void interrupt_rtc
 * Description: Called when CMOS real-time clock Interrupt happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
void interrupt_rtc() {
	 rtc_irq_handler();
}
