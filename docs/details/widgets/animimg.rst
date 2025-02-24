.. _lv_animimg:

============================
Animation Image (lv_animimg)
============================

Overview
********

The animation image is similar to the normal 'Image' Widget. The only
difference is that instead of one source image, you set an array of
multiple source images that supply "frames" in an animation.

You can specify a duration and repeat count.

.. _lv_animimg_parts_and_styles:

Parts and Styles
****************

-  :cpp:enumerator:`LV_PART_MAIN` A background rectangle that uses the :ref:`typical
   background style properties <typical bg props>` and the image itself using the image
   style properties.

.. _lv_animimg_usage:

Usage
*****

Image sources
-------------

To set the image animation images sources, use
:cpp:expr:`lv_animimg_set_src(animimg, dsc[], num)`.

Using the inner animation
-------------------------

For more advanced use cases, the animation used internally by the image can be
retrieved using :cpp:expr:`lv_animimg_get_anim(animimg)`.  Using this, you can
use the :ref:`Animation <animation>` functions, for example, to
override the animation values using the
:cpp:expr:`lv_anim_set_values(anim, start, end)` or to set a callback
on the animation completed event.



.. _lv_animimg_events:

Events
******

No special events are sent by Animation-Image Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_animimg_keys:

Keys
****

No *Keys* are processed by Animation-Image Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_animimg_example:

Example
*******

.. include:: ../../examples/widgets/animimg/index.rst



.. _lv_animimg_api:

API
***
