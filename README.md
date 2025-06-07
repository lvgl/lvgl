
<a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets/sponsor.png" height="32px"></a>

<p align="right">
  <b>English</b> | <a href="./docs/README_zh.md">中文</a> | <a href="./docs/README_pt_BR.md">Português do Brasil</a> | <a href="./docs/README_jp.md">日本語</a> | <a href="./docs/README_he.md">עברית</a>
</p>

<br>

<p align="center">
  <img src="https://lvgl.io/github-assets/logo-colored.png" width=300px>
</p>

  <h1 align="center">Light and Versatile Graphics Library</h1>
  <br>
<div align="center">
  <img src="https://lvgl.io/github-assets/smartwatch-demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
</div>
<br>
<p align="center">
<a href="https://lvgl.io" title="Homepage of LVGL">Website </a> |
<a href="https://docs.lvgl.io/" title="Detailed documentation with 100+ examples">Docs</a> |
<a href="https://forum.lvgl.io" title="Get help and help others">Forum</a> |
<a href="https://lvgl.io/demos" title="Demos running in your browser">Demos</a> |
<a href="https://lvgl.io/services" title="Graphics design, UI implementation and consulting">Services</a>
</p>
<br>

## :ledger: Overview

**Mature and Well-known**<br>
LVGL is the most popular free and open source embedded graphics library to create beautiful UIs for any MCU, MPU and display type. It's supported by industry leading vendors and projects like  Arm, STM32, NXP, Espressif, Nuvoton, Arduino, RT-Thread, Zephyr, NuttX, Adafruit and many more.

**Feature Rich**<br>
It has all the features to create modern and beautiful GUIs: 30+ built-in widgets, a powerful style system, web inspired layout managers, and a typography system supporting many languages. To integrate LVGL into your platform, all you need is at least 32kB RAM and 128 kB Flash, a C compiler, a frame buffer, and at least an 1/10 screen sized buffer for rendering.

**Services**<br>
Our team is ready to help you with graphics design, UI implementation and consulting services. Contact us if you need some support during the development of your next GUI project.

## :rocket: Features

