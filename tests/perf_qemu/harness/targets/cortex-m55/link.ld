/* mps3-an547 (Cortex-M55).
 *
 * The reset vector has to be where the CPU looks for it, so .vectors goes in the ITCM
 * at address 0 and everything else goes in the 2 GB DDR window at 0x60000000. The ITCM
 * is only 512 KB and LVGL's fonts and demo images do not fit there.
 *
 * QEMU models no cache and no wait states, so where the data lives cannot change the
 * instruction count. Only the reset vector's address actually matters here.
 *
 * .data has no separate load address, so QEMU's ELF loader puts it straight at its final
 * address and the startup file has nothing to copy.
 */
MEMORY {
    ITCM (rx)  : ORIGIN = 0x00000000, LENGTH = 512K
    DTCM (rw)  : ORIGIN = 0x20000000, LENGTH = 512K
    DDR  (rwx) : ORIGIN = 0x60000000, LENGTH = 512M
}
ENTRY(Reset_Handler)
SECTIONS {
    .vectors : { KEEP(*(.vectors)) } > ITCM

    .text : { *(.text*) *(.rodata*) } > DDR
    .ARM.extab : { *(.ARM.extab*) } > DDR
    .ARM.exidx : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } > DDR

    .data : { *(.data*) } > DDR
    .bss (NOLOAD) : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(8); _ebss = .; } > DDR

    . = ALIGN(16);
    _heap_start = .;
    _heap_end = ORIGIN(DDR) + LENGTH(DDR);

    /* The stack gets the DTCM to itself, so a stack overflow lands outside any
     * mapped memory instead of quietly eating the heap. */
    _estack = ORIGIN(DTCM) + LENGTH(DTCM);
}
