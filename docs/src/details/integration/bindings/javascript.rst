==========
JavaScript
==========

With `lv_binding_js <https://github.com/lvgl/lv_binding_js>`__ you can use LVGL from within JavaScript.

It uses React's virtual DOM concept to manipulate LVGL UI components, providing a familiar React-like
experience to users.

**Code**

**Code Running on Real Device**


Features
********

- Support all LVGL built-in components
- Fully support LVGL flex and grid styles
- Support most LVGL styles, just write like HTML5 CSS
- Support dynamic image loading
- Fully support LVGL animations


Demo
****

See the `demo <https://github.com/lvgl/lv_binding_js/tree/master/demo>`__ folder


Building
********

The following are developer notes on how to build lvgljs on your native platform. They are not complete guides,
but include notes on the necessary libraries, compile flags, etc.


lvgljs
------

- `Build Notes for embedded Linux device <https://github.com/lvgl/lv_binding_js/blob/master/doc/build/build-device.md>`__
- `Build Notes for SDL Simulator (Linux and macOS) <https://github.com/lvgl/lv_binding_js/blob/master/doc/build/build-simulator.md>`__


JS Bundle
---------

- `JS Bundle build Notes <https://github.com/lvgl/lv_binding_js/blob/master/doc/build/js-bundle.md>`__


Components
**********

- `View <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/View.md>`__
- `Image <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Image.md>`__
- `Button <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Button.md>`__
- `Text <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Text.md>`__
- `Input <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Input.md>`__
- `Textarea <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Textarea.md>`__
- `Switch <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Switch.md>`__
- `Checkbox <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Checkbox.md>`__
- `Dropdownlist <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Dropdownlist.md>`__
- `ProgressBar <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/ProgressBar.md>`__
- `Line <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Line.md>`__
- `Roller <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Roller.md>`__
- `Keyboard <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Keyboard.md>`__
- `Calendar <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Calendar.md>`__
- `Chart <https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Chart.md>`__


Font
****

- `Builtin-Symbol <https://github.com/lvgl/lv_binding_js/blob/master/doc/Symbol/symbol.md>`__


Animation
*********

- `Animation <https://github.com/lvgl/lv_binding_js/blob/master/doc/animate/animate.md>`__


Style
*****

.. include::https://github.com/lvgl/lv_binding_js/blob/master/doc/style/position-size-layout.md

- `position-size-layout <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/position-size-layout.md>`__
- `boxing-model <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/boxing-model.md>`__
- `color <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/color.md>`__
- `flex <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/flex.md>`__
- `grid <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/grid.md>`__
- `font <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/font.md>`__
- `opacity <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/opacity.md>`__
- `display <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/display.md>`__
- `background <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/background.md>`__
- `scroll <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/scroll.md>`__
- `shadow <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/shadow.md>`__
- `recolor <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/recolor.md>`__
- `line <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/line.md>`__
- `transition <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/transition.md>`__
- `transform <https://github.com/lvgl/lv_binding_js/blob/master/doc/style/transform.md>`__


JSAPI
*****

- `network <https://github.com/lvgl/lv_binding_js/blob/master/doc/jsapi/network.md>`__
- `filesystem <https://github.com/lvgl/lv_binding_js/blob/master/doc/jsapi/fs.md>`__
- `dimension <https://github.com/lvgl/lv_binding_js/blob/master/doc/jsapi/dimension.md>`__


Thanks
******

**lvgljs** depends on following excellent work:

- `lvgl <https://github.com/lvgl/lvgl>`__: Create beautiful UIs for any MCU, MPU and display type
- `QuickJS <https://bellard.org/quickjs/>`__: JavaScript engine
- `libuv <https://github.com/libuv/libuv>`__: platform abstraction layer
- `curl <https://github.com/curl/curl>`__: HTTP client
- `txiki.js <https://github.com/saghul/txiki.js>`__: Tiny JavaScript runtime
