#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include "lib.h"


/* List of Exceptions */
// Vector number 32
extern void interrupt_timer();
// Vector number 33
extern void interrupt_keyboard();
// Vector number 40
extern void interrupt_rtc();

#endif
