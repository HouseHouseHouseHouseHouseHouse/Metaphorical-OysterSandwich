DIR     := src/*/

CC      := i686-elf-gcc
AS      := i686-elf-as
LD      := i686-elf-ld

CFILES  := $(shell find $(DIR) -type f -name "*.c")
SFILES  := $(shell find $(DIR) -type f -name "*.s")
HFILES  := $(shell find $(DIR) -type f -name "*.h")

OBJS    := $(patsubst %.c, %.o, $(CFILES))
OBJS    += $(patsubst %.s, %.o, $(SFILES))

CCFLAGS := -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ASFLAGS :=

# For debugging
CCFLAGS += -g

LOOP0	:= /dev/loop6
LOOP1	:= /dev/loop7

.PHONY: all sys iso clean

all: iso clean

sys: $(OBJS) src/boot.o src/main.o
	$(CC) -T linker.ld -o boot/sys -nostdlib src/boot.o src/main.o $(OBJS) -lgcc

iso: sys
	mkdir iso
	cp -r boot/ iso/
	grub-mkrescue -o build.iso iso/
	rm -rf iso/

clean:
	rm src/boot.o src/main.o $(OBJS)

%.o: %.c $(HFILES)
	$(CC) $(CCFLAGS) -c $< -o $@

%.o: %.s
	$(AS) $(ASFLAGS) -c $< -o $@
