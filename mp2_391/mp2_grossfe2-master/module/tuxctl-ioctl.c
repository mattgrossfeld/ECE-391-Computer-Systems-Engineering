/*
 * tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)

/************************ Protocol Implementation *************************/

//Global variables Defined for tux.
bool button_ready = 1;
/*check if button ready to be read from.*/ 
int button_value = 0xFF;/*value of the button.*/
bool tux_success = 1; /*see if last round successful*/
char initial_buf[2] = {MTCP_BIOC_ON, MTCP_LED_USR}; //For initialzing, we need MTCP_BIOC_ON and MTCP_LED_USR.
char LED_buf[6] = {MTCP_LED_SET, 0x0F, 0x00, 0x00, 0x00, 0x00}; //The buffer for LED values.
int disp_buf[16] = {0xE7,0x06,0xCB,0x8F, 0x2E,0xAD,0xED,0x86, 0xEF,0xAF,0xEE,0x6D, 0xE1,0x4F,0xE9,0xE8}; //Buffer for the display.
spinlock_t tux_spinlock = SPIN_LOCK_UNLOCKED;   

/* tuxctl_handle_packet()
 * IMPORTANT : Read the header for tuxctl_ldisc_data_callback() in
 * tuxctl-ld.c. It calls this function, so all warnings there apply
 * here as well.
 */
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet) {
    unsigned a, b, c;
    unsigned long eflags = 0; //Flags for when we use spinlock.

    a = packet[0]; /* Avoid printk() sign extending the 8-bit */
    b = packet[1]; /* values when printing them. */
    c = packet[2];

    if (a == MTCP_RESET) //If its reset, we need to restore the values.
    {
        spin_lock_irqsave(&tux_spinlock, eflags); //Grab the lock and save the flags.
        tux_success = 1; //Successfully performed action.
        button_ready = 1; //Ready to read another button.
        tuxctl_ldisc_put(tty, initial_buf, sizeof(initial_buf)); //Writes out to the tux
        spin_unlock_irqrestore(&tux_spinlock, eflags); //Unlock the spinlock and restore the flags.
     //   break;
    }
    else if (a == MTCP_ACK) //If acknowledged, we can get another button.
    {
        tux_success = 1; //Successfully performed the button press.
        button_ready = 1; //Ready to read another button press.
       // break;
    }
    else if (a == MTCP_BIOC_EVENT) //If it's button's are enabled, we can check to see what button is pressed.
    {
        spin_lock_irqsave(&tux_spinlock, eflags); //Grab the lock and save the flags.
        button_value = (b & BUTTON_ABCS) | ((c & BUTTON_D) << 3) | ((c & BUTTON_UR) << 4) | ((c & BUTTON_L) << 5); //Check what button we pressed or released.
        button_ready = 1; //Ready to read another value.
        spin_unlock_irqrestore(&tux_spinlock, eflags); //Unlock and restore.
        //break;
    /*printk("packet : %x %x %x\n", a, b, c); */
    }
}

/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with tux_success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/

/* tuxctl_ioctl
* DESCRIPTION: Input and Output control for the Tux Controller
* INPUTS: file = File we open.
*         cmd = The command we're performing
*         arg = The command's argument
* OUTPUTS: -EINVAL for failure. 0 for success.
*/
int tuxctl_ioctl(struct tty_struct* tty, struct file* file,
                 unsigned cmd, unsigned long arg) {

    unsigned long eflags = 0; //The flags.
    switch (cmd) {
        case TUX_INIT: //Initialize the tux
            tux_success = 1; //Successful.
            button_ready = 1; //Ready to read button.
            tuxctl_ldisc_put (tty, initial_buf, sizeof(initial_buf)); //Send it to Tux.
            return 0;
            
        case TUX_BUTTONS: //Handling buttons
            if (arg == 0) //Do we have an argument to use?
                return -EINVAL;

            if (button_ready == 1) //If we were going to use our button, stop.
            {
                button_ready = 0; //Button not ready for use.
                
                //copy button state to arg
                spin_lock_irqsave(&tux_spinlock, eflags); //Lock and save flags.
                copy_to_user ((int*)arg, &button_value, sizeof(button_value)); //Copy the data to user (the program).
                spin_unlock_irqrestore(&tux_spinlock, eflags); //Restore the flags.
            }
            break;

        case TUX_SET_LED: //LEDs
            if (tux_success) //Did we not do it successfully?
            {
                tux_success = 0;
                int led = 0;
                for (led = 2; led <= 5; led++) //Initially clear the LED buffer. 2-5 contains the LED values to be loaded.
                {
                    LED_buf[led] = 0x00;
                }
                

                if (LED0_ON & arg) //Buffer for LED0
                {
                    LED_buf[2] = disp_buf[arg & LED0];
                    if (LED0_DECIMAL & arg) //Are we getting the decimal?
                        LED_buf[2] |= MASK_DEC; //OR it with the mask.
                }
                
                if (LED1_ON & arg)  //Buffer for LED1
                {
                    LED_buf[3] = disp_buf[(arg & LED1) >> 4];   
                    if (LED1_DECIMAL & arg) //Are we getting the decimal?
                        LED_buf[3] |= MASK_DEC;//OR it with the mask.
                }
               
                
                if (LED2_ON & arg) //Buffer for LED2
                {
                    LED_buf[4] = disp_buf[(arg & LED2) >> 8];
                    if (LED2_DECIMAL & arg) //Are we getting the decimal?
                        LED_buf[4] |= MASK_DEC;//OR it with the mask.
                }
               
                
                if (LED3_ON & arg) //Buffer for LED3
                {
                    LED_buf[5] = disp_buf[(arg & LED3) >> 12];
                    if (LED3_DECIMAL & arg) //Are we getting the decimal?
                        LED_buf[5] |= MASK_DEC;//OR it with the mask.
                }
                tuxctl_ldisc_put (tty, LED_buf, sizeof(LED_buf)); //Writes the LED buffer.
            }
            return 0;
        case TUX_LED_REQUEST: //Still needs to return 0 for this.
            break;
        case TUX_LED_ACK: //Still needs to return 0.
            break;
        case TUX_READ_LED: //Still needs to return 0.
            break;

        default:
            return -EINVAL;
    }
    return 0;
}
