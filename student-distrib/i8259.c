/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

// ALL MAGIC NUMBER LABELS
#define maskinit 0xff
#define number 16
#define primary 8
#define irq2num 0x4
#define eoisignal 7
#define eoinum 0x2
#define irqs 8
// ALL MAGIC NUMBER LABELS

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/* void i8259_init();
 * Inputs: none
 * Return Value: void but makes sure to initialize the pic
 * Function: initializes the pic by initializing the master and slave then enabling irq */

/* Initialize the 8259 PIC */
void i8259_init(void) {

    // initializing the masks to having all IRQs are set to 1s 
    master_mask = maskinit;//inb(MASTER_8259_PORT+1);
    slave_mask = maskinit;//inb(SLAVE_8259_PORT+1);
    outb(master_mask,MASTER_8259_PORT+1);
    outb(slave_mask,SLAVE_8259_PORT+1);

    // master_mask= inb(MASTER_8259_PORT+1);
    // slave_mask= inb(SLAVE_8259_PORT+1);
    // ICW1 - 1 = 0x10
    outb((ICW1), MASTER_8259_PORT);  // starts the initialization sequence (in cascade mode)
	
	outb((ICW1), SLAVE_8259_PORT);
	
	outb(ICW2_MASTER,MASTER_8259_PORT+1);                 // ICW2: Master PIC vector offset
	
	outb(ICW2_SLAVE, SLAVE_8259_PORT+1);                 // ICW2: Slave PIC vector offset
	
	outb(ICW3_MASTER, MASTER_8259_PORT+1);                       // ICW3: tell Master PIC that there is a slave PIC at IRQ2 (0000 0100)
	
	outb(ICW3_SLAVE,SLAVE_8259_PORT+1);                       // ICW3: tell Slave PIC its cascade identity (0000 0010)
	
	outb(ICW4,MASTER_8259_PORT+1);

	outb(ICW4,SLAVE_8259_PORT+1);

    // restore saved masks.
	outb(master_mask,MASTER_8259_PORT+1);   
	outb(slave_mask,SLAVE_8259_PORT+1);
}

/* void enable_irq();
 * Inputs: irq_num whatever needs to be enabled
 * Return Value: void
 * Function: enables the particular irq that was given in the input */

/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    uint16_t port;
    uint16_t value; 
    if(irq_num>=number){
        return;
    }
    // Primary PIC interupt is set
    if(irq_num<irqs){
        port = MASTER_8259_PORT+1;
    }
    // Secondary PIC interupt is set 
    else{
        // primary port irq2 is set 
        uint16_t primary_port = MASTER_8259_PORT+1;
        uint16_t primary_value;
        primary_value = inb(primary_port) & ~(irq2num);
        outb(primary_value, primary_port);

        port = SLAVE_8259_PORT+1;
        irq_num -= primary;
    }
    value = inb(port) & ~(1 << irq_num);
    outb (value,port);
}

/* void disable_irq();
 * Inputs: irq_num whatever needs to be disabled
 * Return Value: void
 * Function: disables the particular irq that was given in the input */

/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    uint16_t port;
    uint16_t value; 
    // Primary PIC interupt is set
    if(irq_num<irqs){
        port = MASTER_8259_PORT+1; // increment to the next port
    }
    // Secondary PIC interupt is set 
    else{
        port = SLAVE_8259_PORT+1;
        irq_num -= primary;
    }
    value = inb(port) | (1 << irq_num); // or the bit that is related to the irq number 
    outb (value, port);
}

/* void send_eoi();
 * Inputs: irq_num whatever interrupt needs to be ended
 * Return Value: void
 * Function: sends a signal saying it in the end of the interrupt for the particular irq given */

/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    // printf("eoi");
    if(irq_num > eoisignal){
        outb((EOI | (irq_num-primary)),SLAVE_8259_PORT);
        outb(EOI | eoinum, MASTER_8259_PORT);
    }
    else{
    outb(EOI | irq_num, MASTER_8259_PORT);
    }
}
