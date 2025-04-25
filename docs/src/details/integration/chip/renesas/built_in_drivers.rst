================
Built-in Drivers
================

`Renesas <https://renesas.com/>`__ is an official partner of LVGL. Therefore, LVGL contains built-in support for
`Dave2D <https://www.renesas.com/document/mas/tes-dave2d-driver-documentation>`__ (the GPU of Renesas) and we also maintain
ready-to-use Renesas projects.


Dave2D
******

Dave2D is capable of accelerating most of the drawing operations of LVGL:

- Rectangle drawing, even with gradients
- Image drawing, scaling, and rotation
- Letter drawing
- Triangle drawing
- Line drawing


As Dave2D works in the background, the CPU is free for other tasks. In practice, during rendering, Dave2D can reduce the CPU usage by
half or to one-third, depending on the application.


GLCDC
*****

GLCDC is a multi-stage graphics output peripheral available in several Renesas MCUs. It is able to drive LCD panels via a highly
configurable RGB interface.

More info can be found at the :ref:`driver's page<renesas_glcdc>`.


MPU Drivers
***********

Renesas MPUs (i.e., the RZ/G family) can use LVGL MPU drivers.

Wayland
-------

Supported on boards with a Wayland desktop AKA a Wayland compositor.

fbdev
-----

Almost always available.
