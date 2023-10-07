========
Observer
========

Overview
********

The ``lv_observer`` module implements a standard `Observer pattern <https://en.wikipedia.org/wiki/Observer_pattern>`__.
It consists of

- subjects: containing a value
- observers: attached to subjects to be notified on value change


I typical use case looks like this:

.. code:: c
    //It's a global variable
    lv_subject_t my_subject;

    /*-------
     * main.c
     *-------*/

    extern lv_subject_t my_subject;

    void main(void)
    {
        //Initialize the subject with default value of 10
        lv_subject_init_int(&my_subject, 10);

        some_module_init();
    }

    /*--------------
     * some_module.c
     *--------------*/

    extern lv_subject_t some_subject;

    static void some_observer_cb(lv_subject_t * subject, lv_observer_t * observer)
    {
        int32_t v = lv_subject_get_int(subject);
        do_something(v);
    }

    void some_module_init(void)
    {
        lv_subject_add_observer(&some_subject, some_observer_cb, NULL);
    }

    /*--------------
     * some_system.c
     *--------------*/

    extern lv_subject_t some_subject;

    void some_event(void)
    {
        //Set the subject to 30 and notify some_observer_cb
        lv_subject_set_int(&some_subject, 30);
    }



Subject
*******

Subject initialization
----------------------

Subjects have to be static or global :c:expr:`lv_subject_t` type variables.

To initialize a subject :c:expr:`lv_subject_init_<type>(&subject, <params>, init_value)`.
The following initializations exist for types:

- **Integer** :c:expr:`void lv_subject_init_int(lv_subject_t * subject, int32_t value)`
- **String** :c:expr:`void lv_subject_init_string(lv_subject_t * subject, char * buf, char * prev_buf, size_t size, const char * value)`
- **Pointer**  :c:expr:`void lv_subject_init_pointer(lv_subject_t * subject, void * value)`
- **Color** :c:expr:`void lv_subject_init_color(lv_subject_t * subject, lv_color_t color)`
- **Group** :c:expr:`void lv_subject_init_group(lv_subject_t * subject, lv_subject_t * list[], uint32_t list_len)`


Set subject value
-----------------

- **Integer** :c:expr:`void lv_subject_set_int(lv_subject_t * subject, int32_t value)`
- **String** :c:expr:`void lv_subject_copy_string(lv_subject_t * subject, char * buf)`
- **Pointer**  :c:expr:`void lv_subject_set_pointer(lv_subject_t * subject, void * ptr)`
- **Color** :c:expr:`void lv_subject_set_color(lv_subject_t * subject, lv_color_t color)`

Get subject's value
-------------------

- **Integer** :c:expr:`int32_t lv_subject_get_int(lv_subject_t * subject)`
- **String** :c:expr:`const char * lv_subject_get_string(lv_subject_t * subject)`
- **Pointer**  :c:expr:`const void * lv_subject_get_pointer(lv_subject_t * subject)`
- **Color** :c:expr:`lv_color_t lv_subject_get_color(lv_subject_t * subject)`


Get subject's previous value
----------------------------

- **Integer** :c:expr:`int32_t lv_subject_get_previous_int(lv_subject_t * subject)`
- **String** :c:expr:`const char * lv_subject_get_previous_string(lv_subject_t * subject)`
- **Pointer** :c:expr:`const void * lv_subject_get_previous_pointer(lv_subject_t * subject)`
- **Color** :c:expr:`lv_color_t lv_subject_get_previous_color(lv_subject_t * subject)`


Subject groups
--------------

TODO

Observer
********

Subscribe to a subject
----------------------

.. code:: c

    static void some_observer_cb(lv_subject_t * subject, lv_observer_t * observer)
    {
        ...
    }


    lv_observer_t * observer = lv_subject_add_observer(&some_subject, some_observer_cb, user_data);

.. code:: c

    lv_observer_t * observer = lv_subject_add_observer_obj(&some_subject, some_observer_cb, obj, user_data);

.. code:: c

    lv_observer_t * observer = lv_subject_add_observer_with_target(&some_subject, some_observer_cb, some_pointer, user_data);



Unsubscribe from a subject
--------------------------

.. code:: c

    lv_observer_remove(observer)


.. code:: c

    lv_subject_remove_obj(subject, obj)


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
An optional format string can be added with 1 format specifier (e.g. "%d °C")
If the format string is NULL the value will be used directly. In this case on string ans pointer type subjects can be used.

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


Example
-------

.. include:: ../examples/others/observer/index.rst

API
---



