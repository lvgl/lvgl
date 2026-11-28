/**
 * @file lv_draw_sdl.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../lv_draw_private.h"
#include "../sw/lv_draw_sw_private.h"
#if LV_USE_DRAW_SDL
#include LV_SDL_INCLUDE_PATH
#include "lv_draw_sdl.h"
#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"
#include "../../misc/lv_area_private.h"
#include "../lv_draw_image_private.h"

/*********************
 *      DEFINES
 *********************/
#define DRAW_UNIT_ID_SDL     100

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_cache_t * texture_cache;
    lv_draw_buf_t render_draw_buf;
} lv_draw_sdl_unit_t;

typedef struct {
    lv_draw_dsc_t * draw_dsc;
    int32_t w;
    int32_t h;
    SDL_Texture * texture;
    bool dsc_owned;
} cache_data_t;

typedef struct {
    lv_draw_sdl_unit_t * u;
    lv_area_t coords;       /*Relative to `real_area`'s origin*/
    lv_area_t real_area;    /*Relative, (0;0) based*/
} texture_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static bool sdl_takes(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_dsc_t * dsc);
static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
static bool can_draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc);
static bool can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);
static bool can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);
static bool can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc);
static bool draw_to_texture(texture_data_t * tex_data, cache_data_t * cache_data);
static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);
static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords);
static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords);
static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords);
static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc);
static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc);

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = draw_fill,
    .can_draw_fill_cb = can_draw_fill,
    .draw_border_cb = draw_border,
    .can_draw_border_cb = can_draw_border,
    .draw_box_shadow_cb = draw_box_shadow,
    .can_draw_box_shadow_cb = can_draw_box_shadow,
    .draw_label_cb = draw_label,
    .can_draw_label_cb = can_draw_label,
    .draw_image_cb = draw_image,
    .can_draw_image_cb = can_draw_image,
    .draw_layer_cb = draw_layer,
    .can_draw_layer_cb = can_draw_layer,
    .draw_arc_cb = draw_arc,
    .can_draw_arc_cb = can_draw_arc,
    .draw_line_cb = draw_line,
    .can_draw_line_cb = can_draw_line,
    .draw_triangle_cb = draw_triangle,
    .can_draw_triangle_cb = can_draw_triangle,
};
static SDL_Texture * layer_get_texture(lv_layer_t * layer);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static bool sdl_texture_cache_create_cb(cache_data_t * cached_data, void * user_data)
{
    return draw_to_texture(user_data, cached_data);
}

static void sdl_texture_cache_free_cb(cache_data_t * cached_data, void * user_data)
{
    LV_UNUSED(user_data);

    if(cached_data->dsc_owned) lv_free(cached_data->draw_dsc);
    SDL_DestroyTexture(cached_data->texture);
    cached_data->dsc_owned = false;
    cached_data->draw_dsc = NULL;
    cached_data->texture = NULL;
}

