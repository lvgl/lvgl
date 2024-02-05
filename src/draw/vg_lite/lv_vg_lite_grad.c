/**
 * @file lv_vg_lite_grad.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_vg_lite_grad.h"

#if LV_USE_DRAW_VG_LITE

#include "lv_draw_vg_lite_type.h"

/*********************
 *      DEFINES
 *********************/

#define LV_VG_LITE_GRAD_CACHE_SIZE 16

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    vg_lite_linear_gradient_t vg_grad;
    lv_grad_dsc_t lv_grad;
} grad_item_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static vg_lite_linear_gradient_t * lv_vg_lite_linear_grad_get(struct _lv_draw_vg_lite_unit_t * u,
                                                              const lv_grad_dsc_t * grad);

static bool grad_create_cb(grad_item_t * item, void * user_data);
static void grad_free_cb(grad_item_t * item, void * user_data);
static lv_cache_compare_res_t grad_compare_cb(const grad_item_t * lhs, const grad_item_t * rhs);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_vg_lite_grad_init(struct _lv_draw_vg_lite_unit_t * u)
{
    LV_ASSERT_NULL(u);

    lv_cache_ops_t ops = {
        .compare_cb = (lv_cache_compare_cb_t)grad_compare_cb,
        .create_cb = (lv_cache_create_cb_t)grad_create_cb,
        .free_cb = (lv_cache_free_cb_t)grad_free_cb,
    };

    u->grad_cache = lv_cache_create(&lv_cache_class_lru_rb_count, sizeof(grad_item_t), LV_VG_LITE_GRAD_CACHE_SIZE, ops);
    LV_ASSERT_NULL(u->grad_cache);
}

void lv_vg_lite_grad_deinit(struct _lv_draw_vg_lite_unit_t * u)
{
    LV_ASSERT_NULL(u);
    lv_cache_destroy(u->grad_cache, NULL);
}

void lv_vg_lite_draw_linear_grad(
    struct _lv_draw_vg_lite_unit_t * u,
    vg_lite_buffer_t * buffer,
    vg_lite_path_t * path,
    const lv_area_t * area,
    const lv_grad_dsc_t * grad,
    const vg_lite_matrix_t * matrix,
    vg_lite_fill_t fill,
    vg_lite_blend_t blend)
{
    LV_ASSERT_NULL(buffer);
    LV_ASSERT_NULL(path);
    LV_ASSERT_NULL(area);
    LV_ASSERT_NULL(grad);
    LV_ASSERT_NULL(matrix);

    LV_PROFILER_BEGIN;

    vg_lite_linear_gradient_t * gradient = lv_vg_lite_linear_grad_get(u, grad);

    vg_lite_matrix_t * grad_matrix = vg_lite_get_grad_matrix(gradient);
    vg_lite_identity(grad_matrix);
    vg_lite_translate(area->x1, area->y1, grad_matrix);

    if(grad->dir == LV_GRAD_DIR_VER) {
        vg_lite_scale(1, lv_area_get_height(area) / 256.0f, grad_matrix);
        vg_lite_rotate(90, grad_matrix);
    }
    else if(grad->dir == LV_GRAD_DIR_HOR) {
        vg_lite_scale(lv_area_get_width(area) / 256.0f, 1, grad_matrix);
    }
    else {
        LV_ASSERT_MSG(false, "Unknown gradient direction");
    }

    LV_VG_LITE_ASSERT_DEST_BUFFER(buffer);
    LV_VG_LITE_ASSERT_SRC_BUFFER(&gradient->image);
    LV_VG_LITE_ASSERT_PATH(path);

    LV_PROFILER_BEGIN_TAG("vg_lite_draw_grad");
    LV_VG_LITE_CHECK_ERROR(vg_lite_draw_grad(
                               buffer,
                               path,
                               fill,
                               (vg_lite_matrix_t *)matrix,
                               gradient,
                               blend));
    LV_PROFILER_END_TAG("vg_lite_draw_grad");

    LV_PROFILER_END;
}

void lv_vg_lite_linear_grad_drop_all(struct _lv_draw_vg_lite_unit_t * u)
{
    LV_ASSERT_NULL(u);
    lv_cache_drop_all(u->grad_cache, NULL);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static vg_lite_linear_gradient_t * lv_vg_lite_linear_grad_get(struct _lv_draw_vg_lite_unit_t * u,
                                                              const lv_grad_dsc_t * grad)
{
    LV_ASSERT_NULL(u);
    LV_ASSERT_NULL(grad);

    grad_item_t search_key;
    lv_memzero(&search_key, sizeof(grad_item_t));
    search_key.lv_grad = *grad;

    bool cache_hitting = true;
    lv_cache_entry_t * cache_node_entry = lv_cache_acquire(u->grad_cache, &search_key, NULL);
    if(cache_node_entry == NULL) {
        cache_hitting = false;
        cache_node_entry = lv_cache_acquire_or_create(u->grad_cache, &search_key, NULL);
        if(cache_node_entry == NULL) {
            LV_LOG_ERROR("grad cache creating failed");
            return NULL;
        }
    }

    grad_item_t * item = lv_cache_entry_get_data(cache_node_entry);
    return &item->vg_grad;
}

static bool grad_create_cb(grad_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);

    LV_PROFILER_BEGIN;

    vg_lite_error_t error = vg_lite_init_grad(&item->vg_grad);
    if(error != VG_LITE_SUCCESS) {
        LV_PROFILER_END;
        LV_LOG_ERROR("init grad error(%d): %s", (int)error, lv_vg_lite_error_string(error));
        return false;
    }

    vg_lite_uint32_t colors[VLC_MAX_GRADIENT_STOPS];
    vg_lite_uint32_t stops[VLC_MAX_GRADIENT_STOPS];

    /* Gradient setup */
    uint8_t cnt = item->lv_grad.stops_count;
    LV_ASSERT(cnt < VLC_MAX_GRADIENT_STOPS);
    for(uint8_t i = 0; i < cnt; i++) {
        stops[i] = item->lv_grad.stops[i].frac;
        const lv_color_t * c = &item->lv_grad.stops[i].color;
        lv_opa_t opa = item->lv_grad.stops[i].opa;

        /* lvgl color -> gradient color */
        lv_color_t grad_color = lv_color_make(c->blue, c->green, c->red);
        colors[i] = lv_vg_lite_color(grad_color, opa, true);
    }

    LV_VG_LITE_CHECK_ERROR(vg_lite_set_grad(&item->vg_grad, cnt, colors, stops));

    LV_PROFILER_BEGIN_TAG("vg_lite_update_grad");
    LV_VG_LITE_CHECK_ERROR(vg_lite_update_grad(&item->vg_grad));
    LV_PROFILER_END_TAG("vg_lite_update_grad");

    /* Premultiply the gradient image */
    lv_color32_t * c32 = item->vg_grad.image.memory;
    for(int x = 0; x < item->vg_grad.image.width; x++) {
        lv_color_premultiply(c32);
        c32++;
    }

    LV_PROFILER_END;
    return true;
}

static void grad_free_cb(grad_item_t * item, void * user_data)
{
    LV_UNUSED(user_data);
    LV_VG_LITE_CHECK_ERROR(vg_lite_clear_grad(&item->vg_grad));
}

static lv_cache_compare_res_t grad_compare_cb(const grad_item_t * lhs, const grad_item_t * rhs)
{
    if(lhs->lv_grad.stops_count != rhs->lv_grad.stops_count) {
        return lhs->lv_grad.stops_count > rhs->lv_grad.stops_count ? 1 : -1;
    }

    int cmp_res = memcmp(&lhs->lv_grad, &rhs->lv_grad, sizeof(lhs->lv_grad.stops));
    if(cmp_res != 0) {
        return cmp_res > 0 ? 1 : -1;
    }

    return 0;
}

#endif /*LV_USE_DRAW_VG_LITE*/
