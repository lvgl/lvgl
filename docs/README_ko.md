<p align="center">
  <a href="https://lvgl.io/docs" title="문서">문서</a> •
  <a href="https://forum.lvgl.io" title="커뮤니티 포럼">포럼</a> •
  <a href="https://blog.lvgl.io" title="뉴스 및 기사">블로그</a> •
  <a href="https://lvgl.io/services" title="전문 서비스">서비스</a>
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/ca/1x1.png" alt="spacer" width="250px" height="1px">
  <a href="../README.md">EN</a> •
  <a href="./README_zh.md">中文</a> •
  <a href="./README_ja.md">日本語</a> •
  <b>한국어</b> •
  <a href="./README_pt_BR.md">PT</a> •
  <a href="./README_he.md">עברית</a>
</p>

<p align="center">
  <a href="https://lvgl.io"><img src="https://lvgl.io/github-assets/logo-colored.png" height=50px/></a>
  <h1 align="center">가볍고 다재다능한 그래픽 라이브러리</h1>
</p>

<br/>
<br/>

<div align="center">
  <img src="https://github.com/user-attachments/assets/965e8b8b-d240-45ed-9744-bdd81785967d" height="220" alt="벡터 그래픽을 사용한 전기 자전거 데모">
  &nbsp;
  <img src="https://github.com/user-attachments/assets/d83820ed-5448-494e-94c8-3ca1b4ddceb0" height="220" alt="애니메이션 3D 모델을 사용한 ECG 데모">
</div>

<br/>
<br/>
<br/>

<p align="center">
  <a href="#overview" title="LVGL이란">개요</a> •
  <a href="#features" title="LVGL로 할 수 있는 것">기능</a> •
  <a href="#lvgl-pro" title="전문 툴체인">LVGL Pro</a> •
  <a href="#examples" title="C 및 XML 예제">예제</a> •
  <a href="#integration" title="LVGL을 프로젝트에 추가하는 방법">통합</a> •
  <a href="#contributing" title="참여하는 방법">기여</a> •
  <a href="#license" title="라이선스 조건">라이선스</a>
</p>

<a id="overview"></a>
## 개요

**LVGL**은 어떤 벤더의 어떤 MCU 및 MPU에서든, 어떤 플랫폼에서든 그래픽 사용자 인터페이스를
만들 수 있게 해주는 무료 오픈소스 UI 라이브러리입니다.

**요구 사항**: LVGL은 외부 의존성이 없어서 소형 MCU부터 3D를 지원하는 멀티코어 리눅스 기반 MPU까지
어떤 최신 타깃이든 손쉽게 컴파일할 수 있습니다. *일반적인* UI의 경우 약 100kB RAM,
약 200~300kB 플래시, 그리고 렌더링을 위한 화면의 1/10 크기 버퍼만 있으면 됩니다.

**광범위한 채택**: 칩 벤더(NXP, Espressif, Renesas 등), RTOS 프로젝트(Zephyr, NuttX 등),
그리고 보드 제조사(Riverdi, Seeed Studio, VIEWE, Elecrow 등)가 모두 이미 LVGL을 통합했습니다. 개발 보드에 디스플레이가 있다면
그 벤더가 LVGL 지원도 제공할 가능성이 매우 높습니다.