static lv_cache_compare_res_t sdl_texture_cache_compare_cb(const cache_data_t * lhs, const cache_data_t * rhs)
{
    if(lhs == rhs) return 0;

    if(lhs->w != rhs->w) {
        return lhs->w > rhs->w ? 1 : -1;
    }
    if(lhs->h != rhs->h) {
        return lhs->h > rhs->h ? 1 : -1;
    }

    if(lhs->draw_dsc == NULL || rhs->draw_dsc == NULL) {
        if(lhs->draw_dsc == rhs->draw_dsc) return 0;
        if(lhs->draw_dsc == NULL) return -1;
        return 1;
    }

    uint32_t lhs_dsc_size = lhs->draw_dsc->dsc_size;
    uint32_t rhs_dsc_size = rhs->draw_dsc->dsc_size;

    if(lhs_dsc_size != rhs_dsc_size) {
        return lhs_dsc_size > rhs_dsc_size ? 1 : -1;
    }

    const uint8_t * left_draw_dsc = (const uint8_t *)lhs->draw_dsc;
    const uint8_t * right_draw_dsc = (const uint8_t *)rhs->draw_dsc;
    left_draw_dsc += sizeof(lv_draw_dsc_t);
    right_draw_dsc += sizeof(lv_draw_dsc_t);

    int cmp_res = lv_memcmp(left_draw_dsc, right_draw_dsc, lhs->draw_dsc->dsc_size - sizeof(lv_draw_dsc_t));

    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

void lv_draw_sdl_init(void)
{
    lv_draw_sdl_unit_t * draw_sdl_unit = lv_draw_create_unit(sizeof(lv_draw_sdl_unit_t));
    draw_sdl_unit->base_unit.dispatch_cb = dispatch;
    draw_sdl_unit->base_unit.draw_cbs = &draw_cbs;
    /*No software fallback: SDL draws into textures, drop what it can't draw*/
    draw_sdl_unit->base_unit.fallback = NULL;
    draw_sdl_unit->base_unit.id = DRAW_UNIT_ID_SDL;
    draw_sdl_unit->base_unit.name = "SDL";
    draw_sdl_unit->texture_cache = lv_cache_create(&lv_cache_class_lru_rb_count,
    sizeof(cache_data_t), 128, (lv_cache_ops_t) {
        .compare_cb = (lv_cache_compare_cb_t)sdl_texture_cache_compare_cb,
        .create_cb = (lv_cache_create_cb_t)sdl_texture_cache_create_cb,
        .free_cb = (lv_cache_free_cb_t)sdl_texture_cache_free_cb,
    });
    lv_cache_set_name(draw_sdl_unit->texture_cache, "SDL_TEXTURE");

    lv_draw_buf_init(&draw_sdl_unit->render_draw_buf, 0, 0, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO, NULL, 0);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_SDL);
    if(t == NULL) return -1;

    lv_display_t * disp = lv_refr_get_disp_refreshing();
    SDL_Texture * texture = layer_get_texture(layer);
    if(layer != disp->layer_head && texture == NULL) {
        void * buf = lv_draw_layer_alloc_buf(layer);
        if(buf == NULL) {
            t->state = LV_DRAW_TASK_STATE_FAILED;
            return -1;
        }

        SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);
        int32_t w = lv_area_get_width(&layer->buf_area);
        int32_t h = lv_area_get_height(&layer->buf_area);
        layer->user_data = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_TARGET, w, h);
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    lv_draw_unit_draw_task(draw_unit, t);
    t->state = LV_DRAW_TASK_STATE_FINISHED;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();
    return 1;
}

static bool sdl_takes(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(dsc);

    /*If not refreshing the display probably it's a canvas rendering
     *which his not support in SDL as it's not a texture.*/
    return lv_refr_get_disp_refreshing() != NULL;
}

static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    if(dsc->header.cf >= LV_COLOR_FORMAT_PROPRIETARY_START) return false;
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc)
{
    return sdl_takes(draw_unit, layer, &dsc->base);
}

