
        .global start, exit

        .section .text, "x"

start:  
#zero bss area
        la      a0, _bss_start
        la      a1, _bss_end
        beq     a0, a1, bsssk
bsslp:  sw      zero, 0(a0)
        addi    a0, a0, 4
        bne     a0, a1, bsslp
bsssk:

        la      t0, exit_fn
        sw      ra, 0(t0)
#enter main code
        j       main

exit:   la      ra, exit_fn
        lw      ra, 0(ra)
        jr      ra

        .section .bss, "w"
        .align 4
exit_fn:        .word 0

