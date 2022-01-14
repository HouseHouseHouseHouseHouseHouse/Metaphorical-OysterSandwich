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

// Current File Size
uint32_t fileSize;

// Magic String
static const char *ustar0 = "ustar";

// Initialization
bool tar_init(uint32_t *mbootInfo)
{
    // Check if Modules Exist
    if ((mbootInfo[0] & 1 << 3) == 0) return false;
    if (mbootInfo[5] == 0) return false;

    // Module Structure
    uint32_t *modStruct = (uint32_t *) mbootInfo[6];

    // Get Ramdisk Address
    filesystem = (block *) modStruct[0];
}

// Decode ASCII Octal
static uint32_t tar_decodeOctal(char *num, size_t length)
{
    uint32_t n = 0;

    // Iterate over every digit
    for (size_t i = 0; i < length - 1; i++) {
        n += (num[i] - '0' & 0x7) << (length - 2 - i) * 3;
    }

    return n;
}

// Encode ASCII Octal
static void tar_encodeOctal(char *buffer, size_t length, uint32_t num)
{
    // Iterate over every digit
    for (size_t i = 0; i < length - 1; i++) {
        buffer[i] = (num >> (i * 3) & 0x7) + '0';
    }
    buffer[length - 1] = 0;
}

// Find File
int tar_find(char *fname)
{
    // Filesystem Block
    size_t block = 0;

    // Loop through Valid Files
    while (true)
    {
        // Header Block
        fileHeader *header = (fileHeader *) &filesystem[block];

        // Verify Header Block
        for (size_t i = 0; i < 5; i++) {
            if (ustar0[i] != header->ustar0[i]) return -1;
        }
        if (tar_decodeOctal(header->version, 2) != 0) return -1;

        // Compare Filename
        for (size_t i = 0; i < 100; i++)
        {
            // End of Filename
            if (fname[i] == 0)
            {
                // Filename is a Match
                if (header->fname[i] == 0) {
                    fileBase = block;
                    fileSize = tar_decodeOctal(header->size, 12);
                    return 0;
                } else break;
            }

            // Compare Character
            if (fname[i] != header->fname[i]) break;
        }

        // Advance to next Header Block
        block += (tar_decodeOctal(header->size, 12) + 511) / 512 + 1;
    }
}

// Copy File Block
int tar_copy(uint16_t block, char *buffer)
{
    // Determine Length to Copy
    uint16_t length;
    if ((uint32_t) block * 0x0200 > fileSize) return -1;
    else if ((uint32_t) (block + 1) * 0x0200 > fileSize) length = fileSize & 0x01FF;
    else length = 0x0200;

    // Copy Data
    for (size_t i = 0; i < length; i++) {
        buffer[i] = filesystem[fileBase + block].x[i];
    }

    // Return Length
    return length;
}
