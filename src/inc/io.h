#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef IO_H
#define IO_H

// Output
void outb(uint16_t, uint8_t);
void outw(uint16_t, uint16_t);
void outl(uint16_t, uint32_t);

// Input
uint8_t inb(uint16_t);
uint16_t inw(uint16_t);
uint32_t inl(uint16_t);

#endif
