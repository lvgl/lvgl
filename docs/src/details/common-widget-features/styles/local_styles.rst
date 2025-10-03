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



..  Hyperlinks

.. _pseudo-elements:
.. _pseudo-element:   https://developer.mozilla.org/en-US/docs/Learn/CSS/Building_blocks/Selectors#pseudo-classes_and_pseudo-elements
.. _pseudo-classes:
.. _pseudo-class:     https://developer.mozilla.org/en-US/docs/Glossary/Pseudo-class



