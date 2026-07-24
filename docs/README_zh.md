<a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets/sponsor.png" height="32px"></a>

<p align="right">
  <a href="../README.md">English</a>  |  <b>中文</b> | <a href="./README_pt_BR.md">Português do Brasil</a> | <a href="./README_jp.md">日本語</a> | <a href="./README_he.md">עברית</a>
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
  <a href="https://lvgl.io" title="LVGL 官方网站">网站</a> |
  <a href="https://pro.lvgl.io" title="LVGL Pro 基于 XML 的 UI 编辑器">LVGL Pro 编辑器</a> |
  <a href="https://docs.lvgl.io/" title="包含 100+ 示例的详细文档">文档</a> |
  <a href="https://forum.lvgl.io" title="寻求帮助或帮助他人">论坛</a> |
  <a href="https://lvgl.io/demos" title="在浏览器中运行的演示">演示</a> |
  <a href="https://lvgl.io/services" title="图形设计、UI 实现和咨询服务">服务</a>
</p>

<br/>

## 📒 概述

**LVGL** 是一个免费、开源的 UI 库，可用于为来自任何厂商、任何平台的 MCU 或 MPU 创建图形用户界面。

**要求**：LVGL 无需外部依赖，因此可以轻松编译到任何现代平台上，从小型 MCU 到支持 3D 的多核 Linux MPU。简单 UI 仅需约 100kB RAM、200–300kB Flash，以及 1/10 屏幕大小的渲染缓冲区。

**开始使用**：选择一个可直接使用的 VSCode、Eclipse 或其他项目，在 PC 上尝试 LVGL。LVGL 的 UI 代码完全平台无关，因此也可直接用于嵌入式设备。

**LVGL Pro** 是一套完整的工具包，可让你更快地构建、测试、共享和发布 UI。它附带一个 XML 编辑器，你可以在其中快速创建和测试可重用组件，导出 C 代码，或在运行时加载 XML。了解更多信息请点击这里。

## 💡 特性

