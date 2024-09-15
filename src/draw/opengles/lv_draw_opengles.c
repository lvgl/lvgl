/**
 * @file lv_draw_opengles.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_opengles.h"
#if LV_USE_DRAW_OPENGLES
#include "../lv_draw_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"
#include "../../drivers/glfw/lv_opengles_debug.h"
#include "../../drivers/glfw/lv_opengles_texture.h"
#include "../../drivers/glfw/lv_opengles_driver.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "../../draw/lv_draw_label.h"
#include "../../draw/lv_draw_rect.h"
#include "../../draw/lv_draw_arc.h"
#include "../../draw/lv_draw_image.h"
#include "../../draw/lv_draw_triangle.h"
#include "../../draw/lv_draw_line.h"
#include "../../core/lv_obj.h"
#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_area_private.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_OPENGLES 6

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_draw_task_t * task_act;
    lv_cache_t * texture_cache;
} lv_draw_opengles_unit_t;

typedef struct {
    lv_draw_dsc_base_t * draw_dsc;
    int32_t w;
    int32_t h;
    unsigned int texture;
} cache_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void execute_drawing(lv_draw_opengles_unit_t * u);

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static bool draw_to_texture(lv_draw_opengles_unit_t * u, cache_data_t * cache_data);

static unsigned int layer_get_texture(lv_layer_t * layer);
/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
static bool opengles_texture_cache_create_cb(cache_data_t * cached_data, void * user_data)
{
    return draw_to_texture((lv_draw_opengles_unit_t *)user_data, cached_data);
}

static void opengles_texture_cache_free_cb(cache_data_t * cached_data, void * user_data)
{
    LV_UNUSED(user_data);

    lv_free(cached_data->draw_dsc);
    GL_CALL(glDeleteTextures(1, &cached_data->texture));
    cached_data->draw_dsc = NULL;
    cached_data->texture = 0;
}

static lv_cache_compare_res_t opengles_texture_cache_compare_cb(const cache_data_t * lhs, const cache_data_t * rhs)
{
    if(lhs == rhs) return 0;

    if(lhs->w != rhs->w) {
        return lhs->w > rhs->w ? 1 : -1;
    }
    if(lhs->h != rhs->h) {
        return lhs->h > rhs->h ? 1 : -1;
    }

    uint32_t lhs_dsc_size = lhs->draw_dsc->dsc_size;
    uint32_t rhs_dsc_size = rhs->draw_dsc->dsc_size;

    if(lhs_dsc_size != rhs_dsc_size) {
        return lhs_dsc_size > rhs_dsc_size ? 1 : -1;
    }

    const uint8_t * left_draw_dsc = (const uint8_t *)lhs->draw_dsc;
    const uint8_t * right_draw_dsc = (const uint8_t *)rhs->draw_dsc;
    left_draw_dsc += sizeof(lv_draw_dsc_base_t);
    right_draw_dsc += sizeof(lv_draw_dsc_base_t);

    int cmp_res = lv_memcmp(left_draw_dsc, right_draw_dsc, lhs->draw_dsc->dsc_size - sizeof(lv_draw_dsc_base_t));

    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

void lv_draw_opengles_init(void)
{
    lv_draw_opengles_unit_t * draw_opengles_unit = lv_draw_create_unit(sizeof(lv_draw_opengles_unit_t));
    draw_opengles_unit->base_unit.dispatch_cb = dispatch;
    draw_opengles_unit->base_unit.evaluate_cb = evaluate;
    draw_opengles_unit->texture_cache = lv_cache_create(&lv_cache_class_lru_rb_count,
    sizeof(cache_data_t), 128, (lv_cache_ops_t) {
        .compare_cb = (lv_cache_compare_cb_t)opengles_texture_cache_compare_cb,
        .create_cb = (lv_cache_create_cb_t)opengles_texture_cache_create_cb,
        .free_cb = (lv_cache_free_cb_t)opengles_texture_cache_free_cb,
    });
    lv_cache_set_name(draw_opengles_unit->texture_cache, "OPENGLES_TEXTURE");
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_opengles_unit_t * draw_opengles_unit = (lv_draw_opengles_unit_t *) draw_unit;

    /*Return immediately if it's busy with a draw task*/
    if(draw_opengles_unit->task_act) return 0;

    lv_draw_task_t * t = NULL;
    t = lv_draw_get_next_available_task(layer, NULL, DRAW_UNIT_ID_OPENGLES);
    if(t == NULL) return -1;

    unsigned int texture = layer_get_texture(layer);
    if(texture == 0) {
        lv_display_t * disp = lv_refr_get_disp_refreshing();
        if(layer != disp->layer_head) {
            void * buf = lv_draw_layer_alloc_buf(layer);
            if(buf == NULL) return -1;

            int32_t w = lv_area_get_width(&layer->buf_area);
            int32_t h = lv_area_get_height(&layer->buf_area);

            GL_CALL(glGenTextures(1, &texture));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

            /*Color depth: 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
#if LV_COLOR_DEPTH == 8
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, NULL));
#elif LV_COLOR_DEPTH == 16
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, w, h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, NULL));
#elif LV_COLOR_DEPTH == 24
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL));
#elif LV_COLOR_DEPTH == 32
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL));
#else
#error("Unsupported color format")
#endif

            glGenerateMipmap(GL_TEXTURE_2D);

            layer->user_data = (void *)(uintptr_t)texture;
        }
        else {
            layer->user_data = (void *)(uintptr_t)lv_opengles_texture_get_texture_id(disp);
        }
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    draw_opengles_unit->base_unit.target_layer = layer;
    draw_opengles_unit->base_unit.clip_area = &t->clip_area;
    draw_opengles_unit->task_act = t;

    execute_drawing(draw_opengles_unit);

    draw_opengles_unit->task_act->state = LV_DRAW_TASK_STATE_READY;
    draw_opengles_unit->task_act = NULL;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();
    return 1;
}

static int32_t evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);

    if(((lv_draw_dsc_base_t *)task->draw_dsc)->user_data == NULL) {
        task->preference_score = 0;
        task->preferred_draw_unit_id = DRAW_UNIT_ID_OPENGLES;
    }
    return 0;
}

static bool draw_to_texture(lv_draw_opengles_unit_t * u, cache_data_t * cache_data)
{
    static uint8_t opengles_render_buf[2048 * 1024 * 4];

    lv_draw_task_t * task = u->task_act;

    lv_layer_t dest_layer;
    lv_memzero(&dest_layer, sizeof(dest_layer));

    int32_t texture_w = lv_area_get_width(&task->_real_area);
    int32_t texture_h = lv_area_get_height(&task->_real_area);

    lv_draw_buf_t draw_buf;
    dest_layer.draw_buf = &draw_buf;
    lv_draw_buf_init(dest_layer.draw_buf, texture_w, texture_h,
                     LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO, opengles_render_buf, sizeof(opengles_render_buf));
    dest_layer.color_format = LV_COLOR_FORMAT_ARGB8888;

    dest_layer.buf_area = task->_real_area;
    dest_layer._clip_area = task->_real_area;
    dest_layer.phy_clip_area = task->_real_area;
    lv_memzero(opengles_render_buf, lv_area_get_size(&dest_layer.buf_area) * 4 + 100);

    lv_display_t * disp = lv_refr_get_disp_refreshing();

    lv_obj_t * obj = ((lv_draw_dsc_base_t *)task->draw_dsc)->obj;
    bool original_send_draw_task_event = false;
    if(obj) {
        original_send_draw_task_event = lv_obj_has_flag(obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
        lv_obj_remove_flag(obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);
    }

    switch(task->type) {
        case LV_DRAW_TASK_TYPE_FILL: {
                lv_draw_fill_dsc_t * fill_dsc = task->draw_dsc;
                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.base.user_data = (void *)(uintptr_t)1;
                rect_dsc.bg_color = fill_dsc->color;
                rect_dsc.bg_grad = fill_dsc->grad;
                rect_dsc.radius = fill_dsc->radius;
                rect_dsc.bg_opa = fill_dsc->opa;

                lv_draw_rect(&dest_layer, &rect_dsc, &task->area);
            }
            break;
        case LV_DRAW_TASK_TYPE_BORDER: {
                lv_draw_border_dsc_t * border_dsc = task->draw_dsc;
                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.base.user_data = (void *)(uintptr_t)1;
                rect_dsc.bg_opa = LV_OPA_TRANSP;
                rect_dsc.radius = border_dsc->radius;
                rect_dsc.border_color = border_dsc->color;
                rect_dsc.border_opa = border_dsc->opa;
                rect_dsc.border_side = border_dsc->side;
                rect_dsc.border_width = border_dsc->width;
                lv_draw_rect(&dest_layer, &rect_dsc, &task->area);
                break;
            }
        case LV_DRAW_TASK_TYPE_BOX_SHADOW: {
                lv_draw_box_shadow_dsc_t * box_shadow_dsc = task->draw_dsc;
                lv_draw_rect_dsc_t rect_dsc;
                lv_draw_rect_dsc_init(&rect_dsc);
                rect_dsc.base.user_data = (void *)(uintptr_t)1;
                rect_dsc.bg_opa = LV_OPA_0;
                rect_dsc.radius = box_shadow_dsc->radius;
                rect_dsc.bg_color = box_shadow_dsc->color;
                rect_dsc.shadow_opa = box_shadow_dsc->opa;
                rect_dsc.shadow_width = box_shadow_dsc->width;
                rect_dsc.shadow_spread = box_shadow_dsc->spread;
                rect_dsc.shadow_offset_x = box_shadow_dsc->ofs_x;
                rect_dsc.shadow_offset_y = box_shadow_dsc->ofs_y;
                lv_draw_rect(&dest_layer, &rect_dsc, &task->area);
                break;
            }
        case LV_DRAW_TASK_TYPE_LABEL: {
                lv_draw_label_dsc_t label_dsc;
                lv_memcpy(&label_dsc, task->draw_dsc, sizeof(label_dsc));
                label_dsc.base.user_data = (void *)(uintptr_t)1;
                lv_draw_label(&dest_layer, &label_dsc, &task->area);
            }
            break;
        case LV_DRAW_TASK_TYPE_ARC: {
                lv_draw_arc_dsc_t arc_dsc;
                lv_memcpy(&arc_dsc, task->draw_dsc, sizeof(arc_dsc));
                arc_dsc.base.user_data = (void *)(uintptr_t)1;
                lv_draw_arc(&dest_layer, &arc_dsc);
            }
            break;
        case LV_DRAW_TASK_TYPE_LINE: {
                lv_draw_line_dsc_t line_dsc;
                lv_memcpy(&line_dsc, task->draw_dsc, sizeof(line_dsc));
                line_dsc.base.user_data = (void *)(uintptr_t)1;
                lv_draw_line(&dest_layer, &line_dsc);
            }
            break;
        case LV_DRAW_TASK_TYPE_TRIANGLE: {
                lv_draw_triangle_dsc_t triangle_dsc;
                lv_memcpy(&triangle_dsc, task->draw_dsc, sizeof(triangle_dsc));
                triangle_dsc.base.user_data = (void *)(uintptr_t)1;
                lv_draw_triangle(&dest_layer, &triangle_dsc);
            }
            break;
        case LV_DRAW_TASK_TYPE_IMAGE: {
                lv_draw_image_dsc_t image_dsc;
                lv_memcpy(&image_dsc, task->draw_dsc, sizeof(image_dsc));
                image_dsc.base.user_data = (void *)(uintptr_t)1;
                lv_draw_image(&dest_layer, &image_dsc, &task->area);
                break;
            }
        default:
            return false;
    }

    while(dest_layer.draw_task_head) {
        lv_draw_dispatch_layer(disp, &dest_layer);
        if(dest_layer.draw_task_head) {
            lv_draw_dispatch_wait_for_request();
        }
    }

    // unsigned int texture = SDL_CreateTexture(lv_sdl_window_get_renderer(disp), SDL_PIXELFORMAT_ARGB8888,
    //                                           SDL_TEXTUREACCESS_STATIC, texture_w, texture_h);
    // SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    // SDL_UpdateTexture(texture, NULL, opengles_render_buf, texture_w * 4);

    unsigned int texture;

    GL_CALL(glGenTextures(1, &texture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    /*Color depth: 8 (L8), 16 (RGB565), 24 (RGB888), 32 (XRGB8888)*/
#if LV_COLOR_DEPTH == 8
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texture_w, texture_h, 0, GL_RED, GL_UNSIGNED_BYTE, opengles_render_buf));
#elif LV_COLOR_DEPTH == 16
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, texture_w, texture_h, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                         opengles_render_buf));
#elif LV_COLOR_DEPTH == 24
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture_w, texture_h, 0, GL_BGR, GL_UNSIGNED_BYTE, opengles_render_buf));
#elif LV_COLOR_DEPTH == 32
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_w, texture_h, 0, GL_BGRA, GL_UNSIGNED_BYTE,
                         opengles_render_buf));
