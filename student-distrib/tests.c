#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "Terminal.h"
#include "file_sys.h"
#include "RTC.h"

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

// keyboard handler test
int keyboard_handler_test(){
	// checks the keyboard handler 
		asm volatile("int $0x21;");
		return 0;
}

// checks the divide by 0 exception
int divide_0_test(){
	// checks the divide by 0 exception
		int n = 0;
		n = 1/n;
		return 0;
}

// Tests dereferencing video memory address
int dereferencing_vm_test(){
	// Tests dereferencing video memory address
		int i;
		int *j = (int*)0xB8000;
		i = (int)*j;
		return 0;
}

// Tests page fault when dereferencing null pointer 
int dereferencing_null_test(){
	// Tests page fault when dereferencing null pointer 
		int *j, i;
		j = NULL;
		i = *j;
		return 0;
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
// RTC test section----------------------------------------------------------
int RTC_open_test(){
	uint8_t* filename = 0;
    // printf("%d",RTC_open(filename));
	return RTC_open(filename);
}

int RTC_write_and_read_test(){
	int i;
	int count = 80;//used for when characters should go on a new line
	int32_t write_ret;
	for(i = 2; i < 2048; i++){//1024 is the max feq so we can visibly see change in how fast character is printed
    	int buf_int = i;
		write_ret = RTC_write(NULL, (void*) (&buf_int), NULL);//we write a new freq each time
		RTC_read(NULL, NULL, NULL);
		count--;
		if(write_ret != -1){//if the freq is a valid for write
			count = 80;
			putc('\n');
			putc('\n');
			putc('a');
			// putc("a\n");
		}else if(count == 0){//if we filled the entire line with characters
			count = 80;
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
// ----------------------------------------------------------------------------


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
	TEST_OUTPUT("Terminal Write test", terminal_write_test());
	TEST_OUTPUT("Terminal Read test", terminal_read_test());
	terminal_test();

	// RTC Tests 
	// TEST_OUTPUT("RTC_write_and_read_test", RTC_write_and_read_test());
	// TEST_OUTPUT("RTC_open_test", RTC_open_test());
		// File System Tests
			// test_file_driver_small_file();
			// test_file_driver_large_file();
			// test_file_driver_executable_file();
			// test_file_driver_read_twice();
			// test_dir_driver();
			// test_show_files();
			// test_show_frame();


}
