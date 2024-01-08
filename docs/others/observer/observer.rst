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

    lv_observer_remove(observer)

To unsubscribe from a subject with all widgets you can use:

.. code:: c

    lv_subject_remove_obj(subject, obj)

