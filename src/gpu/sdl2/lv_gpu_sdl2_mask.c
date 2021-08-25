#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "lv_gpu_sdl2_mask.h"
#include "draw/lv_draw_mask.h"
#include "misc/lv_mem.h"

SDL_Palette *lv_sdl2_palette_grayscale8 = NULL;

SDL_Surface *lv_sdl2_create_mask_surface(lv_opa_t *pixels, lv_coord_t width, lv_coord_t height, lv_coord_t stride) {
    SDL_Surface *indexed = SDL_CreateRGBSurfaceWithFormatFrom(pixels, width, height, 8,
                                                              stride, SDL_PIXELFORMAT_INDEX8);
    SDL_SetSurfacePalette(indexed, lv_sdl2_palette_grayscale8);
    SDL_Surface *converted = SDL_ConvertSurfaceFormat(indexed, SDL_PIXELFORMAT_ARGB8888, 0);
    SDL_FreeSurface(indexed);
    return converted;
}

SDL_Texture *lv_sdl2_create_mask_texture(SDL_Renderer *renderer, lv_opa_t *pixels, lv_coord_t width,
                                         lv_coord_t height, lv_coord_t stride) {
    SDL_Surface *indexed = lv_sdl2_create_mask_surface(pixels, width, height, stride);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, indexed);
    SDL_FreeSurface(indexed);
    return texture;
}

lv_opa_t *lv_draw_mask_dump(const lv_area_t *coords) {
    SDL_assert(coords->x2 >= coords->x1);
    SDL_assert(coords->y2 >= coords->y1);
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);
    lv_opa_t *mask_buf = lv_mem_buf_get(w * h);
    for (lv_coord_t y = 0; y < h; y++) {
        lv_memset_ff(&mask_buf[y * w], w);
        lv_draw_mask_res_t res = lv_draw_mask_apply(&mask_buf[y * w], (lv_coord_t) coords->x1,
                                                    (lv_coord_t) (y + coords->y1),
                                                    (lv_coord_t) w);
        if (res == LV_DRAW_MASK_RES_TRANSP) {
            lv_memset_00(&mask_buf[y * w], w);
        }
    }
    return mask_buf;
}

SDL_Surface *lv_sdl2_apply_mask_surface(const lv_area_t *coords) {
    lv_coord_t w = lv_area_get_width(coords), h = lv_area_get_height(coords);

    lv_opa_t *mask_buf = lv_draw_mask_dump(coords);
    lv_mem_buf_release(mask_buf);
    return lv_sdl2_create_mask_surface(mask_buf, w, h, w);
}

SDL_Texture *lv_sdl2_gen_mask_texture(SDL_Renderer *renderer, const lv_area_t *coords) {
    SDL_Surface *indexed = lv_sdl2_apply_mask_surface(coords);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, indexed);
    SDL_FreeSurface(indexed);
    return texture;
}


static inline int clamp(int a, int min, int max) {
    return a > min ? (a < max ? a : max) : a;
}

// Based heavily on http://vitiy.info/Code/stackblur.cpp
// See http://vitiy.info/stackblur-algorithm-multi-threaded-blur-for-cpp/
// Stack Blur Algorithm by Mario Klingemann <mario@quasimondo.com>

static unsigned short const stackblur_mul[255] =
        {
                512, 512, 456, 512, 328, 456, 335, 512, 405, 328, 271, 456, 388, 335, 292, 512,
                454, 405, 364, 328, 298, 271, 496, 456, 420, 388, 360, 335, 312, 292, 273, 512,
                482, 454, 428, 405, 383, 364, 345, 328, 312, 298, 284, 271, 259, 496, 475, 456,
                437, 420, 404, 388, 374, 360, 347, 335, 323, 312, 302, 292, 282, 273, 265, 512,
                497, 482, 468, 454, 441, 428, 417, 405, 394, 383, 373, 364, 354, 345, 337, 328,
                320, 312, 305, 298, 291, 284, 278, 271, 265, 259, 507, 496, 485, 475, 465, 456,
                446, 437, 428, 420, 412, 404, 396, 388, 381, 374, 367, 360, 354, 347, 341, 335,
                329, 323, 318, 312, 307, 302, 297, 292, 287, 282, 278, 273, 269, 265, 261, 512,
                505, 497, 489, 482, 475, 468, 461, 454, 447, 441, 435, 428, 422, 417, 411, 405,
                399, 394, 389, 383, 378, 373, 368, 364, 359, 354, 350, 345, 341, 337, 332, 328,
                324, 320, 316, 312, 309, 305, 301, 298, 294, 291, 287, 284, 281, 278, 274, 271,
                268, 265, 262, 259, 257, 507, 501, 496, 491, 485, 480, 475, 470, 465, 460, 456,
                451, 446, 442, 437, 433, 428, 424, 420, 416, 412, 408, 404, 400, 396, 392, 388,
                385, 381, 377, 374, 370, 367, 363, 360, 357, 354, 350, 347, 344, 341, 338, 335,
                332, 329, 326, 323, 320, 318, 315, 312, 310, 307, 304, 302, 299, 297, 294, 292,
                289, 287, 285, 282, 280, 278, 275, 273, 271, 269, 267, 265, 263, 261, 259
        };

