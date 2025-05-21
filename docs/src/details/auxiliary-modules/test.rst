.. _test:

==========
UI Testing
==========

Overview
********

The Test module provides functions to emulate clicks, key presses, encoder turns, time passing, and
compare the UI with reference images.

These functions can be easily used in any test framework (such as Unity, GoogleTest, etc.), and
assertions can be performed to check if, for example:

- A widget's value is different from the expected value after emulating user inputs.
- The values are incorrect after some time has passed.
- The screen's content is different from the reference image.
- Some event functions are not triggered.
- Etc.

Note that it is assumed the tests are performed on a desktop or server environment,
where there are no memory constraints.

Usage
*****

The Test module can be enabled by configuring ``LV_USE_TEST`` to a non-zero value,
and it consists of the following components:

- Helpers
- Display emulation
- Input device emulation
- Screenshot comparison

Helpers
-------

Time
~~~~

To emulate elapsed time, two functions can be used:

1. :cpp:expr:`lv_test_wait(ms)`: Emulates that ``ms`` milliseconds have elapsed, but it also calls ``lv_timer_handler`` after each millisecond.
   This is useful to check if events (e.g., long press, long press repeat) and timers were triggered correctly over time.
2. :cpp:expr:`lv_test_fast_forward(ms)`: Jumps ``ms`` milliseconds ahead and calls ``lv_timer_handler`` only once at the end.

:cpp:expr:`lv_refr_now(NULL)` is called at the end of both functions to ensure that animations and
widget coordinates are recalculated.

:cpp:expr:`lv_refr_now(NULL)` can also be called manually to force LVGL to refresh the emulated display.

Memory Usage
~~~~~~~~~~~~

If ``LV_USE_STDLIB_MALLOC`` is set to ``LV_STDLIB_BUILTIN``, memory usage and memory leaks can be monitored.

.. code-block:: c

    size_t mem1 = lv_test_get_free_mem();
    <create and delete items>
    size_t mem2 = lv_test_get_free_mem();
    if(mem1 != mem2) fail();

It might make sense to create and delete items in a loop many times and add a small tolerance
to the memory leakage test. This might be needed due to potential memory fragmentation. Empirically,
a tolerance of 32 bytes is recommended.

.. code-block:: c

    if(LV_ABS((int64_t)mem2 - (int64_t)mem1) > 32) fail();

Display Emulation
-----------------

By calling :cpp:expr:`lv_test_display_create(hor_res, ver_res)`, a dummy display can be created.

It functions like any other normal display, but its content exists only in memory.

When creating this display, the horizontal and vertical resolutions must be passed. Internally,
a framebuffer will be allocated for this size, and ``XRGB8888`` color format will be set.

The resolution and color format can be changed at any time by calling :cpp:func:`lv_display_set_resolution` and
:cpp:func:`lv_display_set_color_format`.

Input Device Emulation
----------------------

By calling :cpp:func:`lv_test_indev_create_all`, three test input devices will be created:

1. A pointer (for touch or mouse)
2. A keypad
3. An encoder

For example, this is how a scroll gesture can be emulated:

.. code-block:: c

    lv_test_mouse_move_to(20, 30);
    lv_test_mouse_press();
    lv_test_wait(20);
    lv_test_mouse_move_by(0, 100);
    lv_test_wait(20);
    lv_test_mouse_release();
    lv_test_wait(20);

It is recommended to add :cpp:func:`lv_test_wait` after user actions to ensure that
the new state and coordinates are read and applied from the input device.

After that, the user can check if the given widget was really scrolled
by getting the Y coordinate of a child.

.. code-block:: c

    int32_t y_start = lv_obj_get_y(child);
    <scroll emulation>
    int32_t y_end = lv_obj_get_y(child);
    if(y_start + 100 != y_end) fail();

Please refer to :ref:`lv_test_indev_h` for the list of supported input device emulation functions.

Screenshot Comparison
---------------------

``bool lv_test_screenshot_compare(const char * fn_ref)`` is a useful function
to compare the content of the emulated display with reference PNG images.


This function works in a practical way:

- If the folder(s) referenced in ``fn_ref`` do not exist, they will be created automatically.
- If the reference image is not found, it will be created automatically from the rendered screen.
- If the comparison fails, an ``<image_name>_err.png`` file will be created with the rendered content next to the reference image.
- If the comparison fails, the X and Y coordinates of the first divergent pixel, along with the actual and expected colors, will also be printed.

The reference PNG images should have a **32-bit color format** and match the display size.

The test display's content will be converted to ``XRGB8888`` to simplify comparison with the reference images.
The conversion is supported from the following formats (i.e., the test display should have a color
format in this list):

- :cpp:enumerator:`LV_COLOR_FORMAT_XRGB8888`
- :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888`
- :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB888`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565`
- :cpp:enumerator:`LV_COLOR_FORMAT_RGB565_SWAPPED`
- :cpp:enumerator:`LV_COLOR_FORMAT_L8`
- :cpp:enumerator:`LV_COLOR_FORMAT_AL88`
- :cpp:enumerator:`LV_COLOR_FORMAT_I1`

To read and decode PNG images and to store the converted rendered image, a few MBs of RAM are dynamically allocated using the standard ``malloc``
(not :cpp:expr:`lv_malloc`).


The screenshot comparison uses `lodepng` which is built-in to LVGL and just needs to be enabled with
``LV_USE_LODEPNG``.

To avoid making the entire Test module dependent on `lodepng`, screenshot comparison can be individually enabled by
``LV_USE_TEST_SCREENSHOT_COMPARE``.

API
***
