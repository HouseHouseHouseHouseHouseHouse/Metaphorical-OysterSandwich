#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtl.h"

#define IPV4(a, b, c, d) \
    a | \
    b << 8 | \
    c << 16 | \
    d << 24

#ifndef IP_H
#define IP_H

// IP Address
typedef uint32_t ipv4Addr;

// Our IP Address
extern ipv4Addr ip_ipv4Addr;

#endif
