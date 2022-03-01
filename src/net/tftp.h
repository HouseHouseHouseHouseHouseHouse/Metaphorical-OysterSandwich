#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "dgram.h"

#define TFTP_OP_RRQ 1
#define TFTP_OP_WRQ 2
#define TFTP_OP_DATA 3
#define TFTP_OP_ACK 4
#define TFTP_OP_ERR 5

#define TFTP_ERR_NDEF 0
#define TFTP_ERR_ACC 2

#define TFTP_INIT_PORT 69

#ifndef TFTP_H
#define TFTP_H

// Initialization
void tftp_init(void);

// Handle a TFTP Packet
void tftp_handle(port, uint32_t, dgramDesc);

#endif