**Free and Portable**
  - A fully portable C (C++ compatible) library with no external dependencies.
  - Can be compiled to any MCU or MPU, with any (RT)OS.
  - Supports monochrome, ePaper, OLED or TFT displays, or even monitors. [Displays](https://docs.lvgl.io/master/details/main-modules/display/index.html)
  - Distributed under the MIT license, so you can easily use it in commercial projects too.
  - Needs only 32kB RAM and 128 kB Flash, a frame buffer, and at least an 1/10 screen sized buffer for rendering.
  - OS, External memory and GPU are supported but not required.

**Widgets, Styles, Layouts and more**
  - 30+ built-in [Widgets](https://docs.lvgl.io/master/details/widgets/index.html):  Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table and many more.
  - Flexible [Style system](https://docs.lvgl.io/master/details/common-widget-features/styles/style.html) with  ~100 style properties to customize any part of the widgets in any state.
  - [Flexbox](https://docs.lvgl.io/master/details/common-widget-features/layouts/flex.html) and [Grid](https://docs.lvgl.io/master/details/common-widget-features/layouts/grid.html)-like layouts engines to automatically size and position the widgets in a responsive way.
  - Texts are rendered with UTF-8 encoding supporting CJK, Thai, Hindi, Arabic, Persian writing systems.
  - Word wrapping, kerning, text scrolling, sub-pixel rendering, Pinyin-IME Chinese input, Emojis in texts.
  - Rendering engine supporting animations, anti-aliasing, opacity, smooth scrolling, shadows, image transformation, etc  
  - Supports Mouse, Touchpad, Keypad, Keyboard, External buttons, Encoder [Input devices](https://docs.lvgl.io/master/details/main-modules/indev.html).
  - [Multiple display](https://docs.lvgl.io/master/details/main-modules/display/overview.html#how-many-displays-can-lvgl-use) support.

**Binding and Build Support**
  - [MicroPython Binding](https://blog.lvgl.io/2019-02-20/micropython-bindings) exposes LVGL API
  - [PikaScript Binding](https://blog.lvgl.io/2022-08-24/pikascript-and-lvgl) python on MCU lighter and easier.
  - No custom build system is used. You can build LVGL as you build the other files of your project.
  - Support for Make and [CMake](https://docs.lvgl.io/master/details/integration/building/cmake.html) is included out of the box.
  - [Develop on PC](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html) and use the same UI code on embedded hardware.
  - Convert the C UI code to HTML file with our [Emscripten port](https://github.com/lvgl/lv_web_emscripten).

**Docs, Tools, and Services**
  - Detailed [Documentation](https://docs.lvgl.io/) with [100+ simple examples](https://docs.lvgl.io/master/examples.html)
  - [Services](https://lvgl.io/services) such as User interface design, Implementation and Consulting to make UI development simpler and faster.

## :heart: Sponsor

If LVGL saved you a lot of time and money or you just had fun using it, consider [Supporting its Development](https://github.com/sponsors/lvgl).

**How do we spend the donations?**<br>
Our goal is to provide financial compensation for people who do the most for LVGL. It means not only the maintainers but anyone who implements a great feature should get a payment from the accumulated money. We use the donations to cover our operational costs like servers and related services.

**How to donate?**<br>
We use [GitHub Sponsors](https://github.com/sponsors/lvgl) where you can easily send one time or recurring donations. You can also see all of our expenses  in a transparent way.

**How to get paid for your contribution?**<br>
If someone implements or fixes an issue labeled as [Sponsored](https://github.com/lvgl/lvgl/labels/Sponsored) he or she will get a payment for that work. We estimate the required time, complexity and importance of the issue and set a price accordingly. To jump in just comment on a [Sponsored](https://github.com/lvgl/lvgl/labels/Sponsored) issue saying "Hi, I'd like to deal with it. This is how I'm planning to fix/implement it...". A work is considered ready when it's approved and merged by a maintainer. After that you can submit and expense at [opencollective.com](https://opencollective.com/lvgl) and you will receive the payment in a few days.

**Organizations supporting LVGL**<br>
[![Sponsors of LVGL](https://opencollective.com/lvgl/organizations.svg?width=600)](https://opencollective.com/lvgl)

**Individuals supporting LVGL**<br>
[![Backers of LVGL](https://contrib.rocks/image?repo=lvgl/lvgl&max=48)](https://opencollective.com/lvgl)

## :package: Packages
LVGL is available as:
- [Arduino library](https://docs.lvgl.io/master/details/integration/framework/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.lvgl.io/master/details/integration/os/zephyr.html)
- [ESP-IDF(ESP32) component](https://components.espressif.com/components/lvgl/lvgl)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/details/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/details/integration/os/rt-thread.html)
- CMSIS-Pack
- [RIOT OS package](https://doc.riot-os.org/group__pkg__lvgl.html#details)


## :robot: Examples

See some examples of creating widgets, using layouts and applying styles. You will find C and MicroPython code, and links to try out or edit the examples in an online MicroPython editor.

For more examples check out the [Examples](https://github.com/lvgl/lvgl/tree/master/examples) folder.


### Hello world label

![Simple Hello world label example in LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_1.png)

<details>
  <summary>C code</summary>

```c
/*Change the active screen's background color*/
lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

/*Create a white label, set its text and align it to the center*/
lv_obj_t * label = lv_label_create(lv_screen_active());
lv_label_set_text(label, "Hello world");
lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), LV_PART_MAIN);
lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
```
</details>

<details>
  <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_direct=4ab7c40c35b0dc349aa2f0c3b00938d7d8e8ac9f" target="_blank">Online Simulator</a></summary>

```python
# Change the active screen's background color
scr = lv.screen_active()
scr.set_style_bg_color(lv.color_hex(0x003a57), lv.PART.MAIN)

# Create a white label, set its text and align it to the center
label = lv.label(lv.screen_active())
label.set_text("Hello world")
label.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN)
label.align(lv.ALIGN.CENTER, 0, 0)
```
</details>
<br>

### Button with Click Event

![LVGL button with label example](https://github.com/kisvegabor/test/raw/master/readme_example_2.gif)

<details>
  <summary>C code</summary>

```c
lv_obj_t * button = lv_button_create(lv_screen_active());                   /*Add a button to the current screen*/
lv_obj_center(button);                                             /*Set its position*/
lv_obj_set_size(button, 100, 50);                                  /*Set its size*/
lv_obj_add_event_cb(button, button_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(button);                        /*Add a label to the button*/
lv_label_set_text(label, "Button");                             /*Set the labels text*/
lv_obj_center(label);                                           /*Align the label to the center*/
...

void button_event_cb(lv_event_t * e)
{
  printf("Clicked\n");
}
```
</details>

<details>
  <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=926bde43ec7af0146c486de470c53f11f167491e" target="_blank">Online Simulator</a></summary>

```python
def button_event_cb(e):
  print("Clicked")

# Create a Button and a Label
button = lv.button(lv.screen_active())
button.center()
button.set_size(100, 50)
button.add_event_cb(button_event_cb, lv.EVENT.CLICKED, None)

label = lv.label(button)
label.set_text("Button")
label.center()
```
</details>
<br>

### Checkboxes with Layout
![Checkboxes with layout in LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_3.gif)

<details>
  <summary>C code</summary>

```c

lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

lv_obj_t * cb;
cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Apple");
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Banana");
lv_obj_add_state(cb, LV_STATE_CHECKED);
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Lemon");
lv_obj_add_state(cb, LV_STATE_DISABLED);
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);
lv_checkbox_set_text(cb, "Melon\nand a new line");
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);
```

</details>

<details>
  <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=311d37e5f70daf1cb0d2cad24c7f72751b5f1792" target="_blank">Online Simulator</a></summary>

```python
def event_handler(e):
    code = e.get_code()
    obj = e.get_target_obj()
    if code == lv.EVENT.VALUE_CHANGED:
        txt = obj.get_text()
        if obj.get_state() & lv.STATE.CHECKED:
            state = "Checked"
        else:
            state = "Unchecked"
        print(txt + ":" + state)


lv.screen_active().set_flex_flow(lv.FLEX_FLOW.COLUMN)
lv.screen_active().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Apple")
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Banana")
cb.add_state(lv.STATE.CHECKED)
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Lemon")
cb.add_state(lv.STATE.DISABLED)
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
cb.set_text("Melon")
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)
```

</details>
<br>

### Styling a Slider
![Styling a slider with LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_4.gif)


<details>
  <summary>C code</summary>

```c
lv_obj_t * slider = lv_slider_create(lv_screen_active());
lv_slider_set_value(slider, 70, LV_ANIM_OFF);
lv_obj_set_size(slider, 300, 20);
lv_obj_center(slider);

/*Add local styles to MAIN part (background rectangle)*/
lv_obj_set_style_bg_color(slider, lv_color_hex(0x0F1215), LV_PART_MAIN);
lv_obj_set_style_bg_opa(slider, 255, LV_PART_MAIN);
lv_obj_set_style_border_color(slider, lv_color_hex(0x333943), LV_PART_MAIN);
lv_obj_set_style_border_width(slider, 5, LV_PART_MAIN);
lv_obj_set_style_pad_all(slider, 5, LV_PART_MAIN);

/*Create a reusable style sheet for the INDICATOR part*/
static lv_style_t style_indicator;
lv_style_init(&style_indicator);
lv_style_set_bg_color(&style_indicator, lv_color_hex(0x37B9F5));
lv_style_set_bg_grad_color(&style_indicator, lv_color_hex(0x1464F0));
lv_style_set_bg_grad_dir(&style_indicator, LV_GRAD_DIR_HOR);
lv_style_set_shadow_color(&style_indicator, lv_color_hex(0x37B9F5));
lv_style_set_shadow_width(&style_indicator, 15);
lv_style_set_shadow_spread(&style_indicator, 5);
4
/*Add the style sheet to the slider's INDICATOR part*/
lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);

/*Add the same style to the KNOB part too and locally overwrite some properties*/
lv_obj_add_style(slider, &style_indicator, LV_PART_KNOB);

lv_obj_set_style_outline_color(slider, lv_color_hex(0x0096FF), LV_PART_KNOB);
lv_obj_set_style_outline_width(slider, 3, LV_PART_KNOB);
lv_obj_set_style_outline_pad(slider, -5, LV_PART_KNOB);
lv_obj_set_style_shadow_spread(slider, 2, LV_PART_KNOB);
```

</details>

<details>
  <summary>MicroPython code |
<a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=c431c7b4dfd2cc0dd9c392b74365d5af6ea986f0" target="_blank">Online Simulator</a>
</summary>


```python
# Create a slider and add the style
slider = lv.slider(lv.screen_active())
slider.set_value(70, lv.ANIM.OFF)
slider.set_size(300, 20)
slider.center()

# Add local styles to MAIN part (background rectangle)
slider.set_style_bg_color(lv.color_hex(0x0F1215), lv.PART.MAIN)
slider.set_style_bg_opa(255, lv.PART.MAIN)
slider.set_style_border_color(lv.color_hex(0x333943), lv.PART.MAIN)
slider.set_style_border_width(5, lv.PART.MAIN)
slider.set_style_pad_all(5, lv.PART.MAIN)

# Create a reusable style sheet for the INDICATOR part
style_indicator = lv.style_t()
style_indicator.init()
style_indicator.set_bg_color(lv.color_hex(0x37B9F5))
style_indicator.set_bg_grad_color(lv.color_hex(0x1464F0))
style_indicator.set_bg_grad_dir(lv.GRAD_DIR.HOR)
style_indicator.set_shadow_color(lv.color_hex(0x37B9F5))
style_indicator.set_shadow_width(15)
style_indicator.set_shadow_spread(5)

# Add the style sheet to the slider's INDICATOR part
slider.add_style(style_indicator, lv.PART.INDICATOR)
slider.add_style(style_indicator, lv.PART.KNOB)

# Add the same style to the KNOB part too and locally overwrite some properties
slider.set_style_outline_color(lv.color_hex(0x0096FF), lv.PART.KNOB)
slider.set_style_outline_width(3, lv.PART.KNOB)
slider.set_style_outline_pad(-5, lv.PART.KNOB)
slider.set_style_shadow_spread(2, lv.PART.KNOB)
```
</details>
<br>

### English, Hebrew (mixed LTR-RTL) and Chinese texts

![English, Hebrew and Chinese texts with LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_5.png)

<details>
  <summary>C code</summary>

```c
lv_obj_t * ltr_label = lv_label_create(lv_screen_active());
lv_label_set_text(ltr_label, "In modern terminology, a microcontroller is similar to a system on a chip (SoC).");
lv_obj_set_style_text_font(ltr_label, &lv_font_montserrat_16, 0);
lv_obj_set_width(ltr_label, 310);
lv_obj_align(ltr_label, LV_ALIGN_TOP_LEFT, 5, 5);

lv_obj_t * rtl_label = lv_label_create(lv_screen_active());
lv_label_set_text(rtl_label,"מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).");
lv_obj_set_style_base_dir(rtl_label, LV_BASE_DIR_RTL, 0);
lv_obj_set_style_text_font(rtl_label, &lv_font_dejavu_16_persian_hebrew, 0);
lv_obj_set_width(rtl_label, 310);
lv_obj_align(rtl_label, LV_ALIGN_LEFT_MID, 5, 0);

lv_obj_t * cz_label = lv_label_create(lv_screen_active());
lv_label_set_text(cz_label,
                  "嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。");
lv_obj_set_style_text_font(cz_label, &lv_font_source_han_sans_sc_16_cjk, 0);
lv_obj_set_width(cz_label, 310);
lv_obj_align(cz_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
```

</details>

<details>
  <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=18bb38200a64e10ead1aa17a65c977fc18131842" target="_blank">Online Simulator</a></summary>

```python
ltr_label = lv.label(lv.screen_active())
ltr_label.set_text("In modern terminology, a microcontroller is similar to a system on a chip (SoC).")
ltr_label.set_style_text_font(lv.font_montserrat_16, 0);

ltr_label.set_width(310)
ltr_label.align(lv.ALIGN.TOP_LEFT, 5, 5)

rtl_label = lv.label(lv.screen_active())
rtl_label.set_text("מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).")
rtl_label.set_style_base_dir(lv.BASE_DIR.RTL, 0)
rtl_label.set_style_text_font(lv.font_dejavu_16_persian_hebrew, 0)
rtl_label.set_width(310)
rtl_label.align(lv.ALIGN.LEFT_MID, 5, 0)

font_hans_sans_16_cjk = lv.font_load("S:../../assets/font/lv_font_source_han_sans_sc_16_cjk.fnt")

cz_label = lv.label(lv.screen_active())
cz_label.set_style_text_font(font_hans_sans_16_cjk, 0)
cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")
cz_label.set_width(310)
cz_label.align(lv.ALIGN.BOTTOM_LEFT, 5, -5)

```
</details>

## :arrow_forward: Get started
This list will guide you to get started with LVGL step-by-step.

**Get Familiar with LVGL**

  1. Check the [Online demos](https://lvgl.io/demos) to see LVGL in action (3 minutes).
  2. Read the [Introduction](https://docs.lvgl.io/master/intro/index.html) page of the documentation (5 minutes).
  3. Get familiar with the basics on the [Quick overview](https://docs.lvgl.io/master/intro/getting_started.html#lvgl-basics) page (15 minutes).

**Start to Use LVGL**

  4. Set up a [Simulator](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html#simulator) (10 minutes).
  5. Try out some [Examples](https://github.com/lvgl/lvgl/tree/master/examples).
  6. Port LVGL to a board. See the [Porting](https://docs.lvgl.io/master/details/integration/adding-lvgl-to-your-project/index.html) guide or check out the ready-to-use [Projects](https://github.com/lvgl?q=lv_port_).

**Become a Pro**

  7. Read the [Main-Modules](https://docs.lvgl.io/master/details/main-modules/index.html) page to get a better understanding of the library (2-3 hours)
  8. Check the documentation of the [Widgets](https://docs.lvgl.io/master/details/widgets/index.html) to see their features and usage

**Get Help and Help Others**

  9. If you have questions go to the [Forum](http://forum.lvgl.io/)
  10. Read the [Contributing](https://docs.lvgl.io/master/contributing/index.html) guide to see how you can help to improve LVGL (15 minutes)


## :handshake: Services
LVGL LLC was established to provide a solid background for LVGL library and to offer several type of services to help you in UI development. With 15+ years of experience in the user interface and graphics industry we can help you the bring your UI to the next level.

- **Graphics design** Our in-house graphics designers are experts in creating beautiful modern designs which fit to your product and the resources of your hardware.
- **UI implementation** We can also implement your UI based on the design you or we have created. You can be sure that we will make the most out of your hardware and LVGL. If a feature or widget is missing from LVGL, don't worry, we will implement it for you.
- **Consulting and Support** We can support you with consulting as well to avoid pricey and time consuming mistakes during the UI development.
- **Board certification** For companies who are offering development boards, or production ready kits we do board certification which shows how board can run LVGL.


Check out our [Demos](https://lvgl.io/demos) as reference. For more information take look at the [Services page](https://lvgl.io/services).

[Contact us](https://lvgl.io/#contact) and tell how we can help.


## :star2: Contributing
LVGL is an open project and contribution is very welcome. There are many ways to contribute from simply speaking about your project, through writing examples, improving the documentation, fixing bugs or even hosting your own project under the LVGL organization.

For a detailed description of contribution opportunities visit the [Contributing](https://docs.lvgl.io/master/contributing/index.html) section of the documentation.

More than 300 people already left their fingerprint in LVGL. Be one them! See you here! :slightly_smiling_face:

<a href="https://github.com/lvgl/lvgl/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" />
</a>

... and many other.
