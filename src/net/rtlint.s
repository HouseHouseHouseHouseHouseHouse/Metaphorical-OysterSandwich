// Interrupt Handler for Network Driver
.section .text

.global rtl_int
.type rtl_int, @function
rtl_int:
    call rtl_intHandler
    iret
