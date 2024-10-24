.. _vg_lite:

===================
VG-Lite General GPU
===================

This is a generic VG-Lite rendering backend implementation that is designed to utilize
`VeriSilicon <https://verisilicon.com/>`_'s generic API to operate GPU hardware as much as possible.

Even with different chip manufacturers, as long as they use the same version of VG-Lite API as the rendering backend,
LVGL rendering acceleration can be supported without the need for LVGL adaptation work.


Configuration
*************

1. Set :c:macro:`LV_USE_DRAW_VG_LITE` to 1 in ``lv_conf.h`` to enabled the VG-Lite rendering backend.
   Make sure that your hardware has been adapted to the VG-Lite API and that the absolute path to ``vg_lite.h``, which can be directly referenced by lvgl, has been exposed.

2. Confirm the GPU initialization method, there are two ways:

   - The SDK calls the GPU initialization function on its own during system startup, and the GPU is available when LVGL starts; set :c:macro:`LV_VG_LITE_USE_GPU_INIT` to 0.
   - LVGL actively calls the GPU initialization function, and the SDK needs to implement the public function `gpu_init()`.
     LVGL will call it to complete the GPU hardware initialization during startup; set :c:macro:`LV_VG_LITE_USE_GPU_INIT` to 1.

3. Set the :c:macro:`LV_VG_LITE_USE_ASSERT` configuration to enable GPU call parameter checking.
   Due to the complexity of the parameters used in GPU calls, incorrect parameters can result in abnormal GPU hardware operation, such as forgetting to add an end symbol
   to the path or not meeting the alignment requirements for buffer stride.
   To quickly resolve such issues, strict parameter checking has been added before each VG-Lite call, including buffer stride validation and matrix invertibility check.
   When an error parameter is detected, an assertion will occur to print out the error parameter, allowing the user to promptly make corrections and reduce the time wasted on hardware simulation.
   Please note that enabling this check will decrease runtime performance. It is recommended to enable it in Debug mode and disable it in the Release version.

4. Set the :c:macro:`LV_VG_LITE_FLUSH_MAX_COUNT` configuration to specify the flush method.
   VG-Lite uses two sets of command buffer buffers to render instructions, and utilizing this mechanism well can greatly improve drawing efficiency.
   Currently, two buffering methods are supported:

   - Set :c:macro:`LV_VG_LITE_FLUSH_MAX_COUNT` to zero (recommended). The rendering backend will obtain the GPU's working status every time it writes rendering instructions to the command buffer.

   When the GPU is idle, it will immediately call ``vg_lite_flush`` to notify the GPU to start rendering and swap the command buffer. When the GPU is busy, it will continue to fill the command buffer cache with rendering instructions.
   The underlying driver will automatically determine if the command buffer has been filled. When it is about to be filled, it will forcibly wait for the unfinished drawing tasks to end and swap the command buffer.
   This method can effectively improve GPU utilization, especially in scenarios where rendering text, as the GPU's drawing time and the CPU's data preparation time are very close, allowing the CPU and GPU to run in parallel.

   - Set :c:macro:`LV_VG_LITE_FLUSH_MAX_COUNT` to a value greater than zero, such as 8. After writing 8 rendering instructions to the command buffer, the rendering backend

   will call ``vg_lite_flush`` to notify the GPU to start rendering and swap the command buffer.

5. Set the :c:macro:`LV_VG_LITE_USE_BOX_SHADOW` configuration to use GPU rendering for shadows.
   In fact, GPU hardware does not actually support shadow rendering. However, through experimentation, it has been found that a similar shadow effect
   can be achieved by using multiple layers of borders with different levels of transparency.
   It is recommended to enable this configuration in scenarios where the shadow quality requirements are not high, as it can significantly improve rendering efficiency.

6. Set the :c:macro:`LV_VG_LITE_GRAD_CACHE_CNT` configuration to specify the number of gradient cache entries.
   Gradient drawing includes linear gradients and radial gradients. Using a cache can effectively reduce the number of times the gradient image is created and improve drawing efficiency.
   Each individual gradient consumes around 4K of GPU memory pool. If there are many gradients used in the interface, you can try increasing the number of gradient cache entries.
   If the VG-Lite API returns the :c:macro:`VG_LITE_OUT_OF_RESOURCES` error, you can try increasing the size of the GPU memory pool or reducing the number of gradient cache entries.

7. Set the :c:macro:`LV_VG_LITE_STROKE_CACHE_CNT` configuration to specify the number of stroke path caches.
   When the stroke parameters do not change, the previously generated stroke parameters are automatically retrieved from the cache to improve rendering performance.
   The memory occupied by the stroke is strongly related to the path length. If the VG-Lite API returns the :c:macro:`VG_LITE_OUT_OF_RESOURCES` error,
   you can try increasing the size of the GPU memory pool or reducing the number of stroke cache entries.

NOTE: VG-Lite rendering backend does not support multi-threaded calls, please make sure :c:macro:`LV_USE_OS` is always configured as :c:macro:`LV_OS_NONE`.


VG-Lite Simulator
*****************

LVGL integrates a VG-Lite simulator based on ThorVG.
Its purpose is to simplify the debugging of VG-Lite adaptation and reduce the time of debugging and locating problems on hardware devices.
For detailed instructions, see :ref:`vg_lite_tvg`.


API
***

:ref:`lv_draw_vglite_h`

:ref:`lv_vglite_buf_h`

:ref:`lv_vglite_matrix_h`

:ref:`lv_vglite_path_h`

:ref:`lv_vglite_utils_h`


