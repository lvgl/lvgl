<a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets/sponsor.png" height="32px"></a>



<p align="right"><a href="../README.md">English</a> | <a href="./README_zh.md">中文</a> | <a href="./README_pt_BR.md">Português do Brasil</a> | <b>日本語</b> | <a href="./README_he.md">עברית</a>
</p>

<p align="center">
  <img src="https://lvgl.io/github-assets/logo-colored.png" width=300px>
</p>


  <h1 align="center">Light and Versatile Graphics Library</h1>
  <br>
<div align="center">
  <img src="https://raw.githubusercontent.com/kisvegabor/test/master/smartwatch_demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
</div>

<br/>


<p align="center">
  <a href="https://lvgl.io" title="LVGL ホームページ">ウェブサイト</a> |
  <a href="https://pro.lvgl.io" title="LVGL Pro XML ベースの UI エディター">LVGL Pro エディター</a> |
  <a href="https://docs.lvgl.io/" title="100 以上のサンプル付き詳細ドキュメント">ドキュメント</a> |
  <a href="https://forum.lvgl.io" title="質問・回答コミュニティ">フォーラム</a> |
  <a href="https://lvgl.io/demos" title="ブラウザで実行できるデモ">デモ</a> |
  <a href="https://lvgl.io/services" title="グラフィックデザイン、UI 実装、コンサルティング">サービス</a>
</p>

<br/>

## 📒 概要

**LVGL** は、あらゆるメーカーやプラットフォームの MCU および MPU に対応した、無料でオープンソースの UI ライブラリです。

**要件**：LVGL は外部依存がないため、最新のあらゆるターゲットに簡単にコンパイルできます。小型 MCU から 3D 対応のマルチコア Linux MPU まで対応。シンプルな UI の場合、約 100kB の RAM、200～300kB のフラッシュ、および画面の 1/10 サイズのバッファで十分です。

**はじめに**：VSCode、Eclipse などのプロジェクトを選び、PC 上で LVGL を試してみましょう。LVGL の UI コードは完全にプラットフォーム非依存なので、そのまま組込みターゲットでも使用できます。

**LVGL Pro** は、UI をより迅速に構築・テスト・共有・出荷できる完全なツールキットです。XML エディターを備え、再利用可能なコンポーネントの作成・テスト、C コードのエクスポート、XML の実行時読み込みが可能です。詳細は公式サイトをご覧ください。

## 💡 機能

