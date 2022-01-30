#include "dgram.h"

#include "udp.h"

// Registry of Datagram Channels
static struct {
    dgramChannel desc;
    uint8_t service;
    uint32_t association;
} registry[256];
uint8_t registryCounter = 0;

// Register a Datagram Channel
uint16_t dgram_register(dgramChannel desc, enum DgramService service, uint32_t association)
{
    // Copy Descriptor
    registry[registryCounter].desc.localAddr = desc.localAddr;
    registry[registryCounter].desc.remoteAddr = desc.remoteAddr;
    registry[registryCounter].desc.localPort = desc.localPort;
    registry[registryCounter].desc.remotePort = desc.remotePort;

    // Copy Service
    registry[registryCounter].service = service;

    // Copy Association
    registry[registryCounter].association = association;

    // Return Index and Increment
    return registryCounter++;
}

// Send a Datagram
int dgram_send(uint16_t registryIndex, char *buffer, uint16_t length)
{
    // Send Datagram with UDP
    return udp_send(
        registry[registryIndex].desc.remoteAddr,
        registry[registryIndex].desc.localPort,
        registry[registryIndex].desc.remotePort,
        buffer, length
    );
}

// Handle a Datagram
void dgram_handle(dgramChannel desc)
{
    // Channel Index
    uint16_t registryIndex;

    // Find Channel
    for (size_t i = 0; i < 256; i++) {
        if (
            (registry[i].desc.localAddr == desc.localAddr || desc.localAddr == 0) &&
            (registry[i].desc.remoteAddr == desc.remoteAddr || desc.remoteAddr == 0) &&
            (registry[i].desc.localPort == desc.localPort || desc.localPort == 0) &&
            (registry[i].desc.remotePort == desc.remotePort || desc.remotePort == 0)
        ) {
            registryIndex = i;
            break;
        }
    }

    // Handle by Service
    switch(registry[registryIndex].service) {
        case DGRAM_TFTP:
            break;

        default:
            break;
    }
}

// Copy a Datagram
uint16_t dgram_copy(char *buffer, uint16_t length)
{
    return udp_copy(buffer, length);
}
