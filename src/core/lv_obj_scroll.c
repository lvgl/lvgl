/**
 * @file lv_obj_scroll.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj_scroll.h"
#include "lv_obj.h"
#include "lv_indev.h"
#include "lv_disp.h"
#include "lv_indev_scroll.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_obj_class
#define SCROLL_ANIM_TIME_MIN    200    /*ms*/
#define SCROLL_ANIM_TIME_MAX    400    /*ms*/
#define SCROLLBAR_MIN_SIZE      (LV_DPX(10))

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  GLOBAL PROTOTYPES
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void scroll_by_raw(lv_obj_t * obj, lv_coord_t x, lv_coord_t y);
static void scroll_x_anim(void * obj, int32_t v);
static void scroll_y_anim(void * obj, int32_t v);
static void scroll_anim_ready_cb(lv_anim_t * a);
static void scroll_area_into_view(const lv_area_t * area, lv_obj_t * child, lv_point_t * scroll_value, lv_anim_enable_t anim_en);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*=====================
 * Setter functions
 *====================*/

void lv_obj_set_scrollbar_mode(lv_obj_t * obj, lv_scrollbar_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_allocate_spec_attr(obj);

    if(obj->spec_attr->scrollbar_mode == mode) return;
    obj->spec_attr->scrollbar_mode = mode;
    lv_obj_invalidate(obj);
}

void lv_obj_set_scroll_dir(lv_obj_t * obj, lv_dir_t dir)
{
    lv_obj_allocate_spec_attr(obj);

    if(dir != obj->spec_attr->scroll_dir) {
        obj->spec_attr->scroll_dir = dir;
    }
}

void lv_obj_set_scroll_snap_x(lv_obj_t * obj, lv_scroll_snap_t align)
{
    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->scroll_snap_x = align;
}

void lv_obj_set_scroll_snap_y(lv_obj_t * obj, lv_scroll_snap_t align)
{
    lv_obj_allocate_spec_attr(obj);
    obj->spec_attr->scroll_snap_y = align;
}

/*=====================
 * Getter functions
 *====================*/

lv_scrollbar_mode_t lv_obj_get_scrollbar_mode(const lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->scrollbar_mode;
    else return LV_SCROLLBAR_MODE_AUTO;
}

lv_dir_t lv_obj_get_scroll_dir(const lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->scroll_dir;
    else return LV_DIR_ALL;
}

lv_scroll_snap_t lv_obj_get_scroll_snap_x(const lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->scroll_snap_x;
    else return LV_SCROLL_SNAP_NONE;
}

lv_scroll_snap_t lv_obj_get_scroll_snap_y(const lv_obj_t * obj)
{
    if(obj->spec_attr) return obj->spec_attr->scroll_snap_y;
    else return LV_SCROLL_SNAP_NONE;
}

lv_coord_t lv_obj_get_scroll_x(const lv_obj_t * obj)
{
    if(obj->spec_attr == NULL) return 0;
    return -obj->spec_attr->scroll.x;
}

lv_coord_t lv_obj_get_scroll_y(const lv_obj_t * obj)
{
    if(obj->spec_attr == NULL) return 0;
    return -obj->spec_attr->scroll.y;
}

lv_coord_t lv_obj_get_scroll_top(lv_obj_t * obj)
{
    if(obj->spec_attr == NULL) return 0;
    return -obj->spec_attr->scroll.y;
}

lv_coord_t lv_obj_get_scroll_bottom(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_coord_t child_res = LV_COORD_MIN;
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(lv_obj_has_flag_any(child,  LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
        child_res = LV_MAX(child_res, child->coords.y2);
    }

    lv_coord_t pad_top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
    lv_coord_t pad_bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);

    if(child_res != LV_COORD_MIN) {
        child_res -= (obj->coords.y2 - pad_bottom - border_width);
    }

    lv_coord_t self_h = lv_obj_get_self_height(obj);
    self_h = self_h - (lv_obj_get_height(obj) - pad_top - pad_bottom - 2 * border_width);
    self_h -= lv_obj_get_scroll_y(obj);
    return LV_MAX(child_res, self_h);
}

