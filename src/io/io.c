#include "io.h"

// Output
void outb(uint16_t port, uint8_t x)
{
    asm volatile (
        "outb %0, %1" : :
        "a"(x), "Nd"(port)
    );
}

void outw(uint16_t port, uint16_t x)
{
    asm volatile (
        "outw %0, %1" : :
        "a"(x), "Nd"(port)
    );
}

void outl(uint16_t port, uint32_t x)
{
    asm volatile (
        "outl %0, %1" : :
        "a"(x), "Nd"(port)
    );
}

// Input
uint8_t inb(uint16_t port)
{
    uint8_t x;

    asm volatile (
        "inb %1, %0" :
        "=a"(x) :
        "Nd"(port)
    );

    return x;
}

uint16_t inw(uint16_t port)
{
    uint16_t x;

    asm volatile (
        "inw %1, %0" :
        "=a"(x) :
        "Nd"(port)
    );

    return x;
}

uint32_t inl(uint16_t port)
{
    uint32_t x;

    asm volatile (
        "inl %1, %0" :
        "=a"(x) :
        "Nd"(port)
    );

    return x;
}
