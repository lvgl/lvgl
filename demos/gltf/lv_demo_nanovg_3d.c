/**
 * @file lv_demo_nanovg_3d.c
 * Demo for NanoVG 3D extension with cgltf
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_demo_nanovg_3d.h"

#if LV_USE_DEMO_NANOVG_GLTF

#include "../../lvgl_private.h"
#include "../../src/draw/nanovg/lv_nanovg_3d.h"
#include "../../src/draw/nanovg/lv_draw_nanovg_private.h"
#include "../../src/libs/nanovg/nanovg.h"

/* Include OpenGL headers based on backend */
#if LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES2 || LV_NANOVG_BACKEND == LV_NANOVG_BACKEND_GLES3
    #include <GLES2/gl2.h>
#else
    #include <GL/gl.h>
#endif

/*********************
 *      DEFINES
 *********************/

#define DEMO_3D_WIDTH   400
#define DEMO_3D_HEIGHT  400
#define ROTATION_STEP   2.0f
#define UPDATE_PERIOD   30

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_obj_t * canvas;
    lv_nanovg_3d_model_t * model;
    lv_nanovg_3d_camera_t camera;
    float rotation_angle;
    lv_timer_t * timer;
} demo_3d_data_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void demo_timer_cb(lv_timer_t * timer);
static void demo_delete_cb(lv_event_t * e);
static void canvas_draw_cb(lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_demo_nanovg_3d(const char * path)
{
    if(path == NULL) {
        LV_LOG_ERROR("glTF path is NULL");
        return NULL;
    }

    /* Get the 3D context */
    lv_nanovg_3d_ctx_t * ctx3d = lv_draw_nanovg_3d_get_ctx();
    if(ctx3d == NULL) {
        LV_LOG_ERROR("NanoVG 3D context not initialized");
        return NULL;
    }

    /* Create demo data */
    demo_3d_data_t * data = lv_malloc_zeroed(sizeof(demo_3d_data_t));
    if(data == NULL) {
        LV_LOG_ERROR("Failed to allocate demo data");
        return NULL;
    }

    /* Load the model */
    data->model = lv_nanovg_3d_load_model(ctx3d, path);
    if(data->model == NULL) {
        LV_LOG_ERROR("Failed to load model: %s", path);
        lv_free(data);
        return NULL;
    }

    /* Initialize camera */
    lv_nanovg_3d_camera_init(&data->camera);

    /* Calculate camera distance based on model bounds */
    float min_bounds[3], max_bounds[3];
    lv_nanovg_3d_model_get_bounds(data->model, min_bounds, max_bounds);

    float center_x = (min_bounds[0] + max_bounds[0]) * 0.5f;
    float center_y = (min_bounds[1] + max_bounds[1]) * 0.5f;
    float center_z = (min_bounds[2] + max_bounds[2]) * 0.5f;

    float size_x = max_bounds[0] - min_bounds[0];
    float size_y = max_bounds[1] - min_bounds[1];
    float size_z = max_bounds[2] - min_bounds[2];

    float max_size = size_x;
    if(size_y > max_size) max_size = size_y;
    if(size_z > max_size) max_size = size_z;

    /* Set camera to look at model center */
    data->camera.center[0] = center_x;
    data->camera.center[1] = center_y;
    data->camera.center[2] = center_z;

    /* Position camera at appropriate distance */
    float distance = max_size * 2.0f;
    if(distance < 1.0f) distance = 1.0f;

    data->camera.eye[0] = center_x;
    data->camera.eye[1] = center_y + max_size * 0.3f;
    data->camera.eye[2] = center_z + distance;

    /* Create main container */
    lv_obj_t * cont = lv_obj_create(lv_screen_active());
    lv_obj_set_size(cont, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(cont);
    lv_obj_set_style_pad_all(cont, 10, 0);
    lv_obj_set_style_bg_color(cont, lv_color_hex(0x2D2D2D), 0);
    lv_obj_set_style_radius(cont, 10, 0);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(cont, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* Create title label */
    lv_obj_t * title = lv_label_create(cont);
    lv_label_set_text(title, "NanoVG 3D Demo");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_style_text_font(title, &lv_font_montserrat_18, 0);

    /* Create path label */
    lv_obj_t * path_label = lv_label_create(cont);
    lv_label_set_text_fmt(path_label, "Model: %s", path);
    lv_obj_set_style_text_color(path_label, lv_color_hex(0xAAAAAA), 0);
    lv_obj_set_style_text_font(path_label, &lv_font_montserrat_12, 0);

    /* Create the 3D canvas - a simple object with custom draw callback */
    lv_obj_t * obj_3d = lv_obj_create(cont);
    lv_obj_set_size(obj_3d, DEMO_3D_WIDTH, DEMO_3D_HEIGHT);
    lv_obj_set_style_bg_color(obj_3d, lv_color_hex(0x1A1A1A), 0);
    lv_obj_set_style_bg_opa(obj_3d, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(obj_3d, 8, 0);
    lv_obj_set_style_border_width(obj_3d, 2, 0);
    lv_obj_set_style_border_color(obj_3d, lv_color_hex(0x444444), 0);
    lv_obj_remove_flag(obj_3d, LV_OBJ_FLAG_SCROLLABLE);

    /* Store data in canvas user_data for draw callback */
    lv_obj_set_user_data(obj_3d, data);

    /* Add custom draw callback */
    lv_obj_add_event_cb(obj_3d, canvas_draw_cb, LV_EVENT_DRAW_MAIN_END, data);

    data->canvas = obj_3d;

    /* Store data pointer in container */
    lv_obj_set_user_data(cont, data);

    /* Add delete callback */
    lv_obj_add_event_cb(cont, demo_delete_cb, LV_EVENT_DELETE, data);

    /* Create update timer */
    data->timer = lv_timer_create(demo_timer_cb, UPDATE_PERIOD, data);

    /* Create rotation info label */
    lv_obj_t * info = lv_label_create(cont);
    lv_label_set_text(info, "Auto-rotating...");
    lv_obj_set_style_text_color(info, lv_color_hex(0x888888), 0);

    LV_LOG_USER("NanoVG 3D demo created successfully");

    return cont;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void canvas_draw_cb(lv_event_t * e)
{
    demo_3d_data_t * data = (demo_3d_data_t *)lv_event_get_user_data(e);
    if(data == NULL || data->model == NULL) return;

    lv_nanovg_3d_ctx_t * ctx3d = lv_draw_nanovg_3d_get_ctx();
    if(ctx3d == NULL) return;

    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_layer_t * layer = lv_event_get_layer(e);

    /* Get object coordinates */
    lv_area_t coords;
    lv_obj_get_coords(obj, &coords);

    /* Adjust to layer coordinates */
    lv_area_t dest_area = coords;
    lv_area_move(&dest_area, -layer->buf_area.x1, -layer->buf_area.y1);

    int32_t layer_w = lv_area_get_width(&layer->buf_area);
    int32_t layer_h = lv_area_get_height(&layer->buf_area);

    /* Find the NanoVG draw unit */
    lv_draw_nanovg_unit_t * u = (lv_draw_nanovg_unit_t *)lv_draw_get_unit_by_name("NANOVG");

    if(u && u->is_started) {
        /* End the current NanoVG frame */
        nvgEndFrame(u->vg);
        u->is_started = false;
    }

    /* Save OpenGL state */
    lv_nanovg_3d_begin(ctx3d);

    /* Update model rotation */
    lv_nanovg_3d_model_set_rotation(data->model, data->rotation_angle);

    /* Render directly to the current framebuffer */
    lv_nanovg_3d_render_direct(ctx3d, data->model, &dest_area, layer_w, layer_h, &data->camera);

    /* Restore OpenGL state */
    lv_nanovg_3d_end(ctx3d);

    /* Restart NanoVG frame for subsequent drawing */
    if(u) {
        glViewport(0, 0, layer_w, layer_h);
        nvgBeginFrame(u->vg, layer_w, layer_h, 1.0f);
        u->is_started = true;
    }
}

static void demo_timer_cb(lv_timer_t * timer)
{
    demo_3d_data_t * data = (demo_3d_data_t *)timer->user_data;
    if(data == NULL) return;

    /* Rotate model */
    data->rotation_angle += ROTATION_STEP;
    if(data->rotation_angle >= 360.0f) {
        data->rotation_angle -= 360.0f;
    }

    /* Invalidate to trigger redraw */
    if(data->canvas) {
        lv_obj_invalidate(data->canvas);
    }
}

static void demo_delete_cb(lv_event_t * e)
{
    demo_3d_data_t * data = (demo_3d_data_t *)lv_event_get_user_data(e);
    if(data == NULL) return;

    /* Stop timer */
    if(data->timer) {
        lv_timer_delete(data->timer);
        data->timer = NULL;
    }

    /* Free model */
    lv_nanovg_3d_ctx_t * ctx3d = lv_draw_nanovg_3d_get_ctx();
    if(ctx3d && data->model) {
        lv_nanovg_3d_free_model(ctx3d, data->model);
        data->model = NULL;
    }

    /* Free data */
    lv_free(data);
}

#endif /* LV_USE_DEMO_NANOVG_GLTF */