lv_coord_t lv_obj_get_scroll_left(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*Normally can't scroll the object out on the left.
     *So simply use the current scroll position as "left size"*/
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) != LV_BASE_DIR_RTL) {
        if(obj->spec_attr == NULL) return 0;
        return -obj->spec_attr->scroll.x;
    }

    /*With RTL base direction scrolling the left is normal so find the left most coordinate*/
    lv_coord_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);

    lv_coord_t child_res = 0;

    uint32_t i;
    lv_coord_t x1 = LV_COORD_MAX;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
       lv_obj_t * child = lv_obj_get_child(obj, i);
       if(lv_obj_has_flag_any(child,  LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
        x1 = LV_MIN(x1, child->coords.x1);

    }

    if(x1 != LV_COORD_MAX) {
        child_res = x1;
        child_res = (obj->coords.x1 + pad_left + border_width) - child_res;
    } else {
        child_res = LV_COORD_MIN;
    }

    lv_coord_t self_w = lv_obj_get_self_width(obj);
    self_w = self_w - (lv_obj_get_width(obj) - pad_right - pad_left - 2 * border_width);
    self_w += lv_obj_get_scroll_x(obj);

    return LV_MAX(child_res, self_w);
}

lv_coord_t lv_obj_get_scroll_right(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    /*With RTL base dir can't scroll to the object out on the right.
     *So simply use the current scroll position as "right size"*/
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) == LV_BASE_DIR_RTL) {
        if(obj->spec_attr == NULL) return 0;
        return obj->spec_attr->scroll.x;
    }

    /*With other base direction (LTR) scrolling to the right is normal so find the right most coordinate*/
    lv_coord_t child_res = LV_COORD_MIN;
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_cnt(obj); i++) {
        lv_obj_t * child = lv_obj_get_child(obj, i);
        if(lv_obj_has_flag_any(child,  LV_OBJ_FLAG_HIDDEN | LV_OBJ_FLAG_FLOATING)) continue;
        child_res = LV_MAX(child_res, child->coords.x2);
    }

    lv_coord_t pad_right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
    lv_coord_t pad_left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);

    if(child_res != LV_COORD_MIN) {
        child_res -= (obj->coords.x2 - pad_right - border_width);
    }

    lv_coord_t self_w;
    self_w = lv_obj_get_self_width(obj);
    self_w = self_w - (lv_obj_get_width(obj) - pad_right - pad_left - 2 * border_width);
    self_w -= lv_obj_get_scroll_x(obj);
    return LV_MAX(child_res, self_w);
}

void lv_obj_get_scroll_end(struct _lv_obj_t  * obj, lv_point_t * end)
{
    lv_anim_t * a;
    a = lv_anim_get(obj, scroll_x_anim);
    end->x = a ? -a->end_value : lv_obj_get_scroll_x(obj);

    a = lv_anim_get(obj, scroll_y_anim);
    end->y = a ? -a->end_value : lv_obj_get_scroll_y(obj);
}

/*=====================
 * Other functions
 *====================*/

void lv_obj_scroll_by(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)
{
    if(x == 0 && y == 0) return;
    if(anim_en == LV_ANIM_ON) {
        lv_disp_t * d = lv_obj_get_disp(obj);
        lv_anim_t a;
        lv_anim_init(&a);
        lv_anim_set_var(&a, obj);
        lv_anim_set_ready_cb(&a, scroll_anim_ready_cb);

        if(x) {
            uint32_t t = lv_anim_speed_to_time((lv_disp_get_hor_res(d) * 2) >> 2, 0, x);
            if(t < SCROLL_ANIM_TIME_MIN) t = SCROLL_ANIM_TIME_MIN;
            if(t > SCROLL_ANIM_TIME_MAX) t = SCROLL_ANIM_TIME_MAX;
            lv_anim_set_time(&a, t);
            lv_coord_t sx = lv_obj_get_scroll_x(obj);
            lv_anim_set_values(&a, -sx, -sx + x);
            lv_anim_set_exec_cb(&a, scroll_x_anim);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_out);

            lv_res_t res;
            res = lv_event_send(obj, LV_EVENT_SCROLL_BEGIN, &a);
            if(res != LV_RES_OK) return;
            lv_anim_start(&a);
        }

        if(y) {
            uint32_t t = lv_anim_speed_to_time((lv_disp_get_ver_res(d) * 2) >> 2, 0, y);
            if(t < SCROLL_ANIM_TIME_MIN) t = SCROLL_ANIM_TIME_MIN;
            if(t > SCROLL_ANIM_TIME_MAX) t = SCROLL_ANIM_TIME_MAX;
            lv_anim_set_time(&a, t);
            lv_coord_t sy = lv_obj_get_scroll_y(obj);
            lv_anim_set_values(&a, -sy, -sy + y);
            lv_anim_set_exec_cb(&a,  scroll_y_anim);
            lv_anim_set_path_cb(&a, lv_anim_path_ease_out);
            
            lv_res_t res;
            res = lv_event_send(obj, LV_EVENT_SCROLL_BEGIN, &a);
            if(res != LV_RES_OK) return;
            lv_anim_start(&a);
        }
    } else {
        /*Remove pending animations*/
        bool y_del = lv_anim_del(obj, scroll_y_anim);
        bool x_del = lv_anim_del(obj, scroll_x_anim);
        scroll_by_raw(obj, x, y);
        if(y_del || x_del) {
            lv_res_t res;
            res = lv_event_send(obj, LV_EVENT_SCROLL_END, NULL);
            if(res != LV_RES_OK) return;
        }
    }
}

