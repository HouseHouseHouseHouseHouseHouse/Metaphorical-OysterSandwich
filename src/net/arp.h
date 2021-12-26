#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "rtl.h"
#include "ip.h"

#ifndef ARP_H
#define ARP_H

// Query an Address
macAddr arp_query(ipv4Addr target);

// Handle ARP Packet
void arp_handle(uint16_t);

#endif
