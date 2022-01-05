#include "ip.h"

#include "arp.h"

// Packet Header
typedef struct {
    uint8_t versionHeaderLength;
    uint8_t tos;
    uint16_t length;
    uint16_t fragID;
    uint16_t fragOffset;
    uint8_t ttl;
    uint8_t prot;
    uint16_t checksum;
    ipv4Addr src;
    ipv4Addr dest;
} __attribute__((packed)) header;

// Buffers
__attribute__((aligned(0x10)))
static struct {
    header header;
    uint8_t data[0xFFFF - sizeof(header)];
} __attribute__((packed)) send, recv;

// Our Configuration
ipv4Addr ipv4_addr = IPV4(192, 168, 1, 12);
ipv4Addr ipv4_subMask = IPV4(255, 255, 255, 0);
ipv4Addr ipv4_gate = IPV4(192, 168, 1, 1);

// Loopback Addresses
static const ipv4Addr ipv4_loAddr = IPV4(127, 0, 0, 1);
static const ipv4Addr ipv4_loRange = IPV4(127, 0, 0, 0);
static const ipv4Addr ipv4_loSubMask = IPV4(255, 0, 0, 0);

// Send a Packet
int ipv4_send(ipv4Addr dest, enum Protocol prot, char *data, uint16_t length)
{
    // Are we using Loopback
    bool lo = (dest & ipv4_loSubMask) == ipv4_loRange;

    // Set Header Fields
    send.header.versionHeaderLength = 4 << 4 | 5;
    send.header.tos = 0;
    send.header.fragID = 0;
    send.header.fragOffset = IP_FRAGFLAG_DF;
    send.header.ttl = 0x40;

    // Set Protocol
    send.header.prot = prot;

    // Addresses
    send.header.dest = dest;
    if (lo) send.header.src = ipv4_loAddr;
    else send.header.src = ipv4_addr;

    // Packet Length
    uint16_t lengthTotal = sizeof(send.header) + length;
    send.header.length = num_endian(lengthTotal);

    // Checksum
    send.header.checksum = 0;
    send.header.checksum = ip_checksum((uint16_t *) &send.header, sizeof(send.header));

    // Copy Data
    for (size_t i = 0; i < length; i++) {
        send.data[i] = data[i];
    }

    // Loopback
    if (lo)
    {
        // "Transmit" by copying to Receive Buffer
        for (size_t i = 0; i < lengthTotal; i++) {
            recv.data[i] = send.data[i];
        }

        // Handle Packet
        ipv4_handleActually();
        return 0;
    }

    // Length check for Ethernet
    else if (lengthTotal > RTL_MTU) return -1;

    // Local Destination MAC Address
    macAddr destLocal = arp_query(
        (dest & ipv4_subMask) == (ipv4_addr & ipv4_subMask) ?
        dest : ipv4_gate
    );

    // Transmit Frame over Ethernet
    return rtl_transmit((char *) &send, lengthTotal, IPV4, destLocal);
}

// Actually Handle a Packet
void ipv4_handleActually(void)
{
    // Validate Checksum
    if (ip_checksum((uint16_t *) &recv.header, sizeof(recv.header)) != 0) return;

    // Decrement TTL
    recv.header.ttl--;

    // Pass to Protocol
    switch (recv.header.prot) {
        default:
            break;
    }
}

// Handle a Packet
void ipv4_handle(macAddr src, uint16_t recvOffset)
{
    // Copy Packet Over
    rtl_copy(recvOffset, (char *) &recv, sizeof(recv));

    // Validate Version/Options
    if (recv.header.versionHeaderLength != (4 << 4 | 5)) return;

    // Validate Destination
    if (recv.header.dest != ipv4_addr) return;

    // Cache Source MAC Address
    arp_cache(src, recv.header.src);

    // Continue Handling Packet
    ipv4_handleActually();
}

// Copy Packet Payload
uint16_t ipv4_copy(char *buffer, uint16_t maxLength)
{
    // Data Length
    uint16_t length =
        num_endian(recv.header.length) -
        (recv.header.versionHeaderLength & 0x0F) * 4
    ;

    // Impose Maximum
    if (length > maxLength || length > sizeof(recv)) return 0;

    // Copy Data
    for (size_t i = 0; i < length; i++) {
        buffer[i] = recv.data[i];
    }

    // Return Length
    return length;
}

// Calculate IP Checksum
uint16_t ip_checksum(uint16_t *data, uint16_t length)
{
    uint32_t sum = 0;

    // Sum Words and Carry
    for (size_t i = 0; i < length / 2; i++) {
        sum += data[i];
        sum = (sum & 0xFFFF) + (sum >> 16);
    }

    if (length & 1) sum += data[length - 1] & 0xFF;

    // One's Complement
    return ~sum;
}
