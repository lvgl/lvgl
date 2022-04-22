<h1 align="center"> LVGL - Light and Versatile Graphics Library</h1>
<h2 align="center"> LVGL - 轻量级通用型图形库</h2>



<p align="center">
<img src="https://lvgl.io/assets/images/lvgl_widgets_demo.gif">
</p>
<p align="center">
LVGL是一个高度可裁剪、低资源占用、界面美观且易用的嵌入式系统图形库
</p>


<h4 align="center">
<a href="https://lvgl.io">官网</a> &middot;
<a href="https://docs.lvgl.io/">文档</a> &middot;
<a href="https://forum.lvgl.io">论坛</a>  &middot;
<a href="https://lvgl.io/services">服务</a>  &middot;
<a href="https://docs.lvgl.io/master/examples.html">例程</a>
</h4>


[English](./README.md) | **中文** |


---

#### 目录
- [概况与总览](#概况与总览)
- [如何入门](#如何入门)
- [例程](#例程)
- [服务](#服务)
- [如何向社区贡献](#如何向社区贡献)

## 概况与总览
### 特性
* 丰富且强大的模块化[图形组件](https://docs.lvgl.io/master/widgets/index.html)：按钮 (buttons)、图表 (charts)、列表 (lists)、滑动条 (sliders)、图片 (images) 等
* 高级的图形引擎：动画、抗锯齿、透明度、平滑滚动、图层混合等效果
* 支持多种[输入设备](https://docs.lvgl.io/master/overview/indev.html)：触摸屏、 键盘、编码器、按键等
* 支持[多显示设备](https://docs.lvgl.io/master/overview/display.html)
* 不依赖特定的硬件平台，可以在任何显示屏上运行
* 配置可裁剪（最低资源占用：64 kB Flash，16 kB RAM）
* 基于UTF-8的多语种支持，例如中文、日文、韩文、阿拉伯文等
* 可以通过[类CSS](https://docs.lvgl.io/master/overview/style.html)的方式来设计、布局图形界面（例如：[Flexbox](https://docs.lvgl.io/master/layouts/flex.html)、[Grid](https://docs.lvgl.io/master/layouts/grid.html)）
* 支持操作系统、外置内存、以及硬件加速（LVGL已内建支持STM32 DMA2D、SWM341 DMA2D、NXP PXP和VGLite）
* 即便仅有[单缓冲区(frame buffer)](https://docs.lvgl.io/master/porting/display.html)的情况下，也可保证渲染如丝般顺滑
* 全部由C编写完成，并支持C++调用
* 支持Micropython编程，参见：[LVGL API in Micropython](https://blog.lvgl.io/2019-02-20/micropython-bindings)
* 支持[模拟器](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html)仿真，可以无硬件依托进行开发
* 丰富详实的[例程](https://github.com/lvgl/lvgl/tree/master/examples)
* 详尽的[文档](http://docs.lvgl.io/)以及API参考手册，可线上查阅或可下载为PDF格式

### 硬件要求

<table>
  <tr>
    <td> <strong>要求</strong> </td>
    <td><strong>最低要求</strong></td>
    <td><strong>建议要求</strong></td>
  </tr>
  <tr>
    <td><strong>架构</strong></td>
    <td colspan="2">16、32、64位微控制器或微处理器</td>
  </tr>
  <tr>
    <td> <strong>时钟</strong></td>
    <td> &gt; 16 MHz</td>
    <td> &gt; 48 MHz</td>
  </tr>

  <tr>
    <td> <strong>Flash/ROM</strong></td>
    <td> &gt; 64 kB </td>
    <td> &gt; 180 kB</td>
  </tr>

  <tr>
    <td> <strong>Static RAM</strong></td>
    <td> &gt; 16 kB </td>
    <td> &gt; 48 kB</td>
  </tr>

  <tr>
    <td> <strong>Draw buffer</strong></td>
    <td> &gt; 1 &times; <em>hor. res.</em> pixels </td>
    <td> &gt; 1/10屏幕大小 </td>
  </tr>

  <tr>
    <td> <strong>编译器</strong></td>
    <td colspan="2"> C99或更新 </td>
  </tr>
</table>

*注意：资源占用情况与具体硬件平台、编译器等因素有关，上表中仅给出参考值*

### 已经支持的平台
LVGL本身并不依赖特定的硬件平台，任何满足LVGL硬件配置要求的微控制器均可运行LVGL。
如下仅列举其中一部分:

- NXP: Kinetis, LPC, iMX, iMX RT
- STM32F1, STM32F3, STM32F4, STM32F7, STM32L4, STM32L5, STM32H7
- Microchip dsPIC33, PIC24, PIC32MX, PIC32MZ
- [Linux frame buffer](https://blog.lvgl.io/2018-01-03/linux_fb) (/dev/fb)
- [Raspberry Pi](http://www.vk3erw.com/index.php/16-software/63-raspberry-pi-official-7-touchscreen-and-littlevgl)
- [Espressif ESP32](https://github.com/lvgl/lv_port_esp32)
- [Infineon Aurix](https://github.com/lvgl/lv_port_aurix)
- Nordic NRF52 Bluetooth modules
- Quectel modems
- [SYNWIT SWM341](https://www.synwit.cn/)

LVGL也支持：
- [Arduino library](https://docs.lvgl.io/master/get-started/platforms/arduino.html)
- [PlatformIO package](https://platformio.org/lib/show/12440/lvgl)
- [Zephyr library](https://docs.zephyrproject.org/latest/reference/kconfig/CONFIG_LVGL.html)
- [ESP32 component](https://docs.lvgl.io/master/get-started/platforms/espressif.html)
- [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX library](https://docs.lvgl.io/master/get-started/os/nuttx.html)
- [RT-Thread RTOS](https://www.rt-thread.org/document/site/#/rt-thread-version/rt-thread-standard/packages-manual/lvgl-docs/introduction)


## 如何入门
请按照如下顺序来学习LVGL：
1. 使用[网页在线例程](https://lvgl.io/demos)来体验LVGL（3分钟）
2. 阅读文档[简介](https://docs.lvgl.io/master/intro/index.html)章节来初步了解LVGL（5分钟）
3. 再来阅读一下文档快速[快速概览](https://docs.lvgl.io/master/get-started/quick-overview.html)章节来了解LVGL的基本知识（15分钟）
4. 学习如何使用[模拟器](https://docs.lvgl.io/master/get-started/platforms/pc-simulator.html)来在电脑上仿真LVGL（10分钟）
5. 试着动手实践一些[例程](https://github.com/lvgl/lvgl/tree/master/examples)
6. 参考[移植指南](https://docs.lvgl.io/master/porting/index.html)尝试将LVGL移植到一块开发板上，LVGL也已经提供了一些移植好的[工程](https://github.com/lvgl?q=lv_port_)
7. 仔细阅读文档[总览](https://docs.lvgl.io/master/overview/index.html)章节来更加深入的了解和熟悉LVGL（2-3小时）
8. 浏览文档[组件(Widgets)](https://docs.lvgl.io/master/widgets/index.html)章节来了解如何使用它们
9. 如果你有问题可以到LVGL[论坛](http://forum.lvgl.io/)提问
10. 阅读文档[如何向社区贡献](https://docs.lvgl.io/master/CONTRIBUTING.html)章节来看看你能帮LVGL社区做些什么，以促进LVGL软件质量的不断提高（15分钟）

## 例程

更多例程请参见 [examples](https://github.com/lvgl/lvgl/tree/master/examples) 文件夹。

![LVGL button with label example](https://github.com/lvgl/lvgl/raw/master/docs/misc/btn_example.png)

### C
```c
lv_obj_t * btn = lv_btn_create(lv_scr_act());                   /*Add a button to the current screen*/
lv_obj_set_pos(btn, 10, 10);                                    /*Set its position*/
lv_obj_set_size(btn, 100, 50);                                  /*Set its size*/
lv_obj_add_event_cb(btn, btn_event_cb, LV_EVENT_CLICKED, NULL); /*Assign a callback to the button*/

lv_obj_t * label = lv_label_create(btn);                        /*Add a label to the button*/
lv_label_set_text(label, "Button");                             /*Set the labels text*/
lv_obj_center(label);                                           /*Align the label to the center*/
...

void btn_event_cb(lv_event_t * e)
{
  printf("Clicked\n");
}
```
### Micropython
更多信息请到 [Micropython官网](https://docs.lvgl.io/master/get-started/bindings/micropython.html) 查询.
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

## 服务
LVGL 责任有限公司成立的目的是为了给用户使用LVGL图形库提供额外的技术支持，我们致力于提供以下服务：

- 图形设计
- UI设计
- 技术咨询以及技术支持

更多信息请参见 https://lvgl.io/services ，如果有任何问题请随时联系我们。


## 如何向社区贡献
LVGL是一个开源项目，非常欢迎您参与到社区贡献当中。您有很多种方式来为提高LVGL贡献您的一份力量，包括但不限于：

- 介绍你基于LVGL设计的作品或项目
- 写一些例程
- 修改以及完善文档
- 修复bug

请参见文档[如何向社区贡献](https://docs.lvgl.io/master/CONTRIBUTING.html)章节来获取更多信息。
