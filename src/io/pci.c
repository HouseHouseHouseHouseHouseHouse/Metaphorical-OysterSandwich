#include "pci.h"
#include "io.h"

// Set PCI Configuration Address
static void pciConfig(uint16_t address, uint8_t offset)
{
    outl(PCI_CFG_ADDR,
        (uint32_t) address << 8 |
        (uint32_t) (offset & 0xFC) |
        PCI_CFG_ADDR_ENABLE
    );
}

// PCI Configuration Operations
uint16_t pciConfig_read(uint16_t address, uint8_t offset)
{
    pciConfig(address, offset);

    return (uint16_t) (inl(PCI_CFG_DATA) >> (offset & 2) * 8);
}

void pciConfig_write(uint16_t address, uint8_t offset, uint16_t x)
{
    pciConfig(address, offset);

    outl(PCI_CFG_DATA,
        inl(PCI_CFG_DATA) & 0xFFFF >> (offset & 2) * 8 |
        (uint32_t) x << (offset & 2) * 8
    );
}

// Get Vendor/Device ID
uint32_t pciConfig_vendorDevice(uint16_t address)
{
    pciConfig(address, PCI_CFG_VENDOR);

    return inl(PCI_CFG_DATA);
}

// Set Bus Mastering
void pciConfig_busMaster(uint16_t address)
{
    pciConfig(address, PCI_CFG_COMMAND);

    outl(PCI_CFG_DATA,
        inl(PCI_CFG_DATA) |
        PCI_CFG_COMMAND_BUS_MASTER
    );
}

// Get IO Base Address
uint16_t pciConfig_ioBase(uint16_t address)
{
    // Store BAR Value
    static uint32_t bar;

    // Check Header Type
    switch (pciConfig_read(address, PCI_CFG_HEADER) & 0xFF) {
        case 0:
            // Get IO BAR 0
            pciConfig(address, PCI_CFG_BAR0);
            bar = inl(PCI_CFG_DATA);

            // Return if it's an IO address
            if ((bar & 3) == 1) return bar & ~3;

        default:
            return 0;
    }
}

// Get Interrupt Line
uint8_t pciConfig_intLine(uint16_t address)
{
    pciConfig(address, PCI_CFG_INT);

    return inb(PCI_CFG_DATA);
}
