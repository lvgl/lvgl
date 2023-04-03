# Changelog

## [v8.3.6](https://github.com/lvgl/lvgl/compare/v8.3.6...v8.3.5) 3 April 2023

### New Features

- feat(msg): add lv_msg_unsubcribe_obj [`6af0179`](https://github.com/lvgl/lvgl/commit/6af01798d82f90f0c2ba6a9da39c4f10fb427df7)

### Performance

### Fixes

- fix(group): fix default_group becomes wild pointer when deleted [`4076`](https://github.com/lvgl/lvgl/pull/4076)
- fix(fs_posix): allow creating new file and set permission. [`3976`](https://github.com/lvgl/lvgl/pull/3976)
- fix(img): support negative angles [`3846`](https://github.com/lvgl/lvgl/pull/3846)
- fix(gif): synchronize with master [`4003`](https://github.com/lvgl/lvgl/pull/4003)
- fix(gpu): fix STM GPU drivers for some variants [`4004`](https://github.com/lvgl/lvgl/pull/4004)
- fix(img): possible divide by 0 exception (lvgl#3988) [`3990`](https://github.com/lvgl/lvgl/pull/3990)
- fix(arc): fix knob area invalidation [`d0e19eb`](https://github.com/lvgl/lvgl/commit/d0e19eb2d38ba8a500399b0496d7a8363be4003e)
- fix(slider): consider animations on pressing [`0b7777f`](https://github.com/lvgl/lvgl/commit/0b7777f27a7932efe3d594be426e1beb59d80ae3)
- fix(bar): delete running animations when a new value is set without animation [`aa31380`](https://github.com/lvgl/lvgl/commit/aa313806d0ebde475fc2bc360a15172cc1b658a7)
- docs: use a fixed commit of lv_web_emscripten [`501230e`](https://github.com/lvgl/lvgl/commit/501230e0fc95936199b3187d350873c3bb4a94e4)

### Examples

### Docs

- docs(arduino): add note to not use lv_examles library [`2f294aa`](https://github.com/lvgl/lvgl/commit/2f294aa76c8fece98a4fa72304bc6f267ed2a228)
- docs: use a fixed commit of lv_web_emscripten [`501230e`](https://github.com/lvgl/lvgl/commit/501230e0fc95936199b3187d350873c3bb4a94e4)

### CI and tests

### Others

- chore(cmsis-pack): update cmsis-pack for v8.3.6 [`4108`](https://github.com/lvgl/lvgl/pull/4108)
- chore: update the version numbers to v8.3.5-dev [`77670fb`](https://github.com/lvgl/lvgl/commit/77670fb1a55e0f2012ff7a057e535830e7253e22)
- Update build_html_examples.sh [`399069b`](https://github.com/lvgl/lvgl/commit/399069b4a2423c11823581668fe71ce9a7c88e7d)


## [v8.3.5](https://github.com/lvgl/lvgl/compare/v8.3.4...v8.3.5) 7 February 2023

### Performance

- perf(gpu): improve NXP's PXP and VGLite accelerators [`3952`](https://github.com/lvgl/lvgl/pull/3952)
- perf(dam2d): rework stm32 dma2d  [`3904`](https://github.com/lvgl/lvgl/pull/3904)

### Fixes

- fix(monkey): remove executable permissions from source files [`3971`](https://github.com/lvgl/lvgl/pull/3971)
- fix(ci): set Ubuntu version for MicroPython test [`3865`](https://github.com/lvgl/lvgl/pull/3865)
- fix(Kconfig): fix wrong type of LV_FS_STDIO_CACHE_SIZE (v8.3) [`3906`](https://github.com/lvgl/lvgl/pull/3906)
- docs(indev): fix the name of long_press_repeat_time (was long_press_rep_time) [`34c545e`](https://github.com/lvgl/lvgl/commit/34c545ef19dc97c8952a412e533a4cd3924b9fbc)
- fix(roller): consider the recolor setting of the label [`39f4247`](https://github.com/lvgl/lvgl/commit/39f424767fa57376c4cb08cf22951fd56d854fd6)

### Examples

### Docs

- docs(indev): fix the name of long_press_repeat_time (was long_press_rep_time) [`34c545e`](https://github.com/lvgl/lvgl/commit/34c545ef19dc97c8952a412e533a4cd3924b9fbc)

### CI and tests

- ci(esp): fix push to the component registry on tag [`e529230`](https://github.com/lvgl/lvgl/commit/e529230f4bb97b4506c430aac96d5ddaef685dc4)

### Others

- chore(cmsis-pack): update cmsis-pack for v8.3.5 [`3972`](https://github.com/lvgl/lvgl/pull/3972)
- chore: add an option to "LV_TICK_CUSTOM" [`3879`](https://github.com/lvgl/lvgl/pull/3879)

- bump version numbers to v8.3.5-dev [`47c8f8f`](https://github.com/lvgl/lvgl/commit/47c8f8f9822f4c0c0ffbe2f12b380bddefcec475)
- Update layer.md [`9faca8a`](https://github.com/lvgl/lvgl/commit/9faca8a8d4125e21dedbf6e46aa1586a6b57e5b8)

## [v8.3.4](https://github.com/lvgl/lvgl/compare/v8.3.4...v8.3.3) 15 December 2022

### New Features

- feat(keyboard): ported arabic keyboard from release 7.10.0 [`3728`](https://github.com/lvgl/lvgl/pull/3728)
- feat(table): scroll to the selected cell with key navigation [`39d03a8`](https://github.com/lvgl/lvgl/commit/39d03a80f45847a1977cfe9cc6a509b1613d0aca)

### Fixes

- fix(rt-thread): sync rt-thread v5.0.0 rt_align [`3864`](https://github.com/lvgl/lvgl/pull/3864)
- fix(draw): SDL2 gradient support  #3848 [`3856`](https://github.com/lvgl/lvgl/pull/3856)
- fix(esp.cmake): add demos and examples [`3784`](https://github.com/lvgl/lvgl/pull/3784)
- fix(indev): fix scrolling on transformed obejcts [`84cf05d`](https://github.com/lvgl/lvgl/commit/84cf05d8b23b31e000db757a278545e58fcbcbe8)
- fix(style): add the missing support for pct pivot in tranasform style properties [`c8e584f`](https://github.com/lvgl/lvgl/commit/c8e584f879a1e1427e7a8f5ff496af39f17df41d)
- fix(flex): be sure obj-&gt;w_layout and h_layout can't be set at the same time [`c4c4007`](https://github.com/lvgl/lvgl/commit/c4c400716e80a279e7b3d43b8992915fe94441eb)
- fix(chart): fix very dense bar charts [`bb2c2ac`](https://github.com/lvgl/lvgl/commit/bb2c2ac34ac943978513c7ed51885078979b1c10)
- fix(draw): handle LV_COLOR_DEPTH == 1 too in lv_draw_sw_transform [`bd11ad8`](https://github.com/lvgl/lvgl/commit/bd11ad8542eac9ff51420e5afb80f7e6fcf36a5c)
- fix(example): fix warnings [`1e3ca25`](https://github.com/lvgl/lvgl/commit/1e3ca25fed13bbf85c32a60d4b7041cf8bd525ab)
- fix(benchmark): fix warnings [`1ed026c`](https://github.com/lvgl/lvgl/commit/1ed026ca7307957568fe419f1ff39a15b2535b3e)
- fix(draw): fix text color with sub pixel rendering and BGR order [`e050f5c`](https://github.com/lvgl/lvgl/commit/e050f5ca156f79d752894f38f0a437c946205cb4)
- fix(meter): fix setting part_draw_dsc.id in needle img drawing [`716e5e2`](https://github.com/lvgl/lvgl/commit/716e5e2c8bd2a22e7d56e8d7ca33054a11a1f4ed)
- fix(gridnav): fix stucking in pressed state with encoder [`ad56dfa`](https://github.com/lvgl/lvgl/commit/ad56dfaf7046a9bb8c05e877a8c8852cd14a59af)
- fix(darw): add back the disappeared antialising=0 support [`2c17b28`](https://github.com/lvgl/lvgl/commit/2c17b28ac476c95a4153ab6cabb77b1c7208bb74)
- fix(msg): fix typos in API by adding wrappers [`41fa416`](https://github.com/lvgl/lvgl/commit/41fa41613455260ccdeb87ecb890ce026ff0a435)
- fix(draw): fix transformation accuracy [`e06f03d`](https://github.com/lvgl/lvgl/commit/e06f03db72f98439078118518158f52439dd7bf8)
- fix(style): remove the reduntant define of LV_GRADIENT_MAX_STOPS [`903e94b`](https://github.com/lvgl/lvgl/commit/903e94b716ca1b32cdb51de11df679953699e53b)
- demo(benchmark): fix lv_label_set_text_fmt format strings [`ae38258`](https://github.com/lvgl/lvgl/commit/ae3825871e31cd42cad2f310bdfc605150670511)
- demo(benchmark): fix warning [`1173dcb`](https://github.com/lvgl/lvgl/commit/1173dcba96621e20c9a7240c8572bd6573bce6a0)

## [v8.3.3](https://github.com/lvgl/lvgl/compare/v8.3.2...v8.3.3) 06 October 2022

v8.3.3 is the same as v8.3.2. It was released only because the version number was set incorrectly in lvgl.h.

## [v8.3.2](https://github.com/lvgl/lvgl/compare/v8.3.1...v8.3.2) 27 September 2022

### Fixes

- fix(fragment): fixed child fragment event dispatch [`3683`](https://github.com/lvgl/lvgl/pull/3683)
- fix(sdl): clear streaming/target texture with FillRect [`3682`](https://github.com/lvgl/lvgl/pull/3682)
- fix(sdl): transformation with alpha (#3576) [`3678`](https://github.com/lvgl/lvgl/pull/3678)
- fix(draw_sw): fix image cache to access the freed stack space [`3584`](https://github.com/lvgl/lvgl/pull/3584)
- fix(style): use compile time prop_cnt for const styles [`3609`](https://github.com/lvgl/lvgl/pull/3609)
- fix(demo): can not found lvgl.h file [`3477`](https://github.com/lvgl/lvgl/pull/3477)
- fix(ci) checkout lv_micropython release/v8 branch [`3524`](https://github.com/lvgl/lvgl/pull/3524)
- fix(canvas): fix clipéping on transformation [`b884aba`](https://github.com/lvgl/lvgl/commit/b884abae26f3824b27783a85d18ed51e550347c1)
- fix(draw): allow drawing outline with LV_DRAW_COMPLEX == 0 too [`ece3495`](https://github.com/lvgl/lvgl/commit/ece34950040e218fc73605a4e88f1060c2a274f8)
- fix(colorwheel): fix updating color when using lv_colorwheel_set_hsv [`d59bba1`](https://github.com/lvgl/lvgl/commit/d59bba12db115afb4b6aa53eed2625221dfff2fd)
- fix(slider): find the nearest value on click instead of floor [`dfd14fa`](https://github.com/lvgl/lvgl/commit/dfd14fa778aef25d0db61748a58aa9989ce5e2c8)
- fix(draw): fix border drawing with thick borders [`d5b2a9b`](https://github.com/lvgl/lvgl/commit/d5b2a9b2562cbfa327bf0ec03c11d28576037a14)
- fix(refr): fix true double double buffering logic with transparent screens [`8b605cc`](https://github.com/lvgl/lvgl/commit/8b605cc48224d0497cdd936fa77229e0c3d606d2)
- fix(group): be sure obj is removed from its current group in lv_group_add_obj [`5156ee0`](https://github.com/lvgl/lvgl/commit/5156ee058d5de674a00ffd84d15d460de7f0e53b)
- fix(style): add missing invalidation in lv_obj_remove_local_style_prop [`a0515ba`](https://github.com/lvgl/lvgl/commit/a0515ba30dd74b8b22a6709d334eb03782ee1a4d)

### Docs

- docs(draw) remove reference to old lv_fs_add_drv function [`3564`](https://github.com/lvgl/lvgl/pull/3564)
- docs(disp): LV_COLOR_SCREEN_TRANSP remove dependency on LV_COLOR_DEPTH_32 as transparency is supported across all color depths [`3556`](https://github.com/lvgl/lvgl/pull/3556)

### CI and tests

- ci: protect test.c with #if LV_BUILD_TEST [`be485d7`](https://github.com/lvgl/lvgl/commit/be485d7605136d2a5d6a633c7cb5b7c525cae7ee)

### Others

- chore(rt-thread) backport fixes from v9 [`3604`](https://github.com/lvgl/lvgl/pull/3604)

- chore: fix warnings [`7640950`](https://github.com/lvgl/lvgl/commit/76409502163ffe67cfbab9c7f24f2226cc8a5941)
- remove accidentally added code [`5022476`](https://github.com/lvgl/lvgl/commit/5022476edc8676f2a6ef7b919d3578159edeef7c)


## [v8.3.1](https://github.com/lvgl/lvgl/compare/v8.3.0...v8.3.1) 25 July 2022

### Fixes

- fix(led): add bg_color draw descriptors back to led draw event to support LV_DRAW_COMPLEX 0 [`3515`](https://github.com/lvgl/lvgl/pull/3515)
- fix(slider): fix knob drawing in symmetrical mode [`2967172`](https://github.com/lvgl/lvgl/commit/2967172bee806e77da6ee2307c79e867af3f76bc)
- fix(refr): fix lv_refr_get_top_obj [`9750c97`](https://github.com/lvgl/lvgl/commit/9750c97aff4dc3de80559b150852b829f006d6bf)
- fix(arc): fix arc knob invalidation in SYMMETRICAL mode [`a283273`](https://github.com/lvgl/lvgl/commit/a283273bd27599dae6b044a941b6591ad45e059b)

### Examples

- example(freetype): Update the Micropython example to use the Lato font [`71913d3`](https://github.com/lvgl/lvgl/commit/71913d300dde25d1b87d1b44fa1fa47854defd59)
- example(freetype): replace the arial font with lato to avoid licensing issues [`8544cc3`](https://github.com/lvgl/lvgl/commit/8544cc38062d9c817013bbe6aedbb47112e580ad)

### Docs

- docs(readme): fix LVGL version typo (8.3.0) [`3462`](https://github.com/lvgl/lvgl/pull/3462)
- docs(tasmota): support LVGL 8.3.0 (#3511) [`62662f6`](https://github.com/lvgl/lvgl/commit/62662f68e9cf90adcb96d42030eca5fa135b96a5)



## [v8.3.0](https://github.com/lvgl/lvgl/compare/v8.2.0...v8.3.0) 6 July 2022


### Overview

- **Layers** Support transforming (zoom and rotate) any widgets and their children drawn by LVGL. To do this LVGL renders the transformed widgets into a layer and draws that layer as an image applying all the transformations. Layers are also used when `opa` (not `bg_opa`, `border_opa`, etc) and `blend_mode` are set. This way nested objects are blended as one layer to avoid color bleeding. See more [here](https://docs.lvgl.io/master/overview/style.html#opacity-blend-modes-and-transformations).
- **inherit and initial style properties** Besides setting "normal values" for style properties now you can set them to `inherit` (inherit the parent's value) and `initial` (set the system default). See more [here](https://docs.lvgl.io/master/overview/style.html#forced-value-inheritance-default-value)
- **NXP-PXP and VGLITE GPU support** The support for NXP GPUs are added again
- **Color font support** You can use emojis and images in texts with this great new features. See more [here](https://docs.lvgl.io/master/others/imgfont.html).
- **ARM2D GPU support** Get support for ARM's own GPU.
- **PubSub messaging** A publisher-subscriber based messaging system is added to make communication between components easier. See more [here](https://docs.lvgl.io/master/others/msg.html).
- **Pinyin IME** Add support for Pinyin IME Chinese input. See more [here](https://docs.lvgl.io/master/others/ime_pinyin.html).
- **render_start_cb** A new callback is added to `lv_disp_drv_t` to indicate when the rendering starts. It's useful to make synchronization, e.g. wait for a TE signal.


### New Features

- feat(ime_pinyin): add API to support 9-key input mode [`3447`](https://github.com/lvgl/lvgl/pull/3447)
- feat(font): add font placeholder drawing configuration [`3446`](https://github.com/lvgl/lvgl/pull/3446)
- feat(fsdrv): add posix lseek() error checking [`3444`](https://github.com/lvgl/lvgl/pull/3444)
- feat(misc): add asynchronous call function cancellation function [`3439`](https://github.com/lvgl/lvgl/pull/3439)
- feat(ime_pinyin): add API to use Pinyin IME(Chinese input) [`3408`](https://github.com/lvgl/lvgl/pull/3408)
- feat(style) add 'inherit' and 'initial' CSS properties [`3390`](https://github.com/lvgl/lvgl/pull/3390)
- feat(porting): add flushing control to the template [`3384`](https://github.com/lvgl/lvgl/pull/3384)
- feat(anim): add deleted callback (#3279) [`3295`](https://github.com/lvgl/lvgl/pull/3295)
- feat(cmsis-pack): monthly update for May [`3394`](https://github.com/lvgl/lvgl/pull/3394)
- feat(textarea): make it possible to customize the bullet character [`3388`](https://github.com/lvgl/lvgl/pull/3388)
- feat(disp): add a temporary invalidation disable interface [`3378`](https://github.com/lvgl/lvgl/pull/3378)
- feat(group): add edge callbacks when trying to move focus past beginning or end [`3374`](https://github.com/lvgl/lvgl/pull/3374)
- feat(benchmark): make lvgl render at the highest frame rate [`3352`](https://github.com/lvgl/lvgl/pull/3352)
- feat(rt-thread): allow users to control refresh period in the lvgl thread [`3375`](https://github.com/lvgl/lvgl/pull/3375)
- feat(cmsis-pack): Monthly update for May (alpha) [`3359`](https://github.com/lvgl/lvgl/pull/3359)
- feat(demos): add a callback for benchmark [`3353`](https://github.com/lvgl/lvgl/pull/3353)
- feat(gpu): Update lv_gpu_arm2d with new features [`3340`](https://github.com/lvgl/lvgl/pull/3340)
- feat(draw): improve acceleration for LV_IMG_CF_ALPHA_8BIT [`3337`](https://github.com/lvgl/lvgl/pull/3337)
- feat(anim): add the function of getting global animation refresher timer [`3331`](https://github.com/lvgl/lvgl/pull/3331)
- feat(demo): add Weighted FPS and Opa speed log output [`3326`](https://github.com/lvgl/lvgl/pull/3326)
- feat(gpu): Update gpu arm 2d  [`3320`](https://github.com/lvgl/lvgl/pull/3320)
- feat(cmsis-pack): Monthly update for April [`3300`](https://github.com/lvgl/lvgl/pull/3300)
- feat(fsdrv) fix issues for win32 backends [`3284`](https://github.com/lvgl/lvgl/pull/3284)
- feat(cmake-build): Option to allow building shared libraries. [`3278`](https://github.com/lvgl/lvgl/pull/3278)
- feat(hal): add render_start_cb to disp_drv [`3274`](https://github.com/lvgl/lvgl/pull/3274)
- feat(cmsis-pack): monthly update for April (v1.0.3-alpha) [`3271`](https://github.com/lvgl/lvgl/pull/3271)
- feat(benchmark): add trace output for running a specific scenario [`3245`](https://github.com/lvgl/lvgl/pull/3245)
- feat(env_support): cmsis pack monthly update [`3209`](https://github.com/lvgl/lvgl/pull/3209)
- feat(tabview): support vertical scrolling [`3184`](https://github.com/lvgl/lvgl/pull/3184)
- feat(span): add an interface for setting the number of lines [`3200`](https://github.com/lvgl/lvgl/pull/3200)
- feat(indev): add possibility to enable/disable all input devices at once [`3179`](https://github.com/lvgl/lvgl/pull/3179)
- feat(font): add imgfont - can be used to add emojis to label/span  [`3160`](https://github.com/lvgl/lvgl/pull/3160)
- feat(gpu): add gpu arm2d [`3162`](https://github.com/lvgl/lvgl/pull/3162)
- feat(dma2d): add lv_draw_stm32_dma2d_buffer_copy function [`3147`](https://github.com/lvgl/lvgl/pull/3147)
- feat(disp): add screen out animations [`3081`](https://github.com/lvgl/lvgl/pull/3081)
- feat(menu): make menu widget more compatible with encoder [`3061`](https://github.com/lvgl/lvgl/pull/3061)
- feat(label): added animation style property to apply it to circular scrolling animation of label widget [`3128`](https://github.com/lvgl/lvgl/pull/3128)
- feat(script): add pre-commit configuration for code formatting [`3092`](https://github.com/lvgl/lvgl/pull/3092)
- feat(refr): prevents dirty areas from being modified during rendering [`3107`](https://github.com/lvgl/lvgl/pull/3107)
- feat(log): improve lv_log and add log the result from lv_demo_benchmark [`3084`](https://github.com/lvgl/lvgl/pull/3084)
- feat(fragment): add fragment manager (a UI Controller concept) [`2940`](https://github.com/lvgl/lvgl/pull/2940)
- feat(porting): add a macro lv_run_timer_handler_in_period to simplify porting [`3063`](https://github.com/lvgl/lvgl/pull/3063)
- feat(gpu): reattach nxp pxp vglite accelerators(#3322) [`029eef7`](https://github.com/lvgl/lvgl/commit/029eef79c4cf6fef4ad46f7e335011ba4172381b)
- feat(draw): support transforming widgets and improfe sw transform [`318146a`](https://github.com/lvgl/lvgl/commit/318146a2c25362eabf258470be263a4cfeaefe87)
- feat(msg): add publisher-subscriber messaging [`79a29d7`](https://github.com/lvgl/lvgl/commit/79a29d749d3e261ebadbe31fccbff896f63b4d93)
- feat(benchmark): add an API to run specific scene (#3089) [`305ad00`](https://github.com/lvgl/lvgl/commit/305ad00893c0d18d9a65e28ee03d65f76f8abb0a)
- feat(gpu): add SWM341 gpu support (synwit) [`07b7eea`](https://github.com/lvgl/lvgl/commit/07b7eea56c048a0654c254cadebee8caf5f7933b)
- feat(arc): add lv_arc_align_obj_to_angle and lv_arc_rotate_obj_to_angle [`a76bb70`](https://github.com/lvgl/lvgl/commit/a76bb70a79dfa5b841328f07ede0907c700a039a)
- feat(draw): add draw_ctx-&gt;buffer_copy [`d034511`](https://github.com/lvgl/lvgl/commit/d034511bba3a27aa1a29d2e1b612b1adeb4e2ae1)
- feat(dropdown): add lv_dropdown_get_option_index [`9997fb0`](https://github.com/lvgl/lvgl/commit/9997fb00aa60b4478c76fa8387a74ca5b3c595b2)
- feat(tabview) add API to rename tab. [`2c9695a`](https://github.com/lvgl/lvgl/commit/2c9695afb4ed6597ae54806c5eb2a287925343f8)
- feat(indev): send LV_EVENT_PRESS_LOST on release with wait_until_release [`cc18518`](https://github.com/lvgl/lvgl/commit/cc18518e96df63c2a02ee9d423cb7bc23382e5a7)
- feat(style) add 'inherit' and 'initial' CSS properties (#3390) [`9a48de0`](https://github.com/lvgl/lvgl/commit/9a48de0f8b19ec02a44aaf6b330066eed7d0a105)
- feat(draw): improve acceleration for LV_IMG_CF_ALPHA_8BIT (#3337) [`8d3c41d`](https://github.com/lvgl/lvgl/commit/8d3c41d5170dad0455fea3d95b2765db70d3c7c2)
- feat(label): added animation style property to apply it to circular scrolling animation of label widget (#3128) [`340d45c`](https://github.com/lvgl/lvgl/commit/340d45cfa91b7108d43af906fc51b1c431877827)
- feat(gridnav): add lv_gridnav_set_focused [`b6d2daa`](https://github.com/lvgl/lvgl/commit/b6d2daa4935128ca8193863d4deaf58fa40b3154)

### Performance

- perf(draw): speed up non normal blend modes [`5a06fce`](https://github.com/lvgl/lvgl/commit/5a06fce472c103b4204002a7932dd6c6d05eb13c)
- perf(draw): minor optimiziation in point transformation [`c6c2864`](https://github.com/lvgl/lvgl/commit/c6c286404898bf559eca6eb5bb007251790c572c)
- perf(layer): cache the layer_type [`ac2e2f1`](https://github.com/lvgl/lvgl/commit/ac2e2f132e264d5f0f0313f4e6adbcf56d937a14)

### Fixes

- fix(draw): conflict with external ALIGN define [`3336`](https://github.com/lvgl/lvgl/pull/3336)
- fix(arc): fix bug with LV_ARC_MODE_REVERSE (#3417) [`3418`](https://github.com/lvgl/lvgl/pull/3418)
- fix(fragment): memory leak of fragments #3438 [`3442`](https://github.com/lvgl/lvgl/pull/3442)
- fix(draw): solve memory leaking issue [`3437`](https://github.com/lvgl/lvgl/pull/3437)
- fix(gridnav) correct logic in find_last_focusable [`3423`](https://github.com/lvgl/lvgl/pull/3423)
- fix(examples) correct comment in slider example [`3419`](https://github.com/lvgl/lvgl/pull/3419)
- fix(sdl): add transformation support for the SDL backend [`3403`](https://github.com/lvgl/lvgl/pull/3403)
- fix(bmp): fix with LV_COLOR_16_SWAP [`3412`](https://github.com/lvgl/lvgl/pull/3412)
- fix(sdl): fix LRU, reported in #3402 [`3404`](https://github.com/lvgl/lvgl/pull/3404)
- fix(draw) avoid use-after-free when drawing arcs [`3399`](https://github.com/lvgl/lvgl/pull/3399)
- fix(subpx): fix subpixel rendering font is not displaying bug [`3387`](https://github.com/lvgl/lvgl/pull/3387)
- fix(style): reset style lookup table after gc sweep/lv_deinit [`3385`](https://github.com/lvgl/lvgl/pull/3385)
- fix(benchmark): fix the issue that wrong scene number is used [`3372`](https://github.com/lvgl/lvgl/pull/3372)
- fix(draw): fix colour supports for indexed and alpha-only [`3371`](https://github.com/lvgl/lvgl/pull/3371)
- fix(mem): fix TLSF returning the wrong pointer when the requested size is too large [`3325`](https://github.com/lvgl/lvgl/pull/3325)
- fix(demo): fix warning. [`3344`](https://github.com/lvgl/lvgl/pull/3344)
- fix(config): add LV_GPU_SDL_LRU_SIZE [`3348`](https://github.com/lvgl/lvgl/pull/3348)
- feat(draw): improve acceleration for LV_IMG_CF_ALPHA_8BIT [`3337`](https://github.com/lvgl/lvgl/pull/3337)
- fix(txt): fix returned value of lv_txt_iso8859_1_next(..., NULL) [`3338`](https://github.com/lvgl/lvgl/pull/3338)
- fix(benchmark): remove redundant string for the small screens  [`3335`](https://github.com/lvgl/lvgl/pull/3335)
- fix(chart): fix accessing uninitialized point_area [`3327`](https://github.com/lvgl/lvgl/pull/3327)
- fix(config): add LV_LAYER_SIMPLE_BUF_SIZE to Kconfig [`3312`](https://github.com/lvgl/lvgl/pull/3312)
- fix(config): Keep the sequence of widget in order [`3314`](https://github.com/lvgl/lvgl/pull/3314)
- fix(config): fix typo in LV_USE_PERF_MONITOR and LV_USE_MEM_MONITOR [`3313`](https://github.com/lvgl/lvgl/pull/3313)
- fix(refr): initializing row_cnt is to silence the warning [`3309`](https://github.com/lvgl/lvgl/pull/3309)
- fix(meter): fix typo [`3308`](https://github.com/lvgl/lvgl/pull/3308)
- fix(draw): update Makefiles [`3303`](https://github.com/lvgl/lvgl/pull/3303)
- fix(lodepng): fix NULL pointer access [`3307`](https://github.com/lvgl/lvgl/pull/3307)
- fix(Kconfig): change the type of LV_FS_STDIO_LETTER from string to int [`3282`](https://github.com/lvgl/lvgl/pull/3282)
- fix(demo): fix Wformat warning [`3290`](https://github.com/lvgl/lvgl/pull/3290)
- fix(snapshot): add missing ASSERT checks [`3292`](https://github.com/lvgl/lvgl/pull/3292)
- fix(Kconfig): Add LV_USE_GRIDNAV and LV_USE_FRAGMENT to Kconfig [`3270`](https://github.com/lvgl/lvgl/pull/3270)
- fix(msgbox): do not execute init obj when obj == NULL [`3264`](https://github.com/lvgl/lvgl/pull/3264)
- fix(menu): use LV_ASSERT_MALLOC check for new_node [`3263`](https://github.com/lvgl/lvgl/pull/3263)
- fix(canvas):image cache may expire after set canvas's buff [`3267`](https://github.com/lvgl/lvgl/pull/3267)
- fix(obj_style): prevent access to class null pointer [`3252`](https://github.com/lvgl/lvgl/pull/3252)
- fix(png): fix possible memory leak when decoding fails [`3249`](https://github.com/lvgl/lvgl/pull/3249)
- fix(libs): fix possible buffer underflow caused by extension matching [`3250`](https://github.com/lvgl/lvgl/pull/3250)
- fix(fs): track multiple directory handles with win32 backends [`3243`](https://github.com/lvgl/lvgl/pull/3243)
- fix(png): use LV_IMG_CF_TRUE_COLOR_ALPHA instead of LV_IMG_CF_RAW_ALPHA [`3212`](https://github.com/lvgl/lvgl/pull/3212)
- fix(Keil-AC5): slience warnings in Keil-AC5 [`3221`](https://github.com/lvgl/lvgl/pull/3221)
- fix(meter): fix infinite loop caused by loop variable type mismatch [`3232`](https://github.com/lvgl/lvgl/pull/3232)
- fix(chart): remove invalid decision branches [`3231`](https://github.com/lvgl/lvgl/pull/3231)
- fix(gradient): assert before dividing by 0 [`3228`](https://github.com/lvgl/lvgl/pull/3228)
- fix(calendar): fix infinite loop caused by loop variable type mismatch [`3230`](https://github.com/lvgl/lvgl/pull/3230)
- fix(flex): assert before dividing by 0 [`3237`](https://github.com/lvgl/lvgl/pull/3237)
- fix(hal): fix LV_ASSERT_MALLOC wrong placement [`3236`](https://github.com/lvgl/lvgl/pull/3236)
- fix(disp): fix missing null pointer judgment [`3238`](https://github.com/lvgl/lvgl/pull/3238)
- fix(obj_class): fix possible memory leak when the default disp is NULL [`3235`](https://github.com/lvgl/lvgl/pull/3235)
- fix(draw_sw_letter): fix incorrect use of sizeof for a pointer [`3234`](https://github.com/lvgl/lvgl/pull/3234)
- fix(indev): fix null pointer access caused by typo [`3229`](https://github.com/lvgl/lvgl/pull/3229)
- fix(event): remove invalid decision branches [`3233`](https://github.com/lvgl/lvgl/pull/3233)
- fix(draw_mask): remove invalid decision branches [`3225`](https://github.com/lvgl/lvgl/pull/3225)
- fix(spinbox): remove invalid judgment [`3227`](https://github.com/lvgl/lvgl/pull/3227)
- fix(gradient): remove invalid decision branches [`3226`](https://github.com/lvgl/lvgl/pull/3226)
- fix(txt): return 0 if letter_uni is out of range [`3224`](https://github.com/lvgl/lvgl/pull/3224)
- fix(calendar): fix possible array access out of bounds [`3223`](https://github.com/lvgl/lvgl/pull/3223)
- fix(style): remove useless null pointer judgment [`3222`](https://github.com/lvgl/lvgl/pull/3222)
- fix(obj): scrolling exception when use lv_obj_set_parent() [`3210`](https://github.com/lvgl/lvgl/pull/3210)
- fix(libs): fix memcmp memory access overflow [`3205`](https://github.com/lvgl/lvgl/pull/3205)
- fix(png): fix possible file leaks [`3204`](https://github.com/lvgl/lvgl/pull/3204)
- fix(docs): rename task-handler.md to timer-handler.md [`3203`](https://github.com/lvgl/lvgl/pull/3203)
- fix(lru): Fix use of undefined variables [`3181`](https://github.com/lvgl/lvgl/pull/3181)
- fix(rt-thread): Sconscript use LOCAL_CFLAGS to replace LOCAL_CCFLAGS [`3196`](https://github.com/lvgl/lvgl/pull/3196)
- fix(make) make files under draw/gpu [`3202`](https://github.com/lvgl/lvgl/pull/3202)
- fix(docs-CN):fix broken links to docs in dir get-started [`3195`](https://github.com/lvgl/lvgl/pull/3195)
- fix broken links to docs in dir get-started [`3190`](https://github.com/lvgl/lvgl/pull/3190)
- fix(indev): fix warning about formatting uint32_t with %d [`3193`](https://github.com/lvgl/lvgl/pull/3193)
- fix(Kconfig): move LV_USE_IMGFONT to others menu [`3176`](https://github.com/lvgl/lvgl/pull/3176)
- fix(draw): src_buf_tmp will be NULL when LV_DRAW_COMPLEX is '0' [`3163`](https://github.com/lvgl/lvgl/pull/3163)
- fix(span): align the baselines [`3164`](https://github.com/lvgl/lvgl/pull/3164)
- fix(menu): fix crash on delete [`3154`](https://github.com/lvgl/lvgl/pull/3154)
- fix(Kconfig): add missing LV_USE_THEME_MONO [`3146`](https://github.com/lvgl/lvgl/pull/3146)
- fix(demo/stress): remove the unused assets [`3139`](https://github.com/lvgl/lvgl/pull/3139)
- fix(jpg): swap high and low bytes when macro LV_COLOR_16_SWAP is 1 [`3138`](https://github.com/lvgl/lvgl/pull/3138)
- fix(script): in lv_conf_internal fix some widget dependencies when using Kconfig [`3119`](https://github.com/lvgl/lvgl/pull/3119)
- fix(demo): minor fix for benchmark [`3114`](https://github.com/lvgl/lvgl/pull/3114)
- fix(misc): in lv_map() handle if maximum value less than minimum value [`3113`](https://github.com/lvgl/lvgl/pull/3113)
- fix(extra): adjust image decoder initialization order [`3085`](https://github.com/lvgl/lvgl/pull/3085)
- fix(chart): optimize chart invalidation [`3028`](https://github.com/lvgl/lvgl/pull/3028)
- fix(refr): fix performance monitor NULL pointer access [`3105`](https://github.com/lvgl/lvgl/pull/3105)
- fix(misc): Remove duplicate declaration of _lv_log_add. [`3103`](https://github.com/lvgl/lvgl/pull/3103)
- fix(gridnav): get key code from the actual event [`3101`](https://github.com/lvgl/lvgl/pull/3101)
- fix(draw_rect): delete __STDC_VERSION__ to ensure C++ compatibility [`3099`](https://github.com/lvgl/lvgl/pull/3099)
- fix(font):draw placeholder if get_glyph_dsc() returns false [`3000`](https://github.com/lvgl/lvgl/pull/3000)
- fix(conf): work around GCC bug [`3082`](https://github.com/lvgl/lvgl/pull/3082)
- fix(fsdrv): replacing sprintf with lv_snprintf for safety [`3079`](https://github.com/lvgl/lvgl/pull/3079)
- fix(cmsis-pack): add PIDX for cmsis-pack [`3064`](https://github.com/lvgl/lvgl/pull/3064)
- feat(gpu): add SWM341 gpu support (synwit) [`07b7eea`](https://github.com/lvgl/lvgl/commit/07b7eea56c048a0654c254cadebee8caf5f7933b)
- fix(fs): fix cached read and add unit test for lv_fs [`98660a8`](https://github.com/lvgl/lvgl/commit/98660a861d874d29e8356452baff21788b6a9ef1)
- fix(table): invalidate only the changed cell [`306fa19`](https://github.com/lvgl/lvgl/commit/306fa1968238fe33dd95e2865e147bceb4706ad5)
- fix(draw): handle non BLEND_MODE_NORMAL for ARGB drawing [`9ac8ce6`](https://github.com/lvgl/lvgl/commit/9ac8ce69f67234563d4254e29e1903a638bb8f4e)
- fix(draw): revert handling of style_opa on not MAIN parts [`51a7a61`](https://github.com/lvgl/lvgl/commit/51a7a61df365685a7cd04b0512ba3844dcfa7209)
- fix(draw): clip the bg img to the rectangle's area in lv_draw_sw_rect [`77d726e`](https://github.com/lvgl/lvgl/commit/77d726efb2467ff86691dee487f97aac79ea45c2)
- fix(obj): fix LV_OBJ_FLAG_OVERFLOW_VISIBLE [`c742f2c`](https://github.com/lvgl/lvgl/commit/c742f2c8888ad0102cebe91b4069b376068baa81)
- fix(scroll): do not fire scroll begin/end event on every scroll step [`25ce6e3`](https://github.com/lvgl/lvgl/commit/25ce6e3ae9e144e2df5dad34475dda3542015f6a)
- fix(indev): do not send keys to objects in disabled state [`b0a46c4`](https://github.com/lvgl/lvgl/commit/b0a46c4837c922cb1303ef768da3209e7efa45ae)
- fix(disp): make lv_scr_load work better with lv_scr_load_anim and auto_del = true [`52287fd`](https://github.com/lvgl/lvgl/commit/52287fd64ad59c35794d1f4486b777f4eb686abc)
- fix(draw): create intermediate layer for blend modes too [`8b15007`](https://github.com/lvgl/lvgl/commit/8b150075681455c6424ddd536e991307ac828eb4)
- fix(group): in lv_group_remove() fix if the object to focus is deleted [`72cb683`](https://github.com/lvgl/lvgl/commit/72cb683c799f65cd4fbae22dafc3a35c123bb66b)
- fix(draw): be sure angle values are in the correct range [`e624b90`](https://github.com/lvgl/lvgl/commit/e624b90db3515816eee8f6ce72677350487f3a02)
- fix(scroll): send LV_EVENT_SCROLL_BEGIN/END with no animation too [`777fe1e`](https://github.com/lvgl/lvgl/commit/777fe1ea706f38b82ab8ee180548ecb85334a469)
- fix(arc): fix arc image drawing issue [`7153e3f`](https://github.com/lvgl/lvgl/commit/7153e3f8b7b660474b8907954c80e21eb2f0bd21)
- fix(refr): fix memory write out of bounds issue [`13c99fc`](https://github.com/lvgl/lvgl/commit/13c99fc4b66d3e8d0ffcd6fda21d3b5a40b0771c)
- fix(gif): fix rare issue when drawing the gif's background [`b1e2c06`](https://github.com/lvgl/lvgl/commit/b1e2c0665829aa489f444169ce80fcd7cdf487bb)
- fix(chart): fix misaligned horizontal tick lines on bar charts [`4572a0c`](https://github.com/lvgl/lvgl/commit/4572a0c6c92b126e229ce9aada551d71b4f4296b)
- fix(font): use 0 width for non printable characters [`7cf5709`](https://github.com/lvgl/lvgl/commit/7cf5709b0669ab64e437a796c50f6bdb97b9d0d5)
- revert(group): 72cb683c799f65cd4fbae22dafc3a35c123bb66b [`b7b22c1`](https://github.com/lvgl/lvgl/commit/b7b22c190c6d9e11a841289708f55be0be86830f)
- fix(keyboard): don't show popovers on map change [`ac202e7`](https://github.com/lvgl/lvgl/commit/ac202e7b96510b9b12beb8a1eee3dfd65bc56a3d)
- fix(refr): consider masks with LV_OBJ_FLAG_OVERFLOW_VISIBLE [`a7f9dfa`](https://github.com/lvgl/lvgl/commit/a7f9dfa8c3e4fd56cc2db5c3f3926b9391d3661f)
- fix(draw): fix the calculation of the transformed coordinates [`76de7c6`](https://github.com/lvgl/lvgl/commit/76de7c6b7bce6da62f5e961ee477bfa324675683)
- fix(style): fix heap use after free with transition styles [`d9ae58b`](https://github.com/lvgl/lvgl/commit/d9ae58b6977ccfda90e02fa6f5b852d398f8600a)
- fix(tabview, tileview): fix scrolling [`22854ff`](https://github.com/lvgl/lvgl/commit/22854ff3fba236f50893221805c9cc4d378baaca)
- fix(draw): fix disp_bg_img drawing [`dea75d9`](https://github.com/lvgl/lvgl/commit/dea75d9b4a90601bf81bf69d533c4f13e62aa88c)
- fix(textarea): fix max length handling [`127d8e8`](https://github.com/lvgl/lvgl/commit/127d8e82e344cd8762672e787b1ee06390050b65)
- fix(btnmatrix): fix extra draw size calculation to not clip shadow [`7ada130`](https://github.com/lvgl/lvgl/commit/7ada1301c2ee113a5184618538b979f6d9912239)
- fix(indev): scroll_ throw_vect cannot converge to 0 when vect is negative [`e5c11f1`](https://github.com/lvgl/lvgl/commit/e5c11f1f68275d294d5b8892366aa424a5a14bca)
- fix(theme): make the basic theme even more simpler [`62d6f3c`](https://github.com/lvgl/lvgl/commit/62d6f3c533ca6d13fce3056074c1e44ffea355b1)
- fix(color): color mix rounding error [`523062b`](https://github.com/lvgl/lvgl/commit/523062b9ee8a106ad4b3b7bd0ee7baca743f2e5f)
- fix(style): _lv_style_prop_lookup_flags tell all flags for LV_STYLE_PROP_ANY [`e53f602`](https://github.com/lvgl/lvgl/commit/e53f60259c01ab1243b0cf56eb228b7f5eedc203)
- fix(list): use  for icon [`b171f7d`](https://github.com/lvgl/lvgl/commit/b171f7dde2a895142385ea1275f3f51255cb2811)
- fix(layout): fix the handling of FLOATING children [`48728a7`](https://github.com/lvgl/lvgl/commit/48728a7839d6859d7d6fc4f86f5fbcbcd9939348)
- fix(style): make color filter inherited [`5546b9d`](https://github.com/lvgl/lvgl/commit/5546b9d740de8d774071328251413ec29c12d288)
- fix(spinbox): set its default width in its class [`3d92972`](https://github.com/lvgl/lvgl/commit/3d9297269598ca40e2f8dd2d8f31150d41e94cb8)
- fix: fix warning [`6c00552`](https://github.com/lvgl/lvgl/commit/6c005526295aeb277edad42b3a05b0c7e6d72eaf)
- fix(draw): fix transformations on subdivided areas [`cbff8e8`](https://github.com/lvgl/lvgl/commit/cbff8e83e50fecc2b4b43d661deb91d8d81d6696)
- fix(slider): fix left knob in ranged mode [`17f5e0a`](https://github.com/lvgl/lvgl/commit/17f5e0accb15871040a6225a9c0471ceadd6dc16)
- fix(Kconfig): allow unchecking LV_CONF_SKIP [`f3a07a3`](https://github.com/lvgl/lvgl/commit/f3a07a3e8a21f3f9f2c48a2803b8bd991968cb05)
- fix(style): fix using width for both width and height in radius transition [`6acbdaa`](https://github.com/lvgl/lvgl/commit/6acbdaa53d941b891db377e65111bd999f04631d)
- fix(dropdown): fix scrolling when options are CENTER aligned [`e651383`](https://github.com/lvgl/lvgl/commit/e651383688dd29ab2e990cd997118435832d959c)
- fix(grid): fix dead branch [`46bf27d`](https://github.com/lvgl/lvgl/commit/46bf27d50bb668bdd1f84489cb70986ee0ef9fab)
- fix(hal): disable driver-&gt;screen_transp by default regardless to LV_COLOR_SCREEN_TRANSP [`ff7204e`](https://github.com/lvgl/lvgl/commit/ff7204ecadd10132b436b11c8948b9a882b58798)
- fix(theme): fix mono theme init [`5ec6694`](https://github.com/lvgl/lvgl/commit/5ec6694f7874f3c99a764e7ee2d45a933865c91c)
- fix(bmp) fix typo in BPP condition [`cbc38af`](https://github.com/lvgl/lvgl/commit/cbc38afb3a0d3ca02159ab89242749809e64df0c)
- fix(theme): in the basic theme show the textarea cursor only in focuses state [`bb03fb1`](https://github.com/lvgl/lvgl/commit/bb03fb197c7083680fd7dc730794a52561cabfd4)
- fix(draw): fix img recolor [`23eecce`](https://github.com/lvgl/lvgl/commit/23eecce008dacd8e5f5d56d017e4e5705f0c31e6)
- fix(theme) add disabled style to textarea in the default theme [`00f6759`](https://github.com/lvgl/lvgl/commit/00f67597d3c87ff811e5e682c10ef20227218651)
- fix(meter): improve the precision of tick line drawing [`0255c6d`](https://github.com/lvgl/lvgl/commit/0255c6dd39640d7ec639cbd339a0fbdcdfb2bb82)
- fix(gpu): fix warning with NXP GPU [`6be43b8`](https://github.com/lvgl/lvgl/commit/6be43b83b3dc9340263552167dbbb07c1069bdb0)
- fix(color): compensate rounding error during blending [`42d9c07`](https://github.com/lvgl/lvgl/commit/42d9c07eeb0abfdbf8746da3569a5f8bc156ae71)
- fix(examples) use type-safe function for retrieving event param [`71d535d`](https://github.com/lvgl/lvgl/commit/71d535defd730fc20ed8d57faa2550781be4f3d7)
- fix(draw) ensure variable is initialized to avoid warning [`276f28a`](https://github.com/lvgl/lvgl/commit/276f28a8a2f4ac2f6268a4363879faa6296e14ad)
- feat(draw): improve acceleration for LV_IMG_CF_ALPHA_8BIT (#3337) [`8d3c41d`](https://github.com/lvgl/lvgl/commit/8d3c41d5170dad0455fea3d95b2765db70d3c7c2)
- fix(spinbox): rename lv_spinbox_set_pos to lv_spinbox_set_cursor_pos [`a99eb6b`](https://github.com/lvgl/lvgl/commit/a99eb6bb6ae12f3fcb86f5268a0c000fb165e159)
- fix(layout): use uint16_t LV_LAYOUT_FLEX/GRID [`c596a36`](https://github.com/lvgl/lvgl/commit/c596a36d9ecf92ae5ce1ecc812210bf3a7df4999)
- fix(event) avoid using a boolean as a pointer [`06fff4b`](https://github.com/lvgl/lvgl/commit/06fff4b9bac35d63564de87fa63f7bedd8a0f9f2)
- fix(theme): properly disable transitions if LV_THEME_DEFAULT_TRANSITION_TIME==0 [`242112b`](https://github.com/lvgl/lvgl/commit/242112b2df8b6cc12aa9920cc3b2fdc9a11d807f)
- fix(scroll): fix scroll to view to the left [`7c74f65`](https://github.com/lvgl/lvgl/commit/7c74f6556abbc299a79b1490c06151a43c902f61)
- fix(fs): mark the read cache as invalid by default [`54f9987`](https://github.com/lvgl/lvgl/commit/54f99870b3cac619fb7057618637d7ee19d58bb3)
- fix(menu): fix crash on delete (#3154) [`a6c4c13`](https://github.com/lvgl/lvgl/commit/a6c4c134902f9a4c156672a70108e809b58fa18c)
- fix(roller): fix unexpected jump in infinite mode [`18f2d78`](https://github.com/lvgl/lvgl/commit/18f2d78728c758179e4ef01ebc632da4e1263be7)
- fix(conf): work around GCC bug (#3082) [`c6b34bc`](https://github.com/lvgl/lvgl/commit/c6b34bc85bb6f5e57e1c87857e03d1a0bd225e4c)

### Examples
- example(ime_pinyin): improved lv_example_ime_pinyin_1 [`3428`](https://github.com/lvgl/lvgl/pull/3428)
- example(imgfont): fix lvgl.h include path [`3405`](https://github.com/lvgl/lvgl/pull/3405)
- example(btnmatrix): update lv_example_btnmatrix_2 to expicitly check which part is drawn [`6b2eac1`](https://github.com/lvgl/lvgl/commit/6b2eac1dd70df62916b46cee8d4b981ff088b1a7)
- example(slider): make lv_example_slider_3 work with dark theme too [`4a766c5`](https://github.com/lvgl/lvgl/commit/4a766c516db7c2572a075ec5ffe748d30af8c7b9)
- example(span): avoid ambiguous meaing [`7bb09e3`](https://github.com/lvgl/lvgl/commit/7bb09e358026aff3d55d881237624baac77db890)
- demo(benchmark): add LV_DEMO_BENCHMARK_RGB565A8 option [`afaa8c9`](https://github.com/lvgl/lvgl/commit/afaa8c93006a88db9f115b2b318eef790928d2a6)

### Docs

- docs(indev): add comment in input device part [`3422`](https://github.com/lvgl/lvgl/pull/3422)
- docs(slider) mention that VALUE_CHANGED is not sent on release [`3397`](https://github.com/lvgl/lvgl/pull/3397)
- docs(readme): add version portuguese brazilian [`3349`](https://github.com/lvgl/lvgl/pull/3349)
- docs(pc-simulator): add MDK with FastModel [`3318`](https://github.com/lvgl/lvgl/pull/3318)
- docs(intro): update for v8.2.0 [`3316`](https://github.com/lvgl/lvgl/pull/3316)
- docs(readme) update link to the PlatformIO Registry [`3296`](https://github.com/lvgl/lvgl/pull/3296)
- docs(gesture): fix typo lv_indev_act() -&gt; lv_indev_get_act() [`3291`](https://github.com/lvgl/lvgl/pull/3291)
- docs(scroll) add information about scroll coordinates [`3088`](https://github.com/lvgl/lvgl/pull/3088)
- docs(msgbox) fix typo [`3095`](https://github.com/lvgl/lvgl/pull/3095)
- docs(scroll): use LV_DIR_VER instead of LV_DIR_TOP [`3066`](https://github.com/lvgl/lvgl/pull/3066)
- docs: rearrange the get-started section [`8a81532`](https://github.com/lvgl/lvgl/commit/8a8153219163b689e8f96d6a97c1f128eefd7ce2)
- docs: add section for renderers and gpus [`378aaa6`](https://github.com/lvgl/lvgl/commit/378aaa637bdcaef8f06667ab9d56c914e0a61beb)
- docs collapse APIs by default [`ebd20af`](https://github.com/lvgl/lvgl/commit/ebd20af6e9cbd68230f49b6c85d940569a7db81c)
- docs(images): fix notes about breaking change inf v8.2 [`9a1e385`](https://github.com/lvgl/lvgl/commit/9a1e385b2b3498ed70704bf0ed33e4bd263747d8)
- docs(sim): add link to qt-creator [`88bbef1`](https://github.com/lvgl/lvgl/commit/88bbef14bf69725a1ab62bffa6ab79355ea31c2d)
- docs(chart): describe how to set the space between columns [`746917d`](https://github.com/lvgl/lvgl/commit/746917dcca74c53f6b2dc3849c9d588a0bf91b60)
- docs(README): fix broken link [`c2c44c6`](https://github.com/lvgl/lvgl/commit/c2c44c68ee69cdee16fce7833cbf6d6dc0d551ab)
- docs(examples) avoid redirects when loading examples [`d367bb7`](https://github.com/lvgl/lvgl/commit/d367bb7cf17dc34863f4439bba9b66a820088951)
- docs(gesture): describe how prevent sending events after a gesture [`65db5c9`](https://github.com/lvgl/lvgl/commit/65db5c99e05f86d2ec69ebae9f1fc50fe30a3145)
- docs(get-started): add quick-overview to the index [`91ebf81`](https://github.com/lvgl/lvgl/commit/91ebf810aacfe972f0ae140a1a61031eea9cda0c)
- docs(others): add imgfont to the index [`656a0e5`](https://github.com/lvgl/lvgl/commit/656a0e5167dca8c6c29497130e374080397fa45f)

### CI and tests

- ci(slider): add unit test [`3198`](https://github.com/lvgl/lvgl/pull/3198)
- test(line): add unit tests for line widget [`3104`](https://github.com/lvgl/lvgl/pull/3104)
- test(table): replicate issue when reducing table cells [`3121`](https://github.com/lvgl/lvgl/pull/3121)
- test(textarea): add unit test [`3074`](https://github.com/lvgl/lvgl/pull/3074)
- test(table): add unit tests [`3040`](https://github.com/lvgl/lvgl/pull/3040)
- ci(docs) replace use of sed with proper configuration variables [`1816fa5`](https://github.com/lvgl/lvgl/commit/1816fa576cc40ef1795e95ed127d93df5390b0cf)
- ci add Makefile test [`ea79cee`](https://github.com/lvgl/lvgl/commit/ea79cee01a6bec9b3ce5b6c232dd7ca0d020d5c9)
- test(mem) add test for #3324 [`9700664`](https://github.com/lvgl/lvgl/commit/97006647d8ed3af65fd2113ddf01c7882a4dba19)
- test(img): fix image error diff handler [`48d87e1`](https://github.com/lvgl/lvgl/commit/48d87e1ed2d362e9c3bd84eb60c311ad6519ae85)
- ci update docs builder to work with Python 3.10 [`a3d66c9`](https://github.com/lvgl/lvgl/commit/a3d66c9b67d226f8ab4555616ecf2ea62e307962)
- ci make sure LVGL assertions cause tests to fail [`b83c5aa`](https://github.com/lvgl/lvgl/commit/b83c5aa9bc4a278a6758f76e77ac9c403e483948)
- ci remove formatting comment [`d345f76`](https://github.com/lvgl/lvgl/commit/d345f76d02a23d94550b1b60be90585f6f5276b7)
- ci don't run workflows twice on PRs [`fcc1152`](https://github.com/lvgl/lvgl/commit/fcc1152f9c14494f128f26a6b47b00864a70c741)
- ci bump test timeout to 30 seconds [skip ci] [`85e3e23`](https://github.com/lvgl/lvgl/commit/85e3e2387845bd29c9f85b406623e41d36b66808)
- ci limit tests to 15 seconds [`003f18f`](https://github.com/lvgl/lvgl/commit/003f18f86c5c728920575cf1d34dd0f811607a51)
- ci(makefile) fix typo in GitHub action [`a101e70`](https://github.com/lvgl/lvgl/commit/a101e70ebd4120549236abd637049678dd6800e7)
- ci(switch): fix mem leak test [`8481e3a`](https://github.com/lvgl/lvgl/commit/8481e3a33bc3313b679babac31e6193ec4319bcd)
- ci(stale) bump action version [`5977eef`](https://github.com/lvgl/lvgl/commit/5977eeff3c559c0473d5abd8a99687eeb4659c61)
- ci use GCC problem matcher on ARM tests as well [`9fcefe5`](https://github.com/lvgl/lvgl/commit/9fcefe5a49a024054a3cee08d273b8fe5cf8840e)


## [v8.2.0](https://github.com/littlevgl/lvgl/compare/v8.1.0...v8.2.0) 31 January 2022


### Overview

Among many fixes and minor updates these are the most important features in v8.2.0:
- Abstract render layer to make it easier to attach external draw engines
- Add `LV_FLAD_OVERFLOW_VISIBLE`. If enabled the children of an object won't be clipped to the boundary of the object
- Add ffmpeg decoder support to play videos and open a wide variety of image formats
- Add font fallback support
- Add gradient dithering support
- Add "monkey test"
- Add cmsis-pack support
- Add Grid navigation (`lv_gridnav`)

The GPU support for NXP microcontrollers is still not updated to the new draw architecture. See [#3052](https://github.com/lvgl/lvgl/issues/3052)

### Breaking Changes

- :warning: feat(fs): add caching option for lv_fs-read [`2979`](https://github.com/littlevgl/lvgl/pull/2979)
- :warning: feat(span): lv_spangroup_get_expand_width() adds a parameter [`2968`](https://github.com/littlevgl/lvgl/pull/2968)
- :warning: arch(draw): allow replacing the draw engine [`db53ea9`](https://github.com/littlevgl/lvgl/commit/db53ea925c9502b20f38db0fc30c4ef599bdfc33)
- :warning: indexed images are not chroma keyed. Use the alpha chaneel instead.

### Architectural

- arch(draw): separate SW renderer to allow replacing it [`2803`](https://github.com/littlevgl/lvgl/pull/2803)
- arch: merge lv_demos [`5414652`](https://github.com/littlevgl/lvgl/commit/5414652a4108dc6761b859fbb48a43e37e67a37a)
- arch(sdl): migrated to use new backend architecture [`2840`](https://github.com/littlevgl/lvgl/pull/2840)
- arch(env): move rt-thread into env_support folder [`3025`](https://github.com/littlevgl/lvgl/pull/3025)
- arch(env): arch(env): move the cmake folder into the env_support folder [`773d50f`](https://github.com/littlevgl/lvgl/commit/773d50f0acafa279fa7440ddcf15e80cf07eda54)
- arch(env): move the zephyr folder into the env_support folder [`4bd1e7e`](https://github.com/littlevgl/lvgl/commit/4bd1e7e9f7acc5295b65440477e76a048094afbf)

### New Features

- feat(cmsis-pack): prepare for lvgl v8.2.0 release [`3062`](https://github.com/littlevgl/lvgl/pull/3062)
- feat(gridnav): add lv_gridnav [`2911`](https://github.com/littlevgl/lvgl/pull/2911)
- feat: update the cmsis-pack to 0.8.3 [`3021`](https://github.com/littlevgl/lvgl/pull/3021)
- feat(sdl): support rounded images [`3012`](https://github.com/littlevgl/lvgl/pull/3012)
- feat(cmsis-pack): add cmsis-pack support [`2993`](https://github.com/littlevgl/lvgl/pull/2993)
- feat(event): add preprocessing and stop bubbling features for events [`3003`](https://github.com/littlevgl/lvgl/pull/3003)
- feat(draw): add gradient dithering support [`2872`](https://github.com/littlevgl/lvgl/pull/2872)
- feat(symbols): add guards to LV_SYMBOL_* to allow redefining them [`2973`](https://github.com/littlevgl/lvgl/pull/2973)
- feat(obj): subdivide LV_OBJ_FLAG_SCROLL_CHAIN into ...CHAIN_HOR and ...CHAIN_VER [`2961`](https://github.com/littlevgl/lvgl/pull/2961)
- feat(draw): add draw_bg callback to draw_ctx #2934 [`2935`](https://github.com/littlevgl/lvgl/pull/2935)
- feat(docs): add Chinese readme [`2919`](https://github.com/littlevgl/lvgl/pull/2919)
- feat(txt): add used_width parameter to _lv_txt_get_next_line()  [`2898`](https://github.com/littlevgl/lvgl/pull/2898)
- feat(others) add monkey test [`2885`](https://github.com/littlevgl/lvgl/pull/2885)
- feat(rlottie): add animation control options [`2857`](https://github.com/littlevgl/lvgl/pull/2857)
- feat(lv_hal_indev): add missing lv_indev_delete() [`2854`](https://github.com/littlevgl/lvgl/pull/2854)
- feat(freetype): optimize memory allocation [`2849`](https://github.com/littlevgl/lvgl/pull/2849)
- feat(Kconfig): add FreeType config [`2846`](https://github.com/littlevgl/lvgl/pull/2846)
- feat(widgets): add menu widget [`2603`](https://github.com/littlevgl/lvgl/pull/2603)
- feat(refr): add reset function for FPS statistics [`2832`](https://github.com/littlevgl/lvgl/pull/2832)
- feat(Kconfig): add monitor position configuration [`2834`](https://github.com/littlevgl/lvgl/pull/2834)
- feat(examples) add micropython versions of the external library examples [`2762`](https://github.com/littlevgl/lvgl/pull/2762)
- feat(freetype): support bold and italic [`2824`](https://github.com/littlevgl/lvgl/pull/2824)
- feat(font) add fallback support and mem. font load option to FreeType [`2796`](https://github.com/littlevgl/lvgl/pull/2796)
- feat(lib) add ffmpeg video and image decoder [`2805`](https://github.com/littlevgl/lvgl/pull/2805)
- feat(obj): add LV_OBJ_FLAG_OVERFLOW_VISIBLE [`e7ac0e4`](https://github.com/littlevgl/lvgl/commit/e7ac0e41988e5fda772e17292c05d65bcaf58394)
- feat(scrollbar): add more control over scrollbar paddings [`4197b2f`](https://github.com/littlevgl/lvgl/commit/4197b2fd6ebec4b4dcfeeb2c41b724e09b77d1d0)
- feat(dropdown): keep the list on open/close for simpler styling [`9d3134b`](https://github.com/littlevgl/lvgl/commit/9d3134b66e40882c232afa79498c41294603f437)
- feat(qrcode) use destructor instead of lv_qrcode_delete() [`318edd8`](https://github.com/littlevgl/lvgl/commit/318edd8a3f61a65be3ed15a97c0870de0ad4125a)
- feat(disp) allow decoupling the disp_refr timer [`85cc84a`](https://github.com/littlevgl/lvgl/commit/85cc84ad947786bb3d4857290503047946a55c43)
- feat(obj): add lv_obj_get_event_user_data() [`53ececc`](https://github.com/littlevgl/lvgl/commit/53ececc5ec6f62ee4ab47ea66a847679e3836f52)
- feat(obj) add LV_OBJ_FLAG_SCROLL_WITH_ARROW [`70327bd`](https://github.com/littlevgl/lvgl/commit/70327bdb2d758336340c5a3b378ab876bfee2d53)
- feat(slider): consider ext_click_area on the knob with LV_OBJ_FLAG_ADV_HITTEST [`9d3fb41`](https://github.com/littlevgl/lvgl/commit/9d3fb418969c13b93f01a6b0342a1cd8d02e9b6c)

### Performance

- perf(sdl): optimize the use of SDL_RenderSetClipRect [`2941`](https://github.com/littlevgl/lvgl/pull/2941)
- perf(color): add faster lv_color_hex function [`2864`](https://github.com/littlevgl/lvgl/pull/2864)

### Fixes

- fix(micropython) update examples for new API [`3059`](https://github.com/littlevgl/lvgl/pull/3059)
- fix: increase default value of LV_MEM_SIZE for lv_demo_widgets #3057 [`3058`](https://github.com/littlevgl/lvgl/pull/3058)
- fix(cmsis-pack): fix issue #3032 [`3056`](https://github.com/littlevgl/lvgl/pull/3056)
- fix(porting): add missing function prototypes [`3054`](https://github.com/littlevgl/lvgl/pull/3054)
- fix(kconfig): add missing default values [`3050`](https://github.com/littlevgl/lvgl/pull/3050)
- fix(canvas): force canvas to use sw draw [`3045`](https://github.com/littlevgl/lvgl/pull/3045)
- fix(rt-thread): use ARCH_CPU_BIG_ENDIAN to replace RT_USING_BIG_ENDIAN [`3044`](https://github.com/littlevgl/lvgl/pull/3044)
- fix(gradient): general cleanup and fix for alignment issues [`3036`](https://github.com/littlevgl/lvgl/pull/3036)
- fix(draw): rendering issues for vertical gradient with and without dithering [`3034`](https://github.com/littlevgl/lvgl/pull/3034)
- fix uninitialized variable [`3033`](https://github.com/littlevgl/lvgl/pull/3033)
- fix(lru): lower dependency for standard C functions [`3024`](https://github.com/littlevgl/lvgl/pull/3024)
- fix(env_support): move cmsis-pack to env_support folder [`3026`](https://github.com/littlevgl/lvgl/pull/3026)
- fix(doc): full covering opacity is 255, not 256 [`3022`](https://github.com/littlevgl/lvgl/pull/3022)
- fix uninitialized variables [`3023`](https://github.com/littlevgl/lvgl/pull/3023)
- fix various issues for esp32 [`3007`](https://github.com/littlevgl/lvgl/pull/3007)
- fix(sdl): fix clipped image drawing [`2992`](https://github.com/littlevgl/lvgl/pull/2992)
- fix(draw): missed bg_color renaming in the draw function [`3002`](https://github.com/littlevgl/lvgl/pull/3002)
- fix(porting): fix typo and an unmatched prototype [`2998`](https://github.com/littlevgl/lvgl/pull/2998)
- fix(conf) add missing LV_LOG_LEVEL default definition [`2996`](https://github.com/littlevgl/lvgl/pull/2996)
- fix(refr): crash if full_refresh = 1 [`2999`](https://github.com/littlevgl/lvgl/pull/2999)
- fix(Kconfig): adapt to lvgl's built-in demos [`2989`](https://github.com/littlevgl/lvgl/pull/2989)
- fix(Makefile): compilation errors [`2944`](https://github.com/littlevgl/lvgl/pull/2944)
- fix(rlottie): fix variable name [`2971`](https://github.com/littlevgl/lvgl/pull/2971)
- fix(group): in lv_group_del() remove group from indev (lvgl#2963) [`2964`](https://github.com/littlevgl/lvgl/pull/2964)
- fix(obj): old parent's scroll is not updated in lv_obj_set_parent() [`2965`](https://github.com/littlevgl/lvgl/pull/2965)
- fix(fatfs) add missing cast [`2969`](https://github.com/littlevgl/lvgl/pull/2969)
- fix(snapshot) fix memory leak [`2970`](https://github.com/littlevgl/lvgl/pull/2970)
- fix(examples) move event callback registration outside loop in `lv_example_event_3` [`2959`](https://github.com/littlevgl/lvgl/pull/2959)
- fix(canvas): off by one error in size check in lv_canvas_copy_buf [`2950`](https://github.com/littlevgl/lvgl/pull/2950)
- fix(indev) add braces to avoid compiler warning [`2947`](https://github.com/littlevgl/lvgl/pull/2947)
- fix: fix parameter order in function prototypes [`2929`](https://github.com/littlevgl/lvgl/pull/2929)
- fix(style):add const qualifier for lv_style_get_prop() [`2933`](https://github.com/littlevgl/lvgl/pull/2933)
- fix(dropdown): in lv_dropdown_get_selected_str handle if there are no options [`2925`](https://github.com/littlevgl/lvgl/pull/2925)
- fix: lv_deinit/lv_init crash or hang [`2910`](https://github.com/littlevgl/lvgl/pull/2910)
- fix(rt-thread): improve the structure [`2912`](https://github.com/littlevgl/lvgl/pull/2912)
- fix: removed string format warnings for int32_t and uint32_t [`2924`](https://github.com/littlevgl/lvgl/pull/2924)
- fix(lv_fs_win32): add missing include of &lt;stdio.h&gt; [`2918`](https://github.com/littlevgl/lvgl/pull/2918)
- fix: use unsigned integer literal for bit shifing. [`2888`](https://github.com/littlevgl/lvgl/pull/2888)
- chore(lottie) move rlottie_capi.h to lv_rlottie.c [`2902`](https://github.com/littlevgl/lvgl/pull/2902)
- fix(qrcodegen) add brackets around assert calls [`2897`](https://github.com/littlevgl/lvgl/pull/2897)
- fix(list) guard image creation with LV_USE_IMG [`2881`](https://github.com/littlevgl/lvgl/pull/2881)
- fix(snapshot): make fake display size big enough to avoid align issue. [`2883`](https://github.com/littlevgl/lvgl/pull/2883)
- fix(sdl) correct makefile [`2884`](https://github.com/littlevgl/lvgl/pull/2884)
- fix(draw): fix set_px_cb memory write overflow crash. [`2882`](https://github.com/littlevgl/lvgl/pull/2882)
- fix(freetype): fix memset error [`2877`](https://github.com/littlevgl/lvgl/pull/2877)
- fix(span): fix align and break word  [`2861`](https://github.com/littlevgl/lvgl/pull/2861)
- fix(refr): swap buffers only on the last area with direct mode [`2867`](https://github.com/littlevgl/lvgl/pull/2867)
- fix(arc) free memory when drawing full-circle arc [`2869`](https://github.com/littlevgl/lvgl/pull/2869)
- fix(indev): update lv_indev_drv_update to free the read_timer [`2850`](https://github.com/littlevgl/lvgl/pull/2850)
- fix(draw): fix memory access out of bounds when using blend subtract [`2860`](https://github.com/littlevgl/lvgl/pull/2860)
- fix(chart) add lv_chart_refresh() to the functions which modify the data [`2841`](https://github.com/littlevgl/lvgl/pull/2841)
- fix(conf) mismatched macro judgment [`2843`](https://github.com/littlevgl/lvgl/pull/2843)
- fix(ffmpeg): when disabled LV_FFMPEG_AV_DUMP_FORMAT makes av_log quiet [`2838`](https://github.com/littlevgl/lvgl/pull/2838)
- fix(rt-thread): fix a bug of log [`2811`](https://github.com/littlevgl/lvgl/pull/2811)
- fix(log): to allow printf and custom_print_cb to work at same time [`2837`](https://github.com/littlevgl/lvgl/pull/2837)
- fix(keyboard): add missing functions [`2835`](https://github.com/littlevgl/lvgl/pull/2835)
- fix(checkbox) remove unnecessary events [`2829`](https://github.com/littlevgl/lvgl/pull/2829)
- fix(qrcode): replace memcpy() with lv_memcpy() and delete useless macros [`2827`](https://github.com/littlevgl/lvgl/pull/2827)
- fix(font) improve builtin font source files generation process [`2825`](https://github.com/littlevgl/lvgl/pull/2825)
- fix(CMake) split CMakeLists.txt, add options, includes and dependencies [`2753`](https://github.com/littlevgl/lvgl/pull/2753)
- fix(obj): make lv_obj_fade_in/out use the current opa as start value [`2819`](https://github.com/littlevgl/lvgl/pull/2819)
- fix(qrcode):minimize margins as much as possible [`2804`](https://github.com/littlevgl/lvgl/pull/2804)
- fix(scripts): switch all scripts to python3 [`2820`](https://github.com/littlevgl/lvgl/pull/2820)
- fix(event): event_send_core crash in special case. [`2807`](https://github.com/littlevgl/lvgl/pull/2807)
- fix(Kconfig) remove duplicate LV_BUILD_EXAMPLES configuration [`2813`](https://github.com/littlevgl/lvgl/pull/2813)
- fix(obj): in obj event use the current target instead of target [`2785`](https://github.com/littlevgl/lvgl/pull/2785)
- fix(draw_label): radius Mask doesn't work in Specific condition [`2784`](https://github.com/littlevgl/lvgl/pull/2784)
- fix(draw_mask): will crash if get_width/height &lt; 0 [`2793`](https://github.com/littlevgl/lvgl/pull/2793)
- fix(theme) make the basic theme really basic [`a369f18`](https://github.com/littlevgl/lvgl/commit/a369f18c57c6b9d20a37959d621f9cb16348ef99)
- fix(arc): fix knob invalidation [`345f688`](https://github.com/littlevgl/lvgl/commit/345f6882c9802dd9be55dfda5fe50c17e8c002b0)
- fix(theme): add arc, spinner and colorwheel to basic theme [`adc218a`](https://github.com/littlevgl/lvgl/commit/adc218a7b303c564da021714e5a109a5d003fc30)
- fix(conf) define LV_LOG_TRACE_... to 0 in lv_conf_internal.h to avoid warnings [`305284c`](https://github.com/littlevgl/lvgl/commit/305284c2b5aadec7bcfa68c6517c98d44be7c8a9)
- fix(draw): consider opa and clip corner on bg_img [`d51aea4`](https://github.com/littlevgl/lvgl/commit/d51aea4dffc706876ac729373c33a74743bc05e9)
- fix(draw): add grad_cache_mem to GC_ROOTs [`138db9c`](https://github.com/littlevgl/lvgl/commit/138db9c5d6b1f1d42c48d1307f5f508149ab0fda)
- fix(bar, slider): fix shadow drawing on short indicators [`364ca3c`](https://github.com/littlevgl/lvgl/commit/364ca3ca1763fb732a049bfce689e2f588593cd4)
- fix(theme): fix theme initialization issue introduced in 6e0072479 [`d231644`](https://github.com/littlevgl/lvgl/commit/d2316447c5c240960236d41814ef20e63cd56f00)
- fix(draw): add lv_draw_sw_bg [`49642d3`](https://github.com/littlevgl/lvgl/commit/49642d3891c563b6c82bb407bacc4b73329a8c93)
- fix(draw) border_draw crash is special case [`075831a`](https://github.com/littlevgl/lvgl/commit/075831a54c30d294879619c90ca4d16676c0775a)
- fix(theme): fix crash in lv_theme_basic_init [`ca5f04c`](https://github.com/littlevgl/lvgl/commit/ca5f04cfe33e1db0b72a07812557634b86028c27)
- fix(draw): fix indexed image drawing [`5a0dbcc`](https://github.com/littlevgl/lvgl/commit/5a0dbccf890b7a86315140dfe052da6b6aeca531)
- fix(roller): clip overflowing text [`5709528`](https://github.com/littlevgl/lvgl/commit/5709528550f7bdb0a16da1c05ea8094fc085db08)
- fix(align) fix LV_SIZE_CONTENT size calculation with not LEFT or TOP alignment [`9c67642`](https://github.com/littlevgl/lvgl/commit/9c676421ff159de1a96409f5557d36090c1728f9)
- fix(draw): futher bg_img draw fixes [`81bfb76`](https://github.com/littlevgl/lvgl/commit/81bfb765e5baba359e61dcb030f3ee96160a6335)
- fix(btnmatrix): keep the selected button even on release [`d47cd1d`](https://github.com/littlevgl/lvgl/commit/d47cd1d7fe910efc189e2f43f046a09184cfff13)
- fix(sw): make knob size calculation more intuitive [`5ec532d`](https://github.com/littlevgl/lvgl/commit/5ec532dfd5ffa0d47a1ac80c9a468d6362f3d933)
- fix(switch): make knob height calculation similar to slider [`0921dfc`](https://github.com/littlevgl/lvgl/commit/0921dfc8cd9d00e70ead8cbef8a898711af8f43e)
- fix(span): explicitly set span-&gt;txt to the return value of lv_mem_realloc(#3005) [`a9a6cb8`](https://github.com/littlevgl/lvgl/commit/a9a6cb8efd16c55a175791a43a3f4043a3a5e01f)
- fix(example): update LVGL_Arduino.ino [`d79283c`](https://github.com/littlevgl/lvgl/commit/d79283c145f92124c800453bcaf1caf1f9684bc5)
- fix(draw) simplify how outline_pad is compnesated [`81d8be1`](https://github.com/littlevgl/lvgl/commit/81d8be13d67d6b17b663bc703c1e0e18a18890a7)
- fix(obj) make LV_OBJ_FLAG_SCROLL_CHAIN part of the enum instead of define [`f8d8856`](https://github.com/littlevgl/lvgl/commit/f8d88567f635f325d6738ce2343f3b3c29f1e40a)
- fix(label): dot not add dots if the label height &gt; 1 font line height [`4d61f38`](https://github.com/littlevgl/lvgl/commit/4d61f3802013b31b0af5f08f66bb86f5179db141)
- fix(event): crash if an object was deleted in an event [`9810920`](https://github.com/littlevgl/lvgl/commit/9810920fc5d34a984bddf6e41156e87e509cfd27)
- fix(build) fix sdl build with make [`43729d1`](https://github.com/littlevgl/lvgl/commit/43729d1502dad0ca797b4b6fb8c69a48c81a2af7)
- fix(config): fix anonymous choice [`71c739c`](https://github.com/littlevgl/lvgl/commit/71c739cc2dbcebf16e8adc805dda182011e725da)
- chore(docs): fix lv_list_add_text [`a5fbf22`](https://github.com/littlevgl/lvgl/commit/a5fbf22d415a52cb2641c6dfda6937a10e4952cc)
- fix(png) check png magic number to be sure it's a png image [`1092550`](https://github.com/littlevgl/lvgl/commit/1092550775c464f9ae8c406786fe02115776d5c6)
- fix(btnmatrix): fix crash if an empty btnmatrix is pressed [`2392f58`](https://github.com/littlevgl/lvgl/commit/2392f585bb9317153f6fb648d2a660cbdc3e276f)
- fix(mem/perf monitor): fix issue introduced in #2910 [`0788d91`](https://github.com/littlevgl/lvgl/commit/0788d918990fd1c03bd7a04941cfbbdf6d21987c)
- fix(layout) fix layout recalculation trigger in lv_obj_add/clear_fleg [`ee65410`](https://github.com/littlevgl/lvgl/commit/ee65410c3725070ed1779c95fb8742107cdd9267)
- fix(obj) fix lv_obj_fade_in [`4931384`](https://github.com/littlevgl/lvgl/commit/49313840ee9b249f2ef9142e872657856810acfc)
- fix(draw): fix clipping children to parent [`5c98ac8`](https://github.com/littlevgl/lvgl/commit/5c98ac85117c24f4da61803f0dc5a9bb6cfd1fdc)
- fix: remove symlinks to be accepted as an Ardunio library [`6701d36`](https://github.com/littlevgl/lvgl/commit/6701d36afe40130479dc83efc05d4860f3f29636)
- chore: fix typos in FATFS config [`74091c4`](https://github.com/littlevgl/lvgl/commit/74091c42f7cf4e85e46e706692accb65879741e2)
- fix(refr): fix missed buffer switch in double full-screen buffer + direct_mode [`731ef5a`](https://github.com/littlevgl/lvgl/commit/731ef5a75ea7feb7319315bd15bc1a43b899c1ca)
- chore(qrcode): fix warnings [`e9d7080`](https://github.com/littlevgl/lvgl/commit/e9d70803e11378eddf435e66c2181c0fa77211c7)
- docs(event): tell to not adjust widgets in draw events [`933d67f`](https://github.com/littlevgl/lvgl/commit/933d67fe5b8596da203c318aa9551aad1c2887e6)
- fix(table, chart): fix memory leaks [`8d52de1`](https://github.com/littlevgl/lvgl/commit/8d52de14b33262a11de87f5d782611a38726a1a7)
- fix(event): handle object deletion in indev-&gt;fedback_cb [`bfc8edf`](https://github.com/littlevgl/lvgl/commit/bfc8edf802382f78e96125c886427c99c7f9a600)
- fix(roller): snap on press lost [`fa9340c`](https://github.com/littlevgl/lvgl/commit/fa9340c45fd4a86b4a44878286850f3f67133bf4)
- fix(dropdown) be sure the list is the top object on the screen [`cb7fc2b`](https://github.com/littlevgl/lvgl/commit/cb7fc2bb59f788ce8024d62a5b1e821575a9cb74)
- fix(img) fix invalidation issue on transformations [`d5ede0e`](https://github.com/littlevgl/lvgl/commit/d5ede0ebc6685d4857b5ac554d53c0a7373d7532)
- fix(obj) fix comments of lv_obj_set_pos/x/y [`b9a5078`](https://github.com/littlevgl/lvgl/commit/b9a5078cd9d57662fc6e684d57a0ee4e70ca49c0)

### Examples

- example: add non-null judgment to lv_example_obj_2 [`2799`](https://github.com/littlevgl/lvgl/pull/2799)
- example(table): fix text alignment [`b03dc9c`](https://github.com/littlevgl/lvgl/commit/b03dc9cf862584c2e2be2c900fa4ff6e67b336f8)

### Docs

- docs(demos) update information to reflect new layout [`3029`](https://github.com/littlevgl/lvgl/pull/3029)
- docs(porting): remove duplicated content [`2984`](https://github.com/littlevgl/lvgl/pull/2984)
- docs(display) fix typo [`2946`](https://github.com/littlevgl/lvgl/pull/2946)
- docs(get-started) add introduction for Tasmota and Berry  [`2874`](https://github.com/littlevgl/lvgl/pull/2874)
- docs fix spelling, parameter descriptions, comments, etc [`2865`](https://github.com/littlevgl/lvgl/pull/2865)
- docs: spelling fixes [`2828`](https://github.com/littlevgl/lvgl/pull/2828)
- docs(style) minor style fix [`2818`](https://github.com/littlevgl/lvgl/pull/2818)
- docs(porting/display) fix formatting [`2812`](https://github.com/littlevgl/lvgl/pull/2812)
- docs(roadmap) update [`084439e`](https://github.com/littlevgl/lvgl/commit/084439e9476339ff571820e38bb677157edef135)
- docs(widgets) fix edit links [`7ed1a56`](https://github.com/littlevgl/lvgl/commit/7ed1a5625a5139ede832c0058b2bc6309b395321)
- docs(contributing) update commit message format [`1cd851f`](https://github.com/littlevgl/lvgl/commit/1cd851f8c09e813d75feaf9bf312f887f5ba76f0)
- docs(porting): add more details about adding lvgl to your project [`6ce7348`](https://github.com/littlevgl/lvgl/commit/6ce73486d319bfdb1c379d090036a7eeaabf5b43)
- docs(indev): add description about gestures [`2719862`](https://github.com/littlevgl/lvgl/commit/2719862fc3065b5d72c74c3f5f0923c3f6cc82c6)
- docs(style): describe const styles [`28ffae8`](https://github.com/littlevgl/lvgl/commit/28ffae8c931ff01a4e5d426a2e496053e840c094)
- docs(faq): add "LVGL doesn't start, nothing is drawn on the display" section [`0388d92`](https://github.com/littlevgl/lvgl/commit/0388d9218a36debf6c989eb999ae68478d8f6b02)
- docs add demos [`02a6614`](https://github.com/littlevgl/lvgl/commit/02a6614b38b7d94e56d8fc1f858b0e40a46c024d)
- docs(fs): update fs interface description to the latest API [`285e6b3`](https://github.com/littlevgl/lvgl/commit/285e6b39f99c078e57a611cf84cbfc3b546e112e)
- docs(format) let  wrap [`4bf49a8`](https://github.com/littlevgl/lvgl/commit/4bf49a82a3df422ebbfc4e47d4a93c945afdf0fa)
- docs(imgbtn) fix typo [`d792c5f`](https://github.com/littlevgl/lvgl/commit/d792c5f6c2e9d85c693e4f8089cb59c82d8cf805)
- docs(porting) clarify that displays must be registered before input devices [`1c64b78`](https://github.com/littlevgl/lvgl/commit/1c64b78866b4bb920db75a4b19f8ff1eb7f68a76)
- docs(event) fix lv_event_get_original_target vs lv_event_get_current_target [`cdd5128`](https://github.com/littlevgl/lvgl/commit/cdd5128bc0e17b2ffa3f9fc8f5f133d35fca4e35)
- docs(events) rename LV_EVENT_APPLY to LV_EVENT_READY (#2791) [`bf6837f`](https://github.com/littlevgl/lvgl/commit/bf6837f4c045b01144842ae63c4052e4cac7dafb)
- docs(gpu): link style properties and boxing model [`6266851`](https://github.com/littlevgl/lvgl/commit/6266851381d3b1f1e350dc4689e6bc71ece2f5c1)
- docs(gesture): clarify gesture triggering with scrolling [`e3b43ee`](https://github.com/littlevgl/lvgl/commit/e3b43eec943db48f7cbee83e07e531d41bc61ac0)
- docs(contributing): remove the mentioning of the dev branch [`00d4ef3`](https://github.com/littlevgl/lvgl/commit/00d4ef3c53d9b53e993c76d1eb0bafa7b1c9b721)
- docs(bar) fix default range [`eeee48b`](https://github.com/littlevgl/lvgl/commit/eeee48b1c943fc288521e4479d874348f4690842)
- docs(event): tell to not adjust widgets in draw events [`933d67f`](https://github.com/littlevgl/lvgl/commit/933d67fe5b8596da203c318aa9551aad1c2887e6)
- docs(switch) improve wording [`b4986ab`](https://github.com/littlevgl/lvgl/commit/b4986ab5dceb47f934c0a44a58152367f1bf8f43)
- docs(font) fix example to match v8 [`2f80896`](https://github.com/littlevgl/lvgl/commit/2f808965a1892e11cb84f50c6546871d2f2aa122)

### CI and tests

- test(bar): add unit tests [`2845`](https://github.com/littlevgl/lvgl/pull/2845)
- test(switch): add initial unit test [`2794`](https://github.com/littlevgl/lvgl/pull/2794)
- test(demo) add tests for widget and stress demos [`3bd6ad8`](https://github.com/littlevgl/lvgl/commit/3bd6ad80e7e7d0936b6e54ca88760db551f7848b)
- test(dropdown) fix to pass again [`918b3de`](https://github.com/littlevgl/lvgl/commit/918b3defd78245136da92f46fac937815ef35a1a)
- test add support for using system heap [`446b1eb`](https://github.com/littlevgl/lvgl/commit/446b1ebf2bc1ba38b5349c660534f113a9a066a9)
- ci remove formatting request workflow [`6de89e4`](https://github.com/littlevgl/lvgl/commit/6de89e4b7b0a0f72cf53e59a90bd22362088eb71)
- ci initial support for cross-architecture tests [`7008770`](https://github.com/littlevgl/lvgl/commit/7008770261903170d19472a52b54fedaafa7bbda)
- ci create handler for formatting requests [`7af7849`](https://github.com/littlevgl/lvgl/commit/7af78498a898cba6263b51094ffbc486d6b30b3a)
- test(style) add test for gradient [`da8f345`](https://github.com/littlevgl/lvgl/commit/da8f34566b0c0f3335c471c518f0766bdeb65766)
- test(event) add test for #2886 [`51ef9c2`](https://github.com/littlevgl/lvgl/commit/51ef9c242ccfff37905d71132aab33d2f642b427)
- ci add workflow to check code formatting [`a2b555e`](https://github.com/littlevgl/lvgl/commit/a2b555e096f7d401b5d8e877a6b5e81ff81c747a)
- ci attempt to speed up cross tests [`80408f7`](https://github.com/littlevgl/lvgl/commit/80408f704e8442a27f6dca96c41f1d3bded7ce52)
- ci apply my updates to the verify-formatting action [`02f02fa`](https://github.com/littlevgl/lvgl/commit/02f02fa78fc4101b1cde87fe912cb3105a689195)
- ci: add arduino linter action [`f79b00c`](https://github.com/littlevgl/lvgl/commit/f79b00cce0d31c7e5519a871b27d803fdb30fdfd)
- ci update action [`be9722c`](https://github.com/littlevgl/lvgl/commit/be9722c420a1ac2e9efde79135bf96bc508edb33)
- ci more formatting action updates [`1f6037c`](https://github.com/littlevgl/lvgl/commit/1f6037ce98c8617221d321d3371ad6dc8649553a)
- ci disable LeakSanitizer on dockerized tests [`c9e1927`](https://github.com/littlevgl/lvgl/commit/c9e19272c62f01544ff7cb5ef15d65b0d4fce5a5)
- ci one last try at this for tonight [`dddafae`](https://github.com/littlevgl/lvgl/commit/dddafaec942b7886722cdec28e2bd0f20f2a3413)
- ci try alternate checkout mechanism [`cb3de30`](https://github.com/littlevgl/lvgl/commit/cb3de308fdcdebb9c980df1d167a6be3657b2540)
- test(style) fix compile error [`ba083df`](https://github.com/littlevgl/lvgl/commit/ba083dfd6dc31d1d9127542cd1aff860d5a0153c)
- test(template) simplify _test_template.c [`b279f63`](https://github.com/littlevgl/lvgl/commit/b279f63d6bf84159aab855b962a9f431d5c40eb3)
- ci force ccache to be saved every time [`a7c590f`](https://github.com/littlevgl/lvgl/commit/a7c590f10d4c39ae33d89ad86ef608092030654b)
- ci switch to codecov v2 [`6b84155`](https://github.com/littlevgl/lvgl/commit/6b841555cd847d07375b92b54a814c41ccb522de)
- ci more debugging for formatting action [`2f8e4bc`](https://github.com/littlevgl/lvgl/commit/2f8e4bc4c43fa395676e2be5d3d55999206190b4)
- ci inline apt-get commands [`90e2b9f`](https://github.com/littlevgl/lvgl/commit/90e2b9f05e73527dfa2b2df0b1da30512827b8a8)
- ci(micropython) use ESP-IDF 4.4 [`b34fe9e`](https://github.com/littlevgl/lvgl/commit/b34fe9ed8b945fd83a1956cf4ddf2d40485a62ca)
- ci add 5k stack limit [`4122dda`](https://github.com/littlevgl/lvgl/commit/4122dda399679baa3b8bbd2e7055412b132227ab)
- ci force use of ccache in PATH [`6de3fa8`](https://github.com/littlevgl/lvgl/commit/6de3fa8004639ea02d45c1be2985290e65a3d6c0)
- ci add back stack usage check at 4 kilobytes [`89135d6`](https://github.com/littlevgl/lvgl/commit/89135d663daca34c9d9695a4c12b4208ef4ba217)
- ci temporarily disable stack usage check [`1900c21`](https://github.com/littlevgl/lvgl/commit/1900c215482b9b1b5af1dd7c5cb8a95e89906b77)
- ci(cross) use python3 instead of python [`df7eaa0`](https://github.com/littlevgl/lvgl/commit/df7eaa020d656c519b5197cd3d19c587cb1dd234)
- ci use specific version tag [`59b4769`](https://github.com/littlevgl/lvgl/commit/59b476934452d5821424c70954aa32be6f476608)
- ci fix check style action [`5bb3686`](https://github.com/littlevgl/lvgl/commit/5bb3686ea8b6feb55d6bb2b345f5c6cee52d514a)
- ci fix typo in formatting action [`d1ccbf6`](https://github.com/littlevgl/lvgl/commit/d1ccbf607fd3aec61c4606a8f2c268225654b792)
- ci test formatting action [`065d821`](https://github.com/littlevgl/lvgl/commit/065d821c7050af6ad94c7d6dc2d4976a817e54a0)
- ci(micropython) switch to newer GCC action [`1fa7257`](https://github.com/littlevgl/lvgl/commit/1fa7257801f4e0d3c184be438fd7ecb067818c48)
- ci(style) force color on diff to help highlight whitespace changes [`04f47ea`](https://github.com/littlevgl/lvgl/commit/04f47eae0d40c8385535428566d1851ff8ea20eb)
- ci(cross) install build-essential [`772f219`](https://github.com/littlevgl/lvgl/commit/772f219c0af4ba013ee9b71883e7dc265e5d22f9)
- ci force pushing to upstream branch [`8277f78`](https://github.com/littlevgl/lvgl/commit/8277f78d132b4c397f39a9e17cdb7bdd381d1778)
- ci ensure lvgl-bot is used to make commits [`9fcf52a`](https://github.com/littlevgl/lvgl/commit/9fcf52a82bb4dbcfc47e69b7875d66a3d25ba87f)




## [v8.1.0](https://github.com/lvgl/lvgl/compare/v8.0.2...v8.1.0) 10 November 2021

### Overview
v8.1 is a minor release, so besides many fixes it contains a lot of new features too.

Some of the most important features are
- Built in support for SDL based GPU drawing
- Much faster circle drawing in the software renderer
- Several [3rd party libraries](https://docs.lvgl.io/master/libs/index.html) are merged directly into LVGL.
- Add LVGL as an [RT-Thread](https://packages.rt-thread.org/en/detail.html?package=LVGL) and [ESP32](https://components.espressif.com/component/lvgl/lvgl) component

### Breaking Changes

- :warning: feat(calendar): add the header directly into the calendar widget [`2e08f80`](https://github.com/lvgl/lvgl/commit/2e08f80361a9d7e5b97f49af6afc3549ffbf2758)

### Architectural

- arch add small 3rd party libs to lvgl  [`2569`](https://github.com/lvgl/lvgl/pull/2569)

### New Features

- feat(display) add direct_mode drawing mode [`2460`](https://github.com/lvgl/lvgl/pull/2460)
- feat(conf): make LV_MEM_BUF_MAX_NUM configurable [`2747`](https://github.com/lvgl/lvgl/pull/2747)
- feat(disp): add non-fullscreen display utilities [`2724`](https://github.com/lvgl/lvgl/pull/2724)
- feat(rlottie) add LVGL-Rlottie interface as 3rd party lib [`2700`](https://github.com/lvgl/lvgl/pull/2700)
- feat(rtthread): prepare for porting the device-driver of rt-thread [`2719`](https://github.com/lvgl/lvgl/pull/2719)
- feat(fsdrv) add driver based on Win32 API [`2701`](https://github.com/lvgl/lvgl/pull/2701)
- feat(span) indent supports percent for fix and break mode [`2693`](https://github.com/lvgl/lvgl/pull/2693)
- feat(rt-thread): implement rt-thread sconscirpt [`2674`](https://github.com/lvgl/lvgl/pull/2674)
- feat(lv_spinbox) support both right-to-left and left-to-right digit steps when clicking encoder button [`2644`](https://github.com/lvgl/lvgl/pull/2644)
- feat add support for rt-thread RTOS [`2660`](https://github.com/lvgl/lvgl/pull/2660)
- feat(disp): Enable rendering to display subsection [`2583`](https://github.com/lvgl/lvgl/pull/2583)
- feat(keyboard): add user-defined modes [`2651`](https://github.com/lvgl/lvgl/pull/2651)
- feat(event) add LV_EVENT_CHILD_CREATED/DELETED [`2618`](https://github.com/lvgl/lvgl/pull/2618)
- feat(btnmatrix/keyboard): add option to show popovers on button press [`2537`](https://github.com/lvgl/lvgl/pull/2537)
- feat(msgbox) add a content area for custom content [`2561`](https://github.com/lvgl/lvgl/pull/2561)
- feat(tests): Include debug information to test builds [`2568`](https://github.com/lvgl/lvgl/pull/2568)
- feat(drawing) hardware accelerated rendering by SDL2 [`2484`](https://github.com/lvgl/lvgl/pull/2484)
- feat(msgbox): omit title label unless needed [`2539`](https://github.com/lvgl/lvgl/pull/2539)
- feat(msgbox): add function to get selected button index [`2538`](https://github.com/lvgl/lvgl/pull/2538)
- feat(make) add lvgl interface target for micropython [`2529`](https://github.com/lvgl/lvgl/pull/2529)
- feat(obj) add lv_obj_move_to_index(obj, index), renamed lv_obj_get_child_id(obj) to lv_obj_get_index(obj) [`2514`](https://github.com/lvgl/lvgl/pull/2514)
- feat(obj) add lv_obj_swap() function [`2461`](https://github.com/lvgl/lvgl/pull/2461)
- feat(mem) LV_MEM_POOL_ALLOC [`2458`](https://github.com/lvgl/lvgl/pull/2458)
- feat(switch) add smooth animation when changing state [`2442`](https://github.com/lvgl/lvgl/pull/2442)
- feat(anim) add interface for handling lv_anim user data. [`2415`](https://github.com/lvgl/lvgl/pull/2415)
- feat(obj) add lv_is_initialized [`2402`](https://github.com/lvgl/lvgl/pull/2402)
- feat(obj) Backport keypad and encoder scrolling from v7 `lv_page` to v8 `lv_obj` [`2390`](https://github.com/lvgl/lvgl/pull/2390)
- feat(snapshot) add API to take snapshot for object [`2353`](https://github.com/lvgl/lvgl/pull/2353)
- feat(anim) add anim timeline [`2309`](https://github.com/lvgl/lvgl/pull/2309)
- feat(span) Add missing spangroup functions  [`2379`](https://github.com/lvgl/lvgl/pull/2379)
- feat(img) add img_size property [`2284`](https://github.com/lvgl/lvgl/pull/2284)
- feat(calendar) improve MicroPython example [`2366`](https://github.com/lvgl/lvgl/pull/2366)
- feat(spinbox ) add function to set cursor to specific position [`2314`](https://github.com/lvgl/lvgl/pull/2314)

- feat(timer) check if lv_tick_inc is called [`aa6641a`](https://github.com/lvgl/lvgl/commit/aa6641a6f1c1311ce7e0f94783ee7f582452a88f)
- feat(event, widgets) improve the parameter of LV_EVENT_DRAW_PART_BEGIN/END [`88c4859`](https://github.com/lvgl/lvgl/commit/88c485949fca2686357a7dee88d5730678ba9bc7)
- feat(docs) improvements to examples [`4b8c73a`](https://github.com/lvgl/lvgl/commit/4b8c73a5770657ab55bbe825f7887e28c55a8a4a)
- feat(obj) send LV_EVENT_DRAW_PART_BEGIN/END for MAIN and SCROLLBAR parts [`b203167`](https://github.com/lvgl/lvgl/commit/b203167c7583905e2cb4006e57a16432841a2353)
- feat(led) send LV_EVENT_DRAW_PART_BEGIN/END [`fcd4aa3`](https://github.com/lvgl/lvgl/commit/fcd4aa3924469c2a92ab6a04b7bc6de6304cc54a)
- feat(chart) send LV_EVENT_DRAW_PART_BEGIN/END before/after the division line drawing section. [`e0ae2aa`](https://github.com/lvgl/lvgl/commit/e0ae2aa106874b1cf60ba54dd043cde8f834f7e9)
- feat(tests) upload coverage to codecov [`4fff99d`](https://github.com/lvgl/lvgl/commit/4fff99da1dd2f8bd0c1e0012d81d46aaadb0d5a3)
- feat(conf) add better check for Kconfig default [`f8fe536`](https://github.com/lvgl/lvgl/commit/f8fe5366bb051cd5090e4a06658eb0d32decc0b3)
- feat(draw) add LV_BLEND_MODE_MULTIPLY [`cc78ef4`](https://github.com/lvgl/lvgl/commit/cc78ef450649a10f260649dc3ba19ac8a6b88e86)
- feat(test) add assert for screenshot compare [`2f7a005`](https://github.com/lvgl/lvgl/commit/2f7a005bd31c10d0a048f55641e4af11bcb5bbfa)
- feat(event) pass the scroll animation to LV_EVENT_SCROLL_BEGIN [`ca54ecf`](https://github.com/lvgl/lvgl/commit/ca54ecfe0eac880203d23b2d2244b9b63b9f7b77)
- feat(obj) place the scrollbar to the left with RTL base dir. [`906448e`](https://github.com/lvgl/lvgl/commit/906448ef6321f160859f21c5937180bb89d8ef1e)
- feat(log) allow overwriting LV_LOG_... macros [`17b8a76`](https://github.com/lvgl/lvgl/commit/17b8a76c4a887c9cf464484406a6631ea0194ad5)
- feat(arc) add support to LV_OBJ_FLAG_ADV_HITTEST [`dfa4f5c`](https://github.com/lvgl/lvgl/commit/dfa4f5cff561a60b4ffcec17e025f1e056854fff)
- feat(event) add LV_SCREEN_(UN)LOAD_START [`7bae9e3`](https://github.com/lvgl/lvgl/commit/7bae9e3ddde9d6bdc06ae437f20a789cd330a556)
- feat(obj) add lv_obj_del_delayed() [`c6a2e15`](https://github.com/lvgl/lvgl/commit/c6a2e15ec23c8e96f71bafa8e43ef67fc4a73d0a)
- feat(docs) add view on GitHub link [`a716ac6`](https://github.com/lvgl/lvgl/commit/a716ac6ed267e0a2e019fe7d2fda1bef0046cdc7)
- feat(event) add LV_EVENT_SCREEN_LOADED/UNLOADED events [`ee5369e`](https://github.com/lvgl/lvgl/commit/ee5369e2d2ce12f47c78a2bf339aa6fb2421ba2b)
- feat(textarea) remove the need of lv_textarea_set_align [`56ebb1a`](https://github.com/lvgl/lvgl/commit/56ebb1a4c8cc988482ac9f118fa3c654553db941)
- feat(rt-thread): support LVGL projects with GCC/Keil(AC5)/Keil(AC6)/IAR [`32d33fe`](https://github.com/lvgl/lvgl/commit/32d33fe4d9a38f6c215a6b9a631eb987339677ae)
- feat(docs) lazy load individual examples as well [`918d948`](https://github.com/lvgl/lvgl/commit/918d94801f2ee4ad7b6c075d96d2e9195459fbb8)

- feat: add LV_USE_MEM_PERF/MONITOR_POS [`acd0f4f`](https://github.com/lvgl/lvgl/commit/acd0f4fbc71ffbfeb382b7af1fa52caf3cdcda6c)
- feat(canvas) add lv_canvas_set_px_opa [`b3b3ffc`](https://github.com/lvgl/lvgl/commit/b3b3ffc2b3b322f7401d15c4ba2ef0cdb00e2990)
- feat(event) add lv_obj_remove_event_cb_with_user_data [`4eddeb3`](https://github.com/lvgl/lvgl/commit/4eddeb35abee1f9cd2d1fd210f11cc096cb609c7)
- feat(obj) add lv_obj_get_x/y_aligned [`98bc1fe`](https://github.com/lvgl/lvgl/commit/98bc1fe09e12a64333e91b4c25327c283a700af5)

### Performance

- perf(draw) reimplement circle drawing algorithms [`2374`](https://github.com/lvgl/lvgl/pull/2374)
- perf(anim_timeline) add lv_anim_timeline_stop() [`2411`](https://github.com/lvgl/lvgl/pull/2411)

- perf(obj) remove lv_obj_get_child_cnt from cycle limit checks [`ebb9ce9`](https://github.com/lvgl/lvgl/commit/ebb9ce913e604055724fd5f72562c9de0933ff73)
- perf(draw) reimplement rectangle drawing algorithms [`5b3d3dc`](https://github.com/lvgl/lvgl/commit/5b3d3dc8b35bdd16e5dea00ffc40b7a20471079d)
- perf(draw) ignore masks if they don't affect the current draw area [`a842791`](https://github.com/lvgl/lvgl/commit/a8427915c747dfe562f7f7e80adb6d1be5b2eeae)
- perf(refresh) optimize where to wait for lv_disp_flush_ready with 2 buffers [`d0172f1`](https://github.com/lvgl/lvgl/commit/d0172f14a454c98e6979322e7c2622a7001bb3e6)
- perf(draw) speed up additive blending [`3abe517`](https://github.com/lvgl/lvgl/commit/3abe517abf3b62366f2eb4bed77d5c7a691f7ed5)

### Fixes

- fix(bidi): add weak characters to the previous strong character's run [`2777`](https://github.com/lvgl/lvgl/pull/2777)
- fix(draw_img): radius mask doesn't work in specific condition [`2786`](https://github.com/lvgl/lvgl/pull/2786)
- fix(border_post): ignore bg_img_opa draw when draw border_post [`2788`](https://github.com/lvgl/lvgl/pull/2788)
- fix(refresh) switch to portable format specifiers [`2781`](https://github.com/lvgl/lvgl/pull/2781)
- fix(stm32) Mark unused variable in stm32 DMA2D driver [`2782`](https://github.com/lvgl/lvgl/pull/2782)
- fix(conf): Make LV_COLOR_MIX_ROUND_OFS configurable  [`2766`](https://github.com/lvgl/lvgl/pull/2766)
- fix(misc): correct the comment and code style [`2769`](https://github.com/lvgl/lvgl/pull/2769)
- fix(draw_map) use existing variables instead function calls  [`2776`](https://github.com/lvgl/lvgl/pull/2776)
- fix(draw_img): fix typos in API comments [`2773`](https://github.com/lvgl/lvgl/pull/2773)
- fix(draw_img):radius Mask doesn't work in Specific condition [`2775`](https://github.com/lvgl/lvgl/pull/2775)
- fix(proto) Remove redundant prototype declarations [`2771`](https://github.com/lvgl/lvgl/pull/2771)
- fix(conf) better support bool option from Kconfign [`2555`](https://github.com/lvgl/lvgl/pull/2555)
- fix(draw_border):draw error if radius == 0 and parent clip_corner == true [`2764`](https://github.com/lvgl/lvgl/pull/2764)
- fix(msgbox) add declaration for lv_msgbox_content_class [`2761`](https://github.com/lvgl/lvgl/pull/2761)
- fix(core) add L suffix to enums to ensure 16-bit compatibility [`2760`](https://github.com/lvgl/lvgl/pull/2760)
- fix(anim): add lv_anim_get_playtime [`2745`](https://github.com/lvgl/lvgl/pull/2745)
- fix(area) minor fixes [`2749`](https://github.com/lvgl/lvgl/pull/2749)
- fix(mem): ALIGN_MASK should equal 0x3 on 32bit platform [`2748`](https://github.com/lvgl/lvgl/pull/2748)
- fix(template) prototype error [`2755`](https://github.com/lvgl/lvgl/pull/2755)
- fix(anim): remove time_orig from lv_anim_t [`2744`](https://github.com/lvgl/lvgl/pull/2744)
- fix(draw_rect):bottom border lost if enable clip_corner [`2742`](https://github.com/lvgl/lvgl/pull/2742)
- fix(anim) and improvement [`2738`](https://github.com/lvgl/lvgl/pull/2738)
- fix(draw border):border draw error if border width &gt; radius [`2739`](https://github.com/lvgl/lvgl/pull/2739)
- fix(fsdrv): remove the seek call in fs_open [`2736`](https://github.com/lvgl/lvgl/pull/2736)
- fix(fsdrv): skip the path format if LV_FS_xxx_PATH not defined [`2726`](https://github.com/lvgl/lvgl/pull/2726)
- fix: mark unused variable with LV_UNUSED(xxx) instead of (void)xxx [`2734`](https://github.com/lvgl/lvgl/pull/2734)
- fix(fsdrv): fix typo error in commit 752fba34f677ad73aee [`2732`](https://github.com/lvgl/lvgl/pull/2732)
- fix(fsdrv): return error in case of the read/write failure [`2729`](https://github.com/lvgl/lvgl/pull/2729)
- fix(refr) silence compiler warning due to integer type mismatch [`2722`](https://github.com/lvgl/lvgl/pull/2722)
- fix(fs): fix the off-by-one error in the path function [`2725`](https://github.com/lvgl/lvgl/pull/2725)
- fix(timer): remove the code duplication in lv_timer_exec [`2708`](https://github.com/lvgl/lvgl/pull/2708)
- fix(async): remove the wrong comment from lv_async_call [`2707`](https://github.com/lvgl/lvgl/pull/2707)
- fix(kconfig): change CONFIG_LV_THEME_DEFAULT_FONT to CONFIG_LV_FONT_DEFAULT [`2703`](https://github.com/lvgl/lvgl/pull/2703)
- fix add MP support for LVGL 3rd party libraries [`2666`](https://github.com/lvgl/lvgl/pull/2666)
- fix(png) memory leak for sjpg and use lv_mem_... in lv_png [`2704`](https://github.com/lvgl/lvgl/pull/2704)
- fix(gif) unified whence and remove off_t [`2690`](https://github.com/lvgl/lvgl/pull/2690)
- fix(rt-thread): include the rt-thread configuration header file [`2692`](https://github.com/lvgl/lvgl/pull/2692)
- fix(rt-thread): fix the ci error [`2691`](https://github.com/lvgl/lvgl/pull/2691)
- fix(fsdrv) minor fs issue [`2682`](https://github.com/lvgl/lvgl/pull/2682)
- fix(hal) fix typos and wording in docs for lv_hal_indev.h [`2685`](https://github.com/lvgl/lvgl/pull/2685)
- fix(hal tick): add precompile !LV_TICK_CUSTOM for global variables and lv_tick_inc() [`2675`](https://github.com/lvgl/lvgl/pull/2675)
- fix(anim_timeline) avoid calling lv_anim_del(NULL, NULL) [`2628`](https://github.com/lvgl/lvgl/pull/2628)
- fix(kconfig) sync Kconfig with the latest lv_conf_template.h [`2662`](https://github.com/lvgl/lvgl/pull/2662)
- fix(log) reduce the stack usage in log function [`2649`](https://github.com/lvgl/lvgl/pull/2649)
- fix(conf) make a better style alignment in lv_conf_internal.h [`2652`](https://github.com/lvgl/lvgl/pull/2652)
- fix(span) eliminate warning in lv_get_snippet_cnt() [`2659`](https://github.com/lvgl/lvgl/pull/2659)
- fix(config): remove the nonexistent Kconfig [`2654`](https://github.com/lvgl/lvgl/pull/2654)
- fix(Kconfig): add LV_MEM_ADDR config [`2653`](https://github.com/lvgl/lvgl/pull/2653)
- fix(log): replace printf with fwrite to save the stack size [`2655`](https://github.com/lvgl/lvgl/pull/2655)
- fix typos [`2634`](https://github.com/lvgl/lvgl/pull/2634)
- fix LV_FORMAT_ATTRIBUTE fix for gnu &gt; 4.4 [`2631`](https://github.com/lvgl/lvgl/pull/2631)
- fix(meter) make lv_meter_indicator_type_t of type uint8_t [`2632`](https://github.com/lvgl/lvgl/pull/2632)
- fix(span):crash if span-&gt;txt = "" [`2616`](https://github.com/lvgl/lvgl/pull/2616)
- fix(disp) set default theme also for non-default displays [`2596`](https://github.com/lvgl/lvgl/pull/2596)
- fix(label):LONG_DOT mode crash if text Utf-8 encode &gt; 1 [`2591`](https://github.com/lvgl/lvgl/pull/2591)
- fix( example) in lv_example_scroll_3.py float_btn should only be created once [`2602`](https://github.com/lvgl/lvgl/pull/2602)
- fix lv_deinit when LV_USE_GPU_SDL is enabled [`2598`](https://github.com/lvgl/lvgl/pull/2598)
- fix add missing LV_ASSERT_OBJ checks [`2575`](https://github.com/lvgl/lvgl/pull/2575)
- fix(lv_conf_internal_gen.py)  formatting fixes on the generated file [`2542`](https://github.com/lvgl/lvgl/pull/2542)
- fix(span) opa bug [`2584`](https://github.com/lvgl/lvgl/pull/2584)
- fix(snapshot) snapshot is affected by parent's style because of wrong coords [`2579`](https://github.com/lvgl/lvgl/pull/2579)
- fix(label):make draw area contain ext_draw_size [`2587`](https://github.com/lvgl/lvgl/pull/2587)
- fix(btnmatrix): make ORed values work correctly with lv_btnmatrix_has_btn_ctrl [`2571`](https://github.com/lvgl/lvgl/pull/2571)
- fix compiling of examples when cmake is used [`2572`](https://github.com/lvgl/lvgl/pull/2572)
- fix(lv_textarea) fix crash while delete non-ascii character in pwd mode [`2549`](https://github.com/lvgl/lvgl/pull/2549)
- fix(lv_log.h): remove the duplicated semicolon from LV_LOG_xxx [`2544`](https://github.com/lvgl/lvgl/pull/2544)
- fix(zoom) multiplication overflow on 16-bit platforms [`2536`](https://github.com/lvgl/lvgl/pull/2536)
- fix(printf) use __has_include for more accurate limits information [`2532`](https://github.com/lvgl/lvgl/pull/2532)
- fix(font) add assert in lv_font.c if the font is NULL [`2533`](https://github.com/lvgl/lvgl/pull/2533)
- fix(lv_types.h): remove c/c++ compiler version check [`2525`](https://github.com/lvgl/lvgl/pull/2525)
- fix(lv_utils.c): remove the unneeded header inclusion [`2526`](https://github.com/lvgl/lvgl/pull/2526)
- fix(Kconfig) fix the comment in LV_THEME_DEFAULT_DARK [`2524`](https://github.com/lvgl/lvgl/pull/2524)
- fix(sprintf) add format string for rp2 port [`2512`](https://github.com/lvgl/lvgl/pull/2512)
- fix(span) fix some bugs (overflow,decor,align) [`2518`](https://github.com/lvgl/lvgl/pull/2518)
- fix(color) Bad cast in lv_color_mix() caused UB with 16bpp or less [`2509`](https://github.com/lvgl/lvgl/pull/2509)
- fix(imgbtn)  displayed incorrect when the coordinate is negative [`2501`](https://github.com/lvgl/lvgl/pull/2501)
- fix(event) be sure to move all elements in copy “lv_obj_remove_event_cb” [`2492`](https://github.com/lvgl/lvgl/pull/2492)
- fix(draw) use correct pointer in lv_draw_mask assertion [`2483`](https://github.com/lvgl/lvgl/pull/2483)
- feat(mem) LV_MEM_POOL_ALLOC [`2458`](https://github.com/lvgl/lvgl/pull/2458)
- fix(cmake) require 'main' for Micropython [`2444`](https://github.com/lvgl/lvgl/pull/2444)
- fix(docs) add static keyword to driver declaration [`2452`](https://github.com/lvgl/lvgl/pull/2452)
- fix(build) remove main component dependency [`2420`](https://github.com/lvgl/lvgl/pull/2420)
- fix circle drawing algorithms [`2413`](https://github.com/lvgl/lvgl/pull/2413)
- fix(docs) wrong spelling of words in pictures [`2409`](https://github.com/lvgl/lvgl/pull/2409)
- fix(chart) fixed point-following cursor during vertical scroll in charts [`2400`](https://github.com/lvgl/lvgl/pull/2400)
- fix(chart) fixed cursor positioning with large Y rescaling without LV_USE_LARGE_COORD [`2399`](https://github.com/lvgl/lvgl/pull/2399)
- fix(grid.h) typos [`2395`](https://github.com/lvgl/lvgl/pull/2395)
- fix(anim_timeline) heap use after free [`2394`](https://github.com/lvgl/lvgl/pull/2394)
- fix(snapshot) add missing import on MicroPython example [`2389`](https://github.com/lvgl/lvgl/pull/2389)
- fix(disp) Fix assert failure in lv_disp_remove [`2382`](https://github.com/lvgl/lvgl/pull/2382)
- fix(span) modify the underline position [`2376`](https://github.com/lvgl/lvgl/pull/2376)
- fix(color) remove extraneous _LV_COLOR_MAKE_TYPE_HELPER [`2372`](https://github.com/lvgl/lvgl/pull/2372)
- fix(spinner) should not be clickable [`2373`](https://github.com/lvgl/lvgl/pull/2373)
- fix(workflow) silence SDL warning for MicroPython [`2367`](https://github.com/lvgl/lvgl/pull/2367)
- fix (span) fill LV_EVENT_GET_SELF_SIZE [`2360`](https://github.com/lvgl/lvgl/pull/2360)
- fix(workflow) change MicroPython workflow to use master [`2358`](https://github.com/lvgl/lvgl/pull/2358)
- fix(disp) fix memory leak in lv_disp_remove [`2355`](https://github.com/lvgl/lvgl/pull/2355)
- fix(lv_obj.h)typos [`2350`](https://github.com/lvgl/lvgl/pull/2350)
- fix(obj) delete useless type conversion [`2343`](https://github.com/lvgl/lvgl/pull/2343)
- fix(lv_obj_scroll.h) typos [`2345`](https://github.com/lvgl/lvgl/pull/2345)
- fix(txt) enhance the function of break_chars [`2327`](https://github.com/lvgl/lvgl/pull/2327)

- fix(vglite): update for v8 [`e3e3eea`](https://github.com/lvgl/lvgl/commit/e3e3eeaf8c1593d384c6537244a301cdc1abd3d9)
- fix(widgets) use lv_obj_class for all the widgets [`3fb8baf`](https://github.com/lvgl/lvgl/commit/3fb8baf503411e006765020f60f295a4be16ba2d)
- fix(refr) reduce the nesting level in lv_refr_area [`2df1282`](https://github.com/lvgl/lvgl/commit/2df12827dda3f217fa26d2c98445a9b3f1ff22ab)
- fix(pxp): update for v8 [`8a2a4a1`](https://github.com/lvgl/lvgl/commit/8a2a4a11c81d029ff737980b883c62dfbb4b44c6)
- fix(obj) move clean ups from lv_obj_del to lv_obj_destructor [`b063937`](https://github.com/lvgl/lvgl/commit/b06393747f61e36996a0cb22f9309c951f900ded)
- fix (draw) fix arc bg image drawing with full arcs [`c3b6c6d`](https://github.com/lvgl/lvgl/commit/c3b6c6dc64735e1bde492a8d5570f3e3a9500a0b)
- fix(pxp): update RTOS macro for SDK 2.10 [`00c3eb1`](https://github.com/lvgl/lvgl/commit/00c3eb197cb85e480809d97eb722589d75d81d94)
- fix(textarea) style update in oneline mode + improve sroll to cursor [`60d9a5e`](https://github.com/lvgl/lvgl/commit/60d9a5e493bf17ee9887ba44890d00905bc55970)
- feat(led) send LV_EVENT_DRAW_PART_BEGIN/END [`fcd4aa3`](https://github.com/lvgl/lvgl/commit/fcd4aa3924469c2a92ab6a04b7bc6de6304cc54a)
- fix warnings introduced by 3fb8baf5 [`e302403`](https://github.com/lvgl/lvgl/commit/e3024032dc5de2ece4fa17059ebad4189a5fa670)
- fix(roller) fix partial redraw of the selected area [`6bc40f8`](https://github.com/lvgl/lvgl/commit/6bc40f8c4417a94ab26b25220324e471e03ce443)
- fix(flex) fix layout update and invalidation issues [`5bd82b0`](https://github.com/lvgl/lvgl/commit/5bd82b038b841c0f7c93bbdacdbd61d6b9585846)
- fix(indev) focus on objects on release instead of press [`76a8293`](https://github.com/lvgl/lvgl/commit/76a8293375b705a5e02e4f9c8f8a42d99db762e2)
- fix tests [`449952e`](https://github.com/lvgl/lvgl/commit/449952e3b78d02802960dabb0207b960c82e8e5a)
- fix(dropdown) forget the selected option on encoder longpress [`e66b935`](https://github.com/lvgl/lvgl/commit/e66b9350617eee15e94fb6a353283433e4c2c494)
- fix(obj) improve how the focusing indev is determined [`a04f2de`](https://github.com/lvgl/lvgl/commit/a04f2dea644787ea25ef988a43e10c5005c57066)
- fix(workflow) speed up MicroPython workflow [`38ad5d5`](https://github.com/lvgl/lvgl/commit/38ad5d548b2024f0f742ba769a6715fc376541a1)
- fix(test) do not including anything in test files when not running tests [`9043860`](https://github.com/lvgl/lvgl/commit/90438603ad020799b14bc9839a51dceedfdabd7a)
- fix tests [`36b9db3`](https://github.com/lvgl/lvgl/commit/36b9db38b728b40096b9ee613f4482ef9654d570)
- fix(scroll) fire LV_EVENT_SCROLL_BEGIN in the same spot for both axes [`b158932`](https://github.com/lvgl/lvgl/commit/b1589326d41924292fbc2c62b474dec288bc9da5)
- fix(btnmatrix) fix button invalidation on focus change [`77cedfa`](https://github.com/lvgl/lvgl/commit/77cedfa08f3f8aec67c6a2fe8e5ae9bab5a0e7c7)
- fix(tlsf) do not use &lt;assert.h&gt; [`c9745b9`](https://github.com/lvgl/lvgl/commit/c9745b9c4ea9e7c6de4bd8ad9a0d8001bfb91165)
- fix(template) include lvgl.h in lv_port_*_template.c files [`0ae15bd`](https://github.com/lvgl/lvgl/commit/0ae15bd470548ff159f44e7c3f4b225ab3eec928)
- fix(docs) add margin for example description [`b5f632e`](https://github.com/lvgl/lvgl/commit/b5f632ee7a265ce4f2472522b422b8cd5366aaa9)
- fix(imgbtn) use the correct src in LV_EVENT_GET_SELF_SIZE [`04c515a`](https://github.com/lvgl/lvgl/commit/04c515adac764761e60094db789269130ac89b36)
- fix(color) remove extraneous cast for 8-bit color [`157534c`](https://github.com/lvgl/lvgl/commit/157534cdbfaa7b769114126f74c38661b99d025b)
- fix(workflow) use same Unix port variant for MicroPython submodules [`ac68b10`](https://github.com/lvgl/lvgl/commit/ac68b10e539ddb8bde47ec453a99f2b876e12c65)
- fix(README) improve grammar [`de81889`](https://github.com/lvgl/lvgl/commit/de81889cbdc889360e8bc00684f9ca77ff97d89f)
- fix(printf) skip defining attribute if pycparser is used [`ee9bbea`](https://github.com/lvgl/lvgl/commit/ee9bbea29c807707353e8b9ec09048990de18e4e)
- fix(README) spelling correction [`41869f2`](https://github.com/lvgl/lvgl/commit/41869f238e773e599959c9ef2fee0b7206712ee2)
- fix(color) overflow with 16-bit color depth [`fe6d8d7`](https://github.com/lvgl/lvgl/commit/fe6d8d7636ae283afda68e85b2d1f143d8d05462)
- fix(docs) consider an example to be visible over a wider area [`145a0fa`](https://github.com/lvgl/lvgl/commit/145a0fad0857dad7f2066e7d22436827e0d3fd7d)
- fix(codecov) disable uploading coverage for pull requests [`27d88de`](https://github.com/lvgl/lvgl/commit/27d88de899e91cd5bb9fc69fe9d71cb180cfb44b)
- fix(arc) disable LV_OBJ_FLAG_SCROLL_CHAIN by default [`f172eb3`](https://github.com/lvgl/lvgl/commit/f172eb3fd78481d6076ead395abfd765646ad21e)
- fix(template) update lv_objx_template to v8 [`38bb8af`](https://github.com/lvgl/lvgl/commit/38bb8afc16720e8d8fe6e72be6fae4f9da593bbc)
- fix(align) avoid circular references with LV_SIZE_CONTENT [`038b781`](https://github.com/lvgl/lvgl/commit/038b78122e72db67cec886d09eb2d21aaa019df7)
- fix(draw) with additive blending with 32-bit color depth [`786db2a`](https://github.com/lvgl/lvgl/commit/786db2afe6458e24681b8a40fa798429956d3420)
- fix(arc) fix arc invalidation again [`5ced080`](https://github.com/lvgl/lvgl/commit/5ced08001c384bf7c840750c0e254b5f0115a070)
- fix(align) fix lv_obj_align_to [`93b38e9`](https://github.com/lvgl/lvgl/commit/93b38e92be9ed3ae050a1ee6e5b680ab43fd4850)
- fix(scroll) keep the scroll position on object deleted [`52edbb4`](https://github.com/lvgl/lvgl/commit/52edbb46b0741d2761a11ef1b3d516ec96a7c8b3)
- fix(dropdown) handle LV_KEY_ENTER [`8a50edd`](https://github.com/lvgl/lvgl/commit/8a50edd0689c7133ca18fd476596ddc4088f86a9)
- fix various minor warnings [`924bc75`](https://github.com/lvgl/lvgl/commit/924bc754adcbabaf3518bac6067e7ea37f2f0f04)
- fix(textarea) various cursor drawing fixes [`273a0eb`](https://github.com/lvgl/lvgl/commit/273a0eb32f04e81f326288a71682bea1c812c76a)
- fix(label) consider base dir lv_label_get_letter_pos in special cases [`6df5122`](https://github.com/lvgl/lvgl/commit/6df51225c261b252f0935804b0357d6e585da53d)
- fix(imgbtn) add lv_imgbtn_set_state [`26e15fa`](https://github.com/lvgl/lvgl/commit/26e15fa577f97d510b218fb95fc9a4bd440b00bc)
- fix(printf) add (int) casts to log messages to avoid warnings on %d [`d9d3f27`](https://github.com/lvgl/lvgl/commit/d9d3f271267e760c8459b65c392914143a58b89c)
- fix(test) silence make [`7610d38`](https://github.com/lvgl/lvgl/commit/7610d38bb044b1bd95dd68ab57f79f82e2527cca)
- fix(test) silence make [`37fd9d8`](https://github.com/lvgl/lvgl/commit/37fd9d8a24c276079ed26b5d6704bcefc9f8dc70)
- fix(calendar) update the MP example [`0bab4a7`](https://github.com/lvgl/lvgl/commit/0bab4a72cf769872a9adfd5bfa1c4536e6f909a8)
- fix(scroll) fix scroll_area_into_view with objects larger than the parent [`5240fdd`](https://github.com/lvgl/lvgl/commit/5240fdda5ccc33d166f8201818868add5d1d6d0d)
- fix(msgbox) handle NULL btn map parameter [`769c4a3`](https://github.com/lvgl/lvgl/commit/769c4a30cf962be1f74e0b1cd7ebaefbd6ba8a8b)
- fix (scroll) do not send unnecessary scroll end events [`3ce5226`](https://github.com/lvgl/lvgl/commit/3ce5226c9d9db279904c4f076ae77e6e03572e4c)
- fix(obj_pos) consider all alignments in content size calculation but only if x and y = 0 [`5b27ebb`](https://github.com/lvgl/lvgl/commit/5b27ebb4097166f8c4a50ee5d39249939bf79814)
- fix(img decoder) add error handling if the dsc-&gt;data = NULL [`d0c1c67`](https://github.com/lvgl/lvgl/commit/d0c1c673a8ec17b842ebf97d5f21938ec8901346)
- fix(txt): skip basic arabic vowel characters when processing conjunction [`5b54800`](https://github.com/lvgl/lvgl/commit/5b548006eda0695cabf2ee237a7faee8c69e4659)
- fix(typo) rename LV_OBJ_FLAG_SNAPABLE to LV_OBJ_FLAG_SNAPPABLE [`e697807`](https://github.com/lvgl/lvgl/commit/e697807cf5c01be2531fc52df78ecad75ce39a7a)
- fix(lv_printf.h): to eliminate the errors in Keil and IAR [`f6d7dc7`](https://github.com/lvgl/lvgl/commit/f6d7dc7f00d0a20f7f1966ed890a225b1fc87107)
- fix(draw) fix horizontal gradient drawing [`4c034e5`](https://github.com/lvgl/lvgl/commit/4c034e56e049ad3d9bca5eb4b3e8721e60c11d36)
- fix(dropdown) use LV_EVENT_READY/CANCEL on list open/close [`4dd1d56`](https://github.com/lvgl/lvgl/commit/4dd1d566fc30bbaf1424dda8b78df97c6bf07402)
- fix(table) clip overflowing content [`8c15933`](https://github.com/lvgl/lvgl/commit/8c15933030cad6cdbfe4967f566ed6959547fada)
- fix(test) add #if guard to exclude test related files from the build [`c12a22e`](https://github.com/lvgl/lvgl/commit/c12a22ee87681d1344696a3b9531e9100808eb85)
- fix(test) add #if guard to exclude test related files from the build [`fc364a4`](https://github.com/lvgl/lvgl/commit/fc364a466c0693aefa0401f5eddee2bbc3037ef0)
- fix(freetype) fix underline calculation [`76c8ee6`](https://github.com/lvgl/lvgl/commit/76c8ee6b7e81d8640aa5ba620947660a1c90482b)
- fix(style) refresh ext. draw pad for padding and bg img [`37a5d0c`](https://github.com/lvgl/lvgl/commit/37a5d0c85ac28718f4f32eadff3ddaf6b474cf75)
- fix(draw) underflow in subpixel font drawing [`6d5ac70`](https://github.com/lvgl/lvgl/commit/6d5ac702ad20ac3092c224ca36e412b0d6cec321)
- fix(scrollbar) hide the scrollbar if the scrollble flag is removed [`188a946`](https://github.com/lvgl/lvgl/commit/188a9467b1bd45d42368a687736a9151d081c1e8)
- fix(color): minor fixes(#2767) [`a4978d0`](https://github.com/lvgl/lvgl/commit/a4978d0913be705caffe3c080524bb7915a5e3e2)
- fix(group) skip object if an of the parents is hidden [`5799c10`](https://github.com/lvgl/lvgl/commit/5799c1084398b365c7a9669406d4fbe258a501ef)
- fix(obj) fix size invalidation issue on padding change [`33ba722`](https://github.com/lvgl/lvgl/commit/33ba7225f55f0cb17f73ce891466c7ebe1327898)
- fix(label) do not bidi process text in lv_label_ins_text [`e95efc1`](https://github.com/lvgl/lvgl/commit/e95efc152f52b7495acb011353a55b3663f7860e)
- fix(refr) set disp_drv-&gt;draw_buf-&gt;flushing_last correctly with sw rotation [`c514bdd`](https://github.com/lvgl/lvgl/commit/c514bddd9b4064e2eba0c3ec4c7a51415acd74e4)
- fix(draw) fix drawing small arcs [`8081599`](https://github.com/lvgl/lvgl/commit/8081599e9b65c758bbdc0168f857515bebaf1c80)
- fix(chart) invalidation with LV_CHART_UPDATE_MODE_SHIFT [`d61617c`](https://github.com/lvgl/lvgl/commit/d61617cd67f792908a1554a44c663c73a41bb357)
- fix(build) fix micropython build error [`54338f6`](https://github.com/lvgl/lvgl/commit/54338f6e57518a59615bdd191fcf5af1365eabea)
- fix(draw) fix border width of simple (radius=0, no masking) borders [`20f1867`](https://github.com/lvgl/lvgl/commit/20f186759664f31f07d6613ea8d77df256cd4597)
- fix(calendar) fix calculation today and highlighted day [`8f0b5ab`](https://github.com/lvgl/lvgl/commit/8f0b5ab0230007fa72127b78db500b9ceb84bf35)
- fix(style) initialize colors to black instead of zero [`524f8dd`](https://github.com/lvgl/lvgl/commit/524f8dd50b4407c78fa6cd947c42e73eab401da1)
- fix(sjpg) remove unnecessary typedefs [`c2d93f7`](https://github.com/lvgl/lvgl/commit/c2d93f78b98ba347001bd29d58b6654492bb8d70)
- fix(label) fix clipped italic letters [`2efa6dc`](https://github.com/lvgl/lvgl/commit/2efa6dce78604cdf422ff233a99f7dd5f06b821c)
- fix(draw) shadow drawing with large shadow width [`f810265`](https://github.com/lvgl/lvgl/commit/f810265c0d91135b71ae110d33d43841ec0e44f8)
- fix(dropdown) add missing invalidations [`33b5d4a`](https://github.com/lvgl/lvgl/commit/33b5d4a4fe6f28962ee7988f74d5ae842dc49b04)
- fix(dropdown) adjust the handling of keys sent to the dropdown [`e41c507`](https://github.com/lvgl/lvgl/commit/e41c50780495c7d6ac6a2b0edf12fc98c9d85a6b)
- fix(disp) be sure the pending scr load animation is finished in lv_scr_load_anim [`eb6ae52`](https://github.com/lvgl/lvgl/commit/eb6ae526432453e4b9dbc7a760cd65d164050548)
- fix(color) fox color premult precision with 16-bit color depth [`f334226`](https://github.com/lvgl/lvgl/commit/f3342269f272c474265700527f52d3ba92111531)
- fix(obj_pos) save x,y even if the object is on a layout [`a9b660c`](https://github.com/lvgl/lvgl/commit/a9b660c278658224f05fbe43d0199c48711db9fd)
- fix(scrollbar) hide the scrollbar if the scrollable flag is removed [`d9c6ad0`](https://github.com/lvgl/lvgl/commit/d9c6ad0425e761d605124e4555adc72854fec4a6)
- fix(dropdown) fix list position with RTL base direction [`79edb37`](https://github.com/lvgl/lvgl/commit/79edb37b0ab5015111bade6074fda81ae101b91b)
- fix(obj) fix lv_obj_align_to with RTL base direction [`531afcc`](https://github.com/lvgl/lvgl/commit/531afcc6cec7f67df06e369a185aef6fdc85af7b)
- fix(chart) fix sending LV_EVENT_DRAW_PART_BEGIN/END for the cursor [`34b8cd9`](https://github.com/lvgl/lvgl/commit/34b8cd9c12604bc1029efa39bd66322b8b771dbe)
- fix(arduino) fix the prototype of my_touchpad_read in the LVGL_Arduino.ino [`1a62f7a`](https://github.com/lvgl/lvgl/commit/1a62f7a619faa93406bc5895ac3338c232de2226)
- fix(checkbox) consider the bg border when positioning the indicator [`a39dac9`](https://github.com/lvgl/lvgl/commit/a39dac9e5c82ecabd135953acafa335941ca0a89)
- fix(dropdown) send LV_EVENT_VALUE_CHANGED to allow styling of the list [`dae7039`](https://github.com/lvgl/lvgl/commit/dae7039803030f908986602b3ce308dc1c3974af)
- fix(group) fix infinite loop [`bdce0bc`](https://github.com/lvgl/lvgl/commit/bdce0bc60cb6e938ce39a0defe5b24249bc66a99)
- fix(keyboard) use LVGL heap functions instead of POSIX [`b20a706`](https://github.com/lvgl/lvgl/commit/b20a706112a3107db13bbd405991ece4cbe00a88)
- fix(blend) fix green channel with additive blending [`78158f0`](https://github.com/lvgl/lvgl/commit/78158f039f19eb17bf1b7c173922c1af26c1e528)
- fix(btnmatrix) do not show pressed, focused or focus key states on disabled buttons [`3df2a74`](https://github.com/lvgl/lvgl/commit/3df2a7444758d2df023f321ccb5931de44af2a48)
- fix(font) handle the last pixel of the glyphs in font loader correctly [`fa98989`](https://github.com/lvgl/lvgl/commit/fa9898941f8efa1966cb6f326d1eebdd31211d04)
- fix(table) fix an off-by-one issue in self size calculation [`ea2545a`](https://github.com/lvgl/lvgl/commit/ea2545ae5dade0845889174737d072137bbb6591)
- fix shadowed variable [`e209260`](https://github.com/lvgl/lvgl/commit/e20926056b28bb64f38abc764a4fca045757e800)
- fix shadowed variable [`df60018`](https://github.com/lvgl/lvgl/commit/df600183f211bde0ff34add973a7a401a1da9af1)
- fix(chart) be sure the chart doesn't remain scrolled out on zoom out [`ad5b1bd`](https://github.com/lvgl/lvgl/commit/ad5b1bdc00a4a44e775a280f8b686353ef4f2a38)
- fix(docs) commit to meta repo as lvgl-bot instead of actual commit author [`f0e8549`](https://github.com/lvgl/lvgl/commit/f0e8549fe14d4e95aedcc98a63acce5a4ad1145b)
- fix(table) invalidate the table on cell value change [`cb3692e`](https://github.com/lvgl/lvgl/commit/cb3692e3029ae452eab04dce21715b7863a9f2a1)
- fix(group) allow refocusing objects [`1520208`](https://github.com/lvgl/lvgl/commit/1520208b14c38713719f507273024624a0f54f1a)
- fix(tabview) fix with left and right tabs [`17c5744`](https://github.com/lvgl/lvgl/commit/17c57449eeae8a693ad5601cf4169cf44d57d5c9)
- fix(msgbox) create modals on top layer instead of act screen [`5cf6303`](https://github.com/lvgl/lvgl/commit/5cf6303e741ec22e2e87f69af4109855eb637e63)
- fix(theme) show disabled state on buttons of btnmatrix, msgbox and keyboard [`0be582b`](https://github.com/lvgl/lvgl/commit/0be582b391e60774d6158411b835b679b010a99b)
- fix(label) update lv_label_get_letter_pos to work with LV_BASE_DIR_AUTO too [`580e05a`](https://github.com/lvgl/lvgl/commit/580e05a0e1531d86d5229ade4ced2c336fbce634)
- fix(label) fix in lv_label_get_letter_pos with when pos==line_start [`58f3f56`](https://github.com/lvgl/lvgl/commit/58f3f5625c2b29278c3e122d8eeba4d9bc597db9)
- fix(gif) replace printf statement with LVGL logging [`56f62b8`](https://github.com/lvgl/lvgl/commit/56f62b8d7356017319d21d44a8f450705ec6486b)
- fix(docs) add fsdrv back [`64527a5`](https://github.com/lvgl/lvgl/commit/64527a5a1ba9d37883c1303a3d4ee1a41e9b4ed3)
- fix(table) remove unnecessary invalidation on pressing [`6f90f9c`](https://github.com/lvgl/lvgl/commit/6f90f9cefba0bc1ea74e737e0e659402f0309cf7)
- fix(chart) draw line chart indicator (bullet) [`fba37a3`](https://github.com/lvgl/lvgl/commit/fba37a30abd1b4d7af78a288fb61dccacc99da08)
- fix(anim) return the first anim if exec_cb is NULL in lv_anim_get() [`fb7ea10`](https://github.com/lvgl/lvgl/commit/fb7ea1040153bd0f2d5c282f9fb31add32c55ce9)
- fix(label) fix lv_label_get_letter_on with BIDI enabled [`192419e`](https://github.com/lvgl/lvgl/commit/192419e7bb300bd64b51d684827719fe1c22cfdb)
- fix(checkbox) add missing invalidations [`bb39e9d`](https://github.com/lvgl/lvgl/commit/bb39e9d6f95235445e3ea1bc52b0d5a1b7a2e24a)
- fix(draw) fix gradient calculation of the rectangle is clipped [`13e3470`](https://github.com/lvgl/lvgl/commit/13e347055bd54c37e7fcb645120ea9ab3134ebec)
- fix(chart) fix typo in 655f42b8 [`6118d63`](https://github.com/lvgl/lvgl/commit/6118d63c2f23e2a157c84a010dcfa0d1fa851382)
- fix(example) fix lv_example_chart_2 [`89081c2`](https://github.com/lvgl/lvgl/commit/89081c2d6ee418b326538e1f39345d43864993c8)
- fix(calendar) fix the position calculation today [`ad05e19`](https://github.com/lvgl/lvgl/commit/ad05e196fb3937ebcba211495013700c0022f777)
- fix(tick) minor optimization on lv_tick_inc call test [`b4305df`](https://github.com/lvgl/lvgl/commit/b4305df5745684a785be071149de8dd342817db4)
- fix(docs) use let instead of const for variable which gets changed [`3cf5751`](https://github.com/lvgl/lvgl/commit/3cf5751461d6a85974da4e5c66593736ae140a1a)
- fix(theme) fix the switch style in the default theme [`0c0dc8e`](https://github.com/lvgl/lvgl/commit/0c0dc8ea30289254732cbba7ada7fd4f092caf22)
- fix(tlsf) undef printf before define-ing it [`cc935b8`](https://github.com/lvgl/lvgl/commit/cc935b87f69e6107d12d9ba4a2c83103f7dd4356)
- fix(msgbox) prevent the buttons being wider than the msgbox [`73e036b`](https://github.com/lvgl/lvgl/commit/73e036bba748e8677f219f573cba5f82c4158a17)
- fix(chart) don't draw series lines with &lt; 1 points [`655f42b`](https://github.com/lvgl/lvgl/commit/655f42b852669f27ab8bfde84bf70cf0b7ea027d)
- fix(tests) remove src/test_runners when cleaning [`6726b0f`](https://github.com/lvgl/lvgl/commit/6726b0f5df3f4689368782b601bb01f76498123b)
- fix(label) remove duplicated lv_obj_refresh_self_size [`a070ecf`](https://github.com/lvgl/lvgl/commit/a070ecfe8c1cf7c07c035ba6c35c3ffaef56d6e1)
- fix(colorwheel) disable LV_OBJ_FLAG_SCROLL_CHAIN by default [`48d1c29`](https://github.com/lvgl/lvgl/commit/48d1c292a3c19380d5669baf911954cc1b083d43)

- fix(obj) do not set the child's position in lv_obj_set_parent [`d89a5fb`](https://github.com/lvgl/lvgl/commit/d89a5fbbd2af33cf759c120e6a14b334099c4c98)
- feat: add LV_USE_MEM_PERF/MONITOR_POS [`acd0f4f`](https://github.com/lvgl/lvgl/commit/acd0f4fbc71ffbfeb382b7af1fa52caf3cdcda6c)
- fix(scroll) in scroll to view functions respect disabled LV_OBJ_FLAG_SCROLLABLE [`9318e02`](https://github.com/lvgl/lvgl/commit/9318e02ef5e29d2f6ce0ab4b2aa67c6542752822)
- fix(flex) remove unused variable [`747b6a2`](https://github.com/lvgl/lvgl/commit/747b6a2a9af9bafe4e6c778cca23e278cb7e4ea4)
- feat(canvas) add lv_canvas_set_px_opa [`b3b3ffc`](https://github.com/lvgl/lvgl/commit/b3b3ffc2b3b322f7401d15c4ba2ef0cdb00e2990)
- fix(textarea) allow using cursor with not full bg_opa [`c9d3965`](https://github.com/lvgl/lvgl/commit/c9d396571d0726aab5d011f37df648d337e5bc12)
- fix(txt) _lv_txt_get_next_line return 0 on empty texts [`82f3fbc`](https://github.com/lvgl/lvgl/commit/82f3fbcad7b710a89b876c32f3583090c99e847c)
- fix(btnmatrix) always update row_cnt [`86012ae`](https://github.com/lvgl/lvgl/commit/86012aefc7197209357290c780029aa39b3738dc)
- fix(scroll) minor fixes on obj scroll handling [`a4128a8`](https://github.com/lvgl/lvgl/commit/a4128a83562e0daacd949333ba7cbfec650f8050)
- fix(table) consider border width for cell positions [`f2987b6`](https://github.com/lvgl/lvgl/commit/f2987b6591046f1384b0089187fd81da10834021)
- fix(log) be sure LV_LOG_... is not empty if logs are disabled [`47734c4`](https://github.com/lvgl/lvgl/commit/47734c4abedf6b6005069d15a8c4c2fcff73f85e)
- fix(arc) fix LV_ARC_MODE_REVERSE [`df3b969`](https://github.com/lvgl/lvgl/commit/df3b96900b1266ed4856438d9121e39905d510bb)
- fix(obj) in lv_obj_move_to_index() do not send LV_EVENT_CHILD_CHANGED on all changed child [`32e8276`](https://github.com/lvgl/lvgl/commit/32e8276db7403d8dc9c9b9f0c77d331049e8c07d)
- feat(event) add lv_obj_remove_event_cb_with_user_data [`4eddeb3`](https://github.com/lvgl/lvgl/commit/4eddeb35abee1f9cd2d1fd210f11cc096cb609c7)
- fix(draw) fix shadow drawing with radius=0 [`4250e3c`](https://github.com/lvgl/lvgl/commit/4250e3c62737697cd8bc78d991a3d66216efa437)
- fix(msgbox) directly store the pointer of all children [`eb5eaa3`](https://github.com/lvgl/lvgl/commit/eb5eaa39406473cd90a7f78d710ce950cbf47548)
- fix(draw) use the filtered colors in lv_obj_init_draw_xxx_dsc() functions [`78725f2`](https://github.com/lvgl/lvgl/commit/78725f23da24fe22543ab3388c87bf3cfbd0e51a)
- fix(arc) fix full arc invalidation [`98b9ce5`](https://github.com/lvgl/lvgl/commit/98b9ce599751c9de0421acd419430cc6ccd7cad9)
- chore(led) expose LV_LED_BRIGHT_MIN/MAX in led.h [`3f18b23`](https://github.com/lvgl/lvgl/commit/3f18b234f601edefb16b1ffdb0c539e823b1c025)
- fix(group) keep the focused object in lv_group_swap_obj [`a997147`](https://github.com/lvgl/lvgl/commit/a9971471ba34352a1d7b307977cb2f635b28a031)
- fix(obj) swap objects in the group too in  lv_obj_swap() [`52c7558`](https://github.com/lvgl/lvgl/commit/52c7558ab46a7024e05499edb483f115b13086f0)
- fix(theme) use opacity on button's shadow in the default theme [`c5342e9`](https://github.com/lvgl/lvgl/commit/c5342e9324c492c70b65f8c228d44b7a290cf110)
- fix(win) enable  clip_corner and border_post by default [`493ace3`](https://github.com/lvgl/lvgl/commit/493ace352fea0eaa37abccaa0938c0c4a12a995a)
- fix(draw) fix rectangle drawing with clip_corner enabled [`01237da`](https://github.com/lvgl/lvgl/commit/01237da474b9703fb544163db5f66645c2b6935c)
- fix(arc) fix other invalidation issues [`b0a7337`](https://github.com/lvgl/lvgl/commit/b0a733766daee1edfabaec8df4a5fedd0180ccaf)
- feat(obj) add lv_obj_get_x/y_aligned [`98bc1fe`](https://github.com/lvgl/lvgl/commit/98bc1fe09e12a64333e91b4c25327c283a700af5)
- fix(calendar) fix incorrect highlight of today [`adbac52`](https://github.com/lvgl/lvgl/commit/adbac5220b2d75f08de110b3f426066e24f46998)
- fix(arc, meter) fix invalidation in special cases [`0f14f49`](https://github.com/lvgl/lvgl/commit/0f14f49465ca701c98f76ac95bda4a537c0fadfa)
- fix(canvas) invalidate the image on delete [`a1b362c`](https://github.com/lvgl/lvgl/commit/a1b362c98622ecbc063cfb17fb091fdab4522e8a)
- fix(msgbox) return the correct pointer from lv_msgbox_get_text [`50ea6fb`](https://github.com/lvgl/lvgl/commit/50ea6fb3fefb3a6edc958154c575dcdcacbfdb3a)
- fix(bidi) fix the handling of LV_BASE_DIR_AUTO in several widgets [`7672847`](https://github.com/lvgl/lvgl/commit/7672847ce325e909981582b4153993025da7fe50)
- fix(build) remove main component dependency (#2420) [`f2c2393`](https://github.com/lvgl/lvgl/commit/f2c2393b305cd71d2fc01ff8945965dccb8488b4)
- fix(meter) fix inner mask usage [`c28c146`](https://github.com/lvgl/lvgl/commit/c28c14631040fd08da122e192458cb0c65bc9faf)
- fix(log) fix warning for empty log macros [`4dba8df`](https://github.com/lvgl/lvgl/commit/4dba8df2a196fc7a2b7a8686efb6e47fc6cf0fc6)
- fix(theme) improve button focus of keyboard [`2504b7e`](https://github.com/lvgl/lvgl/commit/2504b7e4361ad8009e005faf112987585c2e8356)
- fix(tabview) send LV_EVENT_VALUE_CHANGED only once [`933d282`](https://github.com/lvgl/lvgl/commit/933d2829aca8bc269c0b481f2a535274626374bc)
- fix(obj style) fix children reposition if the parent's padding changes. [`57cf661`](https://github.com/lvgl/lvgl/commit/57cf6610a9ec2e6458035abfdaa5554f4296c89c)
- fix(template) update indev template for v8 [`d8a3d3d`](https://github.com/lvgl/lvgl/commit/d8a3d3d0d759ad0145f134a3f08433f3fdffcb75)
- fix(obj) detecting which indev sent LV_EVENT_FOCUS [`f03d4b8`](https://github.com/lvgl/lvgl/commit/f03d4b8cb9928077a04b839db0bd5c625919d903)
- fix(roller) adjust the size of the selected area correctly [`01d1c87`](https://github.com/lvgl/lvgl/commit/01d1c873e19d0d77e1444ba79468db63f26a448a)
- fix(imgbtn) consider width==LV_SIZE_CONTENT if only mid. img is set [`7e49f48`](https://github.com/lvgl/lvgl/commit/7e49f48894c5c3eb9793dbf1c8630f3cfdc3c091)
- fix(flex) fix NULL pointer dereference [`97ba12f`](https://github.com/lvgl/lvgl/commit/97ba12f280f0fa5400ff18c5317b9736063d8391)
- fix(obj, switch) do not send LV_EVENT_VALUE_CHANGED twice [`713b39e`](https://github.com/lvgl/lvgl/commit/713b39ecdb7e8e219cc295bad7d953ff2136f138)
- fix(coords) fix using large coordinates [`428db94`](https://github.com/lvgl/lvgl/commit/428db9494dc43d65026a9c1fb42c50daede82fc2)
- fix(chart) fix crash if no series are added [`c728b5c`](https://github.com/lvgl/lvgl/commit/c728b5ceda0a5a93d5a0859eb88261db582cf1eb)
- fix(meter) fix needle image invalidation [`54d8e81`](https://github.com/lvgl/lvgl/commit/54d8e8170bd4964909cee15a256408e7f08ccf21)
- fix(mem) add lv_ prefix to tlsf functions and types [`0d52b59`](https://github.com/lvgl/lvgl/commit/0d52b59cb16dda377f8a1ac581a851b830b7bf53)
- fix(pxp) change LV_COLOR_TRANSP to LV_COLOR_CHROMA_KEY to v8 compatibility [`81f3068`](https://github.com/lvgl/lvgl/commit/81f3068dd77d47e7079e6697ea5d00f69202c1bd)

### Examples

- example(chart)  add area chart example [`2507`](https://github.com/lvgl/lvgl/pull/2507)
- example(anim) add demo to use cubic-bezier [`2393`](https://github.com/lvgl/lvgl/pull/2393)
- feat(example) add lv_example_chart_9.py [`2604`](https://github.com/lvgl/lvgl/pull/2604)
- feat(example) add lv_example_chart_8.py [`2611`](https://github.com/lvgl/lvgl/pull/2611)
- feat(example) chart example to add gap between the old and new data  [`2565`](https://github.com/lvgl/lvgl/pull/2565)
- feat(example) add lv example list 2 [`2545`](https://github.com/lvgl/lvgl/pull/2545)
- feat(examples) add MicroPython version of lv_example_anim_3 and allow loading roller font dynamically [`2412`](https://github.com/lvgl/lvgl/pull/2412)
- feat(examples) added MP version of second tabview example [`2347`](https://github.com/lvgl/lvgl/pull/2347)
- fix(example):format codes [`2731`](https://github.com/lvgl/lvgl/pull/2731)
- fix(example) minor fixes in lv_example_chart_2.py [`2601`](https://github.com/lvgl/lvgl/pull/2601)
- feat(example) add text with gradient example [`462fbcb`](https://github.com/lvgl/lvgl/commit/462fbcbf49f47b9f329b6c15d2ca04ef09806cd9)
- fix(example_roller_3) mask free param bug [`2553`](https://github.com/lvgl/lvgl/pull/2553)
- fix(examples) don't compile assets unless needed [`2523`](https://github.com/lvgl/lvgl/pull/2523)
- fix(example) scroll example sqort types [`2498`](https://github.com/lvgl/lvgl/pull/2498)
- fix(examples) join usage [`2425`](https://github.com/lvgl/lvgl/pull/2425)
- fix(examples) add missing lv.PART.INDICATOR [`2423`](https://github.com/lvgl/lvgl/pull/2423)
- fix(examples) use lv.grid_fr for MicroPython [`2419`](https://github.com/lvgl/lvgl/pull/2419)
- fix(examples) remove symlinks [`2406`](https://github.com/lvgl/lvgl/pull/2406)
- fix(examples) import 'u'-prefixed versions of modules [`2365`](https://github.com/lvgl/lvgl/pull/2365)
- fix(examples) remove cast in MP scripts [`2354`](https://github.com/lvgl/lvgl/pull/2354)
- fix(examples) fix MicroPython examples and run the examples with CI [`2339`](https://github.com/lvgl/lvgl/pull/2339)
- fix(examples) align with renamed Micropython APIs [`2338`](https://github.com/lvgl/lvgl/pull/2338)

- fix(examples) adjust canvas example for MicroPython API change [`52d1c2e`](https://github.com/lvgl/lvgl/commit/52d1c2e5b53eda4270abc0caa0eb309b35c010c8)
- fix(example) revert test code [`77e2c1f`](https://github.com/lvgl/lvgl/commit/77e2c1ff3d3ff035a3613f2ed0e5538513e8b4a1)
- feat(example) add checkbox example for radio buttons [`d089b36`](https://github.com/lvgl/lvgl/commit/d089b364e700d1216813106f7b4dfa6cee9aa806)
- feat(example) add text with gradient example [`462fbcb`](https://github.com/lvgl/lvgl/commit/462fbcbf49f47b9f329b6c15d2ca04ef09806cd9)
- fix(examples) exclude example animimg images if animimg is disabled [`4d7d306`](https://github.com/lvgl/lvgl/commit/4d7d30677af9ef158fe51fb1d8900d234ea5e181)
- fix(example) adjust the object sizes in lv_example_anim_timeline_1() [`71a10e4`](https://github.com/lvgl/lvgl/commit/71a10e4ecd4acfddcea279a0b5da219dfb002ff7)
- fix(example) revert text code from lv_example_checkbox_2 [`28e9593`](https://github.com/lvgl/lvgl/commit/28e9593e5802a2e7d493515059c6327e60ccbf28)


### Docs

- docs: fix typo [`2765`](https://github.com/lvgl/lvgl/pull/2765)
- docs(colorwheel) fix old API names [`2643`](https://github.com/lvgl/lvgl/pull/2643)
- docs(display) fix typo [`2624`](https://github.com/lvgl/lvgl/pull/2624)
- docs add static for lv_indev_drv_t [`2605`](https://github.com/lvgl/lvgl/pull/2605)
- docs(animimg) add to extra widgets index and fix example [`2610`](https://github.com/lvgl/lvgl/pull/2610)
- docs(animimg) Add missing animation image page [`2609`](https://github.com/lvgl/lvgl/pull/2609)
- docs(group) remove reference to lv_cont which is gone in v8 [`2580`](https://github.com/lvgl/lvgl/pull/2580)
- docs(style) use correct API name for local styles [`2550`](https://github.com/lvgl/lvgl/pull/2550)
- docs(all) Proofread, fix typos and add clarifications in confusing areas [`2528`](https://github.com/lvgl/lvgl/pull/2528)
- docs(flex) update flex.md [`2517`](https://github.com/lvgl/lvgl/pull/2517)
- docs more spelling fixes [`2499`](https://github.com/lvgl/lvgl/pull/2499)
- docs fix typo: arae -&gt; area [`2488`](https://github.com/lvgl/lvgl/pull/2488)
- docs(readme) fix typo: hosing → hosting. [`2477`](https://github.com/lvgl/lvgl/pull/2477)
- docs update company name and year [`2476`](https://github.com/lvgl/lvgl/pull/2476)
- docs fix typos [`2472`](https://github.com/lvgl/lvgl/pull/2472)
- docs(overview) fix typo [`2465`](https://github.com/lvgl/lvgl/pull/2465)
- docs(bar) fix typos in widget examples [`2463`](https://github.com/lvgl/lvgl/pull/2463)
- docs(overview) fix typo [`2454`](https://github.com/lvgl/lvgl/pull/2454)
- docs(chart) typos [`2427`](https://github.com/lvgl/lvgl/pull/2427)
- docs(layout) add internal padding paragraph to grid and flex layout p… [`2392`](https://github.com/lvgl/lvgl/pull/2392)
- docs(porting) fix indev example to remove v7 bool return [`2381`](https://github.com/lvgl/lvgl/pull/2381)
- docs(README) fix broken references [`2329`](https://github.com/lvgl/lvgl/pull/2329)
- docs(grid) typo fix [`2310`](https://github.com/lvgl/lvgl/pull/2310)
- docs(color) language fixes [`2302`](https://github.com/lvgl/lvgl/pull/2302)
- docs(lv_obj_style) update add_style and remove_style function headers [`2287`](https://github.com/lvgl/lvgl/pull/2287)

- docs(contributing) add commit message format section [`3668e54`](https://github.com/lvgl/lvgl/commit/3668e54f06b9e51f407b6f6eb24829c03e3d0ac5)
- docs minor typo fixes [`84c0086`](https://github.com/lvgl/lvgl/commit/84c00862ae0213a54469e08900da7acf435ed5fe)
- docs(arduino) update some outdated information [`9a77102`](https://github.com/lvgl/lvgl/commit/9a77102c40f68140d0ba2c6c5e493e51a8773f64)
- docs(keyboard) add note regarding event handler [`255f729`](https://github.com/lvgl/lvgl/commit/255f7294d387d65bbc56c0f8af84f7fa2f3cfdfa)
- docs minor CSS fix [`acbb680`](https://github.com/lvgl/lvgl/commit/acbb680683fc726e942f59d4296501838e90bde1)
- docs minor CSS improvements [`7f367d6`](https://github.com/lvgl/lvgl/commit/7f367d6956c4d87b75a90cf1798550e986c5c248)
- docs(keyboard) change `LV_KEYBOARD_MODE_NUM` to `LV_KEYBOARD_MODE_NUMBER` [`6e83d37`](https://github.com/lvgl/lvgl/commit/6e83d378e933c426550a7d6bc8fd0dd7fa9ba051)
- docs(textarea) clarify the use of text selection bg_color [`65673c0`](https://github.com/lvgl/lvgl/commit/65673c0e15c48b5926da26ae1a1b8d0a0a8161a3)
- docs list all examples on one page [`25acaf4`](https://github.com/lvgl/lvgl/commit/25acaf45ca87271106b23b52d0d941228e117859)
- docs(examples) add MicroPython examples [`6f37c4f`](https://github.com/lvgl/lvgl/commit/6f37c4fc560c13545177e15576c5b3085c8f2c2a)
- docs(filesystem) update to v8 [`7971ade`](https://github.com/lvgl/lvgl/commit/7971ade47b15898efb6fca17d34ca30f1ee5c926)
- docs(style) complete the description of style the properties [`55e8846`](https://github.com/lvgl/lvgl/commit/55e8846871f812f888c8354e4ec8974ac0650165)
- docs example list fixes [`cd600d1`](https://github.com/lvgl/lvgl/commit/cd600d105650bae08f9732a654c6a2c85e610cd5)
- docs(style) complete the description of style the properties [`ff087da`](https://github.com/lvgl/lvgl/commit/ff087dafb4ecd016ee4920bfe4f162b1db58f7cb)
- docs(README) update links, examples, and add services menu [`3471bd1`](https://github.com/lvgl/lvgl/commit/3471bd1c698ee58f6632415559dcc34e9d2ee3c0)
- docs(color) update colors' docs [`9056b5e`](https://github.com/lvgl/lvgl/commit/9056b5ee1bfea6796307bdf983a4a00ea47fe9f0)
- docs update lv_fs.h, layer and align.png to v8 [`31ab062`](https://github.com/lvgl/lvgl/commit/31ab0628d5cfc57e55f42e5f59689388b034177c)
- docs(color) minor fix [`ac8f453`](https://github.com/lvgl/lvgl/commit/ac8f4534a51b418377c2eac62dbd731b9be71977)
- docs update changelog [`c386110`](https://github.com/lvgl/lvgl/commit/c386110e2390399ab97936622e59c510ba414e19)
- docs(extra) add extra/README.md [`8cd504d`](https://github.com/lvgl/lvgl/commit/8cd504d58bb679fe1f260e3eee59fcb0b85cb589)
- docs add lazy load to the iframes of the examples [`c49e830`](https://github.com/lvgl/lvgl/commit/c49e830aad2c847611f3398767e85c193909559a)
- docs(os) add example and clarify some points [`d996453`](https://github.com/lvgl/lvgl/commit/d996453207caa50a90a66d05565431fa288be96b)
- docs(rlottie) fix build error [`ce0b564`](https://github.com/lvgl/lvgl/commit/ce0b56458846daa65288f901e9b8ef1083eab468)
- docs include paths in libs [`f5f9562`](https://github.com/lvgl/lvgl/commit/f5f956233657f95b45a45d872e5d6e68c05eecd4)
- docs libs fixes [`8e7bba6`](https://github.com/lvgl/lvgl/commit/8e7bba6acec66a4f6b80496de9fd21a8e3c4c6ee)
- docs(obj) add comment lv_obj_get_x/y/width/height about postponed layout recalculation [`533066e`](https://github.com/lvgl/lvgl/commit/533066e6accbe2cbe1b60556eb61ebb2a07185a2)
- docs fix example list [`ed77ed1`](https://github.com/lvgl/lvgl/commit/ed77ed1dae088ef29194cf3c6bb552e1ee67d78b)
- docs describe the options to include or skip lv_conf.h [`174ef66`](https://github.com/lvgl/lvgl/commit/174ef6692e0b05338890a1cf524d9dcbf5c25f6c)
- docs(overview) spelling fixes [`d2efb8c`](https://github.com/lvgl/lvgl/commit/d2efb8c6e5ceedbb9d9c1a1c89ef709e6570e360)
- docs(table) describe keypad/encoder navigation [`749d1b3`](https://github.com/lvgl/lvgl/commit/749d1b3ec31ec2ef27f594ed0a4af93edb2c10f0)
- docs update CHANGELOG [`0f8bc18`](https://github.com/lvgl/lvgl/commit/0f8bc18f6aacb6a74e0bda59068d3d178fa66434)
- docs(image) mention the frame_id parameter of lv_img_decoder_open [`2433732`](https://github.com/lvgl/lvgl/commit/2433732570a817f566308e025d89227a8c650f5f)
- docs(arduino) update how to use the examples  [`06962a5`](https://github.com/lvgl/lvgl/commit/06962a564fd668eced22b2e9bc19e7732abf94ec)
- docs(rlottie): fix typo in commands [`ed9169c`](https://github.com/lvgl/lvgl/commit/ed9169c56dc1f34b1f021457b78c9f3eccba13cf)
- docs(indev, layer) update lv_obj_set_click() to lv_obj_add_flag() [`bcd99e8`](https://github.com/lvgl/lvgl/commit/bcd99e8e438cc1b63762f8933d26bbb38fd42a2d)
- docs update version support table [`e6e98ab`](https://github.com/lvgl/lvgl/commit/e6e98abbc25cc4aa20b05d1002a651e4012ebff7)
- docs fix example list [`c6f99ad`](https://github.com/lvgl/lvgl/commit/c6f99ad200c7862c2f3cca3811bc2bdc2c95e971)
- docs(examples) add &lt;hr/&gt; to better separate examples [`a1b59e3`](https://github.com/lvgl/lvgl/commit/a1b59e34dd23fb12bd6e9ab0ffa92b2bfcec66b3)
- docs(checkbox) update the comment lv_checkbox_set_text_static [`3e0ddd0`](https://github.com/lvgl/lvgl/commit/3e0ddd028511c6c4a0ba33a15526f404b31a50b8)
- docs(grid) fix missing article [`da0c97a`](https://github.com/lvgl/lvgl/commit/da0c97a367746573fa2385d0ddd184f27ca20dbd)
- docs(display) fix grammar in one spot [`5dbea7d`](https://github.com/lvgl/lvgl/commit/5dbea7d72522e78f66fb468e1d5a98fa28179ed1)
- docs(style) fix typo in style property descriptions [`4e3b860`](https://github.com/lvgl/lvgl/commit/4e3b86020fdc8e183335c6c9b8604129e3e3ddcc)
- docs(flex) fix typo in flex grow section [`e5fafc4`](https://github.com/lvgl/lvgl/commit/e5fafc412214ab01d46ebd37e272e3ffc3164ea4)
- docs(indev) clarify purpose of `continue_reading` flag [`706f81e`](https://github.com/lvgl/lvgl/commit/706f81e5862af27fb0b60cdaf02c650c31787c78)
- docs(license) update company name and year [`7c1eb00`](https://github.com/lvgl/lvgl/commit/7c1eb0064535f2d914b9dc885ebb2a2d0d73381d)
- docs fix typo [`8ab8064`](https://github.com/lvgl/lvgl/commit/8ab806459c1b99990b91b4cd6a656ff6736c1b63)
- docs add libs to the main index [`1a8fed5`](https://github.com/lvgl/lvgl/commit/1a8fed5df02545fe97845e3acd86e33f7048cd8e)
- docs add btn_example.png [`8731ef1`](https://github.com/lvgl/lvgl/commit/8731ef141e2ad2f022b1c01e1bf7605f983b013f)

- docs(btnmatrix) fix typo with set_all/clear_all parameters [`51a82a1`](https://github.com/lvgl/lvgl/commit/51a82a17ffe938d07d94660f49fd18962060943a)

### CI and tests

- ci(micropython) fix git fetch [`2757`](https://github.com/lvgl/lvgl/pull/2757)
- test(txt) initial unit tests and general code cleanup/fixes [`2623`](https://github.com/lvgl/lvgl/pull/2623)
- test add setUp and tearDown to test template [`2648`](https://github.com/lvgl/lvgl/pull/2648)
- test(arc) add initial unit tests [`2617`](https://github.com/lvgl/lvgl/pull/2617)
- ci(micropython) add ESP32 and STM32 tests [`2629`](https://github.com/lvgl/lvgl/pull/2629)
- test(checkbox) add initial tests [`2551`](https://github.com/lvgl/lvgl/pull/2551)
- test(ci) build and run tests in parallel. [`2515`](https://github.com/lvgl/lvgl/pull/2515)
- ci(tests) run tests using ctest [`2503`](https://github.com/lvgl/lvgl/pull/2503)
- ci(tests) add dependency on GNU parallel [`2510`](https://github.com/lvgl/lvgl/pull/2510)
- ci(tests) use common script to install development prereqs [`2504`](https://github.com/lvgl/lvgl/pull/2504)
- test convert Makefile to CMake [`2495`](https://github.com/lvgl/lvgl/pull/2495)
- test Refactor unit test scripts. [`2473`](https://github.com/lvgl/lvgl/pull/2473)

- test(font_loader) migrate the existing font loader test [`bc5b3be`](https://github.com/lvgl/lvgl/commit/bc5b3be61f7751852dc99509a6ab83faaf6d1235)
- test add build test again, add dropdown test, integrate gcov and gvocr [`e35b1d0`](https://github.com/lvgl/lvgl/commit/e35b1d04bdc7d531d72ebce7d1f031be2631e776)
- test(dropdown) add tess for keypad and encoder [`4143b80`](https://github.com/lvgl/lvgl/commit/4143b804c8f4b4324141ad0f529bac4e9acf1442)
- test add keypad and encoder emulators [`e536bb6`](https://github.com/lvgl/lvgl/commit/e536bb6325728db21ef5c729a99f2161a8125625)
- tests add mouse emulator [`2ba810b`](https://github.com/lvgl/lvgl/commit/2ba810b8de19afc3e9ac18e5bd8ab16af10a4433)
- tests add README [`b765643`](https://github.com/lvgl/lvgl/commit/b765643e4902de359e88fdf6d314e9afdb2daa9a)
- test add move tests to test_cases and test_runners directories [`e9e010a`](https://github.com/lvgl/lvgl/commit/e9e010a8468ee307c350e071251f22459173e601)
- test fix CI build error [`c38cae2`](https://github.com/lvgl/lvgl/commit/c38cae22fbf6cef7564fbebe2145a7def20d52e1)
- ci add config for 8bpp [`3eacc59`](https://github.com/lvgl/lvgl/commit/3eacc5923c0a554e7ff4489776a8982dfc142115)
- test move more source files to src folder [`3672f87`](https://github.com/lvgl/lvgl/commit/3672f873328b4471ac9d5d23696f7bc99a87bc43)
- test update CI for the new tests [`a3898b9`](https://github.com/lvgl/lvgl/commit/a3898b931e81860acf197bc88fd3dd6f8885eb2c)
- test cleaned up report folder [`b9b4ba5`](https://github.com/lvgl/lvgl/commit/b9b4ba5b2608f5709678463f62b3d3f937780235)
- test fix build error [`61cda59`](https://github.com/lvgl/lvgl/commit/61cda59cbe8569326ef9d366c520b89be292f5ea)
- test(font_loader) migrate the existing font loader test [`d6dbbaa`](https://github.com/lvgl/lvgl/commit/d6dbbaaa34304b4c889415439ab562056e0840a5)
- test add move tests to test_cases and test_runners directories [`d2e735e`](https://github.com/lvgl/lvgl/commit/d2e735ef36bd99c16ccaa281dcaa5f418e2dec98)
- test add 3rd party libs to all tests and also fix them [`7a95fa9`](https://github.com/lvgl/lvgl/commit/7a95fa9e2de9639a3c2f1990ff63b467be54a7aa)
- test(arc): add test case for adv_hittest [`e83df6f`](https://github.com/lvgl/lvgl/commit/e83df6f14de1a9eb1d137b123fac96c25a1b7715)
- ci create check for lv_conf_internal.h [`5d8285e`](https://github.com/lvgl/lvgl/commit/5d8285e2d37e19670c1daeff229e1dc331f053c4)
- test fix warning and docs build error [`d908f31`](https://github.com/lvgl/lvgl/commit/d908f31f8f50024d8b3c8d0a11aff9cc1b011049)
- ci(micropython) add rp2 port [`1ab5c96`](https://github.com/lvgl/lvgl/commit/1ab5c9689f61fd2991653beec7d023472fc96239)
- test(dropdown) remove dummy test case [`9fb98da`](https://github.com/lvgl/lvgl/commit/9fb98da8a280dc3d5753da1d2aa79eeb1cba47e0)
- ci(codecov) hide statuses on commits for now [`0b7be77`](https://github.com/lvgl/lvgl/commit/0b7be778a29412fe5562a736855121d19350889c)
- ci(docs) run apt-get update before installation [`f215174`](https://github.com/lvgl/lvgl/commit/f215174999a18b0e5904e97bfda48f3b81271aa1)
- test fix LV_USE_LOG_LEVEL -&gt; LV_LOG_LEVEL typo [`80f0b09`](https://github.com/lvgl/lvgl/commit/80f0b09e34596564ca6ec7c23d148f4ce2e17ca3)
- ci(micropython) add GCC problem matcher [`ab316a0`](https://github.com/lvgl/lvgl/commit/ab316a07bc4d89a633fdd00bc7ff8c5db4b00ad8)

- test convert Makefile to CMake (#2495) [`9c846ee`](https://github.com/lvgl/lvgl/commit/9c846ee493862ef11b46942a6e5af3c1ed8468d1)

### Others

- chore: replace (void)xxx with LV_UNUSED(xxx) [`2779`](https://github.com/lvgl/lvgl/pull/2779)
- animation improvement [`2743`](https://github.com/lvgl/lvgl/pull/2743)
- Improve LV_FORMAT_ATTRIBUTE usage [`2673`](https://github.com/lvgl/lvgl/pull/2673)
- Fix typo in commands to build rlottie [`2723`](https://github.com/lvgl/lvgl/pull/2723)
- del(.gitmodules): delete .gitmodules [`2718`](https://github.com/lvgl/lvgl/pull/2718)
- lv_obj_draw_part_dsc_t.text_length added [`2694`](https://github.com/lvgl/lvgl/pull/2694)
- expose LV_COLOR_DEPTH and LV_COLOR_16_SWAP in micropython [`2679`](https://github.com/lvgl/lvgl/pull/2679)
- sync lvgl/lv_fs_if [`2676`](https://github.com/lvgl/lvgl/pull/2676)
- build: always enable CMake install rule in default configuration [`2636`](https://github.com/lvgl/lvgl/pull/2636)
- build: fix lib name in CMakeLists [`2641`](https://github.com/lvgl/lvgl/pull/2641)
- build: remove use of 'project' keyword in CMakeLists [`2640`](https://github.com/lvgl/lvgl/pull/2640)
- build add install rule to CMakeList.txt [`2621`](https://github.com/lvgl/lvgl/pull/2621)
- Fixed row size calculation [`2633`](https://github.com/lvgl/lvgl/pull/2633)
- arch add small 3rd party libs to lvgl  [`2569`](https://github.com/lvgl/lvgl/pull/2569)
- Kconfig: Add missing options [`2597`](https://github.com/lvgl/lvgl/pull/2597)
- Espressif IDF component manager [`2521`](https://github.com/lvgl/lvgl/pull/2521)
- chore(btnmatrix) removed unnecessary semicolon [`2520`](https://github.com/lvgl/lvgl/pull/2520)
- Update README.md [`2516`](https://github.com/lvgl/lvgl/pull/2516)
- Corrected a function name in obj.md [`2511`](https://github.com/lvgl/lvgl/pull/2511)
- Simple spelling fixes [`2496`](https://github.com/lvgl/lvgl/pull/2496)
- added lv_obj_move_up() and lv_obj_move_down() [`2467`](https://github.com/lvgl/lvgl/pull/2467)
- Fix buf name error for "lv_port_disp_template.c" and optimize the arduino example  [`2475`](https://github.com/lvgl/lvgl/pull/2475)
- Fix two examples in the docs with new v8 api [`2486`](https://github.com/lvgl/lvgl/pull/2486)
- kconfig: minor fix for default dark theme option [`2426`](https://github.com/lvgl/lvgl/pull/2426)
- doc(table) update doc on cell merging [`2397`](https://github.com/lvgl/lvgl/pull/2397)
- added example lv_example_anim_timeline_1.py [`2387`](https://github.com/lvgl/lvgl/pull/2387)
- refactor(printf) add printf-like function attribute to _lv_txt_set_text_vfmt and lv_label_set_text_fmt [`2332`](https://github.com/lvgl/lvgl/pull/2332)
- Update win.md [`2352`](https://github.com/lvgl/lvgl/pull/2352)
- Nxp pxp vglite v8 dev [`2313`](https://github.com/lvgl/lvgl/pull/2313)
- More Snapable --&gt; Snappable replacements [`2304`](https://github.com/lvgl/lvgl/pull/2304)
- Spelling and other language fixes to documentation [`2293`](https://github.com/lvgl/lvgl/pull/2293)
- Update quick-overview.md [`2295`](https://github.com/lvgl/lvgl/pull/2295)
- adding micropython examples [`2286`](https://github.com/lvgl/lvgl/pull/2286)

- format run code-formtter.sh [`d67dd94`](https://github.com/lvgl/lvgl/commit/d67dd943cadb3d21a3d9488b6354f669e2e58c65)
- Update ROADMAP.md [`2b1ae3c`](https://github.com/lvgl/lvgl/commit/2b1ae3c107539dec130b988cddca5ddb2b5af652)
- Create .codecov.yml [`e53aa82`](https://github.com/lvgl/lvgl/commit/e53aa82658a1d7324f328c986cb5b7b669803ba2)
- refactor(examples) drop JS-specific code from header.py [`ef41450`](https://github.com/lvgl/lvgl/commit/ef41450ed87f4f4dd936b63349b5a0c9ce880618)
- make test run on master and release/v8.* [`227402a`](https://github.com/lvgl/lvgl/commit/227402a81a1cdd34cd57ec04682166d3961c4481)
- Update release.yml [`0838f12`](https://github.com/lvgl/lvgl/commit/0838f1296b2c55c0b265650ee4310a79730536dd)
- refactor(examples) drop usys import from header.py [`ad1f91a`](https://github.com/lvgl/lvgl/commit/ad1f91ab32c38cab7f0d1448ce3c4e67b47f4526)
- Update ROADMAP.md [`a38fcf2`](https://github.com/lvgl/lvgl/commit/a38fcf2c7aa5fd156d3f2b6965ec4f81d7ff5503)
- Revert "feat(conf) add better check for Kconfig default" [`a5793c7`](https://github.com/lvgl/lvgl/commit/a5793c70a9a60340a5f1c5d33ba1d118af0a76e2)
- remove temporary test file [`a958c29`](https://github.com/lvgl/lvgl/commit/a958c29af7df66f84520036766929232e0c437c4)
- start to implement release/patch [`1626a0c`](https://github.com/lvgl/lvgl/commit/1626a0c029504f26e568677debcb7ab0f6053f83)
- chore(indev) minor formatting [`79ab3d2`](https://github.com/lvgl/lvgl/commit/79ab3d29b01e5f0bff1c754fdc36230584aeaaae)
- add basic patch release script [`1c3ecf1`](https://github.com/lvgl/lvgl/commit/1c3ecf1cc14f5501a345472278cc485a24b8ab9c)
- chore(example) minor improvements on lv_example_list_2 [`bb6d6b7`](https://github.com/lvgl/lvgl/commit/bb6d6b77999fde33f560bde92b394a8811303868)
- tool: add changelog_gen.sh to automatically generate changelog [`6d95521`](https://github.com/lvgl/lvgl/commit/6d955210765de972f78b8c307df2f2387e4580ed)
- update version numbers to v8.1.0-dev [`8691611`](https://github.com/lvgl/lvgl/commit/8691611de2206669cd22e3e97c844fdf2bf494b0)
- chore(test) improve prints [`ea8bed3`](https://github.com/lvgl/lvgl/commit/ea8bed34b49343a4e881bdd42096f69d245ef66e)
- chore(test) improve prints [`0c4bca0`](https://github.com/lvgl/lvgl/commit/0c4bca0f9cbeefaf20fd41e3a561d0e1799bc6b0)
- chore: update lv_conf_internal.h [`41c2dd1`](https://github.com/lvgl/lvgl/commit/41c2dd16ee87f85338603399bb92e1f6eab84bf6)
- chore(format) lv_conf_template.h minor formatting [`3c86d77`](https://github.com/lvgl/lvgl/commit/3c86d777c10c80ec9a4c5d3d403bd1395834004a)
- chore(docs) always deploy master to docs/master as well [`6d05692`](https://github.com/lvgl/lvgl/commit/6d05692d7820a2b833751d6881704b283f1fe618)
- Update CHANGELOG.md [`48fd73d`](https://github.com/lvgl/lvgl/commit/48fd73d20da4f19556660a9fca7faf042c965f56)
- Fix compile errors [`6c956cc`](https://github.com/lvgl/lvgl/commit/6c956cc0f402b96512ed07f8a93003a0319fc49c)
- Update textarea.md [`6d8799f`](https://github.com/lvgl/lvgl/commit/6d8799fbbfb1477ad2e0887644fb4cd900817199)
- chore(assert) add warning about higher memory usage if LV_USE_ASSERT_STYLE is enabled [`33e4330`](https://github.com/lvgl/lvgl/commit/33e433008e23b48540e83bc5399fd0ccb9e90630)
- Update page.html [`9573bab`](https://github.com/lvgl/lvgl/commit/9573bab5cbe2da643f5146e62c176bdd0113d954)
- chore(docs) force docs rebuild [`4a0f413`](https://github.com/lvgl/lvgl/commit/4a0f4139eb98e73b37abf62f66e2cf1c5d4e58db)
- Fix typo error in color.md [`572880c`](https://github.com/lvgl/lvgl/commit/572880ccd3374ccbe81cf09a0620bf95659ca883)
- Update arc.md [`2a9b9e6`](https://github.com/lvgl/lvgl/commit/2a9b9e6e1119db8294fdc63d93548fe06e2b6aa2)
- Update index.rst [`9ce2c77`](https://github.com/lvgl/lvgl/commit/9ce2c7702d15d74f64b7d4bf6273cba442b48c09)
- chore(docs) minor formatting on example's GitHub link [`75209e8`](https://github.com/lvgl/lvgl/commit/75209e893e89b6aa9d6a231af4661ce6a6dd6161)
- chore(lv_conf_template) fix spelling mistake [`9d134a9`](https://github.com/lvgl/lvgl/commit/9d134a99e3f59412ee4a941f20bf70053dd4326d)
- Update CHANGELOG.md [`8472360`](https://github.com/lvgl/lvgl/commit/847236044da01096beae4a586c874b4980f21a55)
- chore(stale) disable on forks [`93c1303`](https://github.com/lvgl/lvgl/commit/93c1303ee7989d25216262e1d0ea244b59b975f6)
- Revert "fix(tests) remove src/test_runners when cleaning" [`ae15a1b`](https://github.com/lvgl/lvgl/commit/ae15a1bbfe122115e5c8ac1f707929673843ad37)

- style fix usage of clang-format directives [`2122583`](https://github.com/lvgl/lvgl/commit/2122583ec23d82422e1e3d6f2b5a20745fa5dd6d)
- Revert "fix(indev) focus on objects on release instead of press" [`f61b2ca`](https://github.com/lvgl/lvgl/commit/f61b2ca45502472cde8ac0983b73dbf153de2b20)

## v8.0.2 (16.07.2021)
- fix(theme) improve button focus of keyboard
- fix(tabview) send LV_EVENT_VALUE_CHANGED only once
- fix(imgbtn) use the correct src in LV_EVENT_GET_SELF_SIZE
- fix(color) remove extraneous cast for 8-bit color
- fix(obj style) fix children reposition if the parent's padding changes.
- fix(color) remove extraneous _LV_COLOR_MAKE_TYPE_HELPER (#2372)
- fix(spinner) should not be clickable (#2373)
- fix(obj) improve how the focusing indev is determined
- fix(template) update indev template for v8
- fix(printf) skip defining attribute if pycparser is used
- refactor(printf) add printf-like function attribute to _lv_txt_set_text_vfmt and lv_label_set_text_fmt (#2332)
- fix(template) include lvgl.h in lv_port_*_template.c files
- fix(obj) detecting which indev sent LV_EVENT_FOCUS
- fix (span) fill LV_EVENT_GET_SELF_SIZE (#2360)
- fix(arc) disable LV_OBJ_FLAG_SCROLL_CHAIN by default
- fix (draw) fix arc bg image drawing with full arcs
- fix(disp) fix memory leak in lv_disp_remove (#2355)
- fix warnings introduced by 3fb8baf5
- fix(widgets) use lv_obj_class for all the widgets
- fix(obj) move clean ups from lv_obj_del to lv_obj_destructor
- fix(roller) fix partial redraw of the selected area
- fix(roller) adjust the size of the selected area correctly
- fix(obj) delete useless type conversion (#2343)
- fix(lv_obj_scroll.h) typos (#2345)
- fix(scroll) fire LV_EVENT_SCROLL_BEGIN in the same spot for both axes
- fix(btnmatrix) fix button invalidation on focus change
- fix(textarea) style update in oneline mode + improve scroll to cursor
- fix(tlsf) do not use <assert.h>
- fix(imgbtn) consider width==LV_SIZE_CONTENT if only mid. img is set
- fix(refr) reduce the nesting level in lv_refr_area
- fix(txt) enhance the function of break_chars (#2327)
- fix(pxp): update RTOS macro for SDK 2.10
- fix(vglite): update for v8
- fix(pxp): update for v8
- fix(flex) fix layout update and invalidation issues
- fix(flex) fix NULL pointer dereference
- fix(obj, switch) do not send LV_EVENT_VALUE_CHANGED twice
- fix(color) overflow with 16-bit color depth
- fix(coords) fix using large coordinates
- fix(chart) fix crash if no series are added
- fix(chart) invalidation with LV_CHART_UPDATE_MODE_SHIFT
- fix(align) fix lv_obj_align_to  G
- fix(table) invalidate the table on cell value change
- fix(label) remove duplicated lv_obj_refresh_self_size
- fix(draw) underflow in subpixel font drawing
- fix (scroll) do not send unnecessary scroll end events


## v8.0.1 (14.06.2021)
- docs(filesystem) update to v8 <a href="https://github.com/lvgl/lvgl/commit/7971ade4">7971ade4</a>
- fix(msgbox) create modals on top layer instead of act screen <a href="https://github.com/lvgl/lvgl/commit/5cf6303e">5cf6303e</a>
- fix(colorwheel) disable LV_OBJ_FLAG_SCROLL_CHAIN by default <a href="https://github.com/lvgl/lvgl/commit/48d1c292">48d1c292</a>
- docs(grid) typo fix (#2310) <a href="https://github.com/lvgl/lvgl/commit/69d109d2">69d109d2</a>
- fix(arduino) fix the prototype of my_touchpad_read in the LVGL_Arduino.ino <a href="https://github.com/lvgl/lvgl/commit/1a62f7a6">1a62f7a6</a>
- fix(meter) fix needle image invalidation <a href="https://github.com/lvgl/lvgl/commit/54d8e817">54d8e817</a>
- fix(mem) add lv_ prefix to tlsf functions and types <a href="https://github.com/lvgl/lvgl/commit/0d52b59c">0d52b59c</a>
- fix(calendar) fix the position calculation today <a href="https://github.com/lvgl/lvgl/commit/ad05e196">ad05e196</a>
- fix(typo) rename LV_OBJ_FLAG_SNAPABLE to LV_OBJ_FLAG_SNAPPABLE <a href="https://github.com/lvgl/lvgl/commit/e697807c">e697807c</a>
- docs(color) language fixes (#2302) <a href="https://github.com/lvgl/lvgl/commit/07ecc9f1">07ecc9f1</a>
- fix(tick) minor optimization on lv_tick_inc call test <a href="https://github.com/lvgl/lvgl/commit/b4305df5">b4305df5</a>
- Spelling and other language fixes to documentation (#2293) <a href="https://github.com/lvgl/lvgl/commit/d0aaacaf">d0aaacaf</a>
- fix(theme) show disabled state on buttons of btnmatrix, msgbox and keyboard <a href="https://github.com/lvgl/lvgl/commit/0be582b3">0be582b3</a>
- fix(scroll) keep the scroll position on object deleted <a href="https://github.com/lvgl/lvgl/commit/52edbb46">52edbb46</a>
- fix(msgbox) handle NULL btn map parameter <a href="https://github.com/lvgl/lvgl/commit/769c4a30">769c4a30</a>
- fix(group) allow refocusing objects <a href="https://github.com/lvgl/lvgl/commit/1520208b">1520208b</a>
- docs(overview) spelling fixes <a href="https://github.com/lvgl/lvgl/commit/d2efb8c6">d2efb8c6</a>
- Merge branch 'master' of https://github.com/lvgl/lvgl <a href="https://github.com/lvgl/lvgl/commit/45960838">45960838</a>
- feat(timer) check if lv_tick_inc is called <a href="https://github.com/lvgl/lvgl/commit/aa6641a6">aa6641a6</a>
- feat(docs) add view on GitHub link <a href="https://github.com/lvgl/lvgl/commit/a716ac6e">a716ac6e</a>
- fix(theme) fix the switch style in the default theme <a href="https://github.com/lvgl/lvgl/commit/0c0dc8ea">0c0dc8ea</a>
- docs fix typo <a href="https://github.com/lvgl/lvgl/commit/8ab80645">8ab80645</a>
- Merge branch 'master' of https://github.com/lvgl/lvgl <a href="https://github.com/lvgl/lvgl/commit/e796448f">e796448f</a>
- feat(event) pass the scroll animation to LV_EVENT_SCROLL_BEGIN <a href="https://github.com/lvgl/lvgl/commit/ca54ecfe">ca54ecfe</a>
- fix(tabview) fix with left and right tabs <a href="https://github.com/lvgl/lvgl/commit/17c57449">17c57449</a>
- chore(docs) force docs rebuild <a href="https://github.com/lvgl/lvgl/commit/4a0f4139">4a0f4139</a>
- chore(docs) always deploy master to docs/master as well <a href="https://github.com/lvgl/lvgl/commit/6d05692d">6d05692d</a>
- fix(template) update lv_objx_template to v8 <a href="https://github.com/lvgl/lvgl/commit/38bb8afc">38bb8afc</a>
- docs(extra) add extra/README.md <a href="https://github.com/lvgl/lvgl/commit/8cd504d5">8cd504d5</a>
- Update CHANGELOG.md <a href="https://github.com/lvgl/lvgl/commit/48fd73d2">48fd73d2</a>
- Update quick-overview.md (#2295) <a href="https://github.com/lvgl/lvgl/commit/5616471c">5616471c</a>
- fix(pxp) change LV_COLOR_TRANSP to LV_COLOR_CHROMA_KEY to v8 compatibility <a href="https://github.com/lvgl/lvgl/commit/81f3068d">81f3068d</a>
- adding micropython examples (#2286) <a href="https://github.com/lvgl/lvgl/commit/c60ed68e">c60ed68e</a>
- docs(color) minor fix <a href="https://github.com/lvgl/lvgl/commit/ac8f4534">ac8f4534</a>
- fix(example) revert test code <a href="https://github.com/lvgl/lvgl/commit/77e2c1ff">77e2c1ff</a>
- fix(draw) with additive blending with 32-bit color depth <a href="https://github.com/lvgl/lvgl/commit/786db2af">786db2af</a>
- docs(color) update colors' docs <a href="https://github.com/lvgl/lvgl/commit/9056b5ee">9056b5ee</a>
- Merge branch 'master' of https://github.com/lvgl/lvgl <a href="https://github.com/lvgl/lvgl/commit/a711a1dd">a711a1dd</a>
- perf(refresh) optimize where to wait for lv_disp_flush_ready with 2 buffers <a href="https://github.com/lvgl/lvgl/commit/d0172f14">d0172f14</a>
- docs(lv_obj_style) update add_style and remove_style function headers (#2287) <a href="https://github.com/lvgl/lvgl/commit/60f7bcbf">60f7bcbf</a>
- fix memory leak of spangroup (#2285) <a href="https://github.com/lvgl/lvgl/commit/33e0926a">33e0926a</a>
- fix make lv_img_cache.h public because cache invalidation is public <a href="https://github.com/lvgl/lvgl/commit/38ebcd81">38ebcd81</a>
- Merge branch 'master' of https://github.com/lvgl/lvgl <a href="https://github.com/lvgl/lvgl/commit/2b292495">2b292495</a>
- fix(btnmatrix) fix focus event handling <a href="https://github.com/lvgl/lvgl/commit/3b58ef14">3b58ef14</a>
- Merge pull request #2280 from lvgl/dependabot/pip/docs/urllib3-1.26.5 <a href="https://github.com/lvgl/lvgl/commit/a2f45b26">a2f45b26</a>
- fix(label) calculating the clip area <a href="https://github.com/lvgl/lvgl/commit/57e211cc">57e211cc</a>
- chore(deps): bump urllib3 from 1.26.4 to 1.26.5 in /docs <a href="https://github.com/lvgl/lvgl/commit/b2f77dfc">b2f77dfc</a>
- fix(docs) add docs about the default group <a href="https://github.com/lvgl/lvgl/commit/29bfe604">29bfe604</a>

## v8.0.0 (01.06.2021)

v8.0 brings many new features like simplified and more powerful scrolling, new layouts inspired by CSS Flexbox and Grid, simplified and improved widgets, more powerful events, hookable drawing, and more.

v8 is a major change and therefore it's not backward compatible with v7.

### Directory structure
- The `lv_` prefix is removed from the folder names
- The `docs` is moved to the `lvgl` repository
- The `examples` are moved to the `lvgl` repository
- Create an `src/extra` folder for complex widgets:
   - It makes the core LVGL leaner
   - In `extra` we can have a lot and specific widgets
   - Good place for contributions

### Widget changes
- `lv_cont` removed, layout features are moved to `lv_obj`
- `lv_page` removed, scroll features are moved to `lv_obj`
- `lv_objmask` the same can be achieved by events
- `lv_meter` added as the union of `lv_linemeter` and `lv_gauge`
- `lv_span` new widget mimicking HTML `<span>`
- `lv_animing` new widget for simple slideshow animations
- \+ many minor changes and improvements

### New scrolling
- Support "elastic" scrolling when scrolled in
- Support scroll chaining among any objects types (not only `lv_pages`s)
- Remove `lv_drag`. Similar effect can be achieved by setting the position in `LV_EVENT_PRESSING`
- Add snapping
- Add snap stop to scroll max 1 snap point

### New layouts
- [CSS Grid](https://css-tricks.com/snippets/css/a-guide-to-grid/)-like layout support
- [CSS Flexbox](https://css-tricks.com/snippets/css/a-guide-to-flexbox/)-like layout support

### Styles
- Optimize and simplify styles
- State is saved in the object instead of the style property
- Object size and position can be set in styles too

### Events
- Allow adding multiple events to an object
- A `user_data` can be attached to the added events

### Driver changes
- `lv_disp_drv_t`, `lv_indev_drv_t`, `lv_fs_drv_t` needs to be `static`
- `...disp_buf...` is renamed to `draw_buf`. See an initialization example [here](https://github.com/lvgl/lv_sim_eclipse_sdl/blob/release/v8.0/main.c#L128-L141).
- No partial update if two screen sized buffers are set
- `disp_drv->full_refresh = 1` makes always the whole display redraw.
- `hor_res` and `ver_res` need to be set in `disp_drv`
- `indev_read_cb` returns `void`. To indicate that there is more that to read set `data->continue_reading = 1` in the `read_cb`

### Other changes
- Remove the copy parameter from create functions
- Simplified File system interface API
- Use a more generic inheritance
- The built-in themes are reworked
- `lv_obj_align` now saved the alignment and realigns the object automatically but can't be used to align to other than the parent
- `lv_obj_align_to` can align to an object but doesn't save the alignment
- `lv_pct(x)` can be used to set the size and position in percentage
- There are many other changes in widgets that are not detailed here. Please refer to the documentation of the widgets.

### New release policy
- We will follow [Release branches with GitLab flow](https://docs.gitlab.com/ee/topics/gitlab_flow.html#release-branches-with-gitlab-flow)
- Minor releases are expected in every 3-4 month
- `master` will always contain the latest changes

### Migrating from v7 to v8
- First and foremost, create a new `lv_conf.h` based on `lv_conf_template.h`.
- To try the new version it's recommended to use a simulator project and see the examples.
- When migrating your project to v8
  - Update the drivers are described above
  - Update the styles
  - Update the events
  - Use the new layouts instead of `lv_cont` features
  - Use `lv_obj` instead of `lv_page`
  - See the changes in [Colors](https://docs.lvgl.io/8.0/overview/color.html)
  - The other parts are mainly minor renames and refactoring. See the functions' documentation for descriptions.

## v7.11.0 (16.03.2021)

### New features
- Add better screen orientation management with software rotation support
- Decide text animation's direction based on base_dir (when using LV_USE_BIDI)

### Bugfixes
- fix(gauge) fix needle invalidation
- fix(bar) correct symmetric handling for vertical sliders

## v7.10.1 (16.02.2021)

### Bugfixes
- fix(draw) overlap outline with background to prevent aliasing artifacts
- fix(indev) clear the indev's `act_obj` in `lv_indev_reset`
- fix(text) fix out of bounds read in `_lv_txt_get_width`
- fix(list) scroll list when button is focused using LV_KEY_NEXT/PREV
- fix(text) improve Arabic contextual analysis by adding hyphen processing and proper handling of lam-alef sequence
- fix(delete) delete animation after the children are deleted
- fix(gauge) consider paddings for needle images

## v7.10.0 (02.02.2021)

### New features
- feat(indev) allow input events to be passed to disabled objects
- feat(spinbox) add inline get_step function for MicroPython support

### Bugfixes
- fix(btnmatrix) fix lv_btnmatrix_get_active_btn_text() when used in a group

## v7.9.1 (19.01.2021)

### Bugfixes
- fix(cpicker) fix division by zero
- fix(dropdown) fix selecting options after the last one
- fix(msgbox) use the animation time provided
- fix(gpu_nxp_pxp) fix incorrect define name
- fix(indev) don't leave edit mode if there is only one object in the group
- fix(draw_rect) fix draw pattern stack-use-after-scope error


## v7.9.0 (05.01.2021)

### New features
- feat(chart) add lv_chart_remove_series and lv_chart_hide_series
- feat(img_cache) allow disabling image caching
- calendar: make get_day_of_week() public
- Added support for Zephyr integration

### Bugfixes
- fix(draw_rect) free buffer used for arabic processing
- fix(win) arabic process the title of the window
- fix(dropdown) arabic process the option in lv_dropdown_add_option
- fix(textarea) buffer overflow in password mode with UTF-8 characters
- fix(textarea) cursor position after hiding character in password mode
- fix(linemeter) draw critical lines with correct color
- fix(lv_conf_internal) be sure Kconfig defines are always uppercase
- fix(kconfig) handle disable sprintf float correctly.
- fix(layout) stop layout after recursion threshold is reached
- fix(gauge) fix redraw with image needle

## v7.8.1 (15.12.2020)

### Bugfixes
- fix(lv_scr_load_anim) fix when multiple screens are loaded at the same time with delay
- fix(page) fix LV_SCROLLBAR_MODE_DRAG

## v7.8.0 (01.12.2020)

### New features
- make DMA2D non blocking
- add unscii-16 built-in font
- add KConfig
- add lv_refr_get_fps_avg()

### Bugfixes
- fix(btnmatrix) handle arabic texts in button matrices
- fix(indev) disabled object shouldn't absorb clicks but let the parent to be clicked
- fix(arabic) support processing again already processed texts with _lv_txt_ap_proc
- fix(textarea) support Arabic letter connections
- fix(dropdown) support Arabic letter connections
- fix(value_str) support Arabic letter connections in value string property
- fix(indev) in LV_INDEV_TYPE_BUTTON recognize 1 cycle long presses too
- fix(arc) make arc work with encoder
- fix(slider) adjusting the left knob too with encoder
- fix reference to LV_DRAW_BUF_MAX_NUM in lv_mem.c
- fix(polygon draw) join adjacent points if they are on the same coordinate
- fix(linemeter) fix invalidation when setting new value
- fix(table) add missing invalidation when changing cell type
- refactor(roller) rename LV_ROLLER_MODE_INIFINITE -> LV_ROLLER_MODE_INFINITE

## v7.7.2 (17.11.2020)
### Bugfixes
- fix(draw_triangle): fix polygon/triangle drawing when the order of points is counter-clockwise
- fix(btnmatrix): fix setting the same map with modified pointers
- fix(arc) fix and improve arc dragging
- label: Repair calculate back `dot` character logical error which cause infinite loop.
- fix(theme_material): remove the bottom border from tabview header
- fix(imgbtn) guess the closest available state with valid src
- fix(spinbox) update cursor position in lv_spinbox_set_step

## v7.7.1 (03.11.2020)
### Bugfixes
- Respect btnmatrix's `one_check` in `lv_btnmatrix_set_btn_ctrl`
- Gauge: make the needle images to use the styles from `LV_GAUGE_PART_PART`
- Group: fix in `lv_group_remove_obj` to handle deleting hidden objects correctly

## v7.7.0 (20.10.2020)

### New features
- Add PXP GPU support (for NXP MCUs)
- Add VG-Lite GPU support (for NXP MCUs)
- Allow max. 16 cell types for table
- Add `lv_table_set_text_fmt()`
- Use margin on calendar header to set distances and padding to the size of the header
- Add `text_sel_bg` style property

### Bugfixes
- Theme update to support text selection background
- Fix imgbtn state change
- Support RTL in table (draw columns right to left)
- Support RTL in pretty layout (draw columns right to left)
- Skip objects in groups if they are in disabled state
- Fix dropdown selection with RTL basedirection
- Fix rectangle border drawing with large width
- Fix `lv_win_clean()`

## v7.6.1 (06.10.2020)

### Bugfixes
- Fix BIDI support in dropdown list
- Fix copying base dir in `lv_obj_create`
- Handle sub pixel rendering in font loader
- Fix transitions with style caching
- Fix click focus
- Fix imgbtn image switching with empty style
- Material theme: do not set the text font to allow easy global font change

## v7.6.0 (22.09.2020)

### New features
- Check whether any style property has changed on a state change to decide if any redraw is required

### Bugfixes
- Fix selection of options with non-ASCII letters in dropdown list
- Fix font loader to support LV_FONT_FMT_TXT_LARGE

## v7.5.0 (15.09.2020)

### New features
- Add `clean_dcache_cb` and `lv_disp_clean_dcache` to enable users to use their own cache management function
- Add `gpu_wait_cb` to wait until the GPU is working. It allows to run CPU a wait only when the rendered data is needed.
- Add 10px and 8ox built in fonts

### Bugfixes
- Fix unexpected DEFOCUS on lv_page when clicking to bg after the scrollable
- Fix `lv_obj_del` and `lv_obj_clean` if the children list changed during deletion.
- Adjust button matrix button width to include padding when spanning multiple units.
- Add rounding to btnmatrix line height calculation
- Add `decmopr_buf` to GC roots
- Fix division by zero in draw_pattern (lv_draw_rect.c) if the image or letter is not found
- Fix drawing images with 1 px height or width

## v7.4.0 (01.09.2020)

The main new features of v7.4 are run-time font loading, style caching and arc knob with value setting by click.

### New features
- Add `lv_font_load()` function - Loads a `lv_font_t` object from a binary font file
- Add `lv_font_free()` function - Frees the memory allocated by the `lv_font_load()` function
- Add style caching to reduce access time of properties with default value
- arc: add set value by click feature
- arc: add `LV_ARC_PART_KNOB` similarly to slider
- send gestures event if the object was dragged. User can check dragging with `lv_indev_is_dragging(lv_indev_act())` in the event function.

### Bugfixes
- Fix color bleeding on border drawing
- Fix using 'LV_SCROLLBAR_UNHIDE' after 'LV_SCROLLBAR_ON'
- Fix cropping of last column/row if an image is zoomed
- Fix zooming and rotating mosaic images
- Fix deleting tabview with LEFT/RIGHT tab position
- Fix btnmatrix to not send event when CLICK_TRIG = true and the cursor slid from a pressed button
- Fix roller width if selected text is larger than the normal

## v7.3.1 (18.08.2020)

### Bugfixes
- Fix drawing value string twice
- Rename  `lv_chart_clear_serie` to `lv_chart_clear_series` and `lv_obj_align_origo` to `lv_obj_align_mid`
- Add linemeter's mirror feature again
- Fix text decor (underline strikethrough) with older versions of font converter
- Fix setting local style property multiple times
- Add missing background drawing and radius handling to image button
- Allow adding extra label to list buttons
- Fix crash if `lv_table_set_col_cnt` is called before `lv_table_set_row_cnt` for the first time
- Fix overflow in large image transformations
- Limit extra button click area of button matrix's buttons. With large paddings it was counter-intuitive. (Gaps are mapped to button when clicked).
- Fix `lv_btnmatrix_set_one_check` not forcing exactly one button to be checked
- Fix color picker invalidation in rectangle mode
- Init disabled days to gray color in calendar

## v7.3.0 (04.08.2020)

### New features
- Add `lv_task_get_next`
- Add `lv_event_send_refresh`, `lv_event_send_refresh_recursive` to easily send `LV_EVENT_REFRESH` to object
- Add `lv_tabview_set_tab_name()` function - used to change a tab's name
- Add `LV_THEME_MATERIAL_FLAG_NO_TRANSITION` and `LV_THEME_MATERIAL_FLAG_NO_FOCUS` flags
- Reduce code size by adding: `LV_USE_FONT_COMPRESSED` and `LV_FONT_USE_SUBPX` and applying some optimization
- Add `LV_MEMCPY_MEMSET_STD` to use standard `memcpy` and `memset`

### Bugfixes
- Do not print warning for missing glyph if its height OR width is zero.
- Prevent duplicated sending of `LV_EVENT_INSERT` from text area
- Tidy outer edges of cpicker widget.
- Remove duplicated lines from `lv_tabview_add_tab`
- btnmatrix: handle combined states of buttons (e.g. checked + disabled)
- textarea: fix typo in lv_textarea_set_scrollbar_mode
- gauge: fix image needle drawing
- fix using freed memory in _lv_style_list_remove_style

## v7.2.0 (21.07.2020)

### New features
- Add screen transitions with `lv_scr_load_anim()`
- Add display background color, wallpaper and opacity. Shown when the screen is transparent. Can be used with `lv_disp_set_bg_opa/color/image()`.
- Add `LV_CALENDAR_WEEK_STARTS_MONDAY`
- Add `lv_chart_set_x_start_point()` function - Set the index of the x-axis start point in the data array
- Add `lv_chart_set_ext_array()` function - Set an external array of data points to use for the chart
- Add `lv_chart_set_point_id()` function - Set an individual point value in the chart series directly based on index
- Add `lv_chart_get_x_start_point()` function - Get the current index of the x-axis start point in the data array
- Add `lv_chart_get_point_id()` function - Get an individual point value in the chart series directly based on index
- Add `ext_buf_assigned` bit field to `lv_chart_series_t` structure - it's true if external buffer is assigned to series
- Add `lv_chart_set_series_axis()` to assign series to primary or secondary axis
- Add `lv_chart_set_y_range()` to allow setting range of secondary y-axis (based on `lv_chart_set_range` but extended with an axis parameter)
- Allow setting different font for the selected text in `lv_roller`
- Add `theme->apply_cb` to replace `theme->apply_xcb` to make it compatible with the MicroPython binding
- Add `lv_theme_set_base()` to allow easy extension of built-in (or any) themes
- Add `lv_obj_align_x()` and `lv_obj_align_y()` functions
- Add `lv_obj_align_origo_x()` and `lv_obj_align_origo_y()` functions

### Bugfixes
- `tileview` fix navigation when not screen sized
- Use 14px font by default to for better compatibility with smaller displays
- `linemeter` fix conversation of current value to "level"
- Fix drawing on right border
- Set the cursor image non-clickable by default
- Improve mono theme when used with keyboard or encoder

## v7.1.0 (07.07.2020)

### New features
- Add `focus_parent` attribute to `lv_obj`
- Allow using buttons in encoder input device
- Add lv_btnmatrix_set/get_align capability
- DMA2D: Remove dependency on ST CubeMX HAL
- Added `max_used` propriety to `lv_mem_monitor_t` struct
- In `lv_init` test if the strings are UTF-8 encoded.
- Add `user_data` to themes
- Add LV_BIG_ENDIAN_SYSTEM flag to lv_conf.h in order to fix displaying images on big endian systems.
- Add inline function lv_checkbox_get_state(const lv_obj_t * cb) to extend the checkbox functionality.
- Add inline function lv_checkbox_set_state(const lv_obj_t * cb, lv_btn_state_t state ) to extend the checkbox functionality.

### Bugfixes
- `lv_img` fix invalidation area when angle or zoom changes
- Update the style handling to support Big endian MCUs
- Change some methods to support big endian hardware.
- remove use of c++ keyword 'new' in parameter of function lv_theme_set_base().
- Add LV_BIG_ENDIAN_SYSTEM flag to lv_conf.h in order to fix displaying images on big endian systems.
- Fix inserting chars in text area in big endian hardware.

## v7.0.2 (16.06.2020)

### Bugfixes
- `lv_textarea` fix wrong cursor position when clicked after the last character
- Change all text related indices from 16-bit to 32-bit integers throughout whole library. #1545
- Fix gestures
- Do not call `set_px_cb` for transparent pixel
- Fix list button focus in material theme
- Fix crash when a text area is cleared with the backspace of a keyboard
- Add version number to `lv_conf_template.h`
- Add log in true double buffering mode with `set_px_cb`
- `lv_dropdown`: fix missing `LV_EVENT_VALUE_CHANGED` event when used with encoder
- `lv_tileview`: fix if not the {0;0} tile is created first
- `lv_debug`: restructure to allow asserting in from `lv_misc` too
- add assert if `_lv_mem_buf_get()` fails
- `lv_textarea`: fix character delete in password mode
- Update `LV_OPA_MIN` and `LV_OPA_MAX` to widen the opacity processed range
- `lv_btnm` fix sending events for hidden buttons
- `lv_gaguge` make `lv_gauge_set_angle_offset` offset the labels and needles too
- Fix typo in the API `scrllable` -> `scrollable`
- `tabview` by default allow auto expanding the page only to right and bottom (#1573)
- fix crash when drawing gradient to the same color
- chart: fix memory leak
- `img`: improve hit test for transformed images

## v7.0.1 (01.06.2020)

### Bugfixes
- Make Micropython working by adding the required variables as GC_ROOT
- Prefix some internal API functions with `_` to reduce the API of LVGL
- Fix built-in SimSun CJK font
- Fix UTF-8 encoding when `LV_USE_ARABIC_PERSIAN_CHARS` is enabled
- Fix DMA2D usage when 32 bit images directly blended
- Fix lv_roller in infinite mode when used with encoder
- Add `lv_theme_get_color_secondary()`
- Add `LV_COLOR_MIX_ROUND_OFS` to adjust color mixing to make it compatible with the GPU
- Improve DMA2D blending
- Remove memcpy from `lv_ll` (caused issues with some optimization settings)
- `lv_chart` fix X tick drawing
- Fix vertical dashed line drawing
- Some additional minor fixes and formattings

## v7.0.0 (18.05.2020)

### Documentation
The docs for v7 is available at https://docs.littlevgl.com/v7/en/html/index.html

### Legal changes

The name of the project is changed to LVGL and the new website is on https://lvgl.io

LVGL remains free under the same conditions (MIT license) and a company is created to manage LVGL and offer services.

### New drawing system
Complete rework of LVGL's draw engine to use "masks" for more advanced and higher quality graphical effects.
A possible use-case of this system is to remove the overflowing content from the rounded edges.
It also allows drawing perfectly anti-aliased circles, lines, and arcs.
Internally, the drawings happen by defining masks (such as rounded rectangle, line, angle).
When something is drawn the currently active masks can make some pixels transparent.
For example, rectangle borders are drawn by using 2 rectangle masks: one mask removes the inner part and another the outer part.

The API in this regard remained the same but some new functions were added:
- `lv_img_set_zoom`: set image object's zoom factor
- `lv_img_set_angle`: set image object's angle without using canvas
- `lv_img_set_pivot`: set the pivot point of rotation

The new drawing engine brought new drawing features too. They are highlighted in the "style" section.

### New style system
The old style system is replaced with a new more flexible and lightweighted one.
It uses an approach similar to CSS: support cascading styles, inheriting properties and local style properties per object.
As part of these updates, a lot of objects were reworked and the APIs have been changed.

- more shadows options: *offset* and *spread*
- gradient stop position to shift the gradient area and horizontal gradient
- `LV_BLEND_MODE_NORMAL/ADDITIVE/SUBTRACTIVE` blending modes
- *clip corner*: crop the content on the rounded corners
- *text underline* and *strikethrough*
- dashed vertical and horizontal lines (*dash gap*, *dash_width*)
- *outline*: a border-like part drawn out of the background. Can have spacing to the background.
- *pattern*: display and image in the middle of the background or repeat it
- *value* display a text which is stored in the style. It can be used e.g. as a light-weighted text on buttons too.
- *margin*: similar to *padding* but used to keep space outside the object

Read the [Style](https://docs.littlevgl.com/v7/en/html/overview/style.html) section of the documentation to learn how the new styles system works.

### GPU integration
To better utilize GPUs, from this version GPU usage can be integrated into LVGL. In `lv_conf.h` any supported GPUs can be enabled with a single configuration option.

Right now, only ST's DMA2D (Chrom-ART) is integrated. More will in the upcoming releases.

### Renames
The following object types are renamed:
- sw -> switch
- ta -> textarea
- cb -> checkbox
- lmeter -> linemeter
- mbox -> msgbox
- ddlist -> dropdown
- btnm -> btnmatrix
- kb -> keyboard
- preload -> spinner
- lv_objx folder -> lv_widgets
- LV_FIT_FILL -> LV_FIT_PARENT
- LV_FIT_FLOOD -> LV_FLOOD_MAX
- LV_LAYOUT_COL_L/M/R -> LV_LAYOUT_COLUMN_LEFT/MID/RIGHT
- LV_LAYOUT_ROW_T/M/B -> LV_LAYOUT_ROW_TOP/MID/BOTTOM

### Reworked and improved object
- `dropdown`: Completely reworked. Now creates a separate list when opened and can be dropped to down/up/left/right.
- `label`: `body_draw` is removed, instead, if its style has a visible background/border/shadow etc it will be drawn. Padding really makes the object larger (not just virtually as before)
- `arc`: can draw background too.
- `btn`: doesn't store styles for each state because it's done naturally in the new style system.
- `calendar`: highlight the pressed datum. The used styles are changed: use `LV_CALENDAR_PART_DATE` normal for normal dates, checked for highlighted, focused for today, pressed for the being pressed. (checked+pressed, focused+pressed also work)
- `chart`: only has `LINE` and `COLUMN` types because with new styles all the others can be described. LV_CHART_PART_SERIES sets the style of the series. bg_opa > 0 draws an area in LINE mode. `LV_CHART_PART_SERIES_BG` also added to set a different style for the series area. Padding in `LV_CHART_PART_BG` makes the series area smaller, and it ensures space for axis labels/numbers.
- `linemeter`, `gauge`: can have background if the related style properties are set. Padding makes the scale/lines smaller. scale_border_width and scale_end_border_width allow to draw an arc on the outer part of the scale lines.
- `gauge`: `lv_gauge_set_needle_img` allows use image as needle
- `canvas`: allow drawing to true color alpha and alpha only canvas, add `lv_canvas_blur_hor/ver` and rename `lv_canvas_rotate` to `lv_canvas_transform`
- `textarea`: If available in the font use bullet (`U+2022`) character in text area password

### New object types
- `lv_objmask`: masks can be added to it. The children will be masked accordingly.

### Others
- Change the built-in fonts to [Montserrat](https://fonts.google.com/specimen/Montserrat) and add built-in fonts from 12 px to 48 px for every 2nd size.
- Add example CJK and Arabic/Persian/Hebrew built-in font
- Add ° and "bullet" to the built-in fonts
- Add Arabic/Persian script support: change the character according to its position in the text.
- Add `playback_time` to animations.
- Add `repeat_count` to animations instead of the current "repeat forever".
- Replace `LV_LAYOUT_PRETTY` with `LV_LAYOUT_PRETTY_TOP/MID/BOTTOM`

### Demos
- [lv_examples](https://github.com/littlevgl/lv_examples) was reworked and new examples and demos were added

### New release policy
- Maintain this Changelog for every release
- Save old major version in new branches. E.g. `release/v6`
- Merge new features and fixes directly into `master` and release a patch or minor releases every 2 weeks.

### Migrating from v6 to v7
- First and foremost, create a new `lv_conf.h` based on `lv_conf_template.h`.
- To try the new version it suggested using a simulator project and see the examples.
- If you have a running project, the most difficult part of the migration is updating to the new style system. Unfortunately, there is no better way than manually updating to the new format.
- The other parts are mainly minor renames and refactoring as described above.
