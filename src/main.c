#include "io/vga.h"
#include "mem/mem.h"
#include "int/int.h"
#include "file/tar.h"

#include "net/rtl.h"
#include "net/arp.h"
#include "net/ip.h"
#include "net/udp.h"

void main(void)
{
    // Multiboot Information
    uint32_t *mbootInfo;
    asm volatile ("movl %%ebx, %0" : "=r"(mbootInfo));

    // Perform Initializations
    mem_init();
    int_init();
    vga_init();

    tar_init(mbootInfo);

    if (!rtl_init()) {
        vga_println("Failed to initialize Ethernet Adaptor");
        return;
    }

    arp_init();

    // Print Brand String
    vga_println("Metaphorical-OysterSandwich, by Jacob Bates");

    // Print MAC Address
    for (size_t i = 0; i < 6; i++) {
        vga_printHex(rtl_macAddr.x[i]);
        if (i < 5) vga_printChar('-');
    }
    vga_println("");

    tar_find("initrd/config");

    // Send Ethernet Frame
    rtl_transmit("Hello, world", 12, ETHER, broadcastAddr);

    // Send IP Packets
    udp_send(IPV4(192, 168, 1, 1), 44444, 44444, "Hello, world", 12);

    // Hold here
    while (true);
}
