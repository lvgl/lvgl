#include "../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../font/lv_font_fmt_txt.h"
#include "../core/lv_refr.h"
#include "sdl/lv_gpu_sdl_utils.h"
#include "sdl/lv_gpu_sdl_draw_cache.h"
#include "lv_gpu_sdl.h"

static void lv_sdl_drv_wait_cb(lv_disp_drv_t *disp_drv);

static void lv_sdl_drv_fb_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *src);

lv_disp_t *lv_sdl_display_init(SDL_Window *window) {
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    lv_disp_draw_buf_t *draw_buf = malloc(sizeof(lv_disp_draw_buf_t));
    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, width,
                                             height);
    lv_disp_draw_buf_init(draw_buf, texture, NULL, width * height);
    lv_disp_drv_t *driver = malloc(sizeof(lv_disp_drv_t));
    lv_disp_drv_init(driver);
    driver->user_data = renderer;
    driver->draw_buf = draw_buf;
    driver->wait_cb = lv_sdl_drv_wait_cb;
    driver->flush_cb = lv_sdl_drv_fb_flush;
    driver->hor_res = (lv_coord_t) width;
    driver->ver_res = (lv_coord_t) height;
    SDL_RendererInfo renderer_info;
    SDL_GetRendererInfo(renderer, &renderer_info);
    SDL_assert(renderer_info.flags & SDL_RENDERER_TARGETTEXTURE);
    SDL_SetRenderTarget(renderer, texture);

    _lv_gpu_sdl_utils_init();
    _lv_gpu_sdl_texture_cache_init();

    return lv_disp_drv_register(driver);
}

void lv_sdl_display_deinit(lv_disp_t *disp) {
    _lv_gpu_sdl_texture_cache_deinit();
    _lv_gpu_sdl_utils_deinit();
    SDL_DestroyTexture((SDL_Texture *) disp->driver->draw_buf->buf1);
    SDL_DestroyRenderer((SDL_Renderer *) disp->driver->user_data);
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

    if (lv_disp_flush_is_last(disp_drv)) {
        SDL_Renderer *renderer = (SDL_Renderer *) disp_drv->user_data;
        SDL_Texture *texture = disp_drv->draw_buf->buf_act;
        SDL_SetRenderTarget(renderer, NULL);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_SetRenderTarget(renderer, texture);
    }
    lv_disp_flush_ready(disp_drv);
}

#endif /*LV_USE_GPU_SDL*/
