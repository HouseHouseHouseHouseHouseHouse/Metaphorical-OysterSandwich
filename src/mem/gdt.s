// Functions
.section .text

// Load GDT Register
.global mem_loadGDT
.type mem_loadGDT, @function
mem_loadGDT:
    lgdt    (gdtr)
    ljmp    $0x0008 , $reloadSegments

reloadSegments:
    movw    $0x0010 , %ax

    movw    %ax     , %ds
    movw    %ax     , %es
    movw    %ax     , %fs
    movw    %ax     , %gs
    movw    %ax     , %ss

    ret
