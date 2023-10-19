=====
CMake
=====

LVGL supports integrating with `CMake <https://cmake.org/>`__. It comes
with preconfigured targets for:

- `Espressif (ESP32) <https://docs.espressif.com/projects/esp-idf/en/v3.3/get-started-cmake/index.html>`__
- `MicroPython <https://docs.micropython.org/en/v1.15/develop/cmodules.html>`__
- `Zephyr <https://docs.zephyrproject.org/latest/guides/zephyr_cmake_package.html>`__

On top of the preconfigured targets you can also use "plain" CMake to
integrate LVGL into any custom C/C++ project.


Prerequisites
*************

*  CMake ( >= 3.12.4 )
*  Compatible build tool e.g.
   *  `Make <https://www.gnu.org/software/make/>`__
   *  `Ninja <https://ninja-build.org/>`__


Building LVGL with CMake
************************

There are many ways to include external CMake projects into your own. A
modern one also used in this example is the CMake `FetchContent <https://cmake.org/cmake/help/latest/module/FetchContent.html>`__
module. This module conveniently allows us to download dependencies
directly at configure time from e.g. `GitHub <https://github.com/>`__.
Here is an example how we might include LVGL into our own project.

.. code:: cmake

   cmake_minimum_required(VERSION 3.14)
   include(FetchContent)

   project(MyProject LANGUAGES C CXX)

   # Build an executable called "MyFirmware"
   add_executable(MyFirmware src/main.c)

   # Specify path to own LVGL config header
   set(LV_CONF_PATH
       ${CMAKE_CURRENT_SOURCE_DIR}/src/lv_conf.h
       CACHE STRING "" FORCE)

   # Fetch LVGL from GitHub
   FetchContent_Declare(lvgl GIT_REPOSITORY https://github.com/lvgl/lvgl.git)
   FetchContent_MakeAvailable(lvgl)

   # The target "MyFirmware" depends on LVGL
   target_link_libraries(MyFirmware PRIVATE lvgl::lvgl)

This configuration declares a dependency between the two targets
**MyFirmware** and **lvgl**. Upon building the target **MyFirmware**
this dependency will be resolved and **lvgl** will be built and linked
with it. Since LVGL requires a config header called `lv_conf.h <https://github.com/lvgl/lvgl/blob/master/lv_conf_template.h>`__
to be includable by its sources we also set the option :c:macro:`LV_CONF_PATH`
to point to our own copy of it.


Additional CMake options
========================

Besides :c:macro:`LV_CONF_PATH` there are few additional CMake options available.


Include paths options
---------------------

-  :c:macro:`LV_LVGL_H_INCLUDE_SIMPLE`: which specifies whether to ``#include "lvgl.h"`` absolute or relative

   ============ ==============
   ON (default) OFF
   ============ ==============
   "lvgl.h"     "../../lvgl.h"
   ============ ==============

-  :c:macro:`LV_CONF_INCLUDE_SIMPLE`: which specifies whether to ``#include "lv_conf.h"`` and ``"lv_drv_conf.h"`` absolute or relative

   =============== =====================
   ON (default)    OFF
   =============== =====================
   "lv_conf.h"     "../../lv_conf.h"
   "lv_drv_conf.h" "../../lv_drv_conf.h"
   =============== =====================

..

   We do not recommend disabling those options unless your folder layout
   makes it absolutely necessary.


Examples/demos options
----------------------

| LVGL `examples <https://docs.lvgl.io/master/examples.html>`__ and
  `demos <https://github.com/lvgl/lvgl/demos>`__ are built by default in
  the main CMake file.
| To disable their built, use:

-  :c:macro:`LV_CONF_BUILD_DISABLE_EXAMPLES`: Set to ``1`` to disable *examples* build
-  :c:macro:`LV_CONF_BUILD_DISABLE_DEMOS`: Set to ``1`` to disable *demos* build


Building LVGL drivers
*********************

To build `LVGL drivers <https://github.com/lvgl/lv_drivers>`__, you can use:

.. code:: cmake

   FetchContent_Declare(lv_drivers
                        GIT_REPOSITORY https://github.com/lvgl/lv_drivers)
   FetchContent_MakeAvailable(lv_drivers)

   # The target "MyFirmware" depends on LVGL and drivers
   target_link_libraries(MyFirmware PRIVATE lvgl::lvgl lvgl::drivers)


Build shared libraries with CMake
*********************************

By default, LVGL will be built as a static library (archive). CMake can
instead be instructed to build LVGL as shared library (.so/.dll/etc.):

.. code:: cmake

   set(BUILD_SHARED_LIBS ON)

OR

.. code:: console

   $ cmake "-DBUILD_SHARED_LIBS=ON" .
