
# Introduction

LVGL (Light and Versatile Graphics Library) is a free and open-source graphics library providing everything you need to create an embedded GUI with easy-to-use graphical elements, beautiful visual effects and a low memory footprint.


## Key features
- Powerful building blocks such as buttons, charts, lists, sliders, images, etc.
- Advanced graphics with animations, anti-aliasing, opacity, smooth scrolling
- Various input devices such as touchpad, mouse, keyboard, encoder, etc.
- Multi-language support with UTF-8 encoding
- Multi-display support, i.e. use multiple TFT, monochrome displays simultaneously
- Fully customizable graphic elements with CSS-like styles
- Hardware independent: use with any microcontroller or display
- Scalable: able to operate with little memory (64 kB Flash, 16 kB RAM)
- OS, external memory and GPU are supported but not required
- Single frame buffer operation even with advanced graphic effects
- Written in C for maximal compatibility (C++ compatible)
- Simulator to start embedded GUI design on a PC without embedded hardware
- Binding to MicroPython
- Tutorials, examples, themes for rapid GUI design
- Documentation is available online and as PDF
- Free and open-source under MIT license

## Requirements
Basically, every modern controller which is able to drive a display is suitable to run LVGL. The minimal requirements are:
<ul>
<li> 16, 32 or 64 bit microcontroller or processor</li>
<li>&gt; 16 MHz clock speed is recommended</li>
<li> Flash/ROM: &gt; 64 kB for the very essential components (&gt; 180 kB is recommended)</li>
<li> RAM:
  <ul>
    <li> Static RAM usage: ~2 kB depending on the used features and object types</li>
    <li> Stack: &gt; 2kB (&gt; 8 kB is recommended)</li>
    <li> Dynamic data (heap): &gt; 4 KB (&gt; 48 kB is recommended if using several objects).
	    Set by <em>LV_MEM_SIZE</em> in <em>lv_conf.h</em>. </li>
    <li> Display buffer:  &gt; <em>"Horizontal resolution"</em> pixels (&gt; 10 &times; <em>"Horizontal resolution"</em> is recommended) </li>
    <li> One frame buffer in the MCU or in an external display controller</li>
	</ul>
</li>
<li> C99 or newer compiler</li>
<li> Basic C (or C++) knowledge:
          <a href="https://www.tutorialspoint.com/cprogramming/c_pointers.htm">pointers</a>,
          <a href="https://www.tutorialspoint.com/cprogramming/c_structures.htm">structs</a>,
          <a href="https://www.geeksforgeeks.org/callbacks-in-c/">callbacks</a>.</li>
</ul>
<em>Note that memory usage may vary depending on architecture, compiler and build options.</em>

