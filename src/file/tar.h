#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifndef TAR_H
#define TAR_H

// Initialization
void tar_init(void);

// Find File
int tar_find(char *);

// Copy File Block
void tar_copy(uint16_t);

#endif
