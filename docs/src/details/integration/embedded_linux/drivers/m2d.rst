.. _m2d_driver:

===
M2D
===

Overview
--------

The **M2D** draw unit can accelerate some LVGL drawing tasks using the libm2d API also offloading the CPU.
Indeed, libm2d is a Linux software library in user-space that abstracts some 2D GPUs by providing graphics applications or frameworks like LVGL with a common API.
libm2d builds GPU commands then relies on libdrm and the DRM subsystem of the Linux kernel to send those commands to the GPU hardware.

M2D is an open-source project on GitHub. For more, please refer to:
https://github.com/linux4sam/libm2d

Getting Started with M2D
------------------------

Prerequisites
~~~~~~~~~~~~~

The M2D draw unit requires:

1. The memory behind any draw buffer to be allocated by the Linux DRM subsystem as GEM objects.
2. Display drivers, like the DRM driver, allocating themselves their framebuffers to first export the GEM objects behind to get the associated GEM PRIME file descriptors then to call for each GEM PRIME file descriptor the `import_cb` callback in the :cpp:type:`_lv_draw_buf_handlers_t` returned by :cpp:func:`lv_draw_buf_get_handlers` so the GEM objects of the framebuffers are finally imported by the M2D draw unit.

Configure M2D Draw Unit
~~~~~~~~~~~~~~~~~~~~~~~

1. Required linked librarires: -lm2d
2. Enable the M2D draw unit support in lv_conf.h, by cmake compiler define or by KConfig

.. code-block:: c

    #define LV_USE_DRAW_M2D 1

Basic initialization
~~~~~~~~~~~~~~~~~~~~

The initialization of the M2D draw unit is done automatically in :cpp:func:`lv_init`.
Once done, no user code is required:

.. code:: c

    #if LV_USE_DRAW_M2D
        lv_draw_m2d_init();
    #endif

:cpp:func:`lv_draw_m2d_init` creates a new :cpp:type:`_lv_draw_m2d_unit_t` draw
unit with additional callbacks.

.. code:: c

    lv_draw_m2d_unit_t * draw_m2d_unit = lv_draw_create_unit(sizeof(lv_draw_m2d_unit_t));
    draw_m2d_unit->base_unit.evaluate_cb = m2d_evaluate;
    draw_m2d_unit->base_unit.dispatch_cb = m2d_dispatch;
    draw_m2d_unit->base_unit.name = "M2D";

Then a :cpp:type:`_lv_map_t` container is also initialized to associate the `data`
pointer of some :cpp:type:`lv_image_dsc_t` with a dynamically created :cpp:type:`_lv_draw_m2d_buf_t`.
Each :cpp:type:`_lv_draw_m2d_buf_t` instance wrapping a `struct m2d_buffer` from
libm2d, any image could then be used as an input surface for some GPU operation,
like copy or blending into the target framebuffer.

.. code:: c

    lv_draw_m2d_init_buf_map();

Finally, the libm2d library itself is initialized.

.. code:: c

    if(m2d_init()) {
        LV_LOG_ERROR("m2d_init() failed");
        return;
    }

Supported Features
------------------

Several drawing features in LVGL can be offloaded; the CPU is then available for
other operations while the GPU is running.

Supported draw tasks are available in "src/draw/m2d/lv_draw_m2d.c"

.. code:: c

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_m2d_fill(t, t->draw_dsc, &area);
            break;
        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_m2d_image(t, t->draw_dsc, &area);
            break;
        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_m2d_layer(t, t->draw_dsc, &area);
            break;
        default:
            break;
    }
