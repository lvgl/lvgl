# TODOs for major versions
Major versions released typically when API changes are required

## Contributing
Please create an issue to suggest a new feature instead of adding pull request to this file.

## v6 (released at: in progrss)
- [ ] lv_img_upscale removal (generate image with with opacity instead)
- [ ] disp_map and disp_fill removal
- [ ] disp_flush, mem_blend, mem_fill rework: usea lv_area_t as parameter an GPU support 2D area
- [ ] disp_flush: not const color map
- [ ] ext size removal, enable drawing on full parent.
- [ ] multiple display support
- [ ] lv_ufs removal (was required only in the the old image system)

## v5 (released at: 20.12.2017)
**Architectural changes**
- [x] Rename repository from *proj_pc* to *pc_simulator*
- [x] Integrate *hal* in LittlevGL as a normal folder
- [x] Create a new repository for examples
- [x] Convert Applications into simple examples 
- [x] Add tests for all object types

**API changes**
- [x] Rework lv_style_t structure
- [x] Remove LV_DOWNSCALE (LV_ANTIALIAS will be used instead)
- [x] Rename (and slightly rework) some function, defines and enums to be more descriptive
- [x] lv_btnm: rework width control. Now a control byte is used for *width*, *hide* and *no long press* settings. 
- [x] LV_LABEL_LONG_DOTS removed, use LV_LABEL_LONG_ROLL instead
- [x] *lv_list_set_element_text_roll()* removed.
- [x] *lv_ddlist_set_fix_height()* instead of *auto_size*
- [x] *lv_list_set_sb_out()* removed because now the scrollbar style can position the scrollbar
- [x] *lv_gauge* rework to make it more like line meter (remove background but add scale lines)
- [x] rename *lv_dispi_...* to *lv_indev_...*
- [x] *lv_dispi_t* removed from *lv_action_type_t*. Use lv_indev_act() instead
- [x] make styles to global variable to ensure less typing
- [x] make fonts to global variables to ensure less typing and easy user-font adding
- [x] join symbol fonts into normal built-in fonts
- [x] add inline functions to avoide direct use of anchestor functions (e.g. for buttons: lv_cont_set_fit -> lv_btn_set_fit)
