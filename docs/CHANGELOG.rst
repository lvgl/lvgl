.. _changelog:

Changelog
=========

`v9.1.0 <https://github.com/lvgl/lvgl/compare/v9.0.0...v9.1.0>`__ 20 March 2024
----------------------------------------------------------------------------------------------

New Features
~~~~~~~~~~~~

- **feat(indev) Add crown support to pointer input device** `5057 <https://github.com/lvgl/lvgl/pull/5057>`__
- **feat(test): test bin image loaded to RAM or not** `5895 <https://github.com/lvgl/lvgl/pull/5895>`__
- **feat(script): change trace_filter default trace log file to log_file.systrace** `5900 <https://github.com/lvgl/lvgl/pull/5900>`__
- **feat(vg_lite): automatically select path type based on opa** `5896 <https://github.com/lvgl/lvgl/pull/5896>`__
- **feat(freertos): support Espressif's FreeRTOS flavor** `5862 <https://github.com/lvgl/lvgl/pull/5862>`__
- **feat(tests): add freetype font stress tests** `5828 <https://github.com/lvgl/lvgl/pull/5828>`__
- **feat(vg_lite): add stroke path support** `5831 <https://github.com/lvgl/lvgl/pull/5831>`__
- **feat(docs): add Chinese translation (jump link)** `5839 <https://github.com/lvgl/lvgl/pull/5839>`__
- **feat(ci): update distro to ubuntu 22.02 to fix freetype build problem** `5834 <https://github.com/lvgl/lvgl/pull/5834>`__
- **feat(image_cache): add resize function to image cache** `5829 <https://github.com/lvgl/lvgl/pull/5829>`__
- **feat(nuttx): add cpu idle getter** `5814 <https://github.com/lvgl/lvgl/pull/5814>`__
- **feat(test): add multiple color formats to stride adjust test** `5690 <https://github.com/lvgl/lvgl/pull/5690>`__
- **feat(libjpeg_turbo): add JPEG image EXIF data parsing** `5263 <https://github.com/lvgl/lvgl/pull/5263>`__
- **feat(freetype): change the tilt of freetype fonts in italics** `5812 <https://github.com/lvgl/lvgl/pull/5812>`__
- **feat(vg_lite): add matrix assert** `5766 <https://github.com/lvgl/lvgl/pull/5766>`__
- **feat(vg_lite): add grad cache size config and auto release cache** `5731 <https://github.com/lvgl/lvgl/pull/5731>`__
- **feat(draw_buf): support indexed image in draw_buf_copy** `5686 <https://github.com/lvgl/lvgl/pull/5686>`__
- **feat(nuttx): update display driver** `5752 <https://github.com/lvgl/lvgl/pull/5752>`__
- **feat(fs): implement littlefs lfs.h driver support** `5562 <https://github.com/lvgl/lvgl/pull/5562>`__
- **feat(test_keyboard): add keyboard test** `5739 <https://github.com/lvgl/lvgl/pull/5739>`__
- **feat(x11): add LV_KEYs** `5704 <https://github.com/lvgl/lvgl/pull/5704>`__
- **feat(display): add save screenshot to file support** `5481 <https://github.com/lvgl/lvgl/pull/5481>`__
- **feat(decoder): do not even try cache if 'no_cache' is set** `5688 <https://github.com/lvgl/lvgl/pull/5688>`__
- **feat(event): return event dsc for later to remove** `5630 <https://github.com/lvgl/lvgl/pull/5630>`__
- **feat(keyboard) add Home and End keys mapping** `5675 <https://github.com/lvgl/lvgl/pull/5675>`__
- **feat(draw_buf): distinguish between lv_image_dsc_t and lv_draw_buf_t** `5496 <https://github.com/lvgl/lvgl/pull/5496>`__
- **feat(indev): remove the PRESSED state on scroll** `5660 <https://github.com/lvgl/lvgl/pull/5660>`__
- **feat(sysmon): show max memory usage** `5661 <https://github.com/lvgl/lvgl/pull/5661>`__
- **feat(image_decoder): add stride warning** `5471 <https://github.com/lvgl/lvgl/pull/5471>`__
- **feat(image, layer): add bitmap mask feature** `5545 <https://github.com/lvgl/lvgl/pull/5545>`__
- **feat(draw-sw): add simple Helium acceleration** `5596 <https://github.com/lvgl/lvgl/pull/5596>`__
- **feat(cache): use unified cache entry free callback** `5612 <https://github.com/lvgl/lvgl/pull/5612>`__
- **feat(draw_buf): use draw_buf as parameter of invalidate_cache API** `5602 <https://github.com/lvgl/lvgl/pull/5602>`__
- **feat(script): add ARGB8565 format support** `5593 <https://github.com/lvgl/lvgl/pull/5593>`__
- **feat(vg_lite): add ARGB8565 support** `5592 <https://github.com/lvgl/lvgl/pull/5592>`__
- **feat(vg_lite): add gpu idle flush** `5571 <https://github.com/lvgl/lvgl/pull/5571>`__
- **feat(vg_lite_tvg): add buffer address alignment config** `5576 <https://github.com/lvgl/lvgl/pull/5576>`__
- **feat(vg_lite/vector): add compatible processing for non-scissor support** `5572 <https://github.com/lvgl/lvgl/pull/5572>`__
- **feat(vg_lite): remove support for RGB565A8** `5574 <https://github.com/lvgl/lvgl/pull/5574>`__
- **feat(vg_lite_label): upgrade path quality to HIGH** `5556 <https://github.com/lvgl/lvgl/pull/5556>`__
- **feat(font_glyph_format): refactor draw and font format into `lv_font_glyph_format_t`** `5540 <https://github.com/lvgl/lvgl/pull/5540>`__
- **feat(vg_lite): add index format decode support** `5476 <https://github.com/lvgl/lvgl/pull/5476>`__
- **feat(vector): add API to append arc** `5510 <https://github.com/lvgl/lvgl/pull/5510>`__
- **feat(nuttx_image_cache): add independent image cache heap support** `5528 <https://github.com/lvgl/lvgl/pull/5528>`__
- **feat(cache): refactor cache framework and add new APIs** `5501 <https://github.com/lvgl/lvgl/pull/5501>`__
- **feat(drivers): add libinput/xkb driver** `5486 <https://github.com/lvgl/lvgl/pull/5486>`__
- **feat(draw_buf): misc update** `5498 <https://github.com/lvgl/lvgl/pull/5498>`__
- **feat(draw): add convenience methods for safely getting correct draw descriptor** `5505 <https://github.com/lvgl/lvgl/pull/5505>`__
- **feat(profiler): add multithreading support and testcase** `5490 <https://github.com/lvgl/lvgl/pull/5490>`__
- **feat(doc): Update display-related documentation to new API** `5489 <https://github.com/lvgl/lvgl/pull/5489>`__
- **feat(image_cache): add image header cache drop** `5472 <https://github.com/lvgl/lvgl/pull/5472>`__
- **feat(fbdev): add docs and allow forcing refresh** `5444 <https://github.com/lvgl/lvgl/pull/5444>`__
- **feat(script): add tool to view bin image** `5451 <https://github.com/lvgl/lvgl/pull/5451>`__
- **feat(vg_lite): add profiler instrumentation** `5434 <https://github.com/lvgl/lvgl/pull/5434>`__
- **feat(draw_letter): adapt `lv_font_glyph_format_t` for draw_letter process** `3c1a765 <https://github.com/lvgl/lvgl/commit/3c1a76506e8d948d5c8e029f3467139bdddf7e16>`__
- **feat(color): add color format ARGB8565** `86016a8 <https://github.com/lvgl/lvgl/commit/86016a819a134b2d71777406cfacb3a25d7685cc>`__

