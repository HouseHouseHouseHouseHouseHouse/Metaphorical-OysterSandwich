#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define VGA_ROWS 25
#define VGA_COLS 80

#define VGA_GREY_BLACK 0x07 << 8

// Position
static size_t row, col;

// Display Buffer
static uint16_t *buffer;

// Initialize
void vga_init(void);

// Print Character
void vga_printChar(char);

// Print String
void vga_print(char *);

// Print Line
void vga_println(char *);

// Print Character as Hex
void vga_printHex(char);
