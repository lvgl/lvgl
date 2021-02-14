/**
 * @file lv_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img.h"
#if LV_USE_IMG != 0

#include "../lv_misc/lv_assert.h"
#include "../lv_draw/lv_img_decoder.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_txt.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_log.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS &lv_img_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_img_constructor(lv_obj_t * obj, const lv_obj_t * copy);
static void lv_img_destructor(lv_obj_t * obj);
static lv_draw_res_t lv_img_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode);
static lv_res_t lv_img_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_img_class = {
         .constructor_cb = lv_img_constructor,
         .destructor_cb = lv_img_destructor,
         .signal_cb = lv_img_signal,
         .draw_cb = lv_img_draw,
         .instance_size = sizeof(lv_img_t),
         .base_class = &lv_obj_class
     };

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_img_create(lv_obj_t * parent, const lv_obj_t * copy)
{
    return lv_obj_create_from_class(&lv_img_class, parent, copy);
}

/*=====================
 * Setter functions
 *====================*/

void lv_img_set_src(lv_obj_t * obj, const void * src)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_invalidate(obj);

    lv_img_src_t src_type = lv_img_src_get_type(src);
    lv_img_t * img = (lv_img_t *)obj;

#if LV_USE_LOG && LV_LOG_LEVEL >= LV_LOG_LEVEL_INFO
    switch(src_type) {
        case LV_IMG_SRC_FILE:
            LV_LOG_TRACE("lv_img_set_src: `LV_IMG_SRC_FILE` type found");
            break;
        case LV_IMG_SRC_VARIABLE:
            LV_LOG_TRACE("lv_img_set_src: `LV_IMG_SRC_VARIABLE` type found");
            break;
        case LV_IMG_SRC_SYMBOL:
            LV_LOG_TRACE("lv_img_set_src: `LV_IMG_SRC_SYMBOL` type found");
            break;
        default:
            LV_LOG_WARN("lv_img_set_src: unknown type");
    }
#endif

    /*If the new source type is unknown free the memories of the old source*/
    if(src_type == LV_IMG_SRC_UNKNOWN) {
        LV_LOG_WARN("lv_img_set_src: unknown image type");
        if(img->src_type == LV_IMG_SRC_SYMBOL || img->src_type == LV_IMG_SRC_FILE) {
            lv_mem_free(img->src);
        }
        img->src      = NULL;
        img->src_type = LV_IMG_SRC_UNKNOWN;
        return;
    }

    lv_img_header_t header;
    lv_img_decoder_get_info(src, &header);

    /*Save the source*/
    if(src_type == LV_IMG_SRC_VARIABLE) {
        LV_LOG_INFO("lv_img_set_src:  `LV_IMG_SRC_VARIABLE` type found");

        /*If memory was allocated because of the previous `src_type` then free it*/
        if(img->src_type == LV_IMG_SRC_FILE || img->src_type == LV_IMG_SRC_SYMBOL) {
            lv_mem_free(img->src);
        }
        img->src = src;
    }
    else if(src_type == LV_IMG_SRC_FILE || src_type == LV_IMG_SRC_SYMBOL) {
        /* If the new and the old src are the same then it was only a refresh.*/
        if(img->src != src) {
            const void * old_src = NULL;
            /* If memory was allocated because of the previous `src_type` then save its pointer and free after allocation.
             * It's important to allocate first to be sure the new data will be on a new address.
             * Else `img_cache` wouldn't see the change in source.*/
            if(img->src_type == LV_IMG_SRC_FILE || img->src_type == LV_IMG_SRC_SYMBOL) {
                old_src = img->src;
            }
            char * new_str = lv_mem_alloc(strlen(src) + 1);
            LV_ASSERT_MALLOC(new_str);
            if(new_str == NULL) return;
            strcpy(new_str, src);
            img->src = new_str;

            if(old_src) lv_mem_free(old_src);
        }
    }

    if(src_type == LV_IMG_SRC_SYMBOL) {
        /*`lv_img_dsc_get_info` couldn't set the with and height of a font so set it here*/
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
        lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_point_t size;
        lv_txt_get_size(&size, src, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        header.w = size.x;
        header.h = size.y;
    }

    img->src_type = src_type;
    img->w        = header.w;
    img->h        = header.h;
    img->cf       = header.cf;
    img->pivot.x = header.w / 2;
    img->pivot.y = header.h / 2;

    lv_obj_handle_self_size_chg(obj);

    /*Provide enough room for the rotated corners*/
    if(img->angle || img->zoom != LV_IMG_ZOOM_NONE) lv_obj_refresh_ext_draw_size(obj);

    lv_obj_invalidate(obj);
}

void lv_img_set_offset_x(lv_obj_t * obj, lv_coord_t x)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    x = x % img->w;

    img->offset.x = x;
    lv_obj_invalidate(obj);
}

void lv_img_set_offset_y(lv_obj_t * obj, lv_coord_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    y = y % img->h;

    img->offset.y = y;
    lv_obj_invalidate(obj);
}

void lv_img_set_angle(lv_obj_t * obj, int16_t angle)
{
    if(angle < 0 || angle >= 3600) angle = angle % 3600;

   lv_img_t * img = (lv_img_t *)obj;
    if(angle == img->angle) return;

    lv_coord_t transf_zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
    transf_zoom = (transf_zoom * img->zoom) >> 8;

    lv_coord_t transf_angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_area_t a;
    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle + img->angle, transf_zoom, &img->pivot);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);

    img->angle = angle;
    lv_obj_refresh_ext_draw_size(obj);

    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle + img->angle, transf_zoom, &img->pivot);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);
}

void lv_img_set_pivot(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    lv_img_t * img = (lv_img_t *)obj;
    if(img->pivot.x == x && img->pivot.y == y) return;

    lv_coord_t transf_zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
    transf_zoom = (transf_zoom * img->zoom) >> 8;

    lv_coord_t transf_angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
    transf_angle += img->angle;

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_area_t a;
    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle, transf_zoom, &img->pivot);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);

    img->pivot.x = x;
    img->pivot.y = y;
    lv_obj_refresh_ext_draw_size(obj);

    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle, transf_zoom, &img->pivot);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);
}

void lv_img_set_zoom(lv_obj_t * obj, uint16_t zoom)
{
   lv_img_t * img = (lv_img_t *)obj;
    if(zoom == img->zoom) return;

    if(zoom == 0) zoom = 1;

    lv_coord_t transf_zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);

    lv_coord_t transf_angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
    transf_angle += img->angle;

    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_area_t a;
    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle, (transf_zoom * img->zoom) >> 8, &img->pivot);
    a.x1 += obj->coords.x1 - 1;
    a.y1 += obj->coords.y1 - 1;
    a.x2 += obj->coords.x1 + 1;
    a.y2 += obj->coords.y1 + 1;
    lv_obj_invalidate_area(obj, &a);

    img->zoom = zoom;
    lv_obj_refresh_ext_draw_size(obj);

    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle, (transf_zoom * img->zoom) >> 8, &img->pivot);
    a.x1 += obj->coords.x1 - 1;
    a.y1 += obj->coords.y1 - 1;
    a.x2 += obj->coords.x1 + 1;
    a.y2 += obj->coords.y1 + 1;
    lv_obj_invalidate_area(obj, &a);
}

void lv_img_set_antialias(lv_obj_t * obj, bool antialias)
{
   lv_img_t * img = (lv_img_t *)obj;
    if(antialias == img->antialias) return;

    img->antialias = antialias;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

const void * lv_img_get_src(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    return img->src;
}

lv_coord_t lv_img_get_offset_x(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    return img->offset.x;
}

lv_coord_t lv_img_get_offset_y(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    return img->offset.y;
}

uint16_t lv_img_get_angle(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    return img->angle;
}

void lv_img_get_pivot(lv_obj_t * obj, lv_point_t * pivot)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    *pivot = img->pivot;
}

uint16_t lv_img_get_zoom(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    return img->zoom;
}

bool lv_img_get_antialias(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

   lv_img_t * img = (lv_img_t *)obj;

    return img->antialias ? true : false;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_img_constructor(lv_obj_t * obj, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_bar create started");

    lv_img_t * img = (lv_img_t *)obj;

    img->src       = NULL;
    img->src_type  = LV_IMG_SRC_UNKNOWN;
    img->cf        = LV_IMG_CF_UNKNOWN;
    img->w         = lv_obj_get_width(obj);
    img->h         = lv_obj_get_height(obj);
    img->angle = 0;
    img->zoom = LV_IMG_ZOOM_NONE;
    img->antialias = LV_COLOR_DEPTH > 8 ? 1 : 0;
    img->offset.x  = 0;
    img->offset.y  = 0;
    img->pivot.x = 0;
    img->pivot.y = 0;

    if(copy == NULL) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
        lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);

        /* Enable auto size for non screens
         * because image screens are wallpapers
         * and must be screen sized*/
        if(obj->parent) lv_obj_set_size(obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    }
    else {
        lv_img_t * copy_img = (lv_img_t *) copy;
        img->zoom      = copy_img->zoom;
        img->angle     = copy_img->angle;
        img->antialias = copy_img->antialias;
        img->offset  = copy_img->offset;
        img->pivot   = copy_img->pivot;
        lv_img_set_src(obj, copy_img->src);
    }
}

static void lv_img_destructor(lv_obj_t * obj)
{
    lv_img_t * img = (lv_img_t *)obj;
    if(img->src_type == LV_IMG_SRC_FILE || img->src_type == LV_IMG_SRC_SYMBOL) {
        lv_mem_free(img->src);
        img->src      = NULL;
        img->src_type = LV_IMG_SRC_UNKNOWN;
    }
}

