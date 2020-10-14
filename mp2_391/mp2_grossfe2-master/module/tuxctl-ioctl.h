#ifndef TUXCTL_H
#define TUXCTL_H

#include "mtcp.h"

#define TUX_SET_LED _IOR('E', 0x10, unsigned long)
#define TUX_READ_LED _IOW('E', 0x11, unsigned long*)
#define TUX_BUTTONS _IOW('E', 0x12, unsigned long*)
#define TUX_INIT _IO('E', 0x13)
#define TUX_LED_REQUEST _IO('E', 0x14)
#define TUX_LED_ACK _IO('E', 0x15)

//TUX DEFINITIONS FOR LEDS AND BUTTONS
#define LED0 0x0000000F //First LED.
#define LED1 0x000000F0 //Second LED.
#define LED2 0x00000F00 //Third LED.
#define LED3 0x0000F000 //Fourth LED

#define LED0_DECIMAL 0x01000000 //The first LED value in decimal.
#define LED1_DECIMAL 0x02000000 //The second LED value in decimal.
#define LED2_DECIMAL 0x04000000 //The third LED value in decimal.
#define LED3_DECIMAL 0x08000000 //The fourth LED value in decimal.

#define LED0_ON 0x00010000 //Turn on the first LED.
#define LED1_ON 0x00020000 //Turn on the second LED
#define LED2_ON 0x00040000 //Turn on the third LED.
#define LED3_ON 0x00080000 //Turn on the fourth LED.

#define MASK_DEC 0x10 //Mask decimal value.
#define BUTTON_ABCS 0x0F //Buttons C, B, A, and Start.
#define BUTTON_UR 0x09 //Buttons Up and right.
#define BUTTON_L 0x02 //Buttons left.
#define BUTTON_D 0x04 //Buttons down.

#endif
