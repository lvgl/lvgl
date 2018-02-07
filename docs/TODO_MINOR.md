# TODOs for minor versions
Minor versions (x.1.0, x.2.0 ...) released when one or more new feature is addded without changing the API. New features can be added with major versions (1.0.0, 2.0.0 ...) too. 

## Contributing
Please create an issue to suggest a new feature instead of adding pull request to this file.

## Ideas
Here are ideas which are not assigned to a minor version yet:
- label: add a horzintal line (e.g. underline or line through).
- lv_split: new object type, a hor. or ver. line for decoration purpose
- lv_valset: new object type, a label with "+" and "-" buttons
- lv_listctrl: new object type, a list various controls on th right (sw, cb erc.)
- lv_inlist: new object type, inline drop down list (a button wich opens a list in place)
- lv_char: new_object type: characteristic set (like chart with draggable points)
- lv_vol: new_object type: volume meter (like a bar with segments)
- Anim. paths: monentum (tnh(x)), curve (exp), shake
- Hover, hover_lost signals
- Detached area (for video rendering where LittlevGL don't put pixels) 
- lv_ta: add placeholder text
- image rotate


## v5.2 (planned)
- [ ] Lua interface to craete GUI with script
- [ ] Arabic glyph convert (based on letter position)
- [ ] Right-to-left write support 
- [ ] Bit based VDB: 1, 2 or 4 bit
- [ ] lv_icon: new object type: an image wich acts as a button (different image for different states)
- [ ] triangle drawing

## v5.1 (released at: in progress)
- [ ] lv_refr_set_roundig_callback: set a sunction modify the invalidated area for speciel diplay controllers
- [ ] lv_group_set_focus_callback: set function to call when a new object is focused
- [ ] lv_obj_get_type() return string, e.g. "lv_slider", "lv_btn" etc
- [ ] Font handling extension for effective Chiese font handling (cutsom read functions)
- [ ] Remove LV_FONT_ANTIALIAS and add fonts with BPP (bit-per-pixel): 1, 2, 4 and 8
- [ ] lv_img: add pixel level opacity option (ARGB8888 or ARGB8565) (make image upscale pointless)
- [ ] LV_ANTIALIAS rework: meaning anti-alias lines and curves
- [ ] Merge symbol fonts (basic, file and feedback) into one font
- [ ] lv_group: different default style_mod function with LV_COLOR_DEPTH   1
- [ ] lv_img_set_data() for const. image data instead of file system usage
- [ ] lv_obj_set_sticky: new object attribute, to force input device to NOT find new object when the object's pressing is lost

## v5.0 (released at: 20.12.2017)
- [x] UTF-8 support
- [x] lv_tabview: new object type to organise content with tabs
- [x] lv_sw: new object type, switch, turn on/off by tap (a little slider)
- [x] lv_roller: new object type, a roller to select a value (like on smartphones) 
- [x] lv_kb: new object type, Keyboard
- [x] lv_btnm: lv_btnm_set_tgl() to toggle last button
- [x] lv_ta: cursor types
- [x] add themes with predefined styles
- [x] partial border draw in styles

## v4.2 (released at: 17.08.2017)
- [x] Double VDB support: one for rendering, another to transfer former rendered image to frame buffer in the background (e.g. with DMA) [#15](https://github.com/littlevgl/lvgl/issues/15)
- [x] lv_group: to control without touch pad. Issue [#14](https://github.com/littlevgl/lvgl/issues/14)
- [x] lv_page: scrl def fit modification: hor:false, ver:true, and always set width to parent width
- [x] lv_btn: add lv_btn_get_..._action
- [x] lv_list: add lv_list_get_element_label/img
- [x] lv_ta: lv_ta_set_one_line to configure the Text area to one lined input field
- [x] style animations add
- [x] lv_btnm:  besides 0. byte (width dsc) 1. byte: hidden (\177, 0x7F, delete)