void lv_obj_scroll_to(lv_obj_t * obj, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim_en)
{
    lv_obj_scroll_to_x(obj, x, anim_en);
    lv_obj_scroll_to_y(obj, y, anim_en);
}

void lv_obj_scroll_to_x(lv_obj_t * obj, lv_coord_t x, lv_anim_enable_t anim_en)
{
    lv_anim_del(obj, scroll_x_anim);

    /*Don't let scroll more then naturally possible by the size of the content*/
    if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) != LV_BASE_DIR_RTL) {
        if(x < 0) x = 0;
        if(x > 0) {
            lv_coord_t  scroll_max = lv_obj_get_scroll_left(obj) + lv_obj_get_scroll_right(obj);
            if(scroll_max < 0) scroll_max = 0;

            if(x > scroll_max) x = scroll_max;
        }
    } else {
        if(x > 0) x = 0;
        if(x < 0) {
            lv_coord_t  scroll_max = lv_obj_get_scroll_left(obj) + lv_obj_get_scroll_right(obj);
            if(scroll_max < 0) scroll_max = 0;

            if(x < -scroll_max) x = -scroll_max;
        }
    }

    lv_coord_t scroll_x = lv_obj_get_scroll_x(obj);
    lv_coord_t diff = -x + scroll_x;

    lv_obj_scroll_by(obj, diff, 0, anim_en);
}

void lv_obj_scroll_to_y(lv_obj_t * obj, lv_coord_t y, lv_anim_enable_t anim_en)
{
    lv_anim_del(obj, scroll_y_anim);

    /*Don't let scroll more then naturally possible by the size of the content*/
    if(y < 0) y = 0;
    if(y > 0) {
        lv_coord_t  scroll_max = lv_obj_get_scroll_top(obj) + lv_obj_get_scroll_bottom(obj);
        if(scroll_max < 0) scroll_max = 0;
        if(y > scroll_max) y = scroll_max;
    }

    lv_coord_t scroll_y = lv_obj_get_scroll_y(obj);
    lv_coord_t diff = -y + scroll_y;

    lv_obj_scroll_by(obj, 0, diff, anim_en);
}

void lv_obj_scroll_to_view(lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    /*Be sure the screens layout is correct*/
    lv_obj_update_layout(obj);

    lv_point_t p = {0, 0};
    scroll_area_into_view(&obj->coords, obj, &p, anim_en);
}

void lv_obj_scroll_to_view_recursive(lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    /*Be sure the screens layout is correct*/
    lv_obj_update_layout(obj);

    lv_point_t p = {0, 0};
    lv_obj_t * child = obj;
    lv_obj_t * parent = lv_obj_get_parent(child);
    while(parent) {
        scroll_area_into_view(&obj->coords, child, &p, anim_en);
        child = parent;
        parent = lv_obj_get_parent(parent);
    }
}

bool lv_obj_is_scrolling(const lv_obj_t * obj)
{
    lv_indev_t * indev = lv_indev_get_next(NULL);
    while(indev) {
        if(lv_indev_get_scroll_obj(indev) == obj) return true;
        indev = lv_indev_get_next(indev);
    }

    return false;
}

