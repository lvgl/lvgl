.. _observer_how_to_use:

==========
How to Use
==========

.. _observer_overview:

Overview
********

.. _observer pattern:  https://en.wikipedia.org/wiki/Observer_pattern

The ``lv_observer`` module is an implementation of the `Observer Pattern`_.

This implementation consists of:

:Subjects:   (in global memory or heap) are "logic packages", each containing the
             value being "observed" and its type (integer (``int32_t``), a string, a
             pointer, an :cpp:type:`lv_color_t`, a ``float``, or a group);

:Observers:  (zero or more per Subject, always dynamically-allocated) are always
             attached to exactly one Subject, and provide user-defined notifications
             each the time Subject's value changes.

A Subject and its Observers can be used in various ways:

1.  Simply subscribe to a Subject and get notified when the Subject's value changes.

2.  Subscribe to a group Subject (connects a group of Subjects) to get notified when
    any of the Subjects' values change in the group.

3.  Bind Widgets to Subjects to automatically match the Widget's value with the
    Subject (e.g. a Label's text or an Arc's value).



.. _observer_usage:

Usage
*****

Using Observer first requires :c:macro:`LV_USE_OBSERVER` be configured to ``1``.
(It is ``1`` by default, and can be set to ``0`` to save some program space if you
will not be using Observer.)

A typical use case looks like this:

.. code-block:: c

    // Any typical global variable
    lv_subject_t my_subject;

    /*-------
     * main.c
     *-------*/

    extern lv_subject_t my_subject;

    void main(void)
    {
        // Initialize Subject as integer with the default value of 10.
        lv_subject_init_int(&my_subject, 10);

        some_module_init();
    }

    /*--------------
     * some_module.c
     *--------------*/

    extern lv_subject_t my_subject;

    // Will be called when Subject's value changes
    static void some_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
    {
        int32_t v = lv_subject_get_int(subject);
        do_something(v);
    }

    void some_module_init(void)
    {
        // Subscribe to Subject as an Observer.
        lv_subject_add_observer(&my_subject, some_observer_cb, NULL);
    }

    /*--------------
     * some_system.c
     *--------------*/

    extern lv_subject_t my_subject;

    void some_event(void)
    {
        // The below call sets Subject's value to 30 and notifies current Observers.
        lv_subject_set_int(&my_subject, 30);
    }



.. _observer_subject:

Subject
-------

Subject Initialization
~~~~~~~~~~~~~~~~~~~~~~

Subjects have to be static or global variables, or dynamically-allocated
:cpp:type:`lv_subject_t` objects.  Reason:  their content must remain valid through
the life of the Subject.

To initialize a Subject use ``lv_subject_init_<type>(&subject, params, init_value)``.
The following initialization functions exist, one for each of the Subject types:

:Integer: void :cpp:expr:`lv_subject_init_int(subject, int_value)`
:Float:   void :cpp:expr:`lv_subject_init_float(subject, float_value)`
:String:  void :cpp:expr:`lv_subject_init_string(subject, buf, prev_buf, buf_size, initial_string)`
:Pointer: void :cpp:expr:`lv_subject_init_pointer(subject, ptr)`
:Color:   void :cpp:expr:`lv_subject_init_color(subject, color)`
:Group:   void :cpp:expr:`lv_subject_init_group(group_subject, subject_list[], count)`


Setting a Subject's Value
~~~~~~~~~~~~~~~~~~~~~~~~~

The following functions are used to update a Subject's value:

:Integer: void :cpp:expr:`lv_subject_set_int(subject, int_value)`
:String:  void :cpp:expr:`lv_subject_copy_string(subject, buf)`
:Pointer: void :cpp:expr:`lv_subject_set_pointer(subject, ptr)`
:Color:   void :cpp:expr:`lv_subject_set_color(subject, color)`

At the end of each of these calls, if the new value differs from the previous value,
a notification is sent to all current Observers.


