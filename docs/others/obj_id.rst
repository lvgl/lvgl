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

Enable :c:macro:`LV_OBJ_ID_AUTO_ASSIGN` to automatically assign an ID to object when it's created.
It's done by calling function :cpp:func:`lv_obj_assign_id` from :cpp:func:`lv_obj_constructor`.

You can either use your own ID generator by defining the function :cpp:func:`lv_obj_assign_id` or you can utilize the built-in one.
To use the builtin ID generator, set :c:macro:`LV_USE_OBJ_ID_BUILTIN` to `1`.

You can directly access the ID by :cpp:expr:`lv_obj_get_id(obj)` or use API :cpp:expr:`lv_obj_stringify_id(obj, buf, len)`
to get a string representation of the ID.

Use custom ID generator
~~~~~~~~~~~~~~~~~~~~~~~

Set :c:macro:`LV_USE_OBJ_ID_BUILTIN` to `0` in ``lv_conf.h``.

Below APIs needed to be implemented and linked to lvgl.

.. code:: c

    void lv_obj_set_id(lv_obj_t * obj, void * id);
    void lv_obj_assign_id(const lv_obj_class_t * class_p, lv_obj_t * obj);
    void lv_obj_free_id(lv_obj_t * obj);
    const char * lv_obj_stringify_id(lv_obj_t * obj, char * buf, uint32_t len);
    int lv_obj_id_compare(void * id1, void * id2);


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

Find child by ID
~~~~~~~~~~~~~~~~

Use API :cpp:expr:`lv_obj_t * lv_obj_get_child_by_id(const lv_obj_t * obj, void * id);` to find a child by ID.
It will walk through all children and return the first child with the given ID.
