.. _changelog:

Change Log
==========

`v9.5.0 <https://github.com/lvgl/lvgl/compare/v9.4.0...v9.5.0>`__ 18 February 2026
---------------------------------------------------------------------------------------------------------------------------------------------------

LVGL v9.5.0 continues expanding MPU capabilities while maintaining the lightness and efficiency that make LVGL viable on resource-constrained MCUs. This release adds native blur and drop shadow rendering, a rewritten Wayland driver with EGL support, complete OpenGL rendering via NanoVG, and significant 3D rendering improvements.

Main Features
~~~~~~~~~~~~~

- **Blur and Drop Shadow**. Native software blur and drop shadow support work on all targets without requiring a GPU. Drop shadows add depth to overlays and cards; blur enables frosted-glass effects and background dimming. :ref:`Learn more <style_properties>`.
- **Wayland Driver Rewrite**. Complete rewrite with SHM backend supporting double-buffered direct mode rendering and a new EGL backend for hardware-accelerated OpenGL. :ref:`Learn more <wayland_driver>`.
- **NanoVG Rendering Backend**. Complete OpenGL draw unit providing GPU-accelerated vector rendering, anti-aliased shapes, and compositing. Auto-initializes when OpenGL context is available. :ref:`Learn more <nanovg_draw_unit>`.
- **Bézier Curved Charts**. New ``LV_CHART_TYPE_CURVE`` draws smooth Bézier curves between data points. Requires ``LV_USE_VECTOR_GRAPHICS`` and a compatible draw unit (VGLite, NanoVG or Software with ThorVG). :ref:`Learn more <lv_chart>`.

GPU Acceleration & Performance
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- **RISC-V Vector Extension Support**. SIMD acceleration for software rendering on RISC-V platforms with V extension support. Speeds up blending operations. Enable with ``LV_USE_DRAW_SW_ASM`` set to ``LV_DRAW_SW_ASM_RISCV_V``.
- **SDL EGL Support**. SDL now supports EGL rendering backend, completing EGL coverage across SDL, Wayland, DRM, and GLFW for consistent hardware acceleration.
- **OpenGL Improvements**. Matrix transformations, RGBA-only texture format, GLSL 330 support, and scissor optimization for opaque fills.

3D Rendering
~~~~~~~~~~~~

- **Runtime glTF Manipulation**. Read and modify glTF model nodes at runtime including scale, rotation, translation, and animation state for dynamic 3D scenes.
- **Raycasting Utilities**. New API for 3D-to-2D coordinate conversion and ray-object intersection testing, enabling touch interaction with 3D objects.
- **Cross-Platform 3D Support**. With EGL now available across all major Linux backends (SDL, Wayland, DRM, GLFW), 3D rendering is fully supported on all platforms.

Style & Theme System
~~~~~~~~~~~~~~~~~~~~

- **LV_STATE_ALT**. New widget state for simple dark/light mode switching without managing separate theme trees.
- **Theme Management API**. New functions: ``lv_obj_remove_theme``, ``lv_obj_bind_style_prop``, and full theme create/copy/delete API for multi-theme applications and data-driven style binding.

Widgets & Property Interface
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

- **Property Interface Rollout**. Arc, bar, switch, checkbox, LED, line, scale, spinbox, spinner, table, tabview, buttonmatrix, span, menu, and chart now expose uniform property interface for data binding.
- **Input Handling**. Key remapping at indev level, configurable gesture thresholds, ``LV_OBJ_FLAG_RADIO_BUTTON`` for radio groups, and keypad events emitted without assigned group.

Media Support
~~~~~~~~~~~~~

- **GStreamer Audio-Free Sources**. Supports video sources without audio tracks, enabling camera stream embedding in LVGL applications.
- **WebP Decoder**. Native WebP image decoding support added alongside existing JPEG, PNG, and GIF support.

Breaking Changes
~~~~~~~~~~~~~~~~

- **Wayland Client-Side Decorations Removed**. Rewritten driver no longer supports client-side window decorations. Use LVGL widgets (e.g., ``lv_win``) for window chrome.
- **XML Engine Removed**. XML UI engine development continues outside the main repository. Separate announcement forthcoming.

Deprecations
~~~~~~~~~~~~

- **lv_fragment deprecated**. Will be removed in a future release. Plan migration for view lifecycle management.
- **lv_wayland_display_close_cb_t deprecated**. Use ``LV_EVENT_DELETE`` on display returned by ``lv_wayland_window_create`` instead.

New Features
~~~~~~~~~~~~

