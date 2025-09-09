.. _requirements:

============
Requirements
============

Basically, every modern controller which is able to drive a display is suitable to
run LVGL.  The minimal requirements are:

* 16, 32 or 64 bit microcontroller or processor
* > 16 MHz clock speed is recommended
* Flash/ROM: > 64 kB for the very essential components (> 180 kB is recommended)
* RAM:

    * Static RAM usage: ~2 kB depending on the used features and Widget types
    * stack: > 2kB (> 8 kB recommended)
    * Dynamic data (heap): > 2 KB (> 48 kB is recommended if using many GUI Widgets).
      Set by :c:macro:`LV_MEM_SIZE` in ``lv_conf.h``.
    * Display buffer:  > *"Horizontal resolution"* pixels (> 10 X *"Horizontal resolution"* is recommended)
    * One frame buffer in the MCU or in an external display controller

* C99 or newer compiler
* Basic C (or C++) knowledge:

  * `pointers <https://www.tutorialspoint.com/cprogramming/c_pointers.htm>`__.
  * `structs <https://www.tutorialspoint.com/cprogramming/c_structures.htm>`__.
  * `callbacks <https://www.geeksforgeeks.org/callbacks-in-c/>`__.

.. note::
    *Memory usage may vary depending on architecture, compiler and build options.*

