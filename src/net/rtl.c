#include "rtl.h"

#include "../io/io.h"
#include "../io/pci.h"
#include "../io/vga.h"
#include "../int/int.h"

// Transmission Buffer
static struct {
    etherHeader header;
    uint8_t data[0x0700 - 14];
} __attribute__((packed)) sendbuffer;

// Receipt Buffer
static uint16_t recvOffset = 0;
static uint8_t recvbuffer[0x2010];

// MAC Address
macAddr rtl_macAddr;
const macAddr broadcastAddr = {{0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF}};

// IO Base Address
static uint16_t ioBase;

// Interrupt Line
static uint8_t intLine;

// Keep track of the Transmission Register pair
static uint8_t tr;

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

// Initialize
bool rtl_init(void)
{
    // Check Vendor/Device ID
    if (pciConfig_vendorDevice(RTL_PCI_ADDR) != RTL_PCI_VENDOR_DEVICE) return false;

    // Get IO Base
    ioBase = pciConfig_ioBase(RTL_PCI_ADDR);

    // Set up ISR
    intLine = pciConfig_intLine(RTL_PCI_ADDR);
    int_setupIRQ(intLine, (uint32_t) &rtl_isr);

    // Enable Bus Mastering
    pciConfig_busMaster(RTL_PCI_ADDR);

    // Power On
    outb(ioBase + RTL_CONFIG1, 0);

    // Reset
    rtl_reset();

    // Get MAC Address
    for (int i = 0; i < 6; i++) {
        rtl_macAddr.x[i] = inb(ioBase + RTL_ID0 + i);
    }

    // Receive Buffer Location
    outl(ioBase + RTL_RBSTART, (uint32_t) &recvbuffer);

    // Configure Interrupts
    outw(ioBase + RTL_IMR, RTL_ISR_ROK | RTL_ISR_TOK | RTL_ISR_LINKCHG | RTL_ISR_SERR);

    // Enable Transmitter & Receiver
    outb(ioBase + RTL_CR, RTL_CR_TE | RTL_CR_RE);

    // Configure Receipt (match/broadcast, wrap)
    outl(ioBase + RTL_RCR, RTL_RCR_APM | RTL_RCR_AB | RTL_RCR_WRAP);

    return true;
}

// Transmit an Ethernet Frame
void rtl_transmit(char *data, size_t length, enum EtherType etherType, macAddr dest)
{
    // Set Header Fields
    sendbuffer.header.src = rtl_macAddr;
    sendbuffer.header.dest = dest;
    sendbuffer.header.etherType = etherType;

    // Copy Data
    for (size_t i = 0; i < length; i++) {
        sendbuffer.data[i] = data[i];
    }

    // Set Transmission Start Address
    outl(ioBase + RTL_TSAD0 + tr * 4, (uint32_t) &sendbuffer);
    
    // Set Size and Transmit
    outl(ioBase + RTL_TSD0 + tr * 4, (uint32_t) length + 14);

    // Wait for the Transmission
    // while ((inl(ioBase + RTL_TSD0 + tr * 4) & RTL_TSD_TOK) == 0);
    // vga_println("TOK");

    // Increment Transmission Register Pair
    tr++;
    tr %= 4;
}

// Handle Receipt of an Ethernet Frame
void rtl_receive(void)
{
    // Get length of packet from receipt header
    uint16_t length =
        recvbuffer[recvOffset + RTL_RECV_LEN] |
        recvbuffer[recvOffset + RTL_RECV_LEN + 1] << 8
    ;

    // Location of ethernet packet header (just past receipt header)
    etherHeader *header =
        (etherHeader *) &recvbuffer[recvOffset + RTL_RECV_PACKET]
    ;

    // Location of ethernet packet data (just past ethernet header)
    uint8_t *data = (uint8_t *) header + sizeof(etherHeader);

    // Print Source MAC
    for (size_t i = 0; i < 6; i++) {
        vga_printHex(header->src.x[i]);
        if (i < 5) vga_printChar('-');
    }
    vga_println("");

    // Print Data
    for (size_t i = RTL_RECV_PACKET + sizeof(etherHeader); i < length - 4; i++) {
        vga_printChar(
            // Wrap around buffer
            recvbuffer[(recvOffset + i) % sizeof(recvbuffer)]
        );
    }
    vga_println("");

    // Update Software Pointer
    recvOffset = recvOffset + length + 4 + 3 & ~3;
    recvOffset %= sizeof(recvbuffer);

    // Update Driver Status
    outw(ioBase + RTL_CAPR, recvOffset - 0x10);
}

// Handle an Interrupt
void rtl_intHandler(void)
{
    // Check the Interrupt Source
    uint16_t intSource = inw(ioBase + RTL_ISR);

    // Receive-OK
    if ((intSource & RTL_ISR_ROK) != 0) {
        vga_println("ISR_ROK");
        rtl_receive();
        outw(ioBase + RTL_ISR, RTL_ISR_ROK);
    }

    // Transmit-OK
    else if ((intSource & RTL_ISR_TOK) != 0) {
        vga_println("ISR_TOK");
        outw(ioBase + RTL_ISR, RTL_ISR_TOK);
    }

    // Link Change
    else if ((intSource & RTL_ISR_LINKCHG) != 0) {
        vga_println("Link Change");
        outw(ioBase + RTL_ISR, RTL_ISR_LINKCHG);
    }

    // System Error
    else if ((intSource & RTL_ISR_SERR) != 0) rtl_reset();

    // End of ISR
    int_end(intLine);
}
