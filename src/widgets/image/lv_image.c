/**
 * @file lv_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_private.h"
#include "../../misc/lv_area_private.h"
#include "../../draw/lv_draw_image_private.h"
#include "../../draw/lv_draw_private.h"
#include "../../core/lv_obj_event_private.h"
#include "../../core/lv_obj_class_private.h"
#include "../../core/lv_obj_class_private.h"
#include "../../core/lv_obj_draw_private.h"

#if LV_USE_IMAGE != 0

#include "../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_image_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_image_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_image_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_image_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void draw_image(lv_event_t * e);
static void scale_update(lv_obj_t * obj, int32_t scale_x, int32_t scale_y);
static void update_align(lv_obj_t * obj);
#if LV_USE_OBJ_PROPERTY
    static void lv_image_set_pivot_helper(lv_obj_t * obj, lv_point_t * pivot);
    static lv_point_t lv_image_get_pivot_helper(lv_obj_t * obj);
#endif

#if LV_USE_OBJ_PROPERTY
static const lv_property_ops_t properties[] = {
    {
        .id = LV_PROPERTY_IMAGE_SRC,
        .setter = lv_image_set_src,
        .getter = lv_image_get_src,
    },
    {
        .id = LV_PROPERTY_IMAGE_OFFSET_X,
        .setter = lv_image_set_offset_x,
        .getter = lv_image_get_offset_x,
    },
    {
        .id = LV_PROPERTY_IMAGE_OFFSET_Y,
        .setter = lv_image_set_offset_y,
        .getter = lv_image_get_offset_y,
    },
    {
        .id = LV_PROPERTY_IMAGE_ROTATION,
        .setter = lv_image_set_rotation,
        .getter = lv_image_get_rotation,
    },
    {
        .id = LV_PROPERTY_IMAGE_PIVOT,
        .setter = lv_image_set_pivot_helper,
        .getter = lv_image_get_pivot_helper,
    },
    {
        .id = LV_PROPERTY_IMAGE_SCALE,
        .setter = lv_image_set_scale,
        .getter = lv_image_get_scale,
    },
    {
        .id = LV_PROPERTY_IMAGE_SCALE_X,
        .setter = lv_image_set_scale_x,
        .getter = lv_image_get_scale_x,
    },
    {
        .id = LV_PROPERTY_IMAGE_SCALE_Y,
        .setter = lv_image_set_scale_y,
        .getter = lv_image_get_scale_y,
    },
    {
        .id = LV_PROPERTY_IMAGE_BLEND_MODE,
        .setter = lv_image_set_blend_mode,
        .getter = lv_image_get_blend_mode,
    },
    {
        .id = LV_PROPERTY_IMAGE_ANTIALIAS,
        .setter = lv_image_set_antialias,
        .getter = lv_image_get_antialias,
    },
    {
        .id = LV_PROPERTY_IMAGE_INNER_ALIGN,
        .setter = lv_image_set_inner_align,
        .getter = lv_image_get_inner_align,
    },
};
#endif

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_image_class = {
    .constructor_cb = lv_image_constructor,
    .destructor_cb = lv_image_destructor,
    .event_cb = lv_image_event,
    .width_def = LV_SIZE_CONTENT,
    .height_def = LV_SIZE_CONTENT,
    .instance_size = sizeof(lv_image_t),
    .base_class = &lv_obj_class,
    .name = "lv_image",
#if LV_USE_OBJ_PROPERTY
    .prop_index_start = LV_PROPERTY_IMAGE_START,
    .prop_index_end = LV_PROPERTY_IMAGE_END,
    .properties = properties,
    .properties_count = sizeof(properties) / sizeof(properties[0]),

#if LV_USE_OBJ_PROPERTY_NAME
    .property_names = lv_image_property_names,
    .names_count = sizeof(lv_image_property_names) / sizeof(lv_property_name_t),
#endif

#endif
};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_image_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

void lv_image_set_src(lv_obj_t * obj, const void * src)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_obj_invalidate(obj);

    lv_image_src_t src_type = lv_image_src_get_type(src);
    lv_image_t * img = (lv_image_t *)obj;

#if LV_USE_LOG && LV_LOG_LEVEL <= LV_LOG_LEVEL_INFO
    switch(src_type) {
        case LV_IMAGE_SRC_FILE:
            LV_LOG_TRACE("`LV_IMAGE_SRC_FILE` type found");
            break;
        case LV_IMAGE_SRC_VARIABLE:
            LV_LOG_TRACE("`LV_IMAGE_SRC_VARIABLE` type found");
            break;
        case LV_IMAGE_SRC_SYMBOL:
            LV_LOG_TRACE("`LV_IMAGE_SRC_SYMBOL` type found");
            break;
        default:
            LV_LOG_WARN("unknown type");
    }
#endif

    /*If the new source type is unknown free the memories of the old source*/
    if(src_type == LV_IMAGE_SRC_UNKNOWN) {
        if(src) LV_LOG_WARN("unknown image type");
        if(img->src_type == LV_IMAGE_SRC_SYMBOL || img->src_type == LV_IMAGE_SRC_FILE) {
            lv_free((void *)img->src);
        }
        img->src      = NULL;
        img->src_type = LV_IMAGE_SRC_UNKNOWN;
        return;
    }

    lv_image_header_t header;
    lv_result_t res = lv_image_decoder_get_info(src, &header);
    if(res != LV_RESULT_OK) {
#if LV_USE_LOG
        char buf[24];
        LV_LOG_WARN("failed to get image info: %s",
                    src_type == LV_IMAGE_SRC_FILE ? (const char *)src : (lv_snprintf(buf, sizeof(buf), "%p", src), buf));
#endif /*LV_USE_LOG*/
        return;
    }

    /*Save the source*/
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        if(header.flags & LV_IMAGE_FLAGS_ALLOCATED) {
            lv_draw_buf_t * buf = (lv_draw_buf_t *)src;
            if(!buf->unaligned_data || !buf->handlers) {
                LV_LOG_ERROR("Invalid draw buffer, unaligned_data: %p, handlers: %p",
                             buf->unaligned_data, (void *)buf->handlers);
                return;
            }
        }

        /*If memory was allocated because of the previous `src_type` then free it*/
        if(img->src_type == LV_IMAGE_SRC_FILE || img->src_type == LV_IMAGE_SRC_SYMBOL) {
            lv_free((void *)img->src);
        }
        img->src = src;
    }
    else if(src_type == LV_IMAGE_SRC_FILE || src_type == LV_IMAGE_SRC_SYMBOL) {
        /*If the new and the old src are the same then it was only a refresh.*/
        if(img->src != src) {
            const void * old_src = NULL;
            /*If memory was allocated because of the previous `src_type` then save its pointer and free after allocation.
             *It's important to allocate first to be sure the new data will be on a new address.
             *Else `img_cache` wouldn't see the change in source.*/
            if(img->src_type == LV_IMAGE_SRC_FILE || img->src_type == LV_IMAGE_SRC_SYMBOL) {
                old_src = img->src;
            }
            char * new_str = lv_strdup(src);
            LV_ASSERT_MALLOC(new_str);
            if(new_str == NULL) return;
            img->src = new_str;

            if(old_src) lv_free((void *)old_src);
        }
    }

    if(src_type == LV_IMAGE_SRC_SYMBOL) {
        /*`lv_image_dsc_get_info` couldn't set the width and height of a font so set it here*/
        const lv_font_t * font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
        int32_t letter_space = lv_obj_get_style_text_letter_space(obj, LV_PART_MAIN);
        int32_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
        lv_point_t size;
        lv_text_get_size(&size, src, font, letter_space, line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        header.w = size.x;
        header.h = size.y;
    }

    img->src_type = src_type;
    img->w        = header.w;
    img->h        = header.h;
    img->cf       = header.cf;

    lv_obj_refresh_self_size(obj);

    update_align(obj);

    /*Provide enough room for the rotated corners*/
    if(img->rotation || img->scale_x != LV_SCALE_NONE || img->scale_y != LV_SCALE_NONE) {
        lv_obj_refresh_ext_draw_size(obj);
    }

    lv_obj_invalidate(obj);
}

