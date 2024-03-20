/**
 * @file lv_draw_sdl_rect.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../../lv_conf_internal.h"

#if LV_USE_GPU_SDL

#include "../lv_draw_rect.h"
#include "../lv_draw_img.h"
#include "../lv_draw_label.h"
#include "../lv_draw_mask.h"
#include "../../core/lv_refr.h"
#include "lv_draw_sdl_utils.h"
#include "lv_draw_sdl_texture_cache.h"
#include "lv_draw_sdl_composite.h"
#include "lv_draw_sdl_mask.h"
#include "lv_draw_sdl_stack_blur.h"
#include "lv_draw_sdl_layer.h"

/*********************
 *      DEFINES
 *********************/

#define FRAG_SPACING 3

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_coord_t radius;
    lv_coord_t size;
} lv_draw_rect_bg_key_t;

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_gradient_stop_t stops[LV_GRADIENT_MAX_STOPS];
    uint8_t stops_count;
    lv_grad_dir_t dir;
} lv_draw_rect_grad_strip_key_t;

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_gradient_stop_t stops[LV_GRADIENT_MAX_STOPS];
    uint8_t stops_count;
    lv_grad_dir_t dir;
    lv_coord_t w;
    lv_coord_t h;
    lv_coord_t radius;
} lv_draw_rect_grad_frag_key_t;

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_coord_t radius;
    lv_coord_t size;
    lv_coord_t blur;
} lv_draw_rect_shadow_key_t;

typedef struct {
    lv_sdl_cache_key_magic_t magic;
    lv_coord_t rout, rin;
    lv_area_t offsets;
} lv_draw_rect_border_key_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void draw_bg_color(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                          const lv_draw_rect_dsc_t * dsc);

static void draw_bg_grad_simple(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                                const lv_grad_dsc_t * grad, bool blend_mod);

static void draw_bg_grad_radius(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                                const lv_draw_rect_dsc_t * dsc);

static void draw_bg_img(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                        const lv_draw_rect_dsc_t * dsc);

static void draw_border(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                        const lv_draw_rect_dsc_t * dsc);

static void draw_shadow(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * clip,
                        const lv_draw_rect_dsc_t * dsc);

static void draw_outline(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * clip,
                         const lv_draw_rect_dsc_t * dsc);

static void draw_border_generic(lv_draw_sdl_ctx_t * ctx, const lv_area_t * outer_area, const lv_area_t * inner_area,
                                const lv_area_t * clip, lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa,
                                lv_blend_mode_t blend_mode);

static void frag_render_borders(SDL_Renderer * renderer, SDL_Texture * frag, lv_coord_t frag_size,
                                const lv_area_t * coords, const lv_area_t * clipped, bool full);

static void frag_render_center(SDL_Renderer * renderer, SDL_Texture * frag, lv_coord_t frag_size,
                               const lv_area_t * coords, const lv_area_t * clipped, bool full);

static lv_draw_rect_bg_key_t rect_bg_key_create(lv_coord_t radius, lv_coord_t size);

static lv_draw_rect_grad_frag_key_t rect_grad_frag_key_create(const lv_grad_dsc_t * grad, lv_coord_t w, lv_coord_t h,
                                                              lv_coord_t radius);

static lv_draw_rect_grad_strip_key_t rect_grad_strip_key_create(const lv_grad_dsc_t * grad);

static lv_draw_rect_shadow_key_t rect_shadow_key_create(lv_coord_t radius, lv_coord_t size, lv_coord_t blur);

static lv_draw_rect_border_key_t rect_border_key_create(lv_coord_t rout, lv_coord_t rin, const lv_area_t * outer_area,
                                                        const lv_area_t * inner_area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/
#define SKIP_BORDER(dsc) ((dsc)->border_opa <= LV_OPA_MIN || (dsc)->border_width == 0 || (dsc)->border_side == LV_BORDER_SIDE_NONE || (dsc)->border_post)
#define SKIP_SHADOW(dsc) ((dsc)->shadow_width == 0 || (dsc)->shadow_opa <= LV_OPA_MIN || ((dsc)->shadow_width == 1 && (dsc)->shadow_spread <= 0 && (dsc)->shadow_ofs_x == 0 && (dsc)->shadow_ofs_y == 0))
#define SKIP_IMAGE(dsc) ((dsc)->bg_img_src == NULL || (dsc)->bg_img_opa <= LV_OPA_MIN)
#define SKIP_OUTLINE(dsc) ((dsc)->outline_opa <= LV_OPA_MIN || (dsc)->outline_width == 0)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_sdl_draw_rect(lv_draw_ctx_t * draw_ctx, const lv_draw_rect_dsc_t * dsc, const lv_area_t * coords)
{
    const lv_area_t * clip = draw_ctx->clip_area;
    lv_draw_sdl_ctx_t * ctx = (lv_draw_sdl_ctx_t *) draw_ctx;

    lv_area_t extension = {0, 0, 0, 0};
    if(!SKIP_SHADOW(dsc)) {
        lv_coord_t ext = (lv_coord_t)(dsc->shadow_spread - dsc->shadow_width / 2 + 1);
        extension.x1 = LV_MAX(extension.x1, -dsc->shadow_ofs_x + ext);
        extension.x2 = LV_MAX(extension.x2, dsc->shadow_ofs_x + ext);
        extension.y1 = LV_MAX(extension.y1, -dsc->shadow_ofs_y + ext);
        extension.y2 = LV_MAX(extension.y2, dsc->shadow_ofs_y + ext);
    }
    if(!SKIP_OUTLINE(dsc)) {
        lv_coord_t ext = (lv_coord_t)(dsc->outline_pad - 1 + dsc->outline_width);
        extension.x1 = LV_MAX(extension.x1, ext);
        extension.x2 = LV_MAX(extension.x2, ext);
        extension.y1 = LV_MAX(extension.y1, ext);
        extension.y2 = LV_MAX(extension.y2, ext);
    }
    /* Coords will be translated so coords will start at (0,0) */
    lv_area_t t_coords = *coords, t_clip = *clip, apply_area, t_area;
    bool has_composite = lv_draw_sdl_composite_begin(ctx, coords, clip, &extension, dsc->blend_mode, &t_coords, &t_clip,
                                                     &apply_area);

    lv_draw_sdl_transform_areas_offset(ctx, has_composite, &apply_area, &t_coords, &t_clip);

    bool has_content = _lv_area_intersect(&t_area, &t_coords, &t_clip);

    SDL_Rect clip_rect;
    lv_area_to_sdl_rect(&t_clip, &clip_rect);
    draw_shadow(ctx, &t_coords, &t_clip, dsc);
    /* Shadows and outlines will also draw in extended area */
    if(has_content) {
        draw_bg_color(ctx, &t_coords, &t_area, dsc);
        draw_bg_img(ctx, &t_coords, &t_area, dsc);
        draw_border(ctx, &t_coords, &t_area, dsc);
    }
    draw_outline(ctx, &t_coords, &t_clip, dsc);

    lv_draw_sdl_composite_end(ctx, &apply_area, dsc->blend_mode);
}

SDL_Texture * lv_draw_sdl_rect_bg_frag_obtain(lv_draw_sdl_ctx_t * ctx, lv_coord_t radius, bool * in_cache)
{
    lv_draw_rect_bg_key_t key = rect_bg_key_create(radius, radius);
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(ctx, &key, sizeof(key), NULL);
    if(texture == NULL) {
        lv_area_t coords = {0, 0, radius * 2 - 1, radius * 2 - 1};
        lv_area_t coords_frag = {0, 0, radius - 1, radius - 1};
        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &coords, radius, false);
        int16_t mask_id = lv_draw_mask_add(&mask_rout_param, NULL);
        texture = lv_draw_sdl_mask_dump_texture(ctx->renderer, &coords_frag, &mask_id, 1);
        SDL_assert(texture != NULL);
        lv_draw_mask_remove_id(mask_id);
        *in_cache = lv_draw_sdl_texture_cache_put(ctx, &key, sizeof(key), texture);
    }
    else {
        *in_cache = true;
    }
    return texture;
}

