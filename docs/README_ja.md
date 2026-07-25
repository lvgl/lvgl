<p align="center">
  <a href="https://lvgl.io/docs" title="Documentation">ドキュメント</a> •
  <a href="https://forum.lvgl.io" title="Community forum">フォーラム</a> •
  <a href="https://blog.lvgl.io" title="News and articles">ブログ</a> •
  <a href="https://lvgl.io/services" title="Professional services">サービス</a>
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/ca/1x1.png" alt="spacer" width="250px" height="1px">
  <a href="../README.md">EN</a> •
  <a href="./README_zh.md">中文</a> •
  <b>日本語</b> •
  <a href="./README_ko.md">한국어</a> •
  <a href="./README_pt_BR.md">PT</a> •
  <a href="./README_he.md">עברית</a>
</p>

<p align="center">
  <a href="https://lvgl.io"><img src="https://lvgl.io/github-assets/logo-colored.png" height=50px/></a>
  <h1 align="center">軽量で汎用性の高いグラフィックスライブラリ</h1>
</p>

<br/>
<br/>

<div align="center">
  <img src="https://github.com/user-attachments/assets/965e8b8b-d240-45ed-9744-bdd81785967d" height="220" alt="ベクターグラフィックスを使ったEbikeデモ">
  &nbsp;
  <img src="https://github.com/user-attachments/assets/d83820ed-5448-494e-94c8-3ca1b4ddceb0" height="220" alt="アニメーション3Dモデルを使ったECGデモ">
</div>

<br/>
<br/>
<br/>

<p align="center">
  <a href="#overview" title="What LVGL is">概要</a> •
  <a href="#features" title="What LVGL can do">機能</a> •
  <a href="#lvgl-pro" title="The professional toolchain">LVGL Pro</a> •
  <a href="#examples" title="C and XML examples">サンプル</a> •
  <a href="#integration" title="How to add LVGL to your project">統合</a> •
  <a href="#contributing" title="How to get involved">コントリビュート</a> •
  <a href="#license" title="Licensing terms">ライセンス</a>
</p>

<a id="overview"></a>
## 概要

**LVGL** は、無料でオープンソースのUIライブラリであり、あらゆるベンダーのあらゆるMCUやMPU向けに、あらゆるプラットフォーム上でグラフィカルユーザーインターフェースを作成できます。

**要件**: LVGLには外部依存関係がないため、小規模なMCUから3D対応のマルチコアLinuxベースMPUまで、あらゆる最新のターゲット向けに簡単にコンパイルできます。*一般的な* UIであれば、必要なのはRAMが約100kB、フラッシュが約200〜300kB、そしてレンダリング用に画面の1/10のバッファサイズだけです。

**幅広い採用**: チップベンダー（NXP、Espressif、Renesasなど）、RTOSプロジェクト（Zephyr、NuttXなど）、ボードメーカー（Riverdi、Seeed Studio、VIEWE、Elecrowなど）はすべて、すでにLVGLを統合しています。開発ボードにディスプレイが搭載されているなら、そのベンダーがLVGLサポートも提供している可能性が非常に高いです。