void lv_image_set_offset_x(lv_obj_t * obj, int32_t x)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    img->offset.x = x;
    lv_obj_invalidate(obj);
}

void lv_image_set_offset_y(lv_obj_t * obj, int32_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    img->offset.y = y;
    lv_obj_invalidate(obj);
}

void lv_image_set_rotation(lv_obj_t * obj, int32_t angle)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    if(img->align > LV_IMAGE_ALIGN_AUTO_TRANSFORM) {
        angle = 0;
    }
    else {
        while(angle >= 3600) angle -= 3600;
        while(angle < 0) angle += 3600;
    }

    if((uint32_t)angle == img->rotation) return;

    lv_obj_update_layout(obj);  /*Be sure the object's size is calculated*/
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    lv_area_t a;
    lv_point_t pivot_px;
    lv_image_get_pivot(obj, &pivot_px);
    lv_image_buf_get_transformed_area(&a, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);

    img->rotation = angle;

    /* Disable invalidations because lv_obj_refresh_ext_draw_size would invalidate
     * the whole ext draw area */
    lv_display_t * disp = lv_obj_get_display(obj);
    lv_display_enable_invalidation(disp, false);
    lv_obj_refresh_ext_draw_size(obj);
    lv_display_enable_invalidation(disp, true);

    lv_image_buf_get_transformed_area(&a, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);
}

