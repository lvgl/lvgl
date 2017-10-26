# TODOs for major versions
Major versions released typically when API changes are required

## Contributing
Please create an issue to suggest a new feature instead of adding pull request to this file.

## v5 
**Architectural changes**
- [ ] Rename repository from *lvgl* to *littlevgl*
- [ ] Rename repository from *proj_pc* to *pc_simulator*
- [ ] Integrate *hal* in LittlevGL as a normal folder
- [ ] Create a new repository for examples
- [ ] Convert Applications into simple examples 
- [ ] Add tests for all object types

**API changes**
- [ ] Remove LV_DOWNSCALE (LV_ANTIALIAS will be used instead)
- [ ] Rename (and slightl rework) some function, defines and enums to be more descriptive
- [ ] lv_btnm: rework width control. Now a control byte is used for *width*, *hide* and *no long press* settings. 
- [x] LV_LABEL_LONG_DOTS removed, use LV_LABEL_LONG_ROLL instead
- [x] *lv_list_set_element_text_roll()* removed.
- [x] *lv_ddlist_set_fix_height()* instead of *auto_size*
- [x] *lv_gauge* rework to make it more like line meter (remove background but add scale lines)
- [x] rename *lv_dispi_...* to *lv_indev_...*
- [x] *lv_dispi_t* removed from *lv_action_type_t*. Use lv_indev_act() instead
