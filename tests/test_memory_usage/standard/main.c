#include <lvgl/lvgl.h>
#include <demos/lv_demos.h>

#define WIDTH 480
#define HEIGHT 320

static uint16_t buffer[WIDTH * HEIGHT];

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(disp);
    LV_UNUSED(area);
    LV_UNUSED(px_map);
}
int main(void)
{
    lv_display_t * display = lv_display_create(WIDTH, HEIGHT);
    lv_display_set_buffers(display, buffer, NULL, sizeof(buffer), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(display, flush_cb);
    lv_demo_widgets();
    lv_demo_widgets_start_slideshow();
    while(1) {
        uint32_t ms = lv_timer_handler();
        lv_sleep_ms(ms);
    }
}
void Reset_Handler(void)
{
    main();
}
