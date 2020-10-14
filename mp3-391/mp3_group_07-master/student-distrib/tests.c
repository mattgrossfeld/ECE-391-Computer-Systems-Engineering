#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "terminal.h"
#include "keyboard.h"
//#include <stdio.h>
#include "rtc.h"

#define PASS 1
#define FAIL 0

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}

/* Checkpoint 1 tests */

/* IDT Test - Example
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;
	int i;
	int result = PASS;
	for (i = 0; i < 10; ++i){
		if ((idt[i].offset_15_00 == NULL) && 
			(idt[i].offset_31_16 == NULL)){
			assertion_failure();
			result = FAIL;
		}
	}

	return result;
}


/* RTC Test
 * 
 * Tests the rtc interrupt handler
 * Inputs: None
 * Outputs: Page changes
 * Side Effects: 
 * Coverage: 
 * Files: rtc.c, rtc.h
 */
void rtc_test() {

	TEST_HEADER;
	rtc_irq_handler();

}

/* Paging Test
 * 
 * Tests the paging exception handler
 * Inputs: None
 * Outputs: Page fault when accessing invalid memory
 * Side Effects: None
 * Coverage: 
 * Files: blank_page.c
 */
int paging_test() {

	TEST_HEADER;
	int* ptr = NULL;
	//int* ptr = (int *)0x1000;
	//int* ptr = (int *) 0x7fffff;
	*ptr = 1;
	return 0;

}

/* Exceptions Test
 * 
 * Tests the exceptions
 * Inputs: None
 * Outputs: Outputs error when dividing by 0
 * Side Effects: None
 * Coverage: 
 * Files: idt.c exceptions.c
 */
 
/*int idt_test1(){
	TEST_HEADER;
	int i = 1;
	return i/0;
}*/

// add more tests here

/* Checkpoint 2 tests */
/* TERMINAL TEST
 * 
 * Tests the terminal_read and terminal_write
 * Inputs: None
 * Outputs: 
 * Side Effects: None
 * Coverage: 
 * Files: terminal.c terminal.h
 */
int terminal_test(){
	int overflowTest = 50;
	char buf[overflowTest];
	terminal_read(0, buf, overflowTest - 1);
	terminal_write(0, buf, overflowTest - 1);
	return 0;
}

/* ls Test
 * 
 * Tests the file system by printing out the names of all files
 * Inputs: boot_blk -- pointer to boot_block struct 
 * Outputs: names of all files
 * Side Effects: None
 * Coverage: 
 * Files: file.c
 */
void file_test_ls() {
	int i;
	dentry_t dentry;

    for (i = 0; i < boot_blk->num_dir_entries; i++) {
        read_dentry_by_index(i, &dentry);
        inode_t* inode_blk = (inode_t *) ((unsigned int)boot_blk + BLOCK_SIZE + dentry.inode_num*BLOCK_SIZE);
        if (strlen(dentry.file_name) > 32) {
        	dentry.file_name[32] = '\0';
        }
        printf("file_name: %s, file_type: %d, file_size: %d\n", dentry.file_name, dentry.file_type, inode_blk->length);
    }

	return;
}

/* cat Test
 * 
 * Tests the file system by printing out a file
 * Inputs: boot_blk -- pointer to boot_block struct 
 * Outputs: file contents
 * Side Effects: None
 * Coverage: 
 * Files: file.c
 */
void file_test_cat(uint8_t* file_name) {
	dentry_t dentry;
	uint8_t buf[9001];

	read_dentry_by_name(file_name, &dentry);
    read_data(dentry.inode_num, 0, buf, 9001);

    int i = 0;
    int length = strlen((int8_t*) buf);
    for (i = 0; i < length; i++) {
    	printf("%c", buf[i]);
    }
    printf("\n%s\n", file_name);

	return;
}


/* cp2 RTC Test
 * 
 * Tests the RTC driver by changing the frequency based on user input
 * Inputs: none
 * Outputs: prints video memory at specified frequency
 * Side Effects: None
 * Coverage: 
 * Files: rtc.c
 */
