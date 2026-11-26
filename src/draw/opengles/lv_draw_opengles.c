/**
 * @file lv_draw_opengles.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_opengles.h"
#if LV_USE_DRAW_OPENGLES

#if LV_USE_DRAW_NANOVG
    #error "LV_USE_DRAW_NANOVG and LV_USE_DRAW_OPENGLES cannot be enabled at the same time. Disable one of them in lv_conf.h or Kconfig."
#endif

#include "../lv_draw_private.h"
#include "../sw/lv_draw_sw_private.h"
#include "../lv_draw_buf_private.h"
#include "../../misc/cache/lv_cache_entry_private.h"
#include "../lv_draw_vector_private.h"
#include "../../drivers/opengles/lv_opengles_debug.h"
#include "../../drivers/opengles/lv_opengles_private.h"
#include "../../core/lv_refr_private.h"
#include "../../display/lv_display_private.h"
#include "../../misc/lv_area_private.h"
#include "../lv_draw_image_private.h"

/*********************
 *      DEFINES
 *********************/

#define DRAW_UNIT_ID_OPENGLES 6

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_draw_unit_t base_unit;
    lv_cache_t * texture_cache;
    unsigned int framebuffer;
    lv_draw_buf_t render_draw_buf;
} lv_draw_opengles_unit_t;

typedef struct {
    lv_draw_dsc_t * draw_dsc;
    int32_t w;
    int32_t h;
    unsigned int texture;
} cache_data_t;

typedef struct {
    lv_draw_opengles_unit_t * u;
    lv_area_t coords;       /*Relative to `real_area`'s origin*/
    lv_area_t real_area;    /*Relative, (0;0) based*/
} texture_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static bool opengles_texture_cache_create_cb(cache_data_t * cached_data, void * user_data);
static void opengles_texture_cache_free_cb(cache_data_t * cached_data, void * user_data);
static lv_cache_compare_res_t opengles_texture_cache_compare_cb(const cache_data_t * lhs, const cache_data_t * rhs);

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);

static bool opengles_takes(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_dsc_t * dsc);
static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc);
static bool can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc);
static bool can_draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc);
static bool can_draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_letter_dsc_t * dsc);
static bool can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc);
static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc);
static bool can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc);
static bool can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc);
static bool can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc);
static bool can_draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc);
static bool can_draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc);
#if LV_USE_VECTOR_GRAPHIC
    static bool can_draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_vector_dsc_t * dsc);
#endif
#if LV_USE_3DTEXTURE
    static bool can_draw_3d(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_3d_dsc_t * dsc);
#endif
static unsigned int draw_to_texture(texture_data_t * tex_data, const lv_draw_dsc_t * dsc, cache_data_t * cache_data);
static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords);
static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords);
static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords);
static void draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords);
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
static void draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_mask_rect_dsc_t * dsc);
static void draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords);
#if LV_USE_VECTOR_GRAPHIC
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc);
#endif

static unsigned int layer_get_texture(lv_layer_t * layer);
static void gpu_buf_clear_cb(lv_draw_buf_t * draw_buf, const lv_area_t * a, lv_layer_t * layer);
static unsigned int get_framebuffer(lv_draw_opengles_unit_t * u);
static unsigned int create_texture(int32_t w, int32_t h, const void * data);

