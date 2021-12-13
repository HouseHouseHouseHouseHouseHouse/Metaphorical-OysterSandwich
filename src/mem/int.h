#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define INT_IDT_ATTR_PR 1 << 7

#define INT_IDT_GATE_INT 0xE
#define INT_IDT_GATE_TRAP 0xF

#ifndef INT_H
#define INT_H

// Actual Structure of an IDT entry
typedef struct {
    uint16_t isr_low;
    uint16_t selector;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t isr_high;
} __attribute__((packed)) IDTEntry;

// IDT will be held here
__attribute__((aligned(0x10)))
extern IDTEntry idt[256];

// IDT Register
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) IDTRegister;

extern IDTRegister idtr;

// Exception Handler
extern void ex();

// Initialization
void int_init(void);

// Encode an IDT entry
void int_encodeIDTEntry(uint8_t, uint32_t, bool);

#endif
