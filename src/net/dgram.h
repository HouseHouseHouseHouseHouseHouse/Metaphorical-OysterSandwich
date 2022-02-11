#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ip.h"
#include "udp.h"

#ifndef DGRAM_H
#define DGRAM_H

// Datagram Services
enum DgramService {
    DGRAM_NONE = 0,
    DGRAM_TFTP = 1
};

// Datagram Address Descriptor
typedef struct {
    ipv4Addr addr;
    port port;
} dgramDesc;

// Register a Datagram Channel
uint16_t dgram_register(dgramDesc, enum DgramService, uint32_t);

// Send a Datagram
int dgram_send(uint16_t, char *, uint16_t);

// Handle a Datagram
void dgram_handle(dgramChannel);

// Copy a Datagram
uint16_t dgram_copy(char *, uint16_t);

#endif
