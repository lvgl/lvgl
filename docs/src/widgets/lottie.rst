.. _lv_lottie:

==================
Lottie (lv_lottie)
==================

Overview
********

The Lottie Widget is capable of parsing, rasterizing, and playing `Lottie animations <https://lottiefiles.com>`__.

The Lottie animations are vector-based animations. Think of them as the modern combination of SVG and GIF.

The animations can be downloaded from various sources, such as `https://lottiefiles.com/ <https://lottiefiles.com/>`__
or you can create your own animations using, for example, Adobe After Effects.

The Lottie Widget is based on :ref:`lv_canvas` because in order to render the animation
the user needs to provide a buffer where the current animation frame is stored.

.. _lv_lottie_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` The background of the Lottie animation. The typical background style properties apply but usually it is left transparent.

.. _lv_lottie_usage:

Usage
*****

Dependencies
------------

The Lottie Widget uses the `ThorVG <https://github.com/thorvg/thorvg>`__ library which is `integrated into LVGL <https://github.com/lvgl/lvgl/tree/master/src/libs/thorvg>`__.
In order to use Lottie animations :c:macro:`LV_USE_THORVG_INTERNAL` (to use the built-in ThorVG) or
:c:macro:`LV_USE_THORVG_EXTERNAL` (to link it externally) needs to be enabled in ``lv_conf.h``. For vector graphics in general
:c:macro:`LV_USE_VECTOR_GRAPHIC` also needs to be enabled.

As ThorVG is written in C++, when using :c:macro:`LV_USE_THORVG_INTERNAL` be sure that you
can compile the cpp files.

Set a buffer
------------

In order to render the animation a buffer needs to be assigned to the Lottie Widget.
The animations are rendered in ARGB8888 format, therefore the buffer's size should be equal to
``target_width x target_height x 4`` bytes.

To keep the buffer size and the animation size consistent,
the size of the Widget (i.e. the size of the animation) is set to the dimensions of the buffer internally.

The buffer can be set with either :cpp:expr:`lv_lottie_set_buffer(lottie, w, h, buf)`
or :cpp:expr:`lv_lottie_set_draw_buf(lottie, draw_buf)`.

When a draw buffer is used, it must be already initialized by the user with :cpp:enumerator:`LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED` color format.

Set a source
------------

``lv_example_lottie_approve.c`` contains an example animation. Instead of storing the JSON string, a hex array is stored for the
following reasons:

- to avoid escaping ``"`` character in the JSON file, and
- some compilers don't support very long strings.

``lvgl/scripts/filetohex.py`` can be used to convert a Lottie file to a hex
array. E.g.:

.. code-block:: shell

   ./filetohex.py path/to/lottie.json --filter-character --null-terminate > out.txt

``--filter-character`` filters out non-ASCII characters and ``--null-terminate`` makes sure that a trailing zero is appended to properly close the string.

To create an animation from data use
:cpp:expr:`lv_lottie_set_src_data(lottie, data, sizeof(data))`

Lottie animations can be opened from JSON files by using :cpp:expr:`lv_lottie_set_src_file(lottie, "path/to/file.json")`.
Note that the Lottie loader doesn't support LVGL's File System interface but a "normal path" should be used without a driver letter.

Get the animation
-----------------

.. code-block:: c

    lv_anim_t * a = lv_lottie_get_anim(lottie)

returns the LVGL animation which controls the
Lottie animation. By default it is running infinitely at 60FPS however the LVGL animation
can be freely adjusted.



.. _lv_lottie_events:

Events
******

No events are emitted by Lottie Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_lottie_keys:

Keys
****

No keys are processed by Lottie Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_lottie_example:

Examples
********

.. include:: /examples/widgets/lottie/index.rst



.. _lv_lottie_api:

API
***
