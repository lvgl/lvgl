# TODOs for patch versions
Patch versions (x.y.1, x.y.2) contain bugfixes without changing the API but they can apppear in minor (x.1.0, x.2.0) or major (1.0.0, 2.0.0) versions too.

Bugfixes are done in `bugfix` branche.

The bugfixes of the still not released version are in `beta` branche.

## Contributing
Please create an issue to introduce a bug instead of adding pull request to this file.

## v5.0.2 (released at: in progress)
- [x] Fix dependencied (Thanks to Zaltora)
- [x] lv_group: fix memory leak (Thanks to BenQoo)
- [x] LV_INDEV_READ_PERIOD 0 build bugfix
- [x] lv_roller: lv_roller_get_selected_str: bugfix (was recursive call)
- [x] lv_obj_get_style: with NULL style check if the parent is focused and use the focused style
- [x] lv_roller: add missing action handling
- [ ] Set 24 bit colors upper byte (alpha) to 0xFF

## v5.0.1 (released at: 02.01.2018)
- [x] lv_list: fixed when mouse and keyboard used together
- [x] lv_btnm: fix bottom border visibility
- [x] theme updates
- [x] line width fix width anti-aliasing
- [x] lv_conf_templ.h add more info

## v5.0 (released at: 21.12.2017)
- [x] lv_btnm: check hide code (\177) at 0. byte position too (if width is not specified)
- [x] lv_img: define *lv_img_raw_header* in *lv_draw.h* because now lv_img can't be disabled
- [x] lv_list: ignore image related things when *lv_img* is not enebled
- [x] lv_ta: fix hegiht if *one_line* and *FONT_ANTIALIAS*
- [x] lv_obj_set_style: fix to update self style too (not only children)

## v4.2  (released at: 17.08.2017)
- [x] lv_slider: don't let indicator or bar to disappear because of hpad/vpad
- [x] lv_ta: memory leak if deleted in password mode
- [x] lv_list: work without *lv_img* by ignore the image file name parameter of *lv_list_add()*
