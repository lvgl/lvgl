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

/** Free a source descriptor.
 *  Only to be called if allocated via lv_img_src_parse
 *  @param src  The src format to free
 */
void lv_img_src_free(lv_img_src_t * src)
{
    if(src->type == LV_IMG_SRC_SYMBOL || src->type == LV_IMG_SRC_FILE) {
        lv_mem_free((void *)src->data);
    }
    lv_memset_00(src, sizeof(*src));
}

void lv_img_src_set_file(lv_img_src_t * src, const char * file_path)
{
    lv_img_src_free(src);

    src->type = LV_IMG_SRC_FILE;
    if(alloc_str_src(src, file_path) == LV_RES_INV)
        return;


    src->ext = lv_fs_get_ext(src->data);
}

void lv_img_src_set_data(lv_img_src_t * src, const uint8_t * data, const size_t len)
{
    lv_img_src_free(src);

    src->type = LV_IMG_SRC_VARIABLE;
    src->data = data;
    src->data_len = len;
}

void lv_img_src_set_raw(lv_img_src_t * src, const lv_img_dsc_t * data)
{
    lv_img_src_free(src);

    src->type = LV_IMG_SRC_VARIABLE;
    src->data = data;
    src->data_len = sizeof(*data);
}

void lv_img_src_set_symbol(lv_img_src_t * src, const char * symbol)
{
    lv_img_src_free(src);

    src->type = LV_IMG_SRC_SYMBOL;
    if(alloc_str_src(src, symbol) == LV_RES_INV)
        return;
}

lv_img_src_t lv_img_src_from_symbol(const char * symbol)
{
    lv_img_src_t obj = lv_img_src_empty();
    lv_img_src_set_symbol((lv_img_src_t *)&obj, symbol);
    obj.type |= _LV_IMG_SRC_MOVABLE;
    return obj;
}

lv_img_src_t lv_img_src_from_data(const uint8_t * data, const size_t len)
{
    lv_img_src_t obj = lv_img_src_empty();
    lv_img_src_set_data((lv_img_src_t *)&obj, data, len);
    obj.type |= _LV_IMG_SRC_MOVABLE;
    return obj;
}

lv_img_src_t lv_img_src_from_file(const char * file_path)
{
    lv_img_src_t obj = lv_img_src_empty();
    lv_img_src_set_file((lv_img_src_t *)&obj, file_path);
    obj.type |= _LV_IMG_SRC_MOVABLE;
    return obj;
}

lv_img_src_t lv_img_src_from_raw(const lv_img_dsc_t * raw)
{
    lv_img_src_t obj = lv_img_src_empty();
    lv_img_src_set_raw((lv_img_src_t *)&obj, raw);
    obj.type |= _LV_IMG_SRC_MOVABLE;
    return obj;
}

lv_img_src_t lv_img_src_empty(void)
{
    lv_img_src_t obj;
    lv_memset(&obj, 0, sizeof(obj));
    obj.type = LV_IMG_SRC_UNKNOWN | _LV_IMG_SRC_MOVABLE;
    return obj;
}

void lv_img_src_copy(lv_img_src_t * dest, const lv_img_src_t * src)
{
    lv_img_src_free(dest);
    dest->type = LV_IMG_SRC_UNKNOWN;
    dest->data = src->data;
    dest->data_len = src->data_len;
    dest->ext = NULL;
    if(src->type != LV_IMG_SRC_VARIABLE && alloc_str_src(dest, (const char *)src->data) == LV_RES_INV) {
        return;
    }
    dest->type = src->type;
    if(src->type == LV_IMG_SRC_FILE) {
        dest->ext = lv_fs_get_ext(dest->data);
    }
}

void lv_img_src_capture(lv_img_src_t * dest, lv_img_src_t * src)
{
    /*Non-moveable objects should be copied*/
    if(LV_BF(src->type, _LV_IMG_SRC_MOVABLE)) {
        lv_img_src_copy(dest, (const lv_img_src_t *)src);
    }
    /*Move movable objects. `src` will be invalid after it.*/
    else {
        lv_img_src_free(dest);
        dest->type = src->type & 0x7F; /*Remove moveable flag*/
        dest->data = src->data;
        dest->data_len = src->data_len;
        dest->ext = src->ext;

        /*Finish moving the type by making it empty*/
        lv_memset(src, 0, sizeof(*src));
    }
}


static lv_res_t alloc_str_src(lv_img_src_t * src, const char * str)
{
    src->data_len = strlen(str);
    src->data = lv_mem_alloc(src->data_len + 1);
    LV_ASSERT_MALLOC(src->data);
    if(src->data == NULL) {
        src->data_len = 0;
        return LV_RES_INV;
    }

    lv_memcpy((void *)src->data, str, src->data_len + 1);
    return LV_RES_OK;
}

