Image button (lv_imagebutton)
========================

Overview
********

The Image button is very similar to the simple 'Button' object. The only
difference is that it displays user-defined images in each state instead
of drawing a rectangle.

You can set a left, right and center image, and the center image will be
repeated to match the width of the object.

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` Refers to the image(s). If background style
   properties are used, a rectangle will be drawn behind the image
   button.

Usage
*****

Image sources
-------------

To set the image in a state, use the
:cpp:expr:`lv_imagebutton_set_src(imagebutton, LV_IMAGEBUTTON_STATE_..., src_left, src_center, src_right)`.

The image sources work the same as described in the `Image object </widgets/img>`__
except that "Symbols" are not supported by the Image button. Any of the sources can ``NULL``.

The possible states are:

- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_RELEASED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_PRESSED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_DISABLED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_CHECKED_RELEASED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_CHECKED_PRESSED`
- :cpp:enumerator:`LV_IMAGEBUTTON_STATE_CHECKED_DISABLED`

If you set sources only in :cpp:enumerator:`LV_IMAGEBUTTON_STATE_RELEASED`, these sources
will be used in other states too. If you set e.g. :cpp:enumerator:`LV_IMAGEBUTTON_STATE_PRESSED`
they will be used in pressed state instead of the released images.

States
------

Instead of the regular ``lv_obj_add/remove_state()`` functions the
:cpp:expr:`lv_imagebutton_set_state(imagebutton, LV_IMAGEBUTTON_STATE_...)` functions should be
used to manually set a state.

Events
******

-  :cpp:enumerator:`LV_EVENT_VALUE_CHANGED` Sent when the button is toggled.

Learn more about :ref:`events`.

Keys
****

-  ``LV_KEY_RIGHT/UP`` Go to toggled state if :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE`
   is enabled.
-  ``LV_KEY_LEFT/DOWN`` Go to non-toggled state if
   :cpp:enumerator:`LV_OBJ_FLAG_CHECKABLE` is enabled.
-  :cpp:enumerator:`LV_KEY_ENTER` Clicks the button

Learn more about :ref:`indev_keys`.

Example
*******

.. include:: ../examples/widgets/imgagebutton/index.rst

API
***
