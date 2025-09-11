/**
 * @file lv_draw_nanovg.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "../lv_draw_private.h"
#include "../../draw/lv_draw_rect.h"
#include "../../misc/lv_area_private.h"

#if !LV_USE_MATRIX
    #error "Needs LV_USE_MATRIX = 1"
#endif

#define NANOVG_GLEW 1
// #define NANOVG_GL3 0
#define NANOVG_GL_USE_UNIFORMBUFFER 0

#ifdef NANOVG_GLEW
    #include <GL/glew.h>
#endif
#define GLFW_INCLUDE_GLEXT
#include <GLFW/glfw3.h>

#include "../../libs/nanovg/nanovg.h"

#define NANOVG_GL2_IMPLEMENTATION
#include "../../libs/nanovg/nanovg_gl.h"

/*********************
 *      DEFINES
 *********************/

#define NANOVG_DRAW_UNIT_ID 20

/**********************
 *      TYPEDEFS
 **********************/

typedef struct _lv_draw_nanovg_unit_t {
    lv_draw_unit_t base_unit;
    NVGcontext * vg;
    bool is_started;
    lv_matrix_t global_matrix;
} lv_draw_nanovg_unit_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static int32_t draw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer);
static int32_t draw_evaluate(lv_draw_unit_t * draw_unit, lv_draw_task_t * task);
static int32_t draw_delete(lv_draw_unit_t * draw_unit);

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
    unit->base_unit.name = "NANOVG";

    unit->vg = nvgCreateGL2(NVG_ANTIALIAS | NVG_STENCIL_STROKES | NVG_DEBUG);
    LV_ASSERT_MSG(unit->vg != NULL, "nvgCreateGL2 init failed");
}

void lv_draw_nanovg_deinit(void)
{
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_matrix_to_nvg_transform(float * t, const lv_matrix_t * m)
{
    t[0] = m->m[0][0];
    t[1] = m->m[1][0];
    t[2] = m->m[0][1];
    t[3] = m->m[1][1];
    t[4] = m->m[0][2];
    t[5] = m->m[1][2];
}

static void lv_draw_nanovg_fill(lv_draw_task_t * t, const lv_draw_fill_dsc_t * dsc, const lv_area_t * coords)
{
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)t->draw_unit;

    lv_area_t clip_area;
    if(!lv_area_intersect(&clip_area, coords, &t->clip_area)) {
        /*Fully clipped, nothing to do*/
        return;
    }

    LV_PROFILER_DRAW_BEGIN;

    float xform[6];
    lv_matrix_to_nvg_transform(xform, &u->global_matrix);
    nvgResetTransform(u->vg);
    nvgTransform(u->vg, xform[0], xform[1], xform[2], xform[3], xform[4], xform[5]);

    // nvgSave(u->vg);

    nvgBeginPath(u->vg);

    const int32_t w = lv_area_get_width(coords);
    const int32_t h = lv_area_get_height(coords);

    if(dsc->radius > 0) {
        const float half_w = w / 2.0f;
        const float half_h = h / 2.0f;

        /*clamping cornerRadius by minimum size*/
        const float r_max = LV_MIN(half_w, half_h);

        nvgRoundedRect(u->vg, coords->x1, coords->y1, w, h, dsc->radius > r_max ? r_max : dsc->radius);
    }
    else {
        nvgRect(u->vg, coords->x1, coords->y1, w, h);
    }

    if(0 && dsc->grad.dir != LV_GRAD_DIR_NONE) {

    }
    else {
        // nvgPathWinding(u->vg, NVG_CCW);
        nvgFillColor(u->vg, nvgRGBA(dsc->color.red, dsc->color.green, dsc->color.blue, dsc->opa));
        nvgFill(u->vg);
    }

    // nvgRestore(u->vg);

    LV_PROFILER_DRAW_END;
}

static void draw_execute(lv_draw_nanovg_unit_t * u, lv_draw_task_t * t)
{
    /* remember draw unit for access to unit's context */
    t->draw_unit = (lv_draw_unit_t *)u;
    lv_layer_t * layer = t->target_layer;

    lv_matrix_identity(&u->global_matrix);
    if(layer->buf_area.x1 || layer->buf_area.y1) {
        lv_matrix_translate(&u->global_matrix, -layer->buf_area.x1, -layer->buf_area.y1);
    }

#if LV_DRAW_TRANSFORM_USE_MATRIX
    lv_matrix_t layer_matrix = t->matrix;
    lv_matrix_multiply(&u->global_matrix, &layer_matrix);
#endif

    switch(t->type) {
        case LV_DRAW_TASK_TYPE_FILL:
            lv_draw_nanovg_fill(t, t->draw_dsc, &t->area);
            break;
        default:
            break;
    }
}

static void on_start(lv_draw_nanovg_unit_t * u, float w, float h)
{
    glViewport(0, 0, w, h);
    nvgBeginFrame(u->vg, w, h, 1.0f);
}

static void on_end(lv_draw_nanovg_unit_t * u)
{
    nvgEndFrame(u->vg);
}

static int32_t draw_dispatch(lv_draw_unit_t * draw_unit, lv_layer_t * layer)
{
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)draw_unit;

    lv_draw_task_t * t = lv_draw_get_available_task(layer, NULL, NANOVG_DRAW_UNIT_ID);
    if(!t || t->preferred_draw_unit_id != NANOVG_DRAW_UNIT_ID) {
        if(u->is_started) {
            on_end(u);
            u->is_started = false;
        }
        return LV_DRAW_UNIT_IDLE;
    }

    void * buf = lv_draw_layer_alloc_buf(layer);
    if(!buf) {
        return LV_DRAW_UNIT_IDLE;
    }

    if(!u->is_started) {
        on_start(u, layer->draw_buf->header.w, layer->draw_buf->header.h);
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
    nvgDeleteGL2(unit->vg);
    unit->vg = NULL;
    return 0;
}
