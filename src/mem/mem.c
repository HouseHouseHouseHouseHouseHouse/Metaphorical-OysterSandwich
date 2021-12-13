#include "mem.h"

// GDT will be held here
uint8_t gdt[256];

// GDT Register Structure
GDTRegister gdtr = { .base = (uint32_t) &gdt, .limit = sizeof(gdt) - 1 };

// Basic GDT Entries
struct GDTEntry segNull = { .base = 0, .limit = 0, .executable = false };
struct GDTEntry segCode = { .base = 0, .limit = 0xFFFFFFFF, .executable = true };
struct GDTEntry segData = { .base = 0, .limit = 0xFFFFFFFF, .executable = false };

// Initialize Memory
void mem_init(void)
{
    // Encode basic GDT entries
    mem_encodeGDTEntry(MEM_SEG_NULL, segNull);
    mem_encodeGDTEntry(MEM_SEG_CODE, segCode);
    mem_encodeGDTEntry(MEM_SEG_DATA, segData);

    // Load GDT
    mem_loadGDT();

    return;
}

// Encode a GDT entry
void mem_encodeGDTEntry(uint8_t target, struct GDTEntry source)
{
    // Set Flags; if our segment is too large, set granularity
    if (source.limit > 0xFFFF) {
        source.limit >>= 12;
        gdt [target + 6] = MEM_GDT_FLAG_SZ | MEM_GDT_FLAG_GR;
    } else {
        gdt [target + 6] = MEM_GDT_FLAG_SZ;
    }

    // Encode Limit
    gdt [target + 0] = source.limit & 0xFF;
    gdt [target + 1] = source.limit >> 8 & 0xFF;
    gdt [target + 6] |= source.limit >> 16 & 0xF;

    // Encode Base
    gdt [target + 2] = source.base & 0xFF;
    gdt [target + 3] = source.base >> 8 & 0xFF;
    gdt [target + 4] = source.base >> 16 & 0xFF;
    gdt [target + 7] = source.base >> 24 & 0xFF;

    // Encode Access Byte
    gdt [target + 5] = MEM_GDT_ACCESS_PR | MEM_GDT_ACCESS_S | MEM_GDT_ACCESS_RW;
    if (source.executable) gdt [target + 5] |= MEM_GDT_ACCESS_EX;

    return;
}
