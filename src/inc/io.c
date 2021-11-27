#include "io.h"

// Output
static inline void outb(uint16_t port, uint8_t x)
{
    asm volatile (
        "outb %0, %1" : :
        "a"(x), "Nd"(port)
    );
}

static inline void outw(uint16_t port, uint16_t x)
{
    asm volatile (
        "outw %0, %1" : :
        "a"(x), "Nd"(port)
    );
}

static inline void outl(uint16_t port, uint32_t x)
{
    asm volatile (
        "outl %0, %1" : :
        "a"(x), "Nd"(port)
    );
}

// Input
static inline uint8_t inb(uint16_t port)
{
    uint8_t x;

    asm volatile (
        "inb %1, %0" :
        "=a"(x) :
        "Nd"(port)
    );

    return x;
}

static inline uint16_t inb(uint16_t port)
{
    uint16_t x;

    asm volatile (
        "inw %1, %0" :
        "=a"(x) :
        "Nd"(port)
    );

    return x;
}

static inline uint32_t inb(uint16_t port)
{
    uint32_t x;

    asm volatile (
        "inl %1, %0" :
        "=a"(x) :
        "Nd"(port)
    );

    return x;
}
