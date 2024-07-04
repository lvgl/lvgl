.. _observer:

========
Observer
========

.. _observer_overview:

Overview
********

The ``lv_observer`` module implements a standard `Observer pattern <https://en.wikipedia.org/wiki/Observer_pattern>`__.

It consists of:

- **subjects**: each containing a value
- **observers**: attached to subjects to be notified on value change


A typical use case looks like this:

.. code:: c

    //It's a global variable
    lv_subject_t my_subject;

    /*-------
     * main.c
     *-------*/

    extern lv_subject_t my_subject;

    void main(void)
    {
        //Initialize the subject as integer with the default value of 10
        lv_subject_init_int(&my_subject, 10);

        some_module_init();
    }

    /*--------------
     * some_module.c
     *--------------*/

    extern lv_subject_t some_subject;

    //Will be called when the related subject's value changes
    static void some_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
    {
        int32_t v = lv_subject_get_int(subject);
        do_something(v);
    }

    void some_module_init(void)
    {
        //Subscribe to a subject
        lv_subject_add_observer(&some_subject, some_observer_cb, NULL);
    }

    /*--------------
     * some_system.c
     *--------------*/

    extern lv_subject_t some_subject;

    void some_event(void)
    {
        //Set the subject's value to 30. It will notify `some_observer_cb`
        lv_subject_set_int(&some_subject, 30);
    }


.. _observer_subject:

Subject
*******

Subject initialization
----------------------

Subjects have to be static or global :cpp:type:`lv_subject_t` type variables.

To initialize a subject use :cpp:expr:`lv_subject_init_<type>(&subject, params, init_value)`.
The following initializations exist for types:

- **Integer** :cpp:expr:`void lv_subject_init_int(lv_subject_t * subject, int32_t value)`
- **String** :cpp:expr:`void lv_subject_init_string(lv_subject_t * subject, char * buf, char * prev_buf, size_t size, const char * value)`
- **Pointer**  :cpp:expr:`void lv_subject_init_pointer(lv_subject_t * subject, void * value)`
- **Color** :cpp:expr:`void lv_subject_init_color(lv_subject_t * subject, lv_color_t color)`
- **Group** :cpp:expr:`void lv_subject_init_group(lv_subject_t * subject, lv_subject_t * list[], uint32_t list_len)`


Set subject value
-----------------

The following functions can be used to set a subject's value:

- **Integer** :cpp:expr:`void lv_subject_set_int(lv_subject_t * subject, int32_t value)`
- **String** :cpp:expr:`void lv_subject_copy_string(lv_subject_t * subject, char * buf)`
- **Pointer**  :cpp:expr:`void lv_subject_set_pointer(lv_subject_t * subject, void * ptr)`
- **Color** :cpp:expr:`void lv_subject_set_color(lv_subject_t * subject, lv_color_t color)`

Get subject's value
-------------------

The following functions can be used to get a subject's value:


- **Integer** :cpp:expr:`int32_t lv_subject_get_int(lv_subject_t * subject)`
- **String** :cpp:expr:`const char * lv_subject_get_string(lv_subject_t * subject)`
- **Pointer**  :cpp:expr:`const void * lv_subject_get_pointer(lv_subject_t * subject)`
- **Color** :cpp:expr:`lv_color_t lv_subject_get_color(lv_subject_t * subject)`


Get subject's previous value
----------------------------

The following functions can be used to get a subject's previous value:


- **Integer** :cpp:expr:`int32_t lv_subject_get_previous_int(lv_subject_t * subject)`
- **String** :cpp:expr:`const char * lv_subject_get_previous_string(lv_subject_t * subject)`
- **Pointer** :cpp:expr:`const void * lv_subject_get_previous_pointer(lv_subject_t * subject)`
- **Color** :cpp:expr:`lv_color_t lv_subject_get_previous_color(lv_subject_t * subject)`

.. _observer_observer:

Observer
********

