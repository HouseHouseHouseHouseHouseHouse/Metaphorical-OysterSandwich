#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PIC0_CMD 0x0020
#define PIC0_DATA 0x0021

#define PIC1_CMD 0x00A0
#define PIC1_DATA 0x00A1

#define PIC_CMD_INIT 1 << 0 | 1 << 4
#define PIC_CMD_END 1 << 5

#define PIC0_OFFSET 0x20
#define PIC1_OFFSET 0x28

#define PIC0_ICW3 4
#define PIC1_ICW3 2

#define PIC_ICW4 1 << 0

#ifndef PIC_H
#define PIC_H

// Initialization
void pic_init(void);

// End Interrupt
void pic_endInterrupt(uint8_t);

// Mask IRQ Line
void pic_maskIRQ(uint8_t, bool);

#endif