void lv_image_set_pivot(lv_obj_t * obj, int32_t x, int32_t y)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    if(img->align > LV_IMAGE_ALIGN_AUTO_TRANSFORM) {
        x = 0;
        y = 0;
    }

    if(img->pivot.x == x && img->pivot.y == y) return;

    lv_obj_update_layout(obj);  /*Be sure the object's size is calculated*/
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    lv_area_t a;
    lv_point_t pivot_px;
    lv_image_get_pivot(obj, &pivot_px);
    lv_image_buf_get_transformed_area(&a, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);

    lv_point_set(&img->pivot, x, y);

    /* Disable invalidations because lv_obj_refresh_ext_draw_size would invalidate
     * the whole ext draw area */
    lv_display_t * disp = lv_obj_get_display(obj);
    lv_display_enable_invalidation(disp, false);
    lv_obj_refresh_ext_draw_size(obj);
    lv_display_enable_invalidation(disp, true);

    lv_image_get_pivot(obj, &pivot_px);
    lv_image_buf_get_transformed_area(&a, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
    a.x1 += obj->coords.x1;
    a.y1 += obj->coords.y1;
    a.x2 += obj->coords.x1;
    a.y2 += obj->coords.y1;
    lv_obj_invalidate_area(obj, &a);
}

void lv_image_set_scale(lv_obj_t * obj, uint32_t zoom)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    /*If scale is set internally, do no overwrite it*/
    if(img->align > LV_IMAGE_ALIGN_AUTO_TRANSFORM) return;

    if(zoom == img->scale_x && zoom == img->scale_y) return;

    if(zoom == 0) zoom = 1;

    scale_update(obj, zoom, zoom);
}

void lv_image_set_scale_x(lv_obj_t * obj, uint32_t zoom)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    /*If scale is set internally, do no overwrite it*/
    if(img->align > LV_IMAGE_ALIGN_AUTO_TRANSFORM) return;

    if(zoom == img->scale_x) return;

    if(zoom == 0) zoom = 1;

    scale_update(obj, zoom, img->scale_y);
}

