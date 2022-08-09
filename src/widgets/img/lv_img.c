/**
 * @file lv_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img.h"
#if LV_USE_IMG != 0

#include "../../misc/lv_assert.h"
#include "../../draw/lv_img_decoder.h"
#include "../../misc/lv_fs.h"
#include "../../misc/lv_txt.h"
#include "../../misc/lv_math.h"
#include "../../misc/lv_log.h"

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
static void lv_img_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_img_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_img_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_img(lv_event_t * e);
static lv_res_t get_caps(lv_img_t * img);
static void extract_metadata(lv_img_t * img, lv_point_t * size_hint);
static void next_frame_task_cb(lv_timer_t * t);
static void next_frame_task_seekable_cb(lv_timer_t * t);
static void start_animation(lv_img_t * img, lv_img_dec_ctx_t * dec_ctx);



/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_img_class = {
    .constructor_cb = lv_img_constructor,
    .destructor_cb = lv_img_destructor,
    .event_cb = lv_img_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_img_t),
    .base_class = &lv_obj_class
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_img_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_img_set_play_mode(lv_obj_t * obj, const lv_img_ctrl_t ctrl)
{
    lv_img_t * img = (lv_img_t *)obj;
    img->ctrl = ctrl;

    if(img->anim_timer && (img->dec_ctx->dest_frame != img->dec_ctx->current_frame ||
                           LV_BF(img->ctrl, LV_IMG_CTRL_PAUSE))) {
        lv_timer_resume(img->anim_timer);
    }
}

lv_res_t lv_img_set_current_frame(lv_obj_t * obj, const lv_frame_index_t index)
{
    lv_img_t * img = (lv_img_t *)obj;
    if(img->dec_ctx == NULL)
        return LV_RES_INV;
    /* Allow rewinding the animation to the beginning, even if not seekable */
    if(!index)
        img->dec_ctx->current_frame = index;
    else if(LV_BF(img->caps, LV_IMG_DEC_SEEKABLE))
        return LV_RES_INV;

    img->dec_ctx->current_frame = LV_MIN(index, img->dec_ctx->total_frames - 1);
    return LV_RES_OK;
}

lv_res_t lv_img_set_stop_at_frame(lv_obj_t * obj, const lv_frame_index_t index, const int forward)
{
    lv_img_t * img = (lv_img_t *)obj;
    if(img->dec_ctx == NULL || LV_BF(img->caps, LV_IMG_DEC_SEEKABLE))
        return LV_RES_INV;

    img->dec_ctx->dest_frame = LV_MIN(index, img->dec_ctx->total_frames - 1);

    img->ctrl = LV_IMG_CTRL_PLAY | LV_IMG_CTRL_STOPAT | (forward ? LV_IMG_CTRL_FORWARD : LV_IMG_CTRL_BACKWARD);
    if(img->anim_timer && img->dec_ctx->dest_frame != img->dec_ctx->current_frame) {
        lv_timer_resume(img->anim_timer);
    }
    return LV_RES_OK;
}

lv_res_t lv_img_set_src(lv_obj_t * obj, lv_img_src_t * src)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_img_t * img = (lv_img_t *)obj;

    lv_obj_invalidate(obj);

    lv_img_src_capture(&img->src, src);
    return get_caps(img);
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
    transf_zoom = ((int32_t)transf_zoom * img->zoom) >> 8;

    lv_coord_t transf_angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);

    lv_obj_update_layout(obj);  /*Be sure the object's size is calculated*/
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
    transf_zoom = ((int32_t)transf_zoom * img->zoom) >> 8;

    lv_coord_t transf_angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
    transf_angle += img->angle;

    lv_obj_update_layout(obj);  /*Be sure the object's size is calculated*/
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

    lv_obj_update_layout(obj);  /*Be sure the object's size is calculated*/
    lv_coord_t w = lv_obj_get_width(obj);
    lv_coord_t h = lv_obj_get_height(obj);
    lv_area_t a;
    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle, ((int32_t)transf_zoom * img->zoom) >> 8, &img->pivot);
    a.x1 += obj->coords.x1 - 1;
    a.y1 += obj->coords.y1 - 1;
    a.x2 += obj->coords.x1 + 1;
    a.y2 += obj->coords.y1 + 1;
    lv_obj_invalidate_area(obj, &a);

    img->zoom = zoom;
    lv_obj_refresh_ext_draw_size(obj);

    _lv_img_buf_get_transformed_area(&a, w, h, transf_angle, ((int32_t)transf_zoom * img->zoom) >> 8, &img->pivot);
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