Subscribe to a subject
----------------------

To subscribe to a subject the following function can be used:

.. code:: c

    lv_observer_t * observer = lv_subject_add_observer(&some_subject, some_observer_cb, user_data);


Where the observer callback should look like this:

.. code:: c

    static void some_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
    {
        ...
    }


It's also possible to save a target widget when subscribing to a subject.
In this case when widget is deleted, it will automatically unsubscribe from the subject.

In the observer callback :cpp:expr:`lv_observer_get_target(observer)` can be used to get the saved widget.

.. code:: c

    lv_observer_t * observer = lv_subject_add_observer_obj(&some_subject, some_observer_cb, obj, user_data);


In more generic case any pointer can be saved a target:

.. code:: c

    lv_observer_t * observer = lv_subject_add_observer_with_target(&some_subject, some_observer_cb, some_pointer, user_data);



Unsubscribe from a subject
--------------------------

.. code:: c

	//`observer` is the return value of `lv_subject_add_observer*`
    lv_observer_remove(observer);

To unsubscribe a widget from a given or all subject use:

.. code:: c

	lv_obj_remove_from_subject(obj, subject); //`subject` can be NULL to unsubcribe from all

.. _observer_subject_groups:

Subject groups
**************

There are cases when a subject changes and the value of some other subjects are also required by the observer.
As a practical example imagine an instrument which measures either voltage or current.
To display the measured value on a label 3 things are required:

1. What do we measure (current or voltage)?
2. What is the measured value?
3. What is the range or unit (mV, V, mA, A)?

When any of these 3 parameters changes the label needs to be updated,
and it needs to know all 3 parameters to compose its text.

To handle this you can create an array from some existing subjects and pass
this array as a parameter when you initialize a subject with group type.

.. code:: c
    static lv_subject_t * subject_list[3] = {&subject_1, &subject_2, &subject_3};
    lv_subject_init_group(&subject_all, subject_list, 3);  /*The last parameter is the number of elements*/

You can add observers to subject groups in the regular way.
The trick is that when any element of the group is notified the subject group will be notified too.

The above Voltage/Current measurement example looks like this in the practice:

.. code:: c

    lv_obj_t * label = lv_label_create(lv_screen_active());

    lv_subject_t subject_mode;  //Voltage or Current
    lv_subject_t subject_value; //Measured value
    lv_subject_t subject_unit;  //The unit
    lv_subject_t subject_all;   //It will be the subject group
    lv_subject_t * subject_list[3] = {&subject_mode, &subject_value, &subject_unit};  //The elements of the group

    lv_subject_init_int(&subject_mode, 0); //Let's say 0 is Voltage, 1 is Current
    lv_subject_init_int(&subject_value, 0);
    lv_subject_init_pointer(&subject_unit, "V");
    lv_subject_init_group(&subject_all, subject_list, 3);

    lv_subject_add_observer_obj(&subject_all, all_observer_cb, label, NULL);

    ...

    static void all_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
    {
        lv_obj_t * label = lv_observer_get_target(observer);
        lv_subject_t * subject_mode = lv_subject_get_group_element(subject, 0);
        lv_subject_t * subject_value = lv_subject_get_group_element(subject, 1);
        lv_subject_t * subject_unit = lv_subject_get_group_element(subject, 2);

        int32_t mode = lv_subject_get_int(subject_mode);
        int32_t value = lv_subject_get_int(subject_value);
        const char * unit = lv_subject_get_pointer(subject_unit);

        lv_label_set_text_fmt(label, "%s: %d %s", mode ? "Current" : "Voltage", value, unit);
    }

.. _observer_widget_binding:

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

Set an integer subject to 1 when an object is checked and set it 0 when unchecked.

.. code:: c

    observer = lv_obj_bind_checked(obj, &subject);

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

.. _observer_example:

Example
*******

.. include:: ../examples/others/observer/index.rst

.. _observer_api:

API
***