static lv_draw_res_t lv_img_draw(lv_obj_t * obj, const lv_area_t * clip_area, lv_draw_mode_t mode)
{
    lv_img_t * img = (lv_img_t *)obj;
    if(mode == LV_DRAW_MODE_COVER_CHECK) {
        if(lv_obj_get_style_clip_corner(obj, LV_PART_MAIN)) return LV_DRAW_RES_MASKED;

        if(img->src_type == LV_IMG_SRC_UNKNOWN || img->src_type == LV_IMG_SRC_SYMBOL) return LV_DRAW_RES_NOT_COVER;

        /*Non true color format might have "holes"*/
        if(img->cf != LV_IMG_CF_TRUE_COLOR && img->cf != LV_IMG_CF_RAW) return LV_DRAW_RES_NOT_COVER;

        /*With not LV_OPA_COVER images can't cover an area */
        if(lv_obj_get_style_img_opa(obj, LV_PART_MAIN) != LV_OPA_COVER) return LV_DRAW_RES_NOT_COVER;

        int32_t angle_final = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
        angle_final += img->angle;

        if(angle_final != 0) return LV_DRAW_RES_NOT_COVER;

        int32_t zoom_final = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
        zoom_final = (zoom_final * img->zoom) >> 8;

        if(zoom_final == LV_IMG_ZOOM_NONE) {
            if(_lv_area_is_in(clip_area, &obj->coords, 0) == false) return LV_DRAW_RES_NOT_COVER;
        }
        else {
            lv_area_t a;
            _lv_img_buf_get_transformed_area(&a, lv_obj_get_width(obj), lv_obj_get_height(obj), 0, zoom_final, &img->pivot);
            a.x1 += obj->coords.x1;
            a.y1 += obj->coords.y1;
            a.x2 += obj->coords.x1;
            a.y2 += obj->coords.y1;

            if(_lv_area_is_in(clip_area, &a, 0) == false) return LV_DRAW_RES_NOT_COVER;
        }

#if LV_DRAW_COMPLEX
        if(lv_obj_get_style_blend_mode(obj, LV_PART_MAIN) != LV_BLEND_MODE_NORMAL) return LV_DRAW_RES_NOT_COVER;
#endif

        return LV_DRAW_RES_COVER;
    }

    int32_t zoom_final = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
    zoom_final = (zoom_final * img->zoom) >> 8;

    int32_t angle_final = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
    angle_final += img->angle;

    lv_coord_t obj_w = lv_obj_get_width(obj);
    lv_coord_t obj_h = lv_obj_get_height(obj);

    lv_area_t bg_coords;
    _lv_img_buf_get_transformed_area(&bg_coords, obj_w, obj_h,
                                     angle_final, zoom_final, &img->pivot);

    /*Modify the coordinates to draw the background for the rotated and scaled coordinates*/
    bg_coords.x1 += obj->coords.x1;
    bg_coords.y1 += obj->coords.y1;
    bg_coords.x2 += obj->coords.x1;
    bg_coords.y2 += obj->coords.y1;

    lv_area_t ori_coords;
    lv_area_copy(&ori_coords, &obj->coords);
    lv_area_copy(&obj->coords, &bg_coords);

    lv_obj_draw_base(MY_CLASS, obj, clip_area, mode);
    lv_area_copy(&obj->coords, &ori_coords);

    if(mode == LV_DRAW_MODE_MAIN_DRAW) {
        if(img->h == 0 || img->w == 0) return true;

        if(zoom_final == 0) return LV_DRAW_RES_OK;

        lv_area_t img_coords;
        lv_area_copy(&img_coords, &bg_coords);
        img_coords.x1 += lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        img_coords.y1 += lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        img_coords.x2 -= lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        img_coords.y2 -= lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

        if(img->src_type == LV_IMG_SRC_FILE || img->src_type == LV_IMG_SRC_VARIABLE) {
            LV_LOG_TRACE("lv_img_draw: start to draw image");

            lv_draw_img_dsc_t img_dsc;
            lv_draw_img_dsc_init(&img_dsc);
            lv_obj_init_draw_img_dsc(obj, LV_PART_MAIN, &img_dsc);

            img_dsc.zoom = zoom_final;
            img_dsc.angle = angle_final;
            img_dsc.pivot.x = img->pivot.x;
            img_dsc.pivot.y = img->pivot.y;
            img_dsc.antialias = img->antialias;

            lv_area_t coords_tmp;

            coords_tmp.y1 = obj->coords.y1 + img->offset.y;
            if(coords_tmp.y1 > obj->coords.y1) coords_tmp.y1 -= img->h;
            coords_tmp.y2 = coords_tmp.y1 + img->h - 1;

            for(; coords_tmp.y1 < obj->coords.y2; coords_tmp.y1 += img->h, coords_tmp.y2 += img->h) {
                coords_tmp.x1 = obj->coords.x1 + img->offset.x;
                if(coords_tmp.x1 > obj->coords.x1) coords_tmp.x1 -= img->w;
                coords_tmp.x2 = coords_tmp.x1 + img->w - 1;

                for(; coords_tmp.x1 < obj->coords.x2; coords_tmp.x1 += img->w, coords_tmp.x2 += img->w) {
                    lv_draw_img(&coords_tmp, clip_area, img->src, &img_dsc);
                }
            }
        }
        else if(img->src_type == LV_IMG_SRC_SYMBOL) {
            LV_LOG_TRACE("lv_img_draw: start to draw symbol");
            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);

//            label_dsc.color = lv_obj_get_style_img_recolor(obj, LV_PART_MAIN);
            lv_draw_label(&obj->coords, clip_area, &label_dsc, img->src, NULL);
        }
        else {
            /*Trigger the error handler of image draw*/
            LV_LOG_WARN("lv_img_draw: image source type is unknown");
            lv_draw_img(&obj->coords, clip_area, NULL, NULL);
        }
    }

    return LV_DRAW_RES_OK;
}