## License
The LVGL project (including all repositories) is licensed under [MIT license](https://github.com/lvgl/lvgl/blob/master/LICENCE.txt).
This means you can use it even in commercial projects.

It's not mandatory, but we highly appreciate it if you write a few words about your project in the [My projects](https://forum.lvgl.io/c/my-projects/10) category of the forum or a private message to [lvgl.io](https://lvgl.io/#contact).

Although you can get LVGL for free there is a massive amount of work behind it. It's created by a group of volunteers who made it available for you in their free time.

To make the LVGL project sustainable, please consider [contributing](/CONTRIBUTING) to the project.
You can choose from [many different ways of contributing](/CONTRIBUTING) such as simply writing a tweet about you using LVGL, fixing bugs, translating the documentation, or even becoming a maintainer.

## Repository layout
All repositories of the LVGL project are hosted on GitHub: https://github.com/lvgl

You will find these repositories there:
- [lvgl](https://github.com/lvgl/lvgl) The library itself with many [examples](https://github.com/lvgl/lvgl/blob/master/examples/) and [demos](https://github.com/lvgl/lvgl/blob/master/demos/).
- [lv_drivers](https://github.com/lvgl/lv_drivers) Display and input device drivers
- [blog](https://github.com/lvgl/blog) Source of the blog's site (https://blog.lvgl.io)
- [sim](https://github.com/lvgl/sim) Source of the online simulator's site (https://sim.lvgl.io)
- [lv_port_...](https://github.com/lvgl?q=lv_port&type=&language=) LVGL ports to development boards or environments
- [lv_binding_..](https://github.com/lvgl?q=lv_binding&type=&language=l) Bindings to other languages

## Release policy

The core repositories follow the rules of [Semantic versioning](https://semver.org/):
- Major versions for incompatible API changes. E.g. v5.0.0, v6.0.0
- Minor version for new but backward-compatible functionalities. E.g. v6.1.0, v6.2.0
- Patch version for backward-compatible bug fixes. E.g. v6.1.1, v6.1.2

Tags like `vX.Y.Z` are created for every release.

### Release cycle
- Bug fixes: Released on demand even weekly
- Minor releases: Every 3-4 months
- Major releases: Approximately yearly

### Branches
The core repositories have at least the following branches:
- `master` latest version, patches are merged directly here.
- `release/vX.Y` stable versions of the minor releases
- `fix/some-description` temporary branches for bug fixes
- `feat/some-description` temporary branches for features


### Changelog

The changes are recorded in [CHANGELOG.md](/CHANGELOG).

### Version support
Before v8 the last minor release of each major series was supported for 1 year.
Starting from v8, every minor release is supported for 1 year.

| Version | Release date | Support end | Active |
|---------|--------------|-------------|--------|
| v5.3    | Feb 1, 2019  |Feb 1, 2020  | No     |
| v6.1    | Nov 26, 2019 |Nov 26, 2020 | No     |
| v7.11   | Mar 16, 2021 |Mar 16, 2022 | No     |
| v8.0    | 1 Jun, 2021  |1 Jun, 2022  | Yes    |
| v8.1    | 10 Nov, 2021 |10 Nov, 2022 | Yes    |
| v8.2    | 31 Jan, 2022 |31 Jan, 2023 | Yes    |
| v8.3    | In progress  |             |        |

## FAQ

### Where can I ask questions?
You can ask questions in the forum: [https://forum.lvgl.io/](https://forum.lvgl.io/).

We use [GitHub issues](https://github.com/lvgl/lvgl/issues) for development related discussion.
You should use them only if your question or issue is tightly related to the development of the library.

Before posting a question, please ready this FAQ section as you might find answer to your issue here too.

### Is my MCU/hardware supported?
Every MCU which is capable of driving a display via parallel port, SPI, RGB interface or anything else and fulfills the [Requirements](#requirements) is supported by LVGL.

This includes:
- "Common" MCUs like STM32F, STM32H, NXP Kinetis, LPC, iMX, dsPIC33, PIC32, SWM341 etc.
- Bluetooth, GSM, Wi-Fi modules like Nordic NRF and Espressif ESP32
- Linux with frame buffer device such as /dev/fb0. This includes Single-board computers like the Raspberry Pi
- Anything else with a strong enough MCU and a peripheral to drive a display

### Is my display supported?
LVGL needs just one simple driver function to copy an array of pixels into a given area of the display.
If you can do this with your display then you can use it with LVGL.

Some examples of the supported display types:
- TFTs with 16 or 32 bit color depth
- Monitors with an HDMI port
- Small monochrome displays
- Gray-scale displays
- even LED matrices
- or any other display where you can control the color/state of the pixels

See the [Porting](/porting/display) section to learn more.

### LVGL doesn't start, randomly crashes or nothing is drawn on the display. What can be the problem?
- Try increasing `LV_MEM_SIZE`.
- Be sure `lv_disp_drv_t`, `lv_indev_drv_t` and `lv_fs_drv_t` are global or `static`.
- Be sure your display works without LVGL. E.g. paint it to red on start up.
- Enable [Logging](porting/log)
- Enable asserts in `lv_conf.h` (`LV_USE_ASSERT_...`)
- If you use an RTOS
   - increase the stack size of the task which calls `lv_timer_handler()`
   - Be sure you used a mutex as [described here](/porting/os)

### My display driver is not called. What have I missed?
Be sure you are calling `lv_tick_inc(x)` in an interrupt and `lv_timer_handler()` in your main `while(1)`.

Learn more in the [Tick](/porting/tick) and [Timer handler](/porting/timer-handler) sections.

### Why is the display driver called only once? Only the upper part of the display is refreshed.
Be sure you are calling `lv_disp_flush_ready(drv)` at the end of your "*display flush callback*".

### Why do I see only garbage on the screen?
Probably there a bug in your display driver. Try the following code without using LVGL. You should see a square with red-blue gradient.

```c
#define BUF_W 20
#define BUF_H 10

lv_color_t buf[BUF_W * BUF_H];
lv_color_t * buf_p = buf;
uint16_t x, y;
for(y = 0; y &lt; BUF_H; y++) {
    lv_color_t c = lv_color_mix(LV_COLOR_BLUE, LV_COLOR_RED, (y * 255) / BUF_H);
    for(x = 0; x &lt; BUF_W; x++){
        (*buf_p) =  c;
        buf_p++;
    }
}

lv_area_t a;
a.x1 = 10;
a.y1 = 40;
a.x2 = a.x1 + BUF_W - 1;
a.y2 = a.y1 + BUF_H - 1;
my_flush_cb(NULL, &a, buf);
```

### Why do I see nonsense colors on the screen?
Probably LVGL's color format is not compatible with your display's color format. Check `LV_COLOR_DEPTH` in *lv_conf.h*.

If you are using 16-bit colors with SPI (or another byte-oriented interface) you probably need to set `LV_COLOR_16_SWAP  1` in *lv_conf.h*.
It swaps the upper and lower bytes of the pixels.

### How to speed up my UI?
- Turn on compiler optimization and enable cache if your MCU has it
- Increase the size of the display buffer
- Use two display buffers and flush the buffer with DMA (or similar peripheral) in the background
- Increase the clock speed of the SPI or parallel port if you use them to drive the display
- If your display has an SPI port consider changing to a model with a parallel interface because it has much higher throughput
- Keep the display buffer in internal RAM (not in external SRAM) because LVGL uses it a lot and it should have a fast access time

### How to reduce flash/ROM usage?
You can disable all the unused features (such as animations, file system, GPU etc.) and object types in *lv_conf.h*.

If you are using GCC/CLANG you can add `-fdata-sections -ffunction-sections` compiler flags and `--gc-sections` linker flag to remove unused functions and variables from the final binary. If possible, add the `-flto` compiler flag to enable link-time-optimisation together with `-Os` for GCC or `-Oz` for CLANG. 

### How to reduce the RAM usage
- Lower the size of the *Display buffer*
- Reduce `LV_MEM_SIZE` in *lv_conf.h*. This memory is used when you create objects like buttons, labels, etc.
- To work with lower `LV_MEM_SIZE` you can create objects only when required and delete them when they are not needed anymore

### How to work with an operating system?

To work with an operating system where tasks can interrupt each other (preemptively) you should protect LVGL related function calls with a mutex.
See the [Operating system and interrupts](/porting/os) section to learn more.
