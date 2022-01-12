#include "tar.h"

// File Header
typedef struct {
    char fname[100];
    char mode[8];
    char userID[8];
    char groupID[8];
    char size[12];
    char timeMod[12];
    char headerChecksum[8];
    char link;
    char fnameLink[100];

    char ustar0[6];
    char version[2];
    char userName[32];
    char groupName[32];
    char devMajor[8];
    char devMinor[8];
    char fnamePrefix[155];
} __attribute__((packed)) fileHeader;

// Filesystem Buffer (512-byte Blocks)
typedef struct { char x[0x0200] } __attribute__((packed)) block;
static block *filesystem;

// Current File Base Block
uint16_t fileBase;

// Magic String
static const char *ustar0 = "ustar\0";

// Initialization
void tar_init(void)
{
}

// Decode ASCII Octal
static uint32_t tar_decodeOctal(char *num, size_t length)
{
    uint32_t n = 0;

    // Iterate over every digit
    for (size_t i = 0; i < length; i++) {
        n += (num[i] - '0' & 0x7) << (length - i) * 3;
    }

    return n;
}

// Encode ASCII Octal
static void tar_encodeOctal(char *buffer, size_t length, uint32_t num)
{
    // Iterate over every digit
    for (size_t i = 0; i < length; i++) {
        buffer[i] = (num >> (i * 3) & 0x7) + '0';
    }
}

// Find File
int tar_find(char *fname)
{
    // Filesystem Block
    size_t block = 0;

    // Loop through Valid Files
    while (true)
    {
        // Verify Header Block
        for (size_t i = 0; i < 6; i++) {
            if (ustar0[i] != filesystem[block].ustar0[i]) return -1;
        }
        if (tar_decodeOctal(&filesystem[block].version, 2) != 0) return -1;

        // Compare Filename
        for (size_t i = 0; i < 100; i++)
        {
            // End of Filename
            if (fname[i] == 0)
            {
                // Filename is a Match
                if (filesystem[block].fname[i] == 0) {
                    fileBase = block;
                    return 0;
                } else break;
            }

            // Compare Character
            if (fname[i] != filesystem[block].fname[i]) break;
        }

        // Advance to next Header Block
        block += (tar_decodeOctal(filesystem[block].size) + 511) / 512 + 1;
    }
}
