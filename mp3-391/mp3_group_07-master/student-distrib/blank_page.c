/* Blank Page Directory */
/* SOURCE CODE IS FROM OSDEV */
#include "blank_page.h"
#include "pagedirectory.h"
#include "enablepaging.h"
#include "types.h"
#include "systemcall.h"
#include "flush_tlb.h"

uint32_t page_directory[1024] __attribute__((aligned(4096)));
uint32_t first_page_table[1024] __attribute__((aligned(4096)));
uint32_t vidmem_table[1024] __attribute__((aligned(4096)));
uint32_t terminal_table[1024] __attribute__((aligned(4096)));

int i;

 /* void paging_init
 * Description: Helper function to initialize paging
 * Inputs:              
 * Return Value: 
 * Function: Mark everything as not present, set the first page and set vid memory*/
void paging_init(){
	//MARK EVERYTHING AS NOT PRESENT
	for(i = 0; i < 1024; i++)
	{
		// 		This sets the following flags to the pages:
		//   	Supervisor: Only kernel-mode can access them
		//   	Write Enabled: It can be both read from and written to
		//  	Not Present: The page table is not present
		page_directory[i] = 0x00000002;
	}

	// 	holds the physical address where we want to start mapping these pages to.
	// 	in this case, we want to map these pages to the very beginning of memory. 
	//	we will fill all 1024 entries in the table, mapping 4 megabytes
	for(i = 0; i < 1024; i++)
	{
		// As the address is page aligned, it will always leave 12 bits zeroed.
		// Those bits are used by the attributes ;)
		// Setting Video Memory to present (Video Memory is at 0xB8000)
		if (i == 184)
			first_page_table[i] = (i * 0x1000) | 3;
		else first_page_table[i] = (i * 0x1000) | 2; // attributes: supervisor level, read/write, present.
	}

	// attributes: user level, read/write, present	
	// Mark 0MB to 4MB as user, write, present
	page_directory[0] = ((unsigned int)first_page_table) | 7;
	// Points to 4MB
	// 0x83 = 1000 0011 (4MB per page, Read/Write & Present)
	page_directory[1] = ((unsigned int)0x400000) | 0x83;
	loadPageDirectory(page_directory);
	enablePaging();
}

 /* void paging_helper
 * Description: Helper function to initialize user level program's paging
 * Inputs:              
 * Return Value: 
 * Function: Enable index's paging*/
void paging_helper(int index){
	// First user level program
	if (index == 0){
		// First user level program is mapped to 8MB in physical address
		// 128MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)EIGHT_MB) | 0x87;
	}
	// Second user level program
	else if (index == 1){
		// Second user level program is mapped to 12MB in physical address
		// 128 MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)_12MB) | 0x87;
	}
	else if (index == 2){
		// Third user level program is mapped to 12MB in physical address
		// 128 MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)_16MB) | 0x87;
	}
	else if (index == 3){
		// Fourth user level program is mapped to 12MB in physical address
		// 128 MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)_20MB) | 0x87;
	}
	else if (index == 4){
		// Fifth user level program is mapped to 12MB in physical address
		// 128 MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)_24MB) | 0x87;
	}
	else if (index == 5){
		// Sixth user level program is mapped to 12MB in physical address
		// 128 MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)_28MB) | 0x87;
	}
	else if (index == 6){
		// Seventh user level program is mapped to 12MB in physical address
		// 128 MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)_32MB) | 0x87;
	}
	else if (index == 7){
		// Eighth user level program is mapped to 12MB in physical address
		// 128 MB is index 32 (4MB per directory index)
		// 0x87 = 1000 0111 (4MB per page, USER bit, Read/Write & Present)
		page_directory[32] = ((unsigned int)_36MB) | 0x87;
	}
	//flush_tlb();
	//loadPageDirectory(page_directory);
	//enablePaging();
	flush_tlb();
}

uint8_t* vidmap_helper(){
	// Set video memory to be user, read/write & present
	// Map table to 136 MB & 1 page directory is 4 MB
	int pagedir_index = 136 / 4;
	vidmem_table[0] = VID_MEM | 7;
	page_directory[pagedir_index] = (unsigned int)&vidmem_table[0] | 0x7;	
	return (uint8_t *)(pagedir_index * 1024 * 0x1000);
}

void terminal_map(){
	// Set terminal memory to be user, read/write & present
	// Map table to 40 MB & 1 page directory is 4 MB
	page_directory[10] = ((unsigned int)_40MB) | 0x87;
	flush_tlb(); 
}

