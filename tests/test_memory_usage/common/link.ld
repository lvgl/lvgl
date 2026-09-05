/* Shared by every target of the memory-usage tests.
 *
 * `.literal` is Xtensa's constant pool and has to be placed with the code it belongs to;
 * `.srodata`/`.sdata`/`.sbss` are the RISC-V small-data sections
 */
MEMORY
{
  FLASH (rx) : ORIGIN = 0x10000000, LENGTH = 16M
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 16M
}

SECTIONS
{
  .text : { *(.literal* .text*) *(.rodata* .srodata*) } > FLASH
  .data : { *(.data* .sdata*) } > RAM AT> FLASH
  .bss  : { *(.bss* .sbss*) *(COMMON) } > RAM
}
