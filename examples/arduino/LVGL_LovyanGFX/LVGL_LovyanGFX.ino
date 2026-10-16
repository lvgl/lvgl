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



/*Set to your screen resolution and rotation*/
#define TFT_HOR_RES   320
#define TFT_VER_RES   240
#define TFT_ROTATION  LV_ROTATION_0

/*LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes*/
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * 2) /* 2 bytes per pixel for RGB565 */
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

#if LV_USE_LOG != 0
void my_print(lv_log_level_t level, const char * buf)
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

    /* Create a simple button with a label
     * ---------------------
     lv_obj_t *button = lv_btn_create(lv_screen_active());
     lv_obj_set_align(button, LV_ALIGN_CENTER);

     lv_obj_t *label = lv_label_create(button);
     lv_label_set_text(label, "Hello Arduino!");

     * Try an example. See all the examples
     *  - Online: https://docs.lvgl.io/master/examples.html
     *  - Source codes: https://github.com/lvgl/lvgl/tree/master/examples
     * ----------------------------------------------------------------

     lv_example_btn_1();

     * Or try out a demo. Don't forget to enable the demos in lv_conf.h. E.g. LV_USE_DEMO_WIDGETS
     * -------------------------------------------------------------------------------------------

     lv_demo_widgets();
     */
    
    lv_obj_t *button = lv_btn_create(lv_screen_active());
    lv_obj_set_align(button, LV_ALIGN_CENTER);

    lv_obj_t *label = lv_label_create(button);
    lv_label_set_text(label, "Hello Arduino!");

    Serial.println("Setup done");
}

void loop()
{
    lv_timer_handler(); /* let the GUI do its work */
    delay(5); /* let this time pass */
}
