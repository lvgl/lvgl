MEMORY
{
  FLASH (rx) : ORIGIN = 0x08000000, LENGTH = 320K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 320K
}

SECTIONS
{
  .text : { *(.text*) *(.rodata*) } > FLASH
  .data : { *(.data*) } > RAM AT> FLASH
  .bss  : { *(.bss*) *(COMMON) } > RAM
}
