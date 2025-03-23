.. _display_redraw_area:

===========================
Constraints on Redrawn Area
===========================

Some display controllers have specific requirements for the window area where the
rendered image can be sent (e.g., `x1` must be even, and `x2` must be odd).

In the case of monochrome displays, `x1` must be `Nx8`, and `x2` must be `Nx8 - 1`.
(If the display uses `LV_COLOR_FORMAT_I1`, LVGL automatically applies this rounding.
See :ref:`display_monochrome`.)

The size of the invalidated (redrawn) area can be controlled as follows:

.. code-block:: c

    void rounder_event_cb(lv_event_t * e)
    {
        lv_area_t * a = lv_event_get_invalidated_area(e);

        a->x1 = a->x1 & (~0x1); /* Ensure x1 is even */
        a->x2 = a->x2 | 0x1;    /* Ensure x2 is odd */
    }

    ...

    lv_display_add_event_cb(disp, rounder_event_cb, LV_EVENT_INVALIDATE_AREA, NULL);



API
***

.. API equals:
    lv_event_get_invalidated_area
