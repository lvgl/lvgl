/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/integration/framework/arduino.html  */

#include <lvgl.h>

/*This example uses the LovyanGFX display interface, you need to setup
 *your display driver accordingly. 'my_display.hpp' contains the example configuration file
 *which by default uses LGFX_AUTODETECT.
 *Ensure your 'lv_conf.h' is set as below;
 #define LV_USE_LOVYAN_GFX 1
 #define LV_LGFX_USER_INCLUDE "my_display.hpp"
 */


#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
#include <LittleFS.h>
#endif

#if LV_USE_FS_ARDUINO_SD
#include <SD.h>
#endif

/*Setup your 'lv_conf.h' to enable the file system interfaces.
 *If using LittleFS, enable it and set a drive letter.
 #define LV_USE_FS_ARDUINO_ESP_LITTLEFS 1
 #define LV_FS_ARDUINO_ESP_LITTLEFS_LETTER 67 // 'C'

 *If using an SD card, enable it and set a drive letter.
 #define LV_USE_FS_ARDUINO_SD 1
 #define LV_FS_ARDUINO_SD_LETTER 68 // 'D'

 *You can use both file systems but make sure they have different drive 
 *letter paths. 
 *When using the SD card and display with a shared SPI bus and the LovyanGFX
 *display interface, make sure the panel 'cfg.bus_shared' is set to 'true'.*/


/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES   240
#define TFT_VER_RES   320
#define TFT_ROTATION  LV_ROTATION_0

/* First time using LittleFS might require formatting */
#define LFS_FORMAT_IF_FAILED false /* Set to true to format LittleFS if mounting fails. */

#define SD_CS 4

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * 2) /* 2 bytes per pixel for RGB565 */
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

#if LV_USE_LOG != 0
void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}
#endif


/*use Arduinos millis() as tick source*/
static uint32_t my_tick(void)
{
    return millis();
}

void setup()
{
    String LVGL_Arduino = "Hello Arduino! ";
    LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();

    Serial.begin(115200);
    Serial.println(LVGL_Arduino);

    bool state = false;
#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
    state = LittleFS.begin(LFS_FORMAT_IF_FAILED);
    Serial.println(state ? "LittleFS initialized" : "LittleFS init failed");
#endif
#if LV_USE_FS_ARDUINO_SD
    state = SD.begin(SD_CS);
    Serial.println(state ? "SD initialized" : "SD init failed");
#endif

    lv_init();

    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

    /* register print function for debugging */
#if LV_USE_LOG != 0
    lv_log_register_print_cb(my_print);
#endif

    lv_display_t * disp;
    /*LovyanGFX can be enabled lv_conf.h to initialize the display in a simple way*/
    disp = lv_lovyan_gfx_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf), true);
    lv_display_set_rotation(disp, TFT_ROTATION);

    
    lv_obj_t *cont = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(cont);
    lv_obj_set_height(cont, lv_pct(100));
    lv_obj_set_width(cont, lv_pct(100));
    lv_obj_set_style_layout(cont, LV_LAYOUT_FLEX, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t *label = lv_label_create(cont);
    lv_label_set_text(label, "Arduino FS test!");

    /* Test loading images from FS
     * ---------------------
     * In the sketch data folder, you'll find some sample PNG images.
     * Upload them to the LittleFS file system or copy them to the SD card.
     * Then load the images from the file systems by using the appropriate drive letter path.
     * Note: The PNG decoder must be enabled in 'lv_conf.h' (LV_USE_LODEPNG) to load PNG images. */

    lv_obj_t *img;
#if LV_USE_FS_ARDUINO_ESP_LITTLEFS
    img = lv_image_create(cont);
    lv_image_set_src(img, "C:/littlefs.png");
#endif
#if LV_USE_FS_ARDUINO_SD
    img = lv_image_create(cont);
    lv_image_set_src(img, "D:/sd.png");
#endif

    Serial.println("Setup done");
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
}
