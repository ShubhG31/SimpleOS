#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "Terminal.h"

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
	// checks the keyboard handler 
		// asm volatile("int $0x21;");

	// checks the divide by 0 exception
		// int n = 0;
		// n = 1/n;
	// Tests dereferencing video memory address
		// int *j = (int*)0xB8000;
		// i = (int)*j;
	// Tests page fault when dereferencing null pointer 
		// int *j;
		// j = NULL;
		// i = *j;
	return result;
}

// add more tests here

/* Checkpoint 2 tests */

/* Terminal Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Lets the user test terminal, with Terminal_read and Terminal_write functions that echos typed strings 
 * Files: Terminal.h/S
 */
int terminal_test(){
	// int result = PASS;
	// char buf[128] = " dfojsdfkha";
	// int terminal = terminal_read(0,buf,128);
	// printf("%s",buf);
	// if (terminal != 128){
	// 	assertion_failure();
	// 	result = FAIL;
	// }
	// return result;

	while(1){
		puts("391OS>");
		char buf[500] = {0};
		int i;
		i = terminal_read(0,(void *)buf,0);
		terminal_write(0,(void*)buf,i);
		putc('\n');
	}
}

int terminal_write_test(){
	int result = PASS;
	char buf[11] = "dfojsdfkha";
	buf[10] = 10;
	int terminal = terminal_write(0,(void*)buf,11);
	if (terminal != 11){
		assertion_failure();
		result = FAIL;
	}
	return result;
}

int terminal_read_test(){
	int result = PASS;
	char buf_test[500] = "HELLO WORLD\n";
	copy_buffer(buf_test);
	char buf[500];
	int terminal = terminal_read(0,(void*)buf,0);
	if (terminal != 11){
		assertion_failure();
		result = FAIL;
	}
	return result;
}
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// TEST_OUTPUT("idt_test", idt_test());
	//  TEST_OUTPUT("Term test", terminal_test());
	clear();
	TEST_OUTPUT("Terminal Write test", terminal_write_test());
	TEST_OUTPUT("Terminal Read test", terminal_read_test());
	// launch your tests here
	terminal_test();
}
