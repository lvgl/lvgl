========
Observer
========

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


.. toctree::
    :maxdepth: 2

    subject
    observer
    subject_groups
    widget_binding


Example
*******

.. include:: /examples/others/observer/index.rst


API
***
