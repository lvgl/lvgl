.. _editor_integration_arduino:

========================================
Using the Exported C Code on Arduino IDE
========================================

Overview
********

This guide explains how to properly integrate your LVGL Editor exported project into 
an Arduino sketch, why it must go inside the src folder, and how to fix include paths 
for successful compilation.

Arduino Build system
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


The ``LV_LVGL_H_INCLUDE_SIMPLE`` define allows flexibility in how LVGL is included.
However, Arduino’s build system doesn’t provide a way to set this define globally across all source files.
As a result, the compiler falls back to:

.. code-block:: c

    #include "lvgl/lvgl.h"

which fails, because Arduino installs the LVGL library headers directly (e.g. ``libraries/lvgl/src/lvgl.h``), not in a nested ``lvgl/`` folder.

To fix this, replace all occurrences of:

.. code-block:: c

    #include "lvgl/lvgl.h"

with:

.. code-block:: c

    #include "lvgl.h"

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
      editor_project_init("");          // Load LVGL Editor UI
      lv_screen_load(main_create()); // load the main screen
    }

    void loop() {
      lv_timer_handler(); // Handle LVGL tasks
      delay(5);
    }
