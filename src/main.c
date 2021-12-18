#include "io/vga.h"
#include "mem/mem.h"
#include "int/int.h"
#include "net/rtl.h"

// Test Ethernet Frame - An ARP Request
char frame[52] = {
    // Hardware Address Type
    0x00, 0x01,

    // Protocol Address Type
    // 0x08, 0x00,
    0x86, 0xDD,

    // Hardware Address Size
    0x06,

    // Protocol Address Size
    // 0x04,
    0x10,

    // Opcode
    0x00, 0x01,

    // Hardware Address of Sender
    0, 0, 0, 0, 0, 0,

    // Protocol Address of Sender
    // 192, 168, 1, 16,
    0xFE, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF,

    // Hardware Address of Target
    0, 0, 0, 0, 0, 0,

    // Protocol Address of Target
    // 224, 0, 0, 1
    0xFF, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};

void main(void)
{
    // Perform Initializations
    mem_init();
    int_init();
    rtl_init();
    vga_init();

    // Print Brand String
    vga_println("Metaphorical-OysterSandwich, by Jacob Bates");

    // Print MAC Address
    for (size_t i = 0; i < 6; i++) {
        vga_printHex(rtl_macAddr.x[i]);
        if (i < 5) vga_printChar('-');
    }
    vga_println("");

    // Send Test Frame
    rtl_transmit((char *) &frame, sizeof(frame), ARP, broadcastAddr);

    // Wait and send another
    for (uint32_t i = 0; i < 1e9; i++);
    rtl_transmit((char *) &frame, sizeof(frame), ARP, broadcastAddr);

    // Hold here
    while (true);
}
