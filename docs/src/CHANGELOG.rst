.. _changelog:

Change Log
==========

`v9.4.0 <https://github.com/lvgl/lvgl/compare/v9.3.0...v9.4.0>`__ 16 October 2025
---------------------------------------------------------------------------------------------------------------------------------------------------

LVGL v9.4.0 brings significant performance improvements, expanded GPU support, and new
capabilities for modern embedded and MPU platforms.

Main Features
~~~~~~~~~~~~~

- **glTF + 3D Support**. Load your glTF 3D models directly into LVGL UIs for rich,
  immersive interfaces. `Learn more
  <https://docs.lvgl.io/master/libs/gltf.html>`__.
- **XML support for LVGL Pro**. Describe your UI declaratively and load it at runtime
  without having to recompile firmware.  Try it out with the newest LVGL Pro Editor.
  `Learn More <https://pro.lvgl.io/>__`

GPU Acceleration
~~~~~~~~~~~~~~~~

- **EVE GPU Renderer Support**. Offload rendering to external EVE chips via SPI,
  freeing up your MCU's CPU for other tasks. `Learn More
  <https://docs.lvgl.io/master/integration/external_display_controllers/eve/gpu.html>`__.
- **ESP PPA Hardware Accelerator**. Achieve 30% faster render times and 30% reduced
  CPU usage on the new ESP32-P4 with native hardware acceleration. `Learn More
  <https://docs.lvgl.io/master/integration/external_display_controllers/eve/gpu.html>`__.
- **NemaGFX Vector Graphic**. Render SVGs and vector graphics natively on ST's
  NeoChrom and other NemaGFX compatible GPUs. `Learn More
  <https://docs.lvgl.io/master/integration/chip_vendors/stm32/neochrom.html#vector-graphics>`__.
- **Unified VGLite Renderer**. Single driver supporting all platforms with a
  consistent feature set. `Learn More
  <https://docs.lvgl.io/master/integration/chip_vendors/nxp/vg_lite_gpu.html>`
- **Dave2D Renderer improvements.** Lower CPU usage on Renesas and Alif platforms with
  optimized Dave2D integration. `Learn More
  <https://docs.lvgl.io/master/integration/chip_vendors/renesas/built_in_drivers.html#dave2d>`

MPU Features
~~~~~~~~~~~~

- **GStreamer Support**. Play videos and multimedia content directly within LVGL UIs
  with full GStreamer codec support. `Learn more
  <https://docs.lvgl.io/master/libs/gstreamer.html>`__.
- **DRM + EGL Driver**. High-performance Linux display rendering with native DRM and
  OpenGL ES (EGL) support for embedded displays capable of rendering glTF models.
  `Learn more
  <https://docs.lvgl.io/master/integration/embedded_linux/drivers/drm.html>`__.
- **ARM NEON Optimization**. Up to 33% faster software rendering with on platforms
  that support NEON SIMD support. `Learn More
  <https://docs.lvgl.io/master/integration/chip_vendors/arm/overview.html#neon-acceleration>`__.

Other Features
~~~~~~~~~~~~~~

- **Faster GIF Library**. No changes required, just upgrade LVGL and your GIFs will
  render faster. `Learn More <https://docs.lvgl.io/master/libs/gif.html>`__.
- **FrogFS support**. Pack directory trees into a single "blob" and load them at
  runtime. `Learn More <https://docs.lvgl.io/master/libs/frogfs.html>`__.

Documentation
~~~~~~~~~~~~~

Alongside these features, we've thoroughly reorganized and proofread the entire
documentation suite, and included numerous new driver guides and platform
integrations to better serve our partners and the embedded community.

New Features
~~~~~~~~~~~~

