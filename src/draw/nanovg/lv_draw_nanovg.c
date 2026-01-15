/**
 * @file lv_draw_nanovg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg.h"

#if LV_USE_DRAW_NANOVG

#include "../../display/lv_display.h"
#include "../../core/lv_refr_private.h"
#include "lv_draw_nanovg_private.h"
#include "lv_nanovg_utils.h"
#include "lv_nanovg_image_cache.h"
#include "lv_nanovg_fbo_cache.h"

#if LV_USE_OPENGLES && LV_USE_EGL
    #include "../../drivers/opengles/lv_opengles_private.h"
#else
    #define NANOVG_GL_STATIC_LINK
#endif

#if defined(NANOVG_GL2_IMPLEMENTATION)
    #ifdef NANOVG_GL_STATIC_LINK
        #include <GL/glew.h>
    #endif
    #define NVG_CTX_CREATE nvgCreateGL2
    #define NVG_CTX_DELETE nvgDeleteGL2
#elif defined(NANOVG_GL3_IMPLEMENTATION)
    #ifdef NANOVG_GL_STATIC_LINK
        #include <GL/glew.h>
    #endif
    #define NVG_CTX_CREATE nvgCreateGL3
    #define NVG_CTX_DELETE nvgDeleteGL3
#elif defined(NANOVG_GLES2_IMPLEMENTATION)
    #ifdef NANOVG_GL_STATIC_LINK
        #include <GLES2/gl2.h>
    #endif
    #define NVG_CTX_CREATE nvgCreateGLES2
    #define NVG_CTX_DELETE nvgDeleteGLES2
#elif defined(NANOVG_GLES3_IMPLEMENTATION)
    #ifdef NANOVG_GL_STATIC_LINK
        #include <GLES3/gl3.h>
    #endif
    #define NVG_CTX_CREATE nvgCreateGLES3
    #define NVG_CTX_DELETE nvgDeleteGLES3
#else
    #error "No NanoVG implementation defined"
#endif

#include "../../libs/nanovg/nanovg_gl.h"
#include "../../libs/nanovg/nanovg_gl_utils.h"

/* GL_BGRA may not be defined on all platforms */
#ifndef GL_BGRA
    #ifdef GL_BGRA_EXT
        #define GL_BGRA GL_BGRA_EXT
    #else
        #define GL_BGRA 0x80E1
    #endif
#endif

/*********************
 *      DEFINES
 *********************/

#define NANOVG_DRAW_UNIT_ID 10

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t draw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t draw_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t draw_delete(lv_draw_unit_t * draw_unit);
static void draw_event_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_nanovg_init(void)
{
    static bool initialized = false;
    if(initialized) return;
    initialized = true;

    lv_draw_nanovg_unit_t * unit = lv_draw_create_unit(sizeof(lv_draw_nanovg_unit_t));
    unit->base_unit.dispatch_cb = draw_dispatch;
    unit->base_unit.evaluate_cb = draw_evaluate;
    unit->base_unit.delete_cb = draw_delete;
    unit->base_unit.event_cb = draw_event_cb;
    unit->base_unit.name = "NANOVG";

    unit->vg = NVG_CTX_CREATE(0);
    LV_ASSERT_MSG(unit->vg != NULL, "NanoVG init failed");

    lv_nanovg_utils_init(unit);
    lv_nanovg_image_cache_init(unit);
    lv_nanovg_fbo_cache_init(unit);
    lv_draw_nanovg_label_init(unit);
}