#if LV_USE_3DTEXTURE
static void draw_3d(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                    const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

static lv_draw_opengles_unit_t * g_unit;
static lv_draw_buf_handlers_t gpu_draw_buf_handlers;

static const lv_draw_unit_draw_cbs_t draw_cbs = {
    .draw_fill_cb = draw_fill,
    .can_draw_fill_cb = can_draw_fill,
    .draw_border_cb = draw_border,
    .can_draw_border_cb = can_draw_border,
    .draw_box_shadow_cb = draw_box_shadow,
    .can_draw_box_shadow_cb = can_draw_box_shadow,
    .draw_letter_cb = draw_letter,
    .can_draw_letter_cb = can_draw_letter,
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
    .draw_mask_rect_cb = draw_mask_rect,
    .can_draw_mask_rect_cb = can_draw_mask_rect,
    .draw_blur_cb = draw_blur,
    .can_draw_blur_cb = can_draw_blur,
#if LV_USE_VECTOR_GRAPHIC
    .draw_vector_cb = draw_vector,
    .can_draw_vector_cb = can_draw_vector,
#endif
#if LV_USE_3DTEXTURE
    .draw_3d_cb = draw_3d,
    .can_draw_3d_cb = can_draw_3d,
#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_opengles_init(void)
{
    lv_draw_opengles_unit_t * draw_opengles_unit = lv_draw_create_unit(sizeof(lv_draw_opengles_unit_t));
    draw_opengles_unit->base_unit.dispatch_cb = dispatch;
    draw_opengles_unit->base_unit.draw_cbs = &draw_cbs;
    draw_opengles_unit->base_unit.fallback = lv_draw_sw_get_draw_unit();
    draw_opengles_unit->base_unit.id = DRAW_UNIT_ID_OPENGLES;
    draw_opengles_unit->base_unit.name = "OPENGLES";
    draw_opengles_unit->texture_cache = lv_cache_create(&lv_cache_class_lru_rb_count,
    sizeof(cache_data_t), LV_DRAW_OPENGLES_TEXTURE_CACHE_COUNT, (lv_cache_ops_t) {
        .compare_cb = (lv_cache_compare_cb_t)opengles_texture_cache_compare_cb,
        .create_cb = (lv_cache_create_cb_t)opengles_texture_cache_create_cb,
        .free_cb = (lv_cache_free_cb_t)opengles_texture_cache_free_cb,
    });
    lv_cache_set_name(draw_opengles_unit->texture_cache, "OPENGLES_TEXTURE");

    lv_draw_buf_init(&draw_opengles_unit->render_draw_buf, 0, 0, LV_COLOR_FORMAT_ARGB8888, LV_STRIDE_AUTO, NULL, 0);

    lv_draw_buf_init_with_default_handlers(&gpu_draw_buf_handlers);
    gpu_draw_buf_handlers.buf_clear_cb = gpu_buf_clear_cb;

    g_unit = draw_opengles_unit;
}

void lv_draw_opengles_deinit(void)
{
    lv_free(g_unit->render_draw_buf.unaligned_data);
    lv_cache_destroy(g_unit->texture_cache, g_unit);
    if(g_unit->framebuffer != 0) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GL_CALL(glDeleteFramebuffers(1, &g_unit->framebuffer));
    }
    g_unit = NULL;
}

const lv_draw_buf_handlers_t * lv_draw_opengles_get_draw_buf_handlers(void)
{
    return &gpu_draw_buf_handlers;
}

static void gpu_buf_clear_cb(lv_draw_buf_t * draw_buf, const lv_area_t * a, lv_layer_t * layer)
{
    if(layer == NULL) return;

    /*Tiled rendering uses temporary tile layers that have no texture of their own.
     *They render into the display's backing layer, so clear that one instead.*/
    if(layer->user_data == NULL) {
        lv_display_t * disp = lv_refr_get_disp_refreshing();
        if(disp == NULL || disp->layer_head == NULL) return;
        layer = disp->layer_head;
    }

    /*`a` is relative to the draw buffer, `lv_draw_opengles_clear_layer_area()` takes
     *screen coordinates.*/
    lv_area_t screen_area;
    if(a == NULL) {
        screen_area = layer->buf_area;
    }
    else {
        screen_area = *a;
        lv_area_move(&screen_area, layer->buf_area.x1, layer->buf_area.y1);
    }

    LV_UNUSED(draw_buf);
    lv_draw_opengles_clear_layer_area(layer, &screen_area);
}

void lv_draw_opengles_clear_layer_area(lv_layer_t * layer, const lv_area_t * area)
{
    if(g_unit == NULL) return;
    unsigned int target_texture = layer_get_texture(layer);
    if(target_texture == 0) return;

    lv_area_t a;
    if(!lv_area_intersect(&a, area, &layer->_clip_area)) return;
    lv_area_move(&a, -layer->buf_area.x1, -layer->buf_area.y1);

    int32_t w = lv_area_get_width(&a);
    int32_t h = lv_area_get_height(&a);
    if(w <= 0 || h <= 0) return;

    int32_t tex_h = lv_area_get_height(&layer->buf_area);

    GLint prev_fbo = 0;
    GLint prev_scissor_box[4] = {0};
    GLfloat prev_clear_color[4] = {0};
    GLboolean prev_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
    GL_CALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prev_fbo));
    GL_CALL(glGetIntegerv(GL_SCISSOR_BOX, prev_scissor_box));
    GL_CALL(glGetFloatv(GL_COLOR_CLEAR_VALUE, prev_clear_color));

    unsigned int framebuffer = get_framebuffer(g_unit);
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_texture, 0));

    GL_CALL(glEnable(GL_SCISSOR_TEST));
    GL_CALL(glScissor(a.x1, tex_h - a.y1 - h, w, h));
    GL_CALL(glClearColor(0.0f, 0.0f, 0.0f, 0.0f));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT));

    /*Restore previous GL state.*/
    if(!prev_scissor_test) {
        GL_CALL(glDisable(GL_SCISSOR_TEST));
    }
    GL_CALL(glScissor(prev_scissor_box[0], prev_scissor_box[1], prev_scissor_box[2], prev_scissor_box[3]));
    GL_CALL(glClearColor(prev_clear_color[0], prev_clear_color[1], prev_clear_color[2], prev_clear_color[3]));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)prev_fbo));
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static bool opengles_texture_cache_create_cb(cache_data_t * cached_data, void * user_data)
{
    LV_PROFILER_DRAW_BEGIN;
    bool ret = draw_to_texture(user_data, cached_data->draw_dsc, cached_data) != 0;
    LV_PROFILER_DRAW_END;
    return ret;
}

