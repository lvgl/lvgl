.. _changelog:

Changelog
=========

`v9.2.0 <https://github.com/lvgl/lvgl/compare/v9.2.1...v9.2.0>`__ 24 October 2024
---------------------------------------------------------------------------------


New Features
~~~~~~~~~~~~

- **feat(gif): backport add loop count control and GIF load result** `6922 <https://github.com/lvgl/lvgl/pull/6922>`__
- **feat(animimg): backport add getter function for underlying animation** `6923 <https://github.com/lvgl/lvgl/pull/6923>`__
- **feat(pxp) Add option to use PXP only for rotation without creating PXP draw unit.** `892d97c <https://github.com/lvgl/lvgl/commit/892d97ccc107045fef25389bf0660cbed7f2ebe7>`__

Performance
~~~~~~~~~~~

- **perf(draw): skip area independence tests with one draw unit** `6825 <https://github.com/lvgl/lvgl/pull/6825>`__
- **perf(vg_lite): reduce matrix and radius calculations** `6800 <https://github.com/lvgl/lvgl/pull/6800>`__

Fixes
~~~~~

- **fix(kconfig): add LV_ATTRIBUTE_MEM_ALIGN, LV_ATTRIBUTE_LARGE_CONST and LV_SYSMON_GET_IDLE configs** `7131 <https://github.com/lvgl/lvgl/pull/7131>`__
- **fix(display): remove lv_display_set_buffers_with_stride function** `7087 <https://github.com/lvgl/lvgl/pull/7087>`__
- **fix(pxp): sync rotation direction with SW render** `7063 <https://github.com/lvgl/lvgl/pull/7063>`__
- **fix(vg_lite): fix image transform clipping area error** `6810 <https://github.com/lvgl/lvgl/pull/6810>`__
- **fix(vg_lite): select blend mode based on premultiplication** `6766 <https://github.com/lvgl/lvgl/pull/6766>`__
- **fix(layout): calculate content width using x alignment** `6948 <https://github.com/lvgl/lvgl/pull/6948>`__
- **fix(calendar): fix lv_calendar_gregorian_to_chinese compile error** `6894 <https://github.com/lvgl/lvgl/pull/6894>`__
- **fix(indev): fix hovering disabled obj resets indev** `6855 <https://github.com/lvgl/lvgl/pull/6855>`__
- **fix(gif): added bounds check in gif decoder** `6863 <https://github.com/lvgl/lvgl/pull/6863>`__
- **fix(freertos): sync signal from isr fixed** `6793 <https://github.com/lvgl/lvgl/pull/6793>`__
- **fix(draw): fix sw compile error when disable LV_DRAW_SW_COMPLEX** `6895 <https://github.com/lvgl/lvgl/pull/6895>`__
- **fix(libinput): private headers** `6869 <https://github.com/lvgl/lvgl/pull/6869>`__
- **fix(color): add missing ARGB8565 alpha check** `6891 <https://github.com/lvgl/lvgl/pull/6891>`__
- **fix(display): remove the unused sw_rotate field** `6866 <https://github.com/lvgl/lvgl/pull/6866>`__
- **fix(bar): fix bit overflow** `6841 <https://github.com/lvgl/lvgl/pull/6841>`__
- **fix(indev): don't wait until release when a new object is found** `6831 <https://github.com/lvgl/lvgl/pull/6831>`__
- **fix(glfw/opengles): fix buf_size calculation error** `6830 <https://github.com/lvgl/lvgl/pull/6830>`__
- **fix(roller): fix stringop overflow** `6826 <https://github.com/lvgl/lvgl/pull/6826>`__
- **fix(perf): perf monitor FPS** `6798 <https://github.com/lvgl/lvgl/pull/6798>`__
- **fix(micropython): missing bidi private header feature guard** `6801 <https://github.com/lvgl/lvgl/pull/6801>`__
- **fix(draw): fix artifact when rotating ARGB8888 images** `6794 <https://github.com/lvgl/lvgl/pull/6794>`__
- **fix(sdl): check against NULL before using the driver data of a display** `6799 <https://github.com/lvgl/lvgl/pull/6799>`__
- **fix(assets): add missing strides** `6790 <https://github.com/lvgl/lvgl/pull/6790>`__
- **fix(arc): ignore hits that are outside drawn background arc** `6753 <https://github.com/lvgl/lvgl/pull/6753>`__
- **fix(vg_lite): fixed clip_radius image cropping error** `6780 <https://github.com/lvgl/lvgl/pull/6780>`__
- **fix(vg_lite/vector): convert gradient matrix to global matrix** `6577 <https://github.com/lvgl/lvgl/pull/6577>`__
- **fix(spangroup): fix height calculation error** `6775 <https://github.com/lvgl/lvgl/pull/6775>`__
- **fix(buttonmatrix): use const arrays** `6765 <https://github.com/lvgl/lvgl/pull/6765>`__
- **fix(ime): fix ime crash when input_char is too long** `6767 <https://github.com/lvgl/lvgl/pull/6767>`__
- **fix(draw): cast color_format in LV_DRAW_BUF_INIT_STATIC** `6729 <https://github.com/lvgl/lvgl/pull/6729>`__
- **fix(sdl): nested comment is not allowed** `6748 <https://github.com/lvgl/lvgl/pull/6748>`__
- **fix(ime_pinyin): fix letter count wrong when using some dictionary** `6752 <https://github.com/lvgl/lvgl/pull/6752>`__
- **fix(anim): use correct variable `repeat_cnt`** `6757 <https://github.com/lvgl/lvgl/pull/6757>`__
- **fix(image): backport lv_image_set_inner_align() behaviour with LV_IMAGE_ALIGN_… (#6864)** `6946 <https://github.com/lvgl/lvgl/pull/6946>`__
- **fix(fs): backport add lv_fs_dir_t to lv_fs.h (#6925)** `6943 <https://github.com/lvgl/lvgl/pull/6943>`__
- **fix(textarea): fix placeholder text cannot be centered** `6879 <https://github.com/lvgl/lvgl/pull/6879>`__
- **fix(Kconfig): Fix non existant LV_STDLIB_BUILTIN (#6851)** `84346f3 <https://github.com/lvgl/lvgl/commit/84346f3ef208ca30d22e61de30e3d9a329142960>`__
- **fix(dropdown): automatically center dropdown content (#6881)** `e961669 <https://github.com/lvgl/lvgl/commit/e961669dd8ae7dc5355e4209cb717eb1674cf453>`__
- **fix(vglite) set cache invalidation callback for all handlers** `64beb5f <https://github.com/lvgl/lvgl/commit/64beb5f811e9ad119b28ad976b2e252a41f4b5ae>`__
- **fix(vglite): Set buffer stride as required by hardware** `c369054 <https://github.com/lvgl/lvgl/commit/c3690544d5b384fa6d2f8d26838a6e1cd976feac>`__
- **fix(sysmon) disable all performance banners from screen with serial redirect** `51e1dfc <https://github.com/lvgl/lvgl/commit/51e1dfcb40c52eb194138c7a4afadc828eb8c4bb>`__
- **fix(vglite) draw when multiple draw units are available** `a109608 <https://github.com/lvgl/lvgl/commit/a1096088062a37f6f5726790b8d947cf9bb7600c>`__
- **fix(vglite) triangle gradient areas fix** `5b837eb <https://github.com/lvgl/lvgl/commit/5b837eb7276c42a620235f544178b69d3f8501d1>`__
- **fix(pxp): Initialize all required handlers** `0f9f322 <https://github.com/lvgl/lvgl/commit/0f9f322733bb9b5709c5da4fd6bba58862461b8a>`__
- **fix(vglite) Small cleanup.** `c3ec922 <https://github.com/lvgl/lvgl/commit/c3ec922708aeaaf0c90abadc9f533b6edab54995>`__
- **fix(pxp) Small cleanup.** `29c9770 <https://github.com/lvgl/lvgl/commit/29c977042e317c741e874338af26041deb97925a>`__
- **fix(pxp) Fixed include header.** `7275e1f <https://github.com/lvgl/lvgl/commit/7275e1fff632d5271a6a16a869a595a8799476fa>`__
- **fix(style): remove transitions when a local style property is set (#6941)** `3d33421 <https://github.com/lvgl/lvgl/commit/3d3342104f390e8b0f0447db5c13dc0619070c2f>`__
- **fix(conf) Remove empty line to make comment more condens.** `8658411 <https://github.com/lvgl/lvgl/commit/8658411a2f5670468d105849d63a5705885ff1dc>`__
- **fix(pxp) invalidate cache for the whole frame buffer** `6dd7b3a <https://github.com/lvgl/lvgl/commit/6dd7b3a97862b1f6fa49f23d8afb76becd8088a8>`__
- **fix(vglite) invalidate cache for the whole frame buffer** `a634dea <https://github.com/lvgl/lvgl/commit/a634deade4709185c260afe1c038aed0291c3310>`__
- **fix(fs): add lv_fs_dir_t to lv_fs.h (#6925)** `c826f31 <https://github.com/lvgl/lvgl/commit/c826f31f0014c0d45311cda25effb36d1f801841>`__

Docs
~~~~

- **docs(CHANGELOG): fix formatting** `72dfc1d <https://github.com/lvgl/lvgl/commit/72dfc1d75198a126dd495483d9cbc5b27c6c882b>`__
- **docs(PXP): update PXP for rotation only feature** `da74d69 <https://github.com/lvgl/lvgl/commit/da74d6983b832637c9553d67f0133b63f260eafb>`__

Others
~~~~~~

- **chore(vg_lite): remove unnecessary buffer checks** `6921 <https://github.com/lvgl/lvgl/pull/6921>`__
- **chore: fix compile warnings** `6975 <https://github.com/lvgl/lvgl/pull/6975>`__
- **chore(Kconfig): add version info to Kconfig file to check mismatch** `6900 <https://github.com/lvgl/lvgl/pull/6900>`__

- **chore(cmsis-pack): create cmsis-pack for v9.2.1 patch (#7092)** `efd965c <https://github.com/lvgl/lvgl/commit/efd965c4c109f4370afa968f9feb11113fb6845c>`__
- **chore: update the version number to v9.2.1** `7c3b20d <https://github.com/lvgl/lvgl/commit/7c3b20d9f2e72b7812e9f453b513d87208505abe>`__
- **chore: cleanup after adding fixes from master** `0b4a188 <https://github.com/lvgl/lvgl/commit/0b4a188c0b0fd1c6076a1d91d8bd89dcaf4a4def>`__
