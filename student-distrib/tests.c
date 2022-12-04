#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "Terminal.h"
#include "file_sys.h"
#include "RTC.h"

#define PASS 1
#define FAIL 0

#define VIDEO 0xB8000

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

/* Keyboard Handler Test
 * 
 * Asserts that first 10 IDT entries are not NULL
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: load keyboard handler then asm volatile value
 * Files: idt.c/h
 */
int keyboard_handler_test(){
	// checks the keyboard handler 
		asm volatile("int $0x21;");
		return 0;
}

/* Divide by Zero Test
 * 
 * Checks the divide by zero exception and returns pass if success
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: n value then divide by zero exception
 * Files: idt.c/h
 */
int divide_0_test(){
		int n = 0;
		n = 1/n;
		return PASS;
}

/* Deferencing Video Memory Address Test
 * 
 * Tests to see if video memory address are being dereferenced
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: video memory address and pointer
 * Files: paging.c/h
 */
int dereferencing_vm_test(){
		int i;
		int *j = (int*)VIDEO;//0xB8000;
		i = (int)*j;
		return PASS;
}

/* Page Fault Test
 * 
 * Tests if a page fault occurs when dereferencing the null pointer
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: null pointer to dereference
 * Files: paging.c/h
 */
int dereferencing_null_test(){
		int *j, i;
		j = NULL;
		i = *j;
		return PASS;
}

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

	while(1){
		puts("391OS>");
		char buf[500] = {0};
		int i;
		i = terminal_read(0,(void *)buf,0);
		terminal_write(0,(void*)buf,i);
		putc('\n');
	}
}

/* Terminal Open Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: Terminal.h/S
 */
int terminal_open_test(){
	int result = PASS;
	char buf[11] = "dfojsdfkha";
	result=terminal_open(buf);
	if(result==0)return PASS;
	else return FAIL;
}

/* Terminal Close Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Files: Terminal.h/S
 */
int terminal_close_test(){
	int result = PASS;
	result=terminal_close(0); // randon number for fd
	return result;
}

/* Terminal Write Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Lets the prints the given text, and checks the return value of bytes written
 * Files: Terminal.h/S
 */
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

/* Terminal Read Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Lets the read function copy keyboard buffer to and checks the return value of bytes copied
 * Files: Terminal.h/S
 */
int terminal_read_test(){
	int result = PASS;

	// THE 500 is for large buffer
	char buf_test[500] = "HELLO WORLD\n"; // sets a large buffer to HELLO WORLD
	// copies the buffer to line buffer in terminal 
	copy_buffer(buf_test);
	// THE 500 is for large buffer
	char buf[500]; // buffer the teminal read is going to copy to 
	int terminal = terminal_read(0,(void*)buf,0); // 0 for not necessary values 
	if (terminal != 11){
		assertion_failure();
		result = FAIL;
	}
	return result;
}

/* RTC Open and Close Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Checks all possible frequencies that RTC can be set to and makes sure it successfully opens and closes
 * Files: RTC.h/S
 */

int RTC_open_test(){
	uint8_t* filename = 0;
    // printf("%d",RTC_open(filename));
	return RTC_open(filename)==0?PASS:FAIL;
}
int RTC_close_test(){
	uint8_t* filename = 0;
    // printf("%d",RTC_open(filename));
	return RTC_open(filename);
}

/* RTC Write and Read Test
 * 
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Checks all possible frequencies that RTC can be set to and makes sure RTC successfully writes and reads
 * Files: RTC.h/S
 */

int RTC_write_and_read_test(){
	int i;
	int count = 80;//used for when characters should go on a new line
	int32_t write_ret;
	for(i = 2; i < 2048; i++){ //1024 is the max feq so we can visibly see change in how fast character is printed
    	int buf_int = i;
		write_ret = RTC_write(NULL, (void*) (&buf_int), NULL);//we write a new freq each time
		RTC_read(NULL, NULL, NULL);
		count--;
		if(write_ret != -1){//if the freq is a valid for write
			count = 80; //used for when characters should go on a new line
			putc('\n');
			putc('\n');
			putc('a');
			// putc("a\n");
		}else if(count == 0){ //if we filled the entire line with characters
			count = 80; //used for when characters should go on a new line
			putc('\n');
			putc('a');
			// putc("a\n");
		}else{
			putc('a');
		}
	}
	putc('\n');
	RTC_write(NULL, NULL, NULL);//checking if we pass NULL into the write
	return PASS;
}


/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	// launch your tests here

	/* Checkpoint 1 tests*/
	// TEST_OUTPUT("idt_test", idt_test());
	// TEST_OUTPUT("keyboard_handler_test", keyboard_handler_test());
	// TEST_OUTPUT("divide_0_test", divide_0_test());
	// TEST_OUTPUT("dereferencing_vm_test", dereferencing_vm_test());
	// TEST_OUTPUT("dereferencing_null_test", dereferencing_null_test());

	/* Checkpoint 2 tests*/

	// Terminal Tests 
	// TEST_OUTPUT("Terminal Write test", terminal_write_test());
	// TEST_OUTPUT("Terminal Read test", terminal_read_test());
	// TEST_OUTPUT("Terminal Open test", terminal_open_test());
	// TEST_OUTPUT("Terminal Close test", terminal_close_test());
	// terminal_test();

	// RTC Tests 
	// TEST_OUTPUT("RTC_write_and_read_test", RTC_write_and_read_test());
	// TEST_OUTPUT("RTC_open_test", RTC_open_test());
	// TEST_OUTPUT("RTC_close_test", RTC_close_test());
		// File System Tests
			// test_file_driver_small_file();
			test_file_driver_large_file();
			// test_file_driver_executable_file();
			// test_file_driver_read_twice();
			// test_dir_driver();
			// test_show_files();
			// test_show_frame();
	
	/* Checkpoint 3 tests*/

	// Tests cases are taken care of in syserr and executing Shell, ls, pingpong. 

}
