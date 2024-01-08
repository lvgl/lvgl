.. _obj_id:

======
OBJ ID
======

LVGL provides an optional field in :cpp:type:`lv_obj_t` to store the object ID.
Object ID can be used in many cases, for example, to identify the object.
Or we can store a program backtrace to where the object is created.

.. _obj_id_usage:

Usage
-----

Enable this feature by setting :c:macro:`LV_USE_OBJ_ID` to `1` in ``lv_conf.h``.
Use the builtin obj ID generator by setting :c:macro:`LV_USE_OBJ_ID_BUILTIN` to `1`.
Otherwise provide your own custom implementation.

The ID is automatically generated and assigned to :cpp:expr:`obj->id` during obj's
construction by calling API :cpp:expr:`lv_obj_assign_id(obj)` from :cpp:func:`lv_obj_constructor`.

You can directly access the ID by :cpp:expr:`obj->id` or use API :cpp:expr:`lv_obj_stringify_id(obj, buf, len)`
to get a string representation of the ID.

Use custom ID generator
~~~~~~~~~~~~~~~~~~~~~~~

Set :c:macro:`LV_USE_OBJ_ID_BUILTIN` to `0` in ``lv_conf.h``. 

Below APIs needed to be implemented and linked to lvgl.

.. code:: c

    void lv_obj_assign_id(const lv_obj_class_t * class_p, lv_obj_t * obj);
    void lv_obj_free_id(lv_obj_t * obj);
    const char * lv_obj_stringify_id(lv_obj_t * obj, char * buf, uint32_t len);


:cpp:func:`lv_obj_assign_id` is called when an object is created. The object final class is passed from
parameter ``class_p``. Note it may be different than :cpp:expr:`obj->class_p` which is the class
currently being constructed.

:cpp:func:`lv_obj_free_id` is called when object is deconstructed. Free any resource allocated in :cpp:func:`lv_obj_assign_id`.

:cpp:func:`lv_obj_stringify_id` converts id to a string representation. The string is stored in ``buf``.

Dump obj tree
~~~~~~~~~~~~~

Use API :cpp:expr:`lv_obj_dump_tree(lv_obj_t * obj, int depth)` to dump the object tree.
It will walk through all children and print the object ID together with object address.

This is useful to debug UI crash. From log we can rebuilt UI the moment before crash.
For example, if the obj is stored to a :cpp:expr:`timer->user_data`, but obj is deleted when timer expired.
Timer callback will crash because of accessing wild pointer.
From the dump log we can clearly see that the obj does not exist.
