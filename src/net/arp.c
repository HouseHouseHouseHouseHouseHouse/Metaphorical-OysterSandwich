#include "arp.h"

#include "num.h"
#include "rtl.h"

// Packet Structure
typedef struct {
    uint16_t hardType;
    uint16_t protType;
    uint8_t hardLen;
    uint8_t protLen;
    uint16_t op;
    macAddr senderHardAddr;
    ipv4Addr senderProtAddr;
    macAddr targetHardAddr;
    ipv4Addr targetProtAddr;
} __attribute__((packed)) packet;

// Buffers
static packet request, reply;

// ARP Cache/Log
static struct {
    ipv4Addr protAddr;
    macAddr hardAddr;
} arp_cache[256];
uint8_t arp_cacheCounter = 0;

// ARP Packet Op-Code
enum OpCode {
    REQUEST = 1,
    REPLY = 2
};

// Query an Address
macAddr arp_query(ipv4Addr target)
{
    // Look through cache
    for (size_t i = 0; i < 256; i++) {
        // If addresses match, return hardware address
        if (
            arp_cache[i].protAddr == target
        ) return arp_cache[i].hardAddr;
    }

    // If nothing was found, make an ARP Request

    // Address Type Mapping
    request.hardType = num_endian(ETHER);
    request.protType = num_endian(IPV4);

    request.hardLen = sizeof(request.senderHardAddr);
    request.protLen = sizeof(request.senderProtAddr);

    // Sender Information
    request.senderHardAddr = rtl_macAddr;
    request.senderProtAddr = ip_ipv4Addr;

    // Target Information
    request.targetHardAddr = emptyAddr;
    request.targetProtAddr = target;

    // Op-Code
    request.op = num_endian(REQUEST);

    // Send ARP Request
    rtl_transmit((char *) &request, sizeof(request), ARP, broadcastAddr);
}
