.. _building_lvgl:

=============
Building LVGL
=============


Make and CMake
**************

LVGL also supports ``make`` and ``CMake`` build systems out of the box.
To add LVGL to your Makefile based build system add these lines to your
main Makefile:

.. code-block:: make

    LVGL_DIR_NAME ?= lvgl     #The name of the lvgl folder (change this if you have renamed it)
    LVGL_DIR ?= ${shell pwd}  #The path where the lvgl folder is
    include $(LVGL_DIR)/$(LVGL_DIR_NAME)/lvgl.mk

For integration with CMake take a look this section of the
:ref:`Documentation <build_cmake>`.


Managed builds
**************
TODO

