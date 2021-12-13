#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RTL_ID0 0x00

#define RTL_TSD0 0x10
#define RTL_TSAD0 0x20

#define RTL_CR 0x37
#define RTL_CONFIG1 0x52

#define RTL_TSD_OWN 1 << 13
#define RTL_TSD_TOK 1 << 15

#define RTL_CR_TE 1 << 2
#define RTL_CR_RE 1 << 3
#define RTL_CR_RST 1 << 4

#define RTL_PCI_VENDOR_DEVICE 0x813910EC
// #define RTL_PCI_ADDR 3 << 8 | 3 << 3 | 0
#define RTL_PCI_ADDR 0 << 8 | 3 << 3 | 0

#ifndef RTL_H
#define RTL_H

// MAC Addresses
typedef struct { uint8_t x[6]; } macAddr;

extern macAddr rtl_macAddr;
extern const macAddr broadcastAddr;

// Ether-type
enum EtherType {
    ARP = 0x08 | 0x06 << 8
};

// IO Base Address
static uint16_t ioBase;

// Keep track of the Transmission Register pair
static uint8_t tr;

// Initialize
bool rtl_init(void);

// Perform a Software Reset
static void rtl_reset(void);

// Transmit an Ethernet Frame
void rtl_transmit(char *, size_t, enum EtherType, macAddr);

#endif
