//
// Created by Mariotaku on 2021/08/20.
//

#include "font/lv_font_fmt_txt.h"
#include "core/lv_refr.h"
#include "sdl2/lv_gpu_sdl2_utils.h"
#include "sdl2/lv_gpu_draw_cache.h"
#include "lv_gpu_sdl.h"

static void lv_sdl_drv_wait_cb(lv_disp_drv_t *disp_drv);

static void lv_sdl_drv_fb_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *src);

lv_disp_t *lv_sdl_display_init(SDL_Window *window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    lv_disp_draw_buf_t *draw_buf = malloc(sizeof(lv_disp_draw_buf_t));
    lv_disp_draw_buf_init(draw_buf, renderer, NULL, width * height);
    lv_disp_drv_t *driver = malloc(sizeof(lv_disp_drv_t));
    lv_disp_drv_init(driver);
    driver->draw_buf = draw_buf;
    driver->wait_cb = lv_sdl_drv_wait_cb;
    driver->flush_cb = lv_sdl_drv_fb_flush;
    driver->hor_res = (lv_coord_t) width;
    driver->ver_res = (lv_coord_t) height;

    lv_gpu_draw_cache_init();

    return lv_disp_drv_register(driver);
}

void lv_sdl_display_deinit(lv_disp_t *disp) {
    lv_gpu_draw_cache_deinit();
    SDL_DestroyRenderer((SDL_Renderer *) disp->driver->draw_buf->buf1);
    free(disp->driver->draw_buf);
    free(disp->driver);
}


static void lv_sdl_drv_wait_cb(lv_disp_drv_t *disp_drv) {
    (void) disp_drv;
    //OPTIONAL: Called periodically while lvgl waits for an operation to be completed
    //          User can execute very simple tasks here or yield the task
}

static void lv_sdl_drv_fb_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *src) {
    (void) src;

    if (area->x2 < 0 || area->y2 < 0 ||
        area->x1 > disp_drv->hor_res - 1 || area->y1 > disp_drv->ver_res - 1) {
        lv_disp_flush_ready(disp_drv);
        return;
    }

    if (disp_drv->draw_buf->flushing_last) {
        SDL_Renderer *renderer = (SDL_Renderer *) disp_drv->draw_buf->buf_act;
        SDL_RenderPresent(renderer);
    }
    lv_disp_flush_ready(disp_drv);
}
