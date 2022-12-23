/**
 * @file lv_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_img_src.h"
#include "lv_img_buf.h"
#include "../misc/lv_assert.h"
#include "../misc/lv_fs.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t alloc_str_src(lv_img_src_t * src, const char * str);
static lv_res_t alloc_arr_src(lv_img_src_t * src, lv_img_src_t ** srcs, const size_t len);
static void img_src_clear(lv_img_src_t * src);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/


lv_img_src_type_t lv_img_src_get_type(const void * src)
{
    lv_img_src_type_t img_src_type = LV_IMG_SRC_UNKNOWN;

    if(src == NULL) return img_src_type;
    const uint8_t * u8_p = src;

    /*The first byte shows the type of the image source*/
    if(u8_p[0] >= 0x20 && u8_p[0] <= 0x7F) {
        return LV_IMG_SRC_FILE; /*If it's an ASCII character then it's file name*/
    }
    else if(u8_p[0] >= 0x80) {
        return LV_IMG_SRC_SYMBOL; /*Symbols begins after 0x7F*/
    }
    return LV_IMG_SRC_VARIABLE; /*`lv_img_dsc_t` is draw to the first byte < 0x20*/
}


void lv_img_src_free(lv_img_src_t * src)
{
    img_src_clear(src);
    lv_free(src);
}


void lv_img_src_set_file(lv_img_src_t * src, const char * file_path)
{
    img_src_clear(src);

    src->type = LV_IMG_SRC_FILE;
    if(alloc_str_src(src, file_path) == LV_RES_INV)
        return;

    src->ext = lv_fs_get_ext(src->data);
}

void lv_img_src_set_data(lv_img_src_t * src, const uint8_t * data, const size_t len)
{
    img_src_clear(src);

    src->type = LV_IMG_SRC_VARIABLE;
    src->data = data;
    src->data_len = len;
}

void lv_img_src_set_raw(lv_img_src_t * src, const lv_img_dsc_t * data)
{
    img_src_clear(src);

    src->type = LV_IMG_SRC_VARIABLE;
    src->data = data;
    src->data_len = sizeof(*data);
}

void lv_img_src_set_symbol(lv_img_src_t * src, const char * symbol)
{
    img_src_clear(src);

    src->type = LV_IMG_SRC_SYMBOL;
    if(alloc_str_src(src, symbol) == LV_RES_INV)
        return;
}

void lv_img_src_set_srcs(lv_img_src_t * src, lv_img_src_t * srcs[], const size_t len, const size_t index)
{
    img_src_clear(src);
    src->type = LV_IMG_SRC_ARRAY;
    if(alloc_arr_src(src, srcs, len) == LV_RES_INV)
        return;
    src->valid_index = index;
}


lv_img_src_t * lv_img_src_from_symbol(const char * symbol)
{
    lv_img_src_t * img_src = lv_img_src_create();
    lv_img_src_set_symbol(img_src, symbol);
    return img_src;
}

lv_img_src_t * lv_img_src_from_data(const uint8_t * data, const size_t len)
{
    lv_img_src_t * img_src = lv_img_src_create();
    lv_img_src_set_data(img_src, data, len);
    return img_src;
}

lv_img_src_t * lv_img_src_from_file(const char * file_path)
{
    lv_img_src_t * img_src = lv_img_src_create();
    lv_img_src_set_file(img_src, file_path);
    return img_src;
}

lv_img_src_t * lv_img_src_from_raw(const lv_img_dsc_t * raw)
{
    lv_img_src_t * img_src = lv_img_src_create();
    lv_img_src_set_raw(img_src, raw);
    return img_src;
}

lv_img_src_t * lv_img_src_from_srcs(lv_img_src_t * srcs[], const size_t len)
{
    lv_img_src_t * img_src = lv_img_src_create();
    lv_img_src_set_srcs(img_src, srcs, len, 0);
    return img_src;
}

lv_img_src_t * lv_img_src_from_src(lv_img_src_t * src)
{
    lv_img_src_t * img_src = lv_img_src_create();
    lv_img_src_set_srcs(img_src, &src, 1, 0);
    img_src->valid_index = -1;
    return img_src;
}