**免费与可移植性**
  - 纯 C（兼容 C++）库，无外部依赖。
  - 可为任意 MCU 或 MPU 编译，支持任何 (RT)OS。支持 Make、CMake 和简单文件匹配。
  - 支持单色、电子墨水、OLED、TFT 显示器，甚至是 PC 显示器。详见 [显示模块](https://docs.lvgl.io/master/main-modules/display/index.html)
  - 采用 MIT 许可证，可自由用于商业项目。
  - 仅需 32kB RAM、128kB Flash、一个帧缓冲区，以及至少 1/10 屏幕大小的渲染缓冲。
  - 支持但不强制要求使用操作系统、外部存储或 GPU。

**控件、样式、布局等**
  - 超过 30 个内置 [控件](https://docs.lvgl.io/master/widgets/index.html)：按钮、标签、滑块、图表、键盘、仪表、弧形、表格等。
  - 灵活的 [样式系统](https://docs.lvgl.io/master/common-widget-features/styles/index.html)，提供约 100 种属性，可自定义控件任意部分与状态。
  - [Flexbox](https://docs.lvgl.io/master/common-widget-features/layouts/flex.html) 和 [Grid](https://docs.lvgl.io/master/common-widget-features/layouts/grid.html) 布局引擎可实现自适应布局。
  - 文本支持 UTF-8 编码，兼容中日韩、泰语、印地语、阿拉伯语、波斯语等。
  - [数据绑定](https://docs.lvgl.io/master/main-modules/data_binding/index.html) 模块轻松连接 UI 与应用逻辑。
  - 渲染引擎支持动画、抗锯齿、不透明度、平滑滚动、阴影、图像变换等。
  - [强大的 3D 渲染引擎](https://docs.lvgl.io/master/libs/gltf.html)，可使用 OpenGL 显示 [glTF 模型](https://sketchfab.com/)。
  - 支持鼠标、触摸板、按键、键盘、外部按钮、编码器等 [输入设备](https://docs.lvgl.io/master/main-modules/indev.html)。
  - 支持 [多显示屏](https://docs.lvgl.io/master/main-modules/display/overview.html#how-many-displays-can-lvgl-use)。

## 📦️ 平台支持

LVGL 无外部依赖，因此可轻松编译到任何设备，并已被多个包管理器与 RTOS 集成：

- [Arduino 库](https://docs.lvgl.io/master/integration/framework/arduino.html)
- [PlatformIO 包](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr 库](https://docs.lvgl.io/master/integration/os/zephyr.html)
- [ESP-IDF (ESP32) 组件](https://components.espressif.com/components/lvgl/lvgl)
- [NXP MCUXpresso 组件](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX 库](https://docs.lvgl.io/master/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/integration/os/rt-thread.html)
- CMSIS-Pack
- [RIOT OS 包](https://doc.riot-os.org/group__pkg__lvgl.html#details)

## 🚀 LVGL Pro 编辑器

LVGL Pro 是一套完整的工具，用于高效构建、测试、共享和交付嵌入式 UI。

它包含四个紧密集成的工具：

1. **XML 编辑器**：LVGL Pro 的核心。桌面应用，用于通过 XML 构建组件与界面，管理数据绑定、翻译、动画、测试等。了解更多 [XML 格式](https://docs.lvgl.io/master/xml/xml/index.html) 和 [编辑器](https://docs.lvgl.io/master/xml/editor/index.html)。
2. **在线预览器**：在浏览器中运行编辑器，打开 GitHub 项目，无需搭建开发环境即可共享。访问 [https://viewer.lvgl.io](https://viewer.lvgl.io)。
3. **CLI 工具**：在 CI/CD 中生成 C 代码并运行测试。详见 [此处](https://docs.lvgl.io/master/xml/tools/cli.html)。
4. **Figma 插件**：直接从 Figma 同步和提取样式。了解更多 [这里](https://docs.lvgl.io/master/xml/tools/figma.html)。

这些工具结合在一起，帮助开发者高效构建 UI、可靠测试，并便于团队协作。

了解更多：https://pro.lvgl.io

## 🤝 商业服务

LVGL LLC 提供多种商业服务，助力 UI 开发。凭借超过 15 年的用户界面与图形经验，我们能将你的产品 UI 提升到更高水平。

- **图形设计**：我们的内部设计师擅长打造美观现代的设计，兼顾产品特性与硬件能力。
- **UI 实现**：我们可以基于你提供或我们设计的 UI 进行实现，充分发挥硬件与 LVGL 的潜力。如缺少特定功能或控件，我们也可为你开发。
- **咨询与支持**：提供 UI 开发咨询，帮助你避免代价高昂、耗时的错误。
- **开发板认证**：为提供开发板或量产套件的企业提供认证服务，展示该板卡的 LVGL 运行能力。

查看我们的 [演示](https://lvgl.io/demos) 以了解示例。更多信息请访问 [服务页面](https://lvgl.io/services)。

[联系我们](https://lvgl.io/#contact)，告诉我们如何帮助你。

## 🧑‍💻 集成 LVGL

集成 LVGL 非常简单。将其添加到项目中并像其他源文件一样编译即可。
配置时复制 `lv_conf_template.h` 为 `lv_conf.h`，启用第一个 `#if 0` 并按需修改配置。
（默认配置通常已足够。）LVGL 也支持通过 Kconfig 配置。

项目中可按以下方式初始化 LVGL 并创建显示和输入设备：

```c
#include "lvgl/lvgl.h" /*定义 LV_LVGL_H_INCLUDE_SIMPLE 可直接使用 "lvgl.h"*/

#define TFT_HOR_RES 320
#define TFT_VER_RES 240

static uint32_t my_tick_cb(void)
{
    return my_get_millisec();
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /*将 px_map 写入帧缓冲或显示控制器对应区域*/
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

    /*初始化 LVGL*/
    lv_init();

    /*设置毫秒级时钟回调，用于 LVGL 计时*/
    lv_tick_set_cb(my_tick_cb);

    /*创建显示对象，用于添加屏幕和控件*/
    lv_display_t * display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);

    /*添加渲染缓冲，这里假设使用 16 位 RGB565 格式*/
    static uint8_t buf[TFT_HOR_RES * TFT_VER_RES / 10 * 2];
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*设置刷新回调，将缓冲内容写入显示设备*/
    lv_display_set_flush_cb(display, my_flush_cb);

    /*创建输入设备用于触摸处理*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read_cb);

    /*驱动已就绪，创建 UI*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_center(label);

    /*循环执行 LVGL 任务*/
    while(1) {
        lv_timer_handler();
        my_sleep_ms(5);
    }
}
```

## 🤖 示例

可在 https://docs.lvgl.io/master/examples.html 查看 100+ 示例。

在线预览器 https://viewer.lvgl.io/ 也提供了易学的 XML 教程。


### Hello World 按钮事件示例

<img width="311" height="232" alt="image" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />

<details>
  <summary>C 代码</summary>

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
  <summary>使用 LVGL Pro 的 XML</summary>

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

### 带数据绑定的滑块

<img width="314" height="233" alt="image" src="https://github.com/user-attachments/assets/268db1a0-946c-42e2-aee4-9550bdf5f4f9" />

<details>
  <summary>C 代码</summary>

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
  <summary>使用 LVGL Pro 的 XML</summary>

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

### 布局中的复选框

<img width="311" height="231" alt="image" src="https://github.com/user-attachments/assets/ba9af647-2ea1-4bc8-b53d-c7b43ce24b6e" />

<details>
  <summary>C 代码</summary>

  ```c
/*创建新屏幕并加载*/
lv_obj_t * scr = lv_obj_create(NULL);
lv_screen_load(scr);

/*设置列布局*/
lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, /*垂直对齐*/
						   LV_FLEX_ALIGN_START,	       /*主轴方向对齐*/
						   LV_FLEX_ALIGN_CENTER);      /*交叉轴方向对齐*/

/*创建 5 个复选框*/
const char * texts[5] = {"Input 1", "Input 2", "Input 3", "Output 1", "Output 2"};
for(int i = 0; i < 5; i++) {
	lv_obj_t * cb = lv_checkbox_create(scr);
	lv_checkbox_set_text(cb, texts[i]);
}

/*更改状态*/
lv_obj_add_state(lv_obj_get_child(scr, 1), LV_STATE_CHECKED);
lv_obj_add_state(lv_obj_get_child(scr, 3), LV_STATE_DISABLED);
```

</details>

<details>
  <summary>使用 LVGL Pro 的 XML</summary>

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


## 🌟 贡献

LVGL 是一个开放项目，欢迎任何形式的贡献。无论是分享你的项目、编写示例、改进文档、修复错误，还是将项目托管到 LVGL 组织下，都非常欢迎。

更多贡献方式详见 [贡献文档](https://docs.lvgl.io/master/contributing/index.html)。

已有超过 600 位开发者为 LVGL 留下足迹。加入我们吧！🙂

<a href="https://github.com/lvgl/lvgl/graphs/contributors"> <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" /> </a>

……还有更多人。