int lv_nanovg_fb_get_image_handle(struct NVGLUframebuffer * fb)
{
    LV_ASSERT_NULL(fb);
    return fb->image;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_execute(lv_draw_nanovg_unit_t * u, lv_draw_task_t * t)
{
    /* remember draw unit for access to unit's context */
    t->draw_unit = (lv_draw_unit_t *)u;
    lv_layer_t * layer = t->target_layer;

    lv_matrix_t global_matrix;
    lv_matrix_identity(&global_matrix);
    if(layer->buf_area.x1 || layer->buf_area.y1) {
        lv_matrix_translate(&global_matrix, -layer->buf_area.x1, -layer->buf_area.y1);
    }

#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_matrix_t layer_matrix = t->matrix;
    lv_matrix_multiply(&global_matrix, &layer_matrix);
#endif

    /* NanoVG will output premultiplied image, set the flag correspondingly. */
    if(layer->draw_buf) {
        lv_draw_buf_set_flag(layer->draw_buf, LV_IMAGE_FLAGS_PREMULTIPLIED);
    }

    nvgReset(u->vg);
    lv_nanovg_transform(u->vg, &global_matrix);

    lv_nanovg_set_clip_area(u->vg, &t->clip_area);

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_nanovg_fill(t, t->draw_dsc, &t->area);
            break;

        case LV_DRAW_TASK_TYPE_BORDER:
            lv_draw_nanovg_border(t, t->draw_dsc, &t->area);
            break;

        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
            lv_draw_nanovg_box_shadow(t, t->draw_dsc, &t->area);
            break;

        case LV_DRAW_TASK_TYPE_LETTER:
            lv_draw_nanovg_letter(t, t->draw_dsc, &t->area);
            break;

        case LV_DRAW_TASK_TYPE_LABEL:
            lv_draw_nanovg_label(t, t->draw_dsc, &t->area);
            break;

        case LV_DRAW_TASK_TYPE_IMAGE:
            lv_draw_nanovg_image(t, t->draw_dsc, &t->area, -1);
            break;

        case LV_DRAW_TASK_TYPE_LAYER:
            lv_draw_nanovg_layer(t, t->draw_dsc, &t->area);
            break;

        case LV_DRAW_TASK_TYPE_LINE:
            lv_draw_nanovg_line(t, t->draw_dsc);
            break;

        case LV_DRAW_TASK_TYPE_ARC:
            lv_draw_nanovg_arc(t, t->draw_dsc, &t->area);
            break;

        case LV_DRAW_TASK_TYPE_TRIANGLE:
            lv_draw_nanovg_triangle(t, t->draw_dsc);
            break;

        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
            lv_draw_nanovg_mask_rect(t, t->draw_dsc);
            break;

#if LV_USE_VECTOR_GRAPHIC
        case LV_DRAW_TASK_TYPE_VECTOR:
            lv_draw_nanovg_vector(t, t->draw_dsc);
            break;
#endif

#if LV_USE_3DTEXTURE
        case LV_DRAW_TASK_TYPE_3D:
            lv_draw_nanovg_3d(t, t->draw_dsc, &t->area);
            break;
#endif
        default:
            LV_LOG_ERROR("unknown draw task type: %d", t->type);
            break;
    }
}

static void on_layer_changed(lv_layer_t * new_layer)
{
    LV_PROFILER_DRAW_BEGIN;

    if(!new_layer->user_data) {
        /* Bind the default framebuffer for normal rendering */
        nvgluBindFramebuffer(NULL);
        LV_PROFILER_DRAW_END;
        return;
    }

    LV_PROFILER_BEGIN_TAG("nvgBindFramebuffer");
    nvgluBindFramebuffer(lv_nanovg_fbo_cache_entry_to_fb(new_layer->user_data));
    LV_PROFILER_END_TAG("nvgBindFramebuffer");

    /* Clear the off-screen framebuffer */
    LV_PROFILER_DRAW_BEGIN_TAG("glClear");
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    LV_PROFILER_DRAW_END_TAG("glClear");

    LV_PROFILER_DRAW_END;
}

