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

// Send an ARP Request
static void arp_request(ipv4Addr target)
{
    // Prepare next Cache Entry (default empty address)
    uint8_t cacheIndex = cacheCounter;
    cacheCounter++;
    cache[cacheIndex].hardAddr = emptyAddr;
    cache[cacheIndex].protAddr = target;

    // Address Type Mapping
    request.hardType = num_endian(ETHER);
    request.protType = num_endian(IPV4);

    request.hardLen = sizeof(macAddr);
    request.protLen = sizeof(ipv4Addr);

    // Sender Information
    request.senderHardAddr = rtl_macAddr;
    request.senderProtAddr = ipv4_addr;

    // Target Information
    request.targetHardAddr = emptyAddr;
    request.targetProtAddr = target;

    // Op-Code
    request.op = num_endian(REQUEST);

    // Transmit ARP Request
    rtl_transmit((char *) &request, sizeof(request), ARP, broadcastAddr);
}

// Send an ARP Reply
static void arp_reply(ipv4Addr target)
{
    // Address Type Mapping
    reply.hardType = num_endian(ETHER);
    reply.protType = num_endian(IPV4);

    reply.hardLen = sizeof(macAddr);
    reply.protLen = sizeof(ipv4Addr);

    // Sender Information
    reply.senderHardAddr = rtl_macAddr;
    reply.senderProtAddr = ipv4_addr;

    // Target Information
    reply.targetHardAddr = arp_query(target);
    reply.targetProtAddr = target;

    // Op-Code
    reply.op = num_endian(REPLY);

    // Send ARP Request
    rtl_transmit((char *) &reply, sizeof(reply), ARP, reply.targetHardAddr);
}

// Initialization
void arp_init(void)
{
    // Resolve Address of Gateway
    arp_request(ipv4_gate);

    // Wait for a Reply
    while (rtl_eqMacAddr(arp_query(ipv4_gate), emptyAddr));
}

// Query an Address
macAddr arp_query(ipv4Addr target)
{
    // Look through cache
    for (size_t i = 0; i < 256; i++)
    {
        // If addresses match, return hardware address
        if (cache[i].protAddr == target) return cache[i].hardAddr;
    }

    // Make an ARP Request
    arp_request(target);

    // Return Empty Address
    return emptyAddr;
}

// Manually Cache Address
void arp_cache(macAddr hardAddr, ipv4Addr protAddr)
{
    // Use an already existing cache entry
    for (size_t i = 0; i < 256; i++) {
        if (cache[i].protAddr == protAddr) {
            cache[i].hardAddr = hardAddr;
            return;
        }
    }

    // Otherwise, use next cache entry
    cache[cacheCounter].protAddr = protAddr;
    cache[cacheCounter].hardAddr = hardAddr;
    cacheCounter++;
}

// Handle ARP Packet
void arp_handle(uint16_t recvOffset)
{
    // Copy Packet Over
    rtl_copy(recvOffset, (char *) &recv, sizeof(recv));

    // Break if address types aren't right
    if (num_endian(recv.hardType) != ETHER || recv.hardLen != sizeof(macAddr)) return;
    if (num_endian(recv.protType) != IPV4 || recv.protLen != sizeof(ipv4Addr)) return;

    // Break if this packet is not for us
    if (recv.targetProtAddr != ipv4_addr) return;

    // Cache Address
    arp_cache(recv.senderHardAddr, recv.senderProtAddr);

    // If this is an ARP Request, send a Reply
    if (num_endian(recv.op) == REQUEST) arp_reply(recv.senderProtAddr);
}