void lv_img_set_size_mode(lv_obj_t * obj, lv_img_size_mode_t mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_img_t * img = (lv_img_t *)obj;
    if(mode == img->obj_size_mode) return;

    img->obj_size_mode = mode;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/
lv_img_ctrl_t lv_img_get_play_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_img_t * img = (lv_img_t *)obj;

    return img->ctrl;
}

size_t lv_img_get_totalframes(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_img_t * img = (lv_img_t *)obj;

    return img->dec_ctx ? img->dec_ctx->total_frames : 0;
}

size_t lv_img_get_current_frame(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_img_t * img = (lv_img_t *)obj;

    return img->dec_ctx ? img->dec_ctx->current_frame : 0;
}



lv_img_src_t * lv_img_get_src(lv_obj_t * obj)
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

lv_img_size_mode_t lv_img_get_size_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_img_t * img = (lv_img_t *)obj;
    return img->obj_size_mode;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_res_t get_caps(lv_img_t * img)
{
    lv_obj_t * obj = (lv_obj_t *)img;

    /*Reset the decoded flag here*/
    img->cf = LV_IMG_CF_UNKNOWN;
    if(img->src->type == LV_IMG_SRC_UNKNOWN)
        return LV_RES_INV;

    if(img->src->type != LV_IMG_SRC_SYMBOL) {
        /*Try to see if we find a decoder that's able to decode the picture. Decoding is delayed until rendering*/
        lv_img_decoder_t * img_dec = lv_img_decoder_accept(img->src, &img->caps);
        if(img_dec == NULL) return LV_RES_INV;
        /*Lie a bit here, we assume we'll be able to decode it later correctly*/
        return LV_RES_OK;
    }

    /*`lv_img_dsc_get_info` couldn't set the with and height of a font so set it here*/
    const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_point_t size;
    lv_txt_get_size(&size, img->src->data, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
    img->w  = size.x;
    img->h  = size.y;
    img->cf = LV_IMG_CF_ALPHA_1BIT;
    img->pivot.x = size.x / 2;
    img->pivot.y = size.y / 2;

    lv_obj_refresh_self_size(obj);

    /*Provide enough room for the rotated corners*/
    if(img->angle || img->zoom != LV_IMG_ZOOM_NONE) lv_obj_refresh_ext_draw_size(obj);
    return LV_RES_OK;
}

static void extract_metadata(lv_img_t * img, lv_point_t * size_hint)
{
    lv_obj_t * obj = (lv_obj_t *)img;
    lv_img_header_t header;
    lv_memzero(&header, sizeof(header));
    if(img->src->type == LV_IMG_SRC_UNKNOWN) return;

    /*Query the image cache now, since it's very likely we'll draw the image later on,
        so don't waste opening a decoder and closing it if it'll be reused */
    lv_img_dec_dsc_in_t dsc = {0};
    dsc.src = img->src;
    /*Don't waste a cached entry for a different color, use the color that'll be drawn*/
    dsc.color.full = lv_color_to32(lv_obj_get_style_img_recolor_filtered(obj, LV_PART_MAIN));
    dsc.size_hint = *size_hint;

    if(lv_img_cache_query(&dsc, &header, &img->caps, img->dec_ctx) == LV_RES_INV) {
        /* Couldn't decode the picture, let's return anyway */
        LV_LOG_WARN("extract_metadata: image failed decoding");
        return;
    }

    img->w       = header.w;
    img->h       = header.h;
    img->cf      = header.cf;
    img->pivot.x = header.w / 2;
    img->pivot.y = header.h / 2;

    lv_obj_refresh_self_size(obj);
}


static void lv_img_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_img_t * img = (lv_img_t *)obj;
    img->w         = lv_obj_get_width(obj);
    img->h         = lv_obj_get_height(obj);
    img->zoom      = LV_IMG_ZOOM_NONE;
    img->antialias = LV_COLOR_DEPTH > 8 ? 1 : 0;

    lv_obj_clear_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_img_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_img_t * img = (lv_img_t *)obj;

    /* If we have captured the context in the animation, we need to release it */
    if(img->dec_ctx && img->dec_ctx->auto_allocated == 0) {
        lv_img_decoder_t * decoder = lv_img_decoder_accept(img->src, NULL);
        lv_img_dec_dsc_t dsc = { .decoder = decoder, .dec_ctx = img->dec_ctx };
        img->dec_ctx->auto_allocated = 1;
        lv_img_decoder_close(&dsc);
        img->dec_ctx = NULL;
    }

    lv_img_src_free(img->src);
    if(img->anim_timer) {
        lv_timer_del(img->anim_timer);
        img->anim_timer = NULL;
        img->ctrl = LV_IMG_CTRL_FORWARD;
    }
}

