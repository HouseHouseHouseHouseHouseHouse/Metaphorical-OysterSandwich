#include "inc/mem.h"
#include "inc/int.h"
#include "inc/rtl.h"
#include "inc/vga.h"

// Test Ethernet Frame - An ARP Request
char frame[42] = {
    // Destination Address
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    
    // Source Address
    0, 0, 0, 0, 0, 0,

    // Ethernet Protocol Type
    0x08, 0x06,

    // Hardware Address Type
    0x00, 0x01,

    // Protocol Address Type
    0x08, 0x00,

    // Hardware Address Size
    0x06,

    // Protocol Address Size
    0x04,

    // Opcode
    0x00, 0x01,

    // Hardware Address of Sender
    0, 0, 0, 0, 0, 0,

    // Protocol Address of Sender
    192, 168, 1, 251,

    // Hardware Address of Target
    0, 0, 0, 0, 0, 0,

    // Protocol Address of Target
    192, 168, 1, 123
};

void main(void)
{
    // Perform Initializations
    mem_init();
    int_init();
    rtl_init();
    vga_init();

    // Print Brand String
    vga_println("Metaphorical-OysterSandwich, by Jacob Bates\0");

    // Print MAC Address
    for (size_t i = 0; i < 6; i++) {
        vga_printHex(rtl_macAddr[i]);
        vga_printChar('-');
    }

    // Send Test Frame
    rtl_transmit(&frame, 42);
}
