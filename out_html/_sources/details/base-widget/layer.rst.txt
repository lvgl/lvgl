.. _layers:

======
Layers
======

When the term "layer" is used in LVGL documentation, it may refer to one of several
things:

1.  for Widgets, the :ref:`layers_creation` creates a natural layering of Widgets;
2.  in the context of pixel rendering (drawing), there are :ref:`draw_layers`;
3.  permanent :ref:`screen_layers` are part of each :ref:`display` object, and
    are covered :ref:`here <screen_layers>`

#1 and #2 are covered below.



.. _layers_creation:

Order of Creation
*****************

By default, LVGL draws new Widgets on top of old Widgets.

For example, assume we add a button to a parent Widget named button1 and
then another button named button2. Then button1 (along with its child
Widget(s)) will be in the background and can be covered by button2 and
its children.

.. image:: /misc/layers.png

.. code-block:: c

   /* Create a screen */
   lv_obj_t * scr = lv_obj_create(NULL);
   lv_screen_load(scr);          /* Load the screen */

   /* Create 2 buttons */
   lv_obj_t * btn1 = lv_button_create(scr);     /* Create the first button on the screen */
   lv_obj_set_pos(btn1, 60, 40);                      /* Set the position of the first button */

   lv_obj_t * btn2 = lv_button_create(scr);     /* Create the second button on the screen */
   lv_obj_set_pos(btn2, 180, 80);                     /* Set the position of the second button */

   /* Add labels to the buttons */
   lv_obj_t * label1 = lv_label_create(btn1);   /* Create a label on the first button */
   lv_label_set_text(label1, "Button 1");             /* Set the text of the label */

   lv_obj_t * label2 = lv_label_create(btn2);   /* Create a label on the second button */
   lv_label_set_text(label2, "Button 2");             /* Set the text of the label */

   /* Delete the second label */
   lv_obj_delete(label2);

.. _layers_order:

Changing Order
--------------

There are four explicit ways to bring a Widget to the foreground:

- Use :cpp:expr:`lv_obj_move_foreground(widget)` to bring a Widget to the foreground.
  Similarly, use :cpp:expr:`lv_obj_move_background(widget)` to move it to the background.
- Use :cpp:expr:`lv_obj_move_to_index(widget, idx)` to move a Widget to a given index in the order of children.

  - ``0``: background
  - ``child_num - 1``: foreground
  - ``< 0``: count from the top, to move forward (up): :cpp:expr:`lv_obj_move_to_index(widget, lv_obj_get_index(widget) - 1)`

- Use :cpp:expr:`lv_obj_swap(widget1, widget2)` to swap the relative layer position of two Widgets.
- When :cpp:expr:`lv_obj_set_parent(widget, new_parent)` is used, ``widget`` will be on the foreground of ``new_parent``.


.. _draw_layers:

Draw Layers
***********

Some style properties cause LVGL to allocate a buffer and render a Widget and its
children there first.  Later that layer will be merged to the screen or its parent
layer after applying some transformations or other modifications.

Simple Layer
------------

The following style properties trigger the creation of a "Simple Layer":

- ``opa_layered``
- ``bitmap_mask_src``
- ``blend_mode``

In this case the Widget will be sliced into ``LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE``
sized chunks.

If there is no memory for a new chunk, LVGL will try allocating the layer after
another chunk is rendered and freed.

Transformed Layer
-----------------

When the widget is transformed a larger part of the Widget needs to rendered to
provide enough data for transformation.  LVGL tries to render as small area of the
widget as possible, but due to the nature of transformations no slicing is possible
in this case.

The following style properties trigger the creation of a "Transform Layer":

- ``transform_scale_x``
- ``transform_scale_y``
- ``transform_skew_x``
- ``transform_skew_y``
- ``transform_rotate``

Clip corner
-----------

The ``clip_corner`` style property also causes LVGL to create a 2 layers with radius
height for the top and bottom parts of the Widget.


.. _layers_api:

API
***
