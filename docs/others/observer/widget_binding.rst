Widget binding
**************


Base object
-----------

Set an object flag if an integer subject's value is equal to a reference value, clear the flag otherwise

.. code:: c

    observer = lv_obj_bind_flag_if_eq(obj, &subject, LV_OBJ_FLAG_*, ref_value);

Set an object flag if an integer subject's value is not equal to a reference value, clear the flag otherwise

.. code:: c

    observer = lv_obj_bind_flag_if_not_eq(obj, &subject, LV_OBJ_FLAG_*, ref_value);

Set an object state if an integer subject's value is equal to a reference value, clear the flag otherwise

.. code:: c

    observer = lv_obj_bind_state_if_eq(obj, &subject, LV_STATE_*, ref_value);

Set an object state if an integer subject's value is not equal to a reference value, clear the flag otherwise

.. code:: c

    observer = lv_obj_bind_state_if_not_eq(obj, &subject, LV_STATE_*, ref_value);


Button
------

Set an integer subject to 1 when a button is checked and set it 0 when unchecked.

.. code:: c

    observer = lv_button_bind_checked(obj, &subject);


Label
-----

Bind an integer, string, or pointer (pointing to a string) subject to a label.
An optional format string can be added with 1 format specifier (e.g. ``"%d °C"``)
If the format string is ``NULL`` the value will be used directly. In this case on string and pointer type subjects can be used.

.. code:: c

    observer = lv_label_bind_text(obj, &subject, format_string);


Arc
---

Bind an integer subject to an arc's value.

.. code:: c

    observer = lv_arc_bind_value(obj, &subject);


Slider
------

Bind an integer subject to a slider's value

.. code:: c

    observer = lv_slider_bind_value(obj, &subject);


Roller
------

Bind an integer subject to a roller's value

.. code:: c

    observer = lv_roller_bind_value(obj, &subject);


Drop-down
---------
Bind an integer subject to a drop-down's value

.. code:: c

    observer = lv_dropdown_bind_value(obj, &subject);