**プロフェッショナルツール**: C言語のコードを書く代わりに、[LVGL Pro](#lvgl-pro) を使うことでUI開発を大幅に高速化・簡素化できます。これはEditor、Figma統合、Online Viewer、CLIを備えた完全なツールキットです。追加のランタイムや隠れた仕掛けなしに、XMLから純粋なLVGLのCコードをエクスポートします。非商用利用および評価目的では無料です。

**始めるには**、[サンプル](#examples) を見たり、[Simulatorプロジェクト](https://lvgl.io/docs/open/integration/pc) を立ち上げたり、LVGL Proの [Online Viewer](https://viewer.lvgl.io/) を試したり、[LVGL Pro](https://lvgl.io/docs/pro/integration/simulator) でワンクリックでUIをウィンドウ内でコンパイル・実行したり、AI対応の [ドキュメント](https://lvgl.io/docs/open) に飛び込んだりしてみてください。[**Ask AI**](https://lvgl.io/docs/open) をクリックして、何でも質問してみましょう！

<a id="features"></a>
## 機能

**無料、ポータブル、スケーラブル**
  - 外部依存関係のない、完全にポータブルなC（C++互換）ライブラリ。
  - 任意の(RT)OSで、任意のMCUまたはMPU向けにコンパイル可能。Make、CMake、シンプルなグロビングのすべてに対応。
  - モノクロ、ePaper、OLED、TFTディスプレイ、さらにはモニターにも対応。[ディスプレイ](https://lvgl.io/docs/open/main-modules/display)
  - MITライセンスの下で配布されているため、商用プロジェクトでも簡単に利用できます。
  - 最小構成では、RAM 32kBとフラッシュ128kB、フレームバッファ、そしてレンダリング用に少なくとも画面の1/10サイズのバッファだけで動作します。
  - OS、外部メモリ、GPUに対応していますが、必須ではありません。

**ウィジェット、スタイル、レイアウト、その他**
  - 30種類以上の組み込み [ウィジェット](https://lvgl.io/docs/open/widgets): Button、Label、Slider、Chart、Keyboard、Meter、Arc、Tableなど多数。
  - 100以上のスタイルプロパティを備えた柔軟な [スタイルシステム](https://lvgl.io/docs/open/common-widget-features/styles) で、あらゆる状態のウィジェットのあらゆる部分をカスタマイズできます。
  - [Flexbox](https://lvgl.io/docs/open/common-widget-features/layouts/flex) と [Grid](https://lvgl.io/docs/open/common-widget-features/layouts/grid) 風のレイアウトエンジンで、ウィジェットのサイズと位置をレスポンシブに自動調整します。
  - UIとアプリケーションを簡単に接続する [データバインディング](https://lvgl.io/docs/open/main-modules/observer)。
  - Mouse、Touchpad、Keypad、Keyboard、外部ボタン、Encoderの [入力デバイス](https://lvgl.io/docs/open/main-modules/indev) に対応。
  - [マルチディスプレイ](https://lvgl.io/docs/open/main-modules/display/overview#how-many-displays-can-lvgl-use) 対応。

**レンダリング**
  - 基本図形、グラデーション、アンチエイリアス、不透明度、スムーススクロール、ボックスシャドウやドロップシャドウ、画像変換などに対応した組み込み2Dレンダリングエンジン。
  - OpenGLで [glTFモデル](https://sketchfab.com/) を表示する [強力な3Dレンダリングエンジン](https://lvgl.io/docs/open/libs/gltf)。
  - ベクターグラフィックス、SVG、Lottie対応。
  - テキストはUTF-8エンコーディングでレンダリングされ、CJK、タイ語、ヒンディー語、アラビア語、ペルシャ語の書記体系に対応します。
  - VG-Lite、Dave2D、NeoChrome、OpenGLなどのGPUを組み込みでサポート。

<a id="lvgl-pro"></a>
## LVGL Pro

C言語でのUI構築はうまく機能しますが、プロジェクトが成長するにつれて反復作業が遅くなり、一貫性を保つのが難しくなります。[LVGL Pro](https://lvgl.io/pro) を使えば、再利用可能なコンポーネントや画面をビジュアルに構築し、変更を即座にプレビューし、データバインディング、翻訳、アニメーション、テストを一箇所で管理できます。

Proは、追加のランタイムや依存関係のない、あなたがすでに使っているのと同じLVGLの、プレーンなLVGLのCコードをエクスポートします。ビルドや出荷の方法を変えることなく、既存のプロジェクトに組み込めます。

<p align="center">
  <img height="500" alt="ライブプレビュー付きのLVGL Pro editorでUIを構築する様子" src="https://github.com/user-attachments/assets/8cef0f05-0ff1-4766-8dfd-1d15e47f181a" />
</p>

何もインストールせずに [viewer.lvgl.io](https://viewer.lvgl.io) でブラウザ上で試すことも、[Editorをダウンロード](https://lvgl.io/pro#download) して無料のCommunityライセンスの下で使うこともできます。

LVGL Proは、密接に関連する4つのツールで構成されています。

1. **Editor**: LVGL Proの中核。XMLでコンポーネントや画面を構築し、データバインディング、翻訳、アニメーション、テストなどを管理するデスクトップアプリです。[XMLフォーマット](https://lvgl.io/docs/pro/syntax) と [ウィジェット](https://lvgl.io/docs/pro/built_in_widgets) について詳しく学べます。
2. **Online Viewer**: Editorをブラウザ上で実行し、GitHubプロジェクトを開き、開発環境をセットアップすることなく簡単に共有できます。[https://viewer.lvgl.io](https://viewer.lvgl.io) にアクセスしてください。
3. **Figma Plugin**: FigmaのデザインをワンクリックでLVGL Proに移行します。仕組みは [こちら](https://lvgl.io/docs/pro/figma) をご覧ください。
4. **CLI Tool**: Cコードを生成し、CI/CDでテストを実行します。詳細は [こちら](https://lvgl.io/docs/pro/cli) をご覧ください。

CommunityとEvaluationのティアは無料で使えます。商用利用については [ライセンス](#license) を参照してください。

<a id="examples"></a>
## サンプル

https://lvgl.io/docs/open/examples で100を超えるCおよびXMLのサンプルを確認できます。

LVGL Proの [Online Viewer](https://viewer.lvgl.io/) にも、多数のチュートリアルや、インタラクティブに操作できる同じサンプルが含まれています。

ちょっとした紹介として、同じシンプルなUIをCとXMLで書いたものを示します。

<p align="center">
  <img width="311" height="232" alt="クリックすると出力する中央揃えのボタン" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />
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

上記のXMLは、[LVGL Pro](#lvgl-pro) が扱うものです。画面をビジュアルに構築すると、Proがそれに対応するCコードを生成してくれます。

<a id="integration"></a>
## 統合

LVGLには外部依存関係がないため、あらゆるデバイス向けに簡単にコンパイルできます。組み込みドライバーが付属しており、多くのパッケージマネージャーやRTOSで利用でき、新しいデバイスへの移植も容易です。

### 事前統合済み

- **チップベンダー**: [ESP32](https://components.espressif.com/components/lvgl/lvgl)、[NXP MCUXpressoコンポーネント](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)、[Renesas FSP](https://lvgl.io/docs/open/integration/chip/renesas)、[STM32](https://lvgl.io/docs/open/integration/chip/stm32)

- **RTOS**: [Zephyr](https://lvgl.io/docs/open/integration/os/zephyr)、[NuttX](https://lvgl.io/docs/open/integration/os/nuttx)、[RT-Thread](https://lvgl.io/docs/open/integration/os/rt-thread)

- **フレームワーク**: [Arduino](https://lvgl.io/docs/open/integration/framework/arduino)、[PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl)、[CMSIS-Pack](https://lvgl.io/docs/open/integration/framework/cmsis-pack)

- **ボードメーカー**: [Seeed Studio](https://www.seeedstudio.com)、[Elecrow](https://www.elecrow.com/display/esp-hmi-display.html)、[Riverdi](https://lvgl.io/docs/open/integration/boards/manufacturers/riverdi)、[VIEWE](https://lvgl.io/docs/open/integration/boards/manufacturers/viewe)、[その他多数](https://lvgl.io/boards)

### 組み込みドライバー
LVGLにはすぐに使えるドライバーが付属しているため、一般的なプラットフォームではディスプレイや入力処理を自分で書く必要はありません。

- **Simulator / デスクトップ**: [SDL](https://lvgl.io/docs/open/integration/pc/sdl)、X11、[Wayland](https://lvgl.io/docs/open/integration/embedded_linux/drivers/wayland) のウィンドウで、PC上でUIを開発・プレビューできます。
- **ディスプレイコントローラー**: 汎用MIPI-DBI/SPI LCD（[ILI9341](https://lvgl.io/docs/open/integration/external_display_controllers/ili9341)、ST7789など）に加え、STの [LTDC](https://lvgl.io/docs/open/integration/chip_vendors/stm32/ltdc) やNXPの [eLCDIF](https://lvgl.io/docs/open/integration/chip_vendors/nxp/elcdif) といったベンダーコントローラー。
- **組み込みLinux**: framebuffer（fbdev）、DRM/KMS、Wayland、そしてタッチとポインタ入力用の `libinput`/`evdev`。詳細は [こちら](https://lvgl.io/docs/open/integration/embedded_linux) をご覧ください。
- **GPU / アクセラレーター**: VG-Lite、NXP PXP、ThinkSilicon NemaGFX、Arm-2D、[OpenGL ES](https://lvgl.io/docs/open/integration/embedded_linux/drivers/opengl_driver)

完全な一覧とセットアップガイドは [統合ドキュメント](https://lvgl.io/docs/open/integration) をご覧ください。

### LVGL Proでの統合

LVGL Proでは、UIのみ、VSCode、Zephyr、Linuxのすぐに使えるプロジェクトをワンクリックで作成できます。
<img width="600" alt="image" src="https://github.com/user-attachments/assets/5aadb850-6b40-49d1-ba96-2296041c7e27" />

### 手動での移植

LVGLの統合はとても簡単です。任意のプロジェクトに配置し、他のファイルと同じようにコンパイルするだけです。LVGLを設定するには、`lv_conf_template.h` を `lv_conf.h` としてコピーし、最初の `#if 0` を有効にして、必要に応じて設定を調整します。（デフォルトの設定で通常は問題ありません。）利用可能であれば、LVGLはKconfigと併用することもできます。

プロジェクトに組み込んだら、次のようにLVGLを初期化し、ディスプレイと入力デバイスを作成できます。

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
## コントリビュート

LVGLはオープンなプロジェクトであり、コントリビュートを大いに歓迎しています。あなたのプロジェクトについて話すこと、サンプルを書くこと、ドキュメントを改善すること、バグを修正すること、さらには自分のプロジェクトをLVGL組織の下でホストすることまで、貢献する方法はたくさんあります。

コントリビュートの機会についての詳しい説明は、ドキュメントの [コントリビュート](https://lvgl.io/docs/open/contributing) セクションをご覧ください。

すでに何百人もの人々がLVGLに足跡を残しています。あなたもその一人になりましょう！ここでお会いしましょう！ 🙂


<a id="license"></a>
## ライセンス

LVGLライブラリは **MITライセンス** の下で配布されているため、オープンソース製品でも商用製品でもロイヤリティなしで自由に使用できます。[`LICENCE.txt`](../LICENCE.txt) を参照してください。

LVGLにバンドルされているすべてのサードパーティライブラリもMIT互換ライセンスの下でリリースされているため、LVGLとその依存関係を安心して使用できます。

**[LVGL Pro](https://lvgl.io/pro)** には別途ライセンスがあります。
- **Community** と **Evaluation** のティアは **非商用利用では無料** であり、学習、趣味のプロジェクト、ツールの評価に最適です。
- LVGL Proの商用利用には有料ライセンスが必要です。[料金と詳細](https://lvgl.io/pro#pricing) を参照してください。
