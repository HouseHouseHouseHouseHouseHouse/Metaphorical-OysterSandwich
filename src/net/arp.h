#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtl.h"
#include "ip.h"

#ifndef ARP_H
#define ARP_H

// Initialization
void arp_init(void);

// Query an Address
macAddr arp_query(ipv4Addr);

// Manually Cache Address
void arp_cache(macAddr, ipv4Addr);

// Handle ARP Packet
void arp_handle(uint16_t);

#endif
