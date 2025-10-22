<p align="right">
  <b>English</b> | <a href="./docs/README_zh.md">中文</a> | <a href="./docs/README_pt_BR.md">Português do Brasil</a> | <a href="./docs/README_jp.md">日本語</a> | <a href="./docs/README_he.md">עברית</a>
</p>

<br>

<p align="center">
  <img src="https://lvgl.io/github-assets/logo-colored.png" width=300px>
</p>

<h1 align="center">Light and Versatile Graphics Library</h1>

<br/>

<div align="center">
  <img src="https://lvgl.io/github-assets/smartwatch-demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
</div>

<br/>

<p align="center">
  <a href="https://lvgl.io" title="Homepage of LVGL">Website</a> |
  <a href="https://pro.lvgl.io" title="LVGL Pro XML based UI Editor">LVGL Pro Editor</a> |
  <a href="https://docs.lvgl.io/" title="Detailed documentation with 100+ examples">Docs</a> |
  <a href="https://forum.lvgl.io" title="Get help and help others">Forum</a> |
  <a href="https://lvgl.io/demos" title="Demos running in your browser">Demos</a> |
<a href="https://lvgl.io/services" title="Graphics design, UI implementation and consulting">Services</a>
</p>

<br/>

### Table of Contents
<p>
  <a href="#ledger-overview">Overview</a> <br/>
  <a href="#-features">Features</a> <br/>
  <a href="#%EF%B8%8F-platform-support">Platform Support</a> <br/>
  <a href="#-lvgl-pro-editor">LVGL Pro Editor</a> <br/>
  <a href="#-commercial-services">Commercial Services</a> <br/>
  <a href="#%E2%80%8D-integrating-lvgl">Integrating LVGL</a> <br/>
  <a href="#-examples">Examples</a> <br/>
  <a href="#-contributing">Contributing</a>
</p>

<br/>

## 📒 Overview

**LVGL** is a free and open-source UI library that enables you to create graphical user interfaces
for any MCUs and MPUs from any vendor on any platform.

**Requirements**: LVGL has no external dependencies, which makes it easy to compile for any modern target,
from small MCUs to multi-core Linux-based MPUs with 3D support. For a simple UI, you need only ~100kB RAM,
~200–300kB flash, and a buffer size of 1/10 of the screen for rendering.

**To get started**, pick a ready-to-use VSCode, Eclipse, or any other project and try out LVGL
on your PC. The LVGL UI code is fully platform-independent, so you can use the same UI code
on embedded targets too.

**LVGL Pro** is a complete toolkit to help you build, test, share, and ship UIs faster.
It comes with an XML Editor where you can quickly create and test reusable components,
export C code, or load the XMLs at runtime. Learn more here.

## 💡 Features

