#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RTL_IOBASE 0x8000

#define RTL_CONFIG_1 0x52

#define RTL_PCI_VENDOR_DEVICE 0x813910EC
#define RTL_PCI_ADDR 0x0018

#ifndef RTL_H
#define RTL_H

// Initialize
void rtl_init(void);

#endif
