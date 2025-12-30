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

<br/>


<p align="center">
  <a href="https://lvgl.io" title="עמוד הבית של LVGL">אתר</a> |
  <a href="https://pro.lvgl.io" title="עורך UI מבוסס XML של LVGL Pro">עורך LVGL Pro</a> |
  <a href="https://docs.lvgl.io/" title="תיעוד מפורט עם יותר מ־100 דוגמאות">תיעוד</a> |
  <a href="https://forum.lvgl.io" title="קבל עזרה ועזור לאחרים">פורום</a> |
  <a href="https://lvgl.io/demos" title="הדגמות שרצות בדפדפן">דוגמאות</a> |
  <a href="https://lvgl.io/services" title="עיצוב גרפי, יישום UI וייעוץ">שירותים</a>
</p>

<br/>

## 📒 סקירה

**LVGL** היא ספריית UI חינמית וקוד פתוח שמאפשרת ליצור ממשקי משתמש גרפיים לכל MCU או MPU מכל ספק ועל כל פלטפורמה.

**דרישות**: ל־LVGL אין תלות חיצונית, מה שמקל על הידור לכל יעד מודרני, החל מ־MCU קטנים ועד MPU מבוססי Linux מרובי ליבות עם תמיכת 3D. עבור UI פשוט דרושים רק ~100kB RAM, כ~200–300kB Flash ומאגר רינדור בגודל 1/10 מגודל המסך.

**להתחלה מהירה**: בחרו פרויקט מוכן ל־VSCode,‏ Eclipse או כל סביבת פיתוח אחרת ונסו את LVGL על המחשב. קוד ה־UI של LVGL בלתי תלוי בפלטפורמה, ולכן ניתן להשתמש באותו קוד גם ביעדים משובצים.

**LVGL Pro** הוא מארז כלים מלא לבנייה, בדיקה, שיתוף והפצה מהירה יותר של UI. הוא כולל עורך XML ליצירה ולבדיקה מהירה של רכיבים לשימוש חוזר, ייצוא קוד C או טעינת קובצי XML בזמן ריצה. מידע נוסף באתר.

## 💡 יכולות