#else
#error("Unsupported color format")
#endif

    glGenerateMipmap(GL_TEXTURE_2D);

    lv_draw_dsc_base_t * base_dsc = task->draw_dsc;

    cache_data->draw_dsc = lv_malloc(base_dsc->dsc_size);
    lv_memcpy((void *)cache_data->draw_dsc, base_dsc, base_dsc->dsc_size);
    cache_data->w = texture_w;
    cache_data->h = texture_h;
    cache_data->texture = texture;

    if(obj) {
        lv_obj_update_flag(obj, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS, original_send_draw_task_event);
    }

    return true;
}

static void blend_texture_layer(lv_draw_opengles_unit_t * u)
{
    // lv_display_t * disp = lv_refr_get_disp_refreshing();
    // SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);

    // SDL_Rect clip_rect;
    // clip_rect.x = u->base_unit.clip_area->x1;
    // clip_rect.y = u->base_unit.clip_area->y1;
    // clip_rect.w = lv_area_get_width(u->base_unit.clip_area);
    // clip_rect.h = lv_area_get_height(u->base_unit.clip_area);

    // lv_draw_task_t * t = u->task_act;
    // lv_draw_image_dsc_t * draw_dsc = t->draw_dsc;
    // SDL_Rect rect;
    // rect.w = (lv_area_get_width(&t->area) * draw_dsc->scale_x) / 256;
    // rect.h = (lv_area_get_height(&t->area) * draw_dsc->scale_y) / 256;

    // rect.x = -draw_dsc->pivot.x;
    // rect.y = -draw_dsc->pivot.y;
    // rect.x = (rect.x * draw_dsc->scale_x) / 256;
    // rect.y = (rect.y * draw_dsc->scale_y) / 256;
    // rect.x += t->area.x1 + draw_dsc->pivot.x;
    // rect.y += t->area.y1 + draw_dsc->pivot.y;

    // lv_layer_t * src_layer = (lv_layer_t *)draw_dsc->src;
    // SDL_Texture * src_texture = layer_get_texture(src_layer);

    // SDL_SetTextureAlphaMod(src_texture, draw_dsc->opa);
    // SDL_SetTextureBlendMode(src_texture, SDL_BLENDMODE_BLEND);
    // SDL_SetRenderTarget(renderer, layer_get_texture(u->base_unit.target_layer));
    // SDL_RenderSetClipRect(renderer, &clip_rect);

    // SDL_Point center = {draw_dsc->pivot.x, draw_dsc->pivot.y};
    // SDL_RenderCopyEx(renderer, src_texture, NULL, &rect, draw_dsc->rotation / 10, &center, SDL_FLIP_NONE);
    // //    SDL_RenderCopy(renderer, src_texture, NULL, &rect);

    // SDL_DestroyTexture(src_texture);
    // SDL_RenderSetClipRect(renderer, NULL);


    lv_area_t clip_area = *u->base_unit.clip_area;

    lv_draw_task_t * t = u->task_act;
    lv_draw_image_dsc_t * draw_dsc = t->draw_dsc;

    lv_area_t area;
    area.x1 = -draw_dsc->pivot.x;
    area.y1 = -draw_dsc->pivot.y;
    area.x1 = (area.x1 * draw_dsc->scale_x) / 256;
    area.y1 = (area.y1 * draw_dsc->scale_y) / 256;
    area.x1 += t->area.x1 + draw_dsc->pivot.x;
    area.y1 += t->area.y1 + draw_dsc->pivot.y;
    lv_area_set_width(&area, lv_area_get_width(&t->area) * draw_dsc->scale_x / 256);
    lv_area_set_height(&area, lv_area_get_height(&t->area) * draw_dsc->scale_y / 256);

    lv_layer_t * src_layer = (lv_layer_t *)draw_dsc->src;
    unsigned int src_texture = layer_get_texture(src_layer);


    lv_layer_t * dest_layer = u->base_unit.target_layer;
    unsigned int target_texture = layer_get_texture(dest_layer);
    LV_ASSERT(target_texture != 0);
    int32_t targ_tex_w = lv_area_get_width(&dest_layer->buf_area);
    int32_t targ_tex_h = lv_area_get_height(&dest_layer->buf_area);

    GL_CALL(glBindTexture(GL_TEXTURE_2D, target_texture));

    unsigned int framebuffer_gl;
    GL_CALL(glGenFramebuffers(1, &framebuffer_gl));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_gl));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_texture, 0));

    lv_opengles_viewport(0, 0, targ_tex_w, targ_tex_h);
    // TODO rotation
    // TODO blending looks wrong
    lv_opengles_render_texture(src_texture, &area, draw_dsc->opa, targ_tex_w, targ_tex_h, &clip_area, false);

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glDeleteFramebuffers(1, &framebuffer_gl));


    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    GL_CALL(glDeleteTextures(1, &src_texture));
}

static void draw_from_cached_texture(lv_draw_opengles_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;

    cache_data_t data_to_find;
    data_to_find.draw_dsc = (lv_draw_dsc_base_t *)t->draw_dsc;

    data_to_find.w = lv_area_get_width(&t->_real_area);
    data_to_find.h = lv_area_get_height(&t->_real_area);
    data_to_find.texture = 0;

    /*user_data stores the renderer to differentiate it from SW rendered tasks.
     *However the cached texture is independent from the renderer so use NULL user_data*/
    void * user_data_saved = data_to_find.draw_dsc->user_data;
    data_to_find.draw_dsc->user_data = NULL;

    /*img_dsc->image_area is an absolute coordinate so it's different
     *for the same image on a different position. So make it relative before using for cache. */
    if(t->type == LV_DRAW_TASK_TYPE_IMAGE) {
        lv_draw_image_dsc_t * img_dsc = (lv_draw_image_dsc_t *)data_to_find.draw_dsc;
        lv_area_move(&img_dsc->image_area, -t->area.x1, -t->area.y1);
    }

    lv_cache_entry_t * entry_cached = lv_cache_acquire_or_create(u->texture_cache, &data_to_find, u);
    if(t->type == LV_DRAW_TASK_TYPE_IMAGE) {
        lv_draw_image_dsc_t * img_dsc = (lv_draw_image_dsc_t *)data_to_find.draw_dsc;
        lv_area_move(&img_dsc->image_area, t->area.x1, t->area.y1);
    }

    if(!entry_cached) {
        return;
    }


    data_to_find.draw_dsc->user_data = user_data_saved;

    cache_data_t * data_cached = lv_cache_entry_get_data(entry_cached);
    unsigned int texture = data_cached->texture;
    // lv_display_t * disp = lv_refr_get_disp_refreshing();
    // SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);

    lv_layer_t * dest_layer = u->base_unit.target_layer;
    // SDL_Rect clip_rect;
    // clip_rect.x = u->base_unit.clip_area->x1 - dest_layer->buf_area.x1;
    // clip_rect.y = u->base_unit.clip_area->y1 - dest_layer->buf_area.y1;
    // clip_rect.w = lv_area_get_width(u->base_unit.clip_area);
    // clip_rect.h = lv_area_get_height(u->base_unit.clip_area);

    // SDL_Rect rect;

    // SDL_SetRenderTarget(renderer, layer_get_texture(dest_layer));

    // rect.x = t->_real_area.x1 - dest_layer->buf_area.x1;
    // rect.y = t->_real_area.y1 - dest_layer->buf_area.y1;
    // rect.w = data_cached->w;
    // rect.h = data_cached->h;

    // SDL_RenderSetClipRect(renderer, &clip_rect);
    // SDL_RenderCopy(renderer, texture, NULL, &rect);

    // SDL_RenderSetClipRect(renderer, NULL);


    unsigned int target_texture = layer_get_texture(dest_layer);
    LV_ASSERT(target_texture != 0);
    int32_t targ_tex_w = lv_area_get_width(&dest_layer->buf_area);
    int32_t targ_tex_h = lv_area_get_height(&dest_layer->buf_area);


    // int32_t src_x = 0;
    // int32_t src_y = 0;

    // int32_t dst_x = u->base_unit.clip_area->x1;
    // int32_t dst_y = u->base_unit.clip_area->y1;

    // int32_t width = lv_area_get_width(u->base_unit.clip_area);
    // int32_t height = lv_area_get_height(u->base_unit.clip_area);

    // int32_t dst_x = t->_real_area.x1;
    // int32_t dst_y = t->_real_area.y1;

    // int32_t width = lv_area_get_width(&t->_real_area);
    // int32_t height = lv_area_get_height(&t->_real_area);


    // lv_area_t a_disp = {0};
    // lv_area_set_width(&a_disp, lv_display_get_horizontal_resolution(disp));
    // lv_area_set_height(&a_disp, lv_display_get_vertical_resolution(disp));

    // lv_area_t a_intersect;
    // lv_area_intersect(&a_intersect, &t->_real_area, &a_disp);

    // int32_t dst_x = a_intersect.x1;
    // int32_t dst_y = a_intersect.y1;

    // int32_t width = lv_area_get_width(&a_intersect);
    // int32_t height = lv_area_get_height(&a_intersect);


    // int32_t dst_x = u->base_unit.clip_area->x1 - dest_layer->buf_area.x1;
    // int32_t dst_y = u->base_unit.clip_area->y1 - dest_layer->buf_area.y1;

    // int32_t width = lv_area_get_width(u->base_unit.clip_area);
    // int32_t height = lv_area_get_height(u->base_unit.clip_area);

    // /*                        (GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ,
    //                            GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ,
    //                            GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth); */
    // GL_CALL(glCopyImageSubData(texture,      GL_TEXTURE_2D, 0, src_x, src_y, 0,
    //                            target_texture, GL_TEXTURE_2D, 0, dst_x, dst_y, 0,
    //                            width, height, 1));


    GL_CALL(glBindTexture(GL_TEXTURE_2D, target_texture));

    unsigned int framebuffer_gl;
    GL_CALL(glGenFramebuffers(1, &framebuffer_gl));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_gl));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_texture, 0));

    lv_opengles_viewport(0, 0, targ_tex_w, targ_tex_h);
    lv_area_t clip_area = *u->base_unit.clip_area;
    lv_area_move(&clip_area, -dest_layer->buf_area.x1, -dest_layer->buf_area.y1);
    lv_area_t render_area = t->_real_area;
    lv_area_move(&render_area, -dest_layer->buf_area.x1, -dest_layer->buf_area.y1);
    lv_opengles_render_texture(texture, &render_area, 0xff, targ_tex_w, targ_tex_h, &clip_area, true);

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glDeleteFramebuffers(1, &framebuffer_gl));

    lv_cache_release(u->texture_cache, entry_cached, u);

    /*Do not cache label's with local text as the text will be freed*/
    if(t->type == LV_DRAW_TASK_TYPE_LABEL) {
        lv_draw_label_dsc_t * label_dsc = t->draw_dsc;
        if(label_dsc->text_local) {
            lv_cache_drop(u->texture_cache, &data_to_find, NULL);
        }
    }
}