void lv_obj_update_snap(lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    lv_obj_update_layout(obj);
    lv_point_t p;
    lv_indev_scroll_get_snap_dist(obj, &p);
    lv_obj_scroll_by(obj, p.x, p.y, anim_en);
}

void lv_obj_get_scrollbar_area(lv_obj_t * obj, lv_area_t * hor_area, lv_area_t * ver_area)
{
    lv_area_set(hor_area, 0, 0, -1, -1);
    lv_area_set(ver_area, 0, 0, -1, -1);

    if(lv_obj_has_flag(obj, LV_OBJ_FLAG_SCROLLABLE) == false) return;

    lv_dir_t sm = lv_obj_get_scrollbar_mode(obj);
    if(sm == LV_SCROLLBAR_MODE_OFF)  return;

    /*If there is no indev scrolling this object but `mode==active` return*/
    lv_indev_t * indev = lv_indev_get_next(NULL);
    if(sm == LV_SCROLLBAR_MODE_ACTIVE) {
        while(indev) {
            if(lv_indev_get_scroll_obj(indev) == obj) break;
            indev = lv_indev_get_next(indev);
        }
        if(indev == NULL)  return;
    }

    lv_coord_t st = lv_obj_get_scroll_top(obj);
    lv_coord_t sb = lv_obj_get_scroll_bottom(obj);
    lv_coord_t sl = lv_obj_get_scroll_left(obj);
    lv_coord_t sr = lv_obj_get_scroll_right(obj);

    lv_dir_t dir = lv_obj_get_scroll_dir(obj);

    bool ver_draw = false;
    if((dir & LV_DIR_VER) &&
            ((sm == LV_SCROLLBAR_MODE_ON) ||
                    (sm == LV_SCROLLBAR_MODE_AUTO && (st > 0 || sb > 0)) ||
                    (sm == LV_SCROLLBAR_MODE_ACTIVE && lv_indev_get_scroll_dir(indev) == LV_DIR_VER))) {
        ver_draw = true;
    }


    bool hor_draw = false;
    if((dir & LV_DIR_HOR) &&
            ((sm == LV_SCROLLBAR_MODE_ON) ||
                    (sm == LV_SCROLLBAR_MODE_AUTO && (sl > 0 || sr > 0)) ||
                    (sm == LV_SCROLLBAR_MODE_ACTIVE && lv_indev_get_scroll_dir(indev) == LV_DIR_HOR))) {
        hor_draw = true;
    }

    if(!hor_draw && !ver_draw) return;

    lv_coord_t end_space = lv_obj_get_style_pad_top(obj, LV_PART_SCROLLBAR);
    lv_coord_t side_space = lv_obj_get_style_pad_right(obj, LV_PART_SCROLLBAR);
    lv_coord_t tickness = lv_obj_get_style_width(obj, LV_PART_SCROLLBAR);

    lv_coord_t obj_h = lv_obj_get_height(obj);
    lv_coord_t obj_w = lv_obj_get_width(obj);

    lv_coord_t ver_reg_space = ver_draw ? tickness + side_space : 0;
    lv_coord_t hor_req_space = hor_draw ? tickness + side_space : 0;
    lv_coord_t rem;

    if(lv_obj_get_style_bg_opa(obj, LV_PART_SCROLLBAR) < LV_OPA_MIN &&
       lv_obj_get_style_border_opa(obj, LV_PART_SCROLLBAR) < LV_OPA_MIN) {
        return;
    }

    /*Draw horizontal scrollbar if the mode is ON or can be scrolled in this direction*/
    lv_coord_t content_h = obj_h + st + sb;
    if(ver_draw && content_h) {
       ver_area->y1 = obj->coords.y1;
       ver_area->y2 = obj->coords.y2;
       ver_area->x2 = obj->coords.x2 - side_space;
       ver_area->x1 =ver_area->x2 - tickness;

        lv_coord_t sb_h = ((obj_h - end_space * 2 - hor_req_space) * obj_h) / content_h;
        sb_h = LV_MAX(sb_h, SCROLLBAR_MIN_SIZE);
        rem = (obj_h - end_space * 2 - hor_req_space) - sb_h;  /*Remaining size from the scrollbar track that is not the scrollbar itself*/
        lv_coord_t scroll_h = content_h - obj_h; /*The size of the content which can be really scrolled*/
        if(scroll_h <= 0) {
           ver_area->y1 = obj->coords.y1 + end_space;
           ver_area->y2 = obj->coords.y2 - end_space - hor_req_space - 1;
           ver_area->x2 = obj->coords.x2 - side_space;
           ver_area->x1 =ver_area->x2 - tickness + 1;
        } else {
            lv_coord_t sb_y = (rem * sb) / scroll_h;
            sb_y = rem - sb_y;

           ver_area->y1 = obj->coords.y1 + sb_y + end_space;
           ver_area->y2 =ver_area->y1 + sb_h - 1;
           ver_area->x2 = obj->coords.x2 - side_space;
           ver_area->x1 =ver_area->x2 - tickness;
            if(ver_area->y1 < obj->coords.y1 + end_space) {
               ver_area->y1 = obj->coords.y1 + end_space;
                if(ver_area->y1 + SCROLLBAR_MIN_SIZE >ver_area->y2)ver_area->y2 =ver_area->y1 + SCROLLBAR_MIN_SIZE;
            }
            if(ver_area->y2 > obj->coords.y2 - hor_req_space - end_space) {
               ver_area->y2 = obj->coords.y2 - hor_req_space - end_space;
                if(ver_area->y2 - SCROLLBAR_MIN_SIZE <ver_area->y1)ver_area->y1 =ver_area->y2 - SCROLLBAR_MIN_SIZE;
            }
        }
    }

    /*Draw horizontal scrollbar if the mode is ON or can be scrolled in this direction*/
    lv_coord_t content_w = obj_w + sl + sr;
    if(hor_draw && content_w) {
        hor_area->y2 = obj->coords.y2 - side_space;
        hor_area->y1 = hor_area->y2 - tickness;
        hor_area->x1 = obj->coords.x1;
        hor_area->x2 = obj->coords.x2;

        lv_coord_t sb_w = ((obj_w - end_space * 2 - ver_reg_space) * obj_w) / content_w;
        sb_w = LV_MAX(sb_w, SCROLLBAR_MIN_SIZE);
        rem = (obj_w - end_space * 2 - ver_reg_space) - sb_w;  /*Remaining size from the scrollbar track that is not the scrollbar itself*/
        lv_coord_t scroll_w = content_w - obj_w; /*The size of the content which can be really scrolled*/
        if(scroll_w <= 0) {
            hor_area->y2 = obj->coords.y2 - side_space;
            hor_area->y1 = hor_area->y2 - tickness + 1;
            hor_area->x1 = obj->coords.x1 + end_space;
            hor_area->x2 = obj->coords.x2 - end_space - ver_reg_space - 1;
        } else {
            lv_coord_t sb_x = (rem * sr) / scroll_w;
            sb_x = rem - sb_x;

            hor_area->x1 = obj->coords.x1 + sb_x + end_space;
            hor_area->x2 = hor_area->x1 + sb_w - 1;
            hor_area->y2 = obj->coords.y2 - side_space;
            hor_area->y1 = hor_area->y2 - tickness;
            if(hor_area->x1 < obj->coords.x1 + end_space) {
                hor_area->x1 = obj->coords.x1 + end_space;
                if(hor_area->x1 + SCROLLBAR_MIN_SIZE > hor_area->x2) hor_area->x2 = hor_area->x1 + SCROLLBAR_MIN_SIZE;
            }
            if(hor_area->x2 > obj->coords.x2 - ver_reg_space - end_space) {
                hor_area->x2 = obj->coords.x2 - ver_reg_space - end_space;
                if(hor_area->x2 - SCROLLBAR_MIN_SIZE < hor_area->x1) hor_area->x1 = hor_area->x2 - SCROLLBAR_MIN_SIZE;
            }
        }
    }
}

