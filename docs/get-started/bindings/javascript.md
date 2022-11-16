# JavaScript

With [lv_binding_js](https://github.com/lvgl/lv_binding_js) you can write lvgl with JavaScript.

It uses React's virtual DOM concept to manipulate lvgl UI components, providing a familiar React-like experience to users.

**Code**

<img src="../../_static/img/js_code.png">


**Code Runing on Real Device**

<img src="../../_static/img/js_on_device.jpg" style="transform: rotate(270deg); max-width: 400px; padding-left: 100px;">

## Table of Contents

  - [Features](#features)
  - [Demo](#demo)
  - [Building](#building)
  - [Components](#components)
  - [Font](#font)
  - [Animation](#animation)
  - [Style](#style)
  - [JSAPI](#jsapi)
  - [Thanks](#thanks)


## Features

- Support all lvgl built-in components
- Fully suport lvgl flex and grid style
- support most lvgl style，just write like html5 css
- support dynamic load image
- Fully support lvgl animation

## Demo

See the [demo](https://github.com/lvgl/lv_binding_js/tree/master/demo) folder


## Building

The following are developer notes on how to build lvgljs on your native platform. They are not complete guides, but include notes on the necessary libraries, compile flags, etc.

### lvgljs

- [ubuntu build Notes for sdl simulator](https://github.com/lvgl/lv_binding_js/blob/master/doc/build/build-ubuntu-arm.md)
- [macos x86 build Notes for sdl simulator](https://github.com/lvgl/lv_binding_js/blob/master/doc/build/build-macos-x86-simulator.md)
- [ubuntu build Notes for platform arm](https://github.com/lvgl/lv_binding_js/blob/master/doc/build/build-ubuntu-x86-simualtor.md)

### JS Bundle
- [JS Bundle build Notes](https://github.com/lvgl/lv_binding_js/blob/master/doc/build/js-bundle.md)

## Components

- [View](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/View.md)
- [Image](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Image.md)
- [Button](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Button.md)
- [Text](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Text.md)
- [Input](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Input.md)
- [Textarea](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Textarea.md)
- [Switch](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Switch.md)
- [Checkbox](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Checkbox.md)
- [Dropdownlist](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Dropdownlist.md)
- [ProgressBar](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/ProgressBar.md)
- [Line](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Line.md)
- [Roller](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Roller.md)
- [Keyboard](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Keyboard.md)
- [Calendar](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Calendar.md)
- [Chart](https://github.com/lvgl/lv_binding_js/blob/master/doc/component/Chart.md)

## Font

[Buitin-Symbol](https://github.com/lvgl/lv_binding_js/blob/master/doc/Symbol/symbol.md)

## Animation

[Animation](https://github.com/lvgl/lv_binding_js/blob/master/doc/animate/animate.md)

## Style

- [position-size-layout](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/position-size-layout.md)
- [boxing-model](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/boxing-model.md)
- [color](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/color.md)
- [flex](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/flex.md)
- [grid](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/grid.md)
- [font](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/font.md)
- [opacity](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/opacity.md)
- [display](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/display.md)
- [background](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/background.md)
- [scroll](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/scroll.md)
- [shadow](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/shadow.md)
- [recolor](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/recolor.md)
- [line](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/line.md)
- [transition](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/transition.md)
- [transform](https://github.com/lvgl/lv_binding_js/blob/master/doc/style/transform.md)

## JSAPI

- [network](https://github.com/lvgl/lv_binding_js/blob/master/doc/jsapi/network.md)
- [filesystem](https://github.com/lvgl/lv_binding_js/blob/master/doc/jsapi/fs.md)
- [dimension](https://github.com/lvgl/lv_binding_js/blob/master/doc/jsapi/dimension.md)

## Thanks

lvgljs depends on following excellent work

[lvgl](https://github.com/lvgl/lvgl): Create beautiful UIs for any MCU, MPU and display type
[QuickJS](https://bellard.org/quickjs/): JavaScript engine
[libuv](https://github.com/libuv/libuv): platform abstraction layer
[curl](https://github.com/curl/curl): HTTP client
[txiki.js](https://github.com/saghul/txiki.js): Tiny JavaScript runtime
