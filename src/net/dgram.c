#include "dgram.h"

#include "udp.h"

/*
    A service can register a port at any time. It can specify the number or choose not to.
    When registering, a service can provide an association key for its own use.
    In order to register, the service must provide an address/port for a remote machine.
    It can specify zero for either of these to indicate all addresses/ports.
    When a datagram is received, this interface will check if the sender matches the remote
    machine and pass it to the appropriate service along with the association key.
    A service can send a datagram by providing the port number an address/port if not specified
    in the registration.
*/

// Registry of Datagram Ports
static struct {
    uint8_t service;
    uint32_t association;
    dgramDesc remote;
} registry[65536];
port registryCounter = 1024;

// Register a Datagram Channel
uint16_t dgram_register(port port, enum DgramService service, uint32_t association, dgramDesc remote)
{
    // Assign a Port if needed
    if (port == 0) port = registryCounter++;
    if (registryCounter == 65536) registryCounter = 1024;

    // Copy Service & Association
    registry[port].service = service;
    registry[port].association = association;

    // Copy Remote Descriptor
    registry[port].desc.addr = desc.addr;
    registry[port].desc.port = desc.port;

    // Return Port and Increment
    return port;
}

// Send a Datagram
int dgram_send(port port, dgramDesc dest, char *buffer, uint16_t length)
{
    // Send Datagram with UDP
    return udp_send(
        // Give precedence to specified destination addr/port
        dest.addr != 0 ? dest.addr : registry[port].remote.addr,
        port,
        dest.port != 0 ? dest.port : registry[port].remote.port,
        buffer, length
    );
}

// Handle a Datagram
void dgram_handle(port port, dgramDesc src)
{
    // Does this Datagram pass restriction?
    if (registry[port].desc.addr != src.addr && registry[port].desc.addr != 0) break;
    if (registry[port].desc.port != src.port && registry[port].desc.port != 0) break;

    // Handle by Service
    switch (registry[port].service) {
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