- **feat(EVE): allow disabling write buffering** `9072 <https://github.com/lvgl/lvgl/pull/9072>`__
- **feat(opengl): support glsl version 100 for default shader** `9029 <https://github.com/lvgl/lvgl/pull/9029>`__
- **feat(subject): add back subject increment events** `9057 <https://github.com/lvgl/lvgl/pull/9057>`__
- **feat(xml): add scrollbar modes** `9058 <https://github.com/lvgl/lvgl/pull/9058>`__
- **feat(test): reintroduce lv_text_get_size without attributes** `9036 <https://github.com/lvgl/lvgl/pull/9036>`__
- **feat(theme): re-initialize the theme on resolution change** `9048 <https://github.com/lvgl/lvgl/pull/9048>`__
- **feat(g2d): add rgb565, png, tiled images and rotation support** `8848 <https://github.com/lvgl/lvgl/pull/8848>`__
- **feat(gltf): replace default gltf env image with a more generic solution** `9045 <https://github.com/lvgl/lvgl/pull/9045>`__
- **feat(drivers): NV3007 driver & docs** `9021 <https://github.com/lvgl/lvgl/pull/9021>`__
- **feat(sysmon): add cpu proc usage when using monitor log mode** `9010 <https://github.com/lvgl/lvgl/pull/9010>`__
- **feat(drm): add ability to select display mode** `8992 <https://github.com/lvgl/lvgl/pull/8992>`__
- **feat(osal): enable linux specific features with os_none** `8989 <https://github.com/lvgl/lvgl/pull/8989>`__
- **feat(egl/gltf): add gltf support with EGL** `8980 <https://github.com/lvgl/lvgl/pull/8980>`__
- **feat(draw): support image colorkey** `8628 <https://github.com/lvgl/lvgl/pull/8628>`__
- **feat(NemaGFX): add vector draw task support** `8938 <https://github.com/lvgl/lvgl/pull/8938>`__
- **feat(chart): add cursor removal method** `8952 <https://github.com/lvgl/lvgl/pull/8952>`__
- **feat(lv_global): move user_data to the top of lv_global_t** `8908 <https://github.com/lvgl/lvgl/pull/8908>`__
- **feat(sdl): Add SDL_Window accessor function to retrieve window from lv_display_t** `8886 <https://github.com/lvgl/lvgl/pull/8886>`__
- **feat(kconfig_verify): add space indentation check** `8962 <https://github.com/lvgl/lvgl/pull/8962>`__
- **feat(xml): add spinbox support** `8957 <https://github.com/lvgl/lvgl/pull/8957>`__
- **feat(EVE): support fonts with a stride alignment** `8937 <https://github.com/lvgl/lvgl/pull/8937>`__
- **feat(xml): support &lt;include_timeline&gt;** `8902 <https://github.com/lvgl/lvgl/pull/8902>`__
- **feat(display): deprecate lv_display_set_antialiasing** `8958 <https://github.com/lvgl/lvgl/pull/8958>`__
- **feat(roller): function to get option string** `8730 <https://github.com/lvgl/lvgl/pull/8730>`__
- **feat(xml): add subject_toggle_event and move the limits to subjects from increment event** `8930 <https://github.com/lvgl/lvgl/pull/8930>`__
- **feat(qrcode): add new set_data helper function** `8950 <https://github.com/lvgl/lvgl/pull/8950>`__
- **feat(draw/sw): add neon support for rgb888 and xrgb8888** `8933 <https://github.com/lvgl/lvgl/pull/8933>`__
- **feat(XML/fsdrv): add frogfs and XML load from blob** `8870 <https://github.com/lvgl/lvgl/pull/8870>`__
- **feat(xml): add new step tests click_on and set_language** `8931 <https://github.com/lvgl/lvgl/pull/8931>`__
- **feat(label): implement api to bind a translation tag to a label** `8948 <https://github.com/lvgl/lvgl/pull/8948>`__
- **feat(vglite): refactor driver config and remove NXP VGLite draw unit** `8911 <https://github.com/lvgl/lvgl/pull/8911>`__
- **feat(pxp): add PXP support for tiled images** `8846 <https://github.com/lvgl/lvgl/pull/8846>`__
- **feat(translation): add language changed event** `8882 <https://github.com/lvgl/lvgl/pull/8882>`__
- **feat(profiler): add posix porting** `8924 <https://github.com/lvgl/lvgl/pull/8924>`__
- **feat(EVE): support bitmaps larger than 511x511** `8732 <https://github.com/lvgl/lvgl/pull/8732>`__
- **feat(XML): XML load from directory** `8774 <https://github.com/lvgl/lvgl/pull/8774>`__
- **feat(wayland): remove deprecated WL_SHELL** `8877 <https://github.com/lvgl/lvgl/pull/8877>`__
- **feat(api_mapping): add ability to disable api mapping** `8745 <https://github.com/lvgl/lvgl/pull/8745>`__
- **feat(xml): add qrcode support in XML** `8904 <https://github.com/lvgl/lvgl/pull/8904>`__
- **feat(draw/dave2d): remove todos** `8896 <https://github.com/lvgl/lvgl/pull/8896>`__
- **feat(opengl): add support for creating an LVGL OpenGL texture display from an existing texture** `8837 <https://github.com/lvgl/lvgl/pull/8837>`__
- **feat(grad): make complex gradients available regardless of whether draw_sw support is enabled** `8834 <https://github.com/lvgl/lvgl/pull/8834>`__
- **feat(event): add ability to delete display and indev on event** `8893 <https://github.com/lvgl/lvgl/pull/8893>`__
- **feat(draw): add event_cb in draw unit** `8891 <https://github.com/lvgl/lvgl/pull/8891>`__
- **feat(sdl): add set_window_size** `8809 <https://github.com/lvgl/lvgl/pull/8809>`__
- **feat(xml): add rotation in arc xml parser** `8899 <https://github.com/lvgl/lvgl/pull/8899>`__
- **feat(style): add `lv_style_merge` and appropriate tests** `8680 <https://github.com/lvgl/lvgl/pull/8680>`__
- **feat(wayland): Restore the touchscreen support** `8785 <https://github.com/lvgl/lvgl/pull/8785>`__
- **feat(gstreamer): add gstreamer support** `8757 <https://github.com/lvgl/lvgl/pull/8757>`__
- **feat(gltf): allow compiling gltf with EGL** `8833 <https://github.com/lvgl/lvgl/pull/8833>`__
- **feat(tests): add lv_deinit test** `8789 <https://github.com/lvgl/lvgl/pull/8789>`__
- **feat(draw_buf): add MVE & NEON support of alpha premultiply** `8712 <https://github.com/lvgl/lvgl/pull/8712>`__
- **feat(OpenGL): add EGL support** `8677 <https://github.com/lvgl/lvgl/pull/8677>`__
- **feat(osal): add lv_sleep_ms interface and implementation.** `8805 <https://github.com/lvgl/lvgl/pull/8805>`__
- **feat(display): add ability to get current screen being loaded** `8746 <https://github.com/lvgl/lvgl/pull/8746>`__
- **feat(display): initial lovyan integration** `8630 <https://github.com/lvgl/lvgl/pull/8630>`__
- **feat(draw): vg_lite: add initial support for the VGLite HAL** `8795 <https://github.com/lvgl/lvgl/pull/8795>`__
- **feat(opengles): add performance measurement points** `8776 <https://github.com/lvgl/lvgl/pull/8776>`__
- **feat(NemaGFX): Support complex gradients: linear, radial** `8696 <https://github.com/lvgl/lvgl/pull/8696>`__
- **feat(tests): add lcd driver test** `8759 <https://github.com/lvgl/lvgl/pull/8759>`__
- **feat(wayland/dmabuf): add window decorations support** `8406 <https://github.com/lvgl/lvgl/pull/8406>`__
- **feat(draw/dave2d): improve performance using block processing** `8725 <https://github.com/lvgl/lvgl/pull/8725>`__
- **feat(vglite): add vglite kernel driver** `8734 <https://github.com/lvgl/lvgl/pull/8734>`__
- **feat(gltf): add gltf support** `8643 <https://github.com/lvgl/lvgl/pull/8643>`__
- **feat(workflows): add gcover report** `8750 <https://github.com/lvgl/lvgl/pull/8750>`__
- **feat(EVE): add asset pre-upload functions** `8710 <https://github.com/lvgl/lvgl/pull/8710>`__
- **feat(draw_buf): make buf_copy overwritable for GPU** `8693 <https://github.com/lvgl/lvgl/pull/8693>`__
- **feat(draw/sw): add neon 64bit support for rgb565 blending operations** `8590 <https://github.com/lvgl/lvgl/pull/8590>`__
- **feat(font_manager): check resource leak before removing source** `8613 <https://github.com/lvgl/lvgl/pull/8613>`__
- **feat(evdev): allow adopting existing fd** `8556 <https://github.com/lvgl/lvgl/pull/8556>`__
- **feat(chart): add stacked chart support** `8573 <https://github.com/lvgl/lvgl/pull/8573>`__
- **feat(xml): add animation support** `8599 <https://github.com/lvgl/lvgl/pull/8599>`__
- **feat(nuttx_profiler): support trace to file** `8674 <https://github.com/lvgl/lvgl/pull/8674>`__
- **feat(vg_lite):  added compatibility with NXP platforms** `8642 <https://github.com/lvgl/lvgl/pull/8642>`__
- **feat(EVE): Add EVE draw unit** `8211 <https://github.com/lvgl/lvgl/pull/8211>`__
- **feat(scripts): add display & drawbuf dump** `8645 <https://github.com/lvgl/lvgl/pull/8645>`__
- **feat(sysmon): add API to start or stop sysmon and dump FPS info** `8533 <https://github.com/lvgl/lvgl/pull/8533>`__
- **feat(obj): allow `LV_SIZE_CONTENT` for min and max width/height** `8448 <https://github.com/lvgl/lvgl/pull/8448>`__
- **feat(NemaGFX): encoded images** `8523 <https://github.com/lvgl/lvgl/pull/8523>`__
- **feat(cache): implement dynamic array with second chance algorithm class** `8433 <https://github.com/lvgl/lvgl/pull/8433>`__
- **feat(font): Remove SimSun Font** `8550 <https://github.com/lvgl/lvgl/pull/8550>`__
- **feat(svg): add SVG2 special path command 'A' support.** `8574 <https://github.com/lvgl/lvgl/pull/8574>`__
- **feat(label): support passing variadic list as param** `8322 <https://github.com/lvgl/lvgl/pull/8322>`__
- **feat(demo): separate the widgets demo into multiple files** `8507 <https://github.com/lvgl/lvgl/pull/8507>`__
- **feat(xml): add lv_xml_set_default_asset_path** `8549 <https://github.com/lvgl/lvgl/pull/8549>`__
- **feat(xml): add float support to subject change events** `8535 <https://github.com/lvgl/lvgl/pull/8535>`__
- **feat(nuttx) : add mouse capabilities to applications** `8425 <https://github.com/lvgl/lvgl/pull/8425>`__
- **feat(state): Add state trickle down** `8499 <https://github.com/lvgl/lvgl/pull/8499>`__
- **feat(events): add event trickle mechanism to propagate events to children** `8415 <https://github.com/lvgl/lvgl/pull/8415>`__
- **feat(chart): support custom LV_EVENT_REFR_EXT_DRAW_SIZE** `8268 <https://github.com/lvgl/lvgl/pull/8268>`__
- **feat(osal/linux): add function which provides cpu usage of the app itself** `8340 <https://github.com/lvgl/lvgl/pull/8340>`__
- **feat(dma2d): enable the DMA2D clock for STM32L4 series** `8429 <https://github.com/lvgl/lvgl/pull/8429>`__
- **feat(anim): call start callback when animation restarts** `8403 <https://github.com/lvgl/lvgl/pull/8403>`__
- **feat(DMA2D): encoded images** `8470 <https://github.com/lvgl/lvgl/pull/8470>`__
- **feat(translation): add translation support** `8255 <https://github.com/lvgl/lvgl/pull/8255>`__
- **feat(xml): add testing support for XMLs** `8327 <https://github.com/lvgl/lvgl/pull/8327>`__
- **feat(observer): add float subject type** `8388 <https://github.com/lvgl/lvgl/pull/8388>`__
- **feat(dma2d): improve DMA2D Compatibility** `8293 <https://github.com/lvgl/lvgl/pull/8293>`__
- **feat(assert): refactoring LV_ASSERT_HANDLER handling** `8434 <https://github.com/lvgl/lvgl/pull/8434>`__
- **feat(anim): add vsync mode** `8087 <https://github.com/lvgl/lvgl/pull/8087>`__
- **feat(drivers): add NXP ELCDIF initial display support** `8349 <https://github.com/lvgl/lvgl/pull/8349>`__
- **feat(draw/ppa): add initial Pixel Processing Accelerator infrastructure for ESP** `8270 <https://github.com/lvgl/lvgl/pull/8270>`__
- **feat(vg_lite): add tile image support** `8226 <https://github.com/lvgl/lvgl/pull/8226>`__
- **feat(draw): Optimize struct alignment** `8409 <https://github.com/lvgl/lvgl/pull/8409>`__
- **feat(vg_lite): support tiled image** `8355 <https://github.com/lvgl/lvgl/pull/8355>`__
- **feat(tiny_ttf): add kerning cache** `8320 <https://github.com/lvgl/lvgl/pull/8320>`__
- **feat(xml): make &lt;style&gt; and &lt;bind&gt; properties child tags instead of props** `02b079f <https://github.com/lvgl/lvgl/commit/02b079f0a23db5589feb6b8e08112220779202de>`__
- **feat(observer): add subject set and increment functions** `3ecd37e <https://github.com/lvgl/lvgl/commit/3ecd37e667af761cce3bf5fef5885c3b02e54427>`__
- **feat(arc_label): add new widget arc label support** `5f28038 <https://github.com/lvgl/lvgl/commit/5f2803863e895938c9786507d9b222cd2543409c>`__
- **feat(obj): add screen_create/load_event API and XML support** `a762e20 <https://github.com/lvgl/lvgl/commit/a762e203e267a9ab29a91f2bc824c7dd0398c8a2>`__
- **feat(observer): add lv_obj_bind_style** `104ede5 <https://github.com/lvgl/lvgl/commit/104ede500ee391220f9960a6555e4b7375274cff>`__
- **feat(xml): add remove_style and remove_style_all support** `3951ea4 <https://github.com/lvgl/lvgl/commit/3951ea4b95c2f29c71bc70756b7d9dbef77129a2>`__
- **feat(arc_label): support recolor feature** `78e3375 <https://github.com/lvgl/lvgl/commit/78e33759644be2c578eacf0bbf1e05be9c0e9c1f>`__
- **feat(scale): add lv_scale_bind_section_min/max_value** `7aaeb66 <https://github.com/lvgl/lvgl/commit/7aaeb6694cd34d21f7d9af819853da0172a816d3>`__
- **feat(span): add lv_spangroup_bind_span_text and lv_spangroup_set_span_text_fmt** `03c4870 <https://github.com/lvgl/lvgl/commit/03c48701b7842eefe2571f26e3787e8b379df985>`__
- **feat(xml): add lv_switch** `250bc8b <https://github.com/lvgl/lvgl/commit/250bc8b039db2875b6e2d375933c94c75101ea8f>`__
- **feat(scripts): add fully automatic test + environment installation verification script** `6fbdb7f <https://github.com/lvgl/lvgl/commit/6fbdb7fef9771d5e50e38b50f023e5efe30d1e51>`__
- **feat(arc_label): support vertical align feature** `cb22319 <https://github.com/lvgl/lvgl/commit/cb2231951cf95aa3e5a5ce2bf482f73436870739>`__
- **feat(examples): add arc label example** `5e8add3 <https://github.com/lvgl/lvgl/commit/5e8add3957000f0ec9e6505393b72db3912ecf33>`__
- **feat(draw): Use Espresssif PPA driver in non-blocking mode, also tune the DMA2D burst size for performance on PPA side** `f3e0056 <https://github.com/lvgl/lvgl/commit/f3e005623aefaf68e3c9bb65f8eadfa3a8c9207c>`__
- **feat(image): add data binding to image src** `d33320b <https://github.com/lvgl/lvgl/commit/d33320b7e5cb2c53525a00d6b91191ecc8218e36>`__
- **feat(xml): add grid support** `92a51f0 <https://github.com/lvgl/lvgl/commit/92a51f056f3bcf8d786df9f20acd9684ea57101b>`__
- **feat(bar): add data binding to the bar** `9bddc81 <https://github.com/lvgl/lvgl/commit/9bddc816a2fedaadb946278bf50ca29df7c76fe1>`__
- **feat(arc_label): support horizontal align feature** `bcfc2ab <https://github.com/lvgl/lvgl/commit/bcfc2ab35994507ed8627be862c990f422a1715e>`__
- **feat(qrcode): support qrcode quiet zone feature** `0b71f7d <https://github.com/lvgl/lvgl/commit/0b71f7d9e87955ced3e1ac31dfc71410ca2b97c9>`__
- **feat(tabview): support getting tab buttons by index** `fbc8dbe <https://github.com/lvgl/lvgl/commit/fbc8dbe6a71e931f97bdbde939a3e8b9d22a968e>`__
- **feat(xml): add support for scroll snapping** `40158e4 <https://github.com/lvgl/lvgl/commit/40158e483ae1a8b3dffaa49e948d9b343d6cd049>`__
- **feat(arc_label): support text offset** `fc83651 <https://github.com/lvgl/lvgl/commit/fc83651c0fce0a6c03bf13b99815fee07c6a431f>`__
- **feat(build.py):  add documentation section to quickly orient new users on where data flows.** `e818acf <https://github.com/lvgl/lvgl/commit/e818acfd7783a6aaed383dc9510a63269f1c94f8>`__
- **feat(xml): qrcode support quiet_zone property** `98423e6 <https://github.com/lvgl/lvgl/commit/98423e6046ae1084351993cacee5d83515606b33>`__
- **feat(arc_label): support text letter space** `33bb46f <https://github.com/lvgl/lvgl/commit/33bb46fcac83ca5ef4f6d0c6028074114cdb1078>`__
- **feat(arc_label): support text direction of counter clockwise** `f55fee8 <https://github.com/lvgl/lvgl/commit/f55fee85fc9f9c2f7468f71d7a4c94522afdc52d>`__
- **feat(arc_label): support more calculate accuracy** `1dc268e <https://github.com/lvgl/lvgl/commit/1dc268ea67d1a0cdf2b58fa94b00d56c8ed9699a>`__
- **feat(custom.css):  add `lvgl-figure-caption` class back...** `ebcd244 <https://github.com/lvgl/lvgl/commit/ebcd2446710943c33479811e12f4718f781061e6>`__
- **feat(arc_label): support radius percentage mode** `7cdc1ef <https://github.com/lvgl/lvgl/commit/7cdc1ef2271bdd1221d71753216e8bbec245b076>`__
- **feat(xml): review the types used for style properties** `96e4f8f <https://github.com/lvgl/lvgl/commit/96e4f8fd444adf2b5fa1a39e1c6c440e7acdc88e>`__
- **feat:  make enum type names consistent with value prefixes** `82cd8ad <https://github.com/lvgl/lvgl/commit/82cd8adf2c081af2d31a0df73ae4a99e8f09b88f>`__
- **feat:  remove line from `requirements.txt` that is not actually required to build docs** `452878f <https://github.com/lvgl/lvgl/commit/452878fbe8cb00a81253a62475a453f3cb9dea48>`__

