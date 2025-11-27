.. _indev_button:

===============
Hardware Button
===============



Overview
********

A *Hardware Button* is an external button typically located next to the screen and
assigned to specific coordinates on the screen. If a button is pressed, it simulates a
press at the assigned coordinate, similar to a touchpad.

To assign Hardware Buttons to coordinates, use
:cpp:expr:`lv_indev_set_button_points(my_indev, points_array)`.

``points_array`` should look like this:

.. code-block:: c

    static const lv_point_t points_array[] = {
        {12,30}, /* First button is assigned to x=12; y=30 */
        {60,90}  /* Second button is assigned to x=60; y=90 */
    };

.. admonition:: Important:

    ``points_array`` must not go out of scope. Declare it either as a global variable
    or as a static variable inside a function.

The index of the pressed button should be set in ``data->btn_id`` in the ``read_cb``.

``data->state`` should also be set to either ``LV_INDEV_STATE_PRESSED`` or
``LV_INDEV_STATE_RELEASED``.



Example
*******

Here is a complete example:

.. code-block:: c

   static const lv_point_t points_array[] = {
       {12,30}, /* First button is assigned to x=12; y=30 */
       {60,90}  /* Second button is assigned to x=60; y=90 */
   };

   lv_indev_t * indev = lv_indev_create();
   lv_indev_set_type(indev, LV_INDEV_TYPE_BUTTON);
   lv_indev_set_button_points(indev, points_array);
   lv_indev_set_read_cb(indev, button_read);

   ...

   void button_read(lv_indev_t * indev, lv_indev_data_t * data)
   {
       /* Get the ID (0,1,2...) of the pressed button.
        * Let's say it returns -1 if no button was pressed */
       int btn_pr = my_btn_read();

       /* Is there a button press? */
       if(btn_pr >= 0) {
          data->btn_id = btn_pr;           /* Save the ID of the pressed button */
          data->state = LV_INDEV_STATE_PRESSED;  /* Set the pressed state */
       } else {
          data->state = LV_INDEV_STATE_RELEASED; /* Set the released state */
       }
   }