Performance
~~~~~~~~~~~

- **perf(draw): minor optimization on rgb565 color blending** `5603 <https://github.com/lvgl/lvgl/pull/5603>`__
- **perf(draw): use the simpler layer clear method** `5470 <https://github.com/lvgl/lvgl/pull/5470>`__
- **perf(vg_lite): add asynchronous rendering support** `5398 <https://github.com/lvgl/lvgl/pull/5398>`__

Fixes
~~~~~

- **fix(obj_tree): fix unintended click triggers on object deletion** `5907 <https://github.com/lvgl/lvgl/pull/5907>`__
- **fix(monkey): add missing include** `5903 <https://github.com/lvgl/lvgl/pull/5903>`__
- **fix(demo/vector_graphic): add missing buffer clear** `5898 <https://github.com/lvgl/lvgl/pull/5898>`__
- **fix(image): fix the image decoder if LV_BIN_DECODER_RAM_LOAD == 0** `5892 <https://github.com/lvgl/lvgl/pull/5892>`__
- **fix(indev): Fix scroll obj cannot be cleared when waiting until release** `5833 <https://github.com/lvgl/lvgl/pull/5833>`__
- **fix(vglite): update NXP's VGLite implementation** `5887 <https://github.com/lvgl/lvgl/pull/5887>`__
- **fix(imagebutton): clipped area of mid part** `5849 <https://github.com/lvgl/lvgl/pull/5849>`__
- **fix(drm): default to XRGB8888 framebuffer** `5851 <https://github.com/lvgl/lvgl/pull/5851>`__
- **fix(vg_lite): fix stroke path memory leak** `5883 <https://github.com/lvgl/lvgl/pull/5883>`__
- **fix(ci): workaround ASAN error in CI with Ubuntu 22.04 image** `5867 <https://github.com/lvgl/lvgl/pull/5867>`__
- **fix(conf): fix typo cause thorvg not enabled** `5837 <https://github.com/lvgl/lvgl/pull/5837>`__
- **fix(indev): always save the last key** `5795 <https://github.com/lvgl/lvgl/pull/5795>`__
- **fix(fsdrv/fatfs): fix does not detect the end of entries in fs_dir_read** `5826 <https://github.com/lvgl/lvgl/pull/5826>`__
- **fix(layer): update the cached layer type on state change** `5801 <https://github.com/lvgl/lvgl/pull/5801>`__
- **fix(file_explorer): fix quick access compiler error** `5783 <https://github.com/lvgl/lvgl/pull/5783>`__
- **fix(msgbox): return the footer in lv_msgbox_get_footer** `5804 <https://github.com/lvgl/lvgl/pull/5804>`__
- **fix(x11): fix assert on delete** `5799 <https://github.com/lvgl/lvgl/pull/5799>`__
- **fix(demo_music): fix the previous button in the music demo is not clickable** `5808 <https://github.com/lvgl/lvgl/pull/5808>`__
- **fix(draw): fix non antialiased rgb565a8 transformation** `5782 <https://github.com/lvgl/lvgl/pull/5782>`__
- **fix(freetpye): fix the problem of incomplete font drawing when setting with italic** `5807 <https://github.com/lvgl/lvgl/pull/5807>`__
- **fix(fsdrv): add missing lv_fs_littlefs_init function declaration** `5778 <https://github.com/lvgl/lvgl/pull/5778>`__
- **fix(vg_lite):fix bug for wrong rendering in vertical or horizontal dir** `5789 <https://github.com/lvgl/lvgl/pull/5789>`__
- **fix(env): added *.cpp glob in ESP configuration** `5761 <https://github.com/lvgl/lvgl/pull/5761>`__
- **fix(draw): fix the invalidation of scaled areas** `5548 <https://github.com/lvgl/lvgl/pull/5548>`__
- **fix(warning): variable set but unused.** `5757 <https://github.com/lvgl/lvgl/pull/5757>`__
- **fix(draw): fix rgb565 with LV_BLEND_MODE_MULTIPLY** `5749 <https://github.com/lvgl/lvgl/pull/5749>`__
- **fix(api_map_v8): add some missing functions** `5710 <https://github.com/lvgl/lvgl/pull/5710>`__
- **fix(refr): fix ARGB8888 buffer clearing in DIRECT mode (#5741)** `5758 <https://github.com/lvgl/lvgl/pull/5758>`__
- **fix(draw_buf): init struct member in order** `5753 <https://github.com/lvgl/lvgl/pull/5753>`__
- **fix(refr): fix clearing the partial buffers in case of ARGB8888 format** `5741 <https://github.com/lvgl/lvgl/pull/5741>`__
- **fix(x11): fix typo** `5742 <https://github.com/lvgl/lvgl/pull/5742>`__
- **fix(draw_vector): check the point length of path in lv_vector_path_get_bounding** `5734 <https://github.com/lvgl/lvgl/pull/5734>`__
- **fix(vg_lite): fix draw layer drawing is incomplete** `5729 <https://github.com/lvgl/lvgl/pull/5729>`__
- **fix(test_span): fix an issue where img has span_1.png, but it is not in the code** `5736 <https://github.com/lvgl/lvgl/pull/5736>`__
- **fix(vg_lite): fix drawing command accumulation** `5730 <https://github.com/lvgl/lvgl/pull/5730>`__
- **fix(vg_lite): fix vector rendering fill dsc matrix not taking effect** `5728 <https://github.com/lvgl/lvgl/pull/5728>`__
- **fix(stdlib): fix issues when using LVGL TLSF memory pool to manage more than 4 GiB of memory** `5720 <https://github.com/lvgl/lvgl/pull/5720>`__
- **fix(vg_lite): remove redundant MOVE_TO operations** `5713 <https://github.com/lvgl/lvgl/pull/5713>`__
- **fix(nuttx): fix compile warning** `5701 <https://github.com/lvgl/lvgl/pull/5701>`__
- **fix(draw_vector): remove redundant MOVE_TO operations** `5715 <https://github.com/lvgl/lvgl/pull/5715>`__
- **fix(draw_buf): use height as loop condition instead of checking data pointers** `5687 <https://github.com/lvgl/lvgl/pull/5687>`__
- **fix(vg_lite): fix the loss of display accuracy of rounded rectangles** `5714 <https://github.com/lvgl/lvgl/pull/5714>`__
- **fix(test): check compiler flag should not contain '_'** `5706 <https://github.com/lvgl/lvgl/pull/5706>`__
- **fix(imagebutton): fix the example** `5719 <https://github.com/lvgl/lvgl/pull/5719>`__
- **fix(drm): eliminate use of non-existent lv_api_map.h and enable smoke tests** `5694 <https://github.com/lvgl/lvgl/pull/5694>`__
- **fix(test): enable -Wpedantic** `5676 <https://github.com/lvgl/lvgl/pull/5676>`__
- **fix(pxp): fix issues in pxp cache management callback** `5685 <https://github.com/lvgl/lvgl/pull/5685>`__
- **fix(windows): improve graphic performance via using high resolution tick count and timer delay implementation** `5711 <https://github.com/lvgl/lvgl/pull/5711>`__
- **fix(windows): improve the document** `5699 <https://github.com/lvgl/lvgl/pull/5699>`__
- **fix(example): fix lv_example_obj_2** `5697 <https://github.com/lvgl/lvgl/pull/5697>`__
- **fix(draw): fix compiler error when LV_DRAW_SW_COMPLEX is disabled** `5695 <https://github.com/lvgl/lvgl/pull/5695>`__
- **fix(test): allow to run test locally on mac** `5672 <https://github.com/lvgl/lvgl/pull/5672>`__
- **fix(sdl): fix buf memleak on lv_display_delete** `5692 <https://github.com/lvgl/lvgl/pull/5692>`__
- **fix(decoder): decoded image should always have same w,h as original image** `5689 <https://github.com/lvgl/lvgl/pull/5689>`__
- **fix(cache): fix warnings** `5671 <https://github.com/lvgl/lvgl/pull/5671>`__
- **fix(dave2d): follow lvgl changes** `5659 <https://github.com/lvgl/lvgl/pull/5659>`__
- **fix(vg_lite): fix typo in decoder_close** `5683 <https://github.com/lvgl/lvgl/pull/5683>`__
- **fix(vg_lite): use float instead of int to avoid type conversion** `5682 <https://github.com/lvgl/lvgl/pull/5682>`__
- **fix(formatting): fix code formatting** `5673 <https://github.com/lvgl/lvgl/pull/5673>`__
- **fix(freetype): fix build break when disable LV_USE_FS_MEMFS** `5651 <https://github.com/lvgl/lvgl/pull/5651>`__
- **fix(array): use memmove instead of memcopy** `5656 <https://github.com/lvgl/lvgl/pull/5656>`__
- **fix(image): fix wrong log level statement** `5655 <https://github.com/lvgl/lvgl/pull/5655>`__
- **fix(tests): fix test_img_emoji_F617.c error include path** `5652 <https://github.com/lvgl/lvgl/pull/5652>`__
- **fix(FreeRTOS): Stacksize calculation** `5647 <https://github.com/lvgl/lvgl/pull/5647>`__
- **fix(benchmark): fix FPS calculation** `5646 <https://github.com/lvgl/lvgl/pull/5646>`__
- **fix(label): consider max-width** `5644 <https://github.com/lvgl/lvgl/pull/5644>`__
- **fix(demo): fix memory leak in vector demo** `5643 <https://github.com/lvgl/lvgl/pull/5643>`__
- **fix(calendar): fix crash when no default is set** `5621 <https://github.com/lvgl/lvgl/pull/5621>`__
- **fix(freetype): fix using memfs to create a FT font** `5627 <https://github.com/lvgl/lvgl/pull/5627>`__
- **fix(indev): fix variable name in preprocessor warning** `5637 <https://github.com/lvgl/lvgl/pull/5637>`__
- **fix(rle): avoid buffer overflow when compressed data is not in pixel unit** `5619 <https://github.com/lvgl/lvgl/pull/5619>`__
- **fix(LVGLImage): use lv_image_dsc_t instead of lv_img_dsc_t** `5629 <https://github.com/lvgl/lvgl/pull/5629>`__
- **fix(vg_lite): fix linear image use after free** `5618 <https://github.com/lvgl/lvgl/pull/5618>`__
- **fix(kconfig): skip lv_conf.h by default** `5617 <https://github.com/lvgl/lvgl/pull/5617>`__
- **fix(image): add data_size filed to all c-array images** `5608 <https://github.com/lvgl/lvgl/pull/5608>`__
- **fix(doc): wrong code for Displays transparency** `5607 <https://github.com/lvgl/lvgl/pull/5607>`__
- **fix(refr): take stride into consideration in partial update mode** `5583 <https://github.com/lvgl/lvgl/pull/5583>`__
- **fix(test): fix typo related to #5559** `5594 <https://github.com/lvgl/lvgl/pull/5594>`__
- **fix(sysmon): fix build error when LV_USE_PERF_MONITOR_LOG_MODE is enabled** `5597 <https://github.com/lvgl/lvgl/pull/5597>`__
- **fix(sdl): use the new draw buffer structure in the layers** `5578 <https://github.com/lvgl/lvgl/pull/5578>`__
- **fix(benchmark): improve the benchmark** `5558 <https://github.com/lvgl/lvgl/pull/5558>`__
- **fix(ffmpeg): add missing stride setting to ffmpeg image** `5580 <https://github.com/lvgl/lvgl/pull/5580>`__
- **fix(sysmon): fix build break** `5585 <https://github.com/lvgl/lvgl/pull/5585>`__
- **fix(tileview): on size change auto. update the pos. of the tiles** `5577 <https://github.com/lvgl/lvgl/pull/5577>`__
- **fix(sdl): fix keyboard handling** `5575 <https://github.com/lvgl/lvgl/pull/5575>`__
- **fix(lv_conf): minor typo fixes in lv_conf_template.h** `5570 <https://github.com/lvgl/lvgl/pull/5570>`__
- **fix(vg_lite): fix arc img not move to arc center** `5554 <https://github.com/lvgl/lvgl/pull/5554>`__
- **fix(vg_lite): add grad image buffer check** `5552 <https://github.com/lvgl/lvgl/pull/5552>`__
- **fix(obj): fix cover change with semi transparent gradients** `5531 <https://github.com/lvgl/lvgl/pull/5531>`__
- **fix(vg_lite): path matrix should use main vector matrix** `5538 <https://github.com/lvgl/lvgl/pull/5538>`__
- **fix(vg_lite): fix LV_RADIUS_CIRCLE not round** `5543 <https://github.com/lvgl/lvgl/pull/5543>`__
- **fix(vg_lite): add config to disable draw shadow** `5534 <https://github.com/lvgl/lvgl/pull/5534>`__
- **fix(neon): update stride from px to bytes** `5526 <https://github.com/lvgl/lvgl/pull/5526>`__
- **fix(script): Remove TRUECOLOR for LVGLImage.py** `5523 <https://github.com/lvgl/lvgl/pull/5523>`__
- **fix(sysmon): support starting LVGL without having a display** `5518 <https://github.com/lvgl/lvgl/pull/5518>`__
- **fix(chart): set the series id correctly** `5482 <https://github.com/lvgl/lvgl/pull/5482>`__
- **fix(vg_lite): fix clip corner image display error** `5517 <https://github.com/lvgl/lvgl/pull/5517>`__
- **fix(arduino): update example LVGL_Arduino.ino for v9** `5499 <https://github.com/lvgl/lvgl/pull/5499>`__
- **fix(fb): set DPI to correctly** `5508 <https://github.com/lvgl/lvgl/pull/5508>`__
- **fix(layer): Update calculation for overlay used memory** `5504 <https://github.com/lvgl/lvgl/pull/5504>`__
- **fix(bin_decoder): fix potential free garbage address** `5509 <https://github.com/lvgl/lvgl/pull/5509>`__
- **fix(decoder): treat ALLOCATED c-array image as draw buffer** `5483 <https://github.com/lvgl/lvgl/pull/5483>`__
- **fix(vg_lite_tvg): fix use after free when change target canvas** `5497 <https://github.com/lvgl/lvgl/pull/5497>`__
- **fix(keyboard): in the example set Arabic font if enabled** `5457 <https://github.com/lvgl/lvgl/pull/5457>`__
- **fix(codespaces): fix the issue where lv_conf.h in codespaces is not enable** `5484 <https://github.com/lvgl/lvgl/pull/5484>`__
- **fix(textarea): fix accepted chars issue on big endian systems** `5479 <https://github.com/lvgl/lvgl/pull/5479>`__
- **fix(draw_line): fix the issue where dash_dap equals 1 and cannot display properly when the line is a horizontal line** `5473 <https://github.com/lvgl/lvgl/pull/5473>`__
- **fix: removed LV_ATTRIBUTE_FAST_MEM function attribute from prototypes** `5467 <https://github.com/lvgl/lvgl/pull/5467>`__
- **fix(arm2d): apply a temporary patch to arm-2d acceleration** `5466 <https://github.com/lvgl/lvgl/pull/5466>`__
- **fix(snapshot): update the layout of the component before the screenshot** `5475 <https://github.com/lvgl/lvgl/pull/5475>`__
- **fix(vg_lite): add missing 24bit color support check** `5469 <https://github.com/lvgl/lvgl/pull/5469>`__
- **fix(display): set last_activity_time to the current time in lv_display** `5463 <https://github.com/lvgl/lvgl/pull/5463>`__
- **fix(observer): fixed lv_subject_remove_all_obj** `5464 <https://github.com/lvgl/lvgl/pull/5464>`__
- **fix(image_decoder): fix decoder not close** `5437 <https://github.com/lvgl/lvgl/pull/5437>`__
- **fix(nuttx): fix build break** `5440 <https://github.com/lvgl/lvgl/pull/5440>`__
- **fix: fix warnings with -flto on GCC 11** `5433 <https://github.com/lvgl/lvgl/pull/5433>`__
- **fix(assert): add new macro to format assert message** `5453 <https://github.com/lvgl/lvgl/pull/5453>`__
- **fix(decoder): if draw unit supports indexed image, don't add to cache** `5438 <https://github.com/lvgl/lvgl/pull/5438>`__
- **fix(bar): mask the background to fix it on value adjustment** `5426 <https://github.com/lvgl/lvgl/pull/5426>`__
- **fix(lvgl.mk): fix vg_lite_tvg.cpp not compiling** `5435 <https://github.com/lvgl/lvgl/pull/5435>`__
- **fix(ci): use the dev branch of PlatformIO** `5432 <https://github.com/lvgl/lvgl/pull/5432>`__
- **fix(vector) : add path bounding and matrix transform functions.** `5389 <https://github.com/lvgl/lvgl/pull/5389>`__
- **fix(warning): fix shadown variable warning** `47750f1 <https://github.com/lvgl/lvgl/commit/47750f1b866e5ea0617035fd208c727878bebc44>`__
- **fix(thorvg): link lvgl_thorvgl with lvgl** `9b09182 <https://github.com/lvgl/lvgl/commit/9b09182fc76032ef0bc8a2d930fa1cf4fd081431>`__
- **fix(warning): error: no newline at end of file** `9a6a194 <https://github.com/lvgl/lvgl/commit/9a6a194680db9ea12f59e94eab6e812cb28d504f>`__
- **fix(color): treat RGB565A8 bpp same as RGB565** `52426ec <https://github.com/lvgl/lvgl/commit/52426ec1919274e282889129f00e00a9a2a9ce60>`__
- **fix(warning): error: a function declaration without a prototype is deprecated in all versions of C** `c81f654 <https://github.com/lvgl/lvgl/commit/c81f654026501ba37d8df2d8ec02c58bd14eb1c3>`__

Examples
~~~~~~~~

Docs
~~~~

- **docs: update README** `5841 <https://github.com/lvgl/lvgl/pull/5841>`__
- **docs: make it easy to add more other translations** `5874 <https://github.com/lvgl/lvgl/pull/5874>`__
- **docs: ignore the READMEs when building the docs** `5840 <https://github.com/lvgl/lvgl/pull/5840>`__
- **docs(st7789): updated the docs + added code example and step-by-step guide for STM32** `5511 <https://github.com/lvgl/lvgl/pull/5511>`__
- **docs(arduino): update tick setup** `5832 <https://github.com/lvgl/lvgl/pull/5832>`__
- **docs(display): use lv_display_delete_refr_timer to delete display timer** `5835 <https://github.com/lvgl/lvgl/pull/5835>`__
- **docs(lv_conf): show how to include something in lv_conf.h** `5740 <https://github.com/lvgl/lvgl/pull/5740>`__
- **docs(tick): simplify and promote lv_tick_set_cb** `5781 <https://github.com/lvgl/lvgl/pull/5781>`__
- **docs(style): output the style properties to style-props.rst** `5802 <https://github.com/lvgl/lvgl/pull/5802>`__
- **docs(changelog): mention that LV_COLOR_DEPTH 8 is not supported yet** `5796 <https://github.com/lvgl/lvgl/pull/5796>`__
- **docs(image): update align to inner_align** `5721 <https://github.com/lvgl/lvgl/pull/5721>`__
- **docs(README_Zh): remove mentions of SquareLine Studio** `5640 <https://github.com/lvgl/lvgl/pull/5640>`__
- **docs(profiler): fix my_get_cpu_cb implementation** `5641 <https://github.com/lvgl/lvgl/pull/5641>`__
- **docs(demos): remove inconsistent READMEs** `5626 <https://github.com/lvgl/lvgl/pull/5626>`__
- **docs(README): remove mentions of SquareLine Studio** `5638 <https://github.com/lvgl/lvgl/pull/5638>`__
- **docs(changelog): mention more features removed in v9** `5632 <https://github.com/lvgl/lvgl/pull/5632>`__
- **docs(micropython): update MicroPython examples in documentation** `5622 <https://github.com/lvgl/lvgl/pull/5622>`__
- **docs(contributing): fix links** `5615 <https://github.com/lvgl/lvgl/pull/5615>`__
- **docs(porting): add missing colon in porting docs** `5613 <https://github.com/lvgl/lvgl/pull/5613>`__
- **docs(contributing): update the feature development workflow** `5601 <https://github.com/lvgl/lvgl/pull/5601>`__
- **docs(tabview): fix tabview doc** `5588 <https://github.com/lvgl/lvgl/pull/5588>`__
- **docs(indev): Document the relation between LV_INDEV_MODE_EVENT and data-&gt;continue_reading** `5586 <https://github.com/lvgl/lvgl/pull/5586>`__
- **docs(pc-simulator): Document how to set up SDL manually, without IDE** `5563 <https://github.com/lvgl/lvgl/pull/5563>`__
- **docs(disp): document lv_refr_now** `5480 <https://github.com/lvgl/lvgl/pull/5480>`__
- **docs(disp): Fix pointer cast in flush_cb example** `5439 <https://github.com/lvgl/lvgl/pull/5439>`__
- **docs(font): about "base dir"** `5429 <https://github.com/lvgl/lvgl/pull/5429>`__

CI and tests
~~~~~~~~~~~~

- **test(conf): update conf and remove deprecated configs** `5881 <https://github.com/lvgl/lvgl/pull/5881>`__
- **tests(freetype): refactor code structure** `5871 <https://github.com/lvgl/lvgl/pull/5871>`__
- **ci(micropython): catch event failure in the test** `5787 <https://github.com/lvgl/lvgl/pull/5787>`__
- **ci(ref_imgs): automatically generate the missing reference image folders** `5696 <https://github.com/lvgl/lvgl/pull/5696>`__
- **ci(assets): add LV_BUILD_TEST guard** `5616 <https://github.com/lvgl/lvgl/pull/5616>`__
- **ci(codecov): disable CodeCov** `5623 <https://github.com/lvgl/lvgl/pull/5623>`__
- **test(asset): add guard to test_img_emoji_F617** `5559 <https://github.com/lvgl/lvgl/pull/5559>`__
- **ci(micropython): use the master branch** `5460 <https://github.com/lvgl/lvgl/pull/5460>`__
- **test(snapshot): add testcase for #5475** `5478 <https://github.com/lvgl/lvgl/pull/5478>`__

Others
~~~~~~

- **chore(cmsis-pack): prepare for v9.1.0** `5917 <https://github.com/lvgl/lvgl/pull/5917>`__
- **chore: fix header files include recursion** `5844 <https://github.com/lvgl/lvgl/pull/5844>`__
- **chore(group): avoid null pointer access** `5863 <https://github.com/lvgl/lvgl/pull/5863>`__
- **refactor(conf): rename LV_DRAW_SW_LAYER_SIMPLE_BUF_SIZE to LV_DRAW_LAYER_SIMPLE_BUF_SIZE** `5798 <https://github.com/lvgl/lvgl/pull/5798>`__
- **chore(freetype): remove unused codes** `5885 <https://github.com/lvgl/lvgl/pull/5885>`__
- **chore: fix compile time warnings** `5872 <https://github.com/lvgl/lvgl/pull/5872>`__
- **adds `extern "C"` to the header files that were missing it.** `5857 <https://github.com/lvgl/lvgl/pull/5857>`__
- **chore(stdlib): remove duplicate function declaration** `5845 <https://github.com/lvgl/lvgl/pull/5845>`__
- **chore(benchmark): add warning for probably low memory** `5797 <https://github.com/lvgl/lvgl/pull/5797>`__
- **chore(group): add assertion to avoid null pointer access** `5769 <https://github.com/lvgl/lvgl/pull/5769>`__
- **refact(Kconfig): update as per lv_conf_template.h** `5780 <https://github.com/lvgl/lvgl/pull/5780>`__
- **Fix the compilation error problem of lvgl9.x on rtthread** `5794 <https://github.com/lvgl/lvgl/pull/5794>`__
- **refact(vg_lite): abstract common pending release logic** `5756 <https://github.com/lvgl/lvgl/pull/5756>`__
- **doc(stm32): fix typo** `5772 <https://github.com/lvgl/lvgl/pull/5772>`__
- **doc(cache): add doc strings for cache APIs** `5718 <https://github.com/lvgl/lvgl/pull/5718>`__
- **chore(docs): fix typo** `5738 <https://github.com/lvgl/lvgl/pull/5738>`__
- **chore(font): remove unused defines** `5716 <https://github.com/lvgl/lvgl/pull/5716>`__
- **chore: add stride information in the image resource file** `5653 <https://github.com/lvgl/lvgl/pull/5653>`__
- **chore(draw_buf): remove unnecessary info logs** `5634 <https://github.com/lvgl/lvgl/pull/5634>`__
- **chore(cmsis-pack): catchup update** `5611 <https://github.com/lvgl/lvgl/pull/5611>`__
- **More chore work in indev files** `5604 <https://github.com/lvgl/lvgl/pull/5604>`__
- **Stride adjust in place** `5423 <https://github.com/lvgl/lvgl/pull/5423>`__
- **chore(conf): use EXTERNAL and INTERNAL macro to config libs** `5046 <https://github.com/lvgl/lvgl/pull/5046>`__
- **chore(vg_lite): use pointer to access matrix element** `5591 <https://github.com/lvgl/lvgl/pull/5591>`__
- **refactor(image): rename align to inner_align** `5560 <https://github.com/lvgl/lvgl/pull/5560>`__
- **chore(deps): bump codecov/codecov-action from 3 to 4** `5567 <https://github.com/lvgl/lvgl/pull/5567>`__
- **chore(deps): bump carlosperate/arm-none-eabi-gcc-action from 1.8.0 to 1.8.1** `5565 <https://github.com/lvgl/lvgl/pull/5565>`__
- **chore(deps): bump mymindstorm/setup-emsdk from 13 to 14** `5566 <https://github.com/lvgl/lvgl/pull/5566>`__
- **chore(deps): bump actions/cache from 3 to 4** `5568 <https://github.com/lvgl/lvgl/pull/5568>`__
- **chore(deps): bump uraimo/run-on-arch-action from 2.6.0 to 2.7.1** `5569 <https://github.com/lvgl/lvgl/pull/5569>`__
- **optionally install demos and libs as well** `5387 <https://github.com/lvgl/lvgl/pull/5387>`__
- **chore(arc): fix typo** `5553 <https://github.com/lvgl/lvgl/pull/5553>`__
- **Revert "feat(font_glyph_format): refactor draw and font format into `lv_font_glyph_format_t`"** `5550 <https://github.com/lvgl/lvgl/pull/5550>`__
- **chore(thorvg): fix warning** `5535 <https://github.com/lvgl/lvgl/pull/5535>`__
- **refactor(snapshot): use draw buffer interface** `5487 <https://github.com/lvgl/lvgl/pull/5487>`__
- **chore(vg_lite): fix typo** `5525 <https://github.com/lvgl/lvgl/pull/5525>`__
- **demo(music): add png assets** `5520 <https://github.com/lvgl/lvgl/pull/5520>`__
- **chore(indev): Cleanup before working in double and triple click support** `5512 <https://github.com/lvgl/lvgl/pull/5512>`__
- **chore(demos): Remove unused definitions** `5506 <https://github.com/lvgl/lvgl/pull/5506>`__
- **chore(obj_draw): remove duplicate invalidate** `5503 <https://github.com/lvgl/lvgl/pull/5503>`__
- **chore(widgets): add parentheses to MY_CLASS definition** `5485 <https://github.com/lvgl/lvgl/pull/5485>`__
- **chore(draw-sw-transform): remove duplicated code** `5488 <https://github.com/lvgl/lvgl/pull/5488>`__
- **chore(README): Fixed #5491 Typo** `5492 <https://github.com/lvgl/lvgl/pull/5492>`__
- **chore(vg_lite): remove 64-bytes alignment requirement** `5477 <https://github.com/lvgl/lvgl/pull/5477>`__
- **chore(refr): fix typo** `5474 <https://github.com/lvgl/lvgl/pull/5474>`__
- **chore(array): use array_front and use it like an array** `5448 <https://github.com/lvgl/lvgl/pull/5448>`__
- **chore(cache): only cache header info for file type of image** `5455 <https://github.com/lvgl/lvgl/pull/5455>`__
- **refactor(indev_scroll): refactor code** `5456 <https://github.com/lvgl/lvgl/pull/5456>`__
- **chore(draw_buf): add header magic to draw buff** `5449 <https://github.com/lvgl/lvgl/pull/5449>`__
- **chore(Kconfig): add missing profiler trace buffer size config** `5436 <https://github.com/lvgl/lvgl/pull/5436>`__
- **chore(libpng): fix warning** `5431 <https://github.com/lvgl/lvgl/pull/5431>`__

Others
~~~~~~


- **refactor(freetype): refactor freetype params** `0c84cc0 <https://github.com/lvgl/lvgl/commit/0c84cc0b3b9b1ea37bd6aa5300e91eee0f2feb98>`__
- **refactor(freetype): refactor glyph_index acquire method** `0b3016c <https://github.com/lvgl/lvgl/commit/0b3016c0e5b5d57141220c030c0aadd9d9c3387e>`__
- **chore(README): Fixed #5491 Typo (#5492)** `9c025d3 <https://github.com/lvgl/lvgl/commit/9c025d357f358c281db441a96a27ce2a01434a8d>`__


v9.0
----

For Other v9.0.x releases visit the `Changelog in the release/v9.0 branch <https://github.com/lvgl/lvgl/blob/release/v9.0/docs/CHANGELOG.rst>`__ .

