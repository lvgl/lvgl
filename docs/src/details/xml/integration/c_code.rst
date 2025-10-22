.. _editor_integration_c:

=========================
Using the Exported C Code
=========================

Overview
********

When C code is exported from LVGL's UI Editor, no XML is needed at all
(``LV_USE_XML`` can be ``0``), and it's very similar to the case when the C code is
written by hand.

The exported C code can simply be dropped into the project and compiled into the
application.

Initialization
**************

The main entry point is a ``<project_name>_init("asset_path")`` function located
in ``<project_name>.c``.

``<project_name>`` is the name of the parent folder by default, which can be overwritten
in ``project.xml`` as ``<project name="my_project">``.


Its only parameter is a path where the file-based assets (fonts and images) are
located. For example, if a PNG and TTF file are located at
``"A:my_ui/v2.3/assets"``, it should be passed to ``ui_init()`` so all the used
paths can be prefixed. That is, if ``images/logo.png`` was used in the UI Editor,
then the full path will be ``"A:my_ui/v2.3/assets/images/logo.png"``.

Note that "init" function creates only the :ref:`permanent screens <xml_screen_permanent>` but
doesn't load any screens. However, this can be done easily by calling ``lv_screen_load(screen1)``.

Screens can also be created easily by calling their generated create function,
e.g. ``main_screen_create()``.

Structure of the Exported Code
******************************

Code export follows some conventions.

The generated files end with ``_gen.c`` or ``_gen.h``. These shouldn't be modified
as they will be overwritten on the next code generation.

For the non-generated files, only a skeleton is generated once (if the file doesn't
exist). Here, the user can add custom code as needed.

From the XMLs of screens and components, "create" functions are generated using
the name of the file. For example:

- ``lv_obj_t * main_screen_create(void)`` from ``main_screen.xml``
- ``lv_obj_t * my_button_create(lv_obj_t * parent, const char * button_text)``
  from ``my_button.xml``

For Components, no setters are created, but all the parameters are passed to the
create function.

From ``globals.xml``, four files are created:

- Generated C and H files containing all subject, font, image, style, and other
  initializations.
- Non-generated C and H files that are just wrappers around the generated ones
  where custom code can be placed.

The name of these files is the name of the folder containing the ``globals.xml`` file by
default. It can be overridden by ``<globals name="my_lib">``.



Adding Custom Code
******************

In some cases, it might be necessary to extend the generated code with hand-written
code. Let's see how it can be done in different cases.

Adding Generic Code
-------------------

:ref:`lv_timer <timer>`, a subject with a special observer, complex animations, or
styles can be added in ``ui.c`` or the C file created from ``globals.xml``.

For example, to add some style properties to an XML-defined style:

.. code-block:: c

    void ui_init(const char * asset_path)
    {
        LV_LOG_USER("ui_init()\n");
        ui_lib_init(asset_path);

        lv_style_set_bg_color(&style_1, get_special_color());
    }

To create a special subject which formats an integer temperature value as a string
with a decimal point:

.. code-block:: c

    /* Assume that subject_temperature_int stores the value with 0.1 degree resolution */
    /* E.g. 345 means 34.5 */

    void temperature_to_string_observer_cb(lv_observer_t * observer,
                                           lv_subject_t * subject)
    {
        int32_t upscaled_value = lv_subject_get_int(&subject_temperature_int);

        int32_t int_part = upscaled_value / 10;
        int32_t fract_part = upscaled_value % 10;

        char buf[64];
        lv_snprintf(buf, sizeof(buf), "%d.%d", int_part, fract_part);
        lv_subject_copy_string(&subject_temperature_string, buf);
    }

    lv_subject_add_observer(&subject_temperature_int,
                            temperature_to_string_observer_cb, NULL);

Component wrapper
-----------------

If a component needs to be extended with a feature or API that is not supported by
the editor, the simplest solution is to create a wrapper component.

For example, let's say ``my_button.xml`` describes some feature of a button, but
some setters, special APIs, or extra features need to be added. In this case, just
manually create new C and H files for a ``super_button`` with a function like:

.. code-block:: c

    lv_obj_t * super_button_create(lv_obj_t * parent, ...some_arguments...)
    {
        lv_obj_t * my_button = my_button_create(parent, ...);

        /* ...do something with the created `my_button` */

        return my_button;
    }



CMake Integration
*****************

A skeleton ``CMakeLists.txt`` file is generated which can be easily integrated into
any project.

A file called ``file_list_gen.cmake`` is generated and used by ``CMakeLists.txt``.
It ensures that only the appropriate files are included.

Arduino Integration
*******************

This guide explains how to properly integrate your LVGL Editor exported project into 
an Arduino sketch, why it must go inside the src folder, and how to fix include paths 
for successful compilation.

Arduino Build system
--------------------

Arduino uses a special build system that automatically compiles:
The main sketch file (.ino), and
All ``.c``, ``.cpp``, and ``.h`` files located in the same directory as the sketch or inside the ``src`` subfolder.
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

and ensure the main header file ie. ``editor_project.h`` is included relatively:

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
  
