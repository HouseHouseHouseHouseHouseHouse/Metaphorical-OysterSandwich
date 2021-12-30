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

// Send a Packet
int ipv4_send(ipv4Addr dest, enum Protocol prot, char *data, uint16_t length)
{
    // Set Header Fields
    send.header.versionHeaderLength = 4 << 4 | 5;
    send.header.tos = 0;
    send.header.fragID = 0;
    send.header.fragOffset = IP_FRAGFLAG_DF;
    send.header.ttl = 0x40;

    // Set Protocol
    send.header.prot = prot;

    // Addresses
    send.header.src = ipv4_addr;
    send.header.dest = dest;

    // Local Destination Address
    ipv4Addr destLocal =
        (dest & ipv4_subMask) == (ipv4_addr & ipv4_subMask) ?
        dest : ipv4_gate
    ;

    uint16_t lengthTotal = sizeof(send.header) + length;

    // Length
    if (lengthTotal > RTL_MTU) return -1;
    send.header.length = num_endian(lengthTotal);

    // Copy Data
    for (size_t i = 0; i < length; i++) {
        send.data[i] = data[i];
    }

    // Checksum
    send.header.checksum = 0;
    send.header.checksum = ip_checksum((uint16_t *) &send.header, sizeof(send.header));

    // Transmit Frame
    return rtl_transmit((char *) &send, lengthTotal, IPV4, arp_query(destLocal));
}

// Handle a Packet
void ipv4_handle(uint16_t recvOffset)
{
    // Copy Packet Over
    rtl_copy(recvOffset, (char *) &recv, sizeof(recv));

    // Validate Destination
    if (recv.header.dest != ipv4_addr) return;

    // Validate Checksum
    if (ip_checksum((uint16_t *) &recv.header, sizeof(recv.header)) != 0) return;

    // Pass to Protocol
    switch (recv.header.prot) {
        default:
            break;
    }
}

// Calculate IP Checksum
uint16_t ip_checksum(uint16_t *data, uint16_t length)
{
    uint16_t sum = 0;
    uint16_t units = 0;

    // Sum Words and Units
    for (size_t i = 0; i < length / 2; i++) {
        sum += data[i];
        units += data[i] >> 15 & 1;
    }

    // Carriage, One's Complement, and Byte Order
    return num_endian(~(sum + units / 2));
}
