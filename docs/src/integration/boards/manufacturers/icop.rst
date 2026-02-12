====
ICOP
====

ICOP makes industrial computer hardware and the development tools for them.
ICOP offers an uncommon combination of hardware and software which distinguishes
them from other board vendors.

For a start, their products are not evaluation boards. They are ready-to-use
industrial PCs. They are a complete solution electrically and mechanically.

They are programmed with the Arduino IDE. You write your embedded application
logic and UI using the Arduino framework and its library ecosystem. Arduino is a
great abstraction to solve real problems when it's available for a platform.
ICOP is one such platform. You have access to all the popular open source
Arduino libraries. There is also support for installing Windows 11 on it.

Inside most of ICOP's panel PCs is an x86 processor. They are DM&P Vortex86
series chips. They execute 32-bit i586 machine code. This has interesting
implications, especially since the programming environment is as low-level as Arduino.
It's a rare case of an x86 CPU being used in a real-time setting.

Panel PCs
*********

These are PCs which are designed to be mounted in a panel. They are more
powerful than typical HMIs.

QEC-PPC-M-090T
--------------

This is an x86 panel PC which you can program using the Arduino IDE.
It has two cores. One core is dedicated to EtherCAT so the other is free
for application logic.

See the
`LVGL port repo <https://github.com/lvgl/lv_port_icop_qec_ppc_m_090t>`_
to get started creating a project for it with LVGL.

See the
`store page <https://www.icop.com.tw/product/QEC-PPC-M-090T>`_
for documentation and the ordering information.

Specs
~~~~~

CPU and Memory

- CPU: 533 MHz Vortex86EX2 32-bit x86 CPU
- RAM: 512MB/1GB DDR3
- Flash: 2GB SLC eMMC, 32MB SPI Flash

Display and Touch

- Resolution: 1024x600
- Display Size: 9"
- Interface: VGA
- Color Depth: 16-bit
- Technology: TFT
- DPI: 132 px/inch
- Touch Pad: Resistive

Connectivity

- Ethernet
- USB x3
- Headphone