static lv_point_t lv_img_get_transformed_size(lv_obj_t * obj)
{
    lv_img_t * img = (lv_img_t *)obj;

    int32_t zoom_final = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
    zoom_final = (zoom_final * img->zoom) >> 8;
    int32_t angle_final = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
    angle_final += img->angle;

    lv_area_t area_transform;
    _lv_img_buf_get_transformed_area(&area_transform, img->w, img->h,
                                     angle_final, zoom_final, &img->pivot);

    return (lv_point_t) {
        lv_area_get_width(&area_transform), lv_area_get_height(&area_transform)
    };
}

static void lv_img_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_event_code_t code = lv_event_get_code(e);

    /*Ancestor events will be called during drawing*/
    if(code != LV_EVENT_DRAW_MAIN && code != LV_EVENT_DRAW_POST) {
        /*Call the ancestor's event handler*/
        lv_res_t res = lv_obj_event_base(MY_CLASS, e);
        if(res != LV_RES_OK) return;
    }

    lv_obj_t * obj = lv_event_get_target(e);
    lv_img_t * img = (lv_img_t *)obj;

    if(code == LV_EVENT_STYLE_CHANGED) {
        /*Refresh the file name to refresh the symbol text size*/
        if(img->src && img->src->type == LV_IMG_SRC_SYMBOL) {
            get_caps(img);
        }
        else {
            /*With transformation it might change*/
            lv_obj_refresh_ext_draw_size(obj);
        }
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {

        lv_coord_t * s = lv_event_get_param(e);
        lv_coord_t transf_zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
        transf_zoom = ((int32_t)transf_zoom * img->zoom) >> 8;

        lv_coord_t transf_angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
        transf_angle += img->angle;

        /*If the image has angle provide enough room for the rotated corners*/
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
    }
    else if(code == LV_EVENT_HIT_TEST) {
        lv_hit_test_info_t * info = lv_event_get_param(e);
        lv_coord_t zoom = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
        zoom = (zoom * img->zoom) >> 8;

        lv_coord_t angle = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
        angle += img->angle;

        /*If the object is exactly image sized (not cropped, not mosaic) and transformed
         *perform hit test on its transformed area*/
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

            info->res = _lv_area_is_point_on(&coords, info->point, 0);
        }
        else {
            lv_area_t a;
            lv_obj_get_click_area(obj, &a);
            info->res = _lv_area_is_point_on(&a, info->point, 0);
        }
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        if(img->cf == LV_IMG_CF_UNKNOWN) {
            lv_point_t size = { lv_obj_get_style_width(obj, LV_PART_MAIN),
                                lv_obj_get_style_height(obj, LV_PART_MAIN)
                              };
            if(size.x == LV_SIZE_CONTENT || size.y == LV_SIZE_CONTENT)
                /*Image is not decoded yet, so let's find its metadata it to figure out its size, if required*/
                extract_metadata(img, &size);
        }

        lv_point_t * p = lv_event_get_param(e);
        if(img->obj_size_mode == LV_IMG_SIZE_MODE_REAL) {
            *p = lv_img_get_transformed_size(obj);
        }
        else {
            p->x = img->w;
            p->y = img->h;
        }
    }
    else if(code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_DRAW_POST || code == LV_EVENT_COVER_CHECK) {
        draw_img(e);
    }
}