static unsigned char const stackblur_shr[255] =
        {
                9, 11, 12, 13, 13, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 17,
                17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 19,
                19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20, 20, 20,
                20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 21,
                21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
                21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22,
                22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
                22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23,
                23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
                23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
                24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
                24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
                24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
                24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24
        };

/// Stackblur algorithm body
void stack_blur_job(lv_opa_t *src, unsigned int w, unsigned int h, unsigned int radius, int cores, int core, int step) {
    SDL_assert(radius >= 2 && radius <= 254);

    unsigned int x, y, xp, yp, i;
    unsigned int sp;
    unsigned int stack_start;
    unsigned char *stack_ptr;

    lv_opa_t *src_ptr;
    lv_opa_t *dst_ptr;

    unsigned long sum_r;
    unsigned long sum_in_r;
    unsigned long sum_out_r;

    unsigned int wm = w - 1;
    unsigned int hm = h - 1;
    unsigned int stride = w;
    unsigned int div = (radius * 2) + 1;
    unsigned int mul_sum = stackblur_mul[radius];
    unsigned char shr_sum = stackblur_shr[radius];
    unsigned char stack[254 * 2 + 1];

    if (step == 1) {
        int minY = core * h / cores;
        int maxY = (core + 1) * h / cores;

        for (y = minY; y < maxY; y++) {
            sum_r =
            sum_in_r =
            sum_out_r = 0;

            src_ptr = src + stride * y; // start of line (0,y)

            for (i = 0; i <= radius; i++) {
                stack_ptr = &stack[i];
                stack_ptr[0] = src_ptr[0];
                sum_r += src_ptr[0] * (i + 1);
                sum_out_r += src_ptr[0];
            }


            for (i = 1; i <= radius; i++) {
                if (i <= wm) src_ptr += 1;
                stack_ptr = &stack[i + radius];
                stack_ptr[0] = src_ptr[0];
                sum_r += src_ptr[0] * (radius + 1 - i);
                sum_in_r += src_ptr[0];
            }


            sp = radius;
            xp = radius;
            if (xp > wm) xp = wm;
            src_ptr = src + (xp + y * w); //   img.pix_ptr(xp, y);
            dst_ptr = src + y * stride; // img.pix_ptr(0, y);
            for (x = 0; x < w; x++) {
                dst_ptr[0] = clamp((sum_r * mul_sum) >> shr_sum, 0, 255);
                dst_ptr += 1;

                sum_r -= sum_out_r;

                stack_start = sp + div - radius;
                if (stack_start >= div) stack_start -= div;
                stack_ptr = &stack[stack_start];

                sum_out_r -= stack_ptr[0];

                if (xp < wm) {
                    src_ptr += 1;
                    ++xp;
                }

                stack_ptr[0] = src_ptr[0];

                sum_in_r += src_ptr[0];
                sum_r += sum_in_r;

                ++sp;
                if (sp >= div) sp = 0;
                stack_ptr = &stack[sp];

                sum_out_r += stack_ptr[0];
                sum_in_r -= stack_ptr[0];
            }

        }
    }

    // step 2
    if (step == 2) {
        int minX = core * w / cores;
        int maxX = (core + 1) * w / cores;

        for (x = minX; x < maxX; x++) {
            sum_r =
            sum_in_r =
            sum_out_r = 0;

            src_ptr = src + x; // x,0
            for (i = 0; i <= radius; i++) {
                stack_ptr = &stack[i];
                stack_ptr[0] = src_ptr[0];
                sum_r += src_ptr[0] * (i + 1);
                sum_out_r += src_ptr[0];
            }
            for (i = 1; i <= radius; i++) {
                if (i <= hm) src_ptr += stride; // +stride

                stack_ptr = &stack[i + radius];
                stack_ptr[0] = src_ptr[0];
                sum_r += src_ptr[0] * (radius + 1 - i);
                sum_in_r += src_ptr[0];
            }

            sp = radius;
            yp = radius;
            if (yp > hm) yp = hm;
            src_ptr = src + (x + yp * w); // img.pix_ptr(x, yp);
            dst_ptr = src + x;               // img.pix_ptr(x, 0);
            for (y = 0; y < h; y++) {
                dst_ptr[0] = clamp((sum_r * mul_sum) >> shr_sum, 0, 255);
                dst_ptr += stride;

                sum_r -= sum_out_r;

                stack_start = sp + div - radius;
                if (stack_start >= div) stack_start -= div;
                stack_ptr = &stack[stack_start];

                sum_out_r -= stack_ptr[0];

                if (yp < hm) {
                    src_ptr += stride; // stride
                    ++yp;
                }

                stack_ptr[0] = src_ptr[0];

                sum_in_r += src_ptr[0];
                sum_r += sum_in_r;

                ++sp;
                if (sp >= div) sp = 0;
                stack_ptr = &stack[sp];

                sum_out_r += stack_ptr[0];
                sum_in_r -= stack_ptr[0];
            }
        }
    }
}

void lv_draw_mask_blur(lv_opa_t *pix, uint16_t w, uint16_t h, uint16_t radius) {
    stack_blur_job(pix, w, h, radius, 1, 0, 1);
    stack_blur_job(pix, w, h, radius, 1, 0, 2);
}

#endif /*LV_USE_GPU_SDL*/