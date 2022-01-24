#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ip.h"

#ifndef UDP_H
#define UDP_H

// Ports
typedef uint16_t port;

// Send a Datagram
int udp_send(ipv4Addr, port, port, char *, uint16_t);

// Handle a Datagram
void udp_handle(ipv4Addr);

#endif