static void draw_img(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_img_t * img = (lv_img_t *)obj;

    if(code == LV_EVENT_COVER_CHECK) {
        /*Cover check only need to know the size and if the picture is transparent*/
        lv_cover_check_info_t * info = lv_event_get_param(e);
        if(info->res == LV_COVER_RES_MASKED) return;
        if(img->src->type == LV_IMG_SRC_UNKNOWN || img->src->type == LV_IMG_SRC_SYMBOL) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        /*Non true color format might have "holes"*/
        if(LV_BT(img->caps, LV_IMG_DEC_TRANSPARENT)) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        /*With not LV_OPA_COVER images can't cover an area */
        if(lv_obj_get_style_img_opa(obj, LV_PART_MAIN) != LV_OPA_COVER) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        int32_t angle_final = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
        angle_final += img->angle;

        if(angle_final != 0) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        int32_t zoom_final = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
        zoom_final = (zoom_final * img->zoom) >> 8;

        const lv_area_t * clip_area = lv_event_get_param(e);
        if(zoom_final == LV_IMG_ZOOM_NONE) {
            if(_lv_area_is_in(clip_area, &obj->coords, 0) == false) {
                info->res = LV_COVER_RES_NOT_COVER;
            }
        }
        else {
            lv_area_t a;
            _lv_img_buf_get_transformed_area(&a, lv_obj_get_width(obj), lv_obj_get_height(obj), 0, zoom_final, &img->pivot);
            a.x1 += obj->coords.x1;
            a.y1 += obj->coords.y1;
            a.x2 += obj->coords.x1;
            a.y2 += obj->coords.y1;

            if(_lv_area_is_in(clip_area, &a, 0) == false) {
                info->res = LV_COVER_RES_NOT_COVER;
            }
        }
        return;
    }

    /* Deferred opening of the pictures for animation or not yet queried picture */
    if((img->dec_ctx == NULL && LV_BT(img->caps, LV_IMG_DEC_ANIMATED)) || img->cf == LV_IMG_CF_UNKNOWN) {
        lv_img_dec_dsc_in_t dsc = {
            .src = img->src,
            .color = {.full = 0},
            .size_hint = {.x = lv_obj_get_width(obj), .y = lv_obj_get_height(obj)}
        };
        lv_img_cache_entry_t * entry = lv_img_cache_open(&dsc, NULL);
        if(entry != NULL) {
            img->caps = entry->dec_dsc.caps;
            if(LV_BT(img->caps, LV_IMG_DEC_ANIMATED)) {
                /* Capture the img decoding context for animated picture to avoid hitting the cache for each frame */
                img->dec_ctx = entry->dec_dsc.dec_ctx;
                img->dec_ctx->auto_allocated = 0;
                entry->dec_dsc.dec_ctx = 0;
            }
            /* Save size and type metadata since we can have deferred decoding */
            img->w = entry->dec_dsc.header.w;
            img->h = entry->dec_dsc.header.h;
            img->cf = entry->dec_dsc.header.cf;

            lv_img_cache_cleanup(entry);
        }
    }
    if(img->dec_ctx && LV_BT(img->caps, LV_IMG_DEC_ANIMATED) && img->anim_timer == NULL) {
        start_animation(img, img->dec_ctx);
    }

    int32_t zoom_final = lv_obj_get_style_transform_zoom(obj, LV_PART_MAIN);
    zoom_final = (zoom_final * img->zoom) >> 8;

    int32_t angle_final = lv_obj_get_style_transform_angle(obj, LV_PART_MAIN);
    angle_final += img->angle;

    lv_coord_t obj_w = lv_obj_get_width(obj);
    lv_coord_t obj_h = lv_obj_get_height(obj);

    lv_coord_t border_width = lv_obj_get_style_border_width(obj, LV_PART_MAIN);
    lv_coord_t pleft = lv_obj_get_style_pad_left(obj, LV_PART_MAIN) + border_width;
    lv_coord_t pright = lv_obj_get_style_pad_right(obj, LV_PART_MAIN) + border_width;
    lv_coord_t ptop = lv_obj_get_style_pad_top(obj, LV_PART_MAIN) + border_width;
    lv_coord_t pbottom = lv_obj_get_style_pad_bottom(obj, LV_PART_MAIN) + border_width;

    lv_point_t bg_pivot;
    bg_pivot.x = img->pivot.x + pleft;
    bg_pivot.y = img->pivot.y + ptop;
    lv_area_t bg_coords;

    if(img->dec_ctx != NULL && LV_BT(img->caps, LV_IMG_DEC_VECTOR)
       && (obj_w != img->w || obj_h != img->h)) {
        /*For vector image that aren't tiled, let's increase the image size*/
        img->w = obj_w;
        img->h = obj_h;
    }

    if(img->obj_size_mode == LV_IMG_SIZE_MODE_REAL) {
        /*Object size equals to transformed image size*/
        lv_obj_get_coords(obj, &bg_coords);
    }
    else {
        _lv_img_buf_get_transformed_area(&bg_coords, obj_w, obj_h, angle_final, zoom_final, &bg_pivot);

        /*Modify the coordinates to draw the background for the rotated and scaled coordinates*/
        bg_coords.x1 += obj->coords.x1;
        bg_coords.y1 += obj->coords.y1;
        bg_coords.x2 += obj->coords.x1;
        bg_coords.y2 += obj->coords.y1;
    }

    lv_area_t ori_coords;
    lv_area_copy(&ori_coords, &obj->coords);
    lv_area_copy(&obj->coords, &bg_coords);

    lv_res_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_area_copy(&obj->coords, &ori_coords);

    if(code == LV_EVENT_DRAW_MAIN) {
        if(img->h == 0 || img->w == 0) return;
        if(zoom_final == 0) return;

        lv_draw_ctx_t * draw_ctx = lv_event_get_draw_ctx(e);

        lv_area_t img_max_area;
        lv_area_copy(&img_max_area, &obj->coords);

        lv_point_t img_size_final = lv_img_get_transformed_size(obj);

        if(img->obj_size_mode == LV_IMG_SIZE_MODE_REAL) {
            img_max_area.x1 -= ((img->w - img_size_final.x) + 1) / 2;
            img_max_area.x2 -= ((img->w - img_size_final.x) + 1) / 2;
            img_max_area.y1 -= ((img->h - img_size_final.y) + 1) / 2;
            img_max_area.y2 -= ((img->h - img_size_final.y) + 1) / 2;
        }
        else {
            img_max_area.x2 = img_max_area.x1 + lv_area_get_width(&bg_coords) - 1;
            img_max_area.y2 = img_max_area.y1 + lv_area_get_height(&bg_coords) - 1;
        }

        img_max_area.x1 += pleft;
        img_max_area.y1 += ptop;
        img_max_area.x2 -= pright;
        img_max_area.y2 -= pbottom;

        if(img->src->type == LV_IMG_SRC_FILE || img->src->type == LV_IMG_SRC_VARIABLE) {
            lv_draw_img_dsc_t img_dsc;
            lv_draw_img_dsc_init(&img_dsc);
            img_dsc.dec_ctx = img->dec_ctx;
            lv_obj_init_draw_img_dsc(obj, LV_PART_MAIN, &img_dsc);

            img_dsc.zoom = zoom_final;
            img_dsc.angle = angle_final;
            img_dsc.pivot.x = img->pivot.x;
            img_dsc.pivot.y = img->pivot.y;
            img_dsc.antialias = img->antialias;

            lv_area_t img_clip_area;
            img_clip_area.x1 = bg_coords.x1 + pleft;
            img_clip_area.y1 = bg_coords.y1 + ptop;
            img_clip_area.x2 = bg_coords.x2 - pright;
            img_clip_area.y2 = bg_coords.y2 - pbottom;
            const lv_area_t * clip_area_ori = draw_ctx->clip_area;

            if(!_lv_area_intersect(&img_clip_area, draw_ctx->clip_area, &img_clip_area)) return;
            draw_ctx->clip_area = &img_clip_area;

            lv_area_t coords_tmp;
            coords_tmp.y1 = img_max_area.y1 + img->offset.y;
            if(coords_tmp.y1 > img_max_area.y1) coords_tmp.y1 -= img->h;
            coords_tmp.y2 = coords_tmp.y1 + img->h - 1;

            for(; coords_tmp.y1 < img_max_area.y2; coords_tmp.y1 += img_size_final.y, coords_tmp.y2 += img_size_final.y) {
                coords_tmp.x1 = img_max_area.x1 + img->offset.x;
                if(coords_tmp.x1 > img_max_area.x1) coords_tmp.x1 -= img->w;
                coords_tmp.x2 = coords_tmp.x1 + img->w - 1;

                for(; coords_tmp.x1 < img_max_area.x2; coords_tmp.x1 += img_size_final.x, coords_tmp.x2 += img_size_final.x) {
                    lv_draw_img(draw_ctx, &img_dsc, &coords_tmp, img->src);
                }
            }
            draw_ctx->clip_area = clip_area_ori;
        }
        else if(img->src->type == LV_IMG_SRC_SYMBOL) {
            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);

            lv_draw_label(draw_ctx, &label_dsc, &obj->coords, (const char *)img->src->data, NULL);
        }
        else {
            /*Trigger the error handler of image draw*/
            LV_LOG_WARN("draw_img: image source type is unknown");
            lv_draw_img(draw_ctx, NULL, &obj->coords, NULL);
        }
    }
}