static void opengles_texture_cache_free_cb(cache_data_t * cached_data, void * user_data)
{
    LV_UNUSED(user_data);
    LV_PROFILER_DRAW_BEGIN;

    lv_free(cached_data->draw_dsc);
    GL_CALL(glDeleteTextures(1, &cached_data->texture));
    cached_data->draw_dsc = NULL;
    cached_data->texture = 0;
    LV_PROFILER_DRAW_END;
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

static int32_t dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, DRAW_UNIT_ID_OPENGLES);
    if(t == NULL) return -1;

    unsigned int texture = layer_get_texture(layer);
    if(texture == 0) {
        lv_display_t * disp = lv_refr_get_disp_refreshing();
        LV_ASSERT(layer != disp->layer_head);
        int32_t w = lv_area_get_width(&layer->buf_area);
        int32_t h = lv_area_get_height(&layer->buf_area);

        texture = create_texture(w, h, NULL);
        layer->user_data = (void *)(uintptr_t)texture;
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;
    lv_draw_unit_draw_task(draw_unit, t);
    t->state = LV_DRAW_TASK_STATE_FINISHED;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();
    return 1;
}

static bool opengles_takes(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(dsc);

    /*If not refreshing the display probably it's a canvas rendering
     *which his not supported in OpenGL as it's not a texture.*/
    return lv_refr_get_disp_refreshing() != NULL;
}

static bool can_draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_fill_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_border_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_box_shadow_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_letter_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_label_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_image_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_arc_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_line(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_line_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_triangle(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_triangle_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_mask_rect_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

static bool can_draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_blur_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}

#if LV_USE_VECTOR_GRAPHIC
static bool can_draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_vector_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}
#endif

#if LV_USE_3DTEXTURE
static bool can_draw_3d(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_draw_3d_dsc_t * dsc)
{
    return opengles_takes(draw_unit, layer, &dsc->base);
}
#endif

