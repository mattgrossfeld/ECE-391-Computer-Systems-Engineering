/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
/* void i8259_init
 * Description: Helper function to init the Intel 8259A PIC chip
 * Inputs:         
 * Return Value: 
 * Function: Sends initialization words to PIC according to 8259A datasheet
 */
void i8259_init(void) {

	master_mask = 	0xFF;
	slave_mask = 	0xFF;

    int i;
    int IRQ_numbers = 16;

    // masks the all the IRQs
    for (i = 0; i < IRQ_numbers; i++){
        disable_irq(i);
    }

   // cli();

	/* send initialization words to PIC; ICW1 gets sent to command ports while 
		ICW2,3,4 get sent to data ports 
		~ got most of this from OSDev wiki ~ */
	//initialize master
	outb(ICW1, 			MASTER_8259_COMMAND_PORT);
	outb(ICW2_MASTER,	MASTER_8259_DATA_PORT);
	outb(ICW3_MASTER, 	MASTER_8259_DATA_PORT);
	outb(ICW4, 			MASTER_8259_DATA_PORT);
	//initialize slave
	outb(ICW1, 			SLAVE_8259_COMMAND_PORT);
	outb(ICW2_SLAVE, 	SLAVE_8259_DATA_PORT);
	outb(ICW3_SLAVE, 	SLAVE_8259_DATA_PORT);
	outb(ICW4, 			SLAVE_8259_DATA_PORT);

  //  sti();

    outb(master_mask, MASTER_8259_DATA_PORT);   
    enable_irq(2);
    outb(slave_mask, SLAVE_8259_DATA_PORT);

}

/* Enable (unmask) the specified IRQ */
/* void enable_irq
 * Description: Helper function to unmask a specified irq on PIC chip
 * Inputs:   irq_num : port number on PIC chip            
 * Return Value: 
 * Function: Sets bitmask to PIC according to 8259A datasheet
 */
void enable_irq(uint32_t irq_num) {

	uint16_t port;
    uint8_t value;

    uint8_t i;
    uint8_t mask = 0xFE; 	// start bitmask at 0xFE (inverse of 0x01)
    
 
 	if(irq_num >= 0 && irq_num < ((IRQ_SIZE << 1) - 1)) {
    	if(irq_num < IRQ_SIZE) {
    	    port = MASTER_8259_DATA_PORT;

    	    for(i = 0; i < irq_num; i++) {
    			//shift all bits to left and add one, so only 1 zero remains
    			mask = (mask << 1) + 1;	
   			}

    	    master_mask &= mask;
    	    value = master_mask;
    	} else {
    	    port = SLAVE_8259_DATA_PORT;

    	    for(i = 0; i < (irq_num - IRQ_SIZE); i++) {
    			//shift all bits to left and add one, so only 1 zero remains
    			mask = (mask << 1) + 1;	
   			}

    	    slave_mask &= mask;
    	    value = slave_mask;
    	}

    	outb(value, port);
    }

}

/* Disable (mask) the specified IRQ */
/* void disable_irq
 * Description: Helper function to mask a specified irq on PIC chip
 * Inputs:   irq_num : port number on PIC chip            
 * Return Value: 
 * Function: Sets bitmask to PIC according to 8259A datasheet
 */
void disable_irq(uint32_t irq_num) {

	uint16_t port;
    uint8_t value;

    uint8_t mask = 0x01; 	// start bitmask at 1, then left shift by irq number
    mask <<= irq_num;
 
 	if(irq_num > 0 && irq_num < ((IRQ_SIZE << 1) - 1)) {
    	if(irq_num < IRQ_SIZE) {
    	    port = MASTER_8259_DATA_PORT;
    	    master_mask |= mask;
    	    value = master_mask;
    	} else {
    	    port = SLAVE_8259_DATA_PORT;
    	    slave_mask |= mask;
    	    value = slave_mask;
    	}

    	outb(value, port);
    }

}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {

	uint32_t master_eoi_mask;

	if(irq_num >= IRQ_SIZE) {
		outb((EOI | (irq_num - IRQ_SIZE)), SLAVE_8259_COMMAND_PORT); //send eoi to slave
		master_eoi_mask = ICW3_SLAVE;
       // outb((EOI | 0x02), MASTER_8259_COMMAND_PORT);
	} else {
		master_eoi_mask = irq_num;
	}
 
	outb((EOI | master_eoi_mask), MASTER_8259_COMMAND_PORT);		//send eoi to master

}
