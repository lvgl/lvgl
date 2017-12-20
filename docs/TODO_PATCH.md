# TODOs for patch versions
Patch versions (x.y.1, x.y.2) contain bugfixes without changing the API but they can apppear in minor (x.1.0, x.2.0) or major (1.0.0, 2.0.0) versions too.

Bugfixes are done in `bugfix` branche.

The bugfixes of the still not released version are in `beta` branche.

## Contributing
Please create an issue to introduce a bug instead of adding pull request to this file.


## Next release
- [x] lv_btnm: check hide code (\177) at 0. byte position too (if width is not specified)
- [ ] lv_img: define *lv_img_raw_header* in *lv_draw.h* because now lv_img can't be disabled
- [ ] lv_list: ignore image related things when *lv_img* is not enebled
- [ ] lv_ta: fix hegiht if *one_line* and *FONT_ANTIALIAS*
- [ ] lv_obj_set_style: fix to update self style too (not only children)

## v4.2  (released at: 17.08.2017)
- [x] lv_slider: don't let indicator or bar to disappear because of hpad/vpad
- [x] lv_ta: memory leak if deleted in password mode
- [x] lv_list: work without *lv_img* by ignore the image file name parameter of *lv_list_add()*
