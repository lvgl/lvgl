.. _changelog:

Change Log
==========

`v9.4.0 <https://github.com/lvgl/lvgl/compare/v9.4.0...v9.3.0>`__ 16 October 2025
---------------------------------------------------------------------------------

LVGL v9.4.0 brings significant performance improvements, expanded GPU support, and new
capabilities for modern embedded and MPU platforms.

Main Features
~~~~~~~~~~~~~

- **glTF + 3D Support**. Load your glTF 3D models directly into LVGL UIs for rich,
  immersive interfaces. `Learn more
  <https://docs.lvgl.io/master/details/libs/gltf.html>`__.
- **XML support for LVGL Pro**. Describe your UI declaratively and load it at runtime
  without having to recompile firmware.  Try it out with the newest LVGL Pro Editor.
  `Learn More <https://pro.lvgl.io/>__`

GPU Acceleration
~~~~~~~~~~~~~~~~

- **EVE GPU Renderer Support**. Offload rendering to external EVE chips via SPI,
  freeing up your MCU's CPU for other tasks. `Learn More
  <https://docs.lvgl.io/master/details/integration/external_display_controllers/eve/gpu.html>`__.
- **ESP PPA Hardware Accelerator**. Achieve 30% faster render times and 30% reduced
  CPU usage on the new ESP32-P4 with native hardware acceleration. `Learn More
  <https://docs.lvgl.io/master/details/integration/external_display_controllers/eve/gpu.html>`__.
- **NemaGFX Vector Graphic**. Render SVGs and vector graphics natively on ST's
  NeoChrom and other NemaGFX compatible GPUs. `Learn More
  <https://docs.lvgl.io/master/details/integration/chip_vendors/stm32/neochrom.html#vector-graphics>`__.
- **Unified VGLite Renderer**. Single driver supporting all platforms with a
  consistent feature set. `Learn More
  <https://docs.lvgl.io/master/details/integration/chip_vendors/nxp/vg_lite_gpu.html>`
- **Dave2D Renderer improvements.** Lower CPU usage on Renesas and Alif platforms with
  optimized Dave2D integration. `Learn More
  <https://docs.lvgl.io/master/details/integration/chip_vendors/renesas/built_in_drivers.html#dave2d>`

MPU Features
~~~~~~~~~~~~

- **GStreamer Support**. Play videos and multimedia content directly within LVGL UIs
  with full GStreamer codec support. `Learn more
  <https://docs.lvgl.io/master/details/libs/gstreamer.html>`__.
- **DRM + EGL Driver**. High-performance Linux display rendering with native DRM and
  OpenGL ES (EGL) support for embedded displays capable of rendering glTF models.
  `Learn more
  <https://docs.lvgl.io/master/details/integration/embedded_linux/drivers/drm.html>`__.
- **ARM NEON Optimization**. Up to 33% faster software rendering with on platforms
  that support NEON SIMD support. `Learn More
  <https://docs.lvgl.io/master/details/integration/chip_vendors/arm/overview.html#neon-acceleration>`__.

Other Features
~~~~~~~~~~~~~~

- **Faster GIF Library**. No changes required, just upgrade LVGL and your GIFs will
  render faster. `Learn More <https://docs.lvgl.io/master/details/libs/gif.html>`__.
- **FrogFS support**. Pack directory trees into a single "blob" and load them at
  runtime. `Learn More <https://docs.lvgl.io/master/details/libs/frogfs.html>`__.

Documentation
~~~~~~~~~~~~~

Alongside these features, we've thoroughly reorganized and proofread the entire
documentation suite, and included numerous new driver guides and platform
integrations to better serve our partners and the embedded community.