static bool draw_to_texture(texture_data_t * tex_data, cache_data_t * cache_data)
{
    lv_draw_sdl_unit_t * u = tex_data->u;
    const lv_area_t * real_area = &tex_data->real_area;
    const lv_area_t * coords = &tex_data->coords;

    int32_t texture_w = lv_area_get_width(real_area);
    int32_t texture_h = lv_area_get_height(real_area);

    if(!lv_draw_buf_reshape(&u->render_draw_buf, LV_COLOR_FORMAT_ARGB8888, texture_w, texture_h, LV_STRIDE_AUTO)) {
        uint8_t * data = u->render_draw_buf.unaligned_data;
        uint32_t data_size = LV_DRAW_BUF_SIZE(texture_w, texture_h, LV_COLOR_FORMAT_ARGB8888);
        if(data == NULL) data = malloc(data_size);
        else data = realloc(data, data_size);

        LV_ASSERT_MALLOC(data);
        lv_result_t init_result = lv_draw_buf_init(&u->render_draw_buf, texture_w, texture_h, LV_COLOR_FORMAT_ARGB8888,
                                                   LV_STRIDE_AUTO, data, data_size);
        LV_ASSERT(init_result == LV_RESULT_OK);
        LV_UNUSED(init_result);
    }

    lv_layer_t dest_layer;
    lv_layer_init(&dest_layer);
    dest_layer.draw_buf = &u->render_draw_buf;
    dest_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    dest_layer.buf_area = *real_area;
    dest_layer._clip_area = *real_area;
    dest_layer.phy_clip_area = *real_area;
    lv_memzero(u->render_draw_buf.data, lv_area_get_size(real_area) * 4);

    lv_draw_dsc_t * dsc = cache_data->draw_dsc;
    LV_ASSERT(dsc != NULL);

    /*Draw with the SW leaves: no front end, so no draw task and no draw events*/
    switch(dsc->type) {
        case LV_DRAW_DSC_TYPE_FILL:
            lv_draw_sw_fill(&dest_layer, real_area, lv_draw_dsc_get_fill_dsc(dsc), coords);
            break;
        case LV_DRAW_DSC_TYPE_BORDER:
            lv_draw_sw_border(&dest_layer, real_area, lv_draw_dsc_get_border_dsc(dsc), coords);
            break;
        case LV_DRAW_DSC_TYPE_BOX_SHADOW:
            lv_draw_sw_box_shadow(&dest_layer, real_area, lv_draw_dsc_get_box_shadow_dsc(dsc), coords);
            break;
        case LV_DRAW_DSC_TYPE_LABEL:
            lv_draw_sw_label(&dest_layer, real_area, lv_draw_dsc_get_label_dsc(dsc), coords);
            break;
        case LV_DRAW_DSC_TYPE_ARC:
            lv_draw_sw_arc(&dest_layer, real_area, lv_draw_dsc_get_arc_dsc(dsc), coords);
            break;
        case LV_DRAW_DSC_TYPE_LINE:
            lv_draw_line_iterate(&dest_layer, real_area, lv_draw_dsc_get_line_dsc(dsc), lv_draw_sw_line, NULL);
            break;
        case LV_DRAW_DSC_TYPE_TRIANGLE:
            lv_draw_sw_triangle(&dest_layer, real_area, lv_draw_dsc_get_triangle_dsc(dsc));
            break;
        case LV_DRAW_DSC_TYPE_IMAGE:
            lv_draw_sw_image(&dest_layer, real_area, lv_draw_dsc_get_image_dsc(dsc), coords);
            break;
        default:
            /*Keep in sync with the draw callbacks: every type they cache must have a case above*/
            LV_UNREACHABLE();
    }

    lv_display_t * disp = lv_refr_get_disp_refreshing();
    SDL_Texture * texture = SDL_CreateTexture(lv_sdl_window_get_renderer(disp), SDL_PIXELFORMAT_ARGB8888,
                                              SDL_TEXTUREACCESS_STATIC, texture_w, texture_h);
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_UpdateTexture(texture, NULL, u->render_draw_buf.data, texture_w * 4);

    cache_data->draw_dsc = lv_malloc(dsc->dsc_size);
    lv_memcpy((void *)cache_data->draw_dsc, dsc, dsc->dsc_size);
    cache_data->dsc_owned = true;
    cache_data->w = texture_w;
    cache_data->h = texture_h;
    cache_data->texture = texture;

    return true;
}

/**
 * Draw `key` from the texture cache, rendering it with SW first if it's not cached yet.
 * `key` is a copy of the dsc with its geometry relative to `coords`.
 * `real_area` is the absolute area the texture covers. With `keep == false` the texture is dropped
 * after drawing (e.g. the dsc points to memory that can be freed).
 */
