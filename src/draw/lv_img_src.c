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

static lv_res_t alloc_str_src(lv_img_src_t * src, const char * str);

static const char * find_ext(const char * src);

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. LV_SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKNOWN
 * @deprecated You shouldn't rely on this function to find out the image type
 */
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

lv_res_t lv_img_src_parse(lv_img_src_t * obj, const void * src)
{
    lv_img_src_type_t src_type = lv_img_src_get_type(src);

    switch(src_type) {
        case LV_IMG_SRC_FILE:
#if LV_USE_LOG && LV_LOG_LEVEL >= LV_LOG_LEVEL_INFO
            LV_LOG_TRACE("lv_img_src_parse: `LV_IMG_SRC_FILE` type found");
#endif
            lv_img_src_set_file(obj, src);
            break;
        case LV_IMG_SRC_VARIABLE: {
#if LV_USE_LOG && LV_LOG_LEVEL >= LV_LOG_LEVEL_INFO
                LV_LOG_TRACE("lv_img_src_parse: `LV_IMG_SRC_VARIABLE` type found");
#endif
                lv_img_dsc_t * id = (lv_img_dsc_t *) src; /*This might break if given any raw data here*/
                lv_img_src_set_data(obj, (const uint8_t *)src, id->data_size);
            }
            break;
        case LV_IMG_SRC_SYMBOL:
#if LV_USE_LOG && LV_LOG_LEVEL >= LV_LOG_LEVEL_INFO
            LV_LOG_TRACE("lv_img_src_parse: `LV_IMG_SRC_SYMBOL` type found");
#endif
            lv_img_src_set_symbol(obj, src);
            break;
        default:
            LV_LOG_WARN("lv_img_src_parse: unknown image type");
            lv_img_src_free(obj);
            return LV_RES_INV;
    }
    return LV_RES_OK;
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

void lv_img_src_set_file(lv_img_src_t * obj, const char * file_path)
{
    lv_img_src_free(obj);

    obj->type = LV_IMG_SRC_FILE;
    if(alloc_str_src(obj, file_path) == LV_RES_INV)
        return;

    obj->ext = find_ext(obj->data); /* stm32 does not have strrchr */
}

void lv_img_src_set_data(lv_img_src_t * obj, const uint8_t * data, const size_t len)
{
    lv_img_src_free(obj);

    obj->type = LV_IMG_SRC_VARIABLE;
    obj->data = data;
    obj->data_len = len;
}

void lv_img_src_set_raw(lv_img_src_t * obj, const lv_img_dsc_t * data)
{
    lv_img_src_free(obj);

    obj->type = LV_IMG_SRC_VARIABLE;
    obj->data = data;
    obj->data_len = sizeof(*data);
}

void lv_img_src_set_symbol(lv_img_src_t * obj, const char * symbol)
{
    lv_img_src_free(obj);

    obj->type = LV_IMG_SRC_SYMBOL;
    if(alloc_str_src(obj, symbol) == LV_RES_INV)
        return;
}

lv_img_src_move_t lv_img_src_from_symbol(const char * symbol)
{
    lv_img_src_move_t obj;
    lv_img_src_set_symbol((lv_img_src_t *)&obj, symbol);
    obj._src.type |= _LV_IMG_SRC_MOVABLE;
    return obj;
}

lv_img_src_move_t lv_img_src_from_data(const uint8_t * data, const size_t len)
{
    lv_img_src_move_t obj;
    lv_img_src_set_data((lv_img_src_t *)&obj, data, len);
    obj._src.type |= _LV_IMG_SRC_MOVABLE;
    return obj;
}

lv_img_src_move_t lv_img_src_from_file(const char * file_path)
{
    lv_img_src_move_t obj;
    lv_img_src_set_file((lv_img_src_t *)&obj, file_path);
    obj._src.type |= _LV_IMG_SRC_MOVABLE;
    return obj;
}

lv_img_src_move_t lv_img_src_from_raw(const lv_img_dsc_t * raw)
{
    lv_img_src_move_t obj;
    lv_img_src_set_raw((lv_img_src_t *)&obj, raw);
    obj._src.type |= _LV_IMG_SRC_MOVABLE;
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
        dest->ext = find_ext(dest->data); /* stm32 does not have strrchr */
    }
}

void lv_img_src_capture(lv_img_src_t * dest, lv_img_src_move_t * src)
{
    /*Make sure the type is actually moveable */
    if(LV_BF(src->_src.type, _LV_IMG_SRC_MOVABLE)) {
        lv_img_src_copy(dest, (const lv_img_src_t *)src);
        return;
    }
    lv_img_src_free(dest);
    dest->type = src->_src.type & 0x7F; /*Remove moveable flag*/
    dest->data = src->_src.data;
    dest->data_len = src->_src.data_len;
    dest->ext = src->_src.ext;

    /*Finish moving the type by making it empty*/
    lv_memset(src, 0, sizeof(*src));
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

static const char * find_ext(const char * src)
{
    const char * r = src ? src + strlen(src) - 1 : NULL;
    while(r && r != src && *r != '.') --r;
    return r;
}