Performance
~~~~~~~~~~~

- **perf(EVE): buffered writes** `9051 <https://github.com/lvgl/lvgl/pull/9051>`__
- **perf(egl): improve egl performance** `8966 <https://github.com/lvgl/lvgl/pull/8966>`__
- **perf(GIF): Faster GIF library drop-in replacement** `8743 <https://github.com/lvgl/lvgl/pull/8743>`__
- **perf(tests): remove duplicate vg-lite compilation** `8841 <https://github.com/lvgl/lvgl/pull/8841>`__
- **perf(test_perf): increase the refresh rate to 60 FPS** `8751 <https://github.com/lvgl/lvgl/pull/8751>`__
- **perf(vg_lite): optimize GPU fill rendering performance** `8609 <https://github.com/lvgl/lvgl/pull/8609>`__
- **perf(vg_lite): optimize the memory usage and search speed of grad item** `8474 <https://github.com/lvgl/lvgl/pull/8474>`__
- **perf(snapshot): render from the top object when taking snapshot.** `8424 <https://github.com/lvgl/lvgl/pull/8424>`__
- **perf(text): reduce the get glyph function call** `8364 <https://github.com/lvgl/lvgl/pull/8364>`__
- **perf(draw): only dispatch refreshing disp** `8305 <https://github.com/lvgl/lvgl/pull/8305>`__
- **perf(arc_label): optimization enhancement of arc label algorithm** `b49bba5 <https://github.com/lvgl/lvgl/commit/b49bba5fcf170bfe85b36f623850c1b89de18622>`__

Fixes
~~~~~

