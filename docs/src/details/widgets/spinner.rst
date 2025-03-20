.. _lv_spinner:

====================
Spinner (lv_spinner)
====================


Overview
********

The Spinner Widget is a spinning arc over a ring, typically used to show some type of
activity is in progress.



.. _lv_spinner_parts_and_styles:

Parts and Styles
****************

Spinner's parts are identical to those of :ref:`Arc <lv_arc_parts_and_styles>`.



.. _lv_spinner_usage:

Usage
*****

Create a spinner
----------------

To create a spinner use
:cpp:expr:`lv_spinner_create(parent)`.

Use :cpp:expr:`lv_spinner_set_anim_params(spinner, spin_duration, angle)` to
customize the duration of one revolution and the length of the arc.



.. _lv_spinner_events:

Events
******

No special events are sent by Spinner Widgets.

.. admonition::  Further Reading

    :ref:`Arc Events <lv_arc_events>`

    Learn more about :ref:`lv_obj_events` emitted by all Widgets.

    Learn more about :ref:`events`.



.. _lv_spinner_keys:

Keys
****

No *Keys* are processed by Spinner Widgets.

.. admonition::  Further Reading

    Learn more about :ref:`indev_keys`.



.. _lv_spinner_example:

Example
*******

.. include:: ../../examples/widgets/spinner/index.rst



.. _lv_spinner_api:

API
***
