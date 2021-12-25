#include "num.h"

// Endianness Conversion
uint16_t num_endian(uint16_t x)
{
    return
        x << 8 & 0xFF00 |
        x >> 8 & 0x00FF
    ;
}