void lv_obj_scrollbar_invalidate(lv_obj_t * obj)
{
    lv_area_t hor_area;
    lv_area_t ver_area;
    lv_obj_get_scrollbar_area(obj, &hor_area, &ver_area);

    if(lv_area_get_size(&hor_area) <= 0 && lv_area_get_size(&ver_area) <= 0) return;

    if(lv_area_get_size(&hor_area) > 0) lv_obj_invalidate_area(obj, &hor_area);
    if(lv_area_get_size(&ver_area) > 0) lv_obj_invalidate_area(obj, &ver_area);
}

void lv_obj_readjust_scroll(lv_obj_t * obj, lv_anim_enable_t anim_en)
{
    /*Be sure the bottom side is not remains scrolled in*/
    /*With snapping the content can't be scrolled in*/
    if(lv_obj_get_scroll_snap_y(obj) == LV_SCROLL_SNAP_NONE) {
        lv_coord_t st = lv_obj_get_scroll_top(obj);
        lv_coord_t sb = lv_obj_get_scroll_bottom(obj);
        if(sb < 0 && st > 0) {
            sb = LV_MIN(st, -sb);
            lv_obj_scroll_by(obj, 0, sb, anim_en);
        }
    }

    if(lv_obj_get_scroll_snap_x(obj) == LV_SCROLL_SNAP_NONE) {
        lv_coord_t sl = lv_obj_get_scroll_left(obj);
        lv_coord_t sr = lv_obj_get_scroll_right(obj);
        if(lv_obj_get_style_base_dir(obj, LV_PART_MAIN) != LV_BASE_DIR_RTL) {
            /*Be sure the left side is not remains scrolled in*/
            if(sr < 0 && sl > 0) {
                sr = LV_MIN(sl, -sr);
                lv_obj_scroll_by(obj, sr, 0, anim_en);
            }
        } else {
            /*Be sure the right side is not remains scrolled in*/
            if(sl < 0 && sr > 0) {
                sr = LV_MIN(sr, -sl);
                lv_obj_scroll_by(obj, sl, 0, anim_en);
            }
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void scroll_by_raw(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    if(x == 0 && y == 0) return;

    lv_obj_allocate_spec_attr(obj);

    obj->spec_attr->scroll.x += x;
    obj->spec_attr->scroll.y += y;

    lv_obj_move_children_by(obj, x, y, true);
    lv_res_t res = lv_event_send(obj, LV_EVENT_SCROLL, NULL);
    if(res != LV_RES_OK) return;
    lv_obj_invalidate(obj);
}

static void scroll_x_anim(void * obj, int32_t v)
{
    scroll_by_raw(obj, v + lv_obj_get_scroll_x(obj), 0);
}

static void scroll_y_anim(void * obj, int32_t v)
{
    scroll_by_raw(obj, 0, v + lv_obj_get_scroll_y(obj));
}

static void scroll_anim_ready_cb(lv_anim_t * a)
{
    lv_event_send(a->var, LV_EVENT_SCROLL_END, NULL);
}

static void scroll_area_into_view(const lv_area_t * area, lv_obj_t * child, lv_point_t * scroll_value, lv_anim_enable_t anim_en)
{
    lv_obj_t * parent = lv_obj_get_parent(child);
    lv_dir_t scroll_dir = lv_obj_get_scroll_dir(parent);
    lv_coord_t snap_goal = 0;
    lv_coord_t act = 0;
    const lv_area_t * area_tmp;

    lv_coord_t y_scroll = 0;
    lv_scroll_snap_t snap_y = lv_obj_get_scroll_snap_y(parent);
    if(snap_y != LV_SCROLL_SNAP_NONE) area_tmp = &child->coords;
    else area_tmp = area;

    lv_coord_t ptop = lv_obj_get_style_pad_top(parent, LV_PART_MAIN);
    lv_coord_t pbottom = lv_obj_get_style_pad_bottom(parent, LV_PART_MAIN);
    lv_coord_t top_diff = parent->coords.y1 + ptop - area_tmp->y1 - scroll_value->y;
    lv_coord_t bottom_diff = -(parent->coords.y2 - pbottom - area_tmp->y2 - scroll_value->y);
    lv_coord_t parent_h = lv_obj_get_height(parent) - ptop - pbottom;
    if((top_diff > 0 && bottom_diff > 0)) y_scroll = 0;
    else if(top_diff > 0) {
        y_scroll = top_diff;
        /*Do not let scrolling in*/
        lv_coord_t st = lv_obj_get_scroll_top(parent);
        if(st - y_scroll < 0) y_scroll = 0;
    }
    else if(bottom_diff > 0) {
        y_scroll = -bottom_diff;
        /*Do not let scrolling in*/
        lv_coord_t sb = lv_obj_get_scroll_bottom(parent);
        if(sb + y_scroll < 0) y_scroll = 0;
    }

    switch(snap_y) {
    case LV_SCROLL_SNAP_START:
        snap_goal = parent->coords.y1 + ptop;
        act = area_tmp->y1 + y_scroll;
        y_scroll += snap_goal - act;
        break;
    case LV_SCROLL_SNAP_END:
        snap_goal = parent->coords.y2 - pbottom;
        act = area_tmp->y2 + y_scroll;
        y_scroll += snap_goal - act;
        break;
    case LV_SCROLL_SNAP_CENTER:
        snap_goal = parent->coords.y1 + ptop + parent_h / 2;
        act = lv_area_get_height(area_tmp) / 2 + area_tmp->y1 + y_scroll;
        y_scroll += snap_goal - act;
        break;
    }

    lv_coord_t x_scroll = 0;
    lv_scroll_snap_t snap_x = lv_obj_get_scroll_snap_x(parent);
    if(snap_x != LV_SCROLL_SNAP_NONE) area_tmp = &child->coords;
    else area_tmp = area;

    lv_coord_t pleft = lv_obj_get_style_pad_left(parent, LV_PART_MAIN);
    lv_coord_t pright = lv_obj_get_style_pad_right(parent, LV_PART_MAIN);
    lv_coord_t left_diff = parent->coords.x1 + pleft - area_tmp->x1 - scroll_value->x;
    lv_coord_t right_diff = -(parent->coords.x2 - pright - area_tmp->x2- scroll_value->x);
    if((left_diff > 0 && right_diff > 0)) x_scroll = 0;
    else if(left_diff > 0) {
        x_scroll = left_diff;
        /*Do not let scrolling in*/
        lv_coord_t sl = lv_obj_get_scroll_left(parent);
        if(sl + x_scroll > 0) x_scroll = 0;
    }
    else if(right_diff > 0) {
        x_scroll = -right_diff;
        /*Do not let scrolling in*/
        lv_coord_t sr = lv_obj_get_scroll_right(parent);
        if(sr + x_scroll < 0) x_scroll = 0;
    }

    lv_coord_t parent_w = lv_obj_get_width(parent) - pleft - pright;
    switch(snap_x) {
    case LV_SCROLL_SNAP_START:
        snap_goal = parent->coords.x1 + pleft;
        act = area_tmp->x1 + x_scroll;
        x_scroll += snap_goal - act;
        break;
    case LV_SCROLL_SNAP_END:
        snap_goal = parent->coords.x2 - pright;
        act = area_tmp->x2 + x_scroll;
        x_scroll += snap_goal - act;
        break;
    case LV_SCROLL_SNAP_CENTER:
        snap_goal = parent->coords.x1 + pleft + parent_w / 2;
        act = lv_area_get_width(area_tmp) / 2 + area_tmp->x1 + x_scroll;
        x_scroll += snap_goal - act;
        break;
    }

    /*Remove any pending scroll animations.*/
    bool y_del = lv_anim_del(parent, scroll_y_anim);
    bool x_del = lv_anim_del(parent, scroll_x_anim);
    if(y_del || x_del) {
        lv_res_t res;
        res = lv_event_send(parent, LV_EVENT_SCROLL_END, NULL);
        if(res != LV_RES_OK) return;
    }

    if((scroll_dir & LV_DIR_LEFT) == 0 && x_scroll < 0) x_scroll = 0;
    if((scroll_dir & LV_DIR_RIGHT) == 0 && x_scroll > 0) x_scroll = 0;
    if((scroll_dir & LV_DIR_TOP) == 0 && y_scroll < 0) y_scroll = 0;
    if((scroll_dir & LV_DIR_BOTTOM) == 0 && y_scroll > 0) y_scroll = 0;

    scroll_value->x += anim_en == LV_ANIM_OFF ? 0 : x_scroll;
    scroll_value->y += anim_en == LV_ANIM_OFF ? 0 : y_scroll;
    lv_obj_scroll_by(parent, x_scroll, y_scroll, anim_en);
}
