#include "icmp.h"

#include "ip.h"

// Message Header
typedef struct {
    uint8_t type;
    uint8_t code;
    uint16_t checksum;
    uint32_t rest;
} __attribute__((packed)) header;

// Buffers
__attribute__((aligned(0x10)))
static struct {
    header header;
    uint8_t data[0x0240];
} __attribute__((packed)) send, recv;
uint16_t length;

// Send a Prepared Message
int icmp_send(ipv4Addr dest)
{
    // Checksum
    send.header.checksum = ip_checksum((uint16_t *) &send, sizeof(send));

    // Send IP Packet
    return ipv4_send(dest, ICMP, (char *) &send, length);
}

// Handle an Echo Request (Prepare an Echo Reply)
void icmp_handleEchoRequest(void)
{
    // Header Fields
    send.header.type = ECHO_REPLY;
    send.header.code = 0;
    send.header.rest = recv.header.rest;

    // Copy Payload
    for (size_t i = 0; i < length - sizeof(recv.header); i++) {
        send.data[i] = recv.data[i];
    }
}

// Handle a Message
void icmp_handle(ipv4Addr src, ipv4Addr dest)
{
    // Copy Packet
    length = ipv4_copy((char *) &recv, sizeof(recv));
    if (length == 0) return;

    // Handle by Type
    switch (recv.header.type)
    {
        case ECHO_REQUEST:
            icmp_handleEchoRequest();
            icmp_send(src);
            break;

        default:
            break;
    }
}
