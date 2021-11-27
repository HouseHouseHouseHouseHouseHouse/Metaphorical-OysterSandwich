#include "rtl.h"
#include "io.h"
#include "pci.h"

// Initialize
bool rtl_init(void)
{
    // Check Vendor/Device ID
    if (pciConfig_vendorDevice(RTL_PCI_ADDR) != RTL_PCI_VENDOR_DEVICE) return false;

    // Enable Bus Mastering
    pciConfig_busMaster(RTL_PCI_ADDR);

    // Set IO Base
    pciConfig_ioBase(RTL_PCI_ADDR, RTL_IOBASE);

    // Power On
    outb(RTL_IOBASE + RTL_CONFIG_1, 0);

    return true;
}
