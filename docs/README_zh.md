<a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets/sponsor.png" height="32px"></a>

<p align="right">
  <a href="../README.md">English</a>  |  <b>中文</b> | <a href="./README_pt_BR.md">Português do Brasil</a> | <a href="./README_jp.md">日本語</a>
</p>
<br>

<p align="center">
  <img src="https://lvgl.io/github-assets/logo-colored.png" width=300px>
</p>


<h1 align="center">
  Light and Versatile Graphics Library
</h1>

<h1 align="center">
  轻量级通用型图形库
</h1>

<div align="center">
  <img src="https://raw.githubusercontent.com/kisvegabor/test/master/smartwatch_demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
</div>

<br>

<p align="center">
<a href="https://lvgl.io" title="Homepage of LVGL">官网 </a> |
<a href="https://docs.lvgl.io/" title="Detailed documentation with 100+ examples">文档</a> |
<a href="https://forum.lvgl.io" title="Get help and help others">论坛</a> |
<a href="https://lvgl.io/demos" title="Demos running in your browser">示例</a> |
<a href="https://lvgl.io/services" title="Graphics design, UI implementation and consulting">服务</a>
</p>


[中文宣传单](./flyers/LVGL-Chinese-Flyer.pdf)

## :ledger: 目录

- [:ledger: 目录](#ledger-目录)
- [:rocket: 概况与总览](#rocket-概况与总览)
  - [硬件要求](#硬件要求)
  - [:package: 已经支持的平台](#package-已经支持的平台)
- [如何入门](#如何入门)
- [:robot: 例程](#robot-例程)
  - [Button with Click Event 按钮与点击事件](#button-with-click-event-按钮与点击事件)
  - [Micropython](#micropython)
  - [Checkboxes with Layout 带布局的复选框](#checkboxes-with-layout-带布局的复选框)
  - [Styling a Slider 设置滑块的样式](#styling-a-slider-设置滑块的样式)
  - [English, Hebrew (mixed LRT-RTL) and Chinese texts 英语、希伯来语（ 双向文本排版 ）和中文](#english-hebrew-mixed-lrt-rtl-and-chinese-texts-英语希伯来语-双向文本排版-和中文)
- [:handshake: 服务](#handshake-服务)
- [:star: 如何向社区贡献](#star-如何向社区贡献)

## :rocket: 概况与总览

**特性**

-  丰富且强大的模块化[图形组件](https://docs.lvgl.io/master/details/widgets/index.html)：按钮
   (buttons)、图表 (charts)、列表 (lists)、滑动条 (sliders)、图片
   (images) 等
-  高级的图形引擎：动画、抗锯齿、透明度、平滑滚动、图层混合等效果
-  支持多种[输入设备](https://docs.lvgl.io/master/details/main-modules/indev.html)：触摸屏、键盘、编码器、按键等
-  支持[多显示设备](https://docs.lvgl.io/master/details/main-modules/display/index.html)
-  不依赖特定的硬件平台，可以在任何显示屏上运行
-  配置可裁剪（最低资源占用：64 kB Flash，16 kB RAM）
-  基于UTF-8的多语种支持，例如中文、日文、韩文、阿拉伯文等
-  可以通过[类CSS](https://docs.lvgl.io/master/details/common-widget-features/styles/style.html)的方式来设计、布局图形界面（例如：[Flexbox](https://docs.lvgl.io/master/details/common-widget-features/layouts/flex.html)、[Grid](https://docs.lvgl.io/master/details/common-widget-features/layouts/grid.html)）
-  支持操作系统、外置内存、以及硬件加速（LVGL已内建支持STM32 DMA2D、SWM341 DMA2D、NXP PXP和VGLite）
-  即便仅有[单缓冲区(frame buffer)](https://docs.lvgl.io/master/details/main-modules/display/index.html)的情况下，也可保证渲染如丝般顺滑
-  全部由C编写完成，并支持C++调用
-  支持Micropython编程，参见：[LVGL API in Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
-  支持[模拟器](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html) 仿真，可以无硬件依托进行开发
-  丰富详实的[例程](https://github.com/lvgl/lvgl/tree/master/examples)
-  详尽的[文档](http://docs.lvgl.io/) 以及API参考手册，可线上查阅或可下载为PDF格式

### 硬件要求

<table>
   <tr>
      <td><strong>要求</strong></td>
      <td><strong>最低要求</strong></td>
      <td><strong>建议要求</strong></td>
   </tr>
   <tr>
      <td><strong>架构</strong></td>
      <td colspan="2">16、32、64位微控制器或微处理器</td>
   </tr>
   <tr>
      <td><strong>时钟</strong></td>
      <td> &gt; 16 MHz</td>
      <td> &gt; 48 MHz</td>
   </tr>
   <tr>
      <td><strong>Flash/ROM</strong></td>
      <td> &gt; 64 kB</td>
      <td> &gt; 180 kB</td>
   </tr>
   <tr>
      <td><strong>Static RAM</strong></td>
      <td> &gt; 16 kB</td>
      <td> &gt; 48 kB</td>
   </tr>
   <tr>
      <td><strong>Draw buffer</strong></td>
      <td> &gt; 1 × <em>hor. res.</em> pixels</td>
      <td> &gt; 1/10屏幕大小 </td>
   </tr>
   <tr>
      <td><strong>编译器</strong></td>
      <td colspan="2">C99或更新 </td>
   </tr>
</table>

*注意：资源占用情况与具体硬件平台、编译器等因素有关，上表中仅给出参考值*

### :package: 已经支持的平台

LVGL本身并不依赖特定的硬件平台，任何满足LVGL硬件配置要求的微控制器均可运行LVGL。
如下仅列举其中一部分:

-  NXP: Kinetis, LPC, iMX, iMX RT
-  STM32F1, STM32F3, STM32F4, STM32F7, STM32L4, STM32L5, STM32H7
-  Microchip dsPIC33, PIC24, PIC32MX, PIC32MZ
-  [Linux frame buffer](https://blog.lvgl.io/2018-01-03/linux_fb) (/dev/fb)
-  [Espressif ESP32](https://github.com/lvgl/lv_port_esp32)
-  [Infineon Aurix](https://github.com/lvgl/lv_port_aurix)
-  Nordic NRF52 Bluetooth modules
-  Quectel modems
-  [SYNWIT SWM341](https://www.synwit.cn/)

LVGL也支持：
- [Arduino library](https://docs.lvgl.io/master/details/integration/framework/arduino.html)
- [PlatformIO package](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/kconfig.html#CONFIG_LVGL)
- [ESP32 component](https://docs.lvgl.io/master/details/integration/chip/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/details/integration/os/nuttx.html)
- [RT-Thread RTOS](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/lvgl-docs/introduction)



## 如何入门

请按照如下顺序来学习LVGL：
1. 使用[网页在线例程](https://lvgl.io/demos) 来体验LVGL（3分钟）
2. 阅读文档[简介](https://docs.lvgl.io/master/intro/introduction.html)章节来初步了解LVGL（5分钟）
3. 再来阅读一下文档快速[快速概览](https://docs.lvgl.io/master/intro/getting_started.html)章节来了解LVGL的基本知识（15分钟）
4. 学习如何使用[模拟器](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html)来在电脑上仿真LVGL（10分钟）
5. 试着动手实践一些[例程](https://github.com/lvgl/lvgl/tree/master/examples)
6. 参考[移植指南](https://docs.lvgl.io/master/details/integration/adding-lvgl-to-your-project/index.html)尝试将LVGL移植到一块开发板上，LVGL也已经提供了一些移植好的[工程](https://github.com/lvgl?q=lv_port_)
7. 仔细阅读文档[总览](https://docs.lvgl.io/master/details/main-modules/index.html)章节来更加深入的了解和熟悉LVGL（2-3小时）
8. 浏览文档[组件(Widgets)](https://docs.lvgl.io/master/details/widgets/index.html)章节来了解如何使用它们
9. 如果你有问题可以到LVGL[论坛](http://forum.lvgl.io/)提问
10. 阅读文档[如何向社区贡献](https://docs.lvgl.io/master/contributing/index.html)章节来看看你能帮LVGL社区做些什么，以促进LVGL软件质量的不断提高（15分钟）


## :robot: 例程

更多例程请参见 [examples](https://github.com/lvgl/lvgl/tree/master/examples) 文件夹。

### Button with Click Event 按钮与点击事件

![Button with Click Event](https://raw.githubusercontent.com/kisvegabor/test/master/readme_example_2.gif)

<details>
  <summary>C code</summary>

```c
lv_obj_t * btn = lv_button_create(lv_screen_active());       /*Add a button to the current screen*/
lv_obj_center(btn);                                          /*Set its position*/
lv_obj_set_size(btn, 100, 50);                               /*Set its size*/
lv_obj_add_event(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(btn);                     /*Add a label to the button*/
lv_label_set_text(label, "Button");                          /*Set the labels text*/
lv_obj_center(label);                                        /*Align the label to the center*/
...

void btn_event_cb(lv_event_t * e)
{
  printf("Clicked\n");
}
```

</details>


### Micropython

更多信息请到 [Micropython官网](https://docs.lvgl.io/master/get-started/bindings/micropython.html) 查询.

<details>
  <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=926bde43ec7af0146c486de470c53f11f167491e" target="_blank">Online Simulator</a> :gb:</summary>

```python
def btn_event_cb(e):
  print("Clicked")

# Create a Button and a Label
btn = lv.btn(lv.screen_active())
btn.center()
btn.set_size(100, 50)
btn.add_event(btn_event_cb, lv.EVENT.CLICKED, None)

label = lv.label(btn)
label.set_text("Button")
label.center()
```

</details>
<br>


### Checkboxes with Layout 带布局的复选框
![Checkboxes with layout in LVGL](https://raw.githubusercontent.com/kisvegabor/test/master/readme_example_3.gif)

<details>
  <summary>C code</summary>

```c

lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

lv_obj_t * cb;
cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Apple");
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Banana");
lv_obj_add_state(cb, LV_STATE_CHECKED);
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "Lemon");
lv_obj_add_state(cb, LV_STATE_DISABLED);
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);
lv_checkbox_set_text(cb, "Melon\nand a new line");
lv_obj_add_event(cb, event_handler, LV_EVENT_ALL, NULL);
```

</details>


<details>
  <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=311d37e5f70daf1cb0d2cad24c7f72751b5f1792" target="_blank">Online Simulator</a> :gb:</summary>

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


lv.scr_act().set_flex_flow(lv.FLEX_FLOW.COLUMN)
lv.scr_act().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Apple")
cb.add_event(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Banana")
cb.add_state(lv.STATE.CHECKED)
cb.add_event(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("Lemon")
cb.add_state(lv.STATE.DISABLED)
cb.add_event(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
cb.set_text("Melon")
cb.add_event(event_handler, lv.EVENT.ALL, None)
```

</details>


### Styling a Slider 设置滑块的样式
![Styling a slider with LVGL](https://raw.githubusercontent.com/kisvegabor/test/master/readme_example_4.gif)


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
<a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=c431c7b4dfd2cc0dd9c392b74365d5af6ea986f0" target="_blank">Online Simulator</a> :gb:
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


### English, Hebrew (mixed LRT-RTL) and Chinese texts 英语、希伯来语（ 双向文本排版 ）和中文

![English, Hebrew and Chinese texts with LVGL](https://raw.githubusercontent.com/kisvegabor/test/master/readme_example_5.png)

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
lv_obj_set_style_text_font(cz_label, &lv_font_simsun_16_cjk, 0);
lv_obj_set_width(cz_label, 310);
lv_obj_align(cz_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
```

</details>

<details>
  <summary>MicroPython code | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=18bb38200a64e10ead1aa17a65c977fc18131842" target="_blank">Online Simulator</a> :gb:</summary>

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

font_simsun_16_cjk = lv.font_load("S:../../assets/font/lv_font_simsun_16_cjk.fnt")

cz_label = lv.label(lv.screen_active())
cz_label.set_style_text_font(font_simsun_16_cjk, 0)
cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")
cz_label.set_width(310)
cz_label.align(lv.ALIGN.BOTTOM_LEFT, 5, -5)

```
</details>



## :handshake: 服务

LVGL
责任有限公司成立的目的是为了给用户使用LVGL图形库提供额外的技术支持，我们致力于提供以下服务：

-  图形设计
-  UI设计
-  技术咨询以及技术支持

更多信息请参见 https://lvgl.io/services ，如果有任何问题请随时联系我们。


## :star: 如何向社区贡献

LVGL是一个开源项目，非常欢迎您参与到社区贡献当中。您有很多种方式来为提高LVGL贡献您的一份力量，包括但不限于：

-  介绍你基于LVGL设计的作品或项目
-  写一些例程
-  修改以及完善文档
-  修复bug

请参见文档[如何向社区贡献](https://docs.lvgl.io/master/contributing/index.html)章节来获取更多信息。
