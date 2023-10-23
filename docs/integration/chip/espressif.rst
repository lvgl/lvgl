=============================
Espressif (ESP32 chip series)
=============================

LVGL can be used and configured as a standard `ESP-IDF <https://github.com/espressif/esp-idf>`__ component.

More information about ESP-IDF build system can be found `here <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/build-system.html>`__.


LVGL demo project for ESP32
---------------------------

We've created `lv_port_esp32 <https://github.com/lvgl/lv_port_esp32>`__,
a project using ESP-IDF and LVGL to show one of the demos from
`demos <https://github.com/lvgl/lvgl/demos>`__. You can configure the
project to use one of the many supported display controllers and targets
(chips).

See `lvgl_esp32_drivers <https://github.com/lvgl/lvgl_esp32_drivers>`__
repository for a complete list of supported display and indev (touch)
controllers and targets.


Using LVGL in your ESP-IDF project
----------------------------------

Prerequisites
~~~~~~~~~~~~~

-  ESP-IDF v4.1 and above
-  ESP evaluation board with a display


Obtaining LVGL
~~~~~~~~~~~~~~

**Option 1:** git submodule

Simply clone LVGL into your ``project_root/components`` directory and it
will be automatically integrated into the project. If the project is a
git repository you can include LVGL as a git submodule:

.. code:: sh

   git submodule add https://github.com/lvgl/lvgl.git components/lvgl

The above command will clone LVGL's main repository into the
``components/lvgl`` directory. LVGL includes a ``CMakeLists.txt`` file
that sets some configuration options so you can use LVGL right away.

**Option 2:** IDF Component Manager

LVGL is also distributed through `IDF Component Manager <https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/tools/idf-component-manager.html>`__.
It allows users to seamlessly integrate `LVGL component <https://components.espressif.com/component/lvgl/lvgl>`__ into
their project with following command:

.. code:: sh

   idf.py add-dependency lvgl/lvgl>=8.*

During next project build, LVGL component will be fetched from the
component registry and added to project build.


Configuration
~~~~~~~~~~~~~

When you are ready to configure LVGL, launch the configuration menu with
``idf.py menuconfig`` in your project root directory, go to
``Component config`` and then ``LVGL configuration``.


Using lvgl_esp32_drivers in ESP-IDF project
-------------------------------------------

You can also add ``lvgl_esp32_drivers`` as a "component". This component
should be located inside a directory named "components" in your project
root directory.

When your project is a git repository you can include
``lvgl_esp32_drivers`` as a git submodule:

.. code:: sh

   git submodule add https://github.com/lvgl/lvgl_esp32_drivers.git components/lvgl_esp32_drivers