static void on_layer_readback(lv_draw_nanovg_unit_t * u, lv_layer_t * layer)
{
    LV_PROFILER_DRAW_BEGIN;
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(layer);

    lv_cache_entry_t * entry = layer->user_data;

    if(!entry) {
        LV_LOG_WARN("No entry available for layer: %p", (void *)layer);
        LV_PROFILER_DRAW_END;
        return;
    }

    if(!layer->draw_buf) {
        LV_LOG_WARN("No draw buffer available for layer: %p", (void *)layer);
        LV_PROFILER_DRAW_END;
        return;
    }

    struct NVGLUframebuffer * fb = lv_nanovg_fbo_cache_entry_to_fb(entry);
    if(!fb) {
        LV_LOG_ERROR("No framebuffer available for layer: %p", (void *)layer);
        LV_PROFILER_DRAW_END;
        return;
    }

    /* Bind the FBO for reading */
    nvgluBindFramebuffer(fb);

    int32_t w = lv_area_get_width(&layer->buf_area);
    int32_t h = lv_area_get_height(&layer->buf_area);
    lv_draw_buf_t * draw_buf = layer->draw_buf;

    /* Read pixels from FBO */
    GLenum format;
    GLenum type;

    /* OpenGL reads bottom-to-top, but LVGL expects top-to-bottom */
    switch(draw_buf->header.cf) {
        case LV_COLOR_FORMAT_ARGB8888:
        case LV_COLOR_FORMAT_XRGB8888:
        case LV_COLOR_FORMAT_ARGB8888_PREMULTIPLIED:
            format = GL_BGRA;
            type = GL_UNSIGNED_BYTE;
            break;

        case LV_COLOR_FORMAT_RGB888:
            format = GL_RGB;
            type = GL_UNSIGNED_BYTE;
            break;

        case LV_COLOR_FORMAT_RGB565:
            format = GL_RGB;
            type = GL_UNSIGNED_SHORT_5_6_5;
            break;

        default:
            LV_LOG_WARN("Unsupported color format: %d", draw_buf->header.cf);
            LV_PROFILER_DRAW_END;
            return;
    }

    for(int32_t y = 0; y < h; y++) {
        /* Reverse Y coordinate */
        void * row = lv_draw_buf_goto_xy(draw_buf, 0, h - 1 - y);
        LV_PROFILER_DRAW_BEGIN_TAG("glReadPixels");
        glReadPixels(0, y, w, 1, format, type, row);
        LV_PROFILER_DRAW_END_TAG("glReadPixels");

        if(draw_buf->header.cf == LV_COLOR_FORMAT_RGB888) {
            /* Swizzle RGB -> BGR */
            lv_color_t * px = row;
            for(int32_t x = 0; x < w; x++) {
                uint8_t r = px->blue;
                px->blue = px->red;
                px->red = r;
                px++;
            }
        }
    }

    /* Bind back to default framebuffer */
    nvgluBindFramebuffer(NULL);

    /* Mark draw_buf as modified */
    lv_draw_buf_flush_cache(draw_buf, NULL);

    LV_PROFILER_DRAW_END;
}

static int32_t draw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)draw_unit;

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, NANOVG_DRAW_UNIT_ID);
    if(!t || t->preferred_draw_unit_id != NANOVG_DRAW_UNIT_ID) {
        lv_nanovg_end_frame(u);
        return LV_DRAW_UNIT_IDLE;
    }

    if(u->current_layer != layer) {
        on_layer_changed(layer);
        u->current_layer = layer;
    }

    if(!u->is_started) {
        const int32_t buf_w = lv_area_get_width(&layer->buf_area);
        const int32_t buf_h = lv_area_get_height(&layer->buf_area);

        glViewport(0, 0, buf_w, buf_h);
        LV_PROFILER_DRAW_BEGIN_TAG("nvgBeginFrame");
        nvgBeginFrame(u->vg, buf_w, buf_h, 1.0f);
        LV_PROFILER_DRAW_END_TAG("nvgBeginFrame");
        u->is_started = true;
    }

    t->state = LV_DRAW_TASK_STATE_IN_PROGRESS;

    draw_execute(u, t);

    t->state = LV_DRAW_TASK_STATE_FINISHED;

    /*The draw unit is free now. Request a new dispatching as it can get a new task*/
    lv_draw_dispatch_request();

    return 1;
}

