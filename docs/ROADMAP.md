# Roadmap

This is a summary for thenew fatures of the major releases and a collection of ideas. 

This list indicates only the current intention and can be changed.

## v8
Planned to September/October 2020
- New scrolling:
  - See [feat/new-scroll]((https://github.com/lvgl/lvgl/tree/feat/new-scroll) branch and  [#1614](https://github.com/lvgl/lvgl/issues/1614)) issue.
  - Remove `lv_page` and support scrolling on `lv_obj`
  - Support "elastic" scrolling when scrolled in
  - Support scroll chaining among any objects types (not only `lv_pages`s)
  - Remove `lv_drag`. Similar effect can be achieved by setting the position in `LV_EVENT_PRESSING`
  - Add snapping?
  - Already working
- New layouts:
  - See [#1615](https://github.com/lvgl/lvgl/issues/1615) issue
  - [CSS Flexbox](https://css-tricks.com/snippets/css/a-guide-to-flexbox/)-like layout support
  - Besides setting width/height in `px` add support to `partent percentage` and `screen percentage`.
  - Work in progress
- Simplified File system interface ([feat/new_fs_api](https://github.com/lvgl/lvgl/tree/feat/new-fs-api) branch) to make porting easier
  - Work in progress
- Add new label alignment modes
  - See [#1656](https://github.com/lvgl/lvgl/issues/1656)

## Ideas
- ...