void lv_image_set_scale_y(lv_obj_t * obj, uint32_t zoom)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    /*If scale is set internally, do no overwrite it*/
    if(img->align > LV_IMAGE_ALIGN_AUTO_TRANSFORM) return;

    if(zoom == img->scale_y) return;

    if(zoom == 0) zoom = 1;

    scale_update(obj, img->scale_x, zoom);
}

void lv_image_set_blend_mode(lv_obj_t * obj, lv_blend_mode_t blend_mode)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    /*If scale is set internally, do no overwrite it*/
    if(img->blend_mode == blend_mode) return;

    img->blend_mode = blend_mode;

    lv_obj_invalidate(obj);
}

void lv_image_set_antialias(lv_obj_t * obj, bool antialias)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    if(antialias == img->antialias) return;

    img->antialias = antialias;
    lv_obj_invalidate(obj);
}

void lv_image_set_inner_align(lv_obj_t * obj, lv_image_align_t align)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    if(align == img->align) return;

    /*If we're removing STRETCH, reset the scale*/
    if(img->align == LV_IMAGE_ALIGN_STRETCH || img->align == LV_IMAGE_ALIGN_CONTAIN ||
       img->align == LV_IMAGE_ALIGN_COVER) {
        lv_image_set_scale(obj, LV_SCALE_NONE);
    }

    img->align = align;
    update_align(obj);

    lv_obj_invalidate(obj);
}

void lv_image_set_bitmap_map_src(lv_obj_t * obj, const lv_image_dsc_t * src)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_image_t * img = (lv_image_t *)obj;
    img->bitmap_mask_src = src;
    lv_obj_invalidate(obj);
}

/*=====================
 * Getter functions
 *====================*/

const void * lv_image_get_src(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->src;
}

int32_t lv_image_get_offset_x(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->offset.x;
}

int32_t lv_image_get_offset_y(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->offset.y;
}

int32_t lv_image_get_rotation(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->rotation;
}

void lv_image_get_pivot(lv_obj_t * obj, lv_point_t * pivot)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    pivot->x = lv_pct_to_px(img->pivot.x, img->w);
    pivot->y = lv_pct_to_px(img->pivot.y, img->h);
}

int32_t lv_image_get_scale(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->scale_x;
}

int32_t lv_image_get_scale_x(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->scale_x;
}

int32_t lv_image_get_scale_y(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->scale_y;
}

int32_t lv_image_get_src_width(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    update_align(obj);

    return img->w;
}

int32_t lv_image_get_src_height(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    update_align(obj);

    return img->h;
}

int32_t lv_image_get_transformed_width(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    update_align(obj);

    lv_point_t pivot_px;
    lv_image_get_pivot(obj, &pivot_px);

    lv_area_t a;
    lv_image_buf_get_transformed_area(&a, img->w, img->h, img->rotation, img->scale_x, img->scale_y, &pivot_px);

    return lv_area_get_width(&a);
}

int32_t lv_image_get_transformed_height(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;
    update_align(obj);

    lv_point_t pivot_px;
    lv_image_get_pivot(obj, &pivot_px);

    lv_area_t a;
    lv_image_buf_get_transformed_area(&a, img->w, img->h, img->rotation, img->scale_x, img->scale_y, &pivot_px);

    return lv_area_get_height(&a);
}

lv_blend_mode_t lv_image_get_blend_mode(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->blend_mode;
}

bool lv_image_get_antialias(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->antialias ? true : false;
}

lv_image_align_t lv_image_get_inner_align(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->align;
}

