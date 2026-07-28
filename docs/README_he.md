<p align="center">
  <a href="https://lvgl.io/docs" title="Documentation">תיעוד</a> •
  <a href="https://forum.lvgl.io" title="Community forum">פורום</a> •
  <a href="https://blog.lvgl.io" title="News and articles">בלוג</a> •
  <a href="https://lvgl.io/services" title="Professional services">שירותים</a>
  <img src="https://upload.wikimedia.org/wikipedia/commons/c/ca/1x1.png" alt="spacer" width="250px" height="1px">
  <a href="../README.md">EN</a> •
  <a href="./README_zh.md">中文</a> •
  <a href="./README_ja.md">日本語</a> •
  <a href="./README_ko.md">한국어</a> •
  <a href="./README_pt_BR.md">PT</a> •
  <b>עברית</b>
</p>

<p align="center">
  <a href="https://lvgl.io"><img src="https://lvgl.io/github-assets/logo-colored.png" height=50px/></a>
  <h1 align="center">ספריית גרפיקה קלה ורב-תכליתית</h1>
</p>

<br/>
<br/>

<div align="center">
  <img src="https://github.com/user-attachments/assets/965e8b8b-d240-45ed-9744-bdd81785967d" height="220" alt="הדגמת אופניים חשמליים עם גרפיקה וקטורית">
  &nbsp;
  <img src="https://github.com/user-attachments/assets/d83820ed-5448-494e-94c8-3ca1b4ddceb0" height="220" alt="הדגמת ECG עם מודל תלת-ממדי מונפש">
</div>

<br/>
<br/>
<br/>

<p align="center">
  <a href="#overview" title="What LVGL is">סקירה כללית</a> •
  <a href="#features" title="What LVGL can do">תכונות</a> •
  <a href="#lvgl-pro" title="The professional toolchain">LVGL Pro</a> •
  <a href="#examples" title="C and XML examples">דוגמאות</a> •
  <a href="#integration" title="How to add LVGL to your project">שילוב</a> •
  <a href="#contributing" title="How to get involved">תרומה</a> •
  <a href="#license" title="Licensing terms">רישיון</a>
</p>

<div dir="rtl">

<a id="overview"></a>
## סקירה כללית

**LVGL** היא ספריית ממשק משתמש חופשית וקוד פתוח המאפשרת לכם ליצור ממשקי משתמש גרפיים
עבור כל MCU ו-MPU מכל יצרן ובכל פלטפורמה.

**דרישות**: ל-LVGL אין תלויות חיצוניות, מה שהופך אותה לקלה להידור עבור כל יעד מודרני,
מ-MCU קטנים ועד MPU מרובי-ליבות מבוססי Linux עם תמיכה בתלת-ממד. עבור ממשק משתמש *טיפוסי*, דרושים רק כ-100kB RAM,
כ-200–300kB flash, ובאפר בגודל של 1/10 מהמסך עבור הרינדור.

