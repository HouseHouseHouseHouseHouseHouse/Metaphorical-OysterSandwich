#include "vga.c"

// Position
static size_t row, col;

// Display Buffer
static uint16_t *buffer;

// Initialize
void vga_init(void)
{
    // Set Buffer and Position
    buffer = (uint16_t *) 0xB8000
    row = 0;
    col = 0;

    // Clear Screen
    for (size_t i = 0; i < VGA_ROWS * VGA_COLS; i++) vga_printChar(' ');
}

// Advance Position
static void advance(void)
{
    if (++col %= VGA_COLS == 0) ++row %= VGA_ROWS;
}

// Print Character
void vga_printChar(char data)
{
    // Write into Buffer
    buffer [VGA_COLS * row + col] = VGA_GREY_BLACK | data;

    advance();
}

// Print String
void vga_print(char *data)
{
    // Loop until we reach a NULL
    for (size_t i = 0; data[i] != 0; i++) {
        // Write into Buffer
        buffer [VGA_COLS * row + col] = VGA_GREY_BLACK | data[i];

        advance();
    }
}

// Print Line
void vga_println(char *data)
{
    // Print Data
    vga_print(data);
    
    // Next Line
    row++;
    col = 0;
}

// Encode Nibble as ASCII Hex Digit
static char encodeNibbleHex(char data, bool lower)
{
    // Choose proper Nibble
    static char nibble = (data
        >> (upper ? 0 : 4)
        & 0xF) + '0'
    ;

    // Encode slightly differently for A-F
    if (nibble > '9') nibble += 'A' - '9';

    return nibble;
}

// Print Character as Hex
void vga_printHex(char data)
{
    // Print Nibbles Sequentially
    vga_printChar(encodeNibbleHex(data, false));
    vga_printChar(encodeNibbleHex(data, true));
}
