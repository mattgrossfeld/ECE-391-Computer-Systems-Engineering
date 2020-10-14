#ifndef IDT_H
#define IDT_H
#include "exceptions.h"

#define idt_range 256

// idt initialization function
extern void idt_init();

// idt initialization's helper function
extern void fill_idt();

#endif