Getting a Subject's Value
~~~~~~~~~~~~~~~~~~~~~~~~~

The following functions are used to get a Subject's current value:


:Integer: int32_t      :cpp:expr:`lv_subject_get_int(subject)`
:Float:   float        :cpp:expr:`lv_subject_get_float(subject)`
:String:  const char * :cpp:expr:`lv_subject_get_string(subject)`
:Pointer: const void * :cpp:expr:`lv_subject_get_pointer(subject)`
:Color:   lv_color_t   :cpp:expr:`lv_subject_get_color(subject)`


Getting a Subject's Previous Value
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The following functions are used to get a Subject's previous value:


:Integer: int32_t      :cpp:expr:`lv_subject_get_previous_int(subject)`
:Float:   float        :cpp:expr:`lv_subject_get_previous_float(subject)`
:String:  const char * :cpp:expr:`lv_subject_get_previous_string(subject)`
:Pointer: const void * :cpp:expr:`lv_subject_get_previous_pointer(subject)`
:Color:   lv_color_t   :cpp:expr:`lv_subject_get_previous_color(subject)`

Setting a Range for a Subject
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The range of the integer and float subjects can be limited by setting minimum and maximum values:

- :cpp:expr:`lv_subject_set_min_value_int(subject, value)`
- :cpp:expr:`lv_subject_set_max_value_int(subject, value)`
- :cpp:expr:`lv_subject_set_min_value_float(subject, value)`
- :cpp:expr:`lv_subject_set_max_value_float(subject, value)`

The default range is the smallest and largest possible values, so there is no limit by default.

.. _observer_observer:

Observer
--------


Subscribing to a Subject
~~~~~~~~~~~~~~~~~~~~~~~~

The action of subscribing to a Subject:

