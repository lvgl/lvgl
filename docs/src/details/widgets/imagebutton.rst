.. _lv_imagebutton:

=============================
Image Button (lv_imagebutton)
=============================


Overview
********

The Image Button is very similar to the simple 'Button' Widget. The only
difference is that it displays user-defined images for each state instead
of drawing a rectangle.  The list of states is covered below.

You can set a left, right and middle image, and the middle image will be
repeated to match the width of the Widget.


.. _lv_imagebutton_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Refers to the image(s). If background style
   properties are used, a rectangle will be drawn behind the Image
   Button.


.. _lv_imagebutton_usage:

Usage
*****

Image sources
-------------

To set the image in a state, use the
:cpp:expr:`lv_imagebutton_set_src(imagebutton, LV_IMAGEBUTTON_STATE_..., src_left, src_mid, src_right)`.

The image sources work the same as described in the :ref:`Image Widget <lv_image>`
except that "Symbols" are not supported by the Image Button. Any of the sources can be ``NULL``.
Typically the middle image should be one of the set image sources.

If only ``src_mid`` is specified, the width of the widget will be set automatically to the
width of the image. However, if all three sources are set, the width needs to be set by the user
(using e.g. :cpp:expr:`lv_obj_set_width`) and the middle image will be tiled to fill the given size.

The possible states are:

- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_RELEASED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_PRESSED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_DISABLED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_CHECKED_RELEASED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_CHECKED_PRESSED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_CHECKED_DISABLED`

The image sources set for state :cpp:enumerator:`LV_IMAGEBUTTON_STATE_RELEASED` are
used for any state that has not had image sources set for it.  If an image sources
have been set for other states, e.g. :cpp:enumerator:`LV_IMAGEBUTTON_STATE_PRESSED`,
they will be used instead when the Image Button is in that state.

Setting State Programmatically
------------------------------

Instead of the regular :cpp:func:`lv_obj_add_state` and :cpp:func:`lv_obj_remove_state` functions,
use :cpp:expr:`lv_imagebutton_set_state(imagebutton, LV_IMAGEBUTTON_STATE_...)` to
set the state of Image Buttons.



.. _lv_imagebutton_events:

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when Image Button's CHECKED state is toggled.
   This requires the Image Button's :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` flag to be set.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_imagebutton_keys:

Keys
****

-  ``LV_KEY_RIGHT/UP`` Go to CHECKED state if :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE`
   is enabled.
-  ``LV_KEY_LEFT/DOWN`` Go to un-CHECKED state if
   :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` is enabled.
-  :cpp:enumerator:`LV_KEY_ENTER` Clicks the Image Button

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_imagebutton_example:

Example
*******

.. include:: ../../examples/widgets/imagebutton/index.rst



.. _lv_imagebutton_api:

API
***
