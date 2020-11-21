<h1 align="center"> LVGL - Light and Versatile Graphics Library</h1> 

<p align="center">
<img src="https://lvgl.io/assets/images/img_1.png">
</p>

<p align="center">
LVGL provides everything you need to create embedded GUI with easy-to-use graphical elements, beautiful visual effects and low memory footprint. 
</p>

<h4 align="center">
<a href="https://lvgl.io">Website </a> &middot; 
<a href="https://lvgl.io/demos">Online demo</a> &middot; 
<a href="https://lvgl.github.io/lv_examples/">Nightly demos</a> &middot; 
<a href="https://docs.lvgl.io/">Docs</a> &middot; 
<a href="https://forum.lvgl.io">Forum</a>
</h4>

---

## Features
* Powerful [building blocks](https://docs.lvgl.io/latest/en/html/widgets/index.html): buttons, charts, lists, sliders, images, etc.
* Advanced graphics: animations, anti-aliasing, opacity, smooth scrolling
* Use [various input devices](https://docs.lvgl.io/latest/en/html/overview/indev.html): touchscreen, mouse, keyboard, encoder, buttons, etc.
* Use [multiple displays](https://docs.lvgl.io/latest/en/html/overview/display.html): e.g. monochrome and color display
* Hardware independent to use with any microcontroller or display
* Scalable to operate with little memory (64 kB Flash, 10 kB RAM)
* Multi-language support with UTF-8 handling, Bidirectional and Arabic script support
* Fully customizable graphical elements via [CSS-like styles](https://docs.lvgl.io/latest/en/html/overview/style.html)
* OS, External memory and GPU are supported but not required
* Smooth rendering even with a [single frame buffer](https://docs.lvgl.io/latest/en/html/porting/display.html)
* Written in C for maximal compatibility (C++ compatible)
* Micropython Binding exposes [LVGL API in Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
* [Simulator](https://docs.lvgl.io/latest/en/html/get-started/pc-simulator.html) to develop on PC without embedded hardware
* [Examples](lv_examples) and tutorials for rapid development
* [Documentation](http://docs.lvgl.io/) and API references

## Requirements
Basically, every modern controller (which is able to drive a display) is suitable to run LVGL. The minimal requirements are:

<table>
  <tr>
    <td> <strong>Name</strong> </td>
    <td><strong>Minimal</strong></td>
    <td><strong>Recommended</strong></td>
  </tr>
  <tr>
    <td><strong>Architecture</strong></td>
    <td colspan="2">16, 32 or 64 bit microcontroller or processor</td>
  </tr>
  <tr>
    <td> <strong>Clock</strong></td>
    <td> &gt; 16 MHz </td>
    <td> &gt; 48 MHz</td>
  </tr>
  
  <tr>
    <td> <strong>Flash/ROM</strong></td>
    <td> &gt; 64 kB </td>
    <td> &gt; 180 kB</td>
  </tr>
  
  <tr>
    <td> <strong>Static RAM</strong></td>
    <td> &gt; 2 kB </td>
    <td> &gt; 4 kB</td>
  </tr>
  
  <tr>
    <td> <strong>Stack</strong></td>
    <td> &gt; 2 kB </td>
    <td> &gt; 8 kB</td>
  </tr>
  
  <tr>
    <td> <strong>Heap</strong></td>
    <td> &gt; 2 kB </td>
    <td> &gt; 8 kB</td>
  </tr>
  
  <tr>
    <td> <strong>Display buffer</strong></td>
    <td> &gt; 1 &times; <em>hor. res.</em> pixels </td>
    <td> &gt; 10 &times; <em>hor. res.</em> pixels </td>
  </tr>
  
  <tr>
    <td> <strong>Compiler</strong></td>
    <td colspan="2"> C99 or newer </td>
  </tr>
</table>

*Note that the memory usage might vary depending on the architecture, compiler and build options.*

Just to mention some platforms:
- STM32F1, STM32F3, STM32F4, STM32F7, STM32L4, STM32L5, STM32H7
- Microchip dsPIC33, PIC24, PIC32MX, PIC32MZ
- NXP: Kinetis, LPC, iMX, iMX RT
- [Linux frame buffer](https://blog.lvgl.io/2018-01-03/linux_fb) (/dev/fb)
- [Raspberry Pi](http://www.vk3erw.com/index.php/16-software/63-raspberry-pi-official-7-touchscreen-and-littlevgl)
- [Espressif ESP32](https://github.com/lvgl/lv_port_esp32)
- [Infineon Aurix](https://github.com/lvgl/lv_port_aurix)
- Nordic NRF52 Bluetooth modules
- Quectel modems

## Get started
This list shows the recommended way of learning the library:
1. Check the [Online demos](https://lvgl.io/demos) to see LVGL in action (3 minutes)
2. Read the [Introduction](https://docs.lvgl.io/latest/en/html/intro/index.html) page of the documentation (5 minutes)
3. Get familiar with the basics on the [Quick overview](https://docs.lvgl.io/latest/en/html/get-started/quick-overview.html) page (15 minutes)
4. Set up a [Simulator](https://docs.lvgl.io/latest/en/html/get-started/pc-simulator.html) (10 minutes)
5. Try out some [Examples](https://github.com/lvgl/lv_examples/)
6. Port LVGL to a board. See the [Porting](https://docs.lvgl.io/latest/en/html/porting/index.html) guide or check the ready to use [Projects](https://github.com/lvgl?q=lv_port_&type=&language=)
7. Read the [Overview](https://docs.lvgl.io/latest/en/html/overview/index.html) page to get a better understanding of the library (2-3 hours)
8. Check the documentation of the [Widgets](https://docs.lvgl.io/latest/en/html/widgets/index.html) to see their features and usage
9. If you have questions go to the [Forum](http://forum.lvgl.io/)
10. Read the [Contributing](https://docs.lvgl.io/latest/en/html/contributing/index.html) guide to see how you can help to improve LVGL (15 minutes) 

## Examples 

For more examples see the [lv_examples](https://github.com/lvgl/lv_examples) repository.

### Button with label
```c
lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button to the current screen*/
lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
lv_obj_set_size(btn, 100, 50);                          /*Set its size*/
lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
lv_label_set_text(label, "Button");                     /*Set the labels text*/

...

void btn_event_cb(lv_obj_t * btn, lv_event_t event)
{
    if(event == LV_EVENT_CLICKED) {
        printf("Clicked\n");
    }
}
```
![LVGL button with label example](https://raw.githubusercontent.com/lvgl/docs/latest/misc/simple_button_example.gif)

### LVGL from Micropython
Learn more about [Micropython](https://docs.lvgl.io/latest/en/html/get-started/micropython.html).
```python
# Create a Button and a Label
scr = lv.obj()
btn = lv.btn(scr)
btn.align(lv.scr_act(), lv.ALIGN.CENTER, 0, 0)
label = lv.label(btn)
label.set_text("Button")

# Load the screen
lv.scr_load(scr)
```

## Contributing
LVGL is an open project and contribution is very welcome. There are many ways to contribute from simply speaking about your project, through writing examples, improving the documentation, fixing bugs to hosing your own project under in LVGL.

For a detailed description of contribution opportunities visit the [Contributing](https://docs.lvgl.io/latest/en/html/contributing/index.html) section of the documentation.
