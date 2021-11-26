#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MEM_SEG_NULL 0
#define MEM_SEG_CODE 8
#define MEM_SEG_DATA 16

#define MEM_GDT_ACCESS_PR 1 << 7
#define MEM_GDT_ACCESS_S 1 << 4
#define MEM_GDT_ACCESS_EX 1 << 3
#define MEM_GDT_ACCESS_RW 1 << 1

#define MEM_GDT_FLAG_GR 1 << 7
#define MEM_GDT_FLAG_SZ 1 << 6

#ifndef MEM_H
#define MEM_H

// Data required to make a GDT entry
struct GDTEntry {
    uint32_t base;
    uint32_t limit;
    bool executable;
};

// GDT will be held here
__attribute__((aligned(0x10)))
extern uint8_t gdt[256];

// GDT Register
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) GDTRegister;

extern GDTRegister gdtr;

// Basic GDT Entries
extern struct GDTEntry segNull, segCode, segData;

// Initialization
void mem_init(void);

// Encode a GDT entry
void mem_encodeGDTEntry(uint8_t, struct GDTEntry);

// Load GDT Register
void mem_loadGDT(void);

#endif