SDL_Texture * lv_draw_sdl_rect_grad_frag_obtain(lv_draw_sdl_ctx_t * ctx, const lv_grad_dsc_t * grad, lv_coord_t w,
                                                lv_coord_t h, lv_coord_t radius, bool * in_cache)
{
    lv_draw_rect_grad_frag_key_t key = rect_grad_frag_key_create(grad, w, h, radius);
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(ctx, &key, sizeof(key), NULL);
    if(texture == NULL) {
        lv_area_t coords = {0, 0, radius * 2 + FRAG_SPACING - 1, radius * 2 + FRAG_SPACING - 1};
        texture = SDL_CreateTexture(ctx->renderer, LV_DRAW_SDL_TEXTURE_FORMAT, SDL_TEXTUREACCESS_TARGET,
                                    lv_area_get_width(&coords), lv_area_get_height(&coords));
        SDL_assert(texture != NULL);

        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &coords, radius, false);
        int16_t mask_id = lv_draw_mask_add(&mask_rout_param, NULL);
        SDL_Texture * mask = lv_draw_sdl_mask_dump_texture(ctx->renderer, &coords, &mask_id, 1);
        SDL_assert(mask != NULL);
        SDL_SetTextureBlendMode(mask, SDL_BLENDMODE_NONE);
        lv_draw_mask_remove_id(mask_id);

        SDL_Texture * target_backup = SDL_GetRenderTarget(ctx->renderer);
        SDL_SetRenderTarget(ctx->renderer, texture);
        SDL_RenderCopy(ctx->renderer, mask, NULL, NULL);
        SDL_DestroyTexture(mask);

        lv_area_t blend_coords = {.x1 = 0, .y1 = 0, .x2 = w - 1, .y2 = h - 1};
        lv_area_t draw_area = {.x1 = 0, .y1 = 0, .x2 = radius - 1, .y2 = radius - 1};
        /* Align to top left */
        lv_area_align(&coords, &draw_area, LV_ALIGN_TOP_LEFT, 0, 0);
        lv_area_align(&coords, &blend_coords, LV_ALIGN_TOP_LEFT, 0, 0);
        draw_bg_grad_simple(ctx, &blend_coords, &draw_area, grad, true);

        /* Align to top right */
        lv_area_align(&coords, &draw_area, LV_ALIGN_TOP_RIGHT, 0, 0);
        lv_area_align(&coords, &blend_coords, LV_ALIGN_TOP_RIGHT, 0, 0);
        draw_bg_grad_simple(ctx, &blend_coords, &draw_area, grad, true);

        /* Align to bottom right */
        lv_area_align(&coords, &draw_area, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        lv_area_align(&coords, &blend_coords, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
        draw_bg_grad_simple(ctx, &blend_coords, &draw_area, grad, true);

        /* Align to bottom left */
        lv_area_align(&coords, &draw_area, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        lv_area_align(&coords, &blend_coords, LV_ALIGN_BOTTOM_LEFT, 0, 0);
        draw_bg_grad_simple(ctx, &blend_coords, &draw_area, grad, true);

        SDL_SetRenderTarget(ctx->renderer, target_backup);
        *in_cache = lv_draw_sdl_texture_cache_put(ctx, &key, sizeof(key), texture);
    }
    else {
        *in_cache = true;
    }
    return texture;
}

SDL_Texture * lv_draw_sdl_rect_grad_strip_obtain(lv_draw_sdl_ctx_t * ctx, const lv_grad_dsc_t * grad, bool * in_cache)
{
    lv_draw_rect_grad_strip_key_t key = rect_grad_strip_key_create(grad);
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(ctx, &key, sizeof(key), NULL);
    if(texture == NULL) {
        Uint32 amask = 0xFF000000;
        Uint32 rmask = 0x00FF0000;
        Uint32 gmask = 0x0000FF00;
        Uint32 bmask = 0x000000FF;
        lv_color_t pixels[256];
        for(int i = 0; i < 256; i++) {
            pixels[i] = lv_gradient_calculate(grad, 256, i);
        }
        int width = grad->dir == LV_GRAD_DIR_VER ? 1 : 256;
        int height = grad->dir == LV_GRAD_DIR_VER ? 256 : 1;
        SDL_Surface * surface = SDL_CreateRGBSurfaceFrom(pixels, width, height, LV_COLOR_DEPTH, width * LV_COLOR_DEPTH / 8,
                                                         rmask, gmask, bmask, amask);
        texture = SDL_CreateTextureFromSurface(ctx->renderer, surface);
        SDL_assert(texture != NULL);
        SDL_FreeSurface(surface);
        *in_cache = lv_draw_sdl_texture_cache_put(ctx, &key, sizeof(key), texture);
    }
    else {
        *in_cache = true;
    }
    return texture;
}

void lv_draw_sdl_rect_bg_frag_draw_corners(lv_draw_sdl_ctx_t * ctx, SDL_Texture * frag, lv_coord_t frag_size,
                                           const lv_area_t * coords, const lv_area_t * clip, bool full)
{
    if(!clip) clip = coords;
    lv_area_t corner_area, dst_area;
    /* Upper left */
    corner_area.x1 = coords->x1;
    corner_area.y1 = coords->y1;
    corner_area.x2 = coords->x1 + frag_size - 1;
    corner_area.y2 = coords->y1 + frag_size - 1;
    if(_lv_area_intersect(&dst_area, &corner_area, clip)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t dw = lv_area_get_width(&dst_area), dh = lv_area_get_height(&dst_area);
        lv_coord_t sx = (lv_coord_t)(dst_area.x1 - corner_area.x1), sy = (lv_coord_t)(dst_area.y1 - corner_area.y1);
        SDL_Rect src_rect = {sx, sy, dw, dh};
        SDL_RenderCopy(ctx->renderer, frag, &src_rect, &dst_rect);
    }
    /* Upper right, clip right edge if too big */
    corner_area.x1 = LV_MAX(coords->x2 - frag_size + 1, coords->x1 + frag_size);
    corner_area.x2 = coords->x2;
    if(_lv_area_intersect(&dst_area, &corner_area, clip)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t dw = lv_area_get_width(&dst_area), dh = lv_area_get_height(&dst_area);
        if(full) {
            lv_coord_t sx = (lv_coord_t)(dst_area.x1 - corner_area.x1),
                       sy = (lv_coord_t)(dst_area.y1 - corner_area.y1);
            SDL_Rect src_rect = {frag_size + FRAG_SPACING + sx, sy, dw, dh};
            SDL_RenderCopy(ctx->renderer, frag, &src_rect, &dst_rect);
        }
        else {
            SDL_Rect src_rect = {corner_area.x2 - dst_area.x2, dst_area.y1 - corner_area.y1, dw, dh};
            SDL_RenderCopyEx(ctx->renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
        }
    }
    /* Lower right, clip bottom edge if too big */
    corner_area.y1 = LV_MAX(coords->y2 - frag_size + 1, coords->y1 + frag_size);
    corner_area.y2 = coords->y2;
    if(_lv_area_intersect(&dst_area, &corner_area, clip)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t dw = lv_area_get_width(&dst_area), dh = lv_area_get_height(&dst_area);
        if(full) {
            lv_coord_t sx = (lv_coord_t)(dst_area.x1 - corner_area.x1),
                       sy = (lv_coord_t)(dst_area.y1 - corner_area.y1);
            SDL_Rect src_rect = {frag_size + FRAG_SPACING + sx, frag_size + FRAG_SPACING + sy, dw, dh};
            SDL_RenderCopy(ctx->renderer, frag, &src_rect, &dst_rect);
        }
        else {
            SDL_Rect src_rect = {corner_area.x2 - dst_area.x2, corner_area.y2 - dst_area.y2, dw, dh};
            SDL_RenderCopyEx(ctx->renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_HORIZONTAL | SDL_FLIP_VERTICAL);
        }
    }
    /* Lower left, right edge should not be clipped */
    corner_area.x1 = coords->x1;
    corner_area.x2 = coords->x1 + frag_size - 1;
    if(_lv_area_intersect(&dst_area, &corner_area, clip)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t dw = lv_area_get_width(&dst_area), dh = lv_area_get_height(&dst_area);
        if(full) {
            lv_coord_t sx = (lv_coord_t)(dst_area.x1 - corner_area.x1),
                       sy = (lv_coord_t)(dst_area.y1 - corner_area.y1);
            SDL_Rect src_rect = {sx, frag_size + FRAG_SPACING + sy, dw, dh};
            SDL_RenderCopy(ctx->renderer, frag, &src_rect, &dst_rect);
        }
        else {
            SDL_Rect src_rect = {dst_area.x1 - corner_area.x1, corner_area.y2 - dst_area.y2, dw, dh};
            SDL_RenderCopyEx(ctx->renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_VERTICAL);
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_bg_color(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                          const lv_draw_rect_dsc_t * dsc)
{
    if(dsc->bg_opa == 0) {
        return;
    }
    lv_coord_t radius = dsc->radius;
    SDL_Color bg_color;
    if(dsc->bg_grad.dir == LV_GRAD_DIR_NONE) {
        lv_color_to_sdl_color(&dsc->bg_color, &bg_color);
    }
    else if(dsc->bg_grad.stops_count == 1) {
        lv_color_to_sdl_color(&dsc->bg_grad.stops[0].color, &bg_color);
    }
    else {
        if(radius <= 0) {
            draw_bg_grad_simple(ctx, coords, draw_area, &dsc->bg_grad, false);
        }
        else {
            draw_bg_grad_radius(ctx, coords, draw_area, dsc);
        }
        return;
    }
    if(radius <= 0) {
        SDL_Rect rect;
        lv_area_to_sdl_rect(draw_area, &rect);
        SDL_SetRenderDrawColor(ctx->renderer, bg_color.r, bg_color.g, bg_color.b, dsc->bg_opa);
        SDL_SetRenderDrawBlendMode(ctx->renderer, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(ctx->renderer, &rect);
        return;
    }

    /*A small texture with a quarter of the rect is enough*/
    lv_coord_t bg_w = lv_area_get_width(coords), bg_h = lv_area_get_height(coords);
    lv_coord_t real_radius = LV_MIN3(bg_w / 2, bg_h / 2, radius);
    bool texture_in_cache = false;
    SDL_Texture * texture = lv_draw_sdl_rect_bg_frag_obtain(ctx, real_radius, &texture_in_cache);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, dsc->bg_opa);
    SDL_SetTextureColorMod(texture, bg_color.r, bg_color.g, bg_color.b);
    lv_draw_sdl_rect_bg_frag_draw_corners(ctx, texture, real_radius, coords, draw_area, false);
    frag_render_borders(ctx->renderer, texture, real_radius, coords, draw_area, false);
    frag_render_center(ctx->renderer, texture, real_radius, coords, draw_area, false);

    if(!texture_in_cache) {
        LV_LOG_WARN("Texture is not cached, this will impact performance.");
        SDL_DestroyTexture(texture);
    }
}

static void draw_bg_grad_simple(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                                const lv_grad_dsc_t * grad, bool blend_mod)
{
    SDL_Rect dstrect;
    lv_area_to_sdl_rect(draw_area, &dstrect);
    SDL_Rect srcrect;
    if(grad->dir == LV_GRAD_DIR_VER) {
        lv_coord_t coords_h = lv_area_get_height(coords);
        srcrect.x = 0;
        srcrect.y = (draw_area->y1 - coords->y1) * 255 / coords_h;
        srcrect.w = 1;
        srcrect.h = dstrect.h * 256 / coords_h;

        if(srcrect.y < 0 || srcrect.y > 255) {
            return;
        }
    }
    else {
        lv_coord_t coords_w = lv_area_get_width(coords);
        srcrect.x = (draw_area->x1 - coords->x1) * 255 / coords_w;
        srcrect.y = 0;
        srcrect.w = dstrect.w * 256 / coords_w;
        srcrect.h = 1;

        if(srcrect.x < 0 || srcrect.x > 255) {
            return;
        }
    }

    bool grad_texture_in_cache = false;
    SDL_Texture * grad_texture = lv_draw_sdl_rect_grad_strip_obtain(ctx, grad, &grad_texture_in_cache);
    if(blend_mod) {
        SDL_SetTextureBlendMode(grad_texture, SDL_BLENDMODE_MOD);
    }
    else {
        SDL_SetTextureBlendMode(grad_texture, SDL_BLENDMODE_BLEND);
    }

    SDL_RenderCopy(ctx->renderer, grad_texture, &srcrect, &dstrect);

    if(!grad_texture_in_cache) {
        LV_LOG_WARN("Texture is not cached, this will impact performance.");
        SDL_DestroyTexture(grad_texture);
    }
}

static void draw_bg_grad_radius(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                                const lv_draw_rect_dsc_t * dsc)
{
    lv_coord_t radius = dsc->radius;
    /*A small texture with a quarter of the rect is enough*/
    lv_coord_t bg_w = lv_area_get_width(coords), bg_h = lv_area_get_height(coords);
    lv_coord_t real_radius = LV_MIN3(bg_w / 2, bg_h / 2, radius);
    bool grad_texture_in_cache = false;
    SDL_Texture * grad_texture = lv_draw_sdl_rect_grad_frag_obtain(ctx, &dsc->bg_grad, bg_w, bg_h, radius,
                                                                   &grad_texture_in_cache);
    SDL_SetTextureBlendMode(grad_texture, SDL_BLENDMODE_BLEND);

    lv_draw_sdl_rect_bg_frag_draw_corners(ctx, grad_texture, real_radius, coords, draw_area, true);
    lv_area_t part_coords;
    lv_area_t part_area;
    if(bg_w > radius * 2) {
        /*Draw left, middle, right*/
        part_coords.x1 = 0;
        part_coords.x2 = radius - 1;
        part_coords.y1 = radius;
        part_coords.y2 = bg_h - radius - 1;

        lv_area_align(coords, &part_coords, LV_ALIGN_LEFT_MID, 0, 0);
        _lv_area_intersect(&part_area, &part_coords, draw_area);
        draw_bg_grad_simple(ctx, coords, &part_area, &dsc->bg_grad, false);

        lv_area_align(coords, &part_coords, LV_ALIGN_RIGHT_MID, 0, 0);
        _lv_area_intersect(&part_area, &part_coords, draw_area);
        draw_bg_grad_simple(ctx, coords, &part_area, &dsc->bg_grad, false);

        part_coords.x1 = radius;
        part_coords.x2 = bg_w - radius - 1;
        part_coords.y1 = 0;
        part_coords.y2 = bg_h - 1;
        lv_area_align(coords, &part_coords, LV_ALIGN_CENTER, 0, 0);
        _lv_area_intersect(&part_area, &part_coords, draw_area);
        draw_bg_grad_simple(ctx, coords, &part_area, &dsc->bg_grad, false);
    }
    else if(bg_h > radius * 2) {
        /*Draw top, middle, bottom*/
        part_coords.x1 = radius;
        part_coords.x2 = bg_w - radius - 1;
        part_coords.y1 = 0;
        part_coords.y2 = radius - 1;

        lv_area_align(coords, &part_coords, LV_ALIGN_TOP_MID, 0, 0);
        _lv_area_intersect(&part_area, &part_coords, draw_area);
        draw_bg_grad_simple(ctx, coords, &part_area, &dsc->bg_grad, false);

        lv_area_align(coords, &part_coords, LV_ALIGN_BOTTOM_MID, 0, 0);
        _lv_area_intersect(&part_area, &part_coords, draw_area);
        draw_bg_grad_simple(ctx, coords, &part_area, &dsc->bg_grad, false);

        part_coords.x1 = 0;
        part_coords.x2 = bg_w - 1;
        part_coords.y1 = radius;
        part_coords.y2 = bg_h - radius - 1;
        lv_area_align(coords, &part_coords, LV_ALIGN_CENTER, 0, 0);
        _lv_area_intersect(&part_area, &part_coords, draw_area);
        draw_bg_grad_simple(ctx, coords, &part_area, &dsc->bg_grad, false);
    }

    if(!grad_texture_in_cache) {
        LV_LOG_WARN("Texture is not cached, this will impact performance.");
        SDL_DestroyTexture(grad_texture);
    }
}

static void draw_bg_img(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                        const lv_draw_rect_dsc_t * dsc)
{
    LV_UNUSED(draw_area);
    if(SKIP_IMAGE(dsc)) return;

    lv_img_src_t src_type = lv_img_src_get_type(dsc->bg_img_src);
    if(src_type == LV_IMG_SRC_SYMBOL) {
        lv_point_t size;
        lv_txt_get_size(&size, dsc->bg_img_src, dsc->bg_img_symbol_font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        lv_area_t a;
        a.x1 = coords->x1 + lv_area_get_width(coords) / 2 - size.x / 2;
        a.x2 = a.x1 + size.x - 1;
        a.y1 = coords->y1 + lv_area_get_height(coords) / 2 - size.y / 2;
        a.y2 = a.y1 + size.y - 1;

        lv_draw_label_dsc_t label_draw_dsc;
        lv_draw_label_dsc_init(&label_draw_dsc);
        label_draw_dsc.font = dsc->bg_img_symbol_font;
        label_draw_dsc.color = dsc->bg_img_recolor;
        label_draw_dsc.opa = dsc->bg_img_opa;
        lv_draw_label((lv_draw_ctx_t *) ctx, &label_draw_dsc, &a, dsc->bg_img_src, NULL);
    }
    else {
        lv_img_header_t header;
        size_t key_size;
        lv_draw_sdl_cache_key_head_img_t * key = lv_draw_sdl_texture_img_key_create(dsc->bg_img_src, 0, &key_size);
        bool key_found;
        lv_img_header_t * cache_header = NULL;
        SDL_Texture * texture = lv_draw_sdl_texture_cache_get_with_userdata(ctx, key, key_size, &key_found,
                                                                            (void **) &cache_header);
        SDL_free(key);
        if(texture) {
            header = *cache_header;
        }
        else if(key_found || lv_img_decoder_get_info(dsc->bg_img_src, &header) != LV_RES_OK) {
            /* When cache hit but with negative result, use default decoder. If still fail, return.*/
            LV_LOG_WARN("Couldn't read the background image");
            return;
        }

        lv_draw_img_dsc_t img_dsc;
        lv_draw_img_dsc_init(&img_dsc);
        img_dsc.blend_mode = dsc->blend_mode;
        img_dsc.recolor = dsc->bg_img_recolor;
        img_dsc.recolor_opa = dsc->bg_img_recolor_opa;
        img_dsc.opa = dsc->bg_img_opa;
        img_dsc.frame_id = 0;

        int16_t radius_mask_id = LV_MASK_ID_INV;
        lv_draw_mask_radius_param_t radius_param;
        if(dsc->radius > 0) {
            lv_draw_mask_radius_init(&radius_param, coords, dsc->radius, false);
            radius_mask_id = lv_draw_mask_add(&radius_param, NULL);
        }

        /*Center align*/
        if(dsc->bg_img_tiled == false) {
            lv_area_t area;
            area.x1 = coords->x1 + lv_area_get_width(coords) / 2 - header.w / 2;
            area.y1 = coords->y1 + lv_area_get_height(coords) / 2 - header.h / 2;
            area.x2 = area.x1 + header.w - 1;
            area.y2 = area.y1 + header.h - 1;

            lv_draw_img((lv_draw_ctx_t *) ctx, &img_dsc, &area, dsc->bg_img_src);
        }
        else {
            lv_area_t area;
            area.y1 = coords->y1;
            area.y2 = area.y1 + header.h - 1;

            for(; area.y1 <= coords->y2; area.y1 += header.h, area.y2 += header.h) {

                area.x1 = coords->x1;
                area.x2 = area.x1 + header.w - 1;
                for(; area.x1 <= coords->x2; area.x1 += header.w, area.x2 += header.w) {
                    lv_draw_img((lv_draw_ctx_t *) ctx, &img_dsc, &area, dsc->bg_img_src);
                }
            }
        }

        if(radius_mask_id != LV_MASK_ID_INV) {
            lv_draw_mask_remove_id(radius_mask_id);
            lv_draw_mask_free_param(&radius_param);
        }
    }
}

static void draw_shadow(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * clip,
                        const lv_draw_rect_dsc_t * dsc)
{
    /*Check whether the shadow is visible*/
    if(SKIP_SHADOW(dsc)) return;

    lv_coord_t sw = dsc->shadow_width;

    lv_area_t core_area;
    core_area.x1 = coords->x1 + dsc->shadow_ofs_x - dsc->shadow_spread;
    core_area.x2 = coords->x2 + dsc->shadow_ofs_x + dsc->shadow_spread;
    core_area.y1 = coords->y1 + dsc->shadow_ofs_y - dsc->shadow_spread;
    core_area.y2 = coords->y2 + dsc->shadow_ofs_y + dsc->shadow_spread;

    lv_area_t shadow_area;
    shadow_area.x1 = core_area.x1 - sw / 2 - 1;
    shadow_area.x2 = core_area.x2 + sw / 2 + 1;
    shadow_area.y1 = core_area.y1 - sw / 2 - 1;
    shadow_area.y2 = core_area.y2 + sw / 2 + 1;

    lv_opa_t opa = dsc->shadow_opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get clipped draw area which is the real draw area.
     *It is always the same or inside `shadow_area`*/
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &shadow_area, clip)) return;

    SDL_Rect core_area_rect;
    lv_area_to_sdl_rect(&shadow_area, &core_area_rect);

    lv_coord_t radius = dsc->radius;
    /* No matter how big the shadow is, what we need is just a corner */
    lv_coord_t frag_size = LV_MIN3(lv_area_get_width(&core_area) / 2, lv_area_get_height(&core_area) / 2,
                                   LV_MAX(sw / 2, radius));

    /* This is how big the corner is after blurring */
    lv_coord_t blur_growth = (lv_coord_t)(sw / 2 + 1);

    lv_coord_t blur_frag_size = (lv_coord_t)(frag_size + blur_growth);

    lv_draw_rect_shadow_key_t key = rect_shadow_key_create(radius, frag_size, sw);

    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(ctx, &key, sizeof(key), NULL);
    bool texture_in_cache = false;
    if(texture == NULL) {
        lv_area_t mask_area = {blur_growth, blur_growth}, mask_area_blurred = {0, 0};
        lv_area_set_width(&mask_area, frag_size * 2);
        lv_area_set_height(&mask_area, frag_size * 2);
        lv_area_set_width(&mask_area_blurred, blur_frag_size * 2);
        lv_area_set_height(&mask_area_blurred, blur_frag_size * 2);

        lv_draw_mask_radius_param_t mask_rout_param;
        lv_draw_mask_radius_init(&mask_rout_param, &mask_area, radius, false);
        int16_t mask_id = lv_draw_mask_add(&mask_rout_param, NULL);
        lv_opa_t * mask_buf = lv_draw_sdl_mask_dump_opa(&mask_area_blurred, &mask_id, 1);
        lv_stack_blur_grayscale(mask_buf, lv_area_get_width(&mask_area_blurred), lv_area_get_height(&mask_area_blurred),
                                sw / 2 + sw % 2);
        texture = lv_sdl_create_opa_texture(ctx->renderer, mask_buf, blur_frag_size, blur_frag_size,
                                            lv_area_get_width(&mask_area_blurred));
        lv_mem_buf_release(mask_buf);
        lv_draw_mask_remove_id(mask_id);
        SDL_assert(texture);
        texture_in_cache = lv_draw_sdl_texture_cache_put(ctx, &key, sizeof(key), texture);
    }
    else {
        texture_in_cache = true;
    }

    SDL_Color shadow_color;
    lv_color_to_sdl_color(&dsc->shadow_color, &shadow_color);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, shadow_color.r, shadow_color.g, shadow_color.b);

    lv_draw_sdl_rect_bg_frag_draw_corners(ctx, texture, blur_frag_size, &shadow_area, clip, false);
    frag_render_borders(ctx->renderer, texture, blur_frag_size, &shadow_area, clip, false);
    frag_render_center(ctx->renderer, texture, blur_frag_size, &shadow_area, clip, false);

    if(!texture_in_cache) {
        LV_LOG_WARN("Texture is not cached, this will impact performance.");
        SDL_DestroyTexture(texture);
    }
}

static void draw_border(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * draw_area,
                        const lv_draw_rect_dsc_t * dsc)
{
    if(SKIP_BORDER(dsc)) return;

    SDL_Color border_color;
    lv_color_to_sdl_color(&dsc->border_color, &border_color);

    lv_coord_t coords_w = lv_area_get_width(coords), coords_h = lv_area_get_height(coords);
    lv_coord_t short_side = LV_MIN(coords_w, coords_h);
    lv_coord_t rout = LV_MIN(dsc->radius, short_side / 2);/*Get the inner area*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);//        lv_area_increase(&area_inner, 1, 1);
    area_inner.x1 += ((dsc->border_side & LV_BORDER_SIDE_LEFT) ? dsc->border_width : -(dsc->border_width + rout));
    area_inner.x2 -= ((dsc->border_side & LV_BORDER_SIDE_RIGHT) ? dsc->border_width : -(dsc->border_width + rout));
    area_inner.y1 += ((dsc->border_side & LV_BORDER_SIDE_TOP) ? dsc->border_width : -(dsc->border_width + rout));
    area_inner.y2 -= ((dsc->border_side & LV_BORDER_SIDE_BOTTOM) ? dsc->border_width : -(dsc->border_width + rout));
    lv_coord_t rin = LV_MAX(rout - dsc->border_width, 0);
    draw_border_generic(ctx, coords, &area_inner, draw_area, rout, rin, dsc->border_color, dsc->border_opa,
                        dsc->blend_mode);
}

static void draw_outline(lv_draw_sdl_ctx_t * ctx, const lv_area_t * coords, const lv_area_t * clip,
                         const lv_draw_rect_dsc_t * dsc)
{
    if(SKIP_OUTLINE(dsc)) return;

    lv_opa_t opa = dsc->outline_opa;

    if(opa > LV_OPA_MAX) opa = LV_OPA_COVER;

    /*Get the inner radius*/
    lv_area_t area_inner;
    lv_area_copy(&area_inner, coords);

    /*Bring the outline closer to make sure there is no color bleeding with pad=0*/
    lv_coord_t pad = dsc->outline_pad - 1;
    area_inner.x1 -= pad;
    area_inner.y1 -= pad;
    area_inner.x2 += pad;
    area_inner.y2 += pad;

    lv_area_t area_outer;
    lv_area_copy(&area_outer, &area_inner);

    area_outer.x1 -= dsc->outline_width;
    area_outer.x2 += dsc->outline_width;
    area_outer.y1 -= dsc->outline_width;
    area_outer.y2 += dsc->outline_width;

    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &area_outer, clip)) return;

    int32_t inner_w = lv_area_get_width(&area_inner);
    int32_t inner_h = lv_area_get_height(&area_inner);
    lv_coord_t rin = dsc->radius;
    int32_t short_side = LV_MIN(inner_w, inner_h);
    if(rin > short_side >> 1) rin = short_side >> 1;

    lv_coord_t rout = rin + dsc->outline_width;

    draw_border_generic(ctx, &area_outer, &area_inner, clip, rout, rin, dsc->outline_color, dsc->outline_opa,
                        dsc->blend_mode);
}

static void draw_border_generic(lv_draw_sdl_ctx_t * ctx, const lv_area_t * outer_area, const lv_area_t * inner_area,
                                const lv_area_t * clip, lv_coord_t rout, lv_coord_t rin, lv_color_t color, lv_opa_t opa,
                                lv_blend_mode_t blend_mode)
{
    opa = opa >= LV_OPA_COVER ? LV_OPA_COVER : opa;

    SDL_Renderer * renderer = ctx->renderer;

    lv_draw_rect_border_key_t key = rect_border_key_create(rout, rin, outer_area, inner_area);
    lv_coord_t radius = LV_MIN3(rout, lv_area_get_width(outer_area) / 2, lv_area_get_height(outer_area) / 2);
    lv_coord_t max_side = LV_MAX4(key.offsets.x1, key.offsets.y1, -key.offsets.x2, -key.offsets.y2);
    lv_coord_t frag_size = LV_MAX(radius, max_side);
    SDL_Texture * texture = lv_draw_sdl_texture_cache_get(ctx, &key, sizeof(key), NULL);
    bool texture_in_cache;
    if(texture == NULL) {
        /* Create a mask texture with size of (frag_size * 2 + FRAG_SPACING) */
        const lv_area_t frag_area = {0, 0, frag_size * 2 + FRAG_SPACING - 1, frag_size * 2 + FRAG_SPACING - 1};

        /*Create mask for the outer area*/
        int16_t mask_ids[2] = {LV_MASK_ID_INV, LV_MASK_ID_INV};
        lv_draw_mask_radius_param_t mask_rout_param;
        if(rout > 0) {
            lv_draw_mask_radius_init(&mask_rout_param, &frag_area, rout, false);
            mask_ids[0] = lv_draw_mask_add(&mask_rout_param, NULL);
        }

        /*Create mask for the inner mask*/
        if(rin < 0) rin = 0;
        const lv_area_t frag_inner_area = {frag_area.x1 + key.offsets.x1, frag_area.y1 + key.offsets.y1,
                                           frag_area.x2 + key.offsets.x2, frag_area.y2 + key.offsets.y2
                                          };
        lv_draw_mask_radius_param_t mask_rin_param;
        lv_draw_mask_radius_init(&mask_rin_param, &frag_inner_area, rin, true);
        mask_ids[1] = lv_draw_mask_add(&mask_rin_param, NULL);

        texture = lv_draw_sdl_mask_dump_texture(renderer, &frag_area, mask_ids, 2);

        lv_draw_mask_remove_id(mask_ids[1]);
        lv_draw_mask_remove_id(mask_ids[0]);
        SDL_assert(texture);
        texture_in_cache = lv_draw_sdl_texture_cache_put(ctx, &key, sizeof(key), texture);
    }
    else {
        texture_in_cache = true;
    }

    SDL_Rect outer_rect;
    lv_area_to_sdl_rect(outer_area, &outer_rect);
    SDL_Color color_sdl;
    lv_color_to_sdl_color(&color, &color_sdl);

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureAlphaMod(texture, opa);
    SDL_SetTextureColorMod(texture, color_sdl.r, color_sdl.g, color_sdl.b);

    lv_draw_sdl_rect_bg_frag_draw_corners(ctx, texture, frag_size, outer_area, clip, true);
    frag_render_borders(renderer, texture, frag_size, outer_area, clip, true);

    if(!texture_in_cache) {
        LV_LOG_WARN("Texture is not cached, this will impact performance.");
        SDL_DestroyTexture(texture);
    }
}

static void frag_render_borders(SDL_Renderer * renderer, SDL_Texture * frag, lv_coord_t frag_size,
                                const lv_area_t * coords, const lv_area_t * clipped, bool full)
{
    lv_area_t border_area, dst_area;
    /* Top border */
    border_area.x1 = coords->x1 + frag_size;
    border_area.y1 = coords->y1;
    border_area.x2 = coords->x2 - frag_size;
    border_area.y2 = coords->y1 + frag_size - 1;
    if(_lv_area_intersect(&dst_area, &border_area, clipped)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t sy = (lv_coord_t)(dst_area.y1 - border_area.y1);
        if(full) {
            SDL_Rect src_rect = {frag_size + 1, sy, 1, lv_area_get_height(&dst_area)};
            SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
        }
        else {
            SDL_Rect src_rect = {frag_size - 1, sy, 1, lv_area_get_height(&dst_area)};
            SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
        }
    }
    /* Bottom border */
    border_area.y1 = LV_MAX(coords->y2 - frag_size + 1, coords->y1 + frag_size);
    border_area.y2 = coords->y2;
    if(_lv_area_intersect(&dst_area, &border_area, clipped)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t dh = lv_area_get_height(&dst_area);
        if(full) {
            lv_coord_t sy = (lv_coord_t)(dst_area.y1 - border_area.y1);
            SDL_Rect src_rect = {frag_size + 1, frag_size + FRAG_SPACING + sy, 1, dh};
            SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
        }
        else {
            lv_coord_t sy = (lv_coord_t)(border_area.y2 - dst_area.y2);
            SDL_Rect src_rect = {frag_size - 1, sy, 1, dh};
            SDL_RenderCopyEx(renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_VERTICAL);
        }
    }
    /* Left border */
    border_area.x1 = coords->x1;
    border_area.y1 = coords->y1 + frag_size;
    border_area.x2 = coords->x1 + frag_size - 1;
    border_area.y2 = coords->y2 - frag_size;
    if(_lv_area_intersect(&dst_area, &border_area, clipped)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t dw = lv_area_get_width(&dst_area);
        lv_coord_t sx = (lv_coord_t)(dst_area.x1 - border_area.x1);
        if(full) {
            SDL_Rect src_rect = {sx, frag_size + 1, dw, 1};
            SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
        }
        else {
            SDL_Rect src_rect = {sx, frag_size - 1, dw, 1};
            SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
        }
    }
    /* Right border */
    border_area.x1 = LV_MAX(coords->x2 - frag_size + 1, coords->x1 + frag_size);
    border_area.x2 = coords->x2;
    if(_lv_area_intersect(&dst_area, &border_area, clipped)) {
        SDL_Rect dst_rect;
        lv_area_to_sdl_rect(&dst_area, &dst_rect);

        lv_coord_t dw = lv_area_get_width(&dst_area);
        if(full) {
            lv_coord_t sx = (lv_coord_t)(dst_area.x1 - border_area.x1);
            SDL_Rect src_rect = {frag_size + FRAG_SPACING + sx, frag_size + 1, dw, 1};
            SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
        }
        else {
            lv_coord_t sx = (lv_coord_t)(border_area.x2 - dst_area.x2);
            SDL_Rect src_rect = {sx, frag_size - 1, dw, 1};
            SDL_RenderCopyEx(renderer, frag, &src_rect, &dst_rect, 0, NULL, SDL_FLIP_HORIZONTAL);
        }
    }
}

static void frag_render_center(SDL_Renderer * renderer, SDL_Texture * frag, lv_coord_t frag_size,
                               const lv_area_t * coords,
                               const lv_area_t * clipped, bool full)
{
    lv_area_t center_area = {
        coords->x1 + frag_size,
        coords->y1 + frag_size,
        coords->x2 - frag_size,
        coords->y2 - frag_size,
    };
    if(center_area.x2 < center_area.x1 || center_area.y2 < center_area.y1) return;
    lv_area_t draw_area;
    if(!_lv_area_intersect(&draw_area, &center_area, clipped)) {
        return;
    }
    SDL_Rect dst_rect;
    lv_area_to_sdl_rect(&draw_area, &dst_rect);
    if(full) {
        SDL_Rect src_rect = {frag_size, frag_size, 1, 1};
        SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
    }
    else {
        SDL_Rect src_rect = {frag_size - 1, frag_size - 1, 1, 1};
        SDL_RenderCopy(renderer, frag, &src_rect, &dst_rect);
    }
}

static lv_draw_rect_bg_key_t rect_bg_key_create(lv_coord_t radius, lv_coord_t size)
{
    lv_draw_rect_bg_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_BG;
    key.radius = radius;
    key.size = size;
    return key;
}

static lv_draw_rect_grad_frag_key_t rect_grad_frag_key_create(const lv_grad_dsc_t * grad, lv_coord_t w, lv_coord_t h,
                                                              lv_coord_t radius)
{
    lv_draw_rect_grad_frag_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_GRAD;
    key.stops_count = grad->stops_count;
    key.dir = grad->dir;
    for(uint8_t i = 0; i < grad->stops_count; i++) {
        key.stops[i].frac = grad->stops[i].frac;
        key.stops[i].color = grad->stops[i].color;
    }
    key.w = w;
    key.h = h;
    key.radius = radius;
    return key;
}

static lv_draw_rect_grad_strip_key_t rect_grad_strip_key_create(const lv_grad_dsc_t * grad)
{
    lv_draw_rect_grad_strip_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_GRAD;
    key.stops_count = grad->stops_count;
    key.dir = grad->dir;
    for(uint8_t i = 0; i < grad->stops_count; i++) {
        key.stops[i].frac = grad->stops[i].frac;
        key.stops[i].color = grad->stops[i].color;
    }
    return key;
}

static lv_draw_rect_shadow_key_t rect_shadow_key_create(lv_coord_t radius, lv_coord_t size, lv_coord_t blur)
{
    lv_draw_rect_shadow_key_t key;
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_SHADOW;
    key.radius = radius;
    key.size = size;
    key.blur = blur;
    return key;
}

static lv_draw_rect_border_key_t rect_border_key_create(lv_coord_t rout, lv_coord_t rin, const lv_area_t * outer_area,
                                                        const lv_area_t * inner_area)
{
    lv_draw_rect_border_key_t key;
    /* VERY IMPORTANT! Padding between members is uninitialized, so we have to wipe them manually */
    SDL_memset(&key, 0, sizeof(key));
    key.magic = LV_GPU_CACHE_KEY_MAGIC_RECT_BORDER;
    key.rout = rout;
    key.rin = rin;
    key.offsets.x1 = inner_area->x1 - outer_area->x1;
    key.offsets.x2 = inner_area->x2 - outer_area->x2;
    key.offsets.y1 = inner_area->y1 - outer_area->y1;
    key.offsets.y2 = inner_area->y2 - outer_area->y2;
    return key;
}

#endif /*LV_USE_GPU_SDL*/