- **fix(windows): correctly calculate Windows window-size** `9060 <https://github.com/lvgl/lvgl/pull/9060>`__
- **fix(g2d): properly check and free an item from the map** `9065 <https://github.com/lvgl/lvgl/pull/9065>`__
- **fix(DMA2D): fix non-async mode build failure** `9069 <https://github.com/lvgl/lvgl/pull/9069>`__
- **fix(vg_lite_driver): general fixes at the vglite driver and vglite draw unit link** `9063 <https://github.com/lvgl/lvgl/pull/9063>`__
- **fix(label): ignore empty translation tags** `9062 <https://github.com/lvgl/lvgl/pull/9062>`__
- **fix(examples): fix format specifier warning** `9066 <https://github.com/lvgl/lvgl/pull/9066>`__
- **fix(g2d): fix compilation error with G2D version &lt;2.3.0** `9061 <https://github.com/lvgl/lvgl/pull/9061>`__
- **fix(chart): fix division by zero if div. line count is 1** `9064 <https://github.com/lvgl/lvgl/pull/9064>`__
- **fix(dma2d): enable dma2d clock for STM32N6** `9044 <https://github.com/lvgl/lvgl/pull/9044>`__
- **fix(indev): fix off by one error resulting in elastic scroll when disabled** `9047 <https://github.com/lvgl/lvgl/pull/9047>`__
- **fix(anim): play animations from their current progress** `9052 <https://github.com/lvgl/lvgl/pull/9052>`__
- **fix(draw_sw): sw vector graphic partial mode fix y_offset** `9006 <https://github.com/lvgl/lvgl/pull/9006>`__
- **fix(wayland): correctly get driver data in lv_wayland_assign_physical_display** `9017 <https://github.com/lvgl/lvgl/pull/9017>`__
- **fix(draw/neon): disable function using an invalid intrinsic on arm32** `9019 <https://github.com/lvgl/lvgl/pull/9019>`__
- **fix(OpenGL/DRM): fix misc info logs** `9027 <https://github.com/lvgl/lvgl/pull/9027>`__
- **fix(draw/vglite): update the wait interrupt function to use the lv_sync_wait instead of the FreeRTOS Semaphore specifc API** `9007 <https://github.com/lvgl/lvgl/pull/9007>`__
- **fix(draw): fix render issue when triangle overhangs by sorting points** `9002 <https://github.com/lvgl/lvgl/pull/9002>`__
- **fix(table): remove free calls to user data** `8881 <https://github.com/lvgl/lvgl/pull/8881>`__
- **fix(wayland): bind driver data to display's driver data instead of user data** `8991 <https://github.com/lvgl/lvgl/pull/8991>`__
- **fix(wayland): multiple windows fix and wl_output support** `8923 <https://github.com/lvgl/lvgl/pull/8923>`__
- **fix(assert): revert asserting in `lv_assert_handler` function** `8976 <https://github.com/lvgl/lvgl/pull/8976>`__
- **fix(file_explorer): fix mem leak when sorting by file kind** `8935 <https://github.com/lvgl/lvgl/pull/8935>`__
- **fix(themes): add malloc assert checks** `8977 <https://github.com/lvgl/lvgl/pull/8977>`__
- **fix(drm/egl): correctly match egl config in config select cb** `8978 <https://github.com/lvgl/lvgl/pull/8978>`__
- **fix(sdl): support UTF-8 characters in the keyboard** `8906 <https://github.com/lvgl/lvgl/pull/8906>`__
- **fix(xml): add the missing parsing of min/max subject limits** `8971 <https://github.com/lvgl/lvgl/pull/8971>`__
- **fix(wayland): bind draw buffers to the window instead of the shm_context** `8878 <https://github.com/lvgl/lvgl/pull/8878>`__
- **fix(draw_buf): check range for draw buffer when accessing pixel** `8907 <https://github.com/lvgl/lvgl/pull/8907>`__
- **fix(opengles): add missing variable declaration** `8969 <https://github.com/lvgl/lvgl/pull/8969>`__
- **fix(menu): larger bitfield for enum value needed on Windows** `8965 <https://github.com/lvgl/lvgl/pull/8965>`__
- **fix(gdb/draw_buf): validate and correct expected data size for pixel buffer** `8944 <https://github.com/lvgl/lvgl/pull/8944>`__
- **fix(EVE): align the dummy buf to 4 bytes** `8953 <https://github.com/lvgl/lvgl/pull/8953>`__
- **fix(vg_lite): change log level from info to warning for unsupported gradient features** `8947 <https://github.com/lvgl/lvgl/pull/8947>`__
- **fix(fbdev): fix stride, bounds checking, and partial flushes in direct render mode in flush callback** `8914 <https://github.com/lvgl/lvgl/pull/8914>`__
- **fix(vg_lite): replace FLT_MIN to -FLT_MAX** `8940 <https://github.com/lvgl/lvgl/pull/8940>`__
- **fix(matrix): fix matrix transformation area coordinate error** `8928 <https://github.com/lvgl/lvgl/pull/8928>`__
- **fix(vector): rename and enhance rectangle path function with xywh parameters** `8934 <https://github.com/lvgl/lvgl/pull/8934>`__
- **fix(msgbox): remove superflous dependency check** `8916 <https://github.com/lvgl/lvgl/pull/8916>`__
- **fix(sysmon): division by zero error** `8918 <https://github.com/lvgl/lvgl/pull/8918>`__
- **fix(flex): fix alignment issue when using rtl in flex layout** `8929 <https://github.com/lvgl/lvgl/pull/8929>`__
- **fix(vector): adjust image stride handling in fill pattern and add test case** `8815 <https://github.com/lvgl/lvgl/pull/8815>`__
- **fix(vg_lite): fix build break after LV_USE_VECTOR_GRAPHIC is turned off** `8912 <https://github.com/lvgl/lvgl/pull/8912>`__
- **fix(keyboard): add missing dependency checks** `8917 <https://github.com/lvgl/lvgl/pull/8917>`__
- **fix(wayland): fix stride calculation when iterating over drawbuf** `8915 <https://github.com/lvgl/lvgl/pull/8915>`__
- **fix(kconfig): add TinyTTF cache count setting support** `8919 <https://github.com/lvgl/lvgl/pull/8919>`__
- **fix(memfs): use string only in lv_fs_path_ex_t for better compatibility** `8571 <https://github.com/lvgl/lvgl/pull/8571>`__
- **fix(os): make lv_lock/unlock public as they should be used by the users** `8889 <https://github.com/lvgl/lvgl/pull/8889>`__
- **fix(menu): allow using menu section as parent of menu container** `8884 <https://github.com/lvgl/lvgl/pull/8884>`__
- **fix(xml): fix grid support** `8888 <https://github.com/lvgl/lvgl/pull/8888>`__
- **fix(font): fix stride calculation of fonts** `8887 <https://github.com/lvgl/lvgl/pull/8887>`__
- **fix(label): make long dot mode compatible with static text** `8823 <https://github.com/lvgl/lvgl/pull/8823>`__
- **fix(opengles): fix memory leak when shader compilation fails** `8854 <https://github.com/lvgl/lvgl/pull/8854>`__
- **fix(sysmon): handle LV_SYSMON_PROC_IDLE_AVAILABLE disabled in lv_linux** `8871 <https://github.com/lvgl/lvgl/pull/8871>`__
- **fix(profiler): remove macro for external usage** `8894 <https://github.com/lvgl/lvgl/pull/8894>`__
- **fix(span): opt span ellipsis more precise in omitting text** `8864 <https://github.com/lvgl/lvgl/pull/8864>`__
- **fix(file_explorer): ensure current_path is null-terminated after appending '/'** `8797 <https://github.com/lvgl/lvgl/pull/8797>`__
- **fix(gdb): use stride to get actual buffer width for display dump** `8853 <https://github.com/lvgl/lvgl/pull/8853>`__
- **fix(draw/dave2d): prevent deadlock by avoid to flush the GPU buffer when empty.** `8869 <https://github.com/lvgl/lvgl/pull/8869>`__
- **fix(fbdev): fix segfault when using display rotation** `8850 <https://github.com/lvgl/lvgl/pull/8850>`__
- **fix(dmabuf): flicker when the buffers are swapped for new frame** `8563 <https://github.com/lvgl/lvgl/pull/8563>`__
- **fix(label): issue when setting fmt text with long mode dots** `8781 <https://github.com/lvgl/lvgl/pull/8781>`__
- **fix(vglite): arc_path calculation overflow on VGLite** `8847 <https://github.com/lvgl/lvgl/pull/8847>`__
- **fix(draw/dave2d): redefine fixed point macros** `8859 <https://github.com/lvgl/lvgl/pull/8859>`__
- **fix(xml): fix multiple delimiter handling in lv_xml_split_str** `8858 <https://github.com/lvgl/lvgl/pull/8858>`__
- **fix(svg): fix stroke dash style not restore bug.** `8844 <https://github.com/lvgl/lvgl/pull/8844>`__
- **fix(draw/sw): check that malloc doesn't return a NULL pointer** `8756 <https://github.com/lvgl/lvgl/pull/8756>`__
- **fix(wayland): throw error when using dmabuf without 2 draw buffers** `8744 <https://github.com/lvgl/lvgl/pull/8744>`__
- **fix(span): fix text width calculations and improve ellipsis handling** `8821 <https://github.com/lvgl/lvgl/pull/8821>`__
- **fix(tjpgd): Fixed memory leak if opening JPEG image failed.** `8707 <https://github.com/lvgl/lvgl/pull/8707>`__
- **fix(sdl): The default size is fixed** `8808 <https://github.com/lvgl/lvgl/pull/8808>`__
- **fix(opengl): fix multiple issues with shader manipulation** `8832 <https://github.com/lvgl/lvgl/pull/8832>`__
- **fix(draw/convert): fix duplicate c declaration symbol during docs build** `8827 <https://github.com/lvgl/lvgl/pull/8827>`__
- **fix(indev_gesture): fix memory leak when deleting indev** `8814 <https://github.com/lvgl/lvgl/pull/8814>`__
- **fix(cache): make the cache module private** `8793 <https://github.com/lvgl/lvgl/pull/8793>`__
- **fix(gltf): add missing initialization call for viewer attribute** `8807 <https://github.com/lvgl/lvgl/pull/8807>`__
- **fix(label): preserve recolor state across line wrapping** `8755 <https://github.com/lvgl/lvgl/pull/8755>`__
- **fix(draw/dma2d): fix freezing in async mode.** `8765 <https://github.com/lvgl/lvgl/pull/8765>`__
- **fix(draw/pxp): fix alignment issue on cache flush.** `8764 <https://github.com/lvgl/lvgl/pull/8764>`__
- **fix(profiler): fix crash during lv_deinit** `8788 <https://github.com/lvgl/lvgl/pull/8788>`__
- **fix(freetype): fix crash during lv_deinit** `8787 <https://github.com/lvgl/lvgl/pull/8787>`__
- **fix(lodepng): check that src is not null before calling memcpy** `8791 <https://github.com/lvgl/lvgl/pull/8791>`__
- **fix(jpeg): fix to use source stride when copy jpeg** `8778 <https://github.com/lvgl/lvgl/pull/8778>`__
- **fix(style): format error in log call** `8754 <https://github.com/lvgl/lvgl/pull/8754>`__
- **fix(test_span): remove the reference before deleting the font** `8753 <https://github.com/lvgl/lvgl/pull/8753>`__
- **fix(event): don't try to trickle to deleted widgets** `8777 <https://github.com/lvgl/lvgl/pull/8777>`__
- **fix(vg_lite): fix rotated vector font clipping error** `8739 <https://github.com/lvgl/lvgl/pull/8739>`__
- **fix(EVE): rounded arcs with transparency** `8767 <https://github.com/lvgl/lvgl/pull/8767>`__
- **fix(image): reset img when src_type is unknow or info is wrong** `8716 <https://github.com/lvgl/lvgl/pull/8716>`__
- **fix(image_decoder): fix memory leaks caused by multi-core rendering** `8735 <https://github.com/lvgl/lvgl/pull/8735>`__
- **fix(image): fix unused var warning when LV_LOG_WARN isn't available** `8591 <https://github.com/lvgl/lvgl/pull/8591>`__
- **fix(tests): fix gcovr report generation failure** `8727 <https://github.com/lvgl/lvgl/pull/8727>`__
- **fix(canvas): add the missing D-Cache flush** `8736 <https://github.com/lvgl/lvgl/pull/8736>`__
- **fix(test_cache): fix access to freed stack variables** `8717 <https://github.com/lvgl/lvgl/pull/8717>`__
- **fix(draw_span): add missing line_space attribute to dsc and remaining_len check** `8718 <https://github.com/lvgl/lvgl/pull/8718>`__
- **fix(drivers): fix compile error for st ltdc** `8704 <https://github.com/lvgl/lvgl/pull/8704>`__
- **fix(scale): initialize label size if text is NULL** `8699 <https://github.com/lvgl/lvgl/pull/8699>`__
- **fix(evdev): add support for multi-touch** `8173 <https://github.com/lvgl/lvgl/pull/8173>`__
- **fix(SVG): fix SVG decoder memory leak. close decoder** `8658 <https://github.com/lvgl/lvgl/pull/8658>`__
- **fix(eve): fix image double ended clipping in EVE** `8675 <https://github.com/lvgl/lvgl/pull/8675>`__
- **fix(xmls): fixed division lines in Chart XML API** `8681 <https://github.com/lvgl/lvgl/pull/8681>`__
- **fix(EVE): fix EVE build break** `8672 <https://github.com/lvgl/lvgl/pull/8672>`__
- **fix(vg_lite): remove the 16px width limit for image tile drawing mode** `8661 <https://github.com/lvgl/lvgl/pull/8661>`__
- **fix(vg_lite_tvg): fix matrix calculation inconsistency with hardware behavior** `8666 <https://github.com/lvgl/lvgl/pull/8666>`__
- **fix(vg_lite): always use phy_clip_area as the scissor area** `8669 <https://github.com/lvgl/lvgl/pull/8669>`__
- **fix(indev): always assign data-&gt;point to the primary point instead of…** `8596 <https://github.com/lvgl/lvgl/pull/8596>`__
- **fix(draw):fix lv_matrix_transform_point** `8667 <https://github.com/lvgl/lvgl/pull/8667>`__
- **fix(demos/widgets): add missing aligned font resource declarations** `8647 <https://github.com/lvgl/lvgl/pull/8647>`__
- **fix(vg_lite): fix incorrect handling of ARGB8888_PREMULTIPLIED** `8610 <https://github.com/lvgl/lvgl/pull/8610>`__
- **fix(vg_lite_tvg): fix incorrect stride handling** `8648 <https://github.com/lvgl/lvgl/pull/8648>`__
- **fix(textarea): fill letter space with selection bg color.** `8238 <https://github.com/lvgl/lvgl/pull/8238>`__
- **fix(msgbox): fix memory leak and correct LV_ASSERT_MALLOC parameters** `8624 <https://github.com/lvgl/lvgl/pull/8624>`__
- **fix(textarea): label scroll to the cursor pos when textarea size change** `8548 <https://github.com/lvgl/lvgl/pull/8548>`__
- **fix(scripts): Union syntax compatible with Python 3.8** `8641 <https://github.com/lvgl/lvgl/pull/8641>`__
- **fix(font loader): overflow error when calculating ids size** `8617 <https://github.com/lvgl/lvgl/pull/8617>`__
- **fix(svg): cast char to unsigned preventing char subscription errors** `8619 <https://github.com/lvgl/lvgl/pull/8619>`__
- **fix(example/label): make animation repeat indefinitely** `8621 <https://github.com/lvgl/lvgl/pull/8621>`__
- **fix(tabview): emit LV_EVENT_VALUE_CHANGED on header-button taps** `8537 <https://github.com/lvgl/lvgl/pull/8537>`__
- **fix(dropdown): support aligning the dropdown button's text** `8601 <https://github.com/lvgl/lvgl/pull/8601>`__
- **fix(mem): change variable attribute location** `8593 <https://github.com/lvgl/lvgl/pull/8593>`__
- **fix(grid): fix uninitialized var warnings** `8592 <https://github.com/lvgl/lvgl/pull/8592>`__
- **fix(dropdown): check for NULL in dropdown label before dereferencing** `8543 <https://github.com/lvgl/lvgl/pull/8543>`__
- **fix(xml): set correct param type for subject_set_string_event** `8595 <https://github.com/lvgl/lvgl/pull/8595>`__
- **fix(xml): add missing type for lv_arc start/end angles** `8580 <https://github.com/lvgl/lvgl/pull/8580>`__
- **fix(xml): specify element type="void" for event/screen events** `8577 <https://github.com/lvgl/lvgl/pull/8577>`__
- **fix(xml): fix incorrect prop names in lv_arc** `8562 <https://github.com/lvgl/lvgl/pull/8562>`__
- **fix(label): fix label clip overflow** `8557 <https://github.com/lvgl/lvgl/pull/8557>`__
- **fix(NemaGFX): non-grad triangle coords** `8524 <https://github.com/lvgl/lvgl/pull/8524>`__
- **fix(draw_vector): ensure vector_graphic can be used if it's enabled** `8522 <https://github.com/lvgl/lvgl/pull/8522>`__
- **fix(svg): add log for unsupport path command.** `8545 <https://github.com/lvgl/lvgl/pull/8545>`__
- **fix(xml): add missing subject_set_float_event to the schema** `8538 <https://github.com/lvgl/lvgl/pull/8538>`__
- **fix(lv_example_bar_7):  make bar fit in 320x240 box for webpage** `8525 <https://github.com/lvgl/lvgl/pull/8525>`__
- **fix(xml): fix scale section props** `8528 <https://github.com/lvgl/lvgl/pull/8528>`__
- **fix(draw/g2d): use image stride if it's set** `8503 <https://github.com/lvgl/lvgl/pull/8503>`__
- **fix(snapshot): not all children rendered during snapshot** `8479 <https://github.com/lvgl/lvgl/pull/8479>`__
- **fix(indev): set the timestamp only if it was not set in the read_cb** `8501 <https://github.com/lvgl/lvgl/pull/8501>`__
- **fix(wayland): call display_flush_ready on intermediate frames** `8471 <https://github.com/lvgl/lvgl/pull/8471>`__
- **fix(examples): wrap logging code with LV_USE_LOG to avoid unused variable warnings** `8377 <https://github.com/lvgl/lvgl/pull/8377>`__
- **fix(g2d): calculate stride because it may not be set in the image descriptor** `8483 <https://github.com/lvgl/lvgl/pull/8483>`__
- **fix: include v9.2 api map to avoid breaking changes** `8490 <https://github.com/lvgl/lvgl/pull/8490>`__
- **fix(vg_lite): fix grad item not fully initialized** `8486 <https://github.com/lvgl/lvgl/pull/8486>`__
- **fix(draw/sw_mask): fix potential buffer overflow** `8478 <https://github.com/lvgl/lvgl/pull/8478>`__
- **fix(lv_deinit): only call xml test call when both LV_USE_XML and LV_USE_TEST are enabled** `8482 <https://github.com/lvgl/lvgl/pull/8482>`__
- **fix(indev): decouple scroll momentum decay from indev read loop (#6832)** `7794 <https://github.com/lvgl/lvgl/pull/7794>`__
- **fix(draw_sw): fix ARGB8888PM case where the color of dest and src is the same but opa is different.** `8454 <https://github.com/lvgl/lvgl/pull/8454>`__
- **fix(wayland): Avoid  premultiplication at the driver level if LVGL is already rendering in pre-multiplied alpha.** `8456 <https://github.com/lvgl/lvgl/pull/8456>`__
- **fix(dropdown): handle the case where options are null lv_dropdown_add_option** `8338 <https://github.com/lvgl/lvgl/pull/8338>`__
- **fix(xml): handle LV_STATE_USER_1-4** `8451 <https://github.com/lvgl/lvgl/pull/8451>`__
- **fix(assert): fix lv_assert_handler function static analyze complain** `8447 <https://github.com/lvgl/lvgl/pull/8447>`__
- **fix(blend/helium): eliminate duplicate function declarations** `8402 <https://github.com/lvgl/lvgl/pull/8402>`__
- **fix(draw): Fix unused function warning for blending** `8414 <https://github.com/lvgl/lvgl/pull/8414>`__
- **fix(bin_decoder):  don't flush cache in case of `use_directly`** `8411 <https://github.com/lvgl/lvgl/pull/8411>`__
- **fix(svg): fix some svg issues.** `8378 <https://github.com/lvgl/lvgl/pull/8378>`__
- **fix(span): fix correct text length calculation** `8368 <https://github.com/lvgl/lvgl/pull/8368>`__
- **fix(uefi): include configuration header instead of full LVGL header** `8380 <https://github.com/lvgl/lvgl/pull/8380>`__
- **fix(sdl): add malloc check after reallocating memory** `8369 <https://github.com/lvgl/lvgl/pull/8369>`__
- **fix(draw_vector): fix the unprocessed layer opa in vector rendering** `8294 <https://github.com/lvgl/lvgl/pull/8294>`__
- **fix(kconfig): Fix depends clauses for LV_USE_OS** `8392 <https://github.com/lvgl/lvgl/pull/8392>`__
- **fix(thorvg): fix compilation on thread-less platforms** `8373 <https://github.com/lvgl/lvgl/pull/8373>`__
- **fix(thorvg): include alloca on zephyr** `8381 <https://github.com/lvgl/lvgl/pull/8381>`__
- **fix(xml): fix screen names and extended component names** `b441ec3 <https://github.com/lvgl/lvgl/commit/b441ec37fac991403dd226b409c7af87cea1fbec>`__
- **fix(custom.js):  fix to correctly deal with when...** `45e4041 <https://github.com/lvgl/lvgl/commit/45e4041d09a2b55e1bcf9df69685aa32371fea40>`__
- **fix(xml): make all obj elements type=void** `8de083e <https://github.com/lvgl/lvgl/commit/8de083e6f371b95085914f7f28ab276d6813762d>`__
- **fix(xml): minor fixes in parsers** `dea37bd <https://github.com/lvgl/lvgl/commit/dea37bd32fecf4e09b472956a9adb7ded87baba1>`__
- **fix(custom.js):  fix examples code-folding issue...** `706715f <https://github.com/lvgl/lvgl/commit/706715f798f66bbbfcc7fee66b08685585eb9c51>`__
- **fix(xml): find constants in styles in the global scope** `84acb67 <https://github.com/lvgl/lvgl/commit/84acb67dcae2728707dfd6ea5a5eadc8af17b282>`__
- **fix(arc_label): fix the pointer overflow issue in 32-bit mode** `059eb4a <https://github.com/lvgl/lvgl/commit/059eb4a3210b2fc4fa0b3eaf865ee62015804489>`__
- **fix(grid): ignore grids without row or column descriptors** `38000e8 <https://github.com/lvgl/lvgl/commit/38000e8ca25eda3934a4ca5cdf3a61aac760d850>`__
- **fix(xml): process 'align' style property** `ec0e3cf <https://github.com/lvgl/lvgl/commit/ec0e3cfab583a5903a687ce1e7ed354cb350f72e>`__
- **fix(xml): fix prop order of subject_set_*** `f7dc114 <https://github.com/lvgl/lvgl/commit/f7dc1148a98433efb0aba5247ed9539bae15d918>`__
- **fix(xml): fix wrong tag names for subject events** `8599e9b <https://github.com/lvgl/lvgl/commit/8599e9be77a0861144eb483f2c9fb5bbce9039aa>`__
- **fix(xml_animation): use lv_obj_find_child_by_name instead of lv_obj_get_child_by_name** `876dacb <https://github.com/lvgl/lvgl/commit/876dacb881982127169387b94c745b47a059fe53>`__
- **fix(scripts/prerequisites-apt): corrected freetype version number for verification** `dccab75 <https://github.com/lvgl/lvgl/commit/dccab751d6aa4dd5cda00c3794bbc017560a4965>`__
- **fix(properties.py):  fix to comply with adjustments** `3a4cf38 <https://github.com/lvgl/lvgl/commit/3a4cf38ef3acfc93f67126a596050f988eb4a336>`__
- **fix:  add `brotli` module required for `fonttools` to...** `4c6624b <https://github.com/lvgl/lvgl/commit/4c6624b79fea4550e4fa962305b1289f23585d88>`__
- **fix(obj xml): fix typo in style_shadow_width** `42a391d <https://github.com/lvgl/lvgl/commit/42a391df538b9f1932e568a1e9f1bb7140300ff9>`__
- **fix(texarea xml): rename placeholder to placeholder_text** `3f41172 <https://github.com/lvgl/lvgl/commit/3f4117270cbf064f4e1215eceed121259ed31ad7>`__
- **fix(xml): fix event_cb parser typo (trigger_str -&gt; trigger)** `244f9cc <https://github.com/lvgl/lvgl/commit/244f9ccb4ab3fc6ee588261aedb7e4f5276669b8>`__
- **fix(scripts/prerequisites-apt): add missing libsdl2-dev:i386 installation** `c0d08d8 <https://github.com/lvgl/lvgl/commit/c0d08d8dd35bc532efa66b2c86c112d82506f86b>`__
- **fix(keyboard xml): add number mode** `248192f <https://github.com/lvgl/lvgl/commit/248192f4b27a1a2ec016335da5e02c1c6928eea6>`__
- **fix(xml): add missing lv_xml_widget.h include** `6ff3cee <https://github.com/lvgl/lvgl/commit/6ff3cee50a3980fe9a9f9996aa324801dcd08c48>`__
- **fix(xml): fix better handling broken components** `592df6b <https://github.com/lvgl/lvgl/commit/592df6b62dab5233966dc7a6fe8e3bb726155432>`__
- **fix(docs): fix docs build for arclabel example building** `f40e0e5 <https://github.com/lvgl/lvgl/commit/f40e0e5f6019790967452fddf67c9300e78d4264>`__

Examples
~~~~~~~~

- **examples(scale): Compass style rotating scale** `8445 <(https://github.com/lvgl/lvgl/pull/8445)>`__

Docs
~~~~

- **docs(xml): fix the descripton of how to initialize the UI from C** `9070 <https://github.com/lvgl/lvgl/pull/9070>`__
- **docs(README): remove outdated content and add new features** `9023 <https://github.com/lvgl/lvgl/pull/9023>`__
- **docs(xml): Fix array type description count to 3 (#9013)** `9028 <https://github.com/lvgl/lvgl/pull/9028>`__
- **docs: update the esp docs indentation at the FS section.** `9031 <https://github.com/lvgl/lvgl/pull/9031>`__
- **docs(editor): improve installation and figma docs** `8998 <https://github.com/lvgl/lvgl/pull/8998>`__
- **docs(drivers): fix broken links** `9022 <https://github.com/lvgl/lvgl/pull/9022>`__
- **docs(espressif): fix indentation at file system reconfigure project.** `9024 <https://github.com/lvgl/lvgl/pull/9024>`__
- **docs(xml/events): rename page to avoid confusion when searching documentation** `8995 <https://github.com/lvgl/lvgl/pull/8995>`__
- **docs(flags): add chapter explaining how to add/remove multiple flags** `9000 <https://github.com/lvgl/lvgl/pull/9000>`__
- **docs(events): add explanation about lv_event_get_target_obj warning and tips** `8996 <https://github.com/lvgl/lvgl/pull/8996>`__
- **docs(xml): add missing licence files** `8984 <https://github.com/lvgl/lvgl/pull/8984>`__
- **docs(editor): add missing intro pages and minor tweaks** `8983 <https://github.com/lvgl/lvgl/pull/8983>`__
- **docs: split indev, style, and widget basics to multiple pages** `8943 <https://github.com/lvgl/lvgl/pull/8943>`__
- **docs(editor): add XML and C integration guides and minor refactoring** `8954 <https://github.com/lvgl/lvgl/pull/8954>`__
- **docs(xml): add help texts in XMLs** `8961 <https://github.com/lvgl/lvgl/pull/8961>`__
- **docs(pc): add the missing PC simulator docs** `8824 <https://github.com/lvgl/lvgl/pull/8824>`__
- **docs(editor): add cli docs** `8970 <https://github.com/lvgl/lvgl/pull/8970>`__
- **docs(snapshot): complementary color format** `8780 <https://github.com/lvgl/lvgl/pull/8780>`__
- **docs(xml): add online share docs** `8973 <https://github.com/lvgl/lvgl/pull/8973>`__
- **docs(editor):  add docs specific to editor** `8972 <https://github.com/lvgl/lvgl/pull/8972>`__
- **docs(style): update font sizes and weights for improved readability** `8968 <https://github.com/lvgl/lvgl/pull/8968>`__
- **docs(events): fix grammatical issue** `8782 <https://github.com/lvgl/lvgl/pull/8782>`__
- **docs (frameworks): PlatformIO** `8942 <https://github.com/lvgl/lvgl/pull/8942>`__
- **docs(xml): add placeholder for editor docs** `8910 <https://github.com/lvgl/lvgl/pull/8910>`__
- **docs(misc):  fix doc issues described in issue #8839** `8857 <https://github.com/lvgl/lvgl/pull/8857>`__
- **docs(timer/indev): add example section about indev interrupts resuming the indev timer** `8843 <https://github.com/lvgl/lvgl/pull/8843>`__
- **docs(opengl): add opengl overview page** `8838 <https://github.com/lvgl/lvgl/pull/8838>`__
- **docs(xml): reorganize and add better descriptions** `8818 <https://github.com/lvgl/lvgl/pull/8818>`__
- **docs(espressif): update esp32 documentation** `8830 <https://github.com/lvgl/lvgl/pull/8830>`__
- **docs(Kconfig):  clarify comments about driver-identifier letter** `8851 <https://github.com/lvgl/lvgl/pull/8851>`__
- **docs(misc):  fix multiple formatting errors and typos** `8738 <https://github.com/lvgl/lvgl/pull/8738>`__
- **docs(misc):  fix formatting errors, typos and missing reference** `8748 <https://github.com/lvgl/lvgl/pull/8748>`__
- **docs(style):  fix arrows in code blocks** `8819 <https://github.com/lvgl/lvgl/pull/8819>`__
- **docs: fix indev.rst typo** `8721 <https://github.com/lvgl/lvgl/pull/8721>`__
- **docs(xml):  next set of XML-doc refinements** `8740 <https://github.com/lvgl/lvgl/pull/8740>`__
- **docs(STM32): simplify and rewrite** `8768 <https://github.com/lvgl/lvgl/pull/8768>`__
- **docs(mipi): align return type of callbacks in documentation with code** `8803 <https://github.com/lvgl/lvgl/pull/8803>`__
- **docs(integration): reorganize the files** `8701 <https://github.com/lvgl/lvgl/pull/8701>`__
- **docs(wayland): use correct window create function name** `8731 <https://github.com/lvgl/lvgl/pull/8731>`__
- **docs:  miscellaneous improvements** `8552 <https://github.com/lvgl/lvgl/pull/8552>`__
- **docs(image.rst):  remove reference to True Color which was removed in v9.0** `8700 <https://github.com/lvgl/lvgl/pull/8700>`__
- **docs(misc):  fix several issues:** `8714 <https://github.com/lvgl/lvgl/pull/8714>`__
- **docs(keyboard): fix typos and unify comments** `8391 <https://github.com/lvgl/lvgl/pull/8391>`__
- **docs(misc):  fix several issues** `8698 <https://github.com/lvgl/lvgl/pull/8698>`__
- **docs(api_doc_builder.py):  fix frail assumptions in API-page generation** `8694 <https://github.com/lvgl/lvgl/pull/8694>`__
- **docs(xml):  results of minor amount of proofreading** `8604 <https://github.com/lvgl/lvgl/pull/8604>`__
- **docs(drivers): Update st7796.rst URL** `8687 <https://github.com/lvgl/lvgl/pull/8687>`__
- **docs(image): document the fact that indexed data is stored as argb8888** `8620 <https://github.com/lvgl/lvgl/pull/8620>`__
- **docs: update docs styling** `8585 <https://github.com/lvgl/lvgl/pull/8585>`__
- **docs(custom.css):  fix text color on view-on-github example buttons** `8652 <https://github.com/lvgl/lvgl/pull/8652>`__
- **docs(custom.css):  Fix field list items being jammed together** `8639 <https://github.com/lvgl/lvgl/pull/8639>`__
- **docs: remove outdated references to LV_ZOOM_IMAGE_NONE** `8632 <https://github.com/lvgl/lvgl/pull/8632>`__
- **docs(LTDC): Clarify LTDC framebuffer address meaning** `8615 <https://github.com/lvgl/lvgl/pull/8615>`__
- **docs(list formatting):  fix erroneous list formatting** `8602 <https://github.com/lvgl/lvgl/pull/8602>`__
- **docs(readme): fix broken links** `8605 <https://github.com/lvgl/lvgl/pull/8605>`__
- **docs(style):  fix definition-list indentation** `8616 <https://github.com/lvgl/lvgl/pull/8616>`__
- **docs(intro):  improve initial presentation of LVGL...** `8597 <https://github.com/lvgl/lvgl/pull/8597>`__
- **docs(renesas): mention relevant information about FSP 6.0** `8572 <https://github.com/lvgl/lvgl/pull/8572>`__
- **docs: fix wording in display event, viewe and renesas** `8546 <https://github.com/lvgl/lvgl/pull/8546>`__
- **docs(home): fix links** `8560 <https://github.com/lvgl/lvgl/pull/8560>`__
- **docs(intro): reorganize and simplify** `8389 <https://github.com/lvgl/lvgl/pull/8389>`__
- **docs:  remove outdated doc material** `8520 <https://github.com/lvgl/lvgl/pull/8520>`__
- **docs(xml): restructure and extend the XML docs** `8495 <https://github.com/lvgl/lvgl/pull/8495>`__
- **docs(Renesas): Add Renesas RA8P1 docs** `8509 <https://github.com/lvgl/lvgl/pull/8509>`__
- **docs(readme): add Hebrew language section to documentation** `8345 <https://github.com/lvgl/lvgl/pull/8345>`__
- **docs(wayland): add missing documentation about its config options** `8476 <https://github.com/lvgl/lvgl/pull/8476>`__
- **docs: fix spacing between same-level sections and h4 and link colors** `8443 <https://github.com/lvgl/lvgl/pull/8443>`__
- **docs(lv_conf): fix links** `8462 <https://github.com/lvgl/lvgl/pull/8462>`__
- **docs(1-level TOC styling):  multiple fixes...** `8431 <https://github.com/lvgl/lvgl/pull/8431>`__
- **docs: Fix several typos and punctuation** `8427 <https://github.com/lvgl/lvgl/pull/8427>`__
- **docs(calendar): fix typo** `8413 <https://github.com/lvgl/lvgl/pull/8413>`__
- **docs(boards): Add Viewe Knob Display links** `8461 <https://github.com/lvgl/lvgl/pull/8461>`__
- **docs(doc-build warnings):  fix warnings caused by LV_FORMAT_ATTRIBUTE** `8384 <https://github.com/lvgl/lvgl/pull/8384>`__
- **docs(xml): add license for the XML format** `8438 <https://github.com/lvgl/lvgl/pull/8438>`__
- **docs(observer): fix typo in parameter description** `8372 <https://github.com/lvgl/lvgl/pull/8372>`__
- **docs(custom.css):  partially address code-highlighting problem...** `8374 <https://github.com/lvgl/lvgl/pull/8374>`__
- **docs(CHANGELOG.rst):  fix link to SVG support page** `8356 <https://github.com/lvgl/lvgl/pull/8356>`__
- **docs(CHANGELOG.rst)  replace `kisvegabor/lvgl_upstream` with...** `9cf1e38 <https://github.com/lvgl/lvgl/commit/9cf1e38a9022f594c9c2d037af4d94db79bf336f>`__
- **docs: generate changelog** `05d8b9e <https://github.com/lvgl/lvgl/commit/05d8b9e90fc37a209d680ac9087f44c1e119d91d>`__
- **docs: Lot of smaller improvements based on feedback** `2b9592e <https://github.com/lvgl/lvgl/commit/2b9592e8ddfaf4537f0b5106711df3be0e3f96c6>`__
- **docs(custom.js): The custom.js file now contains the version selector controller JavaScript and other methods that were previously directly added to the main layout.** `8541a11 <https://github.com/lvgl/lvgl/commit/8541a11ecc79a526c7a8ebd3e805403214ddc25c>`__
- **docs(custom.css): enhance styling and add custom CSS variables** `4f843e8 <https://github.com/lvgl/lvgl/commit/4f843e8cd427a103675ede6c950096947383c4dd>`__
- **docs(data_binding): move the data binding docs to to the given widget's page** `f6f6570 <https://github.com/lvgl/lvgl/commit/f6f657072ee3f6fde848d225e1cd12a25a6aa74d>`__
- **docs:  add dynamic announcements from remote .JSON** `77cfa87 <https://github.com/lvgl/lvgl/commit/77cfa87e7d0e8b03875694b05331230cb4eccb40>`__
- **docs(furo):  convert to furo theme...** `2129d37 <https://github.com/lvgl/lvgl/commit/2129d37bca039c0f378d3743a412ccc40e9b2234>`__
- **docs: Enhance sidebar, remove jQuery dependency** `5b3d6d8 <https://github.com/lvgl/lvgl/commit/5b3d6d89f8b4298e4133c5db3b3b0574db71bf4e>`__
- **docs(misc):  eliminate several Sphinx warnings** `b74ac71 <https://github.com/lvgl/lvgl/commit/b74ac71ed0d2d51fa4bb80be61d1e033c27662c3>`__
- **docs(Sphinx Warnings):  add name to anonymous enums to...** `55ab01e <https://github.com/lvgl/lvgl/commit/55ab01ec733ddab69b75fe495bd0c438fc76d0ef>`__
- **docs(icons.html): Update SVG icons for improved accessibility and consistency** `a13113b <https://github.com/lvgl/lvgl/commit/a13113bafd51b1eb0507ce15b653c9ae835b1632>`__
- **docs(arc_label): add arc label docs** `8487b4c <https://github.com/lvgl/lvgl/commit/8487b4c75bdc88e5ed36abffd6910c38a4231467>`__
- **docs:  fixed several problems...** `4533d29 <https://github.com/lvgl/lvgl/commit/4533d2918ae45cd342fc23e2fa89fa47816ee1b2>`__
- **docs(misc):  minor improvements to:** `450a390 <https://github.com/lvgl/lvgl/commit/450a3902ecdd2bf6cf8fb8c8a1f8a89c84d2033a>`__
- **docs(doxygen_xml.py):  exclude files that...** `e92ce1f <https://github.com/lvgl/lvgl/commit/e92ce1f8f63add95300af682426eb1d93cdaad0f>`__
- **docs(enum-naming):  adjustments per conversation with Gábor** `5bc21a4 <https://github.com/lvgl/lvgl/commit/5bc21a4b1856a115e7bbbee56eacfaf190668b61>`__
- **docs(logo): update dark and light logos** `44a369a <https://github.com/lvgl/lvgl/commit/44a369a9f7a7b79775c3fda6dedfc96d94b9a972>`__
- **docs(custom.css):  format field lists** `45575c0 <https://github.com/lvgl/lvgl/commit/45575c0fafe77f44894811b0f1454cd74a995c0e>`__
- **docs(observer): fix minor typos** `0594775 <https://github.com/lvgl/lvgl/commit/05947756866b7482b2ab8c5eaebe6df90b272d1a>`__
- **docs(API/index.rst):  eliminate Sphinx warnings when...** `0829150 <https://github.com/lvgl/lvgl/commit/0829150e568e12ee3554b35dd0e64acdf9e3ec5f>`__
- **docs(data_binding): fix typos** `db71f48 <https://github.com/lvgl/lvgl/commit/db71f482dbd1b535b73a7022d6ffc37ef13af79f>`__
- **docs(CHANGELOG.rst):  fix parsing error on 8 boldface strings** `82ff3c2 <https://github.com/lvgl/lvgl/commit/82ff3c297f13712f4482849a30e9df44791c38c7>`__
- **docs(custom.css): Refactor link color variables for improved theming consistency** `f725034 <https://github.com/lvgl/lvgl/commit/f72503481ac407c00204ae643610f67f380eddda>`__
- **docs(observer): fix subject name in the example** `8003908 <https://github.com/lvgl/lvgl/commit/8003908e9f9bacd329583ea7b7243eb329b295e2>`__
- **docs(properties.py):  fix several problems...** `f3370d0 <https://github.com/lvgl/lvgl/commit/f3370d01c9855d3384a3a139b47f7c46fad21e1d>`__
- **docs(index.rst): Add `hide-toc` furo theme prop to hide sidebar on the homepage and remove main header image.** `af2c420 <https://github.com/lvgl/lvgl/commit/af2c420ae583c05b266409f6a6931fe3d4d3d313>`__
- **docs(style): add example values to lv_style_selector_t** `6bcdf3c <https://github.com/lvgl/lvgl/commit/6bcdf3c8068e17df01060b94bf0b7cc775e25351>`__
- **docs(observer): minor grammar corrections** `989ac6c <https://github.com/lvgl/lvgl/commit/989ac6cc15a3916bd13c56db09a74b620f9bdab2>`__
- **docs:  (custom.css):  add caption feature and...** `6f26b71 <https://github.com/lvgl/lvgl/commit/6f26b71d95a20c0cf4fa7555410e9cc91ce82d67>`__
- **docs(build.py):  make Sphinx exit with status 1 when new warnings show up** `ef4aa46 <https://github.com/lvgl/lvgl/commit/ef4aa469bf13006fb0aeee21b805045dc45b7efd>`__
- **docs(os/index.rst):  put TOC back in apha sequence** `c994e5c <https://github.com/lvgl/lvgl/commit/c994e5c78ed8effb2ae00ac2ba79c755638e207a>`__
- **docs(custom.css): Fix theme toggle button label color** `bc4b55c <https://github.com/lvgl/lvgl/commit/bc4b55c4b4dd57f05a10ae85182440b4ac250482>`__
- **docs(Makefile):  fix typo for Linux sphinx-build after intermediate directory created** `79e55d4 <https://github.com/lvgl/lvgl/commit/79e55d46b9770aad7f0b516a776a4f8a702209c7>`__
- **docs(lv_obj.h): make enum name consistent with enumerators** `3d1b533 <https://github.com/lvgl/lvgl/commit/3d1b5338cf7ab3684c2c5fb1ff367bd6fbdacccd>`__
- **docs(lv_obj.h): make enum name consistent with enumerators** `ffb29b0 <https://github.com/lvgl/lvgl/commit/ffb29b02c99619928abd8d2bd69b1105caf9b0fb>`__
- **docs(lv_svg.h): fix build of `lv_svg.c`** `cb76aaa <https://github.com/lvgl/lvgl/commit/cb76aaa3d90ab84d1e67cee95f798b16cd4fac7e>`__
- **docs(image.rst):  fix doc-gen warning** `70f8c52 <https://github.com/lvgl/lvgl/commit/70f8c522a91cdecd601ecec3fff0e1d02184d0cd>`__
- **docs(unneeded):  eliminate unneeded docs....** `d2d2acc <https://github.com/lvgl/lvgl/commit/d2d2acca11d2d3f92a48ae558e1c7b0d50649562>`__

CI and tests
~~~~~~~~~~~~

- **ci(release_updater): some release_updater fixes and conf.defaults deprecated configs are non-fatal** `9076 <https://github.com/lvgl/lvgl/pull/9076>`__
- **ci(release_updater): fix 2 port updater URLs** `9074 <https://github.com/lvgl/lvgl/pull/9074>`__
- **ci(pio):  fix the PlatformIO publisher action** `9073 <https://github.com/lvgl/lvgl/pull/9073>`__
- **ci(docs): fix docs ci by fetching lvgl in default gh workspace folder** `9053 <https://github.com/lvgl/lvgl/pull/9053>`__
- **ci(docs): fix docs CI. use the action's checked-out repo for emscripten** `9020 <https://github.com/lvgl/lvgl/pull/9020>`__
- **test(docker): add compatibility to tests' dockerfile for arm platforms** `9015 <https://github.com/lvgl/lvgl/pull/9015>`__
- **ci(docs): fix examples build error when not rebased fully** `8880 <https://github.com/lvgl/lvgl/pull/8880>`__
- **ci(test): generate the I1 test images with pngquant 2.17** `8892 <https://github.com/lvgl/lvgl/pull/8892>`__
- **test(bin_decoder): add lv_bin_decoder test case** `8867 <https://github.com/lvgl/lvgl/pull/8867>`__
- **ci(stalebot): increase stale and close times to 60 and 14 days** `8840 <https://github.com/lvgl/lvgl/pull/8840>`__
- **ci(hw/comment): gracefully exit workflow if triggered by another label** `8828 <https://github.com/lvgl/lvgl/pull/8828>`__
- **test(images): update test images according to LVGLImage script** `8790 <https://github.com/lvgl/lvgl/pull/8790>`__
- **ci(install): add cmake installation workflow** `8794 <https://github.com/lvgl/lvgl/pull/8794>`__
- **tests: add LV_DRAW_BUF_STRIDE_ALIGN = 1 test** `8804 <https://github.com/lvgl/lvgl/pull/8804>`__
- **test: fix syntax warning by using invalid escape sequence '\.'** `8792 <https://github.com/lvgl/lvgl/pull/8792>`__
- **ci(deps): bump actions/checkout from 4 to 5** `8796 <https://github.com/lvgl/lvgl/pull/8796>`__
- **ci:(port_updater): add more CI port updater urls** `8686 <https://github.com/lvgl/lvgl/pull/8686>`__
- **tests: rename makefile directory to makefiles.** `8646 <https://github.com/lvgl/lvgl/pull/8646>`__
- **ci: add hardware performance test workflow** `8567 <https://github.com/lvgl/lvgl/pull/8567>`__
- **ci: find and edit existing comment for benchmark results** `8531 <https://github.com/lvgl/lvgl/pull/8531>`__
- **ci: disable benchmark results comment if there's no diff** `8515 <https://github.com/lvgl/lvgl/pull/8515>`__
- **ci: emulated perf workflow** `7949 <https://github.com/lvgl/lvgl/pull/7949>`__
- **ci(lv_templ): fix filename template mismatches and add CI** `8326 <https://github.com/lvgl/lvgl/pull/8326>`__
- **test(perf): add perf test framework** `8064 <https://github.com/lvgl/lvgl/pull/8064>`__
- **test(arc_label): add arc_label test cases** `9cf663e <https://github.com/lvgl/lvgl/commit/9cf663ed37ec397c24fbfc5c8caa86cef2ac52f2>`__
- **test(qrcode): add qrcode quiet zone test** `bdcb291 <https://github.com/lvgl/lvgl/commit/bdcb291b7f6e9f30a6b54cab3b746ba60c230cc3>`__
- **ci: enable LV_USE_OBJ_NAME as it's required for some XML features** `297aeae <https://github.com/lvgl/lvgl/commit/297aeae7c7c03e76b8394e3cc3f7bac9dcf6d040>`__
- **ci(hw): add missing fontconfig lib dependency for ejlv** `f5fe4f2 <https://github.com/lvgl/lvgl/commit/f5fe4f220710a24767ef2b60eee661a359b07456>`__
- **ci: correctly pull the latest results in case it's a PR** `ac36b29 <https://github.com/lvgl/lvgl/commit/ac36b292b600c853d3a7ac35a8d006f08bd6f7e2>`__
- **ci(perf/build/hw): remove checkout step from workflow** `7081b45 <https://github.com/lvgl/lvgl/commit/7081b45faa769888d229ae01c8169da0cfbe5ee2>`__
- **ci(hw): increase job timeout** `826d167 <https://github.com/lvgl/lvgl/commit/826d16779806c3616be33a30c42d07b783188715>`__
- **ci(build/hw): disable relunctant hw builds on pushes to master** `f6198e6 <https://github.com/lvgl/lvgl/commit/f6198e6ab160bd21fc15b10ffe291527d68857e8>`__
- **ci: update lv_perf image version to a more stable version** `29eac60 <https://github.com/lvgl/lvgl/commit/29eac60c47b237d645e77be9fc50908b1dc043d3>`__
- **ci(perf/hw): correctly pass github token to command** `0b6cab8 <https://github.com/lvgl/lvgl/commit/0b6cab89f9e4755d0c96e590537a96b76c1d2457>`__
- **ci: generate a release body for the benchmark results pre-release** `ea86afd <https://github.com/lvgl/lvgl/commit/ea86afdfa417fcd521b9aaf33fe9e01e803a81c1>`__

Others
~~~~~~

- **chore(cmsis-pack) update cmsis-pack for the last PRs of v9.4.0** `9077 <https://github.com/lvgl/lvgl/pull/9077>`__
- **chore(cmsis-pack): Update for v.9.4.0** `8945 <https://github.com/lvgl/lvgl/pull/8945>`__
- **chore(conf): remove redundant thorvg definition** `9067 <https://github.com/lvgl/lvgl/pull/9067>`__
- **chore: fix examples not showing on docs by fixing path to examplelist.c** `9059 <https://github.com/lvgl/lvgl/pull/9059>`__
- **refactor(style): remove dead code** `9040 <https://github.com/lvgl/lvgl/pull/9040>`__
- **build(cmake): fix MSVC exports for shared/static libraries on Windows** `9026 <https://github.com/lvgl/lvgl/pull/9026>`__
- **build(kconfig): add missing argb8888 premultiplied setting** `8997 <https://github.com/lvgl/lvgl/pull/8997>`__
- **chore(deps): bump actions/setup-python from 5 to 6** `8985 <https://github.com/lvgl/lvgl/pull/8985>`__
- **chore(deps): bump actions/stale from 9 to 10** `8986 <https://github.com/lvgl/lvgl/pull/8986>`__
- **chore(deps): bump actions/checkout from 4 to 5** `8987 <https://github.com/lvgl/lvgl/pull/8987>`__
- **chore(deps): bump actions/github-script from 7 to 8** `8988 <https://github.com/lvgl/lvgl/pull/8988>`__
- **chore(font): add missing special characters to aligned fonts** `8975 <https://github.com/lvgl/lvgl/pull/8975>`__
- **chore(xml): add API docs to lv_xml_update_from_data** `8967 <https://github.com/lvgl/lvgl/pull/8967>`__
- **chore: fix lv_font_montserrat_14_aligned.c include** `8951 <https://github.com/lvgl/lvgl/pull/8951>`__
- **refactor(opengles): mainline usage of shader manager in opengl driver** `8836 <https://github.com/lvgl/lvgl/pull/8836>`__
- **refactor(gdb): lv_global supports lazy loading and refactors some structures and formats.** `8939 <https://github.com/lvgl/lvgl/pull/8939>`__
- **refactor(vector): clean up the API of vector drawing** `8922 <https://github.com/lvgl/lvgl/pull/8922>`__
- **chore(xml): add guards around widget parsers** `8932 <https://github.com/lvgl/lvgl/pull/8932>`__
- **refactor(gdb): re-structure lvglgdb module** `8920 <https://github.com/lvgl/lvgl/pull/8920>`__
- **chore(tests): fix typo in qrcode test** `8900 <https://github.com/lvgl/lvgl/pull/8900>`__
- **chore(lvconf_gen): generate empty lv_conf.h if it doesn't exist** `8885 <https://github.com/lvgl/lvgl/pull/8885>`__
- **chore(repo_updater): add missing boards to the repo updater** `8856 <https://github.com/lvgl/lvgl/pull/8856>`__
- **refactor(sysmon): polish the UI with LV_SYSMON_PROC_IDLE_AVAILABLE** `8519 <https://github.com/lvgl/lvgl/pull/8519>`__
- **chore(kconfig): add missing LV_OS_SDL2 option to LV_USE_OS** `8831 <https://github.com/lvgl/lvgl/pull/8831>`__
- **refactor(opengl): move opengl shader manager to opengles driver** `8829 <https://github.com/lvgl/lvgl/pull/8829>`__
- **chore(font): replace arial with liberationsans** `8784 <https://github.com/lvgl/lvgl/pull/8784>`__
- **scripts: add the frdm-mcxn-947 to the branch updater script** `8752 <https://github.com/lvgl/lvgl/pull/8752>`__
- **chore: remove multiple semicolons** `8723 <https://github.com/lvgl/lvgl/pull/8723>`__
- **Fixes  bug where multiple events with same callback couldn't be removed in single call.** `8706 <https://github.com/lvgl/lvgl/pull/8706>`__
- **chore(deps): bump dawidd6/action-download-artifact from 9 to 11** `8654 <https://github.com/lvgl/lvgl/pull/8654>`__
- **build(zephyr): remove zephyr env cmake** `8695 <https://github.com/lvgl/lvgl/pull/8695>`__
- **chore(stale): ping maintainers on stale bot comments** `8683 <https://github.com/lvgl/lvgl/pull/8683>`__
- **chore(indev): explain why the timer is resumed on press in event driven mode** `8660 <https://github.com/lvgl/lvgl/pull/8660>`__
- **chore(configopts): add support for LV_*_USE_* defines** `8575 <https://github.com/lvgl/lvgl/pull/8575>`__
- **refactor(text): use struct to pass params to lv_text_* and lv_font_* functions** `8500 <https://github.com/lvgl/lvgl/pull/8500>`__
- **chore(examples/scroll): remove unnecessary chinese comments that cause VC compilation failure** `8668 <https://github.com/lvgl/lvgl/pull/8668>`__
- **Revert "feat(vg_lite): adapt premultiply src over blend mode (#6062)"** `8649 <https://github.com/lvgl/lvgl/pull/8649>`__
- **chore(lv_tree): Code reorganization for lv_tree.h** `8583 <https://github.com/lvgl/lvgl/pull/8583>`__
- **refactor(lv_part_t/lv_state_t):  refactor to align types with prototypes** `8511 <https://github.com/lvgl/lvgl/pull/8511>`__
- **refactor(draw): use better names in lv_draw_task_state_t and handle queued vs in_progress** `8512 <https://github.com/lvgl/lvgl/pull/8512>`__
- **Replace legacy v9.0 call** `8633 <https://github.com/lvgl/lvgl/pull/8633>`__
- **build(cmake): only set project name if lvgl's cmakelists is root** `8588 <https://github.com/lvgl/lvgl/pull/8588>`__
- **chore(test/perf): add ability to debug perf tests** `8569 <https://github.com/lvgl/lvgl/pull/8569>`__
- **build(preprocess): replace `which` with shutil.which to provide a cross-platform compatible solution to find pcpp** `8530 <https://github.com/lvgl/lvgl/pull/8530>`__
- **build(esp): fix lvgl components requirements** `8527 <https://github.com/lvgl/lvgl/pull/8527>`__
- **chore(deps): bump dawidd6/action-download-artifact from 9 to 11** `8513 <https://github.com/lvgl/lvgl/pull/8513>`__
- **chore: use filename to better match mpk and json files** `8518 <https://github.com/lvgl/lvgl/pull/8518>`__
- **chore: fix typos again** `8295 <https://github.com/lvgl/lvgl/pull/8295>`__
- **chore(version): update version numbers to v9.4.0-dev** `8498 <https://github.com/lvgl/lvgl/pull/8498>`__
- **refactor(themes): Improved consistency between styles** `8346 <https://github.com/lvgl/lvgl/pull/8346>`__
- **chore(xml): fix mismatches between lv_obj elements and their XML schema** `8485 <https://github.com/lvgl/lvgl/pull/8485>`__
- **chore(api_doc_builder):  fix TOC ordering on API pages.** `8421 <https://github.com/lvgl/lvgl/pull/8421>`__
- **chore(NemaGFX): add NemaPVG lib binaries** `8430 <https://github.com/lvgl/lvgl/pull/8430>`__
- **chore(refr): advance send  LV_EVENT_RENDER_START** `8457 <https://github.com/lvgl/lvgl/pull/8457>`__
- **chore(lvglimage): fix indexed image premultiply error** `8367 <https://github.com/lvgl/lvgl/pull/8367>`__
- **chore(api_doc_builder): fix regex for unix systems** `8342 <https://github.com/lvgl/lvgl/pull/8342>`__
- **chore(image): reduce warning level for src being NULL** `8394 <https://github.com/lvgl/lvgl/pull/8394>`__
- **refactor(drm): rename config allowing us to link with GBM automatically** `8203 <https://github.com/lvgl/lvgl/pull/8203>`__
- **refactor(xml): use single properties and introduce the prop-paramX syntax** `3e61613 <https://github.com/lvgl/lvgl/commit/3e61613fddaad3e23524c854df6ba63872201f75>`__
- **refactor(observer): move bindings to the related widgets** `26fd3e7 <https://github.com/lvgl/lvgl/commit/26fd3e7574a8e68fda56562a32fa62c0e5af510d>`__
- **chore(arc_label): rename arc_label to arclabel** `9489759 <https://github.com/lvgl/lvgl/commit/94897599d2c56c1e0974955f4cd2a6e69e7be83c>`__
- **initial commit** `1906242 <https://github.com/lvgl/lvgl/commit/1906242741cddea3fb680d5f15822a18d888e467>`__
- **chore(doc-build):  adjustments after discussion with André.** `6d546a8 <https://github.com/lvgl/lvgl/commit/6d546a8695aa415c3a527ca163597706bbb631c5>`__
- **chore: minor fixed in XML and observer related codes** `b4c9d2e <https://github.com/lvgl/lvgl/commit/b4c9d2e690adaa18b4fcc788783f0fa59fb20708>`__
- **chore(xml): memzero malloced memoried for consitent behavior** `584bac6 <https://github.com/lvgl/lvgl/commit/584bac61995b54c17e6ef0e9cb6650f32020825e>`__
- **chore: minor formatting and optimizations** `b946eb6 <https://github.com/lvgl/lvgl/commit/b946eb6294d82dea0917589f1d8572d2503238f5>`__
- **refactor(arc_label): improve angle calculations and type safety** `6cbf36a <https://github.com/lvgl/lvgl/commit/6cbf36afaa8adf820f0e1cc5793391280663fdf9>`__
- **chore(observer): add _event suffix to add_subject_set/increment** `bd9e9d2 <https://github.com/lvgl/lvgl/commit/bd9e9d2f49fe8a3c47c60f2cd217d0ffd7a854f5>`__
- **chore(arc_label): documentation, formatting, redundant code fixes** `cb593b3 <https://github.com/lvgl/lvgl/commit/cb593b35ead6b95b311ebfbc6ac76d0a575355c8>`__
- **chore(xml): make element access="bind" -&gt; "custom" for more flexibility** `a134a56 <https://github.com/lvgl/lvgl/commit/a134a56995dfab16dc28a3f8fb3c1a3cc4c69366>`__
- **revert: revert examples** `272da82 <https://github.com/lvgl/lvgl/commit/272da820515fa20b514d89ee47cd55d306fe3ec1>`__
- **chore(doc-build warnings):  struct adjustments to work around Breathe bug** `5d7b2e7 <https://github.com/lvgl/lvgl/commit/5d7b2e7dcf2cdc30d02df9fc74dad26fa39edca0>`__
- **chore: update version** `07271d5 <https://github.com/lvgl/lvgl/commit/07271d5ccd7fea9e56fadefcc44398af48a5cde4>`__
- **chore(lv_obj.h):  fix per discussion** `6639c31 <https://github.com/lvgl/lvgl/commit/6639c31f5a8182342bfaf9b777725d52a57a8d22>`__
