.. _xml_subjects:

=======================
Data binding (Subjects)
=======================

Overview
********

With the help of :ref:`Subjects <observer_subject>`, the interface of the UI can be created.

A subject is a global data item whose value can be set either from the application or the UI, and whose value
can be bound to widget properties.

For example, a ``room1_temperature`` subject's value can be set in the application when the temperature
is measured, and can be bound to a label like this:

.. code-block:: xml

    <lv_label bind_text="room1_temperature"/>

Defining subjects
*****************

Subjects can be created in ``globals.xml`` like this:

.. code-block:: xml

    <globals>
        <subjects>
            <int name="battery_power" value="32"/>
            <string name="user_name" value="John"/>
        </subjects>
    </globals>

As the example shows, a subject consists of a type, name, and initial value.
Currently, only integer and string types are supported.

Simple binding
**************

Some widgets (e.g., label, slider) support binding the subject's value directly to the widget.
These bindings use attributes that start with ``bind_*`` and reference a subject.

.. code-block:: xml

    <lv_slider bind_value="some_subject"/>
    <lv_label bind_text="some_subject"/>

Once a binding is created, if the subject's value changes (e.g., by adjusting the slider),
all bound widgets will be updated automatically.

Complex binding
***************

In more complex cases—when a binding requires multiple parameters—the binding can be added as a child element of a widget.
This allows binding multiple subjects with different parameters. For example:

.. code-block:: xml

    <lv_label text="Hello world">
        <lv_obj-bind_flag_if_eq subject="subject1" flag="hidden" ref_value="10"/>
        <lv_obj-bind_flag_if_gt subject="subject1" flag="clickable" ref_value="20"/>
    </lv_label>

Explanation of complex bindings:

- ``<lv_obj-bind_flag_if_eq>`` — Set a flag if the subject's value **equals** the reference value.
- ``<lv_obj-bind_flag_if_not_eq>`` — Set a flag if the subject's value **does not equal** the reference value.
- ``<lv_obj-bind_flag_if_gt>`` — Set a flag if the subject's value is **greater than** the reference value.
- ``<lv_obj-bind_flag_if_ge>`` — Set a flag if the subject's value is **greater than or equal to** the reference value.
- ``<lv_obj-bind_flag_if_lt>`` — Set a flag if the subject's value is **less than** the reference value.
- ``<lv_obj-bind_flag_if_le>`` — Set a flag if the subject's value is **less than or equal to** the reference value.

- ``<lv_obj-bind_state_if_eq>`` — Set a state if the subject's value **equals** the reference value.
- ``<lv_obj-bind_state_if_not_eq>`` — Set a state if the subject's value **does not equal** the reference value.
- ``<lv_obj-bind_state_if_gt>`` — Set a state if the subject's value is **greater than** the reference value.
- ``<lv_obj-bind_state_if_ge>`` — Set a state if the subject's value is **greater than or equal to** the reference value.
- ``<lv_obj-bind_state_if_lt>`` — Set a state if the subject's value is **less than** the reference value.
- ``<lv_obj-bind_state_if_le>`` — Set a state if the subject's value is **less than or equal to** the reference value.

Note: The ``lv_obj-`` prefix can be omitted. For example, you can simply write ``<bind_state_if_gt>`` instead.
