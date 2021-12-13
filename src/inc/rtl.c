#include "rtl.h"
#include "io.h"
#include "pci.h"
#include "vga.h"

// Ethernet Framebuffer
static struct {
    macAddr src;
    macAddr dest;
    uint16_t etherType;
    uint8_t data[0x0700 - 14];
} __attribute__((packed)) framebuffer;

// MAC Address
macAddr rtl_macAddr;
const macAddr broadcastAddr = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

// IO Base Address
static uint16_t ioBase;

// Keep track of the Transmission Register pair
static uint8_t tr;

// Initialize
bool rtl_init(void)
{
    // Check Vendor/Device ID
    if (pciConfig_vendorDevice(RTL_PCI_ADDR) != RTL_PCI_VENDOR_DEVICE) return false;

    // Enable Bus Mastering
    pciConfig_busMaster(RTL_PCI_ADDR);

    // Get IO Base
    ioBase = pciConfig_ioBase(RTL_PCI_ADDR);

    // Power On
    outb(ioBase + RTL_CONFIG1, 0);

    // Reset
    rtl_reset();

    // Get MAC Address
    for (int i = 0; i < 6; i++) {
        rtl_macAddr.x[i] = inb(ioBase + RTL_ID0 + i);
    }

    // Enable Transmission
    outb(ioBase + RTL_CR, RTL_CR_TE);

    return true;
}

// Perform a Software Reset
static void rtl_reset(void)
{
    // Send Command
    outb(ioBase + RTL_CR, RTL_CR_RST);

    // Poll this bit until it clears
    while ((inb(ioBase + RTL_CR) & RTL_CR_RST) != 0);

    // Reset Transmission Register pair
    tr = 0;
}

// Transmit an Ethernet Frame
void rtl_transmit(char *data, size_t length, enum EtherType etherType, macAddr dest)
{
    // Set Source/Destination Addresses and Ether-type
    framebuffer.src = rtl_macAddr;
    framebuffer.dest = dest;
    framebuffer.etherType = etherType;

    // Copy Data
    for (size_t i = 0; i < length; i++) {
        framebuffer.data[i] = data[i];
    }

    // Set Transmission Start Address
    outl(ioBase + RTL_TSAD0 + tr * 4, (uint32_t) &framebuffer);
    
    // Set Size and Transmit
    outl(ioBase + RTL_TSD0 + tr * 4, (uint32_t) length + 14);

    // Wait for the Transmission
    while ((inl(ioBase + RTL_TSD0 + tr * 4) & RTL_TSD_TOK) == 0);
    vga_println("TOK");

    // Increment Transmission Register Pair
    tr++;
    tr %= 4;
}
