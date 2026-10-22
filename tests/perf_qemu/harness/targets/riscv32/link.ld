/* QEMU -M virt, rv32imac. RAM starts at 0x80000000 and the machine is started with
 * -m 512M, so 448 MB is safely inside it. LVGL's built-in allocator alone is a 32 MB
 * .bss array and the 800x480 draw buffer is another 1.6 MB from the C library's heap. */
MEMORY {
    RAM (rwx) : ORIGIN = 0x80000000, LENGTH = 448M
}
ENTRY(_start)
SECTIONS {
    .text : { KEEP(*(.text.init)) *(.text*) *(.rodata*) } > RAM

    . = ALIGN(8);
    .data : {
        *(.data*)
        . = ALIGN(8);
        PROVIDE(__global_pointer$ = . + 0x800);
        *(.sdata*)
    } > RAM

    /* .sbss has to sit next to .sdata, inside the 2 KB window gp can reach. Left at the
     * end of .bss it lands past the whole LVGL heap, megabytes away, and the linker's
     * gp relaxation silently stops applying to every small global: the startup file's
     * `la gp` is then wasted and the counts are higher than a normal RISC-V build's. */
    .sbss (NOLOAD) : { _sbss = .; *(.sbss*) *(.sbss.*) *(.scommon) } > RAM

    .bss (NOLOAD) : { *(.bss*) *(COMMON) . = ALIGN(8); _ebss = .; } > RAM

    . = ALIGN(16);
    /* picolibc's malloc takes its heap from __heap_start/__heap_end */
    PROVIDE(__heap_start = .);
    PROVIDE(__heap_end = ORIGIN(RAM) + LENGTH(RAM) - 0x100000);

    . = ORIGIN(RAM) + LENGTH(RAM) - 16;
    PROVIDE(_estack = .);
    PROVIDE(__stack = .);
}
