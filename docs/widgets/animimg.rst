.. _lv_animimg:

============================
Animation Image (lv_animimg)
============================

Overview
********

The animation image is similar to the normal 'Image' object. The only
difference is that instead of one source image, you set an array of
multiple source images.

You can specify a duration and repeat count.

.. _lv_animimg_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` A background rectangle that uses the typical
   background style properties and the image itself using the image
   style properties.

.. _lv_animimg_usage:

Usage
*****

Image sources
-------------

To set the image in a state, use the
:cpp:expr:`lv_animimg_set_src(imagebutton, dsc[], num)`.

Using the inner animation
-------------------------

For more advanced use cases, the animation internally used by the image can be
retrieved using the :cpp:expr:`lv_animimg_get_anim(image)`. This way, the
:ref:`Animation <animations>` functions can be used, for example to
override the animation values using the
:cpp:expr:`lv_anim_set_values(anim, start, end)` or to set a callback
on the animation completed event.

.. _lv_animimg_events:

Events
******

No special events are sent by image objects.

See the events of the Base object too.

Learn more about :ref:`events`.

.. _lv_animimg_keys:

Keys
****

No Keys are processed by the object type.

Learn more about :ref:`indev_keys`.

.. _lv_animimg_example:

Example
*******

.. include:: ../examples/widgets/animimg/index.rst

.. _lv_animimg_api:

API
***
