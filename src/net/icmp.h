#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "ip.h"

#ifndef ICMP_H
#define ICMP_H

// ICMP Message Types
enum ICMPType {
    ECHO_REPLY = 0,
    ECHO_REQUEST = 8
};

// Handle a Message
void icmp_handle(ipv4Addr, ipv4Addr);

#endif