static void execute_drawing(lv_draw_opengles_unit_t * u)
{
    lv_draw_task_t * t = u->task_act;

    // if(t->type == LV_DRAW_TASK_TYPE_FILL) {
    //     lv_draw_fill_dsc_t * fill_dsc = t->draw_dsc;
    //     if(fill_dsc->radius == 0 && fill_dsc->grad.dir == LV_GRAD_DIR_NONE) {
    //         SDL_Rect rect;
    //         lv_layer_t * layer = u->base_unit.target_layer;
    //         lv_area_t fill_area = t->area;
    //         lv_area_intersect(&fill_area, &fill_area, u->base_unit.clip_area);

    //         rect.x = fill_area.x1 - layer->buf_area.x1;
    //         rect.y = fill_area.y1 - layer->buf_area.y1;
    //         rect.w = lv_area_get_width(&fill_area);
    //         rect.h = lv_area_get_height(&fill_area);
    //         lv_display_t * disp = lv_refr_get_disp_refreshing();
    //         SDL_Renderer * renderer = lv_sdl_window_get_renderer(disp);
    //         SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    //         SDL_SetRenderDrawColor(renderer, fill_dsc->color.red, fill_dsc->color.green, fill_dsc->color.blue, fill_dsc->opa);
    //         SDL_RenderSetClipRect(renderer, NULL);
    //         SDL_RenderFillRect(renderer, &rect);
    //         return;
    //     }
    // }

    if(t->type == LV_DRAW_TASK_TYPE_LAYER) {
        blend_texture_layer(u);
        return;
    }

    draw_from_cached_texture(u);
}

static unsigned int layer_get_texture(lv_layer_t * layer)
{
    return (unsigned int)(uintptr_t)layer->user_data;
}

#endif /*LV_USE_DRAW_OPENGLES*/