const lv_image_dsc_t * lv_image_get_bitmap_map_src(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);

    lv_image_t * img = (lv_image_t *)obj;

    return img->bitmap_mask_src;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_image_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");

    lv_image_t * img = (lv_image_t *)obj;

    img->src       = NULL;
    img->src_type  = LV_IMAGE_SRC_UNKNOWN;
    img->cf        = LV_COLOR_FORMAT_UNKNOWN;
    img->w         = lv_obj_get_width(obj);
    img->h         = lv_obj_get_height(obj);
    img->rotation     = 0;
    img->scale_x      = LV_SCALE_NONE;
    img->scale_y      = LV_SCALE_NONE;
    img->antialias = LV_COLOR_DEPTH > 8 ? 1 : 0;
    lv_point_set(&img->offset, 0, 0);
    lv_point_set(&img->pivot, LV_PCT(50), LV_PCT(50)); /*Default pivot to image center*/
    img->align     = LV_IMAGE_ALIGN_CENTER;

    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(obj, LV_OBJ_FLAG_ADV_HITTEST);

    LV_TRACE_OBJ_CREATE("finished");
}

static void lv_image_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_image_t * img = (lv_image_t *)obj;
    if(img->src_type == LV_IMAGE_SRC_FILE || img->src_type == LV_IMAGE_SRC_SYMBOL) {
        lv_free((void *)img->src);
        img->src      = NULL;
        img->src_type = LV_IMAGE_SRC_UNKNOWN;
    }
}

