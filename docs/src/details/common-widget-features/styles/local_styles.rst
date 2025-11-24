.. _style_local:

============
Local Styles
============

Overview
********

In addition to "normal" styles, Widgets can also store local styles.
This concept is similar to inline styles in CSS
(e.g. ``<div style="color:red">``) with some modification.

Local styles are like normal styles, but they can't be shared among
other Widgets. If used, local styles are allocated automatically, and
freed when the Widget is deleted. They are useful to add local
customization to a Widget.

Unlike in CSS, LVGL local styles can be assigned to states
(pseudo-classes_) and parts (pseudo-elements_).

Usage
*****

To set a local property use functions like
``lv_obj_set_style_<property_name>(widget, <value>, <selector>);``   For example:

.. code-block:: c

    lv_obj_set_style_bg_color(slider, lv_color_red(), LV_PART_INDICATOR | LV_STATE_FOCUSED);


Binding Local Styles
********************

By using :cpp:expr:`lv_obj_bind_style_prop`, it's possible to bind a style property
to a :ref:`Subject <observer_overview>`\ 's value.

It's a great way to map every slider's color or opacity to a subject and control it
externally.

For example:

.. code-block:: c

   lv_obj_bind_style_prop(slider1, LV_STYLE_BG_OPA, LV_PART_MAIN, &subject_bg_opa);
   lv_obj_bind_style_prop(slider1, LV_STYLE_BG_COLOR, LV_PART_INDICATOR, &subject_bg_color);


..  Hyperlinks

.. _pseudo-elements:
.. _pseudo-element:   https://developer.mozilla.org/en-US/docs/Learn/CSS/Building_blocks/Selectors#pseudo-classes_and_pseudo-elements
.. _pseudo-classes:
.. _pseudo-class:     https://developer.mozilla.org/en-US/docs/Glossary/Pseudo-class