static void start_animation(lv_img_t * img, lv_img_dec_ctx_t * dec_ctx)
{
    if(LV_BT(img->caps, LV_IMG_DEC_ANIMATED)) {
        if(LV_BT(img->caps, LV_IMG_DEC_VFR)) {
            dec_ctx->last_rendering = (uint16_t)lv_tick_get();
        }

        /* Need to create the timer here */
        img->anim_timer = lv_timer_create(
                              LV_BT(img->caps, LV_IMG_DEC_SEEKABLE) ? next_frame_task_seekable_cb : next_frame_task_cb,
                              1000 / dec_ctx->frame_rate, img);
        if(LV_BT(img->ctrl, LV_IMG_CTRL_PAUSE))
            lv_timer_pause(img->anim_timer);
    }
}

static void next_frame_task_seekable_cb(lv_timer_t * t)
{
    lv_obj_t * obj = t->user_data;
    lv_img_t * img = (lv_img_t *) obj;

    if(LV_BT(img->caps, LV_IMG_DEC_VFR)) {
        /* Variable frame rate animation are usually more complex to deal with*/
        uint16_t now = lv_tick_get(); /*Using 16bits to save memory in the decoder context */
        /* Handle rollover here with the small counter */
        uint16_t elapsed = now >= img->dec_ctx->last_rendering ? now - img->dec_ctx->last_rendering
                           : UINT16_MAX - img->dec_ctx->last_rendering + 1 + now;
        if(elapsed < img->dec_ctx->frame_delay) return;

        img->dec_ctx->last_rendering = now;
    }

    if(LV_BT(img->ctrl, LV_IMG_CTRL_PAUSE)) {
        if(img->dec_ctx->current_frame == img->dec_ctx->dest_frame) {
            /* Pause the timer too when it has run once to avoid CPU consumption */
            lv_timer_pause(t);
            return;
        }
        img->dec_ctx->dest_frame = img->dec_ctx->current_frame;
    }
    else {
        if(LV_BT(img->ctrl, LV_IMG_CTRL_STOPAT) && img->dec_ctx->current_frame == img->dec_ctx->dest_frame) {
            lv_timer_pause(t);
            img->ctrl ^= LV_IMG_CTRL_STOPAT | LV_IMG_CTRL_PAUSE;
            lv_event_send(obj, LV_EVENT_READY, NULL);
        }
        else if(LV_BT(img->ctrl, LV_IMG_CTRL_BACKWARD)) {
            if(img->dec_ctx->current_frame > 0)
                --img->dec_ctx->current_frame;
            else { /* Looping ? */
                if(LV_BT(img->ctrl, LV_IMG_CTRL_LOOP))
                    img->dec_ctx->current_frame = img->dec_ctx->total_frames - 1;
                else {
                    lv_event_send(obj, LV_EVENT_READY, NULL);
                    lv_timer_pause(t);
                }
            }
        }
        else {
            ++img->dec_ctx->current_frame;
            if(img->dec_ctx->current_frame == img->dec_ctx->total_frames) {
                /* Looping ? */
                if(LV_BF(img->ctrl, LV_IMG_CTRL_LOOP)) {
                    img->dec_ctx->current_frame--; /*Pause on the last frame*/
                    lv_event_send(obj, LV_EVENT_READY, NULL);
                    lv_timer_pause(t);
                }
                else
                    img->dec_ctx->current_frame = 0;
            }
        }
    }

    lv_obj_invalidate(obj);
}

