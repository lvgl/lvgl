.. _widget_id:

=========
Widget ID
=========

Widgets can optionally have identifiers added to their functionality if needed for
the application.  Exactly how that happens is designed to be flexible, and can morph
with the needs of the application.  It can even be a timestamp or other data current
at the time the Widget was created.



.. _widget_id_usage:

Usage
*****

Enable Widget ID functionality by setting :c:macro:`LV_USE_OBJ_ID` to ``1`` in ``lv_conf.h``.

Once enabled, several things change:

- each Widget will now have a ``void *`` field called ``id``;
- these two API functions become available:

  - :cpp:expr:`lv_obj_get_id(widget)`,
  - :cpp:expr:`lv_obj_find_by_id(widget, id)`;

- several more Widget-ID-related API functions become available if
  :c:macro:`LV_USE_OBJ_ID_BUILTIN` is non-zero (more on this below);
- two additional configuration macros both :c:macro:`LV_OBJ_ID_AUTO_ASSIGN` and
  :c:macro:`LV_USE_OBJ_ID_BUILTIN` now have meaning.


:c:macro:`LV_OBJ_ID_AUTO_ASSIGN`
--------------------------------

This macro  in ``lv_conf.h`` defaults to whatever value :c:macro:`LV_USE_OBJ_ID`
equates to.  You can change this if you wish.  Either way, if it equates to a
non-zero value, it causes two things to happen:

- :cpp:expr:`lv_obj_assign_id(class_p, widget)` will be called at the end of each
  Widget's creation, and
- :cpp:expr:`lv_obj_free_id(widget)` will be called at the end of the sequence when
  each Widget is deleted.

Because of this timing, custom versions of these functions can be used according to
the below, and they can even be used like "event hooks" to implement a trace
operation that occurs when each Widget is created and deleted.

:cpp:expr:`lv_obj_assign_id(class_p, widget)`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This function (whether provided by LVGL or by you --- more on this below) is
responsible for assigning a value to the Widget's ``id`` field, and possibly do
other things, depending on the implementation.

:cpp:expr:`lv_obj_free_id(widget)`
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

This function (whether provided by LVGL or by you --- more on this below) is
responsible for doing the clean-up of any resources allocated by
:cpp:func:`lv_obj_assign_id`


:c:macro:`LV_USE_OBJ_ID_BUILTIN`
--------------------------------

This macro  in ``lv_conf.h`` equates to ``1`` by default.  You can change this if you
wish.  When it equates to a non-zero value the following function implementations are
provided by LVGL:

- :cpp:expr:`lv_obj_assign_id(class_p, widget)`
- :cpp:expr:`lv_obj_free_id(widget)`
- :cpp:expr:`lv_obj_set_id(widget, id)`
- :cpp:expr:`lv_obj_stringify_id(widget, buf, len)`
- :cpp:expr:`lv_obj_id_compare(id1, id2)`

These supply the default implementation for Widget IDs, namely that for each Widget
created, :cpp:expr:`lv_obj_stringify_id(widget, buf, len)` will produce a unique
string for it.  Example:  if the following 6 Widgets are created in this sequence:

- Screen
- Label
- Button
- Label
- Label
- Image

the strings produced by :cpp:expr:`lv_obj_stringify_id(widget, buf, len)` would be

- obj1
- label1
- btn1
- label2
- label3
- image1

respectively.


.. _widget_id_custom_generator:

Using a custom ID generator
---------------------------

If you wish, you can provide custom implementations for several Widget-ID related
functions.  You do this by first setting :c:macro:`LV_USE_OBJ_ID_BUILTIN` to `0` in
``lv_conf.h``.

You will then need to provide implementations for the following functions (and link
them with LVGL):

.. code-block:: c

    const char * lv_obj_stringify_id(lv_obj_t * widget, char * buf, uint32_t len);
    int          lv_obj_id_compare(const void * id1, const void * id2);

If :c:macro:`LV_OBJ_ID_AUTO_ASSIGN` equates to a non-zero value (or if you otherwise
simply need to use them), you will also need to provide implementations for:

.. code-block:: c

    void         lv_obj_assign_id(const lv_obj_class_t * class_p, lv_obj_t * widget);
    void         lv_obj_free_id(lv_obj_t * widget);

If :c:macro:`LV_BUILD_TEST` equates to a non-zero value and you are including LVGL
test code in your compile (or if you otherwise simply need to use them), you
will also need to provide an implementation for:

.. code-block:: c

    void         lv_obj_set_id(lv_obj_t * widget, void * id);


Examples of implementations of these functions exist in ``lv_obj_id_builtin.c``, but
you are free to use a different design if needed.

:cpp:func:`lv_obj_stringify_id` converts the passed ``widget`` to a string
representation (typically incorporating the ``id`` field) and writes it into the
buffer provided in its ``buf`` argument.

:cpp:func:`lv_obj_id_compare` compares 2 ``void * id`` values and returns ``0`` when
they are considered equal, and non-zero otherwise.

If :c:macro:`LV_OBJ_ID_AUTO_ASSIGN` equates to a non-zero value,
:cpp:func:`lv_obj_assign_id` is called when a Widget is created.  It is responsible
for assigning a value to the Widget's ``id`` field.  A pointer to the Widget's final
class is passed in its ``class_p`` argument in case it is needed for determining the
value for the ``id`` field, or for other possible needs related to your design for
Widget IDs.  Note that this pointer may be different than :cpp:expr:`widget->class_p`
which is the class of the Widget currently being created.

If :c:macro:`LV_OBJ_ID_AUTO_ASSIGN` equates to a non-zero value,
:cpp:func:`lv_obj_free_id` is called when a Widget is deleted.  It needs to perform
the clean-up for any resources allocated by :cpp:func:`lv_obj_assign_id`.


Dumping a Widget Tree
---------------------

Regardless of the state of any of the above macros, the function
:cpp:expr:`lv_obj_dump_tree(widget)` provides a "dump" of the Widget Tree for the
specified Widget (that Widget plus all its children recursively) using the
currently-configured method used by the :c:macro:`LV_LOG_USER` macro.  If NULL is
passed instead of a pointer to a "root" Widget, the dump will include the Widget Tree
for all :ref:`Screens`, for all :ref:`Displays <display>` in the system.

For :c:macro:`LV_LOG_USER` to produce output, the following needs to be true in
``lv_conf.h``:

- :c:macro:`LV_USE_LOG` must equate to a non-zero value
- :c:macro:`LV_LOG_LEVEL` <= :c:macro:`LV_LOG_LEVEL_USER`

It will recursively walk through all that Widget's children (starting with the Widget
itself) and print the Widget's parent's address, the Widget's address, and if
:c:macro:`LV_USE_OBJ_ID` equates to a non-zero value, will also print the output of
:cpp:func:`lv_obj_stringify_id` for that Widget.

This can be useful in the event of a UI crash.  From that log you can examine the
state of the Widget Tree when :cpp:expr:`lv_obj_dump_tree(widget)` was called.

For example, if a pointer to a deleted Widget is stored in a Timer's
:cpp:expr:`timer->user_data` field when the timer event callback is called, attempted
use of that pointer will likely cause a crash because the pointer is not valid any
more.  However, a timely dump of the Widget Tree right before that point will show
that the Widget no longer exists.


Find child by ID
----------------

.. warning::
  ``lv_obj_find_by_id(widget, id)`` is deprecated. To find a widget use ``obj_name``.

:cpp:expr:`lv_obj_find_by_id(widget, id)` will perform a recursive walk through
``widget``\ 's children and return the first child encountered having the given ID.
