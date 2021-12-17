#include "pic.h"

#include "../io/io.h"

// Initialize PIC
void pic_init(void)
{
    // Start Initialization (ICW1)
    outb(PIC0_CMD, PIC_CMD_INIT);
    outb(PIC1_CMD, PIC_CMD_INIT);

    // Set Offsets (ICW2)
    outb(PIC0_DATA, PIC0_OFFSET);
    outb(PIC1_DATA, PIC1_OFFSET);

    // Cascade Information (ICW3)
    outb(PIC0_DATA, PIC0_ICW3);
    outb(PIC1_DATA, PIC1_ICW3);

    // Environment Information (ICW4)
    outb(PIC0_DATA, PIC_ICW4);
    outb(PIC1_DATA, PIC_ICW4);

    // Clear All Interrupts (except Slave itself)
    outb(PIC0_DATA, ~(1 << 2));
    outb(PIC1_DATA, ~0);
}

// End Interrupt
void pic_sendEOI(uint8_t line)
{
    // Send Command
    outb(PIC0_CMD, PIC_CMD_END);

    // Send to both if the Slave is responsible
    if ((line & 0xF) < 8) outb(PIC1_CMD, PIC_CMD_END);
}

// Mask IRQ Line
void pic_maskIRQ(uint8_t line, bool service)
{
    // Data Port
    uint16_t port =
        (line & 0xF) < 8 ?
        PIC0_DATA : PIC1_DATA
    ;

    // Bitmask
    uint8_t mask = 1 << (line & 0x7);

    // Set/Unset Bit
    if (service) outb(port, inb(port) & ~mask);
    else outb(port, inb(port) | mask);
}