static unsigned int draw_to_texture(texture_data_t * tex_data, const lv_draw_dsc_t * dsc, cache_data_t * cache_data)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_opengles_unit_t * u = tex_data->u;
    const lv_area_t * real_area = &tex_data->real_area;
    const lv_area_t * coords = &tex_data->coords;

    int32_t texture_w = lv_area_get_width(real_area);
    int32_t texture_h = lv_area_get_height(real_area);

    if(NULL == lv_draw_buf_reshape(&u->render_draw_buf, LV_COLOR_FORMAT_ARGB8888, texture_w, texture_h, LV_STRIDE_AUTO)) {
        uint8_t * data = u->render_draw_buf.unaligned_data;
        uint32_t data_size = LV_DRAW_BUF_SIZE(texture_w, texture_h, LV_COLOR_FORMAT_ARGB8888);
        data = lv_realloc(data, data_size);
        LV_ASSERT_MALLOC(data);
        lv_result_t init_result = lv_draw_buf_init(&u->render_draw_buf, texture_w, texture_h, LV_COLOR_FORMAT_ARGB8888,
                                                   LV_STRIDE_AUTO, data, data_size);
        LV_ASSERT(init_result == LV_RESULT_OK);
    }

    lv_layer_t dest_layer;
    lv_layer_init(&dest_layer);
    dest_layer.draw_buf = &u->render_draw_buf;
    dest_layer.color_format = LV_COLOR_FORMAT_ARGB8888;
    dest_layer.buf_area = *real_area;
    dest_layer._clip_area = *real_area;
    dest_layer.phy_clip_area = *real_area;
    lv_memzero(u->render_draw_buf.data, lv_area_get_size(real_area) * 4);

    if(cache_data != NULL) {
        cache_data->draw_dsc = lv_malloc(dsc->dsc_size);
        LV_ASSERT_MALLOC(cache_data->draw_dsc);
        if(cache_data->draw_dsc == NULL) {
            LV_PROFILER_DRAW_END;
            return 0;
        }
        lv_memcpy((void *)cache_data->draw_dsc, dsc, dsc->dsc_size);
    }

    /*Draw with the SW leaves: no front end, so no draw task and no draw events*/
    switch(dsc->type) {
        case LV_DRAW_DSC_TYPE_FILL:
            lv_draw_sw_fill(&dest_layer, real_area, (const lv_draw_fill_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_BORDER:
            lv_draw_sw_border(&dest_layer, real_area, (const lv_draw_border_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_BOX_SHADOW:
            lv_draw_sw_box_shadow(&dest_layer, real_area, (const lv_draw_box_shadow_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_LABEL:
            lv_draw_sw_label(&dest_layer, real_area, (const lv_draw_label_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_ARC:
            lv_draw_sw_arc(&dest_layer, real_area, (const lv_draw_arc_dsc_t *)dsc, coords);
            break;
        case LV_DRAW_DSC_TYPE_LINE:
            lv_draw_line_iterate(&dest_layer, real_area, (const lv_draw_line_dsc_t *)dsc, lv_draw_sw_line, NULL);
            break;
        case LV_DRAW_DSC_TYPE_TRIANGLE:
            lv_draw_sw_triangle(&dest_layer, real_area, (const lv_draw_triangle_dsc_t *)dsc);
            break;
        case LV_DRAW_DSC_TYPE_IMAGE:
            lv_draw_sw_image(&dest_layer, real_area, (const lv_draw_image_dsc_t *)dsc, coords);
            break;
        default:
            /*The malloced cache_data->draw_dsc will be freed automatically on failure
            *in opengles_texture_cache_free_cb*/
            LV_LOG_ERROR("OpenGLES draw unit does not support tasks of type %d", dsc->type);
            LV_PROFILER_DRAW_END;
            return 0;
    }

    unsigned int texture = create_texture(texture_w, texture_h, u->render_draw_buf.data);

    if(cache_data != NULL) {
        cache_data->w = texture_w;
        cache_data->h = texture_h;
        cache_data->texture = texture;
    }

    LV_PROFILER_DRAW_END;
    return texture;
}

static void draw_texture_to_framebuffer(lv_draw_opengles_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                                        unsigned int texture, const lv_area_t * real_area)
{
    unsigned int target_texture = layer_get_texture(layer);
    int32_t targ_tex_w = lv_area_get_width(&layer->buf_area);
    int32_t targ_tex_h = lv_area_get_height(&layer->buf_area);

    if(target_texture) {
        unsigned int framebuffer = get_framebuffer(u);
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_texture, 0));
    }

    lv_opengles_viewport(0, 0, targ_tex_w, targ_tex_h);
    lv_area_t clip = *clip_area;
    lv_area_move(&clip, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_area_t render_area = *real_area;
    lv_area_move(&render_area, -layer->buf_area.x1, -layer->buf_area.y1);
    lv_opengles_render_texture_internal(texture, &render_area, 0xff, targ_tex_w, targ_tex_h, &clip, false, false);

    if(target_texture) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
}

/**
 * Draw `key` through a texture rendered with SW. With `cacheable` the texture is kept in the texture cache,
 * otherwise it's deleted after drawing (e.g. the dsc points to memory that can change or be freed).
 * `key` is a copy of the dsc with its geometry relative to `coords`. `real_area` is the absolute area the
 * texture covers.
 */
static void draw_cached(lv_draw_opengles_unit_t * u, lv_layer_t * layer, const lv_area_t * clip_area,
                        lv_draw_dsc_t * key, const lv_area_t * coords, const lv_area_t * real_area, bool cacheable)
{
    LV_PROFILER_DRAW_BEGIN;

    /*Absolute coordinates differ for the same dsc on a different position, so render relative to (0;0)*/
    texture_data_t tex_data;
    tex_data.u = u;
    tex_data.coords = *coords;
    tex_data.real_area = *real_area;
    lv_area_move(&tex_data.coords, -coords->x1, -coords->y1);
    lv_area_move(&tex_data.real_area, -coords->x1, -coords->y1);

    if(!cacheable) {
        unsigned int texture = draw_to_texture(&tex_data, key, NULL);
        if(texture != 0) {
            draw_texture_to_framebuffer(u, layer, clip_area, texture, real_area);
            GL_CALL(glDeleteTextures(1, &texture));
        }
        LV_PROFILER_DRAW_END;
        return;
    }

    cache_data_t data_to_find;
    data_to_find.draw_dsc = key;
    data_to_find.w = lv_area_get_width(real_area);
    data_to_find.h = lv_area_get_height(real_area);
    data_to_find.texture = 0;

    lv_cache_entry_t * entry_cached = lv_cache_acquire_or_create(u->texture_cache, &data_to_find, &tex_data);
    if(!entry_cached) {
        LV_PROFILER_DRAW_END;
        return;
    }

    cache_data_t * data_cached = lv_cache_entry_get_data(entry_cached);
    draw_texture_to_framebuffer(u, layer, clip_area, data_cached->texture, real_area);

    lv_cache_release(u->texture_cache, entry_cached, u);

    LV_PROFILER_DRAW_END;
}

static void draw_fill(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_opengles_unit_t * u = (lv_draw_opengles_unit_t *)draw_unit;

    /* the shader-based fill is not working reliably with EGL. */
    if(dsc->radius == 0 && dsc->grad.dir == LV_GRAD_DIR_NONE) {
        lv_area_t fill_area = *coords;
        lv_area_intersect(&fill_area, &fill_area, clip_area);
        lv_area_move(&fill_area, -layer->buf_area.x1, -layer->buf_area.y1);

        unsigned int target_texture = layer_get_texture(layer);
        int32_t targ_tex_w = lv_area_get_width(&layer->buf_area);
        int32_t targ_tex_h = lv_area_get_height(&layer->buf_area);

        if(target_texture) {
            unsigned int framebuffer = get_framebuffer(u);
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
            GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_texture, 0));
        }

        if(dsc->opa >= LV_OPA_MAX) {
            int32_t tex_w = lv_area_get_width(&fill_area);
            int32_t tex_h = lv_area_get_height(&fill_area);
            GL_CALL(glEnable(GL_SCISSOR_TEST));
            GL_CALL(glScissor(fill_area.x1, targ_tex_h - fill_area.y1 - tex_h, tex_w, tex_h));
            /* swap red and blue channels here as they will be swapped back during flushing*/
            GL_CALL(glClearColor((float)dsc->color.blue / 255.0f, (float)dsc->color.green / 255.0f,
                                 (float)dsc->color.red / 255.0f, 1.0f));
            GL_CALL(glClearDepthf(1.0f));
            GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
            GL_CALL(glDisable(GL_SCISSOR_TEST));
        }
        else {
            lv_opengles_viewport(0, 0, targ_tex_w, targ_tex_h);
            lv_opengles_render_fill(dsc->color, &fill_area, dsc->opa, targ_tex_w, targ_tex_h);
        }

        if(target_texture) {
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        }
        return;
    }

    lv_draw_fill_dsc_t key = *dsc;
    draw_cached(u, layer, clip_area, &key.base, coords, coords, true);
}

static void draw_border(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_border_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_border_dsc_t key = *dsc;
    draw_cached((lv_draw_opengles_unit_t *)draw_unit, layer, clip_area, &key.base, coords, coords, true);
}

static void draw_box_shadow(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                            const lv_draw_box_shadow_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_box_shadow_dsc_t key = *dsc;

    lv_area_t real_area = *coords;
    lv_area_increase(&real_area, dsc->spread, dsc->spread);
    lv_area_increase(&real_area, dsc->width, dsc->width);
    lv_area_move(&real_area, dsc->ofs_x, dsc->ofs_y);

    draw_cached((lv_draw_opengles_unit_t *)draw_unit, layer, clip_area, &key.base, coords, &real_area, true);
}

static void draw_label(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_label_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_label_dsc_t key = *dsc;

    /*Do not cache non static (const) texts as the text's pointer can be freed/reallocated
     *at any time resulting in a wild pointer in the cached draw dsc. */
    draw_cached((lv_draw_opengles_unit_t *)draw_unit, layer, clip_area, &key.base, coords, coords, dsc->text_static);
}

static void draw_image(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_image_dsc_t key = *dsc;
    lv_area_move(&key.image_area, -coords->x1, -coords->y1);

    lv_area_t real_area;
    lv_draw_image_get_transformed_area(&real_area, dsc, coords);

    /*Do not cache modifiable images as they might change in the next frame
     *resulting in stale textures in the cache. */
    bool cacheable = !(dsc->header.flags & LV_IMAGE_FLAGS_MODIFIABLE);
    draw_cached((lv_draw_opengles_unit_t *)draw_unit, layer, clip_area, &key.base, coords, &real_area, cacheable);
}

static void draw_layer(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                       const lv_draw_image_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_opengles_unit_t * u = (lv_draw_opengles_unit_t *)draw_unit;
    lv_area_t area;
    area.x1 = -dsc->pivot.x;
    area.y1 = -dsc->pivot.y;
    area.x1 = (area.x1 * dsc->scale_x) / 256;
    area.y1 = (area.y1 * dsc->scale_y) / 256;
    area.x1 += coords->x1 + dsc->pivot.x;
    area.y1 += coords->y1 + dsc->pivot.y;
    lv_area_set_width(&area, lv_area_get_width(coords) * dsc->scale_x / 256);
    lv_area_set_height(&area, lv_area_get_height(coords) * dsc->scale_y / 256);

    lv_layer_t * src_layer = (lv_layer_t *)dsc->src;
    unsigned int src_texture = layer_get_texture(src_layer);

    unsigned int target_texture = layer_get_texture(layer);
    int32_t targ_tex_w = lv_area_get_width(&layer->buf_area);
    int32_t targ_tex_h = lv_area_get_height(&layer->buf_area);

    if(target_texture) {
        unsigned int framebuffer = get_framebuffer(u);
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_texture, 0));
    }

    lv_opengles_viewport(0, 0, targ_tex_w, targ_tex_h);
    // TODO rotation
    lv_opengles_render_texture_internal(src_texture, &area, dsc->opa, targ_tex_w, targ_tex_h, clip_area, false, true);

    if(target_texture) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }

    GL_CALL(glDeleteTextures(1, &src_texture));
    LV_PROFILER_DRAW_END;
}

static void draw_arc(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                     const lv_draw_arc_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_arc_dsc_t key = *dsc;
    key.center.x -= coords->x1;
    key.center.y -= coords->y1;

    draw_cached((lv_draw_opengles_unit_t *)draw_unit, layer, clip_area, &key.base, coords, coords, true);
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

    /*Do not cache lines rendered from points at dsc->points will be freed*/
    draw_cached((lv_draw_opengles_unit_t *)draw_unit, layer, clip_area, &key.base, &coords, &coords,
                dsc->points == NULL);
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

    draw_cached((lv_draw_opengles_unit_t *)draw_unit, layer, clip_area, &key.base, &coords, &coords, true);
}

/*Taken but not drawn: the target is a texture, so the SW fallback can't draw them either*/

static void drop_unsupported(const lv_draw_dsc_t * dsc)
{
    LV_LOG_ERROR("OpenGLES draw unit does not support tasks of type %d", dsc->type);
}

static void draw_letter(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_letter_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    LV_UNUSED(coords);
    drop_unsupported(&dsc->base);
}

static void draw_mask_rect(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                           const lv_draw_mask_rect_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    drop_unsupported(&dsc->base);
}

static void draw_blur(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                      const lv_draw_blur_dsc_t * dsc, const lv_area_t * coords)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    LV_UNUSED(coords);
    drop_unsupported(&dsc->base);
}

