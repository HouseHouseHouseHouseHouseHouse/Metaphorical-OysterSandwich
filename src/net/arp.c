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
static packet request, reply, recv;

// ARP Cache/Log
static struct {
    ipv4Addr protAddr;
    macAddr hardAddr;
} cache[256];
uint8_t cacheCounter = 0;

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
        if (cache[i].protAddr == target) return cache[i].hardAddr;
    }

    // If nothing was found, make an ARP Request

    // Address Type Mapping
    request.hardType = num_endian(ETHER);
    request.protType = num_endian(IPV4);

    request.hardLen = sizeof(request.senderHardAddr);
    request.protLen = sizeof(request.senderProtAddr);

    // Sender Information
    request.senderHardAddr = rtl_macAddr;
    request.senderProtAddr = ipv4_addr;

    // Target Information
    request.targetHardAddr = emptyAddr;
    request.targetProtAddr = target;

    // Op-Code
    request.op = num_endian(REQUEST);

    // Send ARP Request
    rtl_transmit((char *) &request, sizeof(request), ARP, broadcastAddr);

    // Prepare next Entry in ARP Cache
    uint8_t cacheIndex = cacheCounter;
    cacheCounter++;
    cache[cacheIndex].protAddr = target;

    // Wait for and return the Reply
    while (rtl_eqMacAddr(cache[cacheIndex].hardAddr, emptyAddr));
    return cache[cacheIndex].hardAddr;
}

// Handle ARP Request
void arp_handleRequest(void)
{
    // Address Type Mapping
    reply.hardType = num_endian(ETHER);
    reply.protType = num_endian(IPV4);

    reply.hardLen = sizeof(request.senderHardAddr);
    reply.protLen = sizeof(request.senderProtAddr);

    // Sender Information
    reply.senderHardAddr = rtl_macAddr;
    reply.senderProtAddr = ipv4_addr;

    // Target Information
    reply.targetHardAddr = recv.senderHardAddr;
    reply.targetProtAddr = recv.senderProtAddr;

    // Op-Code
    reply.op = num_endian(REPLY);

    // Send ARP Request
    rtl_transmit((char *) &reply, sizeof(reply), ARP, recv.senderHardAddr);
}

// Handle ARP Reply
void arp_handleReply(void)
{
    // Find Cache Entry
    uint8_t cacheIndex = cacheCounter;
    for (size_t i = 0; i < 256; i++) {
        if (cache[i].protAddr == recv.senderProtAddr) {
            cacheIndex = i;
            break;
        }
    }

    // Make a new entry if nothing was found
    if (cacheIndex == cacheCounter) cacheCounter++;

    // Cache the Hardware Address
    cache[cacheIndex].hardAddr = recv.senderHardAddr;
}

// Handle ARP Packet
void arp_handle(uint16_t recvOffset)
{
    // Copy Packet Over
    rtl_copy(recvOffset, (char *) &recv, sizeof(recv));

    // Break if this packet is not for us
    if (recv.targetProtAddr != ipv4_addr) return;

    // Check Op-Code
    switch (num_endian(recv.op)) {
        case REQUEST:
            arp_handleRequest();
            break;

        case REPLY:
            arp_handleReply();
            break;

        default:
            break;
    }
}
