/**
 * @file lv_font_manager_backends.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_font_manager_backends.h"

#if LV_USE_FONT_MANAGER

#include "../../libs/freetype/lv_freetype.h"
#include "../../stdlib/lv_sprintf.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_font_t * builtin_font_create_cb(const lv_font_info_t * info, const void * src);
static void builtin_font_delete_cb(lv_font_t *);
static void * builtin_font_dup_src_cb(const void * src);
static void builtin_font_free_src_cb(void * src);

#if LV_USE_FREETYPE
    static lv_font_t * freetype_font_create_cb(const lv_font_info_t * info, const void * src);
    static void freetype_font_delete_cb(lv_font_t * font);
    static void * freetype_font_dup_src_cb(const void * src);
    static void freetype_font_free_src_cb(void * src);
#endif /*LV_USE_FREETYPE*/

/**********************
 *  STATIC VARIABLES
 **********************/

const lv_font_class_t lv_builtin_font_class = {
    .create_cb = builtin_font_create_cb,
    .delete_cb = builtin_font_delete_cb,
    .dup_src_cb = builtin_font_dup_src_cb,
    .free_src_cb = builtin_font_free_src_cb,
};

#if LV_USE_FREETYPE

const lv_font_class_t lv_freetype_font_class = {
    .create_cb = freetype_font_create_cb,
    .delete_cb = freetype_font_delete_cb,
    .dup_src_cb = freetype_font_dup_src_cb,
    .free_src_cb = freetype_font_free_src_cb,
};

#endif /*LV_USE_FREETYPE*/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

static lv_font_t * builtin_font_create_cb(const lv_font_info_t * info, const void * src)
{
    const lv_builtin_font_src_t * font_src = src;
    while(font_src->font_p) {
        if(info->size == font_src->size) {
            return (lv_font_t *)font_src->font_p;
        }
        font_src++;
    }

    LV_LOG_WARN("No built-in font found with size: %" LV_PRIu32, info->size);
    return NULL;
}

static void builtin_font_delete_cb(lv_font_t *)
{
    /*Nothing to delete*/
}

static void * builtin_font_dup_src_cb(const void * src)
{
    const lv_builtin_font_src_t * font_src = src;
    uint32_t len = 0;

    /*Measure the size of the source data*/
    while(font_src->font_p) {
        len++;
        font_src++;
    }

    if(len == 0) {
        LV_LOG_WARN("No source data found");
        return NULL;
    }

    lv_builtin_font_src_t * new_src = lv_malloc(sizeof(lv_builtin_font_src_t) * len);
    LV_ASSERT_MALLOC(new_src);
    lv_memcpy(new_src, src, sizeof(lv_builtin_font_src_t) * len);

    return new_src;
}

static void builtin_font_free_src_cb(void * src)
{
    lv_free(src);
}

#if LV_USE_FREETYPE

static lv_font_t * freetype_font_create_cb(const lv_font_info_t * info, const void * src)
{
    return lv_freetype_font_create(src, info->render_mode, info->size, info->style);
}

static void freetype_font_delete_cb(lv_font_t * font)
{
    lv_freetype_font_delete(font);
}

static void * freetype_font_dup_src_cb(const void * src)
{
    return lv_strdup(src);
}

static void freetype_font_free_src_cb(void * src)
{
    lv_free(src);
}

#endif /*LV_USE_FREETYPE*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_FONT_MANAGER*/
