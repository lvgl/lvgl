/* QEMU -M virt, Cortex-A53. RAM starts at 0x40000000; the first megabyte is left to
 * QEMU's own loader area, and the machine is started with -m 512M. */
MEMORY {
    RAM (rwx) : ORIGIN = 0x40100000, LENGTH = 448M
}
ENTRY(_start)
SECTIONS {
    .text : { KEEP(*(.text.init)) *(.text*) *(.rodata*) } > RAM

    . = ALIGN(16);
    .data : { *(.data*) } > RAM

    .bss (NOLOAD) : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(16); _ebss = .; } > RAM

    . = ALIGN(4096);
    /* picolibc's malloc takes its heap from __heap_start/__heap_end */
    PROVIDE(__heap_start = .);
    PROVIDE(__heap_end = ORIGIN(RAM) + LENGTH(RAM) - 0x100000);

    . = ORIGIN(RAM) + LENGTH(RAM) - 16;
    PROVIDE(_estack = .);
    PROVIDE(__stack = .);
}