#if LV_USE_VECTOR_GRAPHIC
static void draw_vector(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                        const lv_draw_vector_dsc_t * dsc)
{
    LV_UNUSED(draw_unit);
    LV_UNUSED(layer);
    LV_UNUSED(clip_area);
    drop_unsupported(&dsc->base);
}
#endif

static unsigned int layer_get_texture(lv_layer_t * layer)
{
    return (unsigned int)(uintptr_t)layer->user_data;
}

static unsigned int get_framebuffer(lv_draw_opengles_unit_t * u)
{
    if(u->framebuffer == 0) {
        GL_CALL(glGenFramebuffers(1, &u->framebuffer));
    }
    return u->framebuffer;
}

static unsigned int create_texture(int32_t w, int32_t h, const void * data)
{
    LV_PROFILER_DRAW_BEGIN;
    unsigned int texture;
    GL_CALL(glGenTextures(1, &texture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    /* LV_COLOR_DEPTH 32, 16 are supported but the cached textures will always
     * have full ARGB pixels since the alpha channel is required for blending.
     */
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data));
#if 0
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 20));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    /* GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST));
     * Alternatively, the above form can be used in some cases for slightly faster performance, but
     * visual quality when using image scales that are not exactly 1:1 (or 2:1 or some other increment)
     * will be not as good.
     */