static int32_t draw_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task)
{
    LV_UNUSED(draw_unit);

    switch(task->type) {
        case LV_DRAW_TASK_TYPE_FILL:
        case LV_DRAW_TASK_TYPE_BORDER:
        case LV_DRAW_TASK_TYPE_BOX_SHADOW:
        case LV_DRAW_TASK_TYPE_LETTER:
        case LV_DRAW_TASK_TYPE_LABEL:
        case LV_DRAW_TASK_TYPE_IMAGE:
        case LV_DRAW_TASK_TYPE_LAYER:
        case LV_DRAW_TASK_TYPE_LINE:
        case LV_DRAW_TASK_TYPE_ARC:
        case LV_DRAW_TASK_TYPE_TRIANGLE:
        case LV_DRAW_TASK_TYPE_MASK_RECTANGLE:
#if LV_USE_VECTOR_GRAPHIC
        case LV_DRAW_TASK_TYPE_VECTOR:
#endif
#if LV_USE_3DTEXTURE
        case LV_DRAW_TASK_TYPE_3D:
#endif
            break;

        default:
            /*The draw unit is not able to draw this task. */
            return 0;
    }

    if(task->preference_score > 80) {
        /* The draw unit is able to draw this task. */
        task->preference_score = 80;
        task->preferred_draw_unit_id = NANOVG_DRAW_UNIT_ID;
    }

    return 1;
}

static int32_t draw_delete(lv_draw_unit_t * draw_unit)
{
    lv_draw_nanovg_unit_t * unit = (lv_draw_nanovg_unit_t *)draw_unit;
    lv_draw_nanovg_label_deinit(unit);
    lv_nanovg_fbo_cache_deinit(unit);
    lv_nanovg_image_cache_deinit(unit);
    lv_nanovg_utils_deinit(unit);
    NVG_CTX_DELETE(unit->vg);
    unit->vg = NULL;
    return 0;
}

static void draw_event_cb(lv_event_t * e)
{
    lv_draw_nanovg_unit_t * u = lv_event_get_current_target(e);
    lv_layer_t * layer = lv_event_get_param(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_CANCEL:
            LV_PROFILER_DRAW_BEGIN_TAG("nvgCancelFrame");
            nvgCancelFrame(u->vg);
            LV_PROFILER_DRAW_END_TAG("nvgCancelFrame");
            lv_nanovg_clean_up(u);
            break;
        case LV_EVENT_CHILD_CREATED: {
                /* The internal rendering uses RGBA format, which is switched to LVGL BGRA format during readback. */
                lv_cache_entry_t * entry = lv_nanovg_fbo_cache_get(u, lv_area_get_width(&layer->buf_area),
                                                                   lv_area_get_height(&layer->buf_area), 0, NVG_TEXTURE_RGBA);
                layer->user_data = entry;
            }
            break;
        case LV_EVENT_CHILD_DELETED: {
                lv_cache_entry_t * entry = layer->user_data;
                if(entry) {
                    lv_nanovg_fbo_cache_release(u, entry);
                    layer->user_data = NULL;
                }

                /**
                 * Clear current_layer if it's being deleted, so next dispatch
                 * will properly call on_layer_changed even if layer address is reused
                 */
                if(u->current_layer == layer) {
                    u->current_layer = NULL;
                }
            }
            break;
        case LV_EVENT_SCREEN_LOAD_START:
            on_layer_readback(u, layer);
            break;
        case LV_EVENT_INVALIDATE_AREA:
            lv_nanovg_image_cache_drop(u, lv_event_get_param(e));
            break;
        default:
            break;
    }
}

#endif /* LV_USE_DRAW_NANOVG */
