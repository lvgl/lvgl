/* mps2-an500 (Cortex-M7). Two 4 MB SRAMs and nothing else: ssram1 at 0 and ssram23 at
 * 0x20000000. The 16 MB "PSRAM" window at 0x21000000 that `info mtree` shows is an
 * unimplemented device stub, not memory, so writes there vanish silently.
 *
 * That is the tightest target here, and it is what sizes LV_MEM_SIZE and
 * LV_CACHE_DEF_SIZE in configs/perf_qemu.defconfig for every target, because the numbers
 * are only comparable while the allocator behaves the same way everywhere.
 *
 * The split: code and constants take the first half of ssram1, and the C library's heap
 * takes the second half, because lv_test_display_create() malloc()s the 1.6 MB display
 * buffer from there. LVGL's own heap is a .bss array, so it and the stack get ssram23 to
 * themselves. */
MEMORY {
    FLASH   (rx)  : ORIGIN = 0x00000000, LENGTH = 2M
    CLIBMEM (rw)  : ORIGIN = 0x00200000, LENGTH = 2M
    RAM     (rwx) : ORIGIN = 0x20000000, LENGTH = 4M
}
ENTRY(Reset_Handler)
SECTIONS {
    .vectors : { KEEP(*(.vectors)) } > FLASH
    .text : { *(.text*) *(.rodata*) } > FLASH
    .ARM.extab : { *(.ARM.extab*) } > FLASH
    .ARM.exidx : { __exidx_start = .; *(.ARM.exidx*) __exidx_end = .; } > FLASH

    /* The C library's heap is its own region, so a display buffer that no longer fits
     * fails in _sbrk() rather than by overwriting LVGL's heap. */
    .clibheap (NOLOAD) : {
        _heap_start = .;
        . = ORIGIN(CLIBMEM) + LENGTH(CLIBMEM);
        _heap_end = .;
    } > CLIBMEM

    .data : { *(.data*) } > RAM
    .bss (NOLOAD) : { _sbss = .; *(.bss*) *(COMMON) . = ALIGN(8); _ebss = .; } > RAM

    . = ALIGN(8);
    _estack = ORIGIN(RAM) + LENGTH(RAM);
}
