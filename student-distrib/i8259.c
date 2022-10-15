/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* Initialize the 8259 PIC */
void i8259_init(void) {
    unsigned char mask1, mask2;

    // saving masks 
    mask1 = inb(MASTER_8259_PORT+1);
    mask2 = inb(SLAVE_8259_PORT+1);

    // ICW1 - 1 = 0x10
    outb(ICW1-1 | ICW4, MASTER_8259_PORT);  // starts the initialization sequence (in cascade mode)
	// io_wait();
	outb(ICW1-1 | ICW4, SLAVE_8259_PORT);
	// io_wait();
	outb(ICW2_MASTER,MASTER_8259_PORT+1);                 // ICW2: Master PIC vector offset
	// io_wait();
	outb(ICW2_SLAVE, SLAVE_8259_PORT+1);                 // ICW2: Slave PIC vector offset
	// io_wait();
	outb(ICW3_MASTER, MASTER_8259_PORT+1);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	// io_wait();
	outb(ICW3_SLAVE,SLAVE_8259_PORT+1);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	// io_wait();
 
	outb(ICW4,MASTER_8259_PORT+1);
	// io_wait();
	outb(ICW4,SLAVE_8259_PORT+1);
	// io_wait();
 
    // restore saved masks.
	outb( mask1,MASTER_8259_PORT+1);   
	outb(mask2,SLAVE_8259_PORT+1);
}

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint16_t value; 
    // Primary PIC interupt is set
    if(irq_num<8){
        port = MASTER_8259_PORT+1;
    }
    // Secondary PIC interupt is set 
    else{
        port = SLAVE_8259_PORT+1;
        irq_num -= 8;
    }
    value = inb(port) & ~(1 << irq_num);
    outb (value,port);
}

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint16_t value; 
    // Primary PIC interupt is set
    if(irq_num<8){
        port = MASTER_8259_PORT+1;
    }
    // Secondary PIC interupt is set 
    else{
        port = SLAVE_8259_PORT+1;
        irq_num -= 8;
    }
    value = inb(port) | (1 << irq_num);
    outb (value, port);
}

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if(irq_num > 7){
        outb(EOI,SLAVE_8259_PORT);
    }
    outb(EOI, MASTER_8259_PORT);
}
