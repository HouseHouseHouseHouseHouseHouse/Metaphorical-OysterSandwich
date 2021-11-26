// Multiboot Header Constants
.set FLAGS,     1 << 0 | 1 << 1         // Provide memory map and align modules
.set MAGIC,     0x1BADB002              // Multiboot magic number

// Multiboot Header
.section .multiboot
.align 4

.long MAGIC
.long FLAGS
.long -(MAGIC + FLAGS)

// Stack
.section .bss
.align 16

stack_base:
.skip 0x4000
stack_top:

// Entry Point
.section .text

.global _start
.type _start, @function
_start:

    mov     $stack_top  , %esp          // Set stack pointer
    call    main                        // Call main program

    int     $0x00

    cli

.end:                                   // Loop here when done
    hlt
    jmp     .end

// Size of Kernel
.size _start, . - _start
