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
struct {
    header header;
    uint16_t data[0xFFFF - sizeof(header)];
} __attribute__((packed)) send, recv;

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

    // Checksum
    send.header.checksum = 0;

    // Send IP Packet
    return ipv4_send(destAddr, UDP, (char *) &send, lengthTotal);
}