**Free and Portable**
  - A fully portable C (C++ compatible) library with no external dependencies.
  - Can be compiled for any MCU or MPU, with any (RT)OS. Make, CMake, and simple globbing are all supported.
  - Supports monochrome, ePaper, OLED, or TFT displays, or even monitors. [Displays](https://docs.lvgl.io/master/details/main-modules/display/index.html)
  - Distributed under the MIT license, so you can easily use it in commercial projects too.
  - Needs only 32kB RAM and 128kB Flash, a frame buffer, and at least a 1/10 screen-sized buffer for rendering.
  - OS, external memory, and GPU are supported but not required.

**Widgets, Styles, Layouts, and More**
  - 30+ built-in [Widgets](https://docs.lvgl.io/master/details/widgets/index.html): Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table, and many more.
  - Flexible [Style system](https://docs.lvgl.io/master/details/common-widget-features/styles/index.html) with ~100 style properties to customize any part of the widgets in any state.
  - [Flexbox](https://docs.lvgl.io/master/details/common-widget-features/layouts/flex.html) and [Grid](https://docs.lvgl.io/master/details/common-widget-features/layouts/grid.html)-like layout engines to automatically size and position the widgets responsively.
  - Text is rendered with UTF-8 encoding, supporting CJK, Thai, Hindi, Arabic, and Persian writing systems.
  - [Data bindings](https://docs.lvgl.io/master/details/auxiliary-modules/observer/index.html) to easily connect the UI with the application.
  - Rendering engine supports animations, anti-aliasing, opacity, smooth scrolling, shadows, image transformation, etc.
  - [Powerful 3D rendering engine](https://docs.lvgl.io/master/details/libs/gltf.html) to show [glTF models](https://sketchfab.com/) with OpenGL.
  - Supports Mouse, Touchpad, Keypad, Keyboard, External buttons, Encoder [Input devices](https://docs.lvgl.io/master/details/main-modules/indev.html).
  - [Multiple display](https://docs.lvgl.io/master/details/main-modules/display/overview.html#how-many-displays-can-lvgl-use) support.

## 📦️ Platform Support

LVGL has no external dependencies, so it can be easily compiled for any devices and it's  also available in many package managers and RTOSes:

- [Arduino library](https://docs.lvgl.io/master/details/integration/framework/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.lvgl.io/master/details/integration/os/zephyr.html)
- [ESP-IDF(ESP32) component](https://components.espressif.com/components/lvgl/lvgl)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/details/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/details/integration/os/rt-thread.html)
- CMSIS-Pack
- [RIOT OS package](https://doc.riot-os.org/group__pkg__lvgl.html#details)

## 🚀 LVGL Pro Editor

LVGL Pro is a complete toolkit to build, test, share, and ship embedded UIs efficiently.

It consists of four tightly related tools:

1. **XML Editor**: The heart of LVGL Pro. A desktop app to build components and screens in XML, manage data bindings, translations, animations, tests, and more. Learn more about the [XML Format](https://docs.lvgl.io/master/details/xml/xml/index.html) and the [Editor](https://docs.lvgl.io/master/details/xml/editor/index.html).
2. **Online Viewer**: Run the Editor in your browser, open GitHub projects, and share easily without setting up a developer environment. Visit [https://viewer.lvgl.io](https://viewer.lvgl.io).
3. **CLI Tool**: Generate C code and run tests in CI/CD. See the details [here](https://docs.lvgl.io/master/details/xml/tools/cli.html).
4. **Figma Plugin**: Sync and extract styles directly from Figma. See how it works [here](https://docs.lvgl.io/master/details/xml/tools/figma.html).

Together, these tools let developers build UIs efficiently, test them reliably, and collaborate with team members and customers.

Learn more at https://pro.lvgl.io

## 🤝 Commercial Services

LVGL LLC provides several types of commercial services to help you with UI development. With 15+ years of experience in the user interface and graphics industry, we can help bring your UI to the next level.

- **Graphics design**: Our in-house graphic designers are experts in creating beautiful modern designs that fit your product and the capabilities of your hardware.
- **UI implementation**: We can implement your UI based on the design you or we have created. You can be sure that we will make the most of your hardware and LVGL. If a feature or widget is missing from LVGL, don't worry, we will implement it for you.
- **Consulting and Support**: We also offer consulting to help you avoid costly and time-consuming mistakes during UI development.
- **Board certification**: For companies offering development boards or production-ready kits, we provide board certification to show how the board can run LVGL.

Check out our [Demos](https://lvgl.io/demos) as references. For more information, take a look at the [Services page](https://lvgl.io/services).

[Contact us](https://lvgl.io/#contact) and tell us how we can help.

## 🧑‍💻 Integrating LVGL

Integrating LVGL is very simple. Just drop it into any project and compile it as you would compile other files.
To configure LVGL, copy `lv_conf_template.h` as `lv_conf.h`, enable the first `#if 0`, and adjust the configs as needed.
(The default config is usually fine.) If available, LVGL can also be used with Kconfig.

Once in the project, you can initialize LVGL and create display and input devices as follows:

```c
#include "lvgl/lvgl.h" /*Define LV_LVGL_H_INCLUDE_SIMPLE to include as "lvgl.h"*/

#define TFT_HOR_RES 320
#define TFT_VER_RES 240

static uint32_t my_tick_cb(void)
{
    return my_get_millisec();
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /*Write px_map to the area->x1, area->x2, area->y1, area->y2 area of the
     *frame buffer or external display controller. */
}

static void my_touch_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
   if(my_touch_is_pressed()) {
       data->point.x = touchpad_x;
       data->point.y = touchpad_y;
       data->state = LV_INDEV_STATE_PRESSED;
   } else {
       data->state = LV_INDEV_STATE_RELEASED;
   }
}

void main(void)
{
    my_hardware_init();

    /*Initialize LVGL*/
    lv_init();

    /*Set millisecond-based tick source for LVGL so that it can track time.*/
    lv_tick_set_cb(my_tick_cb);

    /*Create a display where screens and widgets can be added*/
    lv_display_t * display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);

    /*Add rendering buffers to the screen.
     *Here adding a smaller partial buffer assuming 16-bit (RGB565 color format)*/
    static uint8_t buf[TFT_HOR_RES * TFT_VER_RES / 10 * 2]; /* x2 because of 16-bit color depth */
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*Add a callback that can flush the content from `buf` when it has been rendered*/
    lv_display_set_flush_cb(display, my_flush_cb);

    /*Create an input device for touch handling*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read_cb);

    /*The drivers are in place; now we can create the UI*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_center(label);

    /*Execute the LVGL-related tasks in a loop*/
    while(1) {
        lv_timer_handler();
        my_sleep_ms(5);         /*Wait a little to let the system breathe*/
    }
}
```

## 🤖 Examples

You can check out more than 100 examples at https://docs.lvgl.io/master/examples.html

The Online Viewer also contains tutorials to easily learn XML: https://viewer.lvgl.io/


### Hello World Button with an Event

<img width="311" height="232" alt="image" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />

<details>
  <summary>C code</summary>

  ```c
static void button_clicked_cb(lv_event_t * e)
{
	printf("Clicked\n");
}

[...]

  lv_obj_t * button = lv_button_create(lv_screen_active());
  lv_obj_center(button);
  lv_obj_add_event_cb(button, button_clicked_cb, LV_EVENT_CLICKED, NULL);

  lv_obj_t * label = lv_label_create(button);
  lv_label_set_text(label, "Hello from LVGL!");
```
</details>

<details>
  <summary>In XML with LVGL Pro</summary>

```xml
<screen>
	<view>
		<lv_button align="center">
			<event_cb callback="button_clicked_cb" />
			<lv_label text="Hello from LVGL!" />
		</lv_button>
	</view>
</screen>
```

</details>

### Styled Slider with Data-binding

<img width="314" height="233" alt="image" src="https://github.com/user-attachments/assets/268db1a0-946c-42e2-aee4-9550bdf5f4f9" />

<details>
  <summary>C code</summary>

```c
static void my_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
	printf("Slider value: %d\n", lv_subject_get_int(subject));
}

[...]

static lv_subject_t subject_value;
lv_subject_init_int(&subject_value, 35);
lv_subject_add_observer(&subject_value, my_observer_cb, NULL);

lv_style_t style_base;
lv_style_init(&style_base);
lv_style_set_bg_color(&style_base, lv_color_hex(0xff8800));
lv_style_set_bg_opa(&style_base, 255);
lv_style_set_radius(&style_base, 4);

lv_obj_t * slider = lv_slider_create(lv_screen_active());
lv_obj_center(slider);
lv_obj_set_size(slider, lv_pct(80), 16);
lv_obj_add_style(slider, &style_base, LV_PART_INDICATOR);
lv_obj_add_style(slider, &style_base, LV_PART_KNOB);
lv_obj_add_style(slider, &style_base, 0);
lv_obj_set_style_bg_opa(slider, LV_OPA_50, 0);
lv_obj_set_style_border_width(slider, 3, LV_PART_KNOB);
lv_obj_set_style_border_color(slider, lv_color_hex3(0xfff), LV_PART_KNOB);
lv_slider_bind_value(slider, &subject_value);

lv_obj_t * label = lv_label_create(lv_screen_active());
lv_obj_align(label, LV_ALIGN_CENTER, 0, -30);
lv_label_bind_text(label, &subject_value, "Temperature: %d °C");
```

</details>

<details>
  <summary>In XML with LVGL Pro</summary>

```xml
<screen>
	<styles>
		<style name="style_base" bg_opa="100%" bg_color="0xff8800" radius="4" />
		<style name="style_border" border_color="0xfff" border_width="3" />
	</styles>

	<view>
		<lv_label bind_text="value" bind_text-fmt="Temperature: %d °C" align="center" y="-30" />
		<lv_slider align="center" bind_value="value" style_bg_opa="30%">
			<style name="style_base" />
			<style name="style_base" selector="knob" />
			<style name="style_base" selector="indicator" />
			<style name="style_border" selector="knob" />
		</lv_slider>
	</view>
</screen>
```

</details>

### Checkboxes in a Layout

<img width="311" height="231" alt="image" src="https://github.com/user-attachments/assets/ba9af647-2ea1-4bc8-b53d-c7b43ce24b6e" />

<details>
  <summary>C code</summary>

  ```c
/*Create a new screen and load it*/
lv_obj_t * scr = lv_obj_create(NULL);
lv_screen_load(scr);

/*Set a column layout*/
lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, /*Vertical alignment*/
						   LV_FLEX_ALIGN_START,	       /*Horizontal alignment in the track*/
						   LV_FLEX_ALIGN_CENTER);      /*Horizontal alignment of the track*/

/*Create 5 checkboxes*/
const char * texts[5] = {"Input 1", "Input 2", "Input 3", "Output 1", "Output 2"};
for(int i = 0; i < 5; i++) {
	lv_obj_t * cb = lv_checkbox_create(scr);
	lv_checkbox_set_text(cb, texts[i]);
}

/*Change some states*/
lv_obj_add_state(lv_obj_get_child(scr, 1), LV_STATE_CHECKED);
lv_obj_add_state(lv_obj_get_child(scr, 3), LV_STATE_DISABLED);
```

</details>

<details>
  <summary>In XML with LVGL Pro</summary>

```xml
<screen>
	<view
		flex_flow="column"
		style_flex_main_place="space_evenly"
		style_flex_cross_place="start"
		style_flex_track_place="center"
	>
		<lv_checkbox text="Input 1"/>
		<lv_checkbox text="Input 2"/>
		<lv_checkbox text="Input 3" checked="true"/>
		<lv_checkbox text="Output 1"/>
		<lv_checkbox text="Output 2" disabled="true"/>
   </view>
</screen>
```

</details>


## 🌟 Contributing

LVGL is an open project, and contributions are very welcome. There are many ways to contribute, from simply speaking about your project, writing examples, improving the documentation, fixing bugs, or even hosting your own project under the LVGL organization.

For a detailed description of contribution opportunities, visit the [Contributing](https://docs.lvgl.io/master/contributing/index.html)
section of the documentation.

More than 600 people have already left their fingerprint on LVGL. Be one of them! See you here! 🙂

<a href="https://github.com/lvgl/lvgl/graphs/contributors"> <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" /> </a>

... and many more.
