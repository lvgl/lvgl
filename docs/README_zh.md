<p align="center">
  <a href="https://lvgl.io/docs" title="Documentation">文档</a> •
  <a href="https://forum.lvgl.io" title="Community forum">论坛</a> •
  <a href="https://blog.lvgl.io" title="News and articles">博客</a> •
  <a href="https://lvgl.io/services" title="Professional services">服务</a>
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/ca/1x1.png" alt="spacer" width="250px" height="1px">
  <a href="../README.md">EN</a> •
  <b>中文</b> •
  <a href="./README_ja.md">日本語</a> •
  <a href="./README_ko.md">한국어</a> •
  <a href="./README_pt_BR.md">PT</a> •
  <a href="./README_he.md">עברית</a>
</p>

<p align="center">
  <a href="https://lvgl.io"><img src="https://lvgl.io/github-assets/logo-colored.png" height=50px/></a>
  <h1 align="center">轻量级多功能图形库</h1>
</p>

<br/>
<br/>

<div align="center">
  <img src="https://github.com/user-attachments/assets/965e8b8b-d240-45ed-9744-bdd81785967d" height="220" alt="采用矢量图形的电动自行车演示">
  &nbsp;
  <img src="https://github.com/user-attachments/assets/d83820ed-5448-494e-94c8-3ca1b4ddceb0" height="220" alt="带动画 3D 模型的心电图演示">
</div>

<br/>
<br/>
<br/>

<p align="center">
  <a href="#overview" title="What LVGL is">概述</a> •
  <a href="#features" title="What LVGL can do">功能特性</a> •
  <a href="#lvgl-pro" title="The professional toolchain">LVGL Pro</a> •
  <a href="#examples" title="C and XML examples">示例</a> •
  <a href="#integration" title="How to add LVGL to your project">集成</a> •
  <a href="#contributing" title="How to get involved">参与贡献</a> •
  <a href="#license" title="Licensing terms">许可证</a>
</p>

<a id="overview"></a>
## 概述

**LVGL** 是一个免费的开源 UI 库，让你能够为任何厂商、任何平台上的任何 MCU 和 MPU 创建图形用户界面。

**需求条件**：LVGL 没有任何外部依赖，因此可以轻松编译到任何现代目标平台，从小型 MCU 到支持 3D 的多核 Linux MPU 皆可。对于*典型*的 UI，你只需要约 100kB RAM、约 200–300kB flash，以及相当于屏幕 1/10 大小的缓冲区用于渲染。

**广泛采用**：芯片厂商（如 NXP、Espressif、Renesas 等）、RTOS 项目（Zephyr、NuttX 等）以及开发板制造商（Riverdi、Seeed Studio、VIEWE、Elecrow 等）都已经集成了 LVGL。如果一块开发板带有显示屏，那么厂商很可能也提供了 LVGL 支持。

