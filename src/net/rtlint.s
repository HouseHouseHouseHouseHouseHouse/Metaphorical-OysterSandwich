// Interrupt Handler for Network Driver
.section .text

.global rtl_isr
.type rtl_isr, @function
rtl_isr:
    pushal
    cld
    call rtl_intHandler
    popal
    iret
