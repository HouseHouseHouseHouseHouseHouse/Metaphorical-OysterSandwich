#include "udp.h"

#include "ip.h"

// Datagram Header
typedef struct {
    port src;
    port dest;
    uint16_t length;
    uint16_t checksum;
} __attribute__((packed)) header;

// Buffers
__attribute__((aligned(0x10)))
static struct {
    header header;
    uint8_t data[0xFFFF - sizeof(header)];
} __attribute__((packed)) send, recv;

// IP Pseudo-Header
static struct {
    ipv4Addr src;
    ipv4Addr dest;
    uint8_t zero;
    uint8_t prot;
    uint16_t lengthUDP;
} __attribute__((packed)) pseudoHeader;

// Send a Datagram
int udp_send(ipv4Addr destAddr, port src, port dest, char *data, uint16_t length)
{
    // Header Fields
    send.header.src = num_endian(src);
    send.header.dest = num_endian(dest);

    // Length
    uint16_t lengthTotal = sizeof(send.header) + length;
    send.header.length = num_endian(lengthTotal);

    // Copy Data
    if (length > sizeof(send.data)) return -1;
    for (size_t i = 0; i < length; i++) {
        send.data[i] = data[i];
    }

    // IP Pseudo-Header
    pseudoHeader.src = ipv4_getAddr(destAddr);
    pseudoHeader.dest = destAddr;
    pseudoHeader.lengthUDP = num_endian(lengthTotal);
    pseudoHeader.zero = 0;
    pseudoHeader.prot = UDP;

    // Checksum
    send.header.checksum = 0;
    send.header.checksum =
        ip_checksum((uint16_t *) &send, lengthTotal) +
        ip_checksum((uint16_t *) &pseudoHeader, sizeof(pseudoHeader))
    ;
    if (send.header.checksum == 0) send.header.checksum = ~0;

    // Send IP Packet
    return ipv4_send(destAddr, UDP, (char *) &send, lengthTotal);
}

// Handle a Datagram
void udp_handle(ipv4Addr srcAddr)
{
    // Copy Datagram
    ipv4_copy((char *) &recv, sizeof(recv));
    uint16_t length = sizeof(recv.header) + recv.header.length;

    // IP Pseudo-Header
    pseudoHeader.src = srcAddr;
    pseudoHeader.dest = ipv4_getAddr(srcAddr);
    pseudoHeader.lengthUDP = num_endian(length);
    pseudoHeader.zero = 0;
    pseudoHeader.prot = UDP;

    // Verify Checksum
    if (
        ip_checksum((uint16_t *) &recv, length) +
        ip_checksum((uint16_t *) &pseudoHeader, sizeof(pseudoHeader))
    != 0xFFFF) return;

    // Handle by Protocol
    switch (num_endian(recv.header.dest))
    {
        // Port 0
        case UDP_NONE:
            break;

        // Check Sockets
        default:
            break;
    }
}
