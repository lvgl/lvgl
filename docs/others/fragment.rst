.. _fragment:

========
Fragment
========

Fragment is a concept copied from
`Android <https://developer.android.com/guide/fragments>`__.

It represents a reusable portion of your app's UI. A fragment defines
and manages its own layout, has its own lifecycle, and can handle its
own events. Like Android's Fragment that must be hosted by an activity
or another fragment, Fragment in LVGL needs to be hosted by an object,
or another fragment. The fragment's view hierarchy becomes part of, or
attaches to, the host's view hierarchy.

Such concept also has some similarities to `UiViewController on
iOS <https://developer.apple.com/documentation/uikit/uiviewcontroller>`__.

Fragment Manager is a manager holding references to fragments attached
to it, and has an internal stack to achieve navigation. You can use
fragment manager to build navigation stack, or multi pane application
easily.

.. _fragment_usage:

Usage
-----

Enable :c:macro:`LV_USE_FRAGMENT` in ``lv_conf.h``.

Create Fragment Class
~~~~~~~~~~~~~~~~~~~~~

.. code:: c

   struct sample_fragment_t {
       /* IMPORTANT: don't miss this part */
       lv_fragment_t base;
       /* States, object references and data fields for this fragment */
       const char *title;
   };

   const lv_fragment_class_t sample_cls = {
           /* Initialize something needed */
           .constructor_cb = sample_fragment_ctor,
           /* Create view objects */
           .create_obj_cb = sample_fragment_create_obj,
           /* IMPORTANT: size of your fragment struct */
           .instance_size = sizeof(struct sample_fragment_t)
   };

Use ``lv_fragment_manager``
~~~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

   /* Create fragment instance, and objects will be added to container */
   lv_fragment_manager_t *manager = lv_fragment_manager_create(container, NULL);
   /* Replace current fragment with instance of sample_cls, and init_argument is user defined pointer */
   lv_fragment_manager_replace(manager, &sample_cls, init_argument);

Fragment Based Navigation
~~~~~~~~~~~~~~~~~~~~~~~~~

.. code:: c

   /* Add one instance into manager stack. View object of current fragment will be destroyed,
    * but instances created in class constructor will be kept.
    */
   lv_fragment_manager_push(manager, &sample_cls, NULL);

   /* Remove the top most fragment from the stack, and bring back previous one. */
   lv_fragment_manager_pop(manager);

.. _fragment_example:

Example
-------

.. include:: ../examples/others/fragment/index.rst

.. _fragment_api:

API
---