**אימוץ נרחב**: יצרני שבבים (כמו NXP, Espressif, Renesas וכן הלאה), פרויקטי RTOS (Zephyr, NuttX וכו'),
ויצרני לוחות (Riverdi, Seeed Studio, VIEWE, Elecrow וכו') כבר שילבו את LVGL. אם ללוח פיתוח יש תצוגה, סביר מאוד
שהיצרן מציע גם תמיכה ב-LVGL.

**כלים מקצועיים**: במקום לכתוב קוד C, תוכלו להאיץ ולפשט מאוד את פיתוח ממשק המשתמש באמצעות [LVGL Pro](#lvgl-pro), ערכת כלים מלאה עם עורך, שילוב עם Figma, מציג מקוון ו-CLI. היא מייצאת קוד C טהור של LVGL מ-XML, ללא סביבת ריצה נוספת או קסם נסתר. היא חופשית לשימוש לא מסחרי ולהערכה.

**כדי להתחיל**, עיינו ב[דוגמאות](#examples), הפעילו [פרויקט סימולטור](https://lvgl.io/docs/open/integration/pc), חקרו את [המציג המקוון](https://viewer.lvgl.io/) של LVGL Pro, הדרו והריצו ממשק משתמש בחלון ב-[LVGL Pro](https://lvgl.io/docs/pro/integration/simulator) בלחיצה אחת, או צללו אל [התיעוד](https://lvgl.io/docs/open) שלנו המוכן ל-AI. פשוט לחצו על [**Ask AI**](https://lvgl.io/docs/open)
ושאלו כל דבר!

<a id="features"></a>
## תכונות

**חופשית, ניידת וניתנת להרחבה**
  - ספריית C ניידת לחלוטין (תואמת C++) ללא תלויות חיצוניות.
  - ניתנת להידור עבור כל MCU או MPU, עם כל מערכת (RT)OS. יש תמיכה ב-Make, ב-CMake וב-globbing פשוט.
  - תומכת בתצוגות מונוכרום, ePaper, OLED או TFT, ואפילו במסכים. [תצוגות](https://lvgl.io/docs/open/main-modules/display)
  - מופצת תחת רישיון MIT, כך שתוכלו להשתמש בה בקלות גם בפרויקטים מסחריים.
  - במינימום המוחלט דרושים רק 32kB RAM ו-128kB Flash, frame buffer, ובאפר בגודל של לפחות 1/10 מהמסך עבור הרינדור.
  - יש תמיכה במערכת הפעלה, בזיכרון חיצוני וב-GPU, אך הם אינם נדרשים.

**רכיבי ממשק, סגנונות, פריסות ועוד**
  - יותר מ-30 [רכיבי ממשק](https://lvgl.io/docs/open/widgets) מובנים: Button, Label, Slider, Chart, Keyboard, Meter, Arc, Table ועוד רבים.
  - [מערכת סגנונות](https://lvgl.io/docs/open/common-widget-features/styles) גמישה עם יותר מ-100 מאפייני סגנון להתאמה אישית של כל חלק ברכיבי הממשק בכל מצב.
  - מנועי פריסה בסגנון [Flexbox](https://lvgl.io/docs/open/common-widget-features/layouts/flex) ו-[Grid](https://lvgl.io/docs/open/common-widget-features/layouts/grid) לקביעת גודל ומיקום אוטומטיים ורספונסיביים של רכיבי הממשק.
  - [קישורי נתונים](https://lvgl.io/docs/open/main-modules/observer) לחיבור קל של ממשק המשתמש עם היישום.
  - תמיכה בעכבר, במשטח מגע, בלוח מקשים, במקלדת, בלחצנים חיצוניים וב-Encoder [התקני קלט](https://lvgl.io/docs/open/main-modules/indev).
  - תמיכה ב[תצוגות מרובות](https://lvgl.io/docs/open/main-modules/display/overview#how-many-displays-can-lvgl-use).

**רינדור**
  - מנוע רינדור דו-ממדי מובנה התומך בצורות בסיסיות, מעברי צבע, החלקת קצוות, שקיפות, גלילה חלקה, צללי תיבה וצללים מוטלים, טרנספורמציה של תמונות וכו'.
  - [מנוע רינדור תלת-ממדי עוצמתי](https://lvgl.io/docs/open/libs/gltf) להצגת [מודלים של glTF](https://sketchfab.com/) עם OpenGL.
  - תמיכה בגרפיקה וקטורית, ב-SVG וב-Lottie.
  - הטקסט מרונדר בקידוד UTF-8, עם תמיכה במערכות הכתיבה CJK, תאית, הינדי, ערבית ופרסית.
  - תמיכה מובנית ב-GPU כמו VG-Lite, Dave2D, NeoChrome, OpenGL וכו'.

<a id="lvgl-pro"></a>
## LVGL Pro

בניית ממשקי משתמש ב-C עובדת היטב, אך היא הופכת לאיטית לאיטרציה וקשה יותר לשמירה על עקביות ככל שהפרויקט גדל.
[LVGL Pro](https://lvgl.io/pro) מאפשרת לכם לבנות רכיבים ומסכים לשימוש חוזר באופן חזותי, לצפות בשינויים
באופן מיידי, ולנהל קישורי נתונים, תרגומים, אנימציות ובדיקות במקום אחד.

Pro מייצאת קוד C רגיל של LVGL: אותה LVGL שאתם כבר משתמשים בה, ללא סביבת ריצה או תלות נוספת. היא משתלבת
בפרויקט קיים מבלי לשנות את אופן ההידור או ההפצה שלכם.

<p align="center">
  <img height="500" alt="בניית ממשק משתמש בעורך LVGL Pro עם תצוגה מקדימה חיה" src="https://github.com/user-attachments/assets/8cef0f05-0ff1-4766-8dfd-1d15e47f181a" />
</p>

תוכלו לנסות אותה בדפדפן בכתובת [viewer.lvgl.io](https://viewer.lvgl.io) ללא צורך בהתקנה, או
[להוריד את העורך](https://lvgl.io/pro#download) ולהשתמש בו תחת רישיון Community החופשי.

LVGL Pro מורכבת מארבעה כלים הקשורים זה לזה באופן הדוק:

1. **העורך**: לב ליבה של LVGL Pro. יישום שולחני לבניית רכיבים ומסכים ב-XML, לניהול קישורי נתונים, תרגומים, אנימציות, בדיקות ועוד. למדו עוד על [פורמט ה-XML](https://lvgl.io/docs/pro/syntax) ועל [רכיבי הממשק](https://lvgl.io/docs/pro/built_in_widgets).
2. **המציג המקוון**: הפעילו את העורך בדפדפן שלכם, פתחו פרויקטים מ-GitHub ושתפו בקלות ללא הקמת סביבת פיתוח. בקרו בכתובת [https://viewer.lvgl.io](https://viewer.lvgl.io).
3. **תוסף Figma**: העבירו עיצוב מ-Figma ל-LVGL Pro בלחיצה אחת. ראו כיצד זה עובד [כאן](https://lvgl.io/docs/pro/figma).
4. **כלי CLI**: צרו קוד C והריצו בדיקות ב-CI/CD. ראו את הפרטים [כאן](https://lvgl.io/docs/pro/cli).

שכבות ה-Community וה-Evaluation חופשיות לשימוש; ראו [רישוי](#license) לשימוש מסחרי.

<a id="examples"></a>
## דוגמאות

תוכלו לעיין ביותר מ-100 דוגמאות C ו-XML בכתובת https://lvgl.io/docs/open/examples

[המציג המקוון](https://viewer.lvgl.io/) של LVGL Pro מכיל גם מדריכים רבים ואת אותן הדוגמאות שתוכלו לשחק בהן באופן אינטראקטיבי.

כטעימה, הנה אותו ממשק משתמש פשוט כתוב ב-C וב-XML:

<p align="center">
  <img width="311" height="232" alt="לחצן ממורכז שמדפיס בעת לחיצה" src="https://github.com/user-attachments/assets/5948b485-e3f7-4a63-bb21-984381417c4a" />
</p>

</div>

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

<div dir="rtl">

ה-XML שלמעלה הוא מה ש-[LVGL Pro](#lvgl-pro) עובדת איתו: אתם בונים את המסך באופן חזותי והיא מייצרת עבורכם את קוד ה-C.

<a id="integration"></a>
## שילוב

ל-LVGL אין תלויות חיצוניות, כך שניתן להדר אותה בקלות עבור כל התקן. היא מגיעה עם מנהלי התקנים מובנים, זמינה
במנהלי חבילות רבים וב-RTOS רבים, וגם קלה להסבה לכל התקן חדש.

### משולבת מראש

- **יצרני שבבים**: [ESP32](https://components.espressif.com/components/lvgl/lvgl), [NXP MCUXpresso component](https://www.nxp.com/design/software/embedded-software/lvgl-open-source-graphics-library:LITTLEVGL-OPEN-SOURCE-GRAPHICS-LIBRARY), [Renesas FSP](https://lvgl.io/docs/open/integration/chip/renesas), [STM32](https://lvgl.io/docs/open/integration/chip/stm32)

- **מערכות RTOS**: [Zephyr](https://lvgl.io/docs/open/integration/os/zephyr), [NuttX](https://lvgl.io/docs/open/integration/os/nuttx), [RT-Thread](https://lvgl.io/docs/open/integration/os/rt-thread)

- **מסגרות עבודה**: [Arduino](https://lvgl.io/docs/open/integration/framework/arduino), [PlatformIO](https://registry.platformio.org/libraries/lvgl/lvgl), [CMSIS-Pack](https://lvgl.io/docs/open/integration/framework/cmsis-pack)

- **יצרני לוחות**: [Seeed Studio](https://www.seeedstudio.com), [Elecrow](https://www.elecrow.com/display/esp-hmi-display.html), [Riverdi](https://lvgl.io/docs/open/integration/boards/manufacturers/riverdi), [VIEWE](https://lvgl.io/docs/open/integration/boards/manufacturers/viewe), ו[עוד רבים](https://lvgl.io/boards)

### מנהלי התקנים מובנים
LVGL מגיעה עם מנהלי התקנים מוכנים לשימוש, כך שבפלטפורמות נפוצות אינכם צריכים לכתוב בעצמכם את הטיפול בתצוגה ובקלט:

- **סימולטור / שולחני**: חלונות [SDL](https://lvgl.io/docs/open/integration/pc/sdl), X11 ו-[Wayland](https://lvgl.io/docs/open/integration/embedded_linux/drivers/wayland) לפיתוח ולתצוגה מקדימה של ממשק המשתמש שלכם ב-PC.
- **בקרי תצוגה**: מסכי LCD גנריים מסוג MIPI-DBI/SPI ([ILI9341](https://lvgl.io/docs/open/integration/external_display_controllers/ili9341), ST7789 ודומים), בתוספת בקרים של יצרנים כמו [LTDC](https://lvgl.io/docs/open/integration/chip_vendors/stm32/ltdc) של ST ו-[eLCDIF](https://lvgl.io/docs/open/integration/chip_vendors/nxp/elcdif) של NXP.
- **Linux מוטמע**: framebuffer (fbdev), DRM/KMS, Wayland, ו-`libinput`/`evdev` עבור קלט מגע ומצביע. למדו עוד [כאן](https://lvgl.io/docs/open/integration/embedded_linux).
- **GPU / מאיצים**: VG-Lite, NXP PXP, ThinkSilicon NemaGFX, Arm-2D ו-[OpenGL ES](https://lvgl.io/docs/open/integration/embedded_linux/drivers/opengl_driver)

ראו את הרשימה המלאה ואת מדריכי ההגדרה ב[תיעוד השילוב](https://lvgl.io/docs/open/integration).

### ב-LVGL Pro

ב-LVGL Pro תוכלו ליצור פרויקטים מוכנים לשימוש של ממשק משתמש בלבד, VSCode, Zephyr ו-Linux בלחיצה אחת.
<img width="600" alt="image" src="https://github.com/user-attachments/assets/5aadb850-6b40-49d1-ba96-2296041c7e27" />

### הסבה ידנית

שילוב LVGL הוא פשוט מאוד. פשוט הוסיפו אותה לכל פרויקט והדרו אותה כפי שהייתם מהדרים קבצים אחרים.
כדי להגדיר את LVGL, העתיקו את `lv_conf_template.h` בשם `lv_conf.h`, הפעילו את `#if 0` הראשון, והתאימו את ההגדרות לפי הצורך.
(ההגדרה שמוגדרת כברירת מחדל מתאימה בדרך כלל.) אם זמין, ניתן להשתמש ב-LVGL גם עם Kconfig.

לאחר שהיא נמצאת בפרויקט, תוכלו לאתחל את LVGL וליצור התקני תצוגה וקלט באופן הבא:

</div>

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


<div dir="rtl">

<a id="contributing"></a>
## תרומה

LVGL הוא פרויקט פתוח, ותרומות מתקבלות בברכה רבה. יש דרכים רבות לתרום, החל מסתם דיבור על הפרויקט שלכם, כתיבת דוגמאות, שיפור התיעוד, תיקון באגים, ואפילו אירוח הפרויקט שלכם תחת ארגון LVGL.

לתיאור מפורט של הזדמנויות התרומה, בקרו בסעיף [תרומה](https://lvgl.io/docs/open/contributing)
בתיעוד.

מאות אנשים כבר השאירו את טביעת האצבע שלהם ב-LVGL. היו אחד מהם! נתראה כאן! 🙂


<a id="license"></a>
## רישיון

ספריית LVGL מופצת תחת **רישיון MIT**, כך שתוכלו להשתמש בה בחופשיות גם במוצרים
בקוד פתוח וגם במוצרים מסחריים ללא תמלוגים. ראו [`LICENCE.txt`](../LICENCE.txt).

כל ספריות הצד השלישי המצורפות ל-LVGL משוחררות אף הן תחת רישיונות תואמי MIT,
כך שתוכלו להשתמש ב-LVGL ובתלויות שלה בביטחון מלא.

**[LVGL Pro](https://lvgl.io/pro)** מגיעה עם רישוי נפרד:
- שכבות ה-**Community** וה-**Evaluation** הן **חופשיות לשימוש לא מסחרי**, מושלמות עבור
  למידה, פרויקטי תחביב והערכת הכלים.
- שימוש מסחרי ב-LVGL Pro מחייב רישיון בתשלום. ראו את [התמחור והפרטים](https://lvgl.io/pro#pricing).

</div>
