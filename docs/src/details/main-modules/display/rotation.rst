.. _display_rotation:

========
Rotation
========

LVGL supports rotation of the display in 90 degree increments.

The orientation of the display can be changed with
:cpp:expr:`lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_xxx)` where ``xxx`` is
0, 90, 180 or 270.  This will swap the horizontal and vertical resolutions internally
according to the set degree, however it will not perform the actual rotation.
When changing the rotation, the :cpp:enumerator:`LV_EVENT_SIZE_CHANGED` event is
emitted to allow for hardware reconfiguration.  If your display panel and/or its
driver chip(s) do not support rotation, :cpp:func:`lv_draw_sw_rotate` can be used to
rotate the buffer in the :ref:`flush_callback` function.

:cpp:expr:`lv_display_rotate_area(display, &area)` rotates the rendered area
according to the current rotation settings of the display.

Note that in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` the small changed areas
are rendered directly in the frame buffer so they cannot be
rotated later. Therefore in direct mode only the whole frame buffer can be rotated.

In the case of :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` the small rendered areas
can be rotated on their own before flushing to the frame buffer.

:cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL` can work with rotation if the buffer(s)
being rendered to are different than the buffer(s) being rotated to in the flush callback
and the buffers being rendered to do not have a stride requirement.

Below is an example for rotating when the rendering mode is
:cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` and the rotated image should be sent to a
**display controller**.

.. code-block:: c

    /*Rotate a partially rendered area to another buffer and send it*/
    void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
    {
        lv_display_rotation_t rotation = lv_display_get_rotation(disp);
        lv_area_t rotated_area;
        if(rotation != LV_DISPLAY_ROTATION_0) {
            lv_color_format_t cf = lv_display_get_color_format(disp);
            /*Calculate the position of the rotated area*/
            rotated_area = *area;
            lv_display_rotate_area(disp, &rotated_area);
            /*Calculate the source stride (bytes in a line) from the width of the area*/
            uint32_t src_stride = lv_draw_buf_width_to_stride(lv_area_get_width(area), cf);
            /*Calculate the stride of the destination (rotated) area too*/
            uint32_t dest_stride = lv_draw_buf_width_to_stride(lv_area_get_width(&rotated_area), cf);
            /*Have a buffer to store the rotated area and perform the rotation*/
            static uint8_t rotated_buf[500*1014];
            int32_t src_w = lv_area_get_width(area);
            int32_t src_h = lv_area_get_height(area);
            lv_draw_sw_rotate(px_map, rotated_buf, src_w, src_h, src_stride, dest_stride, rotation, cf);
            /*Use the rotated area and rotated buffer from now on*/
            area = &rotated_area;
            px_map = rotated_buf;
        }
        my_set_window(area->x1, area->y1, area->x2, area->y2);
        my_send_colors(px_map);
    }

Below is an example for rotating when the rendering mode is
:cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL` and the image can be rotated directly
into a **frame buffer of the LCD peripheral**.

.. code-block:: c

    /*Rotate a partially rendered area to the frame buffer*/
    void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
    {
        lv_color_format_t cf = lv_display_get_color_format(disp);
        uint32_t px_size = lv_color_format_get_size(cf);
        /*Calculate the position of the rotated area*/
        lv_area_t rotated_area = *area;
        lv_display_rotate_area(disp, &rotated_area);
        /*Calculate the properties of the source buffer*/
        int32_t src_w = lv_area_get_width(area);
        int32_t src_h = lv_area_get_height(area);
        uint32_t src_stride = lv_draw_buf_width_to_stride(src_w, cf);
        /*Calculate the properties of the frame buffer*/
        int32_t fb_stride = lv_draw_buf_width_to_stride(disp->hor_res, cf);
        uint8_t * fb_start = my_fb_address;
        fb_start += rotated_area.y1 * fb_stride + rotated_area.x1 * px_size;
        lv_display_rotation_t rotation = lv_display_get_rotation(disp);
        if(rotation == LV_DISPLAY_ROTATION_0) {
            int32_t y;
            for(y = area->y1; y <= area->y2; y++) {
                lv_memcpy(fb_start, px_map, src_stride);
                px_map += src_stride;
                fb_start += fb_stride;
            }
        }
        else {
            lv_draw_sw_rotate(px_map, fb_start, src_w, src_h, src_stride, fb_stride, rotation, cf);
        }
    }



API
***

.. API equals:  lv_display_set_rotation
