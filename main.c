#include "lvgl.h"
#include "demos/lv_demos.h"
#include <SDL.h>

#define SDL_HOR_RES     1024
#define SDL_VER_RES     768

int main(void)
{
    /* Initialize LVGL core */
    lv_init();

    /* Create SDL2 window and attach as LVGL display */
    lv_display_t * disp = lv_sdl_window_create(SDL_HOR_RES, SDL_VER_RES);
    lv_sdl_window_set_title(disp, "LVGL9 Benchmark - macOS");

    /* Register SDL input devices */
    lv_indev_t * mouse = lv_sdl_mouse_create();
    lv_indev_t * keyboard = lv_sdl_keyboard_create();
    lv_indev_t * mousewheel = lv_sdl_mousewheel_create();

    /* Run the benchmark demo */
    lv_demo_benchmark();

    /* Main loop: drive LVGL timer handler */
    while (1) {
        lv_timer_handler();
        lv_delay_ms(1);
    }

    return 0;
}