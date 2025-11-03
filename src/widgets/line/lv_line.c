/**
 * @file lv_line.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_line_private.h"
#include "../../core/lv_obj_class_private.h"


#if LV_USE_LINE != 0
#include "../../misc/lv_assert.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_types.h"
#include "../../draw/lv_draw.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_line_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_line_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void line_set_points(lv_obj_t * obj, const lv_point_precise_t points[], uint32_t point_num, bool mut);
static void lv_line_event(const lv_obj_class_t * class_p, lv_event_t * e);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_line_class = {
    .constructor_cb = lv_line_constructor,
    .event_cb = lv_line_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_line_t),
    .base_class = &lv_obj_class,
    .name = "lv_line",
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_line_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_line_set_points(lv_obj_t * obj, const lv_point_precise_t points[], uint32_t point_num)
{
    line_set_points(obj, points, point_num, false);
}

void lv_line_set_points_mutable(lv_obj_t * obj, lv_point_precise_t points[], uint32_t point_num)
{
    line_set_points(obj, points, point_num, true);
}

void lv_line_set_y_invert(lv_obj_t * obj, bool en)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    if(line->y_inv == en) return;

    line->y_inv = en ? 1U : 0U;

    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

const lv_point_precise_t * lv_line_get_points(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    return line->point_array.constant;
}

uint32_t lv_line_get_point_count(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    return line->point_num;
}

bool lv_line_is_point_array_mutable(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    return line->point_array_is_mutable;
}

lv_point_precise_t * lv_line_get_points_mutable(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    if(!line->point_array_is_mutable) {
        LV_LOG_WARN("the line point array is not mutable");
        return NULL;
    }
    return line->point_array.mut;
}

bool lv_line_get_y_invert(const lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;

    return line->y_inv == 1U;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_line_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_line_t * line = (lv_line_t *)obj;

    line->point_num   = 0;
    line->point_array.constant = NULL;
    line->y_inv       = 0;
    line->point_array_is_mutable = 0;

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);

    LV_TRACE_OBJ_CREATE("finished");
}

static void line_set_points(lv_obj_t * obj, const lv_point_precise_t points[], uint32_t point_num, bool mut)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_line_t * line = (lv_line_t *)obj;
    line->point_array.constant = points;
    line->point_num      = point_num;
    line->point_array_is_mutable = mut;

    lv_obj_refresh_self_size(obj);

    lv_obj_invalidate(obj);
}

static inline lv_value_precise_t resolve_point_coord(lv_value_precise_t coord, int32_t max)
{
    if(LV_COORD_IS_PCT((int32_t)coord)) {
        return LV_CLAMP(0, max * LV_COORD_GET_PCT((int32_t)coord) / 100, max);
    }
    else {
        return coord;
    }
}

static void lv_line_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);

    if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {
        /*The corner of the skew lines is out of the intended area*/
        int32_t line_width = lv_obj_get_style_line_width(obj, LV_PART_MAIN);
        int32_t * s = lv_event_get_param(e);
        if(*s < line_width) *s = line_width;
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_line_t * line = (lv_line_t *)obj;

        if(line->point_num == 0 || line->point_array.constant == NULL) return;

        lv_point_t * p = lv_event_get_param(e);
        int32_t w = 0;
        int32_t h = 0;

        uint32_t i;
        for(i = 0; i < line->point_num; i++) {
            if(!LV_COORD_IS_PCT((int32_t)line->point_array.constant[i].x)) {
                w = (int32_t)LV_MAX(line->point_array.constant[i].x, w);
            }

            if(!LV_COORD_IS_PCT((int32_t)line->point_array.constant[i].y)) {
                h = (int32_t)LV_MAX(line->point_array.constant[i].y, h);
            }
        }

        p->x = w;
        p->y = h;
    }
    else if(code == LV_EVENT_DRAW_MAIN) {
        lv_line_t * line = (lv_line_t *)obj;
        lv_layer_t * layer = lv_event_get_layer(e);

        if(line->point_num < 2 || line->point_array.constant == NULL) return;

        lv_area_t area;
        lv_obj_get_coords(obj, &area);
        int32_t x_ofs = area.x1 - lv_obj_get_scroll_x(obj);
        int32_t y_ofs = area.y1 - lv_obj_get_scroll_y(obj);

        lv_draw_line_dsc_t line_dsc;
        lv_draw_line_dsc_init(&line_dsc);
        line_dsc.base.layer = layer;
        lv_obj_init_draw_line_dsc(obj, LV_PART_MAIN, &line_dsc);

        /*Read all points and draw the lines*/
        uint32_t i;
        for(i = 0; i < line->point_num - 1; i++) {
            int32_t w = lv_obj_get_width(obj);
            int32_t h = lv_obj_get_height(obj);

            line_dsc.p1.x = resolve_point_coord(line->point_array.constant[i].x, w) + x_ofs;
            line_dsc.p1.y = resolve_point_coord(line->point_array.constant[i].y, h);

            line_dsc.p2.x = resolve_point_coord(line->point_array.constant[i + 1].x, w) + x_ofs;
            line_dsc.p2.y = resolve_point_coord(line->point_array.constant[i + 1].y, h);

            if(line->y_inv == 0) {
                line_dsc.p1.y = line_dsc.p1.y + y_ofs;
                line_dsc.p2.y = line_dsc.p2.y + y_ofs;
            }
            else {
                line_dsc.p1.y = h - line_dsc.p1.y + y_ofs;
                line_dsc.p2.y = h - line_dsc.p2.y + y_ofs;
            }

            lv_draw_line(layer, &line_dsc);
            line_dsc.round_start = 0;   /*Draw the rounding only on the end points after the first line*/
        }
    }
}
#endif