- dynamically allocates an Observer object,
- attaches it to the Subject,
- performs an initial notification to the Observer (allowing the Observer to
  update itself with the Subject's current value), and
- returns a pointer to the newly-created Observer.

Thereafter the Observer will receive a notification each time the Subject's value
changes, as long as that Observer remains attached (subscribed) to that Subject.

Notifications are performed by calling the callback function provided when
subscribing to the Subject.

Simple Subscription
~~~~~~~~~~~~~~~~~~~

To subscribe to a Subject one of the ``lv_subject_add_observer...()`` functions are
used. This is covered below.

Alternately, if you want to bind a Subject's value to a Widget's property, one
of the ``lv_<widget_type>_bind_...()`` functions can be used.  See
:ref:`observer_widget_binding` for more details.

By using  ``lv_obj_add_subject_...()`` it's also possible to change a subject's value
on a trigger. It's covered in :ref:`change_subject_on_event`


For the most basic use case, subscribe to a Subject by using the following function:

    lv_observer_t * observer =  :cpp:expr:`lv_subject_add_observer(&some_subject, some_observer_cb, user_data)`

where the Observer's notification callback should look like this:

.. code-block:: c

    static void some_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
    {
        ...
    }

This function returns a pointer to the newly-created Observer.

When using this method of subscribing, it is the responsibility of the user to call
:cpp:expr:`lv_observer_remove(observer)` when the Observer is no longer needed, which
both unsubscribes it from the Subject and deletes it from the LVGL heap.

Subscribing While Associating Observer with a Non-Widget Object
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The function subscribes to a Subject additionally associates the Observer with a
pointer to any type of object, a copy of which is saved in the Observer's ``target``
field.  This function should be used when the pointer *does not* point to a Widget.

    lv_observer_t * observer =  :cpp:expr:`lv_subject_add_observer_with_target(&some_subject, some_observer_cb, some_pointer, user_data)`

A copy of the passed pointer can be retrieved by calling
:cpp:expr:`lv_observer_get_target(observer)`, e.g. inside the callback function.

When using this method of subscribing, it is the responsibility of the user to call
:cpp:expr:`lv_observer_remove(observer)` when the Observer is no longer needed, which
both unsubscribes it from the Subject and deletes it from the LVGL heap.

Subscribing While Associating Observer with a Widget
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The function below associates a Widget with an Observer while subscribing to a
Subject.  A copy of the pointer to that Widget is saved in the Observer's ``target``
field.  This works exactly like the above method except that when the Widget is
deleted, the Observer thus created will be automatically unsubscribed from the
Subject and deleted from the LVGL heap.  Note this is different from
:ref:`observer_widget_binding`.

    lv_observer_t * observer =  :cpp:expr:`lv_subject_add_observer_obj(&some_subject, some_observer_cb, widget, user_data)`

Any number of Observers can be created and be associated with a Widget this way.

A copy of the pointer to the Widget can be retrieved by calling
:cpp:expr:`lv_observer_get_target_obj(observer)`, e.g. inside the callback function.
Note that this function returns the stored pointer as a ``lv_obj_t *`` type, as
opposed to the ``void *`` type returned by
:cpp:expr:`lv_observer_get_target_obj(observer)`.
(:cpp:expr:`lv_observer_get_target(observer)` can still be used if you need that
pointer as a ``void *`` type for any reason, but in practice, this would be rare.)

Unsubscribing from a Subject
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

When an Observer and its associated events are no longer needed, you can remove it
from memory using any of the methods below, depending on the effect you want to create:

- If Widget needs to be deleted, simply delete the Widget, which will automatically
  gracefully remove the Observer (and its events) from the Subject, while deleting
  the Widget.

- If Widget does NOT need to be deleted:

    - :cpp:expr:`lv_obj_remove_from_subject(widget, subject)` deletes all Observers
      associated with ``widget``.  ``subject`` can be NULL to unsubscribe the Widget
      from all associated Subjects.
    - :cpp:expr:`lv_subject_deinit(subject)` gracefully disconnects ``subject`` from
      all associated Observers and Widget events.  This includes subscriptions made
      using any of the :ref:`observer_widget_binding` functions covered below.
    - :cpp:expr:`lv_observer_remove(observer)` deletes that specific Observer and
      gracefully disconnects it from its ``subject`` and any associated Widgets,
      where ``observer`` is the return value any of the above
      ``lv_subject_add_observer_...()`` functions.


.. _observer_subject_groups:

Subject Groups
--------------

When something in your system relies on more than one value (i.e. it needs to be
notified when any of a SET of two or more values changes), it can be made an
Observer of a Subject Group.

Let us consider an example of an instrument which measures either voltage or current.
To display the measured value on a label, 3 things are required:

1. What is being measured (current or voltage)?
2. What is the measured value?
3. What is the range or unit ("mV", "V", "mA", "A")?

When any of these 3 input values change, the label needs to be updated, and it needs
to know all 3 values to compose its text.

To handle this you can create an array from the addresses of all the Subjects that
are relied upon, and pass that array as a parameter when you initialize a Subject
with GROUP type.

.. code-block:: c

    static lv_subject_t * subject_list[3] = {&subject_1, &subject_2, &subject_3};
    lv_subject_init_group(&subject_all, subject_list, 3);  /* Last argument is number of elements. */

Observers are then added to Subject Groups (e.g. ``subject_all``) in the usual way.
When this is done, a change to the value of any of the Subjects in the group triggers
a notification to all Observers subscribed to the Subject Group (e.g. ``subject_all``).

As an example, the above scenario with Voltage/Current measurement might look like this:

.. code-block:: c

    lv_obj_t * label = lv_label_create(lv_screen_active());

    lv_subject_t subject_mode;  // Voltage or Current
    lv_subject_t subject_value; // Measured value
    lv_subject_t subject_unit;  // The unit
    lv_subject_t subject_all;   // Subject group that connects the above 3 Subjects
    lv_subject_t * subject_list[3] = {&subject_mode, &subject_value, &subject_unit};  // The elements of the group

    lv_subject_init_int(&subject_mode, 0); // Let's say 0 is Voltage, 1 is Current
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

Widget Binding
--------------

The following methods of subscribing to an integer-type Subject associate the
Observer with ONE of a Widget's properties as thought that property itself were the
Observer.  Any of the following Widget properties can be thus bound to an Subject's
integer value:

- flag (or OR-ed combination of flags) from from the ``LV_OBJ_FLAG_...`` enumeration values;
- state (or OR-ed combination of states) from the ``LV_STATE_...`` enumeration values;
- text and/or integer values for
    - Label
    - Span;
    - Arc
    - Drop-Down
    - Roller
    - Slider
    - Scale Section Min/Max values

Any number of Observers can be created for a single Widget, each bound to ONE of
the above properties.

For all of the ``lv_..._bind_...()`` functions covered below, they are similar to
:cpp:expr:`lv_subject_add_observer_obj(&some_subject, some_observer_cb, widget, user_data)`
in that they create an Observer and associates the Widget with it.  What is different
is that updates to the Widget's property thus bound are handled internally -- the
user *does not supply callback functions* for any of these subscribing methods -- the
callback methods are supplied by the Observer subsystem.

.. note::

    While the examples below show saving a reference to the created Observer objects
    for the various ``lv_..._bind_...()`` functions, it is not necessary to do so
    unless you need them for some purpose, because the created Observer objects will
    be automatically deleted when the Widget is deleted.


Any Widget Type
~~~~~~~~~~~~~~~

Flags
^^^^^

The following methods of subscribing to an integer Subject affect a Widget's flag (or
OR-ed combination of flags).  When the subscribing occurs, and each time the
Subject's value is changed thereafter, the Subject's value is compared with the
specified reference value, and the specified flag(s) is (are):