#endif

    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

    LV_PROFILER_DRAW_END;
    return texture;
}

#if LV_USE_3DTEXTURE
static void draw_3d(lv_draw_unit_t * draw_unit, lv_layer_t * layer, const lv_area_t * clip_area,
                    const lv_draw_3d_dsc_t * dsc, const lv_area_t * coords)
{
    LV_PROFILER_DRAW_BEGIN;
    lv_draw_opengles_unit_t * u = (lv_draw_opengles_unit_t *)draw_unit;

    unsigned int target_texture = layer_get_texture(layer);
    int32_t targ_tex_w = lv_area_get_width(&layer->buf_area);
    int32_t targ_tex_h = lv_area_get_height(&layer->buf_area);

    if(target_texture) {
        unsigned int framebuffer = get_framebuffer(u);
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, target_texture, 0));
    }

    lv_opengles_viewport(0, 0, targ_tex_w, targ_tex_h);
    lv_area_t clip = *clip_area;
    lv_area_move(&clip, -layer->buf_area.x1, -layer->buf_area.y1);

    lv_opengles_render_params_t params;
    lv_opengles_render_params_init(&params);
    params.texture = dsc->tex_id;
    params.texture_area = coords;
    params.opa = dsc->opa;
    params.disp_w = targ_tex_w;
    params.disp_h = targ_tex_h;
    params.texture_clip_area = &clip;
    params.h_flip = dsc->h_flip;
    params.v_flip = dsc->v_flip;
    params.blend_opt = true;
    lv_opengles_render(&params);

    if(target_texture) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    }
    LV_PROFILER_DRAW_END;
}
#endif /*LV_USE_3DTEXTURE*/

#endif /*LV_USE_DRAW_OPENGLES*/
