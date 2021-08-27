# Roadmap

This is a summary for planned new features and a collection of ideas.
This list indicates only the current intention and it can be changed.

## v8.1
### Features
- [x] Unit testing (gtest?). See #1658
- [ ] Benchmarking (gem5 or qemu?). See #1660  
- [ ] lv_snapshot: buffer a widget and all of its children into an image. The source widget can be on a different screen too. The resulting image can be transformed.
- [ ] High level GPU support. See #2058

#### New features
- [x] merge MicroPython examples
- [x] add a "Try out yourself" button to the Micropython examples
  
### Discuss
- [ ] CPP binding  
- [ ] Plugins. In v8 core and extra widgets are separated. With the new flexible events, the behavior of the widgets can be modified in a modular way. E.g. a plugin to add faded area to a line chart (as in the widgets demo)

### Docs
- [x] Display the Micropytohn examples too.
- [x] Add a link to the example C and py files
- [x] List of all examples on a page. All in iframes grouped by category (e.g. flex, style, button)

### Others
- [ ] Add automatic rebuild to get binary directly. Similarly to [STM32F746 project](https://github.com/lvgl/lv_port_stm32f746_disco#try-it-with-just-a-few-clicks).
- [ ] Implement release scripts. I've added a basic specification [here](https://github.com/lvgl/lvgl/tree/master/scripts/release), but we should discuss it.
- [ ] Unit test for the core widgets

## v8.2
- [ ] Optimize line and circle drawing and masking
- [ ] Handle stride. See [#1858](https://github.com/lvgl/lvgl/issues/1858)
- [ ] Support LV_STATE_HOVERED

## Ideas
- Reconsider color format management for run time color format setting, and custom color format usage. (Also [RGB888](https://github.com/lvgl/lvgl/issues/1722))
- Make gradients more versatile
- Make image transformations more versatile
- Switch to RGBA colors in styles
- Consider direct binary font format support
- Simplify `group`s. Discussion is [here](https://forum.lvgl.io/t/lv-group-tabindex/2927/3).
- Use [generate-changelog](https://github.com/lob/generate-changelog) to automatically generate changelog
- lv_mem_alloc_aligned(size, align)
- Text node. See [#1701](https://github.com/lvgl/lvgl/issues/1701#issuecomment-699479408)
- CPP binding. See [Forum](https://forum.lvgl.io/t/is-it-possible-to-officially-support-optional-cpp-api/2736)
- Optimize font decompression
- Need coverage report for tests
- Need static analyze (via coverity.io or somehing else)
- Support dot_begin and dot_middle long modes for labels
- Add new label alignment modes. [#1656](https://github.com/lvgl/lvgl/issues/1656)
- Support larger images: [#1892](https://github.com/lvgl/lvgl/issues/1892)

--- 

## v8
- Create an `extra` folder for complex widgets
   - It makes the core LVGL leaner
   - In `extra` we can have a lot and specific widgets
   - Good place for contributions
- New scrolling:
  - See [feat/new-scroll](https://github.com/lvgl/lvgl/tree/feat/new-scroll) branch and  [#1614](https://github.com/lvgl/lvgl/issues/1614)) issue.
  - Remove `lv_page` and support scrolling on `lv_obj`
  - Support "elastic" scrolling when scrolled in
  - Support scroll chaining among any objects types (not only `lv_pages`s)
  - Remove `lv_drag`. Similar effect can be achieved by setting the position in `LV_EVENT_PRESSING`
  - Add snapping
  - Add snap stop to scroll max 1 snap point
  - Already working
- New layouts:
  - See [#1615](https://github.com/lvgl/lvgl/issues/1615) issue
  - [CSS Grid](https://css-tricks.com/snippets/css/a-guide-to-grid/)-like layout support
  - [CSS Flexbox](https://css-tricks.com/snippets/css/a-guide-to-flexbox/)-like layout support
  - Remove `lv_cont` and support layouts on `lv_obj`
- Simplified File system interface ([feat/new_fs_api](https://github.com/lvgl/lvgl/tree/feat/new-fs-api) branch) to make porting easier
  - Work in progress
- Remove the align parameter from `lv_canvas_draw_text`
- Remove the copy parameter from create functions
- Optimize and simplify styles [#1832](https://github.com/lvgl/lvgl/issues/1832)
- Use a more generic inheritance [#1919](https://github.com/lvgl/lvgl/issues/1919)
- Allow adding multiple events to an object