static void draw_cached(lv_draw_sdl_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area, lv_draw_dsc_t * key,
                        const lv_area_t * coords, const lv_area_t * real_area, bool keep)
{
    /*Absolute coordinates differ for the same dsc on a different position, so cache relative to (0;0)*/
    texture_data_t tex_data;
    tex_data.u = u;
    tex_data.coords = *coords;
    tex_data.real_area = *real_area;
    lv_area_move(&tex_data.coords, -coords->x1, -coords->y1);
    lv_area_move(&tex_data.real_area, -coords->x1, -coords->y1);

    cache_data_t data_to_find;
    data_to_find.draw_dsc = key;
    data_to_find.w = lv_area_get_width(real_area);
    data_to_find.h = lv_area_get_height(real_area);
    data_to_find.texture = NULL;
    data_to_find.dsc_owned = false;

    lv_cache_entry_t * entry_cached = lv_cache_acquire_or_create(u->texture_cache, &data_to_find, &tex_data);
    if(!entry_cached) return;

    cache_data_t * data_cached = lv_cache_entry_get_data(entry_cached);
    lv_display_t * disp = lv_refr_get_disp_refreshing();
    SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);

    SDL_Rect clip_rect;
    clip_rect.x = clip_area->x1 - layer->buf_area.x1;
    clip_rect.y = clip_area->y1 - layer->buf_area.y1;
    clip_rect.w = lv_area_get_width(clip_area);
    clip_rect.h = lv_area_get_height(clip_area);

    SDL_Rect rect;
    rect.x = real_area->x1 - layer->buf_area.x1;
    rect.y = real_area->y1 - layer->buf_area.y1;
    rect.w = data_cached->w;
    rect.h = data_cached->h;

    SDL_SetRenderTarget(renderer, layer_get_texture(layer));
    SDL_RenderSetClipRect(renderer, &clip_rect);
    SDL_RenderCopy(renderer, data_cached->texture, NULL, &rect);
    SDL_RenderSetClipRect(renderer, NULL);

    lv_cache_release(u->texture_cache, entry_cached, u);

    if(!keep) lv_cache_drop(u->texture_cache, &data_to_find, u);
}

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    if(dsc->radius == 0 && dsc->grad.dir == LV_GRAD_DIR_NONE) {
        lv_area_t fill_area;
        lv_area_intersect(&fill_area, coords, clip_area);

        SDL_Rect rect;
        rect.x = fill_area.x1 - layer->buf_area.x1;
        rect.y = fill_area.y1 - layer->buf_area.y1;
        rect.w = lv_area_get_width(&fill_area);
        rect.h = lv_area_get_height(&fill_area);
        lv_display_t * disp = lv_refr_get_disp_refreshing();
        SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, dsc->color.red, dsc->color.green, dsc->color.blue, dsc->opa);
        SDL_RenderSetClipRect(renderer, NULL);
        SDL_RenderFillRect(renderer, &rect);
        return;
    }

    lv_draw_fill_dsc_t key = *dsc;
    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, coords, coords, true);
}

static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_border_dsc_t key = *dsc;
    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, coords, coords, true);
}

static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_box_shadow_dsc_t key = *dsc;

    lv_area_t real_area = *coords;
    lv_area_increase(&real_area, dsc->spread, dsc->spread);
    lv_area_increase(&real_area, dsc->width, dsc->width);
    lv_area_move(&real_area, dsc->ofs_x, dsc->ofs_y);

    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, coords, &real_area, true);
}

static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_label_dsc_t key = *dsc;

    /*Don't keep non-static texts: the text can be freed or reallocated, leaving a wild pointer in the cache*/
    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, coords, coords, dsc->text_static);
}

static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_image_dsc_t key = *dsc;
    lv_area_move(&key.image_area, -coords->x1, -coords->y1);

    lv_area_t real_area;
    lv_draw_image_get_transformed_area(&real_area, dsc, coords);

    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, coords, &real_area, true);
}

static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);

    lv_display_t * disp = lv_refr_get_disp_refreshing();
    SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);

    SDL_Rect clip_rect;
    clip_rect.x = clip_area->x1;
    clip_rect.y = clip_area->y1;
    clip_rect.w = lv_area_get_width(clip_area);
    clip_rect.h = lv_area_get_height(clip_area);

    SDL_Rect rect;
    rect.w = (lv_area_get_width(coords) * dsc->scale_x) / 256;
    rect.h = (lv_area_get_height(coords) * dsc->scale_y) / 256;

    rect.x = -dsc->pivot.x;
    rect.y = -dsc->pivot.y;
    rect.x = (rect.x * dsc->scale_x) / 256;
    rect.y = (rect.y * dsc->scale_y) / 256;
    rect.x += coords->x1 + dsc->pivot.x;
    rect.y += coords->y1 + dsc->pivot.y;

    lv_layer_t * src_layer = (lv_layer_t *)dsc->src;
    SDL_Texture * src_texture = layer_get_texture(src_layer);

    SDL_SetTextureAlphaMod(src_texture, dsc->opa);
    SDL_SetTextureBlendMode(src_texture, SDL_BLENDMODE_BLEND);
    SDL_SetRenderTarget(renderer, layer_get_texture(layer));
    SDL_RenderSetClipRect(renderer, &clip_rect);

    SDL_Point center = {dsc->pivot.x, dsc->pivot.y};
    SDL_RenderCopyEx(renderer, src_texture, NULL, &rect, dsc->rotation / 10, &center, SDL_FLIP_NONE);

    SDL_DestroyTexture(src_texture);
    SDL_RenderSetClipRect(renderer, NULL);
}