static lv_res_t lv_img_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = lv_obj_signal_base(MY_CLASS, obj, sign, param);
    if(res != LV_RES_OK) return res;

   lv_img_t * img = (lv_img_t *)obj;

  if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Refresh the file name to refresh the symbol text size*/
        if(img->src_type == LV_IMG_SRC_SYMBOL) {
            lv_img_set_src(obj, img->src);
        }
    }
    else if(sign == LV_SIGNAL_REFR_EXT_DRAW_SIZE) {

        lv_coord_t * s = param;
        lv_coord_t transf_zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
        transf_zoom = (transf_zoom * img->zoom) >> 8;

        lv_coord_t transf_angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
        transf_angle += img->angle;

        /*If the image has angle provide enough room for the rotated corners */
        if(transf_angle || transf_zoom != LV_IMG_ZOOM_NONE) {
            lv_area_t a;
            lv_coord_t w = lv_obj_get_width(obj);
            lv_coord_t h = lv_obj_get_height(obj);
            _lv_img_buf_get_transformed_area(&a, w, h, transf_angle, transf_zoom, &img->pivot);
            lv_coord_t pad_ori = *s;
            *s = LV_MAX(*s, pad_ori - a.x1);
            *s = LV_MAX(*s, pad_ori - a.y1);
            *s = LV_MAX(*s, pad_ori + a.x2 - w);
            *s = LV_MAX(*s, pad_ori + a.y2 - h);
        }

        /*Handle the padding of the background*/
        lv_coord_t left = lv_obj_get_style_pad_left(obj, LV_PART_MAIN);
        lv_coord_t right = lv_obj_get_style_pad_right(obj, LV_PART_MAIN);
        lv_coord_t top = lv_obj_get_style_pad_top(obj, LV_PART_MAIN);
        lv_coord_t bottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN);

        *s = LV_MAX(*s, left);
        *s = LV_MAX(*s, right);
        *s = LV_MAX(*s, top);
        *s = LV_MAX(*s, bottom);
    }
    else if(sign == LV_SIGNAL_HIT_TEST) {
        lv_hit_test_info_t * info = param;
        lv_coord_t zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
        zoom = (zoom * img->zoom) >> 8;

        lv_coord_t angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
        angle += img->angle;

        /* If the object is exactly image sized (not cropped, not mosaic) and transformed
         * perform hit test on it's transformed area */
        if(img->w == lv_obj_get_width(obj) && img->h == lv_obj_get_height(obj) &&
           (zoom != LV_IMG_ZOOM_NONE || angle != 0 || img->pivot.x != img->w / 2 || img->pivot.y != img->h / 2)) {

            lv_coord_t w = lv_obj_get_width(obj);
            lv_coord_t h = lv_obj_get_height(obj);
            lv_area_t coords;
            _lv_img_buf_get_transformed_area(&coords, w, h, angle, zoom, &img->pivot);
            coords.x1 += obj->coords.x1;
            coords.y1 += obj->coords.y1;
            coords.x2 += obj->coords.x1;
            coords.y2 += obj->coords.y1;

            info->result = _lv_area_is_point_on(&coords, info->point, 0);
        }
        else {
            lv_area_t a;
            lv_obj_get_click_area(obj, &a);
            info->result = _lv_area_is_point_on(&a, info->point, 0);
        }
    }
    else if(sign == LV_SIGNAL_GET_SELF_SIZE) {
        lv_point_t * p = param;
        p->x = img->w;
        p->y = img->h;
    }

    return res;
}

#endif
