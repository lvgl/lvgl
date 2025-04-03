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
#include "../../libs/tiny_ttf/lv_tiny_ttf.h"
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
static void builtin_font_delete_cb(lv_font_t * font);
static void * builtin_font_dup_src_cb(const void * src);
static void builtin_font_free_src_cb(void * src);

#if LV_USE_FREETYPE
    static lv_font_t * freetype_font_create_cb(const lv_font_info_t * info, const void * src);
    static void freetype_font_delete_cb(lv_font_t * font);
    static void * freetype_font_dup_src_cb(const void * src);
    static void freetype_font_free_src_cb(void * src);
#endif /*LV_USE_FREETYPE*/

#if LV_USE_TINY_TTF
    static lv_font_t * tiny_ttf_font_create_cb(const lv_font_info_t * info, const void * src);
    static void tiny_ttf_font_delete_cb(lv_font_t * font);
    static void * tiny_ttf_font_dup_src_cb(const void * src);
    static void tiny_ttf_font_free_src_cb(void * src);
#endif /*LV_USE_TINY_TTF*/

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

#if LV_USE_TINY_TTF
const lv_font_class_t lv_tiny_ttf_font_class = {
    .create_cb = tiny_ttf_font_create_cb,
    .delete_cb = tiny_ttf_font_delete_cb,
    .dup_src_cb = tiny_ttf_font_dup_src_cb,
    .free_src_cb = tiny_ttf_font_free_src_cb,
};
#endif /*LV_USE_TINY_TTF*/

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

static void builtin_font_delete_cb(lv_font_t * font)
{
    /*Nothing to delete*/
    LV_UNUSED(font);
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

    lv_builtin_font_src_t * new_src = lv_malloc_zeroed(sizeof(lv_builtin_font_src_t) * (len + 1));
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

#if LV_USE_TINY_TTF

static lv_font_t * tiny_ttf_font_create_cb(const lv_font_info_t * info, const void * src)
{
    const lv_tiny_ttf_font_src_t * font_src = src;

    if(font_src->path) {
#if LV_TINY_TTF_FILE_SUPPORT
        if(font_src->cache_size) {
            return lv_tiny_ttf_create_file_ex(font_src->path, info->size, info->kerning, font_src->cache_size);
        }

        return lv_tiny_ttf_create_file(font_src->path, info->size);
#else
        LV_LOG_WARN("LV_TINY_TTF_FILE_SUPPORT not enabled");
        return NULL;
#endif
    }

    if(font_src->cache_size) {
        return lv_tiny_ttf_create_data_ex(font_src->data, font_src->data_size, info->size, info->kerning, font_src->cache_size);
    }

    return lv_tiny_ttf_create_data(font_src->data, font_src->data_size, info->size);
}

static void tiny_ttf_font_delete_cb(lv_font_t * font)
{
    lv_tiny_ttf_destroy(font);
}

static void * tiny_ttf_font_dup_src_cb(const void * src)
{
    const lv_tiny_ttf_font_src_t * font_src = src;

    lv_tiny_ttf_font_src_t * new_src = lv_malloc_zeroed(sizeof(lv_tiny_ttf_font_src_t));
    LV_ASSERT_MALLOC(new_src);
    *new_src = *font_src;

    if(font_src->path) {
        new_src->path = lv_strdup(font_src->path);
    }

    return new_src;
}

static void tiny_ttf_font_free_src_cb(void * src)
{
    lv_tiny_ttf_font_src_t * font_src = src;
    if(font_src->path) {
        lv_free((char *)font_src->path);
        font_src->path = NULL;
    }

    lv_free(font_src);
}

#endif /*LV_USE_TINY_TTF*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_FONT_MANAGER*/
