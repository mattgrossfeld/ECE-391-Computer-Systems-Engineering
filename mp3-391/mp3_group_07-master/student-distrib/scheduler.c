#include "scheduler.h"
#include "terminal.h"
#include "systemcall.h"
#include "blank_page.h"
#include "enablepaging.h"
#include "file.h"
#include "rtc.h"
#include "keyboard.h"

#define PIT_COMMAND_REG 0x36
#define PIT_COMMAND_PORT 0x43
#define PIT_DATA_PORT 0x40
#define REFRESH_RATE 33 //Might need to change. Originally, made it 75Hz.
#define DIVIDEND 1193180 //Given from OSDev

volatile int32_t current_pid;
//volatile int32_t process_is_active = 0; //The process is in active terminal.
 int32_t process_is_active = 0;
 int32_t active_terminal = 0;

/* init_pit:
* INPUT: None.
* OUTPUT: Returns 0 on success.
* DESCRIPTION: Initializes the PIT.
*/
void init_pit(void)
{
	cli();
	current_pid = 0;
	int divisor = DIVIDEND / REFRESH_RATE;       /* Calculate our divisor */
    outb(PIT_COMMAND_REG, PIT_COMMAND_PORT);             /* Set our command byte 0x36 */
    outb(divisor & 0xFF, PIT_DATA_PORT);   /* Set low byte of divisor */
    outb(divisor >> 8, PIT_DATA_PORT);     /* Set high byte of divisor */
	enable_irq(0);
	sti();
}

/* pit_handler:
* INPUT: None.
* OUTPUT: Returns 0 on success.
* DESCRIPTION: Sets up the scheduler for the PIT.
*/
void pit_handler(void)
{
	cli();
	send_eoi(0); //EOI to IRQ0.
			scheduler_helper(current_pid, &active_terminal, &process_is_active);
			if (is_process_open(current_pid) == 0 || is_process_active(&process_is_active) == 1) //Skip if the process doesn't exist or if the process is active.
			{	
				if (current_pid < 8)
					current_pid++;
				else
					current_pid = 0;
				sti();
				return;
			}
			pcb_t* process = (pcb_t*)(EIGHT_MB - (EIGHT_KB*(1+current_pid)));
			//pcb_t* process2 = get_this_pcb();

			//Save ESP and EBP, hold old pid of the previosu process
				

			/*check which terminal is active*/
	/*		int x;
			for(x = 0; x < 3; x++) //For each terminal.
			{
				if(active_terminals[x].pid == current_pid)
				{
					active_terminal = x;
					break;
				}
			}
	*/			
			int32_t parentPage = process->pcb_id;
			if (parentPage == -1)
			{
				if (current_pid < 8)
					current_pid++;
				else
					current_pid = 0;
				sti();
				return;
			};
			paging_helper(parentPage);

				asm volatile(
				"movl %%esp, %0;"
				"movl %%ebp, %1;"
				: "=r" (process->parent_esp), "=r" (process->parent_ebp)
				);	
				
			tss.esp0 = EIGHT_MB - (process->pcb_id*EIGHT_KB) - 4; 
			//tss.esp0 = current_process->parent_esp;
			
			asm volatile (
				"movl %0, %%ebp;"
				"movl %1, %%esp;"
				:
				: "r" (process->parent_ebp), "r" (process->parent_esp)
				);

			/*re-enable paging*/
	//		paging_helper(process->pcb_id);

			/*Swap the TSS and ESP, EBP*/
	/*		tss.esp0 = EIGHT_MB - (EIGHT_KB *process->pcb_id) - 4;
			
			asm volatile(
				"movl %0, %%esp;"
				"movl %1, %%ebp;"
				: 
				: "r" (current_pcb->parent_esp), "r" (current_pcb->parent_ebp)
			);
	*/
			/*flush TLB*/
			flush_tlb();
			if (current_pid < 8)
					current_pid++;
				else
					current_pid = 0;
		//sti();
		//if ()
	sti();
	return;
} 
