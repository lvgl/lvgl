#include <lvgl/lvgl.h>
#include <demos/lv_demos.h>

#define WIDTH 240
#define HEIGHT 240

static uint16_t buffer[(WIDTH * HEIGHT) / 10];

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(disp);
    LV_UNUSED(area);
    LV_UNUSED(px_map);
}
int main(void)
{
    lv_display_t * display = lv_display_create(WIDTH, HEIGHT);
    lv_display_set_buffers(display, buffer, NULL, sizeof(buffer), LV_DISPLAY_RENDER_MODE_PARTIAL);
    lv_display_set_flush_cb(display, flush_cb);
    LV_IMAGE_DECLARE(lvgl_logo);
    lv_obj_t * image = lv_image_create(lv_screen_active());
    lv_image_set_src(image, &lvgl_logo);
    lv_obj_t * button = lv_button_create(lv_screen_active());
    lv_obj_t * label = lv_label_create(button);
    lv_label_set_text(label, "Hello World");
    lv_obj_center(button);
    lv_obj_align(image, LV_ALIGN_TOP_MID, 0, 320 / 4);

    while(1) {
        uint32_t ms = lv_timer_handler();
        lv_sleep_ms(ms);
    }
}

void Reset_Handler(void)
{
    main();
}