**전문 도구**: C 코드를 작성하는 대신, Editor, Figma 통합, Online Viewer, CLI를 갖춘 완전한 툴킷인 [LVGL Pro](#lvgl-pro)를 사용하면 UI 개발을 크게 가속하고 단순화할 수 있습니다. 추가 런타임이나 숨겨진 마법 없이 XML에서 순수 LVGL C 코드를 내보냅니다. 비상업적 용도와 평가용으로는 무료입니다.

**시작하려면** [예제](#examples)를 둘러보고, [시뮬레이터 프로젝트](https://lvgl.io/docs/open/integration/pc)를 실행해 보고, LVGL Pro의 [Online Viewer](https://viewer.lvgl.io/)를 탐색하고, [LVGL Pro](https://lvgl.io/docs/pro/integration/simulator)에서 클릭 한 번으로 UI를 창에서 컴파일하고 실행해 보거나, AI 지원 [문서](https://lvgl.io/docs/open)를 살펴보세요. [**Ask AI**](https://lvgl.io/docs/open)를 클릭해서
무엇이든 물어보세요!

<a id="features"></a>
## 기능

**무료, 이식 가능, 확장 가능**
  - 외부 의존성이 없는 완전히 이식 가능한 C(C++ 호환) 라이브러리입니다.
  - 어떤 (RT)OS든 사용하여 어떤 MCU 또는 MPU에서든 컴파일할 수 있습니다. Make, CMake, 그리고 간단한 globbing이 모두 지원됩니다.
  - 흑백, ePaper, OLED, TFT 디스플레이, 심지어 모니터까지 지원합니다. [디스플레이](https://lvgl.io/docs/open/main-modules/display)
  - MIT 라이선스로 배포되므로 상업 프로젝트에서도 손쉽게 사용할 수 있습니다.
  - 최소한으로는 32kB RAM과 128kB 플래시, 프레임 버퍼, 그리고 렌더링을 위한 최소 1/10 화면 크기 버퍼만 필요합니다.
  - OS, 외부 메모리, GPU는 지원되지만 필수는 아닙니다.

**위젯, 스타일, 레이아웃 등**
  - 30개 이상의 내장 [위젯](https://lvgl.io/docs/open/widgets): Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table 등 다수.
  - 어떤 상태에서든 위젯의 모든 부분을 커스터마이징할 수 있는 100개 이상의 스타일 속성을 갖춘 유연한 [스타일 시스템](https://lvgl.io/docs/open/common-widget-features/styles).
  - 위젯의 크기와 위치를 반응형으로 자동 조정하는 [Flexbox](https://lvgl.io/docs/open/common-widget-features/layouts/flex) 및 [Grid](https://lvgl.io/docs/open/common-widget-features/layouts/grid) 유사 레이아웃 엔진.
  - UI를 애플리케이션과 손쉽게 연결하는 [데이터 바인딩](https://lvgl.io/docs/open/main-modules/observer).
  - Mouse, Touchpad, Keypad, Keyboard, 외부 버튼, Encoder [입력 장치](https://lvgl.io/docs/open/main-modules/indev)를 지원합니다.
  - [다중 디스플레이](https://lvgl.io/docs/open/main-modules/display/overview#how-many-displays-can-lvgl-use) 지원.

**렌더링**
  - 기본 도형, 그라디언트, 안티앨리어싱, 불투명도, 부드러운 스크롤, 박스 및 드롭 섀도, 이미지 변환 등을 지원하는 내장 2D 렌더링 엔진.
  - OpenGL로 [glTF 모델](https://sketchfab.com/)을 표시하는 [강력한 3D 렌더링 엔진](https://lvgl.io/docs/open/libs/gltf).
  - 벡터 그래픽, SVG, Lottie 지원.
  - 텍스트는 UTF-8 인코딩으로 렌더링되며, CJK, 태국어, 힌디어, 아랍어, 페르시아어 문자 체계를 지원합니다.
  - VG-Lite, Dave2D, NeoChrome, OpenGL 등 GPU에 대한 내장 지원.

<a id="lvgl-pro"></a>
## LVGL Pro

C로 UI를 만드는 것도 잘 작동하지만, 프로젝트가 커질수록 반복 작업이 느려지고 일관성을 유지하기가 어려워집니다.
[LVGL Pro](https://lvgl.io/pro)를 사용하면 재사용 가능한 컴포넌트와 화면을 시각적으로 만들고, 변경 사항을
즉시 미리 보고, 데이터 바인딩, 번역, 애니메이션, 테스트를 한곳에서 관리할 수 있습니다.

Pro는 평범한 LVGL C 코드를 내보냅니다. 이미 사용 중인 바로 그 LVGL이며, 추가 런타임이나 의존성이 없습니다. 빌드하거나
배포하는 방식을 바꾸지 않고도 기존 프로젝트에 그대로 들어갑니다.

<p align="center">
  <img height="500" alt="실시간 미리 보기와 함께 LVGL Pro 에디터에서 UI 구축하기" src="https://github.com/user-attachments/assets/8cef0f05-0ff1-4766-8dfd-1d15e47f181a" />
</p>

아무것도 설치하지 않고 브라우저에서 [viewer.lvgl.io](https://viewer.lvgl.io)로 사용해 보거나,
[Editor를 다운로드](https://lvgl.io/pro#download)하여 무료 Community 라이선스로 사용할 수 있습니다.

LVGL Pro는 긴밀하게 연관된 네 가지 도구로 구성됩니다:

1. **Editor**: LVGL Pro의 핵심. XML로 컴포넌트와 화면을 만들고 데이터 바인딩, 번역, 애니메이션, 테스트 등을 관리하는 데스크톱 앱입니다. [XML 형식](https://lvgl.io/docs/pro/syntax)과 [위젯](https://lvgl.io/docs/pro/built_in_widgets)에 대해 자세히 알아보세요.
2. **Online Viewer**: 브라우저에서 Editor를 실행하고, GitHub 프로젝트를 열고, 개발 환경을 설정하지 않고도 손쉽게 공유하세요. [https://viewer.lvgl.io](https://viewer.lvgl.io)를 방문하세요.
3. **Figma 플러그인**: 클릭 한 번으로 Figma 디자인을 LVGL Pro로 옮기세요. 작동 방식은 [여기](https://lvgl.io/docs/pro/figma)에서 확인하세요.
4. **CLI 도구**: C 코드를 생성하고 CI/CD에서 테스트를 실행하세요. 자세한 내용은 [여기](https://lvgl.io/docs/pro/cli)를 참조하세요.

Community 및 Evaluation 등급은 무료로 사용할 수 있으며, 상업적 사용에 대해서는 [라이선스](#license)를 참조하세요.

<a id="examples"></a>
## 예제

https://lvgl.io/docs/open/examples 에서 100개가 넘는 C 및 XML 예제를 확인할 수 있습니다.

LVGL Pro의 [Online Viewer](https://viewer.lvgl.io/)에도 많은 튜토리얼과 함께 인터랙티브하게 다뤄볼 수 있는 동일한 예제들이 포함되어 있습니다.

맛보기로, 동일한 간단한 UI를 C와 XML로 작성한 예를 보여드립니다:

<p align="center">
  <img width="311" height="232" alt="클릭하면 출력하는 가운데 정렬된 버튼" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />
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

위의 XML은 [LVGL Pro](#lvgl-pro)가 다루는 것입니다: 화면을 시각적으로 만들면 Pro가 C 코드를 생성해 줍니다.

<a id="integration"></a>
## 통합

LVGL은 외부 의존성이 없어서 어떤 장치에서든 손쉽게 컴파일할 수 있습니다. 내장 드라이버가 함께 제공되고,
많은 패키지 관리자와 RTOS에서 이용할 수 있으며, 새로운 장치에 포팅하기도 쉽습니다.

### 사전 통합됨

- **칩 벤더**: [ESP32](https://components.espressif.com/components/lvgl/lvgl), [NXP MCUXpresso 컴포넌트](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY), [Renesas FSP](https://lvgl.io/docs/open/integration/chip/renesas), [STM32](https://lvgl.io/docs/open/integration/chip/stm32)

- **RTOS**: [Zephyr](https://lvgl.io/docs/open/integration/os/zephyr), [NuttX](https://lvgl.io/docs/open/integration/os/nuttx), [RT-Thread](https://lvgl.io/docs/open/integration/os/rt-thread)

- **프레임워크**: [Arduino](https://lvgl.io/docs/open/integration/framework/arduino), [PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl), [CMSIS-Pack](https://lvgl.io/docs/open/integration/framework/cmsis-pack)

- **보드 제조사**: [Seeed Studio](https://www.seeedstudio.com), [Elecrow](https://www.elecrow.com/display/esp-hmi-display.html), [Riverdi](https://lvgl.io/docs/open/integration/boards/manufacturers/riverdi), [VIEWE](https://lvgl.io/docs/open/integration/boards/manufacturers/viewe), 그리고 [그 외 다수](https://lvgl.io/boards)

### 내장 드라이버
LVGL은 바로 사용할 수 있는 드라이버와 함께 제공되므로, 일반적인 플랫폼에서는 디스플레이와 입력 처리를 직접 작성할 필요가 없습니다:

- **시뮬레이터 / 데스크톱**: PC에서 UI를 개발하고 미리 볼 수 있는 [SDL](https://lvgl.io/docs/open/integration/pc/sdl), X11, [Wayland](https://lvgl.io/docs/open/integration/embedded_linux/drivers/wayland) 창.
- **디스플레이 컨트롤러**: 범용 MIPI-DBI/SPI LCD([ILI9341](https://lvgl.io/docs/open/integration/external_display_controllers/ili9341), ST7789 및 유사 제품)와 ST의 [LTDC](https://lvgl.io/docs/open/integration/chip_vendors/stm32/ltdc), NXP의 [eLCDIF](https://lvgl.io/docs/open/integration/chip_vendors/nxp/elcdif) 같은 벤더 컨트롤러.
- **임베디드 리눅스**: 프레임버퍼(fbdev), DRM/KMS, Wayland, 그리고 터치 및 포인터 입력을 위한 `libinput`/`evdev`. 자세한 내용은 [여기](https://lvgl.io/docs/open/integration/embedded_linux)에서 확인하세요.
- **GPU / 가속기**: VG-Lite, NXP PXP, ThinkSilicon NemaGFX, Arm-2D, 그리고 [OpenGL ES](https://lvgl.io/docs/open/integration/embedded_linux/drivers/opengl_driver)

전체 목록과 설정 가이드는 [통합 문서](https://lvgl.io/docs/open/integration)에서 확인하세요.

### LVGL Pro에서

LVGL Pro에서는 클릭 한 번으로 바로 사용할 수 있는 UI 전용, VSCode, Zephyr, Linux 프로젝트를 만들 수 있습니다.
<img width="600" alt="image" src="https://github.com/user-attachments/assets/5aadb850-6b40-49d1-ba96-2296041c7e27" />

### 수동으로 포팅하기

LVGL 통합은 매우 간단합니다. 어떤 프로젝트에든 넣고 다른 파일을 컴파일하듯이 컴파일하기만 하면 됩니다.
LVGL을 구성하려면 `lv_conf_template.h`를 `lv_conf.h`로 복사하고, 첫 번째 `#if 0`을 활성화한 뒤, 필요에 따라 구성을 조정하세요.
(기본 구성으로도 대개 충분합니다.) 가능한 경우 LVGL은 Kconfig와 함께 사용할 수도 있습니다.

프로젝트에 넣은 후에는 다음과 같이 LVGL을 초기화하고 디스플레이 및 입력 장치를 생성할 수 있습니다:

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
## 기여

LVGL은 열린 프로젝트이며, 기여를 매우 환영합니다. 단순히 여러분의 프로젝트에 대해 이야기하는 것부터, 예제를 작성하고, 문서를 개선하고, 버그를 수정하거나, 심지어 LVGL 조직 아래에 여러분의 프로젝트를 호스팅하는 것까지, 기여할 수 있는 방법은 많습니다.

기여 기회에 대한 자세한 설명은 문서의 [기여](https://lvgl.io/docs/open/contributing)
섹션을 방문하세요.

이미 수백 명의 사람들이 LVGL에 자취를 남겼습니다. 여러분도 그중 한 명이 되세요! 여기서 만나요! 🙂


<a id="license"></a>
## 라이선스

LVGL 라이브러리는 **MIT 라이선스**로 배포되므로, 로열티 없이 오픈소스 및 상업 제품 모두에서
자유롭게 사용할 수 있습니다. [`LICENCE.txt`](../LICENCE.txt)를 참조하세요.

LVGL과 함께 번들로 제공되는 모든 서드파티 라이브러리도 MIT 호환 라이선스로 배포되므로,
LVGL과 그 의존성을 안심하고 사용할 수 있습니다.

**[LVGL Pro](https://lvgl.io/pro)**는 별도의 라이선스를 적용합니다:
- **Community** 및 **Evaluation** 등급은 **비상업적 용도로 무료**이며, 학습, 취미 프로젝트,
  도구 평가에 완벽합니다.
- LVGL Pro의 상업적 사용에는 유료 라이선스가 필요합니다. [가격 및 상세 정보](https://lvgl.io/pro#pricing)를 참조하세요.
