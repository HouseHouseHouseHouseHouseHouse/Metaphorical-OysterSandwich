#include "tftp.h"

#include "dgram.h"

// Open Connections/Transfers
static struct {
    uint16_t block;
    bool write;
    char fname[100];
} transfers[256];
static uint8_t transferCounter = 0;

// Packet Buffer
static struct {
    uint16_t op;
    uint16_t block;
    char data [0xFFFF - 2 * sizeof(uint16_t)];
} __attribute__((packed)) buffer;

// Initialization
void tftp_init(void)
{
    // Register Initial Connection Port
    dgram_register(TFTP_INIT_PORT, dgram_descAll, DGRAM_TFTP, 0);
}

// Prepare next Data Packet for a Transfer
uint16_t tftp_data(uint8_t a)
{
    // Is this a writing transfer?
    if (transfers[a].write) return 0;

    // Prepare Buffer
    buffer.op = TFTP_OP_DATA;
    buffer.block = num_endian(transfers[a].block);

    // Get File & Block
    tar_find(transfers[a].fname);
    return tar_copy(transfers[a].block, (char *) &buffer.data[2]) + 4;
}

// Handle a TFTP Packet
void tftp_handle(port port, uint32_t a, dgramDesc remote)
{
    // Copy Packet
    dgram_copy(&buffer, 0xFFFF);

    // Is this a new connection?
    if (port == DGRAM_TFTP)
    {
        // Is this a request?
        if (num_endian(buffer.op) == TFTP_OP_RRQ || num_endian(buffer.op) == TFTP_OP_WRQ)
        {
            // Register new Port for Transfer (association is transfer number)
            a = transferCounter;
            port = dgram_register(0, remote, DGRAM_TFTP, a);

            // Filename
            char *fname = (char *) buffer.block;
            for (size_t i = 0; i < 100; i++) {
                transfers[transferCounter].fname[i] = fname[i];
                if (fname[i] == 0) break;
            }

            // Block Number/Write
            transfers[transferCounter].block = 0;
            transfers[transferCounter].write = buffer.op == TFTP_OP_WRQ;

            // Increment Counter
            transferCounter++;
        }

        // Reply with ERROR for Write Request
        if (transfers[a].write) {
            buffer.op = TFTP_OP_ERR;
            return;
        }

        // Reply with First Data Block for Read Request
        else {
            dgram_send(port, remote, (char *) &buffer, tftp_data(a));
        }

        return;
    }

    // Operation
    switch (num_endian(buffer.op))
    {
        // Acknowledgement
        case TFTP_OP_ACK:

            // Error if not ACK'ing the block we just sent
            if (num_endian(buffer.block) != transfers[a].block) return;

            // Reply with Next Data Block
            dgram_send(port, remote, (char *) &buffer, tftp_data(a));
            break;
    }
}
