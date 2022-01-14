#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef TAR_H
#define TAR_H

// Initialization
bool tar_init(uint32_t *);

// Find File
int tar_find(char *);

// Copy File Block
int tar_copy(uint16_t, char *);

#endif