- SET when the Subject's integer value fulfills the indicated condition, and
- CLEARED otherwise.

Here are the functions that carry out this method of subscribing to a Subject.  The
``flags`` argument can contain a single, or bit-wise OR-ed combination of any of the
``LV_OBJ_FLAG_...`` enumeration values.

:equal:                  :cpp:expr:`lv_obj_bind_flag_if_eq(widget, &subject, flags, ref_value)`
:not equal:              :cpp:expr:`lv_obj_bind_flag_if_not_eq(widget, &subject, flags, ref_value)`
:greater than:           :cpp:expr:`lv_obj_bind_flag_if_gt(widget, &subject, flags, ref_value)`
:greater than or equal:  :cpp:expr:`lv_obj_bind_flag_if_ge(widget, &subject, flags, ref_value)`
:less than:              :cpp:expr:`lv_obj_bind_flag_if_lt(widget, &subject, flags, ref_value)`
:less than or equal:     :cpp:expr:`lv_obj_bind_flag_if_le(widget, &subject, flags, ref_value)`

States
^^^^^^

The following methods of subscribing to an integer Subject affect a Widget's states
(or OR-ed combination of states).  When the subscribing occurs, and each time the
Subject's value is changed thereafter, the Subject's value is compared with the
specified reference value, and the specified state(s) is (are):

- SET when the Subject's integer value fulfills the indicated condition, and
- CLEARED otherwise.

Here are the functions that carry out this method of subscribing to a Subject.  The
``states`` argument can contain a single, or bit-wise OR-ed combination of any of the
``LV_STATE_...`` enumeration values.

:equal:                  :cpp:expr:`lv_obj_bind_state_if_eq(widget, &subject, states, ref_value)`
:not equal:              :cpp:expr:`lv_obj_bind_state_if_not_eq(widget, &subject, states, ref_value)`
:greater than:           :cpp:expr:`lv_obj_bind_state_if_gt(widget, &subject, states, ref_value)`
:greater than or equal:  :cpp:expr:`lv_obj_bind_state_if_ge(widget, &subject, states, ref_value)`
:less than:              :cpp:expr:`lv_obj_bind_state_if_lt(widget, &subject, states, ref_value)`
:less than or equal:     :cpp:expr:`lv_obj_bind_state_if_le(widget, &subject, states, ref_value)`