static void lv_image_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    lv_event_code_t code = lv_event_get_code(e);

    /*Call the ancestor's event handler*/
    lv_result_t res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_image_t * img = (lv_image_t *)obj;
    lv_point_t pivot_px;
    lv_image_get_pivot(obj, &pivot_px);

    if(code == LV_EVENT_STYLE_CHANGED) {
        /*Refresh the file name to refresh the symbol text size*/
        if(img->src_type == LV_IMAGE_SRC_SYMBOL) {
            lv_image_set_src(obj, img->src);
        }
        else {
            /*With transformation it might change*/
            lv_obj_refresh_ext_draw_size(obj);
        }
    }
    else if(code == LV_EVENT_REFR_EXT_DRAW_SIZE) {

        int32_t * s = lv_event_get_param(e);

        /*If the image has angle provide enough room for the rotated corners*/
        if(img->rotation || img->scale_x != LV_SCALE_NONE || img->scale_y != LV_SCALE_NONE) {
            lv_area_t a;
            int32_t w = lv_obj_get_width(obj);
            int32_t h = lv_obj_get_height(obj);
            lv_image_buf_get_transformed_area(&a, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
            *s = LV_MAX(*s, -a.x1);
            *s = LV_MAX(*s, -a.y1);
            *s = LV_MAX(*s, a.x2 - w);
            *s = LV_MAX(*s, a.y2 - h);
        }
    }
    else if(code == LV_EVENT_SIZE_CHANGED) {
        if(img->align == LV_IMAGE_ALIGN_STRETCH || img->align == LV_IMAGE_ALIGN_CONTAIN ||
           img->align == LV_IMAGE_ALIGN_COVER) {
            update_align(obj);
            if(img->rotation || img->scale_x != LV_SCALE_NONE || img->scale_y != LV_SCALE_NONE) {
                lv_obj_refresh_ext_draw_size(obj);
            }
        }
    }
    else if(code == LV_EVENT_HIT_TEST) {
        lv_hit_test_info_t * info = lv_event_get_param(e);

        /*If the object is exactly image sized (not cropped, not mosaic) and transformed
         *perform hit test on its transformed area*/
        if(img->w == lv_obj_get_width(obj) && img->h == lv_obj_get_height(obj) &&
           (img->scale_x != LV_SCALE_NONE || img->scale_y != LV_SCALE_NONE ||
            img->rotation != 0)) {

            int32_t w = lv_obj_get_width(obj);
            int32_t h = lv_obj_get_height(obj);
            lv_area_t coords;
            lv_image_buf_get_transformed_area(&coords, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
            coords.x1 += obj->coords.x1;
            coords.y1 += obj->coords.y1;
            coords.x2 += obj->coords.x1;
            coords.y2 += obj->coords.y1;

            info->res = lv_area_is_point_on(&coords, info->point, 0);
        }
        else {
            lv_area_t a;
            lv_obj_get_click_area(obj, &a);
            info->res = lv_area_is_point_on(&a, info->point, 0);
        }
    }
    else if(code == LV_EVENT_GET_SELF_SIZE) {
        lv_point_t * p = lv_event_get_param(e);
        p->x = img->w;
        p->y = img->h;
    }
    else if(code == LV_EVENT_DRAW_MAIN || code == LV_EVENT_DRAW_POST || code == LV_EVENT_COVER_CHECK) {
        draw_image(e);
    }
}

static void draw_image(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_image_t * img = (lv_image_t *)obj;
    if(code == LV_EVENT_COVER_CHECK) {
        lv_cover_check_info_t * info = lv_event_get_param(e);
        if(info->res == LV_COVER_RES_MASKED) return;
        if(img->src_type == LV_IMAGE_SRC_UNKNOWN || img->src_type == LV_IMAGE_SRC_SYMBOL) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        /*Non true color format might have "holes"*/
        if(lv_color_format_has_alpha(img->cf)) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        /*With not LV_OPA_COVER images can't cover an area */
        if(lv_obj_get_style_image_opa(obj, LV_PART_MAIN) != LV_OPA_COVER) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        if(img->rotation != 0) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }

        if(img->scale_x == LV_SCALE_NONE && img->scale_y == LV_SCALE_NONE) {
            if(lv_area_is_in(info->area, &obj->coords, 0) == false) {
                info->res = LV_COVER_RES_NOT_COVER;
                return;
            }
        }
        else {
            lv_area_t a;
            lv_point_t pivot_px;
            lv_image_get_pivot(obj, &pivot_px);
            lv_image_buf_get_transformed_area(&a, lv_obj_get_width(obj), lv_obj_get_height(obj), 0, img->scale_x, img->scale_y,
                                              &pivot_px);
            a.x1 += obj->coords.x1;
            a.y1 += obj->coords.y1;
            a.x2 += obj->coords.x1;
            a.y2 += obj->coords.y1;

            if(lv_area_is_in(info->area, &a, 0) == false) {
                info->res = LV_COVER_RES_NOT_COVER;
                return;
            }
        }
        if(img->bitmap_mask_src) {
            info->res = LV_COVER_RES_NOT_COVER;
            return;
        }
    }
    else if(code == LV_EVENT_DRAW_MAIN) {

        if(img->h == 0 || img->w == 0) return;
        if(img->scale_x == 0 || img->scale_y == 0) return;

        lv_layer_t * layer = lv_event_get_layer(e);

        if(img->src_type == LV_IMAGE_SRC_FILE || img->src_type == LV_IMAGE_SRC_VARIABLE) {
            lv_draw_image_dsc_t draw_dsc;
            lv_draw_image_dsc_init(&draw_dsc);
            draw_dsc.base.layer = layer;
            lv_obj_init_draw_image_dsc(obj, LV_PART_MAIN, &draw_dsc);

            lv_area_t clip_area_ori = layer->_clip_area;

            lv_image_get_pivot(obj, &draw_dsc.pivot);
            draw_dsc.scale_x = img->scale_x;
            draw_dsc.scale_y = img->scale_y;
            draw_dsc.rotation = img->rotation;
            draw_dsc.antialias = img->antialias;
            draw_dsc.blend_mode = img->blend_mode;
            draw_dsc.bitmap_mask_src = img->bitmap_mask_src;
            draw_dsc.src = img->src;

            lv_area_set(&draw_dsc.image_area, obj->coords.x1,
                        obj->coords.y1,
                        obj->coords.x1 + img->w - 1,
                        obj->coords.y1 + img->h - 1);

            draw_dsc.clip_radius = lv_obj_get_style_radius(obj, LV_PART_MAIN);

            lv_area_t coords;
            if(img->align < LV_IMAGE_ALIGN_AUTO_TRANSFORM) {
                lv_area_align(&obj->coords, &draw_dsc.image_area, img->align, img->offset.x, img->offset.y);
                coords = draw_dsc.image_area;
            }
            else if(img->align == LV_IMAGE_ALIGN_CONTAIN || img->align == LV_IMAGE_ALIGN_COVER) {
                int32_t scale = lv_image_get_scale(obj);
                lv_point_t offset;
                offset.x = (lv_obj_get_width(obj) - img->w * scale / LV_SCALE_NONE) / 2;
                offset.y = (lv_obj_get_height(obj) - img->h * scale / LV_SCALE_NONE) / 2;
                lv_area_move(&draw_dsc.image_area, offset.x, offset.y);
                lv_area_move(&draw_dsc.image_area, img->offset.x, img->offset.y);
                coords = draw_dsc.image_area;
            }
            else if(img->align == LV_IMAGE_ALIGN_TILE) {
                lv_area_intersect(&layer->_clip_area, &layer->_clip_area, &obj->coords);
                lv_area_move(&draw_dsc.image_area, img->offset.x, img->offset.y);

                lv_area_move(&draw_dsc.image_area,
                             ((layer->_clip_area.x1 - draw_dsc.image_area.x1 - (img->w - 1)) / img->w) * img->w,
                             ((layer->_clip_area.y1 - draw_dsc.image_area.y1 - (img->h - 1)) / img->h) * img->h);
                coords = layer->_clip_area;
                draw_dsc.tile = 1;
            }
            else {
                coords = draw_dsc.image_area;
            }

            lv_draw_image(layer, &draw_dsc, &coords);
            layer->_clip_area = clip_area_ori;
        }
        else if(img->src_type == LV_IMAGE_SRC_SYMBOL) {
            lv_draw_label_dsc_t label_dsc;
            lv_draw_label_dsc_init(&label_dsc);
            label_dsc.base.layer = layer;
            lv_obj_init_draw_label_dsc(obj, LV_PART_MAIN, &label_dsc);
            label_dsc.text = img->src;
            lv_area_t * coords;
            lv_area_t aligned_coords;
            const bool image_area_is_same_as_coords = img->w == lv_area_get_width(&obj->coords)
                                                      && img->h == lv_area_get_height(&obj->coords);
            const bool needs_inner_alignment = img->align > LV_IMAGE_ALIGN_TOP_LEFT
                                               && !image_area_is_same_as_coords;
            const bool has_offset = img->offset.x || img->offset.y;
            const bool inner_alignment_is_transforming = img->align >= LV_IMAGE_ALIGN_AUTO_TRANSFORM;
            if((needs_inner_alignment || has_offset) && !inner_alignment_is_transforming) {
                lv_point_t text_size;
                lv_text_get_size(&text_size, label_dsc.text, label_dsc.font, label_dsc.letter_space,
                                 label_dsc.line_space, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
                lv_area_set(&aligned_coords, 0, 0, text_size.x, text_size.y);
                lv_area_align(&obj->coords, &aligned_coords, img->align, img->offset.x, img->offset.y);
                coords = &aligned_coords;
            }
            else {
                coords = &obj->coords;
            }
            lv_draw_label(layer, &label_dsc, coords);
        }
        else if(img->src == NULL) {
            /*Do not need to draw image when src is NULL*/
            LV_LOG_WARN("image source is NULL");
        }
        else {
            /*Trigger the error handler of image draw*/
            LV_LOG_WARN("image source type is unknown");
        }
    }
}

static void scale_update(lv_obj_t * obj, int32_t scale_x, int32_t scale_y)
{
    lv_image_t * img = (lv_image_t *)obj;

    lv_obj_update_layout(obj);  /*Be sure the object's size is calculated*/
    int32_t w = lv_obj_get_width(obj);
    int32_t h = lv_obj_get_height(obj);
    lv_area_t a;
    lv_point_t pivot_px;
    lv_image_get_pivot(obj, &pivot_px);
    lv_image_buf_get_transformed_area(&a, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
    a.x1 += obj->coords.x1 - 1;
    a.y1 += obj->coords.y1 - 1;
    a.x2 += obj->coords.x1 + 1;
    a.y2 += obj->coords.y1 + 1;
    lv_obj_invalidate_area(obj, &a);

    img->scale_x = scale_x;
    img->scale_y = scale_y;

    /* Disable invalidations because lv_obj_refresh_ext_draw_size would invalidate
     * the whole ext draw area */
    lv_display_t * disp = lv_obj_get_display(obj);
    lv_display_enable_invalidation(disp, false);
    lv_obj_refresh_ext_draw_size(obj);
    lv_display_enable_invalidation(disp, true);

    lv_image_buf_get_transformed_area(&a, w, h, img->rotation, img->scale_x, img->scale_y, &pivot_px);
    a.x1 += obj->coords.x1 - 1;
    a.y1 += obj->coords.y1 - 1;
    a.x2 += obj->coords.x1 + 1;
    a.y2 += obj->coords.y1 + 1;
    lv_obj_invalidate_area(obj, &a);
}

static void update_align(lv_obj_t * obj)
{
    lv_image_t * img = (lv_image_t *)obj;
    if(img->align == LV_IMAGE_ALIGN_STRETCH) {
        lv_image_set_rotation(obj, 0);
        lv_image_set_pivot(obj, 0, 0);
        if(img->w != 0 && img->h != 0) {
            lv_obj_update_layout(obj);
            int32_t scale_x = lv_obj_get_width(obj) * LV_SCALE_NONE / img->w;
            int32_t scale_y = lv_obj_get_height(obj) * LV_SCALE_NONE / img->h;
            scale_update(obj, scale_x, scale_y);
        }
    }
    else if(img->align == LV_IMAGE_ALIGN_CONTAIN) {
        lv_image_set_rotation(obj, 0);
        lv_image_set_pivot(obj, 0, 0);
        if(img->w != 0 && img->h != 0) {
            lv_obj_update_layout(obj);
            int32_t scale_x = lv_obj_get_width(obj) * LV_SCALE_NONE / img->w;
            int32_t scale_y = lv_obj_get_height(obj) * LV_SCALE_NONE / img->h;
            int32_t scale = LV_MIN(scale_x, scale_y);
            scale_update(obj, scale, scale);
        }
    }
    else if(img->align == LV_IMAGE_ALIGN_COVER) {
        lv_image_set_rotation(obj, 0);
        lv_image_set_pivot(obj, 0, 0);
        if(img->w != 0 && img->h != 0) {
            lv_obj_update_layout(obj);
            int32_t scale_x = lv_obj_get_width(obj) * LV_SCALE_NONE / img->w;
            int32_t scale_y = lv_obj_get_height(obj) * LV_SCALE_NONE / img->h;
            int32_t scale = LV_MAX(scale_x, scale_y);
            scale_update(obj, scale, scale);
        }
    }
    else if(img->align == LV_IMAGE_ALIGN_TILE) {
        lv_image_set_rotation(obj, 0);
        lv_image_set_pivot(obj, 0, 0);
        scale_update(obj, LV_SCALE_NONE, LV_SCALE_NONE);
    }
}

#if LV_USE_OBJ_PROPERTY
static void lv_image_set_pivot_helper(lv_obj_t * obj, lv_point_t * pivot)
{
    lv_image_set_pivot(obj, pivot->x, pivot->y);
}

static lv_point_t lv_image_get_pivot_helper(lv_obj_t * obj)
{
    lv_point_t pivot;
    lv_image_get_pivot(obj, &pivot);
    return pivot;
}
#endif

#endif
