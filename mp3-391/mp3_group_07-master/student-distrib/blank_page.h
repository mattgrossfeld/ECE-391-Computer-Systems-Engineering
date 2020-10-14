#ifndef BLANK_PAGE_H
#define BLANK_PAGE_H

#include "lib.h"
#include "types.h"
#define VID_MEM 0xB8000

// Initialize Paging
extern void paging_init();
extern void paging_helper(int index);
extern uint8_t* vidmap_helper();
extern void flush_tlb();
extern void terminal_map();
#endif