Checked State
^^^^^^^^^^^^^

The following method of subscribing to an integer Subject affects a Widget's
:cpp:enumerator:`LV_STATE_CHECKED` state.  When the subscribing occurs, and each time
the Subject's value is changed thereafter, the Subject's value is compared to a
reference value of ``0``, and the :cpp:enumerator:`LV_STATE_CHECKED` state is:

- CLEARED when the Subject's value is 0, and
- SET when the Subject's integer value is non-zero.

Note that this is a two-way binding (Subject <===> Widget) so direct (or
programmatic) interaction with the Widget that causes its
:cpp:enumerator:`LV_STATE_CHECKED` state to be SET or CLEARED also causes the
Subject's value to be set to ``1`` or ``0`` respectively.

- :cpp:expr:`lv_obj_bind_checked(widget, &subject)`

Specific Widget Types
~~~~~~~~~~~~~~~~~~~~~

To learn how to bind subjects to Arcs, Labels, Sliders, etc. visit the "Data binding"
section of the given widget's documentation. For example: :ref:`Data binding for lv_label <lv_label_data_binding>`.


Change Subject on Event
-----------------------

It's a common requirement to update a Subject based on a user action (trigger).
To simplify this, *subject set* and *increment* actions can be attached directly to any widget.

Internally, these are implemented as special event callbacks.
Note: these callbacks are **not** automatically removed when a subject is de-initialized.


Toggle
~~~~~~

:cpp:expr:`lv_obj_add_subject_toggle_event(obj, subject)`

Toggle the subjects value when the trigger happens. IF the value was not 0, it will be 0.
If it was 0 it will be 1.

For example:

:cpp:expr:`lv_obj_add_subject_toggle_event(button1, subject1, LV_EVENT_CLICKED)`

This will toggle `subject1` between 0 and 1 each time `button1` is clicked.

Increment
~~~~~~~~~

:cpp:expr:`lv_obj_add_subject_increment_event(obj, subject, trigger, step)`
increments an integer subject's value by ``step``.

It returns a :cpp:type:`lv_subject_increment_dsc_t` pointer to configure the
event further:

- :cpp:expr:`lv_obj_set_subject_increment_event_min_value(obj, dsc, min_value)`:
  Set a minimum value for the event. Default ``INT32_MIN``
- :cpp:expr:`lv_obj_set_subject_increment_event_max_value(obj, dsc, max_value)`:
  Set a maximum value for the event. Default ``INT32_MAX``
- :cpp:expr:`lv_obj_set_subject_increment_event_rollover(obj, dsc, rollover)`:
  Set what to do when the min/max value is crossed. ``false``: stop at the min/max
  value; ``true``: jump to the other end. Default ``false``

It works on both integer and float subjects, but the min/max value is an integer in both cases.

If the subject also sets a min/max value the narrower range will be used.

Using a negative ``step`` will decrement the value instead.

For example:

:cpp:expr:`lv_obj_add_subject_increment_event(button1, subject1, LV_EVENT_CLICKED, 5)`

This will increment ``subject1`` by 5 when ``button1`` is clicked, stopping at the limits
set by the min/max values of the event or :cpp:expr:`lv_subject_set_min_value_int()`
and :cpp:expr:`lv_subject_set_max_value_int()` (same for float subjects).

Set to a Value
~~~~~~~~~~~~~~

- :cpp:expr:`lv_obj_add_subject_set_int_event(obj, subject, trigger, value)`
- :cpp:expr:`lv_obj_add_subject_set_string_event(obj, subject, trigger, text)`

These functions set the given subject (integer or string) to a fixed value when the specified trigger event occurs.

.. _observer_api:

API
***