**专业工具**：与其编写 C 代码，你还可以使用 [LVGL Pro](#lvgl-pro) 来大幅加速并简化 UI 开发。它是一套完整的工具包，包含 Editor、Figma 集成、Online Viewer 和 CLI。它能从 XML 导出纯粹的 LVGL C 代码，没有额外的运行时，也没有隐藏的黑魔法。它对非商业用途和评估用途免费。

**开始上手**，可以浏览[示例](#examples)、启动一个[模拟器项目](https://lvgl.io/docs/open/integration/pc)、探索 LVGL Pro 的 [Online Viewer](https://viewer.lvgl.io/)、在 [LVGL Pro](https://lvgl.io/docs/pro/integration/simulator) 中一键编译并在窗口中运行 UI，或者深入阅读我们面向 AI 就绪的[文档](https://lvgl.io/docs/open)。只需点击 [**Ask AI**](https://lvgl.io/docs/open)，即可尽情提问！

<a id="features"></a>
## 功能特性

**免费、可移植、可扩展**
  - 完全可移植的 C（兼容 C++）库，没有任何外部依赖。
  - 可为任何 MCU 或 MPU、任何 (RT)OS 编译。支持 Make、CMake 以及简单的通配（globbing）。
  - 支持单色屏、ePaper、OLED、TFT 显示屏，甚至显示器。[显示屏](https://lvgl.io/docs/open/main-modules/display)
  - 基于 MIT 许可证发布，因此你也可以轻松将其用于商业项目。
  - 最低仅需 32kB RAM 和 128kB Flash、一个帧缓冲区，以及至少屏幕 1/10 大小的缓冲区用于渲染。
  - 支持 OS、外部内存和 GPU，但并非必需。

**控件、样式、布局及更多**
  - 30 多种内置[控件](https://lvgl.io/docs/open/widgets)：Button、Label、Slider、Chart、Keyboard、Meter、Arc、Table 等等。
  - 灵活的[样式系统](https://lvgl.io/docs/open/common-widget-features/styles)，拥有 100 多个样式属性，可在任何状态下定制控件的任何部分。
  - 类似 [Flexbox](https://lvgl.io/docs/open/common-widget-features/layouts/flex) 和 [Grid](https://lvgl.io/docs/open/common-widget-features/layouts/grid) 的布局引擎，可自动地响应式调整控件的尺寸和位置。
  - [数据绑定](https://lvgl.io/docs/open/main-modules/observer)，可轻松将 UI 与应用程序连接起来。
  - 支持鼠标、触摸板、小键盘、键盘、外部按钮、编码器等[输入设备](https://lvgl.io/docs/open/main-modules/indev)。
  - 支持[多显示屏](https://lvgl.io/docs/open/main-modules/display/overview#how-many-displays-can-lvgl-use)。

**渲染**
  - 内置 2D 渲染引擎，支持基本形状、渐变、抗锯齿、不透明度、平滑滚动、盒子阴影和投影阴影、图像变换等。
  - [强大的 3D 渲染引擎](https://lvgl.io/docs/open/libs/gltf)，可通过 OpenGL 展示 [glTF 模型](https://sketchfab.com/)。
  - 支持矢量图形、SVG 和 Lottie。
  - 文本采用 UTF-8 编码渲染，支持 CJK、泰文、印地文、阿拉伯文和波斯文书写系统。
  - 内置对 VG-Lite、Dave2D、NeoChrome、OpenGL 等 GPU 的支持。

<a id="lvgl-pro"></a>
## LVGL Pro

用 C 构建 UI 效果不错，但随着项目变大，迭代会变慢，也更难保持一致性。[LVGL Pro](https://lvgl.io/pro) 让你能够可视化地构建可复用的组件和屏幕、即时预览变更，并在一处统一管理数据绑定、翻译、动画和测试。

Pro 导出的是纯粹的 LVGL C 代码：就是你已经在用的那个 LVGL，没有额外的运行时或依赖。它能直接放入现有项目，无需改变你构建或发布的方式。

<p align="center">
  <img height="500" alt="在 LVGL Pro 编辑器中构建 UI 并实时预览" src="https://github.com/user-attachments/assets/8cef0f05-0ff1-4766-8dfd-1d15e47f181a" />
</p>

你可以在浏览器中通过 [viewer.lvgl.io](https://viewer.lvgl.io) 直接试用，无需安装任何东西；也可以[下载 Editor](https://lvgl.io/pro#download) 并在免费的 Community 许可证下使用。

LVGL Pro 由四个紧密关联的工具组成：

1. **Editor**：LVGL Pro 的核心。一款桌面应用，用于以 XML 构建组件和屏幕，管理数据绑定、翻译、动画、测试等。进一步了解 [XML 格式](https://lvgl.io/docs/pro/syntax) 和[控件](https://lvgl.io/docs/pro/built_in_widgets)。
2. **Online Viewer**：在浏览器中运行 Editor，打开 GitHub 项目，无需搭建开发环境即可轻松分享。访问 [https://viewer.lvgl.io](https://viewer.lvgl.io)。
3. **Figma 插件**：一键将 Figma 设计迁移到 LVGL Pro。在[这里](https://lvgl.io/docs/pro/figma)查看其工作原理。
4. **CLI 工具**：在 CI/CD 中生成 C 代码并运行测试。详情见[这里](https://lvgl.io/docs/pro/cli)。

Community 和 Evaluation 层级可免费使用；商业用途请参见[许可证](#license)。

<a id="examples"></a>
## 示例

你可以在 https://lvgl.io/docs/open/examples 查看 100 多个 C 和 XML 示例

LVGL Pro 的 [Online Viewer](https://viewer.lvgl.io/) 还包含许多教程以及这些相同的示例，你可以进行交互式体验。

作为预告，下面是用 C 和 XML 编写的同一个简单 UI：

<p align="center">
  <img width="311" height="232" alt="一个居中的按钮，点击时会打印" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />
</p>

<!-- GitHub renders fenced code blocks inside table cells, so this shows C and XML side by side. -->
<table>
<tr>
<td width="50%">

```c
static void button_clicked_cb(lv_event_t * e)
{
    printf("Clicked\n");
}

/* ... */

lv_obj_t * button = lv_button_create(lv_screen_active());
lv_obj_center(button);
lv_obj_add_event_cb(button, button_clicked_cb,
                    LV_EVENT_CLICKED, NULL);

lv_obj_t * label = lv_label_create(button);
lv_label_set_text(label, "Hello from LVGL!");
```

</td>
<td width="50%">

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

</td>
</tr>
</table>

上面的 XML 正是 [LVGL Pro](#lvgl-pro) 所使用的：你可视化地构建屏幕，它就会为你生成 C 代码。

<a id="integration"></a>
## 集成

LVGL 没有任何外部依赖，因此可以轻松地为任何设备编译。它自带内置驱动，可在众多包管理器和 RTOS 中获取，也很容易移植到任何新设备上。

### 预集成

- **芯片厂商**：[ESP32](https://components.espressif.com/components/lvgl/lvgl)、[NXP MCUXpresso 组件](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)、[Renesas FSP](https://lvgl.io/docs/open/integration/chip/renesas)、[STM32](https://lvgl.io/docs/open/integration/chip/stm32)

- **RTOS**：[Zephyr](https://lvgl.io/docs/open/integration/os/zephyr)、[NuttX](https://lvgl.io/docs/open/integration/os/nuttx)、[RT-Thread](https://lvgl.io/docs/open/integration/os/rt-thread)

- **框架**：[Arduino](https://lvgl.io/docs/open/integration/framework/arduino)、[PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl)、[CMSIS-Pack](https://lvgl.io/docs/open/integration/framework/cmsis-pack)

- **开发板制造商**：[Seeed Studio](https://www.seeedstudio.com)、[Elecrow](https://www.elecrow.com/display/esp-hmi-display.html)、[Riverdi](https://lvgl.io/docs/open/integration/boards/manufacturers/riverdi)、[VIEWE](https://lvgl.io/docs/open/integration/boards/manufacturers/viewe)，以及[更多](https://lvgl.io/boards)

### 内置驱动
LVGL 附带了开箱即用的驱动，因此在常见平台上，你无需自己编写显示和输入处理代码：

- **模拟器 / 桌面**：[SDL](https://lvgl.io/docs/open/integration/pc/sdl)、X11 和 [Wayland](https://lvgl.io/docs/open/integration/embedded_linux/drivers/wayland) 窗口，可在 PC 上开发和预览你的 UI。
- **显示控制器**：通用的 MIPI-DBI/SPI LCD（[ILI9341](https://lvgl.io/docs/open/integration/external_display_controllers/ili9341)、ST7789 及类似型号），以及厂商控制器，如 ST 的 [LTDC](https://lvgl.io/docs/open/integration/chip_vendors/stm32/ltdc) 和 NXP 的 [eLCDIF](https://lvgl.io/docs/open/integration/chip_vendors/nxp/elcdif)。
- **嵌入式 Linux**：framebuffer (fbdev)、DRM/KMS、Wayland，以及用于触摸和指针输入的 `libinput`/`evdev`。在[这里](https://lvgl.io/docs/open/integration/embedded_linux)进一步了解。
- **GPU / 加速器**：VG-Lite、NXP PXP、ThinkSilicon NemaGFX、Arm-2D 和 [OpenGL ES](https://lvgl.io/docs/open/integration/embedded_linux/drivers/opengl_driver)

在[集成文档](https://lvgl.io/docs/open/integration)中查看完整列表和设置指南。

### 在 LVGL Pro 中

在 LVGL Pro 中，你只需单击一下即可创建开箱即用的纯 UI、VSCode、Zephyr 和 Linux 项目。
<img width="600" alt="image" src="https://github.com/user-attachments/assets/5aadb850-6b40-49d1-ba96-2296041c7e27" />

### 手动移植

集成 LVGL 非常简单。只需将它放入任何项目，像编译其他文件一样编译它即可。要配置 LVGL，将 `lv_conf_template.h` 复制为 `lv_conf.h`，启用开头的 `#if 0`，并根据需要调整配置。（默认配置通常就够用了。）如果可用，LVGL 也可以配合 Kconfig 使用。

进入项目后，你可以按如下方式初始化 LVGL 并创建显示设备和输入设备：

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

    /* signal LVGL that we're done */
    lv_display_flush_ready(disp);
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


<a id="contributing"></a>
## 参与贡献

LVGL 是一个开放的项目，非常欢迎大家贡献。贡献的方式有很多，从简单地谈论你的项目、编写示例、改进文档、修复 bug，甚至到在 LVGL 组织下托管你自己的项目。

有关贡献机会的详细说明，请访问文档的[参与贡献](https://lvgl.io/docs/open/contributing)章节。

已经有数百人在 LVGL 上留下了他们的印记。快来成为其中一员吧！我们在这里等你！🙂


<a id="license"></a>
## 许可证

LVGL 库基于 **MIT 许可证**发布，因此你可以在开源和商业产品中自由使用它，无需支付任何版税。参见 [`LICENCE.txt`](../LICENCE.txt)。

与 LVGL 捆绑的所有第三方库也都在与 MIT 兼容的许可证下发布，因此你可以放心地使用 LVGL 及其依赖。

**[LVGL Pro](https://lvgl.io/pro)** 采用单独的许可授权：
- **Community** 和 **Evaluation** 层级对**非商业用途免费**，非常适合学习、业余项目以及评估这些工具。
- LVGL Pro 的商业用途需要付费许可证。参见[价格与详情](https://lvgl.io/pro#pricing)。
