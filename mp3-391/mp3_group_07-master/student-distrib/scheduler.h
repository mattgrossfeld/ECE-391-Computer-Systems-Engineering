#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "lib.h"
#include "i8259.h"
#include "types.h"
#include "x86_desc.h"

extern void init_pit(void);
extern void pit_handler(void);

#endif