**無料でポータブル**
  - 外部依存のない完全な C（C++ 互換）ライブラリ。
  - あらゆる MCU / MPU、任意の (RT)OS に対応。Make、CMake、glob 構文をサポート。
  - モノクロ、電子ペーパー、OLED、TFT、さらにはモニターにも対応。[ディスプレイ](https://docs.lvgl.io/master/main-modules/display/index.html)
  - MIT ライセンスで商用利用も簡単。
  - 必要リソースはわずか 32kB RAM、128kB フラッシュ、フレームバッファ、および画面の 1/10 の描画バッファ。
  - OS、外部メモリ、GPU の使用は任意。

**ウィジェット・スタイル・レイアウトなど**
  - 30 以上の [ウィジェット](https://docs.lvgl.io/master/widgets/index.html)：ボタン、ラベル、スライダー、チャート、キーボード、メーター、アーク、テーブルなど。
  - 約 100 のプロパティを持つ柔軟な [スタイルシステム](https://docs.lvgl.io/master/common-widget-features/styles/index.html)。
  - [Flexbox](https://docs.lvgl.io/master/common-widget-features/layouts/flex.html)、[Grid](https://docs.lvgl.io/master/common-widget-features/layouts/grid.html) レイアウトエンジンによる自動配置。
  - UTF-8 文字レンダリング。CJK、タイ語、ヒンディー語、アラビア語、ペルシャ語などをサポート。
  - [データバインディング](https://docs.lvgl.io/master/main-modules/data_binding/index.html) により、UI とアプリケーションを簡単に接続。
  - レンダリングエンジンはアニメーション、アンチエイリアス、不透明度、スムーズスクロール、影、画像変換をサポート。
  - [3D レンダリングエンジン](https://docs.lvgl.io/master/libs/gltf.html) により [glTF モデル](https://sketchfab.com/) を OpenGL で表示可能。
  - マウス、タッチパッド、キー入力、外部ボタン、エンコーダなどに対応。[入力デバイス](https://docs.lvgl.io/master/main-modules/indev.html)
  - [マルチディスプレイ](https://docs.lvgl.io/master/main-modules/display/overview.html#how-many-displays-can-lvgl-use) をサポート。

## 📦️ 対応プラットフォーム

LVGL は外部依存がなく、さまざまなデバイスで簡単にビルドでき、主要なパッケージマネージャや RTOS にも統合されています：

- [Arduino ライブラリ](https://docs.lvgl.io/master/integration/framework/arduino.html)
- [PlatformIO パッケージ](https://registry.platformio.org/libraries/lvgl/lvgl)
- [Zephyr ライブラリ](https://docs.lvgl.io/master/integration/os/zephyr.html)
- [ESP-IDF (ESP32) コンポーネント](https://components.espressif.com/components/lvgl/lvgl)
- [NXP MCUXpresso コンポーネント](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [NuttX ライブラリ](https://docs.lvgl.io/master/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/integration/os/rt-thread.html)
- CMSIS-Pack
- [RIOT OS パッケージ](https://doc.riot-os.org/group__pkg__lvgl.html#details)

## 🚀 LVGL Pro エディター

LVGL Pro は、組込み UI を効率的に構築・テスト・共有・出荷するための完全なツール群です。

以下の 4 つのツールで構成されています：

1. **XML エディター**：LVGL Pro の中心。XML でコンポーネントや画面を作成し、データバインディング、翻訳、アニメーション、テストなどを管理。詳細は [XML フォーマット](https://docs.lvgl.io/master/xml/xml/index.html) および [エディター](https://docs.lvgl.io/master/xml/editor/index.html)。
2. **オンラインビューア**：ブラウザでエディターを実行し、GitHub プロジェクトを開いて簡単に共有。環境構築は不要。アクセス：[https://viewer.lvgl.io](https://viewer.lvgl.io)
3. **CLI ツール**：C コードの生成や CI/CD テストを自動化。詳細は [こちら](https://docs.lvgl.io/master/xml/tools/cli.html)。
4. **Figma プラグイン**：Figma から直接スタイルを同期・抽出。[詳細はこちら](https://docs.lvgl.io/master/xml/tools/figma.html)。

これらのツールを組み合わせることで、開発者は効率的に UI を構築し、信頼性の高いテストとチームコラボレーションを実現できます。

詳細は https://pro.lvgl.io へ。

## 🤝 商用サービス

LVGL LLC は、UI 開発を支援するさまざまな商用サービスを提供しています。ユーザーインターフェースとグラフィック分野で 15 年以上の経験を活かし、あなたの製品 UI を次のレベルへ導きます。

- **グラフィックデザイン**：社内デザイナーが、製品とハードウェア性能に最適化された美しくモダンなデザインを提供。
- **UI 実装**：お客様または当社が作成したデザインに基づき、最適な UI を実装。必要なウィジェットがない場合も新たに開発します。
- **コンサルティング・サポート**：UI 開発での時間やコストの無駄を避けるための専門支援。
- **ボード認証**：開発ボードや製品キットを提供する企業向けに、LVGL の動作実績を示す認証を提供。

参考として [デモ](https://lvgl.io/demos) をご覧ください。詳細は [サービスページ](https://lvgl.io/services) を参照ください。

[お問い合わせ](https://lvgl.io/#contact) からご連絡ください。

## 🧑‍💻 LVGL の統合

LVGL の統合は非常に簡単です。プロジェクトに追加して他のファイルと同様にコンパイルするだけです。
設定するには、`lv_conf_template.h` を `lv_conf.h` としてコピーし、最初の `#if 0` を有効化して必要に応じて設定を調整します。
（通常はデフォルト設定で十分です。）Kconfig にも対応しています。

プロジェクト内で LVGL を初期化し、ディスプレイと入力デバイスを作成する例：

```c
#include "lvgl/lvgl.h" /*LV_LVGL_H_INCLUDE_SIMPLE を定義すると "lvgl.h" としてインクルード可能*/

#define TFT_HOR_RES 320
#define TFT_VER_RES 240

static uint32_t my_tick_cb(void)
{
    return my_get_millisec();
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /*px_map をフレームバッファまたは外部ディスプレイコントローラに書き込む*/
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

    /*LVGL を初期化*/
    lv_init();

    /*ミリ秒単位のタイマーコールバックを設定*/
    lv_tick_set_cb(my_tick_cb);

    /*ディスプレイを作成*/
    lv_display_t * display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);

    /*描画バッファを追加（16bit RGB565 の場合）*/
    static uint8_t buf[TFT_HOR_RES * TFT_VER_RES / 10 * 2];
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*描画内容をフラッシュするコールバックを設定*/
    lv_display_set_flush_cb(display, my_flush_cb);

    /*入力デバイスを作成（タッチ用）*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read_cb);

    /*UI を作成*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_center(label);

    /*メインループで LVGL を実行*/
    while(1) {
        lv_timer_handler();
        my_sleep_ms(5);
    }
}
```

## 🤖 サンプル

100 以上のサンプルが https://docs.lvgl.io/master/examples.html にあります。

オンラインビューア https://viewer.lvgl.io/ でも XML のチュートリアルを学べます。


### Hello World ボタンイベント

<img width="311" height="232" alt="image" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />

<details>
  <summary>C コード</summary>

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
  <summary>LVGL Pro の XML</summary>

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

### データバインド付きスライダー

<img width="314" height="233" alt="image" src="https://github.com/user-attachments/assets/268db1a0-946c-42e2-aee4-9550bdf5f4f9" />

<details>
  <summary>C コード</summary>

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
  <summary>LVGL Pro の XML</summary>

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

### レイアウト内のチェックボックス

<img width="311" height="231" alt="image" src="https://github.com/user-attachments/assets/ba9af647-2ea1-4bc8-b53d-c7b43ce24b6e" />

<details>
  <summary>C コード</summary>

  ```c
/* 新しい画面を作成してロード */
lv_obj_t * scr = lv_obj_create(NULL);
lv_screen_load(scr);

/* 縦方向のレイアウトを設定 */
lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY,
						   LV_FLEX_ALIGN_START,
						   LV_FLEX_ALIGN_CENTER);

/* チェックボックスを 5 個作成 */
const char * texts[5] = {"Input 1", "Input 2", "Input 3", "Output 1", "Output 2"};
for(int i = 0; i < 5; i++) {
	lv_obj_t * cb = lv_checkbox_create(scr);
	lv_checkbox_set_text(cb, texts[i]);
}

/* 状態変更 */
lv_obj_add_state(lv_obj_get_child(scr, 1), LV_STATE_CHECKED);
lv_obj_add_state(lv_obj_get_child(scr, 3), LV_STATE_DISABLED);
```

</details>

<details>
  <summary>LVGL Pro の XML</summary>

```xml
<screen

