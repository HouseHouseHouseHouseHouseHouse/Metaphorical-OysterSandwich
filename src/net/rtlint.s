// Interrupt Handler for Network Driver
.section .text

.global rtl_int
.type rtl_int, @function
rtl_int:
    pushad
    cld
    call rtl_intHandler
    popad
    iret
