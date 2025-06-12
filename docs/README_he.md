<a href="https://github.com/sponsors/lvgl" target="_blank"><img align="left" src="https://lvgl.io/github-assets/sponsor.png" height="32px"></a>

<p align="right">
  <b>עברית</b> | <a href="./docs/README_zh.md">中文</a> | <a href="./docs/README_pt_BR.md">Português do Brasil</a> | <a href="./docs/README_jp.md">日本語</a> | <a href="./README.md">English</a>
</p>

<br>

<p align="center">
  <img src="https://lvgl.io/github-assets/logo-colored.png" width=300px>
</p>

<h1 align="center">ספריית גרפיקה קלה ורב-שימושית</h1>
<br>
<div align="center">
  <img src="https://lvgl.io/github-assets/smartwatch-demo.gif">
  &nbsp;
  <img border="1px" src="https://lvgl.io/github-assets/widgets-demo.gif">
</div>
<br>
<p align="center">
<a href="https://lvgl.io" title="עמוד הבית של LVGL">אתר הבית </a> |
<a href="https://docs.lvgl.io/" title="תיעוד מפורט עם 100+ דוגמאות">תיעוד</a> |
<a href="https://forum.lvgl.io" title="קבל עזרה ועזור לאחרים">פורום</a> |
<a href="https://lvgl.io/demos" title="דמוים שרצים בדפדפן שלך">דמוים</a> |
<a href="https://lvgl.io/services" title="עיצוב גרפי, מימוש ממשק וייעוץ">שירותים</a>
</p>
<br>

## :ledger: סקירה כללית

**בוגרת ומוכרת**  
LVGL היא ספריית הגרפיקה החינמית והפתוחה הפופולרית ביותר ליצירת ממשקי משתמש יפים לכל MCU, MPU וסוג תצוגה. הספרייה נתמכת על ידי יצרני תעשייה מובילים ופרויקטים כגון Arm, STM32, NXP, Espressif, Nuvoton, Arduino, RT-Thread, Zephyr, NuttX, Adafruit ועוד רבים.

**עשירה בתכונות**  
הספרייה כוללת את כל מה שצריך ליצירת GUI מודרני ויפה: מעל 30 ווידג'טים מובנים, מערכת עיצוב עוצמתית, מנגנוני פריסה בהשראת האינטרנט, ומערכת טיפוגרפיה התומכת בשפות רבות. כדי לשלב את LVGL בפלטפורמה שלך, נדרשים לפחות 32kB RAM ו־128kB Flash, קומפיילר C, buffer גרפי ו־1/10 מגודל המסך כ־buffer נוסף לעיבוד.

**שירותים**  
הצוות שלנו מוכן לעזור בעיצוב גרפי, מימוש ממשקים ושירותי ייעוץ. פנו אלינו אם אתם זקוקים לתמיכה בפרויקט ה־GUI הבא שלכם.

## :rocket: תכונות

