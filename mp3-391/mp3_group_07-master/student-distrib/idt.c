/* Handle IDT */
#include "idt.h"

// Include exceptions header file
#include "exceptions.h"
#include "x86_desc.h"
#include "interrupts.h"
#include "keyboard.h"
#include "keyboard_wrapper.h"
#include "rtc_wrapper.h"
#include "systemcall_wrapper.h"
#include "pit_wrapper.h"

 /* void idt_init
 * Description: Initializes IDT
 * Inputs:              
 * Return Value: 
 * Function: Sets parameters in idt*/
void idt_init(){
	SET_IDT_ENTRY(idt[0], exception_divbyzero); 
	SET_IDT_ENTRY(idt[1], exception_debug); 
	SET_IDT_ENTRY(idt[2], exception_nmi); 
	SET_IDT_ENTRY(idt[3], exception_breakpoint); 
	SET_IDT_ENTRY(idt[4], exception_overflow); 
	SET_IDT_ENTRY(idt[5], exception_range); 
	SET_IDT_ENTRY(idt[6], exception_invalidopcode); 
	SET_IDT_ENTRY(idt[7], exception_device); 
	SET_IDT_ENTRY(idt[8], exception_doublefault); 
	SET_IDT_ENTRY(idt[9], exception_coprocessor); 
	SET_IDT_ENTRY(idt[10], exception_invalidtss); 
	SET_IDT_ENTRY(idt[11], exception_segmentnotpresent); 
	SET_IDT_ENTRY(idt[12], exception_stacksegment); 
	SET_IDT_ENTRY(idt[13], exception_generalprotection); 
	SET_IDT_ENTRY(idt[14], exception_pagefault); 
	SET_IDT_ENTRY(idt[15], exception_reserved);
	SET_IDT_ENTRY(idt[16], exception_floatingpoint); 
	SET_IDT_ENTRY(idt[17], exception_alignmentcheck); 
	SET_IDT_ENTRY(idt[18], exception_machinecheck); 
	SET_IDT_ENTRY(idt[19], exception_simdfloating);
	SET_IDT_ENTRY(idt[20], exception_virtualization); 
	SET_IDT_ENTRY(idt[21], exception_reserved); 
	SET_IDT_ENTRY(idt[22], exception_reserved);  
	SET_IDT_ENTRY(idt[23], exception_reserved); 
	SET_IDT_ENTRY(idt[24], exception_reserved); 
	SET_IDT_ENTRY(idt[25], exception_reserved); 
	SET_IDT_ENTRY(idt[26], exception_reserved); 
	SET_IDT_ENTRY(idt[27], exception_reserved); 
	SET_IDT_ENTRY(idt[28], exception_reserved); 
	SET_IDT_ENTRY(idt[29], exception_reserved);
	SET_IDT_ENTRY(idt[30], exception_security); 
	SET_IDT_ENTRY(idt[31], exception_reserved);
	SET_IDT_ENTRY(idt[32], pit_wrapper);  
	SET_IDT_ENTRY(idt[33], keyboard_wrapper);
	SET_IDT_ENTRY(idt[40], rtc_wrapper);  
	SET_IDT_ENTRY(idt[0x80], systemcall_wrapper);
	fill_idt();
}	

 /* void fill_idt
 * Description: Helper function to initialize IDT
 * Inputs:              
 * Return Value: 
 * Function: Sets parameters in idt*/

// index is for system calls
void fill_idt(){
	/*int i;
	// P is segment present flag(always set to 1) bit 15
	// D is data size (always set to 1 because 32 bits)
	for (i = 0; i < exceptions_range; i++){
		idt[i].seg_selector = KERNEL_CS;
		idt[i].reserved4 = 0;
        idt[i].reserved3 = 0;
        idt[i].reserved2 = 1;
        idt[i].reserved1 = 1;
        idt[i].size = 1;  
        idt[i].reserved0 = 0;
		if (index == 0x80)
			idt[i].dpl = 3;
		else idt[i].dpl = 0;
        idt[i].present = 1;
	}*/

	int i;

	for (i = 0; i < idt_range; i++){
		if (i < 34 || i == 40 || i == 0x80){
			idt[i].seg_selector = KERNEL_CS;
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 0;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			idt[i].size = 1;  
			idt[i].reserved0 = 0;
			idt[i].dpl = 0;
			idt[i].present = 1;
			//if (i == 32) 
			//	idt[i].present = 0;
		}
		else {
			idt[i].seg_selector = KERNEL_CS;
			idt[i].reserved4 = 0;
			idt[i].reserved3 = 0;
			idt[i].reserved2 = 1;
			idt[i].reserved1 = 1;
			idt[i].size = 1;  
			idt[i].reserved0 = 0;
			idt[i].dpl = 0;
			idt[i].present = 0;

		}
	}
	idt[0x80].dpl = 3;

}

