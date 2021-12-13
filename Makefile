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

.PHONY: all test debug sys iso clean test

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

test:
	qemu-system-i386 -cdrom build.iso \
	-netdev user,id=if0 \
	-device rtl8139,netdev=if0,mac=52:54:00:12:34:56 \
	-object filter-dump,id=network_filter_object,netdev=if0,file=netdump.dat \
	-s -S