- **feat(opengl): throw error if both draw nanovg and opengles draw units are enabled** `9719 <https://github.com/lvgl/lvgl/pull/9719>`__
- **feat(gltf): upgrade fastgltf with latest fixes** `9727 <https://github.com/lvgl/lvgl/pull/9727>`__
- **feat(obj): add back support for user flags** `9711 <https://github.com/lvgl/lvgl/pull/9711>`__
- **feat(style): add lv_obj_remove_theme** `9692 <https://github.com/lvgl/lvgl/pull/9692>`__
- **feat(text): skip leading spaces after line wrap** `9639 <https://github.com/lvgl/lvgl/pull/9639>`__
- **feat(draw/opengl): support dynamic video textures in OpenGLES** `9704 <https://github.com/lvgl/lvgl/pull/9704>`__
- **feat(drm/egl): auto infer drm egl config from use_opengles** `9671 <https://github.com/lvgl/lvgl/pull/9671>`__
- **feat(gstreamer): send event on end of stream** `9634 <https://github.com/lvgl/lvgl/pull/9634>`__
- **feat(display): send screen load events to display** `9600 <https://github.com/lvgl/lvgl/pull/9600>`__
- **feat(chart): add curved type** `9564 <https://github.com/lvgl/lvgl/pull/9564>`__
- **feat(indev): emit events for keypad without assigned group** `9656 <https://github.com/lvgl/lvgl/pull/9656>`__
- **feat(style): add LV_STATE_ALT for simple dark/light switching** `9691 <https://github.com/lvgl/lvgl/pull/9691>`__
- **feat(svg): simplify node attributes inheritance introduce in #9424** `9690 <https://github.com/lvgl/lvgl/pull/9690>`__
- **feat(NemaGFX): Add nema libs for cortex M7 and M55** `9374 <https://github.com/lvgl/lvgl/pull/9374>`__
- **feat(ibl_sampler): fallback to default env image and remove unnecessary allocation** `9675 <https://github.com/lvgl/lvgl/pull/9675>`__
- **feat(msgbox): add support for formatted text** `9583 <https://github.com/lvgl/lvgl/pull/9583>`__
- **feat(obj): allow null class names if obj_name isn't set** `9653 <https://github.com/lvgl/lvgl/pull/9653>`__
- **feat(opengl): add matrix transformations to opengles rendering** `9654 <https://github.com/lvgl/lvgl/pull/9654>`__
- **feat(ppa): make the PPA burst length configurable** `9612 <https://github.com/lvgl/lvgl/pull/9612>`__
- **feat(nanovg): add draw 3D support** `9571 <https://github.com/lvgl/lvgl/pull/9571>`__
- **feat(xml): remove the XML parser and loader** `9565 <https://github.com/lvgl/lvgl/pull/9565>`__
- **feat(indev): add api to set gesture thresholds** `9641 <https://github.com/lvgl/lvgl/pull/9641>`__
- **feat(sdl): add EGL support** `9462 <https://github.com/lvgl/lvgl/pull/9462>`__
- **feat(calendar): add 2026 to header** `9630 <https://github.com/lvgl/lvgl/pull/9630>`__
- **feat(draw): make lv_draw_mask_rect_dsc_t public** `9454 <https://github.com/lvgl/lvgl/pull/9454>`__
- **feat(line): allow defining an array of point in the draw task** `9269 <https://github.com/lvgl/lvgl/pull/9269>`__
- **feat(svg): support inheriting node attributes** `9424 <https://github.com/lvgl/lvgl/pull/9424>`__
- **feat(wayland): add display rotation support for the shm backend** `9386 <https://github.com/lvgl/lvgl/pull/9386>`__
- **feat(svg): support inline styles** `9423 <https://github.com/lvgl/lvgl/pull/9423>`__
- **feat(draw): add RISCV V extension for software draw** `9504 <https://github.com/lvgl/lvgl/pull/9504>`__
- **feat(canvas): add api to skip canvas invalidation when setting pixel** `9592 <https://github.com/lvgl/lvgl/pull/9592>`__
- **feat(wayland): add egl support** `9398 <https://github.com/lvgl/lvgl/pull/9398>`__
- **feat(gstreamer): add support for sources with no audio** `9551 <https://github.com/lvgl/lvgl/pull/9551>`__
- **feat(drm/egl): add nanovg support** `9542 <https://github.com/lvgl/lvgl/pull/9542>`__
- **feat(arclabel): support lv_arclabel_get_text_angle to get real rendered text size in degree** `9546 <https://github.com/lvgl/lvgl/pull/9546>`__
- **feat(opengl): auto initialize nanovg draw unit** `9543 <https://github.com/lvgl/lvgl/pull/9543>`__
- **feat(vg_lite): add support for special format** `9464 <https://github.com/lvgl/lvgl/pull/9464>`__
- **feat(dropshadow): add drop shadow support** `9331 <https://github.com/lvgl/lvgl/pull/9331>`__
- **feat(draw): add nanovg rendering backend** `8865 <https://github.com/lvgl/lvgl/pull/8865>`__
- **feat(vg_lite_tvg): add VG_LITE_BLEND_DST_IN emulation support** `9528 <https://github.com/lvgl/lvgl/pull/9528>`__
- **feat(display): add rotate point method** `9513 <https://github.com/lvgl/lvgl/pull/9513>`__
- **feat(arclabel): support overflow mode** `9484 <https://github.com/lvgl/lvgl/pull/9484>`__
- **feat(demos): allow specifying parent for widgets and music demos** `9444 <https://github.com/lvgl/lvgl/pull/9444>`__
- **feat(fragment): deprecate lv_fragment** `9460 <https://github.com/lvgl/lvgl/pull/9460>`__
- **feat(xml): add slot support** `9193 <https://github.com/lvgl/lvgl/pull/9193>`__
- **feat(jpeg): add support for orientation and CMYK** `9296 <https://github.com/lvgl/lvgl/pull/9296>`__
- **feat(opengl): add glsl version 330 support** `9384 <https://github.com/lvgl/lvgl/pull/9384>`__
- **feat(group): add user_data getter and setter** `9466 <https://github.com/lvgl/lvgl/pull/9466>`__
- **feat(core): add external data and destructor feature** `9112 <https://github.com/lvgl/lvgl/pull/9112>`__
- **feat(glfw): remove glew dependency and use glad instead** `9319 <https://github.com/lvgl/lvgl/pull/9319>`__
- **feat(obj): add LV_OBJ_FLAG_RADIO_BUTTON to easily create radio buttons** `9328 <https://github.com/lvgl/lvgl/pull/9328>`__
- **feat(obj): add scroll x and scroll y properties** `9346 <https://github.com/lvgl/lvgl/pull/9346>`__
- **feat(slider): add min_value and max_value setter** `9433 <https://github.com/lvgl/lvgl/pull/9433>`__
- **feat(scale): update needles when scale is transformed** `9340 <https://github.com/lvgl/lvgl/pull/9340>`__
- **feat(opengl): use only rgba format textures** `9304 <https://github.com/lvgl/lvgl/pull/9304>`__
- **feat(nuttx): add mock to the NuttX driver for compilation testing** `9419 <https://github.com/lvgl/lvgl/pull/9419>`__
- **feat(xml): add imagebutton support to XML** `9381 <https://github.com/lvgl/lvgl/pull/9381>`__
- **feat(gltf): make linear_output optional to enable tonemapping and gamma adjustment** `9260 <https://github.com/lvgl/lvgl/pull/9260>`__
- **feat(test): add lv_xml_test_run_to and lv_test_screenshot_result_t** `9189 <https://github.com/lvgl/lvgl/pull/9189>`__
- **feat(file_explorer): allow hiding the back button** `9202 <https://github.com/lvgl/lvgl/pull/9202>`__
- **feat(obj): allow setting child size to LV_PCT() of LV_SIZE_CONTENT parent by using the size of fixed/clamped children to set parent size** `9243 <https://github.com/lvgl/lvgl/pull/9243>`__
- **feat(drm): add return type to set_file** `9365 <https://github.com/lvgl/lvgl/pull/9365>`__
- **feat(fbdev): add return type to set_file** `9364 <https://github.com/lvgl/lvgl/pull/9364>`__
- **feat(array): add remove_unordered function** `9339 <https://github.com/lvgl/lvgl/pull/9339>`__
- **feat(gltf): allow modifying model's nodes at runtime** `9142 <https://github.com/lvgl/lvgl/pull/9142>`__
- **feat(wayland): rewrite driver** `9195 <https://github.com/lvgl/lvgl/pull/9195>`__
- **feat(blur): add blur support** `9223 <https://github.com/lvgl/lvgl/pull/9223>`__
- **feat(examples/ffmpeg_player): specify default decoder** `9218 <https://github.com/lvgl/lvgl/pull/9218>`__
- **feat(xml): support selector with local styles** `9184 <https://github.com/lvgl/lvgl/pull/9184>`__
- **feat(fs): support multiple littlefs filesystems** `8868 <https://github.com/lvgl/lvgl/pull/8868>`__
- **feat(obj): support `LV_SIZE_CONTENT` and `LV_PCT` min width/height in combination with flex grow** `8685 <https://github.com/lvgl/lvgl/pull/8685>`__
- **feat(scripts/check_gcov_coverage): add analysis of specified file paths** `9264 <https://github.com/lvgl/lvgl/pull/9264>`__
- **feat(gltf): support sharing the background environment across multiple objects** `9009 <https://github.com/lvgl/lvgl/pull/9009>`__
- **feat(theme): add api to create, copy and delete themes** `9167 <https://github.com/lvgl/lvgl/pull/9167>`__
- **feat(example/style): convert old transform demo to an example** `9050 <https://github.com/lvgl/lvgl/pull/9050>`__
- **feat(xml): add color percentage support to animations** `9209 <https://github.com/lvgl/lvgl/pull/9209>`__
- **feat(benchmark): show warnings for typical mistakes** `9171 <https://github.com/lvgl/lvgl/pull/9171>`__
- **feat(style): add lv_obj_bind_style_prop** `9173 <https://github.com/lvgl/lvgl/pull/9173>`__
- **feat(scripts): add automatic gcovr code coverage analysis** `9227 <https://github.com/lvgl/lvgl/pull/9227>`__
- **feat(timer): resume timer handler after attribute changes** `9205 <https://github.com/lvgl/lvgl/pull/9205>`__
- **feat(image_decoder): add webp decoder** `9175 <https://github.com/lvgl/lvgl/pull/9175>`__
- **feat(translate): support translation tags in tabview and list** `9079 <https://github.com/lvgl/lvgl/pull/9079>`__
- **feat(libpng): optimize the details of performance measurement** `9179 <https://github.com/lvgl/lvgl/pull/9179>`__
- **feat(image_decoder): add performance measurement** `9178 <https://github.com/lvgl/lvgl/pull/9178>`__
- **feat(vg_lite): add dynamic input parameter printing control** `9180 <https://github.com/lvgl/lvgl/pull/9180>`__
- **feat(indev): implement key remapping (#8455)** `8981 <https://github.com/lvgl/lvgl/pull/8981>`__
- **feat(keyboard): add definitions for control mode buttons** `9101 <https://github.com/lvgl/lvgl/pull/9101>`__
- **feat(gltf): gltf view raycasting and 3d point to screen point conversion** `9143 <https://github.com/lvgl/lvgl/pull/9143>`__
- **feat(gltf): use alternate blending mode for glTF views** `9115 <https://github.com/lvgl/lvgl/pull/9115>`__
- **feat(ffmpeg_player): add set_decoder** `9122 <https://github.com/lvgl/lvgl/pull/9122>`__
- **feat(xml): add lv_spinner** `9091 <https://github.com/lvgl/lvgl/pull/9091>`__
- **feat(translation): const-correct arrays of strings** `9147 <https://github.com/lvgl/lvgl/pull/9147>`__
- **feat(fs): add read file with alloc API** `9109 <https://github.com/lvgl/lvgl/pull/9109>`__
- **feat(gif): use gif draw raw mode to support stride mode** `9121 <https://github.com/lvgl/lvgl/pull/9121>`__
- **feat(class): ensure classes have names** `9097 <https://github.com/lvgl/lvgl/pull/9097>`__
- **feat(example/scroll): convert old scroll demo to an example** `9049 <https://github.com/lvgl/lvgl/pull/9049>`__
- **feat(spinbox): add property interface support** `0290020 <https://github.com/lvgl/lvgl/commit/029002092ebf6b426a00e2f768de393eee422697>`__
- **feat(bar): add property interface support** `0bd6422 <https://github.com/lvgl/lvgl/commit/0bd6422a5e863d54b0cdb953bbe95179d63e5744>`__
- **feat(arc): add property API support** `09b3143 <https://github.com/lvgl/lvgl/commit/09b3143eb80850e140ead4d88a863abd42f8a648>`__
- **feat(chart): add property interface support** `7801fd2 <https://github.com/lvgl/lvgl/commit/7801fd22e862e82a972f11418897ce6a725724ff>`__
- **feat(scale): add property interface support** `54487a9 <https://github.com/lvgl/lvgl/commit/54487a9d50388c4abead176efd36ba6785819730>`__
- **feat(span): add property interface support** `8a2b610 <https://github.com/lvgl/lvgl/commit/8a2b610f099c21880d09daa32915b6c2f2dafaa1>`__
- **feat(spinner): add property interface support** `bb76fd2 <https://github.com/lvgl/lvgl/commit/bb76fd2129526e3d8863f842adbce9bb4205256c>`__
- **feat(tabview): add property interface support** `492770e <https://github.com/lvgl/lvgl/commit/492770e77fc080ecfea87f9d80f86c7762ce14cd>`__
- **feat(dropdown): static and non-static setter for text** `af5c3f1 <https://github.com/lvgl/lvgl/commit/af5c3f12f6e9d8db2f7105b461d060615ca33f6d>`__
- **feat(table): add property interface support** `f66cc41 <https://github.com/lvgl/lvgl/commit/f66cc4187c02051b39f54e755cbbd21536f26467>`__
- **feat(menu): add property interface support** `8bf6952 <https://github.com/lvgl/lvgl/commit/8bf69520b1043b6253948554864328cb94175d6d>`__
- **feat(buttonmatrix): add property interface support** `847cc0c <https://github.com/lvgl/lvgl/commit/847cc0c8dea8abb9952e9248a50a6ed9833c4736>`__
- **feat(led): add property interface support** `43f35a2 <https://github.com/lvgl/lvgl/commit/43f35a2f3ccc5fb64f873e9f223e41bd77c76660>`__
- **feat(line): add property interface support** `5ac2ca4 <https://github.com/lvgl/lvgl/commit/5ac2ca44fff8b82d442ed2bdbcdc4a5a87855f39>`__
- **feat(switch): add property interface support** `0fb0f37 <https://github.com/lvgl/lvgl/commit/0fb0f37fa44b8621b98bba69010ecace88142811>`__
- **feat(checkbox): add property interface support** `7f924a3 <https://github.com/lvgl/lvgl/commit/7f924a3a7240c8220e06112282cb20b819567786>`__
- **feat(textarea): static and non-static setter for accepted_chars** `e20517a <https://github.com/lvgl/lvgl/commit/e20517aa70f9c4e804f9db7ce8959cfcfb23d540>`__
- **feat(dave2d): always invalidate cache when using Renesas RZA** `9d14a04 <https://github.com/lvgl/lvgl/commit/9d14a04e8a112a859b7acdc416dd90db717cb3c3>`__
- **feat(draw_buf): add missing D-Cache flush** `d703c3a <https://github.com/lvgl/lvgl/commit/d703c3a90b3f1d69e3615777a3384a0f987f6552>`__
- **feat(draw_buf_convert): add missing profiler** `4016f35 <https://github.com/lvgl/lvgl/commit/4016f35235912c7582f06f3234dbb8c061275fbb>`__

Performance
~~~~~~~~~~~

- **perf(draw/opengl): use glScissor for opaque fills** `9222 <https://github.com/lvgl/lvgl/pull/9222>`__
- **perf(label): make LAYOUT_COMPLETED a display event** `9215 <https://github.com/lvgl/lvgl/pull/9215>`__
- **perf(vg_lite): add bitmap font cache for non-aligned fonts** `9343 <https://github.com/lvgl/lvgl/pull/9343>`__
- **perf(line): check that line has at least 2 points before drawing** `9185 <https://github.com/lvgl/lvgl/pull/9185>`__
- **perf(gltf): optimize gltf material negotiation** `9038 <https://github.com/lvgl/lvgl/pull/9038>`__
- **perf(opengl): remove long delay when destroying shader programs** `9037 <https://github.com/lvgl/lvgl/pull/9037>`__
- **perf(label): reduce lv_label_refr_text calls** `9041 <https://github.com/lvgl/lvgl/pull/9041>`__

Fixes
~~~~~

- **fix(blur): render non-backdrop blur when the main rendering is done** `9752 <https://github.com/lvgl/lvgl/pull/9752>`__
- **fix(refr): flag when all tasks are added to screen layers** `9739 <https://github.com/lvgl/lvgl/pull/9739>`__
- **fix(cmake): declare `fatfs` as private dependencies** `9742 <https://github.com/lvgl/lvgl/pull/9742>`__
- **fix(wayland/g2d): fix busy buffer warnings** `9730 <https://github.com/lvgl/lvgl/pull/9730>`__
- **fix(draw/g2d): disable image rendering** `9729 <https://github.com/lvgl/lvgl/pull/9729>`__
- **fix(NemaGFX): Add back support for indexed images** `9714 <https://github.com/lvgl/lvgl/pull/9714>`__
- **fix(core): add missing includes for external data feature** `9715 <https://github.com/lvgl/lvgl/pull/9715>`__
- **fix(freetype): check if ctx is null before deleting font** `9728 <https://github.com/lvgl/lvgl/pull/9728>`__
- **fix(opengl): correctly apply matrix transforms** `9726 <https://github.com/lvgl/lvgl/pull/9726>`__
- **fix(wayland): read and flush compositor events on a timer** `9720 <https://github.com/lvgl/lvgl/pull/9720>`__
- **fix(wayland/shm): damage surface before returning if not last flush** `9721 <https://github.com/lvgl/lvgl/pull/9721>`__
- **fix(thorvg): ensure height is always positive** `9712 <https://github.com/lvgl/lvgl/pull/9712>`__
- **fix(indev): only reset last point and it's movement only if no previous object existed** `9707 <https://github.com/lvgl/lvgl/pull/9707>`__
- **fix(uefi): add missing trailing newline to driver headers** `9710 <https://github.com/lvgl/lvgl/pull/9710>`__
- **fix(gles): transpose lv_matrix_t before uploading to shader** `9703 <https://github.com/lvgl/lvgl/pull/9703>`__
- **fix(flex): use min size when item is set to grow and parent is LV_SIZE_CONTENT** `8806 <https://github.com/lvgl/lvgl/pull/8806>`__
- **fix(gltf): flip vertically by default** `9694 <https://github.com/lvgl/lvgl/pull/9694>`__
- **fix(draw/opengles): red/blue channel swap in fill operations** `9693 <https://github.com/lvgl/lvgl/pull/9693>`__
- **fix(LTDC): warn when rotating and DRAW_SW is disabled** `9670 <https://github.com/lvgl/lvgl/pull/9670>`__
- **fix(arc): handle indicator padding for knob position and arc invalidation area** `9302 <https://github.com/lvgl/lvgl/pull/9302>`__
- **fix(dave2d): fix several rendering issues (mostly restore the original alpha)** `9566 <https://github.com/lvgl/lvgl/pull/9566>`__
- **fix(draw/helium): include lv_conf_internal to avoid C declarations** `9658 <https://github.com/lvgl/lvgl/pull/9658>`__
- **fix(fbdev): use display offset and clip area to display area** `9668 <https://github.com/lvgl/lvgl/pull/9668>`__
- **fix(draw/line): fix buffer overflow when point is outside display area** `9669 <https://github.com/lvgl/lvgl/pull/9669>`__
- **fix(gltf): avoid resetting value_changed node attribute** `9674 <https://github.com/lvgl/lvgl/pull/9674>`__
- **fix(gltf): remove designated initializers causing warnings** `9676 <https://github.com/lvgl/lvgl/pull/9676>`__
- **fix(gltf/data): add missing extern "C" to header file** `9679 <https://github.com/lvgl/lvgl/pull/9679>`__
- **fix(ibl_sampler): restore gl state and add missing ressources deletion** `9678 <https://github.com/lvgl/lvgl/pull/9678>`__
- **fix(stdlib): wrong variable name when mem_add_junk is enabled** `9677 <https://github.com/lvgl/lvgl/pull/9677>`__
- **fix(fbdev): add missing errno.h include** `9673 <https://github.com/lvgl/lvgl/pull/9673>`__
- **fix(image): add transform support for AL88 color format images** `9627 <https://github.com/lvgl/lvgl/pull/9627>`__
- **fix(svg): set stride in decoder info to prevent warning** `9657 <https://github.com/lvgl/lvgl/pull/9657>`__
- **fix(g2d): fix hardware version check with LV_USE_PXP** `9644 <https://github.com/lvgl/lvgl/pull/9644>`__
- **fix(run_tests.sh): fix gcov versions mismatch issue** `9652 <https://github.com/lvgl/lvgl/pull/9652>`__
- **fix(image): incorrect L8 image alpha channel to ARGB8888 blend** `9645 <https://github.com/lvgl/lvgl/pull/9645>`__
- **fix(draw/arc): add missing lv_draw.h include** `9640 <https://github.com/lvgl/lvgl/pull/9640>`__
- **fix(gstreamer): empty frame queue on destruction** `9633 <https://github.com/lvgl/lvgl/pull/9633>`__
- **fix(lovyan_gfx): resolve double rotation in touch coordinates** `9628 <https://github.com/lvgl/lvgl/pull/9628>`__
- **fix(property): remove LV_PROPERTY_TEXTAREA_INSERT_REPLACE (#9606)** `9625 <https://github.com/lvgl/lvgl/pull/9625>`__
- **fix(property): fix reading style property returns uninitialized if non-local** `9234 <https://github.com/lvgl/lvgl/pull/9234>`__
- **fix(blur): fix regression causing VS compiler error** `9620 <https://github.com/lvgl/lvgl/pull/9620>`__
- **fix(obj_pos): alignment is not reversed when switching from LTR to RTL** `9616 <https://github.com/lvgl/lvgl/pull/9616>`__
- **fix(dropdown): symbol property behaves like IMGSRC instead of TEXT** `9608 <https://github.com/lvgl/lvgl/pull/9608>`__
- **fix(drm): set stride for draw buffers** `9609 <https://github.com/lvgl/lvgl/pull/9609>`__
- **fix(textarea): scroll to position when style changes** `9407 <https://github.com/lvgl/lvgl/pull/9407>`__
- **fix(grid): negative width of the object with column span in the grid using RTL** `9596 <https://github.com/lvgl/lvgl/pull/9596>`__
- **fix(obj_pos): incorrect x coordinate in rtl mode** `9598 <https://github.com/lvgl/lvgl/pull/9598>`__
- **fix(draw/sw): add missing LV_PROFILER_DRAW_END** `9601 <https://github.com/lvgl/lvgl/pull/9601>`__
- **fix(wayland/egl): fix config selector for nanovg** `9603 <https://github.com/lvgl/lvgl/pull/9603>`__
- **fix(opengl): fallback to RGB for desktop GL** `9588 <https://github.com/lvgl/lvgl/pull/9588>`__
- **fix(gltf): add missing LV_RESULT_INVALID check** `9580 <https://github.com/lvgl/lvgl/pull/9580>`__
- **fix(gltf): fix memory leaks caused by incomplete resource cleanup** `9579 <https://github.com/lvgl/lvgl/pull/9579>`__
- **fix(opengles_shader): fix index out of bounds** `9573 <https://github.com/lvgl/lvgl/pull/9573>`__
- **fix(gltf): fix unused-function warning** `9576 <https://github.com/lvgl/lvgl/pull/9576>`__
- **fix(gltf): fix heap use after free caused by timer not being deleted** `9574 <https://github.com/lvgl/lvgl/pull/9574>`__
- **fix(widget/span): undefined lv_subject_t if LV_USE_OBSERVER undefined** `9577 <https://github.com/lvgl/lvgl/pull/9577>`__
- **fix(glfw): add gl_rgb565 fallback definition** `9575 <https://github.com/lvgl/lvgl/pull/9575>`__
- **fix(egl): add missing ctx null check** `9572 <https://github.com/lvgl/lvgl/pull/9572>`__
- **fix: remove unused code and fix code signature warning** `9570 <https://github.com/lvgl/lvgl/pull/9570>`__
- **fix(lodepng): fix memory leak when cache addition fails** `9569 <https://github.com/lvgl/lvgl/pull/9569>`__
- **fix(draw_sw): remove unused code in  `lv_draw_sw_blend_to_a8.c`** `9567 <https://github.com/lvgl/lvgl/pull/9567>`__
- **fix: format specifier warning in font manager** `9549 <https://github.com/lvgl/lvgl/pull/9549>`__
- **fix(test): add missing malloc check to screenshot compare** `9541 <https://github.com/lvgl/lvgl/pull/9541>`__
- **fix(xml): pass raw # if there's no constant name** `9418 <https://github.com/lvgl/lvgl/pull/9418>`__
- **fix(drm/egl): red and blue channel swap with RGB565** `9461 <https://github.com/lvgl/lvgl/pull/9461>`__
- **fix(arclabel): fix arclabel opa set** `9483 <https://github.com/lvgl/lvgl/pull/9483>`__
- **fix(refr): restore layer opacity and recolor properties when layer area retrieval fails** `9521 <https://github.com/lvgl/lvgl/pull/9521>`__
- **fix(vg_lite): fix the missing handling of rotated vector fonts g-&gt;ofs_x** `9519 <https://github.com/lvgl/lvgl/pull/9519>`__
- **fix(vg_lite): workaround vg_lite_blit_rect offset x/y hardware bug** `9522 <https://github.com/lvgl/lvgl/pull/9522>`__
- **fix(draw_sw_line): fix horizontal line incorrect dash length** `9501 <https://github.com/lvgl/lvgl/pull/9501>`__
- **fix(draw): support RGB565_SWAPPED in software blur** `9512 <https://github.com/lvgl/lvgl/pull/9512>`__
- **fix(wayland): fix touch support by correctly setting driver data** `9486 <https://github.com/lvgl/lvgl/pull/9486>`__
- **fix(vg_lite): fix incomplete coordinate calculation when converting scissor_area to bounding_box_area** `9511 <https://github.com/lvgl/lvgl/pull/9511>`__
- **fix(vg_lite): wait for vg_lite_tvg finish before drawing the mask** `9489 <https://github.com/lvgl/lvgl/pull/9489>`__
- **fix(anim): ignore large apparent animation duration** `9280 <https://github.com/lvgl/lvgl/pull/9280>`__
- **fix(arclabel): fix arclabel arc length calc in recolor mode** `9467 <https://github.com/lvgl/lvgl/pull/9467>`__
- **fix(image_header_cache): fix dump iter selection error** `9465 <https://github.com/lvgl/lvgl/pull/9465>`__
- **fix(arclabel): fix arc length calculating precise** `9457 <https://github.com/lvgl/lvgl/pull/9457>`__
- **fix(opengl): compilation error when using glfw** `9459 <https://github.com/lvgl/lvgl/pull/9459>`__
- **fix(image_cache): fix image cache dump memory leaks** `9448 <https://github.com/lvgl/lvgl/pull/9448>`__
- **fix(conf_internal): explicitly disable the glTF demo when build_demo is disabled** `9408 <https://github.com/lvgl/lvgl/pull/9408>`__
- **fix(gltf): remove fastgltf memory mapped file feature check** `9422 <https://github.com/lvgl/lvgl/pull/9422>`__
- **fix(gltf): correct argument type to match declaration** `9421 <https://github.com/lvgl/lvgl/pull/9421>`__
- **fix(gltf): add declaration for loading model from bytes** `9420 <https://github.com/lvgl/lvgl/pull/9420>`__
- **fix(demo): fix compiler warning of unused variable c** `9370 <https://github.com/lvgl/lvgl/pull/9370>`__
- **fix(flex): fix rounding may leave unused space on track with flex_grow** `9217 <https://github.com/lvgl/lvgl/pull/9217>`__
- **fix(gstreamer): fix buffer map memory leak** `9378 <https://github.com/lvgl/lvgl/pull/9378>`__
- **fix(draw/dma2d): correct cache handling** `9366 <https://github.com/lvgl/lvgl/pull/9366>`__
- **fix(gltf): stride mismatch issue** `9385 <https://github.com/lvgl/lvgl/pull/9385>`__
- **fix(gltf): force depth mask for transparent items used with refractive materials** `9396 <https://github.com/lvgl/lvgl/pull/9396>`__
- **fix(tabview): re-set tab bar size after changing its position** `9399 <https://github.com/lvgl/lvgl/pull/9399>`__
- **fix(demo): fix compiler warnings regarding the use of LV_UNUSED** `9403 <https://github.com/lvgl/lvgl/pull/9403>`__
- **fix(tiny_ttf): release entry for correct cache** `9429 <https://github.com/lvgl/lvgl/pull/9429>`__
- **fix(cache): entry alloc** `9427 <https://github.com/lvgl/lvgl/pull/9427>`__
- **fix(xml): allow animating bg_image_opa in XML timelines** `9425 <https://github.com/lvgl/lvgl/pull/9425>`__
- **fix(opengles): fix the memory leak when the shader manager is destroyed (#9439)** `9440 <https://github.com/lvgl/lvgl/pull/9440>`__
- **fix(rb): make compare result type a 32bit integer** `9442 <https://github.com/lvgl/lvgl/pull/9442>`__
- **fix(draw_buf)：change the align log level to INFO** `9428 <https://github.com/lvgl/lvgl/pull/9428>`__
- **fix(gltf): correctly handle matrices for animation and cameras** `9273 <https://github.com/lvgl/lvgl/pull/9273>`__
- **fix(nuttx): fix profiler build break** `9417 <https://github.com/lvgl/lvgl/pull/9417>`__
- **fix(blur):  fix VS compiler error** `9391 <https://github.com/lvgl/lvgl/pull/9391>`__
- **fix(freetype): set pixel size before kerning calculation (#9367)** `9388 <https://github.com/lvgl/lvgl/pull/9388>`__
- **fix(xml): fix compile warning of uninitialized value** `9368 <https://github.com/lvgl/lvgl/pull/9368>`__
- **fix(gif): compatibility issue when logging uint32_t variables** `9349 <https://github.com/lvgl/lvgl/pull/9349>`__
- **fix(display): always have a solid background with non-alpha color formats** `9380 <https://github.com/lvgl/lvgl/pull/9380>`__
- **fix(demo): make the Log out and Invite button not overlap on 480 hor. res.** `9383 <https://github.com/lvgl/lvgl/pull/9383>`__
- **fix(roller): transformed roller click location** `9212 <https://github.com/lvgl/lvgl/pull/9212>`__
- **fix(screen): double free when loading screens** `9276 <https://github.com/lvgl/lvgl/pull/9276>`__
- **fix(ime): restrict to lowercase input to avoid overflow** `9320 <https://github.com/lvgl/lvgl/pull/9320>`__
- **fix(spinbox): fix stack corruption** `9256 <https://github.com/lvgl/lvgl/pull/9256>`__
- **fix(binfont_loader): add include to include missing function from API page** `9344 <https://github.com/lvgl/lvgl/pull/9344>`__
- **fix(ESP): fix esp toolchain format specifier edge case** `9336 <https://github.com/lvgl/lvgl/pull/9336>`__
- **fix(EVE): update lv_font_fmt_txt.h include path** `9337 <https://github.com/lvgl/lvgl/pull/9337>`__
- **fix(gif): fix negative loop count handling** `9241 <https://github.com/lvgl/lvgl/pull/9241>`__
- **fix(sdl): do not dereferencing nullptr in lv_sdl_mouse_handler if an invalid windows id (0) has been received** `9299 <https://github.com/lvgl/lvgl/pull/9299>`__
- **fix(label): skip label draw if width is negative** `9297 <https://github.com/lvgl/lvgl/pull/9297>`__
- **fix(indev): send LV_EVENT_KEY as indev event** `9208 <https://github.com/lvgl/lvgl/pull/9208>`__
- **fix(glfw): fix deinitialization order to prevent segfault** `9220 <https://github.com/lvgl/lvgl/pull/9220>`__
- **fix(draw/sw): remove param from draw_sw_mask_rect** `9288 <https://github.com/lvgl/lvgl/pull/9288>`__
- **fix(property): fix return type of LV_PROPERTY_OBJ_SCROLL_END** `9203 <https://github.com/lvgl/lvgl/pull/9203>`__
- **fix(property): fix some non-integer style property types** `9245 <https://github.com/lvgl/lvgl/pull/9245>`__
- **fix(indev): ensure timer triggers when elapsed time meets threshold** `9275 <https://github.com/lvgl/lvgl/pull/9275>`__
- **fix(gif): add disposal handle to fix gif display error** `9213 <https://github.com/lvgl/lvgl/pull/9213>`__
- **fix(scripts/check_gcov_coverage): skip statistics for non-executable lines** `9239 <https://github.com/lvgl/lvgl/pull/9239>`__
- **fix(vg_lite): add a path print length limit** `9248 <https://github.com/lvgl/lvgl/pull/9248>`__
- **fix(NemaGFX): update lv_font_fmt_txt.h include path** `9235 <https://github.com/lvgl/lvgl/pull/9235>`__
- **fix(draw/ppa): fix build and rendering issues** `9162 <https://github.com/lvgl/lvgl/pull/9162>`__
- **fix(widgets): make animations on state change more consistent** `9174 <https://github.com/lvgl/lvgl/pull/9174>`__
- **fix(workflows): fix commit ID error in push events mode** `9231 <https://github.com/lvgl/lvgl/pull/9231>`__
- **fix(gif): add missing D-cache flush** `9228 <https://github.com/lvgl/lvgl/pull/9228>`__
- **fix(label): lv_label_mark_need_refr_text not invalidate area** `9176 <https://github.com/lvgl/lvgl/pull/9176>`__
- **fix(ffmpeg_player): increase align to 32** `9166 <https://github.com/lvgl/lvgl/pull/9166>`__
- **fix(driver/st_ltdc): Add missing lv_os_private.h include** `9191 <https://github.com/lvgl/lvgl/pull/9191>`__
- **fix(LTDC): add cache cleaning** `9192 <https://github.com/lvgl/lvgl/pull/9192>`__
- **fix(fbdev): Implement proper cleanup from deletion event in FBDEV** `9169 <https://github.com/lvgl/lvgl/pull/9169>`__
- **fix(example): correct lv_example_chart_5 display** `9177 <https://github.com/lvgl/lvgl/pull/9177>`__
- **fix(canvas): correct RGB565 pixel offset in lv_canvas_get_px** `9188 <https://github.com/lvgl/lvgl/pull/9188>`__
- **fix(ffmpeg): for nv12 pix_fmt** `9131 <https://github.com/lvgl/lvgl/pull/9131>`__
- **fix(opengles): update fb address after reshaping texture** `9134 <https://github.com/lvgl/lvgl/pull/9134>`__
- **fix(obj_tree): fixes exact object name match** `9150 <https://github.com/lvgl/lvgl/pull/9150>`__
- **fix(gltf): fix indexation of model binds and and a new uniform scale bind** `9102 <https://github.com/lvgl/lvgl/pull/9102>`__
- **fix(fs): set data to NULL if read file failed** `9156 <https://github.com/lvgl/lvgl/pull/9156>`__
- **fix(keyboard): remove and add state to text area instead of keyboard** `9105 <https://github.com/lvgl/lvgl/pull/9105>`__
- **fix(indev): Abort sending Keypad INDEV_ACT events following invalid return** `9135 <https://github.com/lvgl/lvgl/pull/9135>`__
- **fix(xml): correclty use integer format specifier** `9133 <https://github.com/lvgl/lvgl/pull/9133>`__
- **fix(test): fix memory leak on failed screenshot compare** `9088 <https://github.com/lvgl/lvgl/pull/9088>`__
- **fix(scale): incorrect decrement in lv_scale_set_line_needle_value loop** `9081 <https://github.com/lvgl/lvgl/pull/9081>`__
- **fix(vg_lite): fix warning if LV_LOG option is closed** `9094 <https://github.com/lvgl/lvgl/pull/9094>`__
- **fix(flex): fix crash with flex_grow on screen** `9090 <https://github.com/lvgl/lvgl/pull/9090>`__
- **fix(drm): fix drm driver memory leaks** `31e4138 <https://github.com/lvgl/lvgl/commit/31e41380d19ae70392ffb59eb8041df5f3937978>`__
- **fix(gltf): convert baseColorFactor to sRGB for more accurate mid-range colors** `bdb7099 <https://github.com/lvgl/lvgl/commit/bdb7099c3a65029a4399a566340b79e65b069986>`__
- **fix(workflows): skip the coverage analysis if it's not a pull request** `b9cdc4a <https://github.com/lvgl/lvgl/commit/b9cdc4a6100a85c01059e098da57d714362b3f93>`__
- **fix(dave2d): fix leftover typo from #8725** `abec5b7 <https://github.com/lvgl/lvgl/commit/abec5b7c6ed6e1c24ffda6a0f297a874aea89dcf>`__

Examples
~~~~~~~~

Docs
~~~~

- **docs(draw_descriptors): fix examples missing code** `9745 <https://github.com/lvgl/lvgl/pull/9745>`__
- **docs(draw/sdl): add missing draw_sw requirement** `9747 <https://github.com/lvgl/lvgl/pull/9747>`__
- **docs(renesas): update link in docs to Renesas plugin v1.0.1** `9735 <https://github.com/lvgl/lvgl/pull/9735>`__
- **docs(overview): remove conflict marker** `9733 <https://github.com/lvgl/lvgl/pull/9733>`__
- **docs(wayland): remove server side window decorations promise** `9708 <https://github.com/lvgl/lvgl/pull/9708>`__
- **docs(README):  document how to work with :c:macro: role limitation** `9700 <https://github.com/lvgl/lvgl/pull/9700>`__
- **docs(integration): make the integration more effective and informative** `9507 <https://github.com/lvgl/lvgl/pull/9507>`__
- **docs(win):  fix link list in API section** `9697 <https://github.com/lvgl/lvgl/pull/9697>`__
- **docs(style_api_gen): add missing * to style API documentation** `9684 <https://github.com/lvgl/lvgl/pull/9684>`__
- **docs(animation): clarify constraint in animations** `9686 <https://github.com/lvgl/lvgl/pull/9686>`__
- **docs(style API):  add API documentation to `style_api_gen.py`** `9637 <https://github.com/lvgl/lvgl/pull/9637>`__
- **docs(macros): fix many macro links to API docs** `9533 <https://github.com/lvgl/lvgl/pull/9533>`__
- **docs(buttonmatrix): fix event name typo** `9635 <https://github.com/lvgl/lvgl/pull/9635>`__
- **docs(rt-thread):  update BSP list and place the table and links in a form that is easier to maintain** `9611 <https://github.com/lvgl/lvgl/pull/9611>`__
- **docs: add missing xml redirects** `9593 <https://github.com/lvgl/lvgl/pull/9593>`__
- **docs(canvas): clarify buffer allocation details** `9563 <https://github.com/lvgl/lvgl/pull/9563>`__
- **docs(links):  fixed broken external links** `9525 <https://github.com/lvgl/lvgl/pull/9525>`__
- **docs(display): mention palette in example flush_cb** `9534 <https://github.com/lvgl/lvgl/pull/9534>`__
- **docs(API): link api pages to source code header files** `9456 <https://github.com/lvgl/lvgl/pull/9456>`__
- **docs(clipping): document how clipping works in drawing** `9446 <https://github.com/lvgl/lvgl/pull/9446>`__
- **docs(editor): Zephyr integration guide.** `9360 <https://github.com/lvgl/lvgl/pull/9360>`__
- **docs(ICOP): add docs for board manufacturer ICOP** `9413 <https://github.com/lvgl/lvgl/pull/9413>`__
- **docs(color):  fix unexpected API page links** `9338 <https://github.com/lvgl/lvgl/pull/9338>`__
- **docs(layers):  clarify how to control Widget layering** `9379 <https://github.com/lvgl/lvgl/pull/9379>`__
- **docs(display_controllers):  fix erroneous doc cross-references** `9382 <https://github.com/lvgl/lvgl/pull/9382>`__
- **docs(examples):  fix erroneous example headings** `9355 <https://github.com/lvgl/lvgl/pull/9355>`__
- **docs(3dtexture):  fix web link with syntax error** `9357 <https://github.com/lvgl/lvgl/pull/9357>`__
- **docs(base_widget.rst):  add `obj` example to docs** `9356 <https://github.com/lvgl/lvgl/pull/9356>`__
- **docs(custom.css):  fix 2 CSS color issues** `9330 <https://github.com/lvgl/lvgl/pull/9330>`__
- **docs(examples): add back show c code button in widget pages** `9354 <https://github.com/lvgl/lvgl/pull/9354>`__
- **docs(redirects):  add redirect for renesas XML integration page** `9345 <https://github.com/lvgl/lvgl/pull/9345>`__
- **docs(examples):  make `example_list.py` robust** `9317 <https://github.com/lvgl/lvgl/pull/9317>`__
- **docs(wayland): remove mention of custom timer handler** `9329 <https://github.com/lvgl/lvgl/pull/9329>`__
- **docs(redirects):  add redirects from URLs in prev READMEs** `9322 <https://github.com/lvgl/lvgl/pull/9322>`__
- **docs(redirects):  add redirect for prev getting_started page** `9321 <https://github.com/lvgl/lvgl/pull/9321>`__
- **docs(build.py):  add changes needed with removal of details/ and intro/ dirs** `9315 <https://github.com/lvgl/lvgl/pull/9315>`__
- **docs(color_format): add rgb565 swapped format info** `9310 <https://github.com/lvgl/lvgl/pull/9310>`__
- **docs(directory structure):  simplify by removing details/ and intro/ dirs** `9313 <https://github.com/lvgl/lvgl/pull/9313>`__
- **docs(examples):  fix libs/, others/ and porting/ missing from examples page.** `9291 <https://github.com/lvgl/lvgl/pull/9291>`__
- **docs(images): remove outdated information and reorganize image docs** `9221 <https://github.com/lvgl/lvgl/pull/9221>`__
- **docs(config):  improve documentation in config template** `9251 <https://github.com/lvgl/lvgl/pull/9251>`__
- **docs(examples):  make example pathing more robust** `9308 <https://github.com/lvgl/lvgl/pull/9308>`__
- **docs(README):  fix typo** `9286 <https://github.com/lvgl/lvgl/pull/9286>`__
- **docs(libpng):  fix documentation typo** `9290 <https://github.com/lvgl/lvgl/pull/9290>`__
- **docs(examples/grad):  fix missing source code in gradient examples** `9289 <https://github.com/lvgl/lvgl/pull/9289>`__
- **docs(Widget Tree): provide data lost from end of doc** `9158 <https://github.com/lvgl/lvgl/pull/9158>`__
- **docs(README):  add instructions for new documentation features** `9149 <https://github.com/lvgl/lvgl/pull/9149>`__
- **docs(editor): Arduino usage guide** `9104 <https://github.com/lvgl/lvgl/pull/9104>`__
- **docs(torizon): update torizon os docs to work with latest LVGL** `9095 <https://github.com/lvgl/lvgl/pull/9095>`__
- **docs(Guides):  add structure to TOC for Guides section** `9281 <https://github.com/lvgl/lvgl/pull/9281>`__
- **docs(tinyttf): remove file loading in the examples** `9266 <https://github.com/lvgl/lvgl/pull/9266>`__
- **docs(editor): Add brief section on installing e2 Studio plugin to the docs** `9257 <https://github.com/lvgl/lvgl/pull/9257>`__
- **docs(Renesas): Add Renesas RZ/G3E and RA8D2 to docs** `9253 <https://github.com/lvgl/lvgl/pull/9253>`__
- **docs(threading):  simplify and add work-queue concept to Threading doc** `9106 <https://github.com/lvgl/lvgl/pull/9106>`__
- **docs(xml): add Renesas Eclipse / E2Studio docs** `9216 <https://github.com/lvgl/lvgl/pull/9216>`__
- **docs(debugging):  proofread debugging section** `9163 <https://github.com/lvgl/lvgl/pull/9163>`__
- **docs(style):  add table width enhancements and bold + italic + underline support** `9124 <https://github.com/lvgl/lvgl/pull/9124>`__
- **docs:  factor out repeated substitution definitions** `9125 <https://github.com/lvgl/lvgl/pull/9125>`__
- **docs(macro api links):  fix links to macros in API pages** `9161 <https://github.com/lvgl/lvgl/pull/9161>`__
- **docs(mipi):  clarify generic MIPI with example code** `9099 <https://github.com/lvgl/lvgl/pull/9099>`__
- **docs: move ext links to include dir** `9128 <https://github.com/lvgl/lvgl/pull/9128>`__
- **docs:  fix enumerator expansion upon API page links** `9114 <https://github.com/lvgl/lvgl/pull/9114>`__
- **docs(misc):  remove last warnings from doc-build with skip_api argument** `9119 <https://github.com/lvgl/lvgl/pull/9119>`__
- **docs(misc):  fix typos, grammar and add missing function links to api pages** `9126 <https://github.com/lvgl/lvgl/pull/9126>`__
- **docs(gif):  fix outdated content about using GIFs as C arrays** `9093 <https://github.com/lvgl/lvgl/pull/9093>`__

CI and tests
~~~~~~~~~~~~

- **test(vector): add more draw vector test case** `9524 <https://github.com/lvgl/lvgl/pull/9524>`__
- **test(pinyin): update ref images** `9610 <https://github.com/lvgl/lvgl/pull/9610>`__
- **test: fix missing EOL warnings** `9548 <https://github.com/lvgl/lvgl/pull/9548>`__
- **test(svg): add svg file testcase** `9535 <https://github.com/lvgl/lvgl/pull/9535>`__
- **ci(release): add steps to prepare and upload lite package** `9443 <https://github.com/lvgl/lvgl/pull/9443>`__
- **test(canvas): add RGB565 pixel read test** `9516 <https://github.com/lvgl/lvgl/pull/9516>`__
- **test(canvas): add canvas test case and fix the null pointer access crash in the draw buffer** `9478 <https://github.com/lvgl/lvgl/pull/9478>`__
- **test(slider): add slider test case** `9477 <https://github.com/lvgl/lvgl/pull/9477>`__
- **test(async): add async test case** `9475 <https://github.com/lvgl/lvgl/pull/9475>`__
- **test(utils): add utils test case** `9474 <https://github.com/lvgl/lvgl/pull/9474>`__
- **test(palette): add palette test case** `9473 <https://github.com/lvgl/lvgl/pull/9473>`__
- **test(ll): add ll test case** `9472 <https://github.com/lvgl/lvgl/pull/9472>`__
- **test(color): add color test case** `9471 <https://github.com/lvgl/lvgl/pull/9471>`__
- **test(circle_buf): add circle buffer test case** `9470 <https://github.com/lvgl/lvgl/pull/9470>`__
- **test(area): add area test case** `9469 <https://github.com/lvgl/lvgl/pull/9469>`__
- **test(arc): add arc test case** `9476 <https://github.com/lvgl/lvgl/pull/9476>`__
- **test(fs): add fs test case and add fs mock** `9479 <https://github.com/lvgl/lvgl/pull/9479>`__
- **test(snapshot): add snapshot test case** `9481 <https://github.com/lvgl/lvgl/pull/9481>`__
- **test(image): add image test case** `9480 <https://github.com/lvgl/lvgl/pull/9480>`__
- **test(math): fix lv_pow failure to handle negative numbers exp and add full-feature coverage test** `9450 <https://github.com/lvgl/lvgl/pull/9450>`__
- **test(ime_pinyin): fix crash and memory leak and add full-feature coverage test** `9434 <https://github.com/lvgl/lvgl/pull/9434>`__
- **test(cache): add cache_entry_alloc** `9438 <https://github.com/lvgl/lvgl/pull/9438>`__
- **test(menu): fix double free and add full-feature coverage test** `9432 <https://github.com/lvgl/lvgl/pull/9432>`__
- **test(tick): add tick test case** `9416 <https://github.com/lvgl/lvgl/pull/9416>`__
- **test: update demo widgets ref image** `9358 <https://github.com/lvgl/lvgl/pull/9358>`__
- **ci: deploy doc builds to release folders** `9352 <https://github.com/lvgl/lvgl/pull/9352>`__
- **ci(deps): bump actions/checkout from 5 to 6** `9334 <https://github.com/lvgl/lvgl/pull/9334>`__
- **ci(deps): bump JamesIves/github-pages-deploy-action from 4.7.3 to 4.7.4** `9333 <https://github.com/lvgl/lvgl/pull/9333>`__
- **ci(tests): retain the gcov report for coverage analysis** `9274 <https://github.com/lvgl/lvgl/pull/9274>`__
- **test(draw_buf): add draw buf premultiply test** `9259 <https://github.com/lvgl/lvgl/pull/9259>`__
- **test(display): improve lv_display test coverage** `9110 <https://github.com/lvgl/lvgl/pull/9110>`__
- **test(demo_widgets): add screenshot comparison** `9111 <https://github.com/lvgl/lvgl/pull/9111>`__
- **test(theme): improve theme test coverage** `9116 <https://github.com/lvgl/lvgl/pull/9116>`__
- **test(mem): improve memory test coverage** `9117 <https://github.com/lvgl/lvgl/pull/9117>`__
- **test(vector): add demo vector graphic test** `9130 <https://github.com/lvgl/lvgl/pull/9130>`__
- **test(array): improve lv_array test coverage** `9108 <https://github.com/lvgl/lvgl/pull/9108>`__
- **test(indev): merge indev testcase** `9071 <https://github.com/lvgl/lvgl/pull/9071>`__

Others
~~~~~~

- **feat(env): always use the C library when building for QNX** `9738 <https://github.com/lvgl/lvgl/pull/9738>`__
- **revert(canvas): remove api to skip canvas invalidation when setting pixel** `9751 <https://github.com/lvgl/lvgl/pull/9751>`__
- **chore(cmsis-pack): Prepare for v9.5.0** `9748 <https://github.com/lvgl/lvgl/pull/9748>`__
- **build(esp): only add fatfs component if use_fs_fatfs is enabled** `9746 <https://github.com/lvgl/lvgl/pull/9746>`__
- **chore(font_gen): replace os.system with subprocess.run** `9689 <https://github.com/lvgl/lvgl/pull/9689>`__
- **build(deps): bump dawidd6/action-download-artifact from 12 to 14** `9666 <https://github.com/lvgl/lvgl/pull/9666>`__
- **build(deps): bump JamesIves/github-pages-deploy-action from 4.7.6 to 4.8.0** `9667 <https://github.com/lvgl/lvgl/pull/9667>`__
- **chore(ppa): update cache alignment error message for 128-byte lines** `9526 <https://github.com/lvgl/lvgl/pull/9526>`__
- **chore(gif):  move GIF Widget to widgets/ directory** `9617 <https://github.com/lvgl/lvgl/pull/9617>`__
- **chore(docs): lock sphinx version to 8.2.3** `9626 <https://github.com/lvgl/lvgl/pull/9626>`__
- **chore(msgbox): fix the incorrect return value docs** `9584 <https://github.com/lvgl/lvgl/pull/9584>`__
- **chore: fix warning of span testcase** `9547 <https://github.com/lvgl/lvgl/pull/9547>`__
- **refactor(opengles): set texture id to layer head on disp creation** `9397 <https://github.com/lvgl/lvgl/pull/9397>`__
- **chore(CODEOWNERS): add @FASTSHIFT to default owners** `9531 <https://github.com/lvgl/lvgl/pull/9531>`__
- **refactor(canvas): clarify parameter names for lv_canvas_copy_buf** `9414 <https://github.com/lvgl/lvgl/pull/9414>`__
- **build(cmake): check if demos and examples exist** `9510 <https://github.com/lvgl/lvgl/pull/9510>`__
- **build(esp): add missing lv_demos.c source file to esp build process** `9517 <https://github.com/lvgl/lvgl/pull/9517>`__
- **chore(tests): clean up erroneously submitted temporary files** `9515 <https://github.com/lvgl/lvgl/pull/9515>`__
- **refactor(gif): add new API and add full testcase** `9452 <https://github.com/lvgl/lvgl/pull/9452>`__
- **chore: add codeowners** `9458 <https://github.com/lvgl/lvgl/pull/9458>`__
- **build(deps): bump actions/upload-artifact from 5 to 6** `9495 <https://github.com/lvgl/lvgl/pull/9495>`__
- **build(deps): bump JamesIves/github-pages-deploy-action from 4.7.4 to 4.7.6** `9496 <https://github.com/lvgl/lvgl/pull/9496>`__
- **build(deps): bump dawidd6/action-download-artifact from 11 to 12** `9497 <https://github.com/lvgl/lvgl/pull/9497>`__
- **build(deps): bump actions/cache from 4 to 5** `9498 <https://github.com/lvgl/lvgl/pull/9498>`__
- **build(Kconfig): auto select LV_USE_GENERIC_MIPI** `9491 <https://github.com/lvgl/lvgl/pull/9491>`__
- **chore(image_converter): add AL88 color format support** `9453 <https://github.com/lvgl/lvgl/pull/9453>`__
- **build(kconfig): make log level selection more readable** `9409 <https://github.com/lvgl/lvgl/pull/9409>`__
- **build(test): ffmpeg compile error on macOS** `9371 <https://github.com/lvgl/lvgl/pull/9371>`__
- **chore: fix compile warnings** `9369 <https://github.com/lvgl/lvgl/pull/9369>`__
- **refactor(obj_property): use LV_ARRAYLEN for LV_OBJ_SET_PROPERTY_ARRAY** `9395 <https://github.com/lvgl/lvgl/pull/9395>`__
- **refactor(indev): remove the unused last_obj field from lv_indev_t** `9219 <https://github.com/lvgl/lvgl/pull/9219>`__
- **chore(lvglgdb): show classp name if lv_class name is not set** `9376 <https://github.com/lvgl/lvgl/pull/9376>`__
- **chore(vg_lite): minimize the header file inclusion** `9361 <https://github.com/lvgl/lvgl/pull/9361>`__
- **chore(lvglgdb): fix TypeError in lv_obj.py while dump specify object** `9375 <https://github.com/lvgl/lvgl/pull/9375>`__
- **chore: update version to v9.5.0-dev** `9350 <https://github.com/lvgl/lvgl/pull/9350>`__
- **chore(lv_conf_internal): replace the tab and add automatic checks** `9323 <https://github.com/lvgl/lvgl/pull/9323>`__
- **chore: update version to v9.4.0-dev** `9316 <https://github.com/lvgl/lvgl/pull/9316>`__
- **chore(tests/unity): fix the case discrepancy in the size_t comparison macro definition** `9318 <https://github.com/lvgl/lvgl/pull/9318>`__
- **chore: fix changelog generator to be compatible with reST** `9168 <https://github.com/lvgl/lvgl/pull/9168>`__
- **build(Kconfig): add missing support_rgb565_swapped config** `9282 <https://github.com/lvgl/lvgl/pull/9282>`__
- **chore(scripts/run_tests): remove redundant apt list commands** `9226 <https://github.com/lvgl/lvgl/pull/9226>`__
- **refactor(draw_buf): add inline tag for draw buffer flag functions** `9229 <https://github.com/lvgl/lvgl/pull/9229>`__
- **chore(test_conf): fix build break if webp library not installed** `9225 <https://github.com/lvgl/lvgl/pull/9225>`__
- **refactor(canvas): remove draw_sw include** `9204 <https://github.com/lvgl/lvgl/pull/9204>`__
- **refactor(arc): remove redundant code** `9198 <https://github.com/lvgl/lvgl/pull/9198>`__
- **refactor: reorganize the others folder** `9164 <https://github.com/lvgl/lvgl/pull/9164>`__
- **build(deps): bump actions/upload-artifact from 4 to 5** `9182 <https://github.com/lvgl/lvgl/pull/9182>`__
- **Make scroll animation timing configurable via defines** `9160 <https://github.com/lvgl/lvgl/pull/9160>`__
- **build(kconfig): add 3d texture and gltf configs** `9132 <https://github.com/lvgl/lvgl/pull/9132>`__
- **chore(gdb): add initial project configuration and installation instructions for lvglgdb** `9145 <https://github.com/lvgl/lvgl/pull/9145>`__
- **chore(gdb): support rb/cache associate debug obj and commands** `9089 <https://github.com/lvgl/lvgl/pull/9089>`__
- **chore(scripts): remove rz-g2l-evkit from port updater as it still uses v8.3** `9092 <https://github.com/lvgl/lvgl/pull/9092>`__
- **chore(vg_lite): correct spelling of 'high' in color key structure** `9087 <https://github.com/lvgl/lvgl/pull/9087>`__
- **chore(gdb): add gdb cmd for nuttx drivers to support multi-instance** `9068 <https://github.com/lvgl/lvgl/pull/9068>`__
- **chore(widgets): use helper for properties in class** `7193931 <https://github.com/lvgl/lvgl/commit/71939310c2642af50483fa65baca3e77ddb021e9>`__
- **chore(property): add macro to help property definitions** `55aa15d <https://github.com/lvgl/lvgl/commit/55aa15dddd3495bd4eabe2bcbfd70c4aa2dc10d8>`__
- **Revert "fix(workflows): fix commit ID error in push events mode (#9231)"** `cafeb18 <https://github.com/lvgl/lvgl/commit/cafeb18455fd3da679793654e55dbb57cff1f131>`__
