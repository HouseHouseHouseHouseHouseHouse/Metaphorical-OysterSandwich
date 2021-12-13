#include "int.h"
#include "mem.h"

// IDT will be held here
IDTEntry idt[256];

// IDT Register Structure
IDTRegister idtr = { .base = (uint32_t) &idt, .limit = sizeof(idt) - 1 };

// Initialize Interrupts
void int_init(void)
{
    for (int i = 0; i < 32; i++) {
        int_encodeIDTEntry(i, (uint32_t) &ex, false);
    }

    asm volatile (
        "lidt %0; sti" : :
        "m"(idtr)
    );
}

// Encode IDT Entry
void int_encodeIDTEntry(uint8_t target, uint32_t isr_base, bool trap)
{
    // Encode ISR Base Address
    idt[target].isr_low = isr_base & 0xFFFF;
    idt[target].isr_high = isr_base >> 16 & 0xFFFF;

    // Use Code Segment
    idt[target].selector = MEM_SEG_CODE;

    // Set Gate Type
    idt[target].attributes = 
        INT_IDT_ATTR_PR | 
        (trap ? INT_IDT_GATE_TRAP : INT_IDT_GATE_INT)
    ;

    // Clear Reserved Bits
    idt[target].reserved = 0;
}
