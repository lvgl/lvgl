.. _display_tiling:

===============
Tiled Rendering
===============

When multiple CPU cores are available and a large area needs to be redrawn, LVGL must
identify independent areas that can be rendered in parallel.

For example, if there are 4 CPU cores, one core can draw the screen's background
while the other 3 must wait until it is finished. If there are 2 buttons on the
screen, those 2 buttons can be rendered in parallel, but 2 cores will still remain
idle.

Due to dependencies among different areas, CPU cores cannot always be fully utilized.

To address this, LVGL can divide large areas that need to be updated into smaller
tiles. These tiles are independent, making it easier to find areas that can be
rendered concurrently.

Specifically, if there are 4 tiles and 4 cores, there will always be an independent
area for each core within one of the tiles.

The maximum number of tiles can be set using the function
:cpp:expr:`lv_display_set_tile_cnt(disp, cnt)`. The default value is
:cpp:expr:`LV_DRAW_SW_DRAW_UNIT_CNT` (or 1 if software rendering is not enabled).

Small areas are not further divided into smaller tiles because the overhead of
spinning up 4 cores would outweigh the benefits.

The ideal tile size is calculated as ``ideal_tile_size = draw_buf_size / tile_cnt``.
For example, in :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT` mode on an 800x480
screen, the display buffer is 800x480 = 375k pixels. If there are 4 tiles, the ideal
tile size is approximately 93k pixels. Based on this, core utilization is as follows:

- 30k pixels: 1 core
- 90k pixels: 1 core
- 95k pixels: 2 cores (above 93k pixels, 2 cores are used)
- 150k pixels: 2 cores
- 200k pixels: 3 cores (above 186k pixels, 3 cores are used)
- 300k pixels: 4 cores (above 279k pixels, 4 cores are used)
- 375k pixels: 4 cores

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_DIRECT`, the screen-sized draw buffer is
divided by the tile count to determine the ideal tile sizes. If smaller areas are
refreshed, it may result in fewer cores being used.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_FULL`, the maximum number of tiles is
always created when the entire screen is refreshed.

In :cpp:enumerator:`LV_DISPLAY_RENDER_MODE_PARTIAL`, the partial buffer is divided
into tiles. For example, if the draw buffer is 1/10th the size of the screen and
there are 2 tiles, then 1/20th + 1/20th of the screen area will be rendered at once.

Tiled rendering only affects the rendering process, and the :ref:`flush_callback` is
called once for each invalidated area. Therefore, tiling is not visible from the
flushing point of view.



API
***

.. API equals:  lv_display_set_tile_cnt, LV_DISPLAY_RENDER_MODE_FULL
