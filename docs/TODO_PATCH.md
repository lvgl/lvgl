# TODOs for patch versions
Patch versions (x.y.1, x.y.2) contain bugfixes without changing the API but they can apppear in minor (x.1.0, x.2.0) or major (1.0.0, 2.0.0) versions too.

The bugfixes of the still not released version are in `beta` branche.

## Contributing
Please create an issue to introduce a bug instead of adding pull request to this file.

## v5.1.1 (in progress)
- [ ] lv_line: set line.width ext. size to not trim parts on x = 0, y = 0 coordinates

## v5.0.3 (released on: 09.03.2018)
- [x] lv_chart: Fix the use of point_num more then 256 (Thanks to upbeat27) 
- [x] lv_label: fix 'offset' with LV_ANTIALIAS (LV_LABEL_LONG_ROLL was effected)
- [x] lv_label: anim. time wasn't allyed for LV_LABEL_LONG_ROLL
- [x] lv_txt_ut8_size fiy for 4 byte characters
- [x] lv_slider: fix knob_in with not zero min value
- [x] lv_area_is_on: handled some cases wrong
- [x] lv_indev: buffered indevs (return *true* in indev_read) was handled as non-buffered
- [x] drag: don't invalidate if the object wasn't moved

## v5.0.2 (released on: 19.01.2018)
- [x] Fix dependencied (Thanks to Zaltora)
- [x] lv_group: fix memory leak (Thanks to BenQoo)
- [x] LV_INDEV_READ_PERIOD 0 build bugfix
- [x] lv_roller: lv_roller_get_selected_str: bugfix (was recursive call)
- [x] lv_obj_get_style: with NULL style check if the parent is focused and use the focused style
- [x] lv_roller: add missing action handling
- [x] Set 24 bit colors upper byte (alpha) to 0xFF

## v5.0.1 (released on: 02.01.2018)
- [x] lv_list: fixed when mouse and keyboard used together
- [x] lv_btnm: fix bottom border visibility
- [x] theme updates
- [x] line width fix width anti-aliasing
- [x] lv_conf_templ.h add more info

## v5.0 (released on: 21.12.2017)
- [x] lv_btnm: check hide code (\177) at 0. byte position too (if width is not specified)
- [x] lv_img: define *lv_img_raw_header* in *lv_draw.h* because now lv_img can't be disabled
- [x] lv_list: ignore image related things when *lv_img* is not enebled
- [x] lv_ta: fix hegiht if *one_line* and *FONT_ANTIALIAS*
- [x] lv_obj_set_style: fix to update self style too (not only children)

## v4.2  (released on: 17.08.2017)
- [x] lv_slider: don't let indicator or bar to disappear because of hpad/vpad
- [x] lv_ta: memory leak if deleted in password mode
- [x] lv_list: work without *lv_img* by ignore the image file name parameter of *lv_list_add()*
