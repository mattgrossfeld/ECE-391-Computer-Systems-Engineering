#include "rtc.h"
#include "lib.h"	//using cli, sti, outb, inb
#include "types.h"
#include "i8259.h"
#include "systemcall.h"

volatile int interruption = 0; //Sees if an interruption has occured/interruptions enabled.

/* void rtc_init
 * Description: Helper function to initialize RTC
 * Inputs:              
 * Return Value: 
 * Function: Sets parameters in RTC according to datasheet
 */
void rtc_init(void) {

	//disable interrupts for duration of init
	cli();

	//set initial frequency		~ code comes from OSDev wiki ~
	outb(STATUS_REG_A, RTC_COMMAND_PORT);
	unsigned char prev_SR_A = inb(RTC_DATA_PORT); //store previous value of status reg A
	outb(STATUS_REG_A, RTC_COMMAND_PORT);
	outb((0x0F | (MASK_HIGH & prev_SR_A)), RTC_DATA_PORT); //0F = 2.

	//enable irq 8 on pic 
	outb(STATUS_REG_B, RTC_COMMAND_PORT);
	unsigned char prev_SR_B = inb(RTC_DATA_PORT); //store prev val of status reg B
	outb(STATUS_REG_B, RTC_COMMAND_PORT);
	outb((0x40 | prev_SR_B), RTC_DATA_PORT);
	enable_irq(RTC_IRQ_NUM);	//calls function in i8259.c

	//re-enable interrupts now that init is done
	sti();
	
}


/* void rtc_irq_handler
 * Description: Helper function to handle interrupts generated by the RTC
 * Inputs:              
 * Return Value: 
 * Function: Takes in data when interrupt is triggered and sends eoi signal when done.
 */
void rtc_irq_handler(void) {

	cli();

	send_eoi(RTC_IRQ_NUM);

	outb(STATUS_REG_C, RTC_COMMAND_PORT);	// select register C
	inb(RTC_DATA_PORT);		//  dispose of contents. this re-enables irq for rtc

	interruption = 1;
	enable_irq(RTC_IRQ_NUM);	//calls function in i8259.c
	sti();
}

/* int32_t rtc_open
 * Description: Function to open files by the RTC driver.
 * Inputs: filename - The name of the file we're opening.       
 * Return Value: Returns a 32-bit integer.
 * If file does not exist, return -1.
 * If file opened successfully, return 0.
 * Function: Takes filename and opens it in the OS.
 */
int32_t rtc_open (const uint8_t* filename)
{
	if (filename == NULL) //Check if valid.
		return -1;

	rtc_set_freq(2);
	sti();
	enable_irq(RTC_IRQ_NUM);	//calls function in i8259.c

	return 0;
}

/* int32_t rtc_close
 * Description: Function to close files by the RTC driver.
 * Inputs: fd - The file directory of the file we're closing.       
 * Return Value: return -1 if invalid/unsucessful. Successful, return 0.
 * Function: Takes fd and closes it in the OS.
 */
int32_t rtc_close (int32_t fd)
{
	if (fd == 1 || fd == 0) //Cannot close the default descriptors (1 or 0)
        return -1;
    
    disable_irq(RTC_IRQ_NUM); //Disable the IRQ for the RTC.
    return 0;
}

/* int32_t rtc_read
 * Description: Function to read files via the RTC driver.
 * Inputs: fd - The file directory we're reading from.
 * buf - The input buffer data is read from.
 * nbytes - The amount of data being read in bytes.      
 * Return Value: Always returns 0 for RTC AFTER an interrupt.
 * Function: Reads data from file/device.
 */
int32_t rtc_read(int32_t fd, void* buf, int32_t nbytes)
{
	interruption = 0; //Reset the interruption flag.
	sti();
	enable_irq(RTC_IRQ_NUM);	//calls function in i8259.c
	while (interruption == 0) //Wait for the next interruption.
	{
	}

	return 0; //Returns 0 after the interrupt.
}

/* int32_t rtc_write
 * Description: Function to write to files via the RTC driver.
 * Inputs: fd - The file directory of the file we're writing to.
 * buf - The buffer data is written to. Frequency in this case
 * nbytes - The amount of data we're writing in bytes.       
 * Return Value: Returns -1 on failure. If successful, returns # bytes written.
 * Function: Writes data to file/device.
 */
int32_t rtc_write (int32_t fd, const void* buf, int32_t nbytes)
{
	if (nbytes != 4 || buf == NULL || fd == 1 || fd == 0) //Must write 4 bytes. Can't write to the first 2 directories.
		return -1;

	int32_t frequency = *((int32_t*) buf);
	if (rtc_set_freq(frequency) != 0)
		return -1;

	return nbytes;
}

/* int32_t rtc_set_freq
 * Description: Function to set the frequency of RTC interrupt rate.
 * Inputs: frequency - the frequency the RTC interrupt rate is set to.
 * Return Value: Returns -1 on failure. If successful, returns 0.
 * Function: Writes data to file/device.
 */
//NOTE: Values for frequency MUST be a power of 2, starting at 2 and ends at 1024.
int32_t rtc_set_freq(int32_t frequency)
{
	char freq = 0x0F;

	if (frequency == 2)
		freq = 0x0F;

    else if (frequency == 4)
        freq = 0x0E;

    else if (frequency == 8)
        freq = 0x0D;

    else if (frequency == 16)
        freq = 0x0C;
  
    else if (frequency == 32)
        freq = 0x0B;
  
    else if (frequency == 64)
        freq = 0x0A;
  
    else if (frequency == 128)
        freq = 0x09;
  
    else if (frequency == 256)
        freq = 0x08;
  
    else if (frequency == 512)
        freq = 0x07;
  
    else if (frequency == 1024)
        freq = 0x06;
  
    else
    {
        return -1;
    }

    outb(STATUS_REG_A, RTC_COMMAND_PORT);
	unsigned char prev_SR_A = inb(RTC_DATA_PORT); //store previous value of status reg A
	outb(STATUS_REG_A, RTC_COMMAND_PORT);
	outb((freq | (MASK_HIGH & prev_SR_A)), RTC_DATA_PORT);
	return 0;
}