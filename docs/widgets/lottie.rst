.. _lv_lottie:

==================
Lottie (lv_lottie)
==================

Overview
********

The Lottie widget is capable of parsing, rasterizing, and playing `Lottie animations <https://lottiefiles.com>`__.

The Lottie animations are vector based animation. Think of it as the modern combination of SVG and GIF.

The animations can be downloaded from various sources, such as `https://lottiefiles.com/ <https://lottiefiles.com/>`__
or you can create your own animations using for example Adobe After Effects.

The Lottie widget is based on :ref:`lv_canvas` because in order to render the animation
the user needs to provide a buffer where the current frame is stored.

.. _lv_lottie_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the Lottie animation. The typical background style properties apply but usually it is left transparent.

.. _lv_lottie_usage:

Usage
*****

Dependencies
------------

The Lottie widget uses the `ThorVG <https://github.com/thorvg/thorvg>`__ library which is `integrated into LVGL <https://github.com/lvgl/lvgl/tree/master/src/libs/thorvg>`__.
In order to use Lottie animations ``LV_USE_THORVG_INTERNAL`` (to use the built-in ThorVG) or
``LV_USE_THORVG_EXTERNAL`` (to link it externally) needs to enabled. For vector graphics in general
``LV_USE_VECTOR_GRAPHIC`` also needs to be enabled.

As ThorVG is written in C++, when using ``LV_USE_THORVG_INTERNAL`` be sure that you
can compile the cpp files.

Set a buffer
------------

In order to render the animation a buffer needs to assign to the Lottie widget.
The animations are rendered in ARGB8888 format, therefor the buffer's size should be equal to
``target_width x target_height x 4`` bytes.

To keep the buffer size and the animation size consistent,
the size of the widget (i.e. the size of the animation) is set to the dimensions of the buffer internally.

The buffer can be set with either :cpp:expr:`void lv_lottie_set_buffer(lottie, w, h, buf);`
or :cpp:expr:`lv_lottie_set_draw_buf(lottie, draw_buf)`.

When a draw buffer is used, it must be already initialized by the user with :cpp:expr:`LV_COLOR_FORMAT_ARGB8888` color format.

Set a source
------------

``lv_example_lottie_approve.c`` contains an example animation. Instead storing the JSON string, a hex array is stored for the
following reasons:
- avoid escaping ``"`` character in the JSON file
- some compilers don't support very long strings

``lvgl/scripts/filetohex.py`` can be used to convert a Lottie file a hex
array. E.g.:

.. code:: shell

   ./filetohex.py path/to/lottie.json > out.txt

To create an animation from data use
:cpp:enumerator:`lv_lottie_set_src_data(lottie, data, sizeof(data))`

Lottie animations can be opened from JSON files by using :cpp:enumerator:`lv_lottie_set_src_file(lottie, "path/to/file.json")`.
Note that the Lottie loader doesn't support LVGL's File System interface but a "normal path" should be used without a driver letter.

Get the animation
-----------------

``lv_anim_t * a = lv_lottie_get_anim(lottie)`` return the LVGL animation which controls the
Lottie animation. By default it is running infinitely at 60FPS however the LVGL animation
can be freely adjusted.

.. _lv_lottie_events:

Events
******

No special events are sent by the Lottie widget.

Learn more about :ref:`events`.

.. _lv_lottie_keys:

Keys
****

No keys are processed by the Lottie widget
Learn more about :ref:`indev_keys`.

.. _lv_lottie_example:

Example
*******

.. include:: ../examples/widgets/lottie/index.rst

.. _lv_lottie_api:

API
***
