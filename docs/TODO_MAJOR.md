# TODOs for major versions
Major versions released typically when API changes are required

## Contributing
Please create an issue to suggest a new feature instead of adding pull request to this file.

## v5 
**Architectural changes**
- [ ] rename repository from *lvgl* to *littlevgl*
- [ ] rename repository from *proj_pc* to *pc_simulator*
- [ ] integrate *hal* in LittlevGL as a normal folder
- [ ] integrate *misc* in LittlevGl as submodule
- [ ] create a new repository for examples
- [ ] convert Applications into simple examples 

**API changes**
- [ ] define renames: e.g. *USE_LV_BTN* to *LV_BTN_USE*
- [ ] Remove LV_DOWNSCALE (LV_ANTIALIAS will be used instead)
- [ ] *lv_ta_get_txt* rename to *lv_ta_get_text* 
- [ ] lv_btnm_set_styles for *tpr*, t*rel* and *ina* too
- [ ] LV_LABEL_LONG_DOTS removed, use LV_LABEL_LONG_ROLL instead
- [ ] lv_list_set_element_text_roll removed.