**חינמית וניידת**
-ספריית C מלאה (תואמת ++C) ללא תלות בספריות חיצוניות.
-ניתנת לקימפול על כל MCU או MPU, עם או בלי (RT)OS.
-תומכת בתצוגות מונוכרום, ePaper, OLED, TFT ואפילו במסכים גדולים. [מידע על תצוגות](https://docs.lvgl.io/master/details/main-modules/display/index.html)
-תחת רישיון MIT – מותר לשימוש חופשי, כולל בפרויקטים מסחריים.
-דורשת רק 32kB RAM ו־128kB Flash, buffer גרפי, ו־1/10 מגודל המסך ל־rendering.
-תמיכה ב־OS, זיכרון חיצוני ו־GPU – אך לא נדרש.

**ווידג'טים, עיצובים, פריסות ועוד**
-מעל 30 [ווידג'טים](https://docs.lvgl.io/master/details/widgets/index.html) מובנים: כפתור, תווית, סליידר, טבלה, מקלדת, מד, קשת, לוח ועוד.
-מערכת [עיצובים גמישה](https://docs.lvgl.io/master/details/common-widget-features/styles/style.html) עם ~100 מאפייני עיצוב להתאמה אישית.
-מנועים לפריסה בסגנון [Flexbox](https://docs.lvgl.io/master/details/common-widget-features/layouts/flex.html) ו-[Grid](https://docs.lvgl.io/master/details/common-widget-features/layouts/grid.html) להתאמה רספונסיבית.
-תומכת בטקסטים ב־UTF-8 כולל עברית, סינית, תאית, הינדית, ערבית, פרסית ועוד.
-תמיכה ב־word wrapping, kerning, גלילה, תצוגת תווים בעברית, סמיילים ועוד.
-מנוע גרפי עם אנימציות, anti-aliasing, שקיפויות, גלילה חלקה, צללים, שינוי תמונות ועוד.
-תמיכה בעכבר, מסך מגע, מקשים חיצוניים, מקודדים ועוד. [התקני קלט](https://docs.lvgl.io/master/details/main-modules/indev.html).
-תמיכה בריבוי מסכים. [מידע נוסף](https://docs.lvgl.io/master/details/main-modules/display/overview.html#how-many-displays-can-lvgl-use)

**קישוריות ותמיכה בבנייה**
-[קישור ל־MicroPython](https://blog.lvgl.io/2019-02-20/micropython-bindings) – כל ה־API של LVGL בפייתון.
-[קישור ל־PikaScript](https://blog.lvgl.io/2022-08-24/pikascript-and-lvgl) – פייתון למיקרו-בקרים.
-אין צורך במערכת בנייה ייחודית – פשוט לקמפל כמו כל קובץ C אחר.
-תמיכה מלאה ב־Make וב־[CMake](https://docs.lvgl.io/master/details/integration/building/cmake.html).
-פיתוח ב־PC באמצעות [סימולטור](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html) – אותו קוד רץ גם בחומרה משובצת.
-המרת קוד C ל־HTML באמצעות [פורטים ל־Emscripten](https://github.com/lvgl/lv_web_emscripten).

**תיעוד, כלים ושירותים**
-[תיעוד מלא](https://docs.lvgl.io/) כולל [מעל 100 דוגמאות](https://docs.lvgl.io/master/examples.html)
-[שירותים](https://lvgl.io/services): עיצוב UI, מימוש וייעוץ – להאיץ פיתוח.

## :heart: נותני חסות

אם LVGL חסכה לך זמן וכסף, או אם נהנית להשתמש בה – שקול [לתמוך בפרויקט](https://github.com/sponsors/lvgl).

**מה עושים עם התרומות?**  
המטרה היא לתגמל כל מי שתורם בצורה משמעותית לספרייה – לא רק את המתחזקים, אלא כל מי שמוסיף פיצ'ר משמעותי או מתקן באגים. התרומות מכסות גם עלויות תפעול (שרתים, שירותים וכו').

**איך תורמים?**  
באמצעות [GitHub Sponsors](https://github.com/sponsors/lvgl) – אפשר חד־פעמי או חודשי, הכל גלוי.

**איך מקבלים תשלום על תרומה?**  
אם תיקנת או פיתחת נושא שמסומן ב־[Sponsored](https://github.com/lvgl/lvgl/labels/Sponsored), לאחר המיזוג ניתן להגיש חשבונית ולקבל תשלום דרך [opencollective.com](https://opencollective.com/lvgl).

**חברות שתומכות ב־LVGL**  
[![Sponsors of LVGL](https://opencollective.com/lvgl/organizations.svg?width=600)](https://opencollective.com/lvgl)

**תורמים פרטיים**  
[![Backers of LVGL](https://contrib.rocks/image?repo=lvgl/lvgl&max=48)](https://opencollective.com/lvgl)

## :package: חבילות
LVGL זמינה כ:
- [ספריית Arduino](https://docs.lvgl.io/master/details/integration/framework/arduino.html)
- [חבילת PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl)
- [ספריית Zephyr](https://docs.lvgl.io/master/details/integration/os/zephyr.html)
- [רכיב ESP-IDF(ESP32)](https://components.espressif.com/components/lvgl/lvgl)
- [רכיב NXP MCUXpresso](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [ספריית NuttX](https://docs.lvgl.io/master/details/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/details/integration/os/rt-thread.html)
- CMSIS-Pack
- [חבילת RIOT OS](https://doc.riot-os.org/group__pkg__lvgl.html#details)

## :robot: דוגמאות

ראה כמה דוגמאות של יצירת ווידג'טים, שימוש בפריסות והחלת סגנונות. תמצא כאן קוד ב-C ו-MicroPython, וקישורים לניסיון או עריכת הדוגמאות בעורך MicroPython מקוון.

לדוגמאות נוספות, בדוק את [תיקיית הדוגמאות](https://github.com/lvgl/lvgl/tree/master/examples).


### תווית Hello world

![דוגמה פשוטה של תווית Hello world ב-LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_1.png)

<details>
  <summary>קוד C</summary>

```c
/*שינוי צבע הרקע של המסך הפעיל*/
lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

/*יצירת תווית לבנה, הגדרת הטקסט שלה ויישור למרכז*/
lv_obj_t * label = lv_label_create(lv_screen_active());
lv_label_set_text(label, "Hello world");
lv_obj_set_style_text_color(label, lv_color_hex(0xffffff), LV_PART_MAIN);
lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
```
</details>

<details>
  <summary>קוד MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_direct=4ab7c40c35b0dc349aa2f0c3b00938d7d8e8ac9f" target="_blank">סימולטור מקוון</a></summary>

```python
# שינוי צבע הרקע של המסך הפעיל
scr = lv.screen_active()
scr.set_style_bg_color(lv.color_hex(0x003a57), lv.PART.MAIN)

# יצירת תווית לבנה, הגדרת הטקסט שלה ויישור למרכז
label = lv.label(lv.screen_active())
label.set_text("Hello world")
label.set_style_text_color(lv.color_hex(0xffffff), lv.PART.MAIN)
label.align(lv.ALIGN.CENTER, 0, 0)
```
</details>
<br>

### כפתור עם אירוע לחיצה

![דוגמת כפתור LVGL עם תווית](https://github.com/kisvegabor/test/raw/master/readme_example_2.gif)

<details>
  <summary>קוד C</summary>

```c
lv_obj_t * button = lv_button_create(lv_screen_active());                   /*הוספת כפתור למסך הנוכחי*/
lv_obj_center(button);                                             /*הגדרת המיקום שלו*/
lv_obj_set_size(button, 100, 50);                                  /*הגדרת הגודל שלו*/
lv_obj_add_event_cb(button, button_event_cb, LV_EVENT_CLICKED, NULL); /*הקצאת callback לכפתור*/

lv_obj_t * label = lv_label_create(button);                        /*הוספת תווית לכפתור*/
lv_label_set_text(label, "כפתור");                             /*הגדרת טקסט התווית*/
lv_obj_center(label);                                           /*יישור התווית למרכז*/
...

void button_event_cb(lv_event_t * e)
{
  printf("נלחץ\n");
}
```
</details>

<details>
  <summary>קוד MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=926bde43ec7af0146c486de470c53f11f167491e" target="_blank">סימולטור מקוון</a></summary>

```python
def button_event_cb(e):
  print("נלחץ")

# יצירת כפתור ותווית
button = lv.button(lv.screen_active())
button.center()
button.set_size(100, 50)
button.add_event_cb(button_event_cb, lv.EVENT.CLICKED, None)

label = lv.label(button)
label.set_text("כפתור")
label.center()
```
</details>
<br>

### תיבות סימון עם פריסה
![תיבות סימון עם פריסה ב-LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_3.gif)

<details>
  <summary>קוד C</summary>

```c
lv_obj_set_flex_flow(lv_screen_active(), LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(lv_screen_active(), LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER);

lv_obj_t * cb;
cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "תפוח");
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "בננה");
lv_obj_add_state(cb, LV_STATE_CHECKED);
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_checkbox_set_text(cb, "לימון");
lv_obj_add_state(cb, LV_STATE_DISABLED);
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);

cb = lv_checkbox_create(lv_screen_active());
lv_obj_add_state(cb, LV_STATE_CHECKED | LV_STATE_DISABLED);
lv_checkbox_set_text(cb, "מלון\nושורה חדשה");
lv_obj_add_event_cb(cb, event_handler, LV_EVENT_ALL, NULL);
```
</details>

<details>
  <summary>קוד MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=311d37e5f70daf1cb0d2cad24c7f72751b5f1792" target="_blank">סימולטור מקוון</a></summary>

```python
def event_handler(e):
    code = e.get_code()
    obj = e.get_target_obj()
    if code == lv.EVENT.VALUE_CHANGED:
        txt = obj.get_text()
        if obj.get_state() & lv.STATE.CHECKED:
            state = "מסומן"
        else:
            state = "לא מסומן"
        print(txt + ":" + state)


lv.screen_active().set_flex_flow(lv.FLEX_FLOW.COLUMN)
lv.screen_active().set_flex_align(lv.FLEX_ALIGN.CENTER, lv.FLEX_ALIGN.START, lv.FLEX_ALIGN.CENTER)

cb = lv.checkbox(lv.screen_active())
cb.set_text("תפוח")
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("בננה")
cb.add_state(lv.STATE.CHECKED)
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.set_text("לימון")
cb.add_state(lv.STATE.DISABLED)
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)

cb = lv.checkbox(lv.screen_active())
cb.add_state(lv.STATE.CHECKED | lv.STATE.DISABLED)
cb.set_text("מלון")
cb.add_event_cb(event_handler, lv.EVENT.ALL, None)
```
</details>
<br>

### עיצוב מחוון (Slider)
![עיצוב מחוון עם LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_4.gif)

<details>
  <summary>קוד C</summary>

```c
lv_obj_t * slider = lv_slider_create(lv_screen_active());
lv_slider_set_value(slider, 70, LV_ANIM_OFF);
lv_obj_set_size(slider, 300, 20);
lv_obj_center(slider);

/*הוספת סגנונות מקומיים לחלק הראשי (מלבן הרקע)*/
lv_obj_set_style_bg_color(slider, lv_color_hex(0x0F1215), LV_PART_MAIN);
lv_obj_set_style_bg_opa(slider, 255, LV_PART_MAIN);
lv_obj_set_style_border_color(slider, lv_color_hex(0x333943), LV_PART_MAIN);
lv_obj_set_style_border_width(slider, 5, LV_PART_MAIN);
lv_obj_set_style_pad_all(slider, 5, LV_PART_MAIN);

/*יצירת גיליון סגנונות לשימוש חוזר עבור חלק המחוון*/
static lv_style_t style_indicator;
lv_style_init(&style_indicator);
lv_style_set_bg_color(&style_indicator, lv_color_hex(0x37B9F5));
lv_style_set_bg_grad_color(&style_indicator, lv_color_hex(0x1464F0));
lv_style_set_bg_grad_dir(&style_indicator, LV_GRAD_DIR_HOR);
lv_style_set_shadow_color(&style_indicator, lv_color_hex(0x37B9F5));
lv_style_set_shadow_width(&style_indicator, 15);
lv_style_set_shadow_spread(&style_indicator, 5);

/*הוספת גיליון הסגנונות לחלק המחוון של הסליידר*/
lv_obj_add_style(slider, &style_indicator, LV_PART_INDICATOR);

/*הוספת אותו סגנון גם לחלק הכפתור ושינוי מקומי של כמה מאפיינים*/
lv_obj_add_style(slider, &style_indicator, LV_PART_KNOB);

lv_obj_set_style_outline_color(slider, lv_color_hex(0x0096FF), LV_PART_KNOB);
lv_obj_set_style_outline_width(slider, 3, LV_PART_KNOB);
lv_obj_set_style_outline_pad(slider, -5, LV_PART_KNOB);
lv_obj_set_style_shadow_spread(slider, 2, LV_PART_KNOB);
```
</details>

<details>
  <summary>קוד MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=c431c7b4dfd2cc0dd9c392b74365d5af6ea986f0" target="_blank">סימולטור מקוון</a></summary>

```python
# יצירת מחוון והוספת הסגנון
slider = lv.slider(lv.screen_active())
slider.set_value(70, lv.ANIM.OFF)
slider.set_size(300, 20)
slider.center()

# הוספת סגנונות מקומיים לחלק הראשי (מלבן הרקע)
slider.set_style_bg_color(lv.color_hex(0x0F1215), lv.PART.MAIN)
slider.set_style_bg_opa(255, lv.PART.MAIN)
slider.set_style_border_color(lv.color_hex(0x333943), lv.PART.MAIN)
slider.set_style_border_width(5, lv.PART.MAIN)
slider.set_style_pad_all(5, lv.PART.MAIN)

# יצירת גיליון סגנונות לשימוש חוזר עבור חלק המחוון
style_indicator = lv.style_t()
style_indicator.init()
style_indicator.set_bg_color(lv.color_hex(0x37B9F5))
style_indicator.set_bg_grad_color(lv.color_hex(0x1464F0))
style_indicator.set_bg_grad_dir(lv.GRAD_DIR.HOR)
style_indicator.set_shadow_color(lv.color_hex(0x37B9F5))
style_indicator.set_shadow_width(15)
style_indicator.set_shadow_spread(5)

# הוספת גיליון הסגנונות לחלק המחוון של הסליידר
slider.add_style(style_indicator, lv.PART.INDICATOR)
slider.add_style(style_indicator, lv.PART.KNOB)

# הוספת אותו סגנון לחלק הכפתור ושינוי מקומי של כמה מאפיינים
slider.set_style_outline_color(lv.color_hex(0x0096FF), lv.PART.KNOB)
slider.set_style_outline_width(3, lv.PART.KNOB)
slider.set_style_outline_pad(-5, lv.PART.KNOB)
slider.set_style_shadow_spread(2, lv.PART.KNOB)
```
</details>
<br>

### טקסטים באנגלית, עברית (מעורב LTR-RTL) וסינית

![טקסטים באנגלית, עברית וסינית עם LVGL](https://github.com/kisvegabor/test/raw/master/readme_example_5.png)

<details>
  <summary>קוד C</summary>

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
lv_obj_set_style_text_font(cz_label, &lv_font_source_han_sans_sc_16_cjk, 0);
lv_obj_set_width(cz_label, 310);
lv_obj_align(cz_label, LV_ALIGN_BOTTOM_LEFT, 5, -5);
```
</details>

<details>
  <summary>קוד MicroPython | <a href="https://sim.lvgl.io/v8.3/micropython/ports/javascript/index.html?script_startup=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/header.py&script=https://raw.githubusercontent.com/lvgl/lvgl/0d9ab4ee0e591aad1970e3c9164fd7c544ecce70/examples/widgets/slider/lv_example_slider_2.py&script_direct=18bb38200a64e10ead1aa17a65c977fc18131842" target="_blank">סימולטור מקוון</a></summary>

```python
ltr_label = lv.label(lv.screen_active())
ltr_label.set_text("In modern terminology, a microcontroller is similar to a system on a chip (SoC).")
ltr_label.set_style_text_font(lv.font_montserrat_16, 0)
ltr_label.set_width(310)
ltr_label.align(lv.ALIGN.TOP_LEFT, 5, 5)

rtl_label = lv.label(lv.screen_active())
rtl_label.set_text("מעבד, או בשמו המלא יחידת עיבוד מרכזית (באנגלית: CPU - Central Processing Unit).")
rtl_label.set_style_base_dir(lv.BASE_DIR.RTL, 0)
rtl_label.set_style_text_font(lv.font_dejavu_16_persian_hebrew, 0)
rtl_label.set_width(310)
rtl_label.align(lv.ALIGN.LEFT_MID, 5, 0)

cz_label = lv.label(lv.screen_active())
cz_label.set_text("嵌入式系统（Embedded System），\n是一种嵌入机械或电气系统内部、具有专一功能和实时计算性能的计算机系统。")
cz_label.set_style_text_font(lv.font_source_han_sans_sc_16_cjk, 0)
cz_label.set_width(310)
cz_label.align(lv.ALIGN.BOTTOM_LEFT, 5, -5)
```
</details>

## :arrow_forward: התחלה מהירה
הרשימה הבאה תדריך אותך כיצד להתחיל עם LVGL שלב אחר שלב.

**הכרת LVGL**

1.בדוק את [הדמו המקוון](https://lvgl.io/demos) כדי לראות את LVGL בפעולה (3 דקות).
2.קרא את עמוד [המבוא](https://docs.lvgl.io/master/intro/index.html) בתיעוד (5 דקות).
3.הכר את הבסיס בעמוד [סקירה מהירה](https://docs.lvgl.io/master/intro/getting_started.html#lvgl-basics) (15 דקות).

**התחל להשתמש ב-LVGL**

4.הגדר [סימולטור](https://docs.lvgl.io/master/details/integration/ide/pc-simulator.html#simulator) (10 דקות).
5.נסה כמה [דוגמאות](https://github.com/lvgl/lvgl/tree/master/examples).
6.העבר את LVGL ללוח. ראה את מדריך [הפורטינג](https://docs.lvgl.io/master/details/integration/adding-lvgl-to-your-project/index.html) או בדוק את [הפרויקטים](https://github.com/lvgl?q=lv_port_) המוכנים לשימוש.

**הפוך למקצוען**

7.קרא את עמוד [המודולים הראשיים](https://docs.lvgl.io/master/details/main-modules/index.html) כדי להבין טוב יותר את הספרייה (2-3 שעות)
8.בדוק את התיעוד של [הווידג'טים](https://docs.lvgl.io/master/details/widgets/index.html) כדי לראות את התכונות והשימוש שלהם

**קבל עזרה ועזור לאחרים**

9.אם יש לך שאלות, גש ל[פורום](http://forum.lvgl.io/)
10.קרא את מדריך [התרומה לפרויקט](https://docs.lvgl.io/master/contributing/index.html) כדי לראות כיצד תוכל לעזור לשפר את LVGL (15 דקות)


## :handshake: שירותים
LVGL LLC הוקמה כדי לספק רקע מוצק לספריית LVGL ולהציע מספר סוגים של שירותים כדי לעזור לך בפיתוח ממשק המשתמש. עם 15+ שנות ניסיון בתעשיית ממשק המשתמש והגרפיקה, אנחנו יכולים לעזור לך להביא את ממשק המשתמש שלך לרמה הבאה.

- **עיצוב גרפי** המעצבים הגרפיים שלנו הם מומחים ביצירת עיצובים מודרניים יפים המתאימים למוצר שלך ולמשאבי החומרה שלך.
- **מימוש ממשק משתמש** אנחנו יכולים גם לממש את ממשק המשתמש שלך בהתבסס על העיצוב שאתה או אנחנו יצרנו. אתה יכול להיות בטוח שנפיק את המרב מהחומרה שלך ומ-LVGL. אם חסרה תכונה או ווידג'ט ב-LVGL, אל דאגה, נממש אותם עבורך.
- **ייעוץ ותמיכה** אנחנו יכולים לתמוך בך גם בייעוץ כדי למנוע טעויות יקרות וזוללות זמן במהלך פיתוח ממשק המשתמש.
- **הסמכת לוחות** לחברות המציעות לוחות פיתוח או ערכות מוכנות לייצור, אנו מבצעים הסמכת לוחות המראה כיצד הלוח יכול להריץ LVGL.


בדוק את [הדמואים](https://lvgl.io/demos) שלנו כדוגמה. למידע נוסף, עיין ב[עמוד השירותים](https://lvgl.io/services).

[צור קשר](https://lvgl.io/#contact) וספר לנו כיצד נוכל לעזור.


## :star2: תרומה לפרויקט
LVGL הוא פרויקט פתוח ותרומה מתקבלת בברכה. ישנן דרכים רבות לתרום, החל מלדבר על הפרויקט שלך, דרך כתיבת דוגמאות, שיפור התיעוד, תיקון באגים ואפילו אירוח הפרויקט שלך תחת ארגון LVGL.

לתיאור מפורט של הזדמנויות תרומה, בקר בסעיף [תרומה לפרויקט](https://docs.lvgl.io/master/contributing/index.html) בתיעוד.

יותר מ-300 אנשים כבר השאירו את חותמם ב-LVGL. היה אחד מהם! נתראה כאן! :slightly_smiling_face:

<a href="https://github.com/lvgl/lvgl/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" />
</a>

... ורבים אחרים.