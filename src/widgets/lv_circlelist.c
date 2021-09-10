/**
 * @file lv_circle_list.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_circlelist.h"
#if LV_USE_CIRCLELIST != 0

#include "../core/lv_indev.h"
#include "../draw/lv_draw.h"
#include "../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_circlelist_class

#ifdef LV_USE_LARGE_COORD
typedef int64_t lv_coord_super_t;
#else
typedef int32_t lv_coord_super_t;
#endif
#define SCALE_FLOAT_TO_INT 1024

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_circlelist_constructor(const lv_obj_class_t* class_p,
                                      lv_obj_t* obj);
static void lv_circlelist_destructor(const lv_obj_class_t* class_p,
                                     lv_obj_t* obj);
static void lv_circlelist_event(const lv_obj_class_t* class_p, lv_event_t* e);

static void init_circle_list_info(lv_obj_t* obj);
static void lv_circlelist_move(lv_obj_t* obj, lv_point_t* pos);
static void lv_circlelist_layout_init(lv_obj_t* obj);

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_obj_class_t lv_circlelist_class = {
    .constructor_cb = lv_circlelist_constructor,
    .destructor_cb = lv_circlelist_destructor,
    .event_cb = lv_circlelist_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_circlelist_t),
    .base_class = &lv_obj_class,
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t* lv_circlelist_create(lv_obj_t* parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t* obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Add item to list, item must have fixed size before adding to list.
 */
void lv_circlelist_add(lv_obj_t* obj, lv_obj_t* child, lv_coord_t h)
{
    lv_circlelist_t* list = (lv_circlelist_t*)obj;
    if (list->radius <= 0) {
        init_circle_list_info(obj);
    }

    lv_circlelist_item_t* items;
    items = lv_mem_realloc(list->items,
                           sizeof(lv_circlelist_item_t) * (list->count + 1));
    if (items == NULL) {
        LV_LOG_ERROR("no memory.\n");
        return;
    }
    list->items = items;

    lv_circlelist_item_t* item = &items[list->count];
    item->obj = child;
    item->width = list->radius * 2;
    item->height = h;

    list->count++;

    /* Need to pass pressing event to circle_list object. */
    lv_obj_clear_flag(child, LV_OBJ_FLAG_CLICKABLE);

    lv_circlelist_layout_init(obj);
}

/*=====================
 * Getter functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_circlelist_constructor(const lv_obj_class_t* class_p,
                                      lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    /**
     * Scroll is not supported as it conflicts circle layout.
     */
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLL_ELASTIC);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_radius(obj, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_pad_column(obj, 10, 0); /* Default gap to 10px. */

    lv_circlelist_t* list = (lv_circlelist_t*)obj;
    list->count = 0;
    list->items = NULL;
    list->focus = NULL;
    list->press_point.x = 0;
    list->press_point.y = 0;
    list->radius = -1;
    list->start.x = 0;
    list->start.y = 0;

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_circlelist_destructor(const lv_obj_class_t* class_p,
                                     lv_obj_t* obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_circlelist_t* list = (lv_circlelist_t*)obj;
    if (list->items)
        lv_mem_free(list->items);
    list->count = 0;
    LV_TRACE_OBJ_CREATE("finished");
}

static lv_coord_t get_item_pos_y(lv_circlelist_item_t* item)
{
    return lv_obj_get_style_y(item->obj, 0);
}

static lv_coord_t get_item_height(lv_circlelist_item_t* item)
{
    return lv_obj_get_style_height(item->obj, 0);
}

static lv_coord_t get_list_item_gap(lv_obj_t* obj)
{
    return lv_obj_get_style_pad_column(obj, 0);
}

static void init_circle_list_info(lv_obj_t* obj)
{
    lv_circlelist_t* list = (lv_circlelist_t*)obj;
    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);

    w -= lv_obj_get_style_pad_left(obj, 0);
    w -= lv_obj_get_style_pad_right(obj, 0);
    h -= lv_obj_get_style_pad_top(obj, 0);
    h -= lv_obj_get_style_pad_bottom(obj, 0);

    list->radius = LV_MIN(w, h) / 2 - lv_obj_get_style_border_width(obj, 0);
    list->start.x = lv_obj_get_style_pad_left(obj, 0);
    list->start.y = lv_obj_get_style_pad_top(obj, 0);

    list->center.x = list->start.x + list->radius;
    list->center.y = list->start.y + list->radius;

    LV_LOG_TRACE("list radius: %d, center: (%d, %d)\n", list->radius,
                 list->center.x, list->center.y);
}

static bool is_point_inside_circle(lv_circlelist_t* list, lv_coord_t x,
                                   lv_coord_t y)
{
    if (LV_ABS(x - list->center.x) > list->radius
        || LV_ABS(y - list->center.y) > list->radius)
        return false;

    lv_coord_super_t radius = list->radius;
    lv_coord_super_t yc = list->center.y;
    lv_coord_super_t xc = list->center.x;
    lv_coord_super_t distance = (xc - x) * (xc - x) + (yc - y) * (yc - y);
    distance = (distance * (lv_coord_super_t)(0.98f * 32768)) >> 15;
    return distance < radius * radius;
}

/* Return scale * SCALE_FLOAT_TO_INT */
static lv_coord_super_t obj_layout_get_scale(lv_coord_t _radius,
                                             lv_coord_t _press_y, lv_coord_t _h,
                                             lv_coord_t _yc, float ref_y)
{
    /**
     * The circle list follows three rules:
     * 1. scale.
     *    The item is scaled to make sure the it's always inside the circle.
     *    scale = (Xcenter - X1) / radius;
     * 2. path.
     *    The left-up corner or left-down corner should always in a circle.
     *    (X1 - Xcenter)^2 + (Y1 - Ycenter)^2 = radius^2, or:
     *    (X1 - Xcenter)^2 + (Y2 - Ycenter)^2 = radius^2, or:
     * 3. item pressed follows indev.
     *    The item being pressed should always moves along with input device.
     *    Assume the point being pressed inside the item is ref_f which is
     *    normalized to 1. So, 1.0 means pressing down edge of item and 0.5
     *    means center.
     *    Assume the input device moves to position press_y.
     *    then Y1 = press_y - ref_y * scale * height_original
     * Then the problem is to solve scale and check if point (x1, y1) and
     * (x1, y2) is inside the circle. If so, the scale value is valid.
     */
    lv_coord_super_t radius = _radius;
    lv_coord_super_t press_y = _press_y;
    lv_coord_super_t h = _h;
    lv_coord_super_t yc = _yc;
    lv_coord_super_t scale;

    lv_coord_super_t a = radius * radius;
    lv_coord_super_t b = press_y - yc;
    lv_coord_super_t c = ref_y * h;
    lv_coord_super_t bc = b * c;
    lv_coord_super_t a_c2 = a + c * c;
    /* clang-format off */
    lv_coord_super_t square = (SCALE_FLOAT_TO_INT * SCALE_FLOAT_TO_INT * bc * bc) / (a_c2 * a_c2);
    square -= ((b * b - a) * SCALE_FLOAT_TO_INT * SCALE_FLOAT_TO_INT) / a_c2;
    /* clang-format on */
    lv_coord_super_t residue = (SCALE_FLOAT_TO_INT * bc) / a_c2;

    lv_sqrt_res_t res;
    lv_sqrt(square, &res, 0x8000);
    square = res.i + (res.f >> 8);
    lv_coord_super_t scale_small = -square + residue;
    lv_coord_super_t scale_large = square + residue;

    /* Only value between 0 to 1.0f is valid. */
    if (scale_small > 0 && scale_small <= SCALE_FLOAT_TO_INT) {
        scale = scale_small;
    } else if (scale_large > 0 && scale_large <= SCALE_FLOAT_TO_INT) {
        scale = scale_large;
    } else {
        return 0;
    }
    return scale;
}

static bool obj_layout_based_on_y1(lv_circlelist_t* list,
                                   lv_circlelist_item_t* item, float ref_y,
                                   lv_coord_t y)
{
    /**
     * Layout based on y1 position, to limit it on circle edge.
     */
    lv_obj_t* child = item->obj;
    lv_coord_super_t scale;
    scale = obj_layout_get_scale(list->radius, y, item->height, list->center.y,
                                 ref_y);

    lv_coord_t x1 = list->center.x - scale * list->radius / SCALE_FLOAT_TO_INT;
    lv_coord_t y1 = y - ref_y * scale * item->height / SCALE_FLOAT_TO_INT;
    lv_coord_t y2 = y1 + scale * item->height / SCALE_FLOAT_TO_INT;

    /* (x1, y2) must fall inside the circle. */
    if (!is_point_inside_circle(list, x1, y2)) {
        return false;
    }

    lv_obj_set_pos(child, x1, y1);
    lv_coord_t w = item->width * scale / SCALE_FLOAT_TO_INT;
    lv_coord_t h = item->height * scale / SCALE_FLOAT_TO_INT;
    lv_obj_set_size(child, w, h);
    return true;
}

static bool obj_layout_based_on_y2(lv_circlelist_t* list,
                                   lv_circlelist_item_t* item, float ref_y,
                                   lv_coord_t y)
{
    /**
     * Layout based on y2 position, to limit it on circle edge.
     */
    lv_obj_t* child = item->obj;
    lv_coord_super_t scale;
    scale = obj_layout_get_scale(list->radius, y, item->height, list->center.y,
                                 -(1 - ref_y));
    lv_coord_t x1 = list->center.x - scale * list->radius / SCALE_FLOAT_TO_INT;
    lv_coord_t y1 = y - ref_y * scale * item->height / SCALE_FLOAT_TO_INT;
    lv_coord_t y2 = y1 + scale * item->height / SCALE_FLOAT_TO_INT;

    /* (x1, y2) must fall inside the circle. */
    if (!is_point_inside_circle(list, x1, y2)) {
        return false;
    }

    lv_obj_set_pos(child, x1, y1);

    lv_coord_t w = item->width * scale / SCALE_FLOAT_TO_INT;
    lv_coord_t h = item->height * scale / SCALE_FLOAT_TO_INT;
    lv_obj_set_size(child, w, h);
    return true;
}

/**
 * Move object by moving reference point ref_y to position y.
 */
static void move_obj_to(lv_circlelist_t* list, lv_circlelist_item_t* item,
                        float ref_y, lv_coord_t y)
{
    if (obj_layout_based_on_y1(list, item, ref_y, y)) {
        return;
    }

    if (obj_layout_based_on_y2(list, item, ref_y, y)) {
        return;
    }
    lv_obj_set_size(item->obj, 0, 0);
}

/**
 * Move focused object whoese reference point is decided by press point.
 */
static void focused_obj_layout(lv_obj_t* obj)
{
    lv_circlelist_t* list = (lv_circlelist_t*)obj;
    if (list->focus == NULL)
        return;
    lv_circlelist_item_t* item = list->focus;
    move_obj_to(list, item, list->focus_y, list->press_point.y);
}

static void lv_circlelist_set_focus(lv_obj_t* obj, lv_point_t* pos)
{
    lv_circlelist_t* list = (lv_circlelist_t*)obj;
    bool found = false;
    int i;

    lv_point_t pos_abs;
    lv_coord_t gap = get_list_item_gap(obj);
    pos_abs.x = pos->x + obj->coords.x1;
    pos_abs.y = pos->y + obj->coords.y1;
    for (i = 0; i < list->count; i++) {
        lv_area_t* coord = &list->items[i].obj->coords;
        if (pos_abs.y >= coord->y1 && pos_abs.y <= coord->y2 + gap) {
            found = true;
            break;
        }
    }

    if (!found) {
        return;
    }

    lv_circlelist_item_t* item = &list->items[i];
    list->focus = item;
    list->focus_item_index = i;

    lv_obj_t* focused = item->obj;
    lv_coord_t h = lv_obj_get_height(focused);

    list->focus_y = pos_abs.y - focused->coords.y1;
    list->focus_y /= h;
}

/**
 * List has scrolled, 'pos' is the latest mouse position.
 */
static void lv_circlelist_move(lv_obj_t* obj, lv_point_t* pos)
{
    lv_circlelist_t* list = (lv_circlelist_t*)obj;

    list->press_point = *pos;

    if (!list->focus)
        return;

    focused_obj_layout(obj);

    lv_coord_t gap = get_list_item_gap(obj);
    /* For objects above focused object. */
    if (list->focus_item_index > 0) {
        lv_coord_t abs_y = get_item_pos_y(list->focus);
        int i;
        i = list->focus_item_index - 1;
        for (; i >= 0; i--) {
            abs_y -= gap;
            move_obj_to(list, &list->items[i], 1.0f, abs_y);
            abs_y -= get_item_height(&list->items[i]);
        }
    }

    /* For objects below focused object */
    if (list->focus_item_index < list->count - 1) {
        lv_coord_t abs_y = get_item_pos_y(list->focus);
        abs_y += get_item_height(list->focus);
        int i;
        i = list->focus_item_index + 1;
        for (; i < list->count; i++) {
            abs_y += gap;
            move_obj_to(list, &list->items[i], 0.0f, abs_y);
            abs_y += get_item_height(&list->items[i]);
        }
    }
}

static void lv_circlelist_scroll_end(lv_obj_t* obj, lv_point_t* pos)
{
    lv_circlelist_t* list = (lv_circlelist_t*)obj;

    list->focus = NULL;
    list->focus_y = 0;
    list->focus_item_index = 0;
}

static void lv_circlelist_event(const lv_obj_class_t* class_p, lv_event_t* e)
{
    LV_UNUSED(class_p);

    lv_res_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if (res != LV_RES_OK)
        return;

    lv_obj_t* obj = lv_event_get_current_target(e);
    lv_circlelist_t* list = (lv_circlelist_t*)obj;

    if (list->radius < 0) {
        init_circle_list_info(obj);
    }

    lv_event_code_t code = lv_event_get_code(e);

    lv_indev_t* indev = lv_indev_get_act();

    lv_point_t point;
    lv_indev_get_point(indev, &point);

    /* Convert to relative position. */
    point.y -= obj->coords.y1;
    point.x -= obj->coords.x1;

    switch (code) {
    case LV_EVENT_PRESSED: {
        lv_circlelist_set_focus(obj, &point);
        break;
    }
    case LV_EVENT_PRESSING: {
        if (list->press_point.y != point.y)
            lv_circlelist_move(obj, &point);
        break;
    }
    case LV_EVENT_RELEASED: {
        lv_circlelist_scroll_end(obj, &point);
        break;
    }

    case LV_EVENT_SCROLL:
    case LV_EVENT_SCROLL_BEGIN:
    case LV_EVENT_SCROLL_END:
        break;
    default:
        break;
    }
}

static void lv_circlelist_layout_init(lv_obj_t* obj)
{
    lv_circlelist_t* list = (lv_circlelist_t*)obj;

    if (!list->count)
        return;
    list->focus = &list->items[0];
    list->focus_item_index = 0;
    list->focus_y = 0.5f;
    list->press_point.x = 0;
    lv_point_t pos;
    pos.x = 0;
    pos.y = list->start.y + list->radius;
    lv_circlelist_move(obj, &pos);
}
#endif