static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_arc_dsc_t key = *dsc;
    key.center.x -= coords->x1;
    key.center.y -= coords->y1;

    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, coords, coords, true);
}

static void draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_line_dsc_t * dsc)
{
    /*The same area as `lv_draw_line()`*/
    lv_area_t coords;
    if(dsc->points == NULL) {
        coords.x1 = (int32_t)LV_MIN(dsc->p1.x, dsc->p2.x) - dsc->width;
        coords.x2 = (int32_t)LV_MAX(dsc->p1.x, dsc->p2.x) + dsc->width;
        coords.y1 = (int32_t)LV_MIN(dsc->p1.y, dsc->p2.y) - dsc->width;
        coords.y2 = (int32_t)LV_MAX(dsc->p1.y, dsc->p2.y) + dsc->width;
    }
    else {
        coords.x1 = LV_COORD_MAX;
        coords.y1 = LV_COORD_MAX;
        coords.x2 = LV_COORD_MIN;
        coords.y2 = LV_COORD_MIN;
        int32_t i;
        for(i = 0; i < dsc->point_cnt; i++) {
            if(dsc->points[i].x == LV_DRAW_LINE_POINT_NONE || dsc->points[i].y == LV_DRAW_LINE_POINT_NONE) continue;
            coords.x1 = (int32_t)LV_MIN(coords.x1, dsc->points[i].x) - dsc->width;
            coords.x2 = (int32_t)LV_MAX(coords.x2, dsc->points[i].x) + dsc->width;
            coords.y1 = (int32_t)LV_MIN(coords.y1, dsc->points[i].y) - dsc->width;
            coords.y2 = (int32_t)LV_MAX(coords.y2, dsc->points[i].y) + dsc->width;
        }
        if(coords.x1 == LV_COORD_MAX) return;
        lv_area_increase(&coords, dsc->width, dsc->width);
    }

    lv_draw_line_dsc_t key = *dsc;
    key.p1.x -= coords.x1;
    key.p1.y -= coords.y1;
    key.p2.x -= coords.x1;
    key.p2.y -= coords.y1;

    /*Don't keep lines drawn from points: the points are freed with the task*/
    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, &coords, &coords, dsc->points == NULL);
}

static void draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                          const lv_draw_triangle_dsc_t * dsc)
{
    /*The same area as `lv_draw_triangle()`*/
    lv_area_t coords;
    coords.x1 = (int32_t)LV_MIN3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    coords.y1 = (int32_t)LV_MIN3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);
    coords.x2 = (int32_t)LV_MAX3(dsc->p[0].x, dsc->p[1].x, dsc->p[2].x);
    coords.y2 = (int32_t)LV_MAX3(dsc->p[0].y, dsc->p[1].y, dsc->p[2].y);

    lv_draw_triangle_dsc_t key = *dsc;
    uint32_t i;
    for(i = 0; i < 3; i++) {
        key.p[i].x -= coords.x1;
        key.p[i].y -= coords.y1;
    }

    draw_cached((lv_draw_sdl_unit_t *)draw_unit, layer, clip_area, &key.base, &coords, &coords, true);
}

static SDL_Texture * layer_get_texture(lv_layer_t * layer)
{
    return layer->user_data;
}

#endif /*LV_USE_DRAW_SDL*/
