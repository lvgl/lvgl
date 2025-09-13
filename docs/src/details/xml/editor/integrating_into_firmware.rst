.. include:: custom_tools.txt

.. _integrating an editor project into firmware:

===========================================
Integrating an Editor Project into Firmware
===========================================

Prerequisites
*************

- Configure application so it compiles and runs successfully (without the UI).
- Add the Editor project folder alongside the project, or within it, e.g. as a Git
  submodule.  If the Editor project uses any pure "Component Projects", these
  directories can also be included (not combined, but kept separate and referred to
  from within the Editor Project's ``project.xml`` file).
- In ``lv_conf.h``, ensure ``LV_USE_OBJ_NAME`` is set to a non-zero value since this
  is needed by Editor projects.
- In ``lv_conf.h``, if your firmware project needs to use XML files at run time,
  ensure ``LV_USE_XML`` is set to a non-zero value to include XML-parsing and related
  logic with the application.




Using Exported C Code
*********************

If any of the project's generated ``.c`` files are to be used in the firmware project,
ensure they are added to the projects list of files to be compiled.  Ensure the
Editor project root directory (containing the ``ui.h`` file) is added to the list
of "Additional Include Directories" for the project since many generated ``.c`` files
include `ui.h` with no path included.

If the UI is going to be used entirely from its generated ``.c`` and ``.h`` files,
you can do so by calling ``ui_init(...)`` (exported from ``ui.c`` in Editor
project, passing the path to the directory containing the ``project.xml`` file
(typically the root directory of the Editor project).  This calls all the relevant
initialization and registration functions, followed by
``lv_screen_load(screen_name_create ())``, which builds the necessary
Widget/Component tree LVGL needs for your interactive UI.



Using XML Files
***************

For components that will be used via XML files, each will need to be registered with
the relevant ``lv_xml_xxxxxx_register()`` function and later created with
``lv_xml_create()``.  ``xxxxxx`` is the type of thing being registered
(e.g. component, style, test, translation, widget).  Note that the generated ``.c``
files related to Widgets must *also* be part of the compiled code.  For fonts,
images, events and subjects, the relevant ``lv_xml_register_yyyyyy()`` function
must be called where ``yyyyyy`` is font, image, event_cb and subject respectively.
These registration functions give the "thing" a name that it can be referred to
thereafter (e.g. in calling ``lv_xml_create()`` and ``lv_obj_find_by_name()``)

For each XML component to be included in a project, register its name with the
application by making a call to one of the following:

.. code-block:: c

    /* If component is in an XML file... */
    lv_xml_component_register_from_file("A:path/to/my_button.xml");

    /* Or if component is in a C array... */
    lv_xml_component_register_from_data("my_button", xml_data_of_my_button);

After this is done, each UI element (Component/Widget/Screen) can be created by

.. code-block:: c

    /* Create UI element previously registered. */
    lv_xml_create(parent, my_component_name, attrs);

where ``attrs`` is an string containing the desired list of attributes in XML syntax.
Example:

.. code-block:: c

    /* Create UI element previously registered. */
    lv_xml_create(parent, my_component_name, "value=\"25\"");

.. todo:: verify or correct above syntax for ``attrs`` argument.  This should
          be well documented in the function's documentation as well.



Combining C and XML
*******************

.. todo::  fill in



VSCode Simulator
****************

The LVGL `VSCode Simulator`_ uses SDL to run LVGL UIs on the PC (Linux, Windows and
Mac), without any real embedded hardware.

- First configure the simulator so it can run the default LVGL application.

- Add the Editor project folder alongside the main folder of the simulator project.
  This can be the folder itself, a symlink or as git submodule.

- In the CMakeLists.txt of the Editor add the following after
  ``add_library(lib-ui ${PROJECT_SOURCES})``:

  .. code-block:: cmake

    target_compile_definitions(lib-ui PUBLIC LV_LVGL_H_INCLUDE_SIMPLE)
    target_include_directories(lib-ui PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/lvgl
    )

- In the root ``CMakeLists.txt`` of the simulator project just add:

  .. code-block:: cmake

    add_subdirectory(battery_indicator)
    target_link_libraries(main lib-ui)

- In the ``main.c`` file of the simulator project add the UI header file:

  .. code-block:: c

    #include "examples/ui.h"

- In ``main()`` function, after `lv_init()` and display/input/tick initialization,
  call the init function of the UI:

  .. code-block:: c

    int main(int argc, char **argv)
    {
        lv_init();
        hal_init(410, 502);
        ui_init(NULL);
        while(1) {
            lv_timer_handler();
            usleep(5 * 1000);
        }
        return 0;
    }

- Compile and run the application.  You should see a screen open with the UI.



ESP-IDF
*******

- Add the Editor project folder inside ``components`` folder of the ESP-IDF project.
  This can be the folder itself, a symlink or as a git submodule.

- Remove the contents of the ``CMakeLists.txt`` of the UI project and add something
  like the following:

  .. code-block:: cmake

    include(${CMAKE_CURRENT_LIST_DIR}/file_list_gen.cmake)

    idf_component_register(
        SRCS ${PROJECT_SOURCES}
        INCLUDE_DIRS "."
        REQUIRES lvgl
    )

    target_compile_definitions(${COMPONENT_LIB} PUBLIC
        LV_LVGL_H_INCLUDE_SIMPLE
        LV_CONF_INCLUDE_SIMPLE
    )

  This will tell IDF build system to consider the Editor UI project as an IDF component.

- In the ``CMakeLists.txt`` inside the main folder of the IDF register the component.
  For example:

  .. code-block:: cmake

    idf_component_register(
        SRCS main.c
        INCLUDE_DIRS .
        REQUIRES examples)

- In ``main.c`` add the UI include header

  .. code-block:: c

    #include "ui.h"

- After display and LVGL initialization, call the init function for the UI.
  For example:

  .. code-block:: c

    void app_main(void)
    {
        bsp_display_start();

        bsp_display_lock(0);

        ui_init(NULL);

        bsp_display_unlock();
    }

- Finally compile and flash the application to the embedded hardware.