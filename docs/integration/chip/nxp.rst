===
NXP
===

NXP has integrated LVGL into the MCUXpresso SDK packages for several of our
microcontrollers as an optional software component, allowing easy evaluation and
migration into your product design. LVGL is a free and open-source embedded
graphic library with features that enable you need to create embedded GUIs with
intuitive graphical elements, beautiful visual effects and a low memory
footprint. The complete graphic framework includes a variety of widgets for you
to use in the creation of your GUI, and supports more advanced functions such as
animations and anti-aliasing.

LVGL enables graphics in our free GUI Guider UI tool. It's available for use
with NXP’s general purpose and crossover microcontrollers, providing developers
with a tool for creating complete, high quality GUI applications with LVGL.

Creating new project with LVGL
------------------------------

`Download an SDK for a supported board <https://www.nxp.com/design/software/embedded-software/littlevgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY?&tid=vanLITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY>`__
today and get started with your next GUI application. It comes fully configured
with LVGL (and with PXP/VGLite support if the modules are present), no
additional integration work is required.

HW acceleration for NXP iMX RT platforms
----------------------------------------

Depending on the RT platform used, the acceleration can be done by NXP PXP
(PiXel Pipeline) and/or the Verisilicon GPU through an API named VGLite. Each
accelerator has its own context that allows them to be used individually as well
simultaneously (in LVGL multithreading mode).

PXP accelerator
~~~~~~~~~~~~~~~
Basic configuration:
^^^^^^^^^^^^^^^^^^^^

- Select NXP PXP engine in "lv_conf.h": Set :c:macro:`LV_USE_DRAW_PXP` to `1`.
- Enable PXP asserts in "lv_conf.h": Set :c:macro: `LV_USE_PXP_ASSERT` to `1`.
  There are few PXP assertions that can stop the program execution in case the
  c:macro: `LV_ASSERT_HANDLER` is set to `while(1);` (Halt by default). Else,
  there will be logged just an error message via `LV_LOG_ERROR`.
- If :c:macro:`SDK_OS_FREE_RTOS` symbol is defined, FreeRTOS implementation
  will be used, otherwise bare metal code will be included.

Basic initialization:
^^^^^^^^^^^^^^^^^^^^^

PXP draw initialization is done automatically in :cpp:func:`lv_init()` once the
PXP is enabled, no user code is required:

.. code:: c

  #if LV_USE_DRAW_PXP
    lv_draw_pxp_init();
  #endif

During PXP initialization, a new draw unit `lv_draw_pxp_unit_t` will be created
with the additional callbacks:

.. code:: c

    lv_draw_pxp_unit_t * draw_pxp_unit = lv_draw_create_unit(sizeof(lv_draw_pxp_unit_t));
    draw_pxp_unit->base_unit.evaluate_cb = _pxp_evaluate;
    draw_pxp_unit->base_unit.dispatch_cb = _pxp_dispatch;
    draw_pxp_unit->base_unit.delete_cb = _pxp_delete;

and an addition thread `_pxp_render_thread_cb()` will be spawned in order to
handle the supported draw tasks.

.. code:: c

  #if LV_USE_OS
    lv_thread_init(&draw_pxp_unit->thread, LV_THREAD_PRIO_HIGH, _pxp_render_thread_cb, 2 * 1024, draw_pxp_unit);
  #endif

If `LV_USE_OS` is not defined, then no additional draw thread will be created
and the PXP drawing task will get executed on the same LVGL main thread.

`_pxp_evaluate()` will get called after each task is being created and will
analyze if the task is supported by PXP or not. If it is supported, then an
preferred score and the draw unit id will be set to the task. An `score` equal
to `100` is the default CPU score. Smaller score means that PXP is capable of
drawing it faster.

`_pxp_dispatch()` is the PXP dispatcher callback, it will take a ready to draw
task (having the `DRAW_UNIT_ID_PXP` set) and will pass the task to the PXP draw
unit for processing.

`_pxp_delete()` will cleanup the PXP draw unit.

Features supported:
^^^^^^^^^^^^^^^^^^^

Several drawing features in LVGL can be offloaded to the PXP engine. The CPU is
available for other operations while the PXP is running. RTOS is required to
block the LVGL drawing thread and switch to another task or suspend the CPU for
power savings.

Supported draw tasks are available in "src/draw/nxp/pxp/lv_draw_pxp.c":

.. code:: c

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_pxp_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_pxp_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_pxp_layer(draw_unit, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }

Additional, the screen rotation can be handled by the PXP:

.. code::c

  void lv_draw_pxp_rotate(const void * src_buf, void * dest_buf, int32_t src_width, int32_t src_height,
                          int32_t src_stride, int32_t dest_stride, lv_display_rotation_t rotation,
                          lv_color_format_t cf);

- Fill area with color (w/o radius, w/o gradient) + optional opacity.
- Blit source image RGB565/ARGB888/XRGB8888 over destination.
  RGB565/RGB888/ARGB888/XRGB8888 + optional opacity.
- Recolor source image RGB565.
- Scale and rotate (90, 180, 270 degree) source image RGB565.
- Blending layers (w/ same supported formats as blitting).
- Rotate screen (90, 180, 270 degree).

Known limitations:
^^^^^^^^^^^^^^^^^^

- PXP can only rotate at 90x angles.
- Rotation is not supported for images unaligned to blocks of 16x16 pixels. PXP
  is set to process 16x16 blocks to optimize the system for memory bandwidth and
  image processing time. The output engine essentially truncates any output
  pixels after the desired number of pixels has been written. When rotating a
  source image and the output is not divisible by the block size, the incorrect
  pixels could be truncated and the final output image can look shifted.
- Recolor or transformation for images w/ opacity or alpha channel can't be
  obtained in a single PXP pipeline configuration. Two or multiple steps would
  be required.
- Buffer address must be aligned to 64 bytes: set :c:macro:`LV_DRAW_BUF_ALIGN`
  to `64` in "lv_conf.h".
  No stride alignment is required: set :c:macro:`LV_DRAW_BUF_STRIDE_ALIGN` to
  `1` in "lv_conf.h".

Project setup:
^^^^^^^^^^^^^^

- Add PXP related source files (and corresponding headers if available) to
  project:

   - "src/draw/nxp/pxp/lv_draw_buf_pxp.c": draw buffer callbacks
   - "src/draw/nxp/pxp/lv_draw_pxp_fill.c": fill area
   - "src/draw/nxp/pxp/lv_draw_pxp_img.c": blit image (w/ optional recolor or
   transformation)
   - "src/draw/nxp/pxp/lv_draw_pxp_layer.c": layer blending
   - "src/draw/nxp/pxp/lv_draw_pxp.c": draw unit initialization
   - "src/draw/nxp/pxp/lv_pxp_cfg.c": init, deinit, run/wait PXP device
   - "src/draw/nxp/pxp/lv_pxp_osa.c": OS abstraction (FreeRTOS or bare metal)
   - "src/draw/nxp/pxp/lv_pxp_utils.c": function helpers

- PXP related code depends on two drivers provided by MCU SDK. These drivers
  need to be added to project:

   - fsl_pxp.c: PXP driver
   - fsl_cache.c: CPU cache handling functions

PXP default configuration:
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Implementation depends on multiple OS-specific functions. The struct
  :cpp:struct:`pxp_cfg_t` with callback pointers is used as a parameter for the
  :cpp:func:`lv_pxp_init()` function. Default implementation for FreeRTOS and
  bare metal is provided in lv_pxp_osa.c.

   - :cpp:func:`pxp_interrupt_init()`: Initialize PXP interrupt (HW setup,
   OS setup)
   - :cpp:func:`pxp_interrupt_deinit()`: Deinitialize PXP interrupt (HW setup,
   OS setup)
   - :cpp:func:`pxp_run()`: Start PXP job. Use OS-specific mechanism to block
   drawing thread.
   - :cpp:func:`pxp_wait()`: Wait for PXP completion.

VGLite accelerator
~~~~~~~~~~~~~~~~~~

Extra drawing features in LVGL can be handled by the VGLite engine. The
CPU is available for other operations while the VGLite is running. An
RTOS is required to block the LVGL drawing thread and switch to another
task or suspend the CPU for power savings.

Basic configuration:
^^^^^^^^^^^^^^^^^^^^

- Select NXP VGLite engine in "lv_conf.h": Set :c:macro:`LV_USE_DRAW_VGLITE` to
  `1`. :c:macro:`SDK_OS_FREE_RTOS` symbol needs to be defined so that FreeRTOS
  driver osal implementation will be enabled.
- Enable VGLite asserts in "lv_conf.h": Set :c:macro: `LV_USE_VGLITE_ASSERT` to
  `1`.
  VGLite assertions will verify the driver API status code and in any error, it
  can stop the program execution in case the c:macro: `LV_ASSERT_HANDLER` is set
  to `while(1);` (Halt by default). Else, there will be logged just an error
  message via `LV_LOG_ERROR`.

Basic initialization:
^^^^^^^^^^^^^^^^^^^^^

Initialize VGLite GPU before calling :cpp:func:`lv_init()` by specifying the
width/height of tessellation window. The default values for tesselation width
and height, and command buffer size are in the SDK file "vglite_support.h".

.. code:: c

  #if LV_USE_GPU_NXP_VG_LITE
    #include "vg_lite.h"
    #include "vglite_support.h"
  #endif
    ...
  #if LV_USE_DRAW_VGLITE
    if(vg_lite_init(DEFAULT_VG_LITE_TW_WIDTH, DEFAULT_VG_LITE_TW_HEIGHT) != VG_LITE_SUCCESS)
    {
        PRINTF("VGLite init error. STOP.");
        vg_lite_close();
        while (1)
            ;
    }

    if (vg_lite_set_command_buffer_size(VG_LITE_COMMAND_BUFFER_SIZE) != VG_LITE_SUCCESS)
    {
        PRINTF("VGLite set command buffer. STOP.");
        vg_lite_close();
        while (1)
            ;
    }
  #endif

VGLite draw initialization is done automatically in :cpp:func:`lv_init()` once
the VGLite is enabled, no user code is required:

.. code:: c

  #if LV_USE_DRAW_VGLITE
    lv_draw_vglite_init();
  #endif

During VGLite initialization, a new draw unit `lv_draw_vglite_unit_t` will be
created with the additional callbacks:

.. code:: c

    lv_draw_vglite_unit_t * draw_vglite_unit = lv_draw_create_unit(sizeof(lv_draw_vglite_unit_t));
    draw_vglite_unit->base_unit.evaluate_cb = _vglite_evaluate;
    draw_vglite_unit->base_unit.dispatch_cb = _vglite_dispatch;
    draw_vglite_unit->base_unit.delete_cb = _vglite_delete;

and an addition thread `_vglite_render_thread_cb()` will be spawned in order to
handle the supported draw tasks.

.. code:: c

  #if LV_USE_OS
    lv_thread_init(&draw_vglite_unit->thread, LV_THREAD_PRIO_HIGH, _vglite_render_thread_cb, 2 * 1024, draw_vglite_unit);
  #endif

If `LV_USE_OS` is not defined, then no additional draw thread will be created
and the VGLite drawing task will get executed on the same LVGL main thread.

`_vglite_evaluate()` will get called after each task is being created and will
analyze if the task is supported by VGLite or not. If it is supported, then an
preferred score and the draw unit id will be set to the task. An `score` equal
to `100` is the default CPU score. Smaller score means that VGLite is capable of
drawing it faster.

`_vglite_dispatch()` is the VGLite dispatcher callback, it will take a ready to
draw task (having the `DRAW_UNIT_ID_VGLITE` set) and will pass the task to the
VGLite draw unit for processing.

`_vglite_delete()` will cleanup the VGLite draw unit.

Advanced configuration:
^^^^^^^^^^^^^^^^^^^^^^^

- Enable VGLite blit split in "lv_conf.h":
  Set :c:macro: `LV_USE_VGLITE_BLIT_SPLIT` to `1`.
  Enabling the blit split workaround will mitigate any quality degradation issue
  on screen's dimension > 352 pixels.

.. code:: c

  #define VGLITE_BLIT_SPLIT_THR 352

