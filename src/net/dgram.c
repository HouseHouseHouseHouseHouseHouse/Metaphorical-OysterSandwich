#include "dgram.h"

#include "udp.h"

/*
    A service can register a port at any time. It can specify the number or choose not to.
    When registering, a service can provide an association key for its own use.
    In order to register, the service must provide an address/port for a remote machine.
    It can specify zero for either of these to indicate all addresses/ports.
    When a datagram is received, this interface will check if the sender matches the remote
    machine and pass it to the appropriate service along with the association key, port number,
    and remote address/port.
    A service can send a datagram by providing the port number an address/port if not specified
    in the registration.
*/

// Registry of Datagram Ports
static struct {
    enum DgramService service;
    uint32_t association;
    dgramDesc remote;
} registry[65536];
port registryCounter = 1024;

// Catch-all Descriptor
dgramDesc dgram_descAll = {
    .addr = 0,
    .port = 0
};

// Register a Port
uint16_t dgram_register(port port, dgramDesc remote, enum DgramService service, uint32_t association)
{
    // Assign a Port if needed
    if (port == 0) port = registryCounter++;
    if (registryCounter == 65536) registryCounter = 1024;

    // Copy Service & Association
    registry[port].service = service;
    registry[port].association = association;

    // Copy Remote Descriptor
    registry[port].remote.addr = remote.addr;
    registry[port].remote.port = remote.port;

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
    if (registry[port].remote.addr != src.addr && registry[port].remote.addr != 0) return;
    if (registry[port].remote.port != src.port && registry[port].remote.port != 0) return;

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
