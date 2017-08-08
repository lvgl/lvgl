# TODOs for minor versions
Minor versions (x.1.0, x.2.0 ...) released when one or more new feature is addded without changing the API. New features can be added with major versions (1.0.0, 2.0.0 ...) too. 

## Contributing
Please create an issue to suggest a new feature instead of adding pull request to this file.

## Ideas
Here are ideas whish are not assigned to a minor version yet:
- label: add a horzintal line (e.g. underline or line through). 
- label long mode: dot begin
- music player app
- files app update: show content as text
- GUI remote control
- automatically build GUI from file (e.g. XML, JSON or HTML)
- lv_split: new object type, a hor. or ver. line for decoration purpose
- lv_valset: new object type, a label with "+" and "-" buttons
- lv_tabview: new object type to organise content with tabs
- lv_btngrp: new object type to display more buttons to choose an option from them
- lv_switch: new object type, turn on/off by tap (a little slider)

## v4.2
- [x] Double VDB support: one for rendering, another to transfer former rendered image to frame buffer in the background (e.g. with DMA) [#15](https://github.com/littlevgl/lvgl/issues/15)
- [x] lv_group: to control without touch pad. Issue [#14](https://github.com/littlevgl/lvgl/issues/14)
- [x] lv_page: scrl def fit modification: hor:false, ver:true, and always set width to parent width
- [x] lv_btn: add lv_btn_get_..._action
- [x] lv_list: add lv_list_get_element_label/img
- [x] style animations add
- [ ] lv_btnm:  besides 0. byte (width dsc) 1. byte: hidden (\177, 0x7F, delete)
- [ ] lv_label: font attribut to override style's font (no extra style needed to set only different font)
- [ ] lv_label: padding enable/disable to increase size with hpad and vpad (useful with layouts)
