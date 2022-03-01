#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ip.h"
#include "udp.h"

#ifndef DGRAM_H
#define DGRAM_H

// Datagram Services
enum DgramService {
    DGRAM_NONE,
    DGRAM_TFTP
};

// Datagram Address Descriptor
typedef struct {
    ipv4Addr addr;
    port port;
} dgramDesc;

extern dgramDesc dgram_descAll;

// Register a Port
uint16_t dgram_register(port, dgramDesc, enum DgramService, uint32_t);

// Send a Datagram
int dgram_send(port, dgramDesc, char *, uint16_t);

// Handle a Datagram
void dgram_handle(port, dgramDesc);

// Copy a Datagram
uint16_t dgram_copy(char *, uint16_t);

#endif
