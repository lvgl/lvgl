.. _editor_integration_arduino:

========================================
Using the Exported C Code on Arduino IDE
========================================

Overview
********

This guide explains how to properly integrate your LVGL Editor exported project into 
an Arduino sketch, why it must go inside the src folder, and how to fix include paths 
for successful compilation.

Arduino Build System
--------------------

Arduino uses a special build system that automatically compiles:

- The main sketch file (``.ino``)
- All ``.c``, ``.cpp``, and ``.h`` files located in the same directory as the sketch or inside the ``src`` subfolder.

Any files placed outside these locations will not be compiled or linked automatically.

Because LVGL Editor exports projects with nested folders like:

- ``screens/``
- ``widgets/``
- ``components/``

Arduino would ignore those files unless they’re inside ``src/``.
That’s why placing your exported project under ``src/`` ensures everything is detected and compiled.

Example structure
-----------------

.. code-block:: text

    YourSketch/
    ├── YourSketch.ino
    └── src/
          └── editor_project/
              ├── editor_project.h
              ├── editor_project.c
              ├── screens/
              ├── widgets/
              └── components/

Include paths
-------------

LVGL source files typically use this include pattern:

.. code-block:: c

    #ifdef LV_LVGL_H_INCLUDE_SIMPLE
        #include "lvgl.h"
    #else
        #include "lvgl/lvgl.h"
    #endif

The ``LV_LVGL_H_INCLUDE_SIMPLE`` define controls how LVGL headers are included.
However, the Arduino IDE does not provide a way to add custom compiler symbols through its interface.
Instead, you must modify the ``platform.txt`` file for the specific board core you are using.

To ensure correct compilation, you need to instruct the Arduino build system to define ``LV_LVGL_H_INCLUDE_SIMPLE`` during compilation.

1. Locate the ``platform.txt`` file for your Arduino core.
    Example paths:

    - ESP32: ``hardware/esp32/esp32/``
    - AVR (Uno, Mega): ``hardware/arduino/avr/``

2. Open the file in a text editor.
3. Find the line starting with: ``build.extra_flags=``
4. Add the following flag to it: ``-DLV_LVGL_H_INCLUDE_SIMPLE``

The modified line should look similar to this:

.. code-block:: text

    build.extra_flags=-DARDUINO_HOST_OS="{runtime.os}" -DARDUINO_FQBN="{build.fqbn}" -DESP32=ESP32 -DCORE_DEBUG_LEVEL={build.code_debug} {build.loop_core} {build.event_core} {build.defines} {build.extra_flags.{build.mcu}} {build.zigbee_mode} -DLV_LVGL_H_INCLUDE_SIMPLE

Relative paths
--------------

Since the LVGL Editor exports files in multiple nested directories, files like ``main_gen.c`` or ``widget_button.c`` need relative paths to find the main header file.

Change:

.. code-block:: c

    #include "editor_project.h"

to:

.. code-block:: c

    #include "../editor_project.h"

(or adjust accordingly depending on how deep the file is nested).

Without this, Arduino won’t locate the header file and will throw "No such file or directory" errors.

Step-by-Step Setup
------------------

1. Create the ``src`` Folder inside your sketch folder

.. code-block:: text

    YourSketch/
    ├── YourSketch.ino
    └── src/

2. Copy or create the LVGL Editor Project inside ``src`` folder

3. Update Includes

Edit files to use:

.. code-block:: c

    #include "lvgl.h"

and ensure the main header file i.e., ``editor_project.h`` is included relatively:

.. code-block:: c

    #include "../editor_project.h"

4. Reapply Changes After Regeneration

Each time you re-export your project from the LVGL Editor, these files are regenerated, so repeat Step 3.

Example Arduino sketch
----------------------

.. code-block:: cpp

    #include <lvgl.h>
    #include "src/editor_project/editor_project.h"  // Adjust to match your project name

    void my_disp_init();
    void my_touch_init();

    void setup() {
      Serial.begin(115200);
      lv_init();          // Initialize LVGL
      my_disp_init();     // Setup display driver
      my_touch_init();    // Setup input driver

      // Parameter to editor_project_init() is the asset path.
      // Provide the base directory where LVGL should look for asset files (fonts & images).
      // If you are not loading assets from a filesystem, pass an empty string "".
      //
      // Example folder structure:
      //   /editor_project/fonts/font.ttf
      //   /editor_project/images/image.bin
      //
      // LVGL uses a drive letter prefix based on what you configure (e.g. "C", "D", etc.).
      // If you configure the drive letter as "C", the path would be:
      //   C:/editor_project/
      //
      // Example:
      // editor_project_init("C:/editor_project/");
      editor_project_init("");       // Load LVGL Editor UI
      lv_screen_load(main_create()); // Load the main screen
    }

    void loop() {
      lv_timer_handler(); // Handle LVGL tasks
      delay(5);
    }