void rtc_test_cp2() {

	TEST_HEADER;
	int keyboard;


/* Testing frequency change*/
	printf("Testing frequency change.\n");
	printf("RTC Test:\nPress 1 for 2 Hz\nPress 2 for 4 Hz\nPress 3 for 8 Hz\nPress 4 for 16 Hz\n");

	while(keyboard != 0x02 && keyboard != 0x03 && keyboard != 0x04 && keyboard != 0x05) {
		keyboard = inb(0x60);
	}
	printf("\n");
	if(keyboard == 0x02) {
		rtc_set_freq(2);
		//test_interrupts();
	} else if(keyboard == 0x03) {
		rtc_set_freq(4);
		//test_interrupts();
	} else if(keyboard == 0x04) {
		rtc_set_freq(8);
		//test_interrupts();
	} else if(keyboard == 0x05) {
		rtc_set_freq(16);
		//test_interrupts();
	}

/* Testing read*/
	printf("Testing rtc read. Using 32 as frequency\n");
	int32_t testFreq = 32;
	rtc_write(3, &testFreq, 4);

	int32_t ret = rtc_read(3, &testFreq, 4);
	if (ret == 0)
		printf("READ SUCCESSFUL! :)\n");
	else
		printf("READ FAILED! :(\n");

/* Testing write*/
	printf("Testing rtc write:\n");
	testFreq = 16;
	ret = rtc_write(3, &testFreq, 4);
	printf("Writing with fd = 3, freq = 16, nbytes = 4\n");
	if (ret != 4)
		printf("First write test failed!\n");
	else
		printf("First write test successful. Write successful.\n");

	printf("Writing with fd = 3, freq = 15, nbytes = 4\n");
	testFreq = 15;
	ret = rtc_write(3, &testFreq, 4);	
	if (ret != -1)
		printf("Second write test failed!\n");
	else
		printf("Second write test successsful! Write failed.\n");

	printf("Writing with fd = 0, freq = 16, nbytes = 4\n");
	testFreq = 16;
	ret = rtc_write(0, &testFreq, 4);
	if (ret != -1)		
		printf("Third write test failed!\n");
	else
		printf("Third write test successful! Write failed.\n");

	printf("Writing with fd = 3, freq = 16, nbytes = 3\n");
	ret = rtc_write(3, &testFreq, 3);
	if (ret != -1)
		printf("Fourth write test failed!\n");
	else
		printf("Fourth write test successful! Write failed.\n");

/*Testing open and close*/
	while (keyboard != 0x1C)
		keyboard = inb(0x60);
	if (keyboard == 0x1C)
		clear();
	printf("Testing RTC open and close\n");
	testFreq = 128;
	printf("Setting the frequency to 128.\n");
	rtc_write(3, &testFreq, 4);
	uint8_t testOpen = (uint8_t)testFreq;
	ret = rtc_open(&testOpen);
	if (ret != 0)
		printf("Open failed!\n");
	else
		printf("Open successful. Frequency reverted to 2.\n");
	printf("Closing...\n");
	ret = rtc_close(3);
	if (ret != 0)
		printf("Close failed!\n");
	else
		printf("Close successful!. \n");
}




/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){

/*	int test_to_run;

	printf("<< Begin Testing >>\n");
	
while(test_to_run != 9) {
	printf("Select test to run:\n1. IDT Test\n2. RTC Test\n9. DONE TESTING\n");
	scanf("%d", &test_to_run);

	if(test_to_run == 1) {*/

		//TEST_OUTPUT("idt_test", idt_test());
		//TEST_OUTPUT("paging_test", paging_test());
		//TEST_OUTPUT("idt_test1", idt_test1());
		
		/* CP 2 TERMINAL DRIVER TEST */
		//terminal_test();
		
		/* File System Tests */
		//file_test_cat((uint8_t*) "verylargetextwithverylongname.tx");
		//file_test_cat((uint8_t*) "testprint");
		//file_test_cat((uint8_t*) "grep");
		//file_test_cat((uint8_t*) "counter");
		//file_test_ls();

		//rtc_test_cp2();

//	}
//}


	
	
	// launch your tests here
	return;
}