**חינמית וניידת**
  - ספריית C מלאה (תואמת ++C) ללא תלות חיצונית.
  - ניתנת לקימפול לכל MCU או MPU, עם כל (RT)OS. תומכת ב־Make,‏ CMake וב־globbing פשוט.
  - תמיכה במסכי מונוכרום, ePaper,‏ OLED או TFT, ואף במסכים של מחשבים. ראו [תצוגות](https://docs.lvgl.io/master/main-modules/display/index.html)
  - רישיון MIT המקל על שימוש מסחרי.
  - נדרשים רק 32kB RAM ו־128kB Flash, מאגר מסגרת, ולפחות מאגר רינדור בגודל 1/10 מגודל המסך.
  - מערכת הפעלה, זיכרון חיצוני ו־GPU נתמכים אך אינם נדרשים.

**וידג'טים, סגנונות, פריסות ועוד**
  - יותר מ־30 [וידג'טים](https://docs.lvgl.io/master/widgets/index.html) מובנים: Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table ועוד רבים.
  - [מערכת סגנונות](https://docs.lvgl.io/master/common-widget-features/styles/index.html) גמישה עם כ~100 מאפייני עיצוב להתאמה של כל חלקי הווידג'טים בכל מצב.
  - מנועי פריסה בסגנון [Flexbox](https://docs.lvgl.io/master/common-widget-features/layouts/flex.html) ו־[Grid](https://docs.lvgl.io/master/common-widget-features/layouts/grid.html) למיקום וגודל רספונסיביים.
  - טקסט מעובד בקידוד UTF-8 עם תמיכה במערכות כתיבה סינית/יפנית/קוריאנית, תאית, הינדי, ערבית ופרסית.
  - [קישורי נתונים](https://docs.lvgl.io/master/main-modules/observer/index.html) לחיבור קל בין ה־UI ליישום.
  - מנוע רינדור עם תמיכה באנימציות, החלקת קצוות, שקיפות, גלילה חלקה, צללים, טרנספורמציות תמונה ועוד.
  - [מנוע רינדור 3D חזק](https://docs.lvgl.io/master/libs/gltf.html) להצגת [מודלי glTF](https://sketchfab.com/) עם OpenGL.
  - תמיכה בעכבר, משטח מגע, לוח מקשים, מקלדת, כפתורים חיצוניים ואנקודר [התקני קלט](https://docs.lvgl.io/master/main-modules/indev.html).
  - תמיכה ב[מסכים מרובים](https://docs.lvgl.io/master/main-modules/display/overview.html#how-many-displays-can-lvgl-use).

## 📦️ תמיכת פלטפורמות

ל־LVGL אין תלות חיצונית, ולכן ניתן לקמפל אותה בקלות לכל התקן. היא זמינה גם במנהלי חבילות רבים ובמערכות RTOS:

- [ספריית Arduino](https://docs.lvgl.io/master/integration/framework/arduino.html)
- [חבילת PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl)
- [ספריית Zephyr](https://docs.lvgl.io/master/integration/os/zephyr.html)
- [רכיב ESP-IDF ‏(ESP32)](https://components.espressif.com/components/lvgl/lvgl)
- [רכיב NXP MCUXpresso](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY)
- [ספריית NuttX](https://docs.lvgl.io/master/integration/os/nuttx.html)
- [RT-Thread RTOS](https://docs.lvgl.io/master/integration/os/rt-thread.html)
- CMSIS-Pack
- [חבילת RIOT OS](https://doc.riot-os.org/group__pkg__lvgl.html#details)

## 🚀 עורך LVGL Pro

LVGL Pro הוא מארז כלים מלא לבנייה, בדיקה, שיתוף והפצה יעילים של UI משובץ.

הוא מורכב מארבעה כלים המשולבים זה בזה:

1. **עורך XML**: הלב של LVGL Pro. יישום שולחני לבניית רכיבים ומסכים ב־XML, ניהול קישורי נתונים, תרגומים, אנימציות, בדיקות ועוד. קראו עוד על [פורמט ה־XML](https://docs.lvgl.io/master/xml/xml/index.html) ועל [העורך](https://docs.lvgl.io/master/xml/editor/index.html).
2. **מציג מקוון**: הריצו את העורך בדפדפן, פתחו פרויקטים מ־GitHub ושיתפו בקלות בלי להקים סביבת פיתוח. בקרו ב־[https://viewer.lvgl.io](https://viewer.lvgl.io).
3. **כלי CLI**: הפקת קוד C והרצת בדיקות ב־CI/CD. פרטים [כאן](https://docs.lvgl.io/master/xml/tools/cli.html).
4. **תוסף Figma**: סנכרון וחילוץ סגנונות ישירות מ־Figma. מידע נוסף [כאן](https://docs.lvgl.io/master/xml/tools/figma.html).

ביחד, הכלים מאפשרים לבנות UI ביעילות, לבדוק באמינות ולשתף פעולה עם צוותים ולקוחות.

מידע נוסף: https://pro.lvgl.io

## 🤝 שירותים מסחריים

חברת LVGL LLC מספקת מגוון שירותים מסחריים לסיוע בפיתוח UI. עם יותר מ־15 שנות ניסיון בתחום הממשקים והגרפיקה, נוכל לקדם את ה־UI של המוצר שלכם לרמה הבאה.

- **עיצוב גרפי**: המעצבים הפנימיים שלנו מומחים בעיצוב מודרני ויפה שמתאים למוצר וליכולות החומרה.
- **מימוש UI**: ניישם את ה־UI על בסיס העיצוב שלכם או שלנו, ונמצה את יכולות החומרה ו־LVGL. אם חסר רכיב או יכולת ב־LVGL, נפתח זאת עבורכם.
- **ייעוץ ותמיכה**: ייעוץ מקצועי למניעת טעויות יקרות וגוזלות זמן במהלך פיתוח ה־UI.
- **אישור לוחות**: לחברות שמציעות לוחות פיתוח או ערכות מוכנות לייצור אנו מספקים הסמכה שמדגימה כיצד הלוח מריץ LVGL.

עיינו ב[דוגמאות](https://lvgl.io/demos) שלנו לע参考. למידע נוסף ראו את [עמוד השירותים](https://lvgl.io/services).

[צרו קשר](https://lvgl.io/#contact) וספרו לנו כיצד נוכל לעזור.

## 🧑‍💻 שילוב LVGL

שילוב LVGL פשוט מאוד. הוסיפו אותו לפרויקט וקומפלו כמו קבצים אחרים.
כדי להגדיר, העתיקו את `lv_conf_template.h` ל־`lv_conf.h`, הפעילו את ה־`#if 0` הראשון והתאימו את ההגדרות לפי הצורך.
(בדרך כלל ברירת המחדל מספקת.) ניתן גם להשתמש ב־LVGL עם Kconfig כשזמין.

לאחר מכן אפשר לאתחל את LVGL וליצור התקני תצוגה וקלט כך:

```c
#include "lvgl/lvgl.h" /*הגדירו LV_LVGL_H_INCLUDE_SIMPLE כדי לכלול כ-"lvgl.h"*/

#define TFT_HOR_RES 320
#define TFT_VER_RES 240

static uint32_t my_tick_cb(void)
{
    return my_get_millisec();
}

static void my_flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    /*כתבו את px_map לאזור המתאים במאגר המסגרת או בבקר התצוגה החיצוני*/
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

    /*אתחול LVGL*/
    lv_init();

    /*הגדרת מקור טיק במילישניות כדי של־LVGL תהיה ספירת זמן*/
    lv_tick_set_cb(my_tick_cb);

    /*יצירת תצוגה להוספת מסכים ווידג'טים*/
    lv_display_t * display = lv_display_create(TFT_HOR_RES, TFT_VER_RES);

    /*הוספת מאגרי רינדור
     *כאן מוסיפים מאגר חלקי קטן בהנחה של 16 סיביות (RGB565)*/
    static uint8_t buf[TFT_HOR_RES * TFT_VER_RES / 10 * 2]; /* x2 בגלל עומק צבע 16 ביט */
    lv_display_set_buffers(display, buf, NULL, sizeof(buf), LV_DISPLAY_RENDER_MODE_PARTIAL);

    /*קולבק לרענון התוכן מן המאגר אל התצוגה*/
    lv_display_set_flush_cb(display, my_flush_cb);

    /*יצירת התקן קלט למגע*/
    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touch_read_cb);

    /*הדרייברים מוכנים, יוצרים UI*/
    lv_obj_t * label = lv_label_create(lv_screen_active());
    lv_label_set_text(label, "Hello world");
    lv_obj_center(label);

    /*לולאת משימות של LVGL*/
    while(1) {
        lv_timer_handler();
        my_sleep_ms(5);         /*השהיה קצרה להורדת עומס המערכת*/
    }
}
```

## 🤖 דוגמאות

מעל 100 דוגמאות זמינות ב־https://docs.lvgl.io/master/examples.html

המציג המקוון כולל גם מדריכים ללימוד XML: ‏https://viewer.lvgl.io/


### כפתור Hello World עם אירוע

<img width="311" height="232" alt="image" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />

<details>
  <summary>קוד C</summary>

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
  <summary>ב־XML עם LVGL Pro</summary>

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

### מחוון מעוצב עם קישור נתונים

<img width="314" height="233" alt="image" src="https://github.com/user-attachments/assets/268db1a0-946c-42e2-aee4-9550bdf5f4f9" />

<details>
  <summary>קוד C</summary>

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
  <summary>ב־XML עם LVGL Pro</summary>

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

### תיבות סימון בפריסה

<img width="311" height="231" alt="image" src="https://github.com/user-attachments/assets/ba9af647-2ea1-4bc8-b53d-c7b43ce24b6e" />

<details>
  <summary>קוד C</summary>

  ```c
/* יצירת מסך חדש וטעינתו */
lv_obj_t * scr = lv_obj_create(NULL);
lv_screen_load(scr);

/* הגדרת פריסת עמודה */
lv_obj_set_flex_flow(scr, LV_FLEX_FLOW_COLUMN);
lv_obj_set_flex_align(scr, LV_FLEX_ALIGN_SPACE_EVENLY, /*יישור אנכי*/
						   LV_FLEX_ALIGN_START,	       /*יישור אופקי במסילה*/
						   LV_FLEX_ALIGN_CENTER);      /*יישור המסילה*/

/* יצירת 5 תיבות סימון */
const char * texts[5] = {"Input 1", "Input 2", "Input 3", "Output 1", "Output 2"};
for(int i = 0; i < 5; i++) {
	lv_obj_t * cb = lv_checkbox_create(scr);
	lv_checkbox_set_text(cb, texts[i]);
}

/* שינוי מצבים */
lv_obj_add_state(lv_obj_get_child(scr, 1), LV_STATE_CHECKED);
lv_obj_add_state(lv_obj_get_child(scr, 3), LV_STATE_DISABLED);
```

</details>

<details>
  <summary>ב־XML עם LVGL Pro</summary>

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


## 🌟 תרומה

LVGL הוא פרויקט פתוח ותרומות מתקבלות בברכה. אפשר לתרום בדרכים רבות: לשתף על הפרויקט שלכם, לכתוב דוגמאות, לשפר את התיעוד, לתקן תקלות ואפילו לארח פרויקט משלכם תחת ארגון LVGL.

לתיאור מפורט של אפשרויות התרומה, בקרו בפרק [Contributing](https://docs.lvgl.io/master/contributing/index.html) בתיעוד.

יותר מ־600 אנשים כבר הותירו חותם על LVGL. הצטרפו אלינו. נתראה שם 🙂

<a href="https://github.com/lvgl/lvgl/graphs/contributors"> <img src="https://contrib.rocks/image?repo=lvgl/lvgl&max=48" /> </a>

...ועוד רבים.