lv_img_src_t * lv_img_src_parse(const void * raw)
{
    switch(lv_img_src_get_type(raw)) {
        case LV_IMG_SRC_FILE:
            return lv_img_src_from_file((const char *)raw);
        case LV_IMG_SRC_SYMBOL:
            return lv_img_src_from_symbol((const char *)raw);
        case LV_IMG_SRC_VARIABLE:
            return lv_img_src_from_raw((const lv_img_dsc_t *)raw);
        default:
            return NULL;
    }
}

lv_img_src_t * lv_img_src_create()
{
    lv_img_src_t * img_src = lv_malloc(sizeof(lv_img_src_t));
    if(img_src == NULL) {
        LV_ASSERT_MALLOC(img_src);
        return NULL;
    }
    lv_memset(img_src, 0, sizeof(img_src));
    img_src->type = LV_IMG_SRC_UNKNOWN;
    return img_src;
}

void lv_img_src_copy(lv_img_src_t * dest, const lv_img_src_t * src)
{
    img_src_clear(dest);
    dest->type = LV_IMG_SRC_UNKNOWN;
    dest->data = src->data;
    dest->data_len = src->data_len;
    dest->ext = NULL;
    if(src->type == LV_IMG_SRC_ARRAY) {
        if(alloc_arr_src(dest, (lv_img_src_t **)src->data, src->data_len) == LV_RES_INV) {
            return;
        }
        if(src->valid_index >= 0) {
            /*Only copy the array if not referenced*/
            for(size_t i = 0; i < src->data_len; i++)
                lv_img_src_copy(((lv_img_src_t **)dest->data)[i], ((lv_img_src_t **)src->data)[i]);
        }
        dest->valid_index = src->valid_index;
    }
    else if(src->type != LV_IMG_SRC_VARIABLE && alloc_str_src(dest, (const char *)src->data) == LV_RES_INV) {
        return;
    }
    dest->type = src->type;
    if(src->type == LV_IMG_SRC_FILE) {
        dest->ext = lv_fs_get_ext(dest->data);
    }
}

lv_img_src_t * lv_img_src_dup(const lv_img_src_t * src)
{
    lv_img_src_t * r = lv_img_src_create();
    if(r == NULL) return NULL;
    lv_img_src_copy(r, src);
    return r;
}


void lv_img_src_capture(lv_img_src_t ** dest, lv_img_src_t * src)
{
    if(*dest != NULL) {
        /*Check if we can reuse the destination to avoid reallocating*/
        if((*dest)->type == LV_IMG_SRC_ARRAY && (*dest)->valid_index < 0) {
            /*Capture a reference to the source in the first element of the array, that won't be freed*/
            *((lv_img_src_t **)(*dest)->data) = src;
            return;
        }
        else lv_img_src_free(*dest);
    }
    *dest = src;
}


static lv_res_t alloc_str_src(lv_img_src_t * src, const char * str)
{
    src->data_len = strlen(str);
    src->data = lv_malloc(src->data_len + 1);
    LV_ASSERT_MALLOC(src->data);
    if(src->data == NULL) {
        src->data_len = 0;
        return LV_RES_INV;
    }

    lv_memcpy((void *)src->data, str, src->data_len + 1);
    return LV_RES_OK;
}

static lv_res_t alloc_arr_src(lv_img_src_t * src, lv_img_src_t ** srcs, const size_t len)
{
    src->data_len = len;
    src->data = lv_malloc(src->data_len * sizeof(*srcs));
    LV_ASSERT_MALLOC(src->data);
    if(src->data == NULL) {
        src->data_len = 0;
        return LV_RES_INV;
    }

    lv_memcpy((void *)src->data, srcs, src->data_len * sizeof(*srcs));
    return LV_RES_OK;
}



static void img_src_clear(lv_img_src_t * src)
{
    if(src == NULL) return;

    if(src->type == LV_IMG_SRC_ARRAY && src->valid_index >= 0) {
        for(size_t i = 0; i < src->data_len; i++)
            lv_img_src_free(((lv_img_src_t **)src->data)[i]);
    }
    if(src->type == LV_IMG_SRC_SYMBOL || src->type == LV_IMG_SRC_FILE || src->type == LV_IMG_SRC_ARRAY) {
        lv_free((void *)src->data);
    }

    src->data_len = 0;
    src->ext = NULL;
    src->type = LV_IMG_SRC_UNKNOWN;
}

