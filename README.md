**IMPORTANT NOTE** The next major version (v9.0.0) is developed in the master branch. 
The last stable version is available in the [release/v8.3](https://github.com/lvgl/lvgl/tree/release/v8.3) branch.

---

<p align="right"><b>English</b> | <a href="./README_zh.md">中文</a> | <a href="/README_pt_BR.md">Português do Brasil</a></p>

  <br>
<p align="center">
  <img src="https://lvgl.io/assets/images/logo_lvgl.png">
</p>

  <h1 align="center">Light and Versatile Graphics Library</h1>
  <br>
<div align="center">
  <img src="https://github.com/kisvegabor/test/raw/master/smartwatch_demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/assets/images/lvgl_widgets_demo.gif">
</div>
<br>
<p align="center">
<a href="https://lvgl.io" title="Homepage of LVGL">Website </a> |
<a href="https://docs.lvgl.io/" title="Detailed documentation with 100+ examples">Docs</a> |
<a href="https://forum.lvgl.io" title="Get help and help others">Forum</a> |
<a href="https:/lvgl.io/demos" title="Demos running in your browser">Demos</a> |
<a href="https://lvgl.io/services" title="Graphics design, UI implementation and consulting">Services</a> |
<a href="https://squareline.io/" title="UI Editor for LVGL">SquareLine Studio</a>
</p>
<br>

## :ledger: Overview

**Mature and Well-known**<br>
LVGL is the most popular free and open source embedded graphics library to create beautiful UIs for any MCU, MPU and display type. It's supported by industry leading vendors and projects like  Arm, STM32, NXP, Espressif, Nuvoton, Arduino, RT-Thread, Zephyr, NuttX, Adafruit and many more.

**Feature Rich**<br>
It has all the features to create modern and beautiful GUIs: 30+ built-in widgets, a powerful style system, web inspired layout managers, and a typography system supporting many languages. To integrate LVGL into your platform, all you need is at least 32kB RAM and 128 kB Flash, a C compiler, a frame buffer, and at least an 1/10 screen sized buffer for rendering.

**UI Editor**<br>
SquareLine Studio is a professional yet affordable drag and drop UI editor for LVGL. It runs on Windows, Linux and MacOS too and you can try it out even without registering to the website. 

**Services**<br>
Our team is ready to help you with graphics design, UI implementation and consulting services. Contact us if you need some support during the development of your next GUI project.


## :rocket: Features 

**Free and Portable**
  - A fully portable C (C++ compatible) library with no external dependencies. 
  - Can be compiled to any MCU or MPU, with any (RT)OS
  - Supports monochrome, ePaper, OLED or TFT displays, or even monitors.
  - Distributed under the MIT licence, so you can easily use it in commercial projects too.
  - Needs only 32kB RAM and 128 kB Flash, a frame buffer, and at least an 1/10 screen sized buffer for rendering. 
  - OS, External memory and GPU are supported but not required. 

**Widgets, Styles, Layouts and more**
  - 30+ built-in widgets:  Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table and many more.
  - Flexible style system with  ~100 style properties to customize any part of the widgets in any state.
  - Flexbox and Grid-like layouts engines to automatically size and position the widgets in a responsive way.
  - Texts are rendered with UTF-8 encoding supporting CJK, Thai, Hindi, Arabic, Persian writing systems. 
  - Word wrapping, kerning, text scrolling, sub-pixel rendering, Pinyin-IME Chinese input, Emojis in texts.
  - Rendering engine supporting animations, anti-aliasing, opacity, smooth scrolling, shadows, image transformation, etc  
  - Supports Mouse, Touchpad, Keypad, keyboard, External buttons, Encoder input devices. 
  - Multiple display support.
  
**Binding and Build Support**
  - [Micropython binding](https://blog.lvgl.io/2019-02-20/micropython-bindings) exposes LVGL API
  - No custom build system is used. You can build LVGL as you build the other files of your project.
  - Support for Make and CMake is included out of the box.
  - Develop on PC and use the same UI code on embedded hardware. 
  - Convert C UI code to HTML file with our [Emscripten port](https://github.com/lvgl/lv_web_emscripten).

**Docs, Tools, and Services**
  - Detailed documentation with [100+ simple examples](https://docs.lvgl.io/master/index.html)
  - [SquareLine Studio](https://squareline.io/) - A professional and easy-to-use UI editor software to speed up and simplify the UI development.
  - [Services](https://lvgl.io/services) such as User interface design, Implementation and Consulting to make UI development simpler and faster.

## :arrow_forward: Get started
This list will guide you to get started with LVGL step-by-step.

**Get Familiar with LVGL**

  1. Check the [Online demos](https://lvgl.io/demos) to see LVGL in action (3 minutes)
  2. Read the [Introduction](https://docs.lvgl.io/master/intro/index.html) page of the documentation (5 minutes)
  3. Get familiar with the basics on the [Quick overview](https://docs.lvgl.io/master/get-started/quick-overview.html) page (15 minutes)

**Start to Use LVGL**
  
  4. Set up a [Simulator](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html) (10 minutes)
  5. Try out some [Examples](https://github.com/lvgl/lvgl/tree/master/examples)
  6. Port LVGL to a board. See the [Porting](https://docs.lvgl.io/master/porting/index.html) guide or check the ready to use [Projects](https://github.com/lvgl?q=lv_port_)

**Become a Pro**

  7. Read the [Overview](https://docs.lvgl.io/master/overview/index.html) page to get a better understanding of the library (2-3 hours)
  8. Check the documentation of the [Widgets](https://docs.lvgl.io/master/widgets/index.html) to see their features and usage

**Get Help and Help Others**

  9. If you have questions go to the [Forum](http://forum.lvgl.io/)
  10. Read the [Contributing](https://docs.lvgl.io/master/CONTRIBUTING.html) guide to see how you can help to improve LVGL (15 minutes)

**Go for More**

  11. Download and try out [SquareLine Studio](https://squareline.io/).
  12. Contact us for [Services](https://lvgl.io/services).

## :package: Packages 
LVGL is available as:
- [Arduino library](https://docs.lvgl.io/master/get-started/platforms/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/reference/kconfig/CONFIG_LVGL.html)
- [ESP32 component](https://docs.lvgl.io/master/get-started/platforms/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/get-started/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/get-started/os/rt-thread.html)
- NXP MCUXpresso library
- CMSIS-Pack

## :robot: Examples

See how to create a button with a click event in C and MicroPython. For more examples see the [Examples](https://github.com/lvgl/lvgl/tree/master/examples) folder.

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

Learn more about [Micropython](https://docs.lvgl.io/master/get-started/bindings/micropython.html).

## :handshake: Services
LVGL LLC was established to provide a solid background for LVGL library and to offer several type of services to help you in UI development. With 15+ years of experience in the user interface and graphics industry we can help you the bring your UI to the next level.

- **Graphics design** Our in-house graphics designers are experts in creating beautiful modern designs which fit to your product and the resources of your hardware.
- **UI implementation** We can also implement your UI based on the design you or we have created. You can be sure that we will make the most out of your hardware and LVGL. If a feature or widget is missing from LVGL, don't worry, we will implement it for you.
- **Consulting and Support** We can support you with consulting as well to avoid pricey and time consuming mistakes during the UI development.
- **Board certificate** For companies who are offering development boards, or production ready kits we do board certification which shows how board can run LVGL.


Check out our [Demos](https://lvgl.io/demos) as reference. For more information take look at the [Services page](https://lvgl.io/services). 

[Contact us](https://lvgl.io/#contact) and tell how we can help.


## :star2: Contributing
LVGL is an open project and contribution is very welcome. There are many ways to contribute from simply speaking about your project, through writing examples, improving the documentation, fixing bugs or even hosting your own project under the LVGL organization.

For a detailed description of contribution opportunities visit the [Contributing](https://docs.lvgl.io/master/CONTRIBUTING.html) section of the documentation.

More than 300 people already left their fingerprint in LVGL. Be one them! See your here! :slightly_smiling_face: 

<a href="https://github.com/lvgl/lvgl/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" />
</a>

... and many other.
