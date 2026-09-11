#include <lvgl/lvgl.h>
#include <demos/lv_demos.h>

#define WIDTH 480
#define HEIGHT 320

static uint16_t buffer[WIDTH * HEIGHT];

static void flush_cb(lv_display_t * disp, const lv_area_t * area, uint8_t * px_map)
{
    LV_UNUSED(area);
    LV_UNUSED(px_map);
    lv_display_flush_ready(disp);
}

static void indev_read_cb(lv_indev_t * indev, lv_indev_data_t * data)
{
    LV_UNUSED(indev);
    data->state = LV_INDEV_STATE_RELEASED;
}

int main(void)
{
    lv_init();
    lv_display_t * display = lv_display_create(WIDTH, HEIGHT);
    lv_display_set_buffers(display, buffer, NULL, sizeof(buffer), LV_DISPLAY_RENDER_MODE_FULL);
    lv_display_set_flush_cb(display, flush_cb);

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, indev_read_cb);

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