- By default, the blit split threshold is set to 352. Blits with width or height
  higher than this value will be done in multiple steps. Value must be multiple
  of stride alignment in px. For most color formats, the alignment is 16px
  (except the index formats). Transformation will not be supported once with
  the blit split.

- Enable VGLite draw task synchronously in "lv_conf.h":
  Set :c:macro: `LV_USE_VGLITE_DRAW_ASYNC` to `1`.
  Multiple draw tasks can be queued and flushed them once to the GPU based on
  the GPU idle status. If GPU is busy, the task will be queued, and the VGLite
  dispatcher will ask for a new available task. If GPU is idle, the queue with
  any pending tasks will be flushed to the GPU. The completion status of draw
  task will be sent to the main LVGL thread asynchronously.

Features supported:
^^^^^^^^^^^^^^^^^^^

Several drawing features in LVGL can be offloaded to the VGLite engine. The CPU
is available for other operations while the GPU is running. RTOS is required to
block the LVGL drawing thread and switch to another task or suspend the CPU for
power savings.

Supported draw tasks are available in "src/draw/nxp/pxp/lv_draw_vglite.c":

.. code:: c

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_vglite_label(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_vglite_fill(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_vglite_border(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_vglite_img(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_vglite_arc(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_vglite_line(draw_unit, t->draw_dsc);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_vglite_layer(draw_unit, t->draw_dsc, &t->area);
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_vglite_triangle(draw_unit, t->draw_dsc);
            break;
        default:
            break;
    }

All the below opration can be done in addition with optional opacity.
- Fill area with color (w/ radius or gradient).
- Blit source image (any format from `_vglite_src_cf_supported()`) over
  destination (any format from `_vglite_dest_cf_supported()`).
- Recolor source image.
- Scale and rotate (any decimal degree) source image.
- Blending layers (w/ same supported formats as blitting).
- Draw letters (blit bitmap letters - raster font).
- Draw full borders (LV_BORDER_SIDE_FULL).
- Draw arcs (w/ rounded edges).
- Draw lines (w/ dash or rounded edges).
- Draw triangles with color (w/ gradient).

Known limitations:
^^^^^^^^^^^^^^^^^^

- Source image alignment: The byte alignment requirement for a pixel depends on
  the specific pixel format. Both buffer address and buffer stride must be
  aligned. As general rule, the alignment is set to 16 pixels. This makes the
  buffer address alignment to be 32 bytes for RGB565 and 64 bytes for ARGB8888.
- For pixel engine (PE) destination, the alignment should be 64 bytes for all
  tiled (4x4) buffer layouts. The pixel engine has no additional alignment
  requirement for linear buffer layouts (:c:macro:`VG_LITE_LINEAR`).

Project setup:
^^^^^^^^^^^^^^

- Add VGLite related source files (and corresponding headers if available) to
  project:

   - "src/draw/nxp/vglite/lv_draw_buf_vglite.c": draw buffer callbacks
   - "src/draw/nxp/vglite/lv_draw_vglite_arc.c": draw arc
   - "src/draw/nxp/vglite/lv_draw_vglite_border.c": draw border
   - "src/draw/nxp/vglite/lv_draw_vglite_fill.c": fill area
   - "src/draw/nxp/vglite/lv_draw_vglite_img.c": blit image (w/ optional
   recolor or transformation)
   - "src/draw/nxp/vglite/lv_draw_vglite_label.c": draw label
   - "src/draw/nxp/vglite/lv_draw_vglite_layer.c": layer blending
   - "src/draw/nxp/vglite/lv_draw_vglite_line.c": draw line
   - "src/draw/nxp/vglite/lv_draw_vglite_triangle.c": draw triangle
   - "src/draw/nxp/vglite/lv_draw_vglite.c": draw unit initialization
   - "src/draw/nxp/vglite/lv_vglite_buf.c": init/get vglite buffer
   - "src/draw/nxp/vglite/lv_vglite_matrix.c": set vglite matrix
   - "src/draw/nxp/vglite/lv_vglite_path.c": create vglite path data
   - "src/draw/nxp/vglite/lv_vglite_utils.c": function helpers
