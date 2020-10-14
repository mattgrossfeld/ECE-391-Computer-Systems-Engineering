/* List of Interrupts */
#include "exceptions.h"

/* void exception_divbyzero
 * Description: Called when divide-by-zero exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_divbyzero() {
	 printf ("Fault: Divide-by-zero Error\n");
	 while (1){
	 }
 }
 
 /* void exception_debug
 * Description: Called when debug exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_debug() {
	 printf ("Fault/Trap: Debug\n");
	 while (1){
	 }
 }
 
 /* void exception_nmi
 * Description: Called when non-maskable interrupt exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_nmi() {
	 printf ("Interrupt: Non-maskable Interrupt\n");
	 while (1){
	 }
 }
 
 /* void exception_breakpoint
 * Description: Called when breakpoint exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_breakpoint() {
	 printf ("Trap: Breakpoint\n");
	 while (1){
	 }
 }
 
 /* void exception_overflow
 * Description: Called when overflow exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_overflow() {
	 printf ("Trap: Overflow\n");
	 while (1){
	 }
 }
 
 /* void exception_range
 * Description: Called when bound range exceeded exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_range() {
	 printf ("Fault: Bound Range Exceeded\n");
	 while (1){
	 }
 }
 
 /* void exception_invalidopcode
 * Description: Called when invalid opcode exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_invalidopcode() {
	 printf ("Fault: Invalid Opcode\n");
	 while (1){
	 }
 }
 
 /* void exception_device
 * Description: Called when device not available exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_device() {
	 printf ("Fault: Device not available\n");
	 while (1){
	 }
 }
 
 /* void exception_doublefault
 * Description: Called when double fault exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_doublefault() {
	 printf ("Abort: Double Fault\n");
	 while (1){
	 }
 }
 
 /* void exception_coprocessor
 * Description: Called when coprocessor segment overrun exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_coprocessor() {
	 printf ("Fault: Coprocessor Segment Overrun\n");
	 while (1){
	 }
 }
 
 /* void exception_invalidtss
 * Description: Called when invalid TSS exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_invalidtss() {
	 printf ("Fault: Invalid TSS\n");
	 while (1){
	 }
 }
 
 /* void exception_segmentnotpresent
 * Description: Called when Segment Not Present exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_segmentnotpresent() {
	 printf ("Fault: Segment Not Present\n");
	 while (1){
	 }
 }
 
 /* void exception_stacksegment
 * Description: Called when Stack Segment Fault exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_stacksegment() {
	 printf ("Fault: Stack segment Fault\n");
	 while (1){
	 }
 }
 
 /* void exception_generalprotection
 * Description: Called when General Protection Fault exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_generalprotection() {
	 printf ("Fault: General Protection Fault\n");
	 while (1){
	 }
 }
 
 
 /* void exception_pagefault
 * Description: Called when Page Fault exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_pagefault() {
	 printf ("Fault: Page Fault\n");
	 while (1){
	 }
 }
 
 /* void exception_pagefault
 * Description: Called when Page Fault exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_reserved() {
	 printf ("Reserved Exception\n");
	 while (1){
	 }
 }
 
 /* void exception_floatingpoint
 * Description: Called when x87 Floating-Point exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_floatingpoint() {
	 printf ("Fault: x87 Floating-Point Exception");
	 while (1){
	 }
 }
 
 /* void exception_alignmentcheck
 * Description: Called when Alignment Check exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_alignmentcheck() {
	 printf ("Fault: Alignment Check\n");
	 while (1){
	 }
 }
 
 /* void exception_machinecheck
 * Description: Called when Machine Check exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_machinecheck() {
	 printf ("Abort: Machine Check\n");
	 while (1){
	 }
 }
 
 /* void exception_simdfloating
 * Description: Called when SIMD Floating-Point exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_simdfloating() {
	 printf ("Fault: SIMD Floating-Point Exception\n");
	 while (1){
	 }
 }
 
 /* void exception_virtualization
 * Description: Called when virtualization exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_virtualization() {
	 printf ("Fault: Virtualization Exception\n");
	 while (1){
	 }
 }
 
   /* void exception_security
 * Description: Called when security exception happens
 * Inputs:              
 * Return Value: 
 * Function: Prints exception type to screen */
 void exception_security() {
	 printf ("Fault: Security Exception\n");
	 while (1){
	 }
 }
