.. _display_endianness:

====================
Swapping Endian-ness
====================

In case of RGB565 color format it might be required to swap the 2 bytes
because the SPI, I2C or 8 bit parallel port periphery sends them in the wrong order.

The ideal solution is configure the hardware to handle the 16 bit data with different byte order,
however if this is not possible :cpp:expr:`lv_draw_sw_rgb565_swap(buf, buf_size_in_px)`
can be called in the :ref:`flush_callback` to swap the bytes.

If you wish you can also write your own function, or use assembly instructions for
the fastest possible byte swapping.

Note that this is not about swapping the Red and Blue channel but converting

``RRRRR GGG | GGG BBBBB``

to

``GGG BBBBB | RRRRR GGG``.
