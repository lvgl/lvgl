.. _display_inactivity:

======================
Inactivity Measurement
======================

A user's inactivity time is measured and stored with each ``lv_display_t`` object.
Every use of an :ref:`Input Device <indev>` (if :ref:`associated with the display
<indev_other_features>`) counts as activity. To get time elapsed since the last
activity, use :cpp:expr:`lv_display_get_inactive_time(display1)`. If ``NULL`` is
passed, the lowest inactivity time among all displays will be returned (in this case
NULL does *not* mean the :ref:`default_display`).

You can manually trigger an activity using
:cpp:expr:`lv_display_trigger_activity(display1)`.  If ``display1`` is ``NULL``, the
:ref:`default_display` will be used (**not all displays**).


.. admonition::  Further Reading

    -  `lv_port_disp_template.c <https://github.com/lvgl/lvgl/blob/master/examples/porting/lv_port_disp_template.c>`__
       for a template for your own driver.
    -  :ref:`Drawing <draw>` to learn more about how rendering works in LVGL.



API
***

.. API equals:  lv_display_get_inactive_time
