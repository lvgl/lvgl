**IMPORTANT NOTE** The next major version (v9.0.0) is developed in the master branch. 
The last stable version is available in the [release/v8.3](https://github.com/lvgl/lvgl/tree/release/v8.3) branch.

---

<p align="right"><b>English</b> | <a href="./README_zh.md">中文</a> | <a href="/README_pt_BR.md">Português do Brasil</a></p>

<p align="center">
  <img src="https://lvgl.io/assets/images/logo_lvgl.png">
</p>

  <h1 align="center">Light and Versatile Graphics Library</h1>
  <br>
<p align="center">
  <img src="https://lvgl.io/assets/images/lvgl_widgets_demo.gif">
</p>
<br>
<p align="center">
  <a href="https://lvgl.io" title="Homepage of LVGL">Website </a> |
  <a href="https://docs.lvgl.io/" title="Detailed documentation with 100+ examples">Docs</a> |
  <a href="https://forum.lvgl.io" title="Get help and help others">Forum</a> |
  <a href="https://lvgl.io/services" title="Graphics design, UI implementation and consulting">Services</a> |
  <a href="https:/lvgl.io/demos" title="Demos running in your browser">Demos</a> |
  <a href="https://squareline.io/" title="UI Editor for LVGL">SquareLine Studio</a>
</p>
<br>

## Overview

LVGL is the most popular free and open source embedded graphics library to create beautiful UIs for any MCU, MPU and display type. It's supported by industry leading vendors and projects like  **Arm, STM32, NXP, Espressif, Nuvoton, Arduino, RT-Thread, Zephyr, NuttX, Adafruit and many more**.

It has all the features to create modern and beautiful GUIs: 30+ built-in widgets, a powerful style system, web inspired layout managers, and a typography system supporting many languages. To integrate LVGL into your platform, all you need is at least **32kB RAM and 128 kB flash**, a C compiler, a frame buffer, and at least an 1/10 screen sized buffer for rendering.

**SquareLine Studio** is a professional yet affordable **drag and drop UI editor** for LVGL. It runs on Windows, Linux and MacOS too and you can try it out even without registering to the website. 

Our team is ready to help you with **UI design, implementation and consulting services**. Feel free to contact us if you need some support during the development of your next UI.


## Features

### Free and Portable
- **Fully portable** LVGL is a C (C++ compatible) library with no external dependencies. It can be compiled to any microcontrollers or microprocessors, with any (RT)OS to drive monochrome, ePaper, OLED or TFT displays, or even monitors.

- **Free** LVGL is distributed under the MIT licence, so you can easily use it in commercial projects too.

- **Minimal requirements** C99 or newer compiler, minimum 32kB RAM and 128 kB flash, a frame buffer, and at least an 1/10 screen sized buffer for rendering. OS, External memory and GPU are supported but not required. 

### Widgets, Styles, Layouts and more
- **30+ built-in widgets**  Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table and many more.

- **Flexible style system**  ~100 style properties (e.g. radius, opacity, gradient, border, shadow, etc.) allow you to customize any part of the widgets (e.g. background, scrollbar, indicator, knob, etc) in any state (normal, pressed, check, focused, etc).

- **Powerful layout engine** Flexbox and Grid-like layouts to automatically size and position the widgets in a responsive way

- **Advanced text features** Texts are rendered with UTF-8 encoding supporting CJK, Thai, Hindi, Arabic, Persian writing systems. Word wrapping, kerning, text scrolling, sub-pixel rendering, Pinyin-IME Chinese input, Emojis in texts are also available out of the box.

- **Fast rendering engine** Supporting animations, anti-aliasing, opacity, smooth scrolling, blending modes, rounded rectangles, image transformation, etc  

- **Various Input Devices** Supporting mouse, touchpad, Keypad, keyboard, external buttons, encoder. 

- **Multiple display support** Use one MCU to drive more screens. 

### Binding and Build Support

- **Micropython binding** Exposes [LVGL API in Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)

- **No custom build system** LVGL is distributed as an organized collection of C files that can be easily integrated into any build system. Support for Make and CMake is included out of the box.

- **Develop on PC** Use the same UI code on PC and embedded hardware. 

- **Convert the UI to HTML** You can compile the C UI code to a single HTML file with our [Emscripten port](https://github.com/lvgl/lv_web_emscripten).

### Docs, Tools, and Services
- **Detailed docs** with [100+ simple examples](https://docs.lvgl.io/master/index.html)

- **SquareLine Studio** A professional and easy-to-use UI editor software to speed up and simplify the UI development.
 
- **Services** User interface design, implementation and consulting services to make UI development simpler and faster

## Get started
This list will guide you to get started with LVGL step-by-step.
### Get Familiar with LVGL
1. Check the [Online demos](https://lvgl.io/demos) to see LVGL in action (3 minutes)
2. Read the [Introduction](https://docs.lvgl.io/master/intro/index.html) page of the documentation (5 minutes)
3. Get familiar with the basics on the [Quick overview](https://docs.lvgl.io/master/get-started/quick-overview.html) page (15 minutes)

### Start to Use LVGL
4. Set up a [Simulator](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html) (10 minutes)
5. Try out some [Examples](https://github.com/lvgl/lvgl/tree/master/examples)
6. Port LVGL to a board. See the [Porting](https://docs.lvgl.io/master/porting/index.html) guide or check the ready to use [Projects](https://github.com/lvgl?q=lv_port_)

### Become a Pro
7. Read the [Overview](https://docs.lvgl.io/master/overview/index.html) page to get a better understanding of the library (2-3 hours)
8. Check the documentation of the [Widgets](https://docs.lvgl.io/master/widgets/index.html) to see their features and usage

### Get Help and Help Others
9. If you have questions go to the [Forum](http://forum.lvgl.io/)
10. Read the [Contributing](https://docs.lvgl.io/master/CONTRIBUTING.html) guide to see how you can help to improve LVGL (15 minutes)

### Go for More
11. Download and try out [SquareLine Studio](https://squareline.io/).
12. Contact us for [Services](https://lvgl.io/services).

## Packages
LVGL is available as:
- [Arduino library](https://docs.lvgl.io/master/get-started/platforms/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/reference/kconfig/CONFIG_LVGL.html)
- [ESP32 component](https://docs.lvgl.io/master/get-started/platforms/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/get-started/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/get-started/os/rt-thread.html)
- CMSIS-Pack

## Examples

For more examples see the [examples](https://github.com/lvgl/lvgl/tree/master/examples) folder.

![LVGL button with label example](https://github.com/lvgl/lvgl/raw/master/docs/misc/btn_example.png)

### C
```c
lv_obj_t * btn = lv_btn_create(lv_scr_act());                   /*Add a button to the current screen*/
lv_obj_set_pos(btn, 10, 10);                                    /*Set its position*/
lv_obj_set_size(btn, 100, 50);                                  /*Set its size*/
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(btn);                        /*Add a label to the button*/
lv_label_set_text(label, "Button");                             /*Set the labels text*/
lv_obj_center(label);                                           /*Align the label to the center*/
...

void btn_event_cb(lv_event_t * e)
{
  printf("Clicked\n");
}
```
### Micropython
Learn more about [Micropython](https://docs.lvgl.io/master/get-started/bindings/micropython.html).
```python
def btn_event_cb(e):
  print("Clicked")

# Create a Button and a Label
btn = lv.btn(lv.scr_act())
btn.set_pos(10, 10)
btn.set_size(100, 50)
btn.add_event_cb(btn_event_cb, lv.EVENT.CLICKED, None)

label = lv.label(btn)
label.set_text("Button")
label.center()
```

## Services
LVGL Kft was established to provide a solid background for LVGL library. We offer several type of services to help you in UI development:
- Graphics design
- UI implementation
- Consulting/Support

For more information see https://lvgl.io/services
Feel free to contact us if you have any questions.


## Contributing
LVGL is an open project and contribution is very welcome. There are many ways to contribute from simply speaking about your project, through writing examples, improving the documentation, fixing bugs to hosting your own project under the LVGL organization.

For a detailed description of contribution opportunities visit the [Contributing](https://docs.lvgl.io/master/CONTRIBUTING.html) section of the documentation.