static void next_frame_task_cb(lv_timer_t * t)
{
    lv_obj_t * obj = t->user_data;
    lv_img_t * img = (lv_img_t *) obj;

    /* If the object isn't visible, just end this iteration */
    if(!lv_obj_is_visible(obj)) {
        img->dec_ctx->last_rendering = lv_tick_get();
        return;
    }

    /* Variable frame rate animation are usually more complex to deal with*/
    uint16_t now = lv_tick_get(); /*Using 16bits to save memory in the decoder context */
    /* Handle rollover here with the small counter */
    uint16_t elapsed = now >= img->dec_ctx->last_rendering ? now - img->dec_ctx->last_rendering
                       : UINT16_MAX - img->dec_ctx->last_rendering + 1 + now;
    if(elapsed < img->dec_ctx->frame_delay) return;

    uint16_t err_delay = elapsed - img->dec_ctx->frame_delay; /* Don't accumulate error due to timer period  */
    img->dec_ctx->last_rendering = now - err_delay; /* Let's diffuse the error */

    if(LV_BT(img->ctrl, LV_IMG_CTRL_PAUSE)) {
        /* Pause the timer too when it has run once to avoid CPU consumption */
        lv_timer_pause(t);
        return;
    }

    /* End of animation ?*/
    if(img->dec_ctx->current_frame == img->dec_ctx->total_frames) {
        /* If the animation is looping and we have the loop flag, let's rewind */
        if(LV_BT(img->ctrl, LV_IMG_CTRL_LOOP) || LV_BT(img->caps, LV_IMG_DEC_LOOPING)) {
            /* The user overrided the animation's format or it's the standard mode, let's rewind */
            img->dec_ctx->current_frame = 0; /* Rewind */
        }
        else {
            lv_res_t res = lv_event_send(obj, LV_EVENT_READY, NULL);
            lv_timer_pause(t);
            if(res != LV_FS_RES_OK) return;
        }
    }
    else {
        img->dec_ctx->current_frame++;
    }

    lv_obj_invalidate(obj);
}

#endif
