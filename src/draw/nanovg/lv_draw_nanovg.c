/**
 * @file lv_draw_nanovg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_nanovg.h"

#if LV_USE_DRAW_NANOVG

#include "lv_draw_nanovg_private.h"
#include "lv_nanovg_utils.h"

#define NANOVG_GLES2_IMPLEMENTATION

#if defined(NANOVG_GL2_IMPLEMENTATION)
#include <GL/glew.h>
#define NVG_CTX_CREATE nvgCreateGL2
#define NVG_CTX_DELETE nvgDeleteGL2
#elif defined(NANOVG_GL3_IMPLEMENTATION)
#include <GL/glew.h>
#define NVG_CTX_CREATE nvgCreateGL3
#define NVG_CTX_DELETE nvgDeleteGL3
#elif defined(NANOVG_GLES2_IMPLEMENTATION)
#include <GLES2/gl2.h>
#define NVG_CTX_CREATE nvgCreateGLES2
#define NVG_CTX_DELETE nvgDeleteGLES2
#elif defined(NANOVG_GLES3_IMPLEMENTATION)
#include <GLES3/gl3.h>
#define NVG_CTX_CREATE nvgCreateGLES3
#define NVG_CTX_DELETE nvgDeleteGLES3
#endif

#include "../../libs/nanovg/nanovg_gl.h"
#include "../../libs/nanovg/nanovg_gl_utils.h"

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
    lv_draw_nanovg_unit_t * unit = lv_draw_create_unit(sizeof(lv_draw_nanovg_unit_t));
    unit->base_unit.dispatch_cb = draw_dispatch;
    unit->base_unit.evaluate_cb = draw_evaluate;
    unit->base_unit.delete_cb = draw_delete;
    unit->base_unit.event_cb = draw_event_cb;
    unit->base_unit.name = "NANOVG";

    unit->vg = NVG_CTX_CREATE(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    LV_ASSERT_MSG(unit->vg != NULL, "NanoVG init failed");

    lv_nanovg_utils_init(unit);
    lv_draw_nanovg_label_init(unit);
}

void lv_draw_nanovg_event_param_init(lv_draw_nanovg_event_param_t * param)
{
    LV_ASSERT_NULL(param);
    lv_memzero(param, sizeof(lv_draw_nanovg_event_param_t));
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
            lv_draw_nanovg_image(t, t->draw_dsc, &t->area, true, -1);
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
        default:
            LV_LOG_ERROR("unknown draw task type: %d", t->type);
            break;
    }
}

static int32_t draw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)draw_unit;

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, NANOVG_DRAW_UNIT_ID);
    if(!t || t->preferred_draw_unit_id != NANOVG_DRAW_UNIT_ID) {
        if(u->is_started) {
            lv_nanovg_end_frame(u);
        }
        return LV_DRAW_UNIT_IDLE;
    }

   const int32_t buf_w = lv_area_get_width(&layer->buf_area);
   const int32_t buf_h = lv_area_get_height(&layer->buf_area);

    if(!u->is_started) {
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
    lv_nanovg_utils_deinit(unit);
    NVG_CTX_DELETE(unit->vg);
    unit->vg = NULL;
    return 0;
}

static void draw_event_cb(lv_event_t * e)
{
    lv_draw_nanovg_unit_t * u = lv_event_get_current_target(e);
    lv_draw_nanovg_event_param_t * param = lv_event_get_param(e);

    switch(lv_event_get_code(e)) {
        case LV_EVENT_CANCEL:
            nvgCancelFrame(u->vg);
            lv_nanovg_clean_up(u);
            break;
        case LV_EVENT_CHILD_CREATED:
            param->fb = nvgluCreateFramebuffer(u->vg, param->width, param->height, 0);
            if(!param->fb) {
                LV_LOG_ERROR("Failed to create framebuffer");
            }
            break;
        case LV_EVENT_CHILD_CHANGED:
            nvgluBindFramebuffer(param->fb);
            break;
        case LV_EVENT_CHILD_DELETED:
            if(param->fb) {
                nvgluDeleteFramebuffer(param->fb);
                param->fb = NULL;
            }
            break;
        default:
            break;
    }
}

#endif /* LV_USE_DRAW_NANOVG */
