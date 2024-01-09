/**
 * @file lv_image_decoder.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_decoder.h"
#include "../misc/lv_assert.h"
#include "../draw/lv_draw_image.h"
#include "../misc/lv_ll.h"
#include "../stdlib/lv_string.h"
#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/
#define img_decoder_ll_p &(LV_GLOBAL_DEFAULT()->img_decoder_ll)
#define img_cache_p (LV_GLOBAL_DEFAULT()->img_cache)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t img_width_to_stride(lv_image_header_t * header);

#if LV_CACHE_DEF_SIZE > 0
static lv_cache_compare_res_t image_decoder_cache_compare_cb(const lv_image_cache_data_t * lhs,
                                                             const lv_image_cache_data_t * rhs);
static void image_decoder_cache_free_cb(lv_image_cache_data_t * entry, void * user_data);

static lv_result_t try_cache(lv_image_decoder_dsc_t * dsc);
#endif
/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize the image decoder module
 */
void _lv_image_decoder_init(void)
{
    _lv_ll_init(img_decoder_ll_p, sizeof(lv_image_decoder_t));

#if LV_CACHE_DEF_SIZE > 0
    img_cache_p = lv_cache_create(&lv_cache_class_lru_rb_size,
    sizeof(lv_image_cache_data_t), LV_CACHE_DEF_SIZE, (lv_cache_ops_t) {
        .compare_cb = (lv_cache_compare_cb_t)image_decoder_cache_compare_cb,
        .create_cb = NULL,
        .free_cb = (lv_cache_free_cb_t)image_decoder_cache_free_cb,
    });
#endif
}

/**
 * Deinitialize the image decoder module
 */
void _lv_image_decoder_deinit(void)
{
#if LV_CACHE_DEF_SIZE > 0
    lv_cache_destroy(img_cache_p, NULL);
#endif
    _lv_ll_clear(img_decoder_ll_p);
}

lv_result_t lv_image_decoder_get_info(const void * src, lv_image_header_t * header)
{
    lv_memzero(header, sizeof(lv_image_header_t));

    if(src == NULL) return LV_RESULT_INVALID;

    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RESULT_INVALID;
    }

    lv_result_t res = LV_RESULT_INVALID;
    lv_image_decoder_t * decoder;
    _LV_LL_READ(img_decoder_ll_p, decoder) {
        if(decoder->info_cb) {
            res = decoder->info_cb(decoder, src, header);
            if(res == LV_RESULT_OK) {
                if(header->stride == 0) header->stride = img_width_to_stride(header);
                break;
            }
        }
    }

    return res;
}

lv_result_t lv_image_decoder_open(lv_image_decoder_dsc_t * dsc, const void * src, const lv_image_decoder_args_t * args)
{
    lv_memzero(dsc, sizeof(lv_image_decoder_dsc_t));

    if(src == NULL) return LV_RESULT_INVALID;
    lv_image_src_t src_type = lv_image_src_get_type(src);
    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        if(img_dsc->data == NULL) return LV_RESULT_INVALID;
    }

    dsc->src_type = src_type;

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        size_t fnlen = lv_strlen(src);
        dsc->src = lv_malloc(fnlen + 1);
        LV_ASSERT_MALLOC(dsc->src);
        if(dsc->src == NULL) {
            LV_LOG_WARN("Out of memory");
            return LV_RESULT_INVALID;
        }
        lv_strcpy((char *)dsc->src, src);
    }
    else {
        dsc->src = src;
    }

    lv_result_t res = LV_RESULT_INVALID;

    lv_image_decoder_t * decoder;
    lv_image_decoder_args_t * args_copy = NULL;

    static const lv_image_decoder_args_t def_args = {
        .stride_align = LV_DRAW_BUF_STRIDE_ALIGN != 1,
        .premultiply = false,
        .no_cache = false,
        .use_indexed = false,
    };

    /*Make a copy of args */
    dsc->args = args ? *args : def_args;

    _LV_LL_READ(img_decoder_ll_p, decoder) {
        /*Info and Open callbacks are required*/
        if(decoder->info_cb == NULL || decoder->open_cb == NULL) continue;

        res = decoder->info_cb(decoder, src, &dsc->header);
        if(res != LV_RESULT_OK) continue;

        if(dsc->header.stride == 0) dsc->header.stride = img_width_to_stride(&dsc->header);

        dsc->decoder = decoder;

#if LV_CACHE_DEF_SIZE > 0
        dsc->cache = img_cache_p;

        /*Check the cache first*/
        if(try_cache(dsc) == LV_RESULT_OK) return LV_RESULT_OK;
#endif

        res = decoder->open_cb(decoder, dsc, args);

        /*Opened successfully. It is a good decoder for this image source*/
        if(res == LV_RESULT_OK) return res;

        /*Prepare for the next loop*/
        lv_memzero(&dsc->header, sizeof(lv_image_header_t));

        dsc->error_msg = NULL;
        dsc->decoded  = NULL;
        dsc->cache_entry = NULL;
        dsc->user_data = NULL;
        dsc->time_to_open = 0;
    }

    if(dsc->src_type == LV_IMAGE_SRC_FILE)
        lv_free((void *)dsc->src);

    if(args_copy) lv_free(args_copy);

    return res;
}

lv_result_t lv_image_decoder_get_area(lv_image_decoder_dsc_t * dsc, const lv_area_t * full_area,
                                      lv_area_t * decoded_area)
{
    lv_result_t res = LV_RESULT_INVALID;
    if(dsc->decoder->get_area_cb) res = dsc->decoder->get_area_cb(dsc->decoder, dsc, full_area, decoded_area);

    return res;
}

void lv_image_decoder_close(lv_image_decoder_dsc_t * dsc)
{
    if(dsc->decoder) {
        if(dsc->decoder->close_cb) dsc->decoder->close_cb(dsc->decoder, dsc);

        if(dsc->src_type == LV_IMAGE_SRC_FILE) {
            lv_free((void *)dsc->src);
            dsc->src = NULL;
        }
    }
}

/**
 * Create a new image decoder
 * @return pointer to the new image decoder
 */
lv_image_decoder_t * lv_image_decoder_create(void)
{
    lv_image_decoder_t * decoder;
    decoder = _lv_ll_ins_head(img_decoder_ll_p);
    LV_ASSERT_MALLOC(decoder);
    if(decoder == NULL) return NULL;

    lv_memzero(decoder, sizeof(lv_image_decoder_t));

    return decoder;
}

void lv_image_decoder_delete(lv_image_decoder_t * decoder)
{
    _lv_ll_remove(img_decoder_ll_p, decoder);
    lv_free(decoder);
}

lv_image_decoder_t * lv_image_decoder_get_next(lv_image_decoder_t * decoder)
{
    if(decoder == NULL)
        return _lv_ll_get_head(img_decoder_ll_p);
    else
        return _lv_ll_get_next(img_decoder_ll_p, decoder);
}

void lv_image_decoder_set_info_cb(lv_image_decoder_t * decoder, lv_image_decoder_info_f_t info_cb)
{
    decoder->info_cb = info_cb;
}

void lv_image_decoder_set_open_cb(lv_image_decoder_t * decoder, lv_image_decoder_open_f_t open_cb)
{
    decoder->open_cb = open_cb;
}

void lv_image_decoder_set_get_area_cb(lv_image_decoder_t * decoder, lv_image_decoder_get_area_cb_t get_area_cb)
{
    decoder->get_area_cb = get_area_cb;
}

void lv_image_decoder_set_close_cb(lv_image_decoder_t * decoder, lv_image_decoder_close_f_t close_cb)
{
    decoder->close_cb = close_cb;
}

void lv_image_decoder_set_cache_free_cb(lv_image_decoder_t * decoder, lv_cache_free_cb_t cache_free_cb)
{
    decoder->cache_free_cb = cache_free_cb;
}

#if LV_CACHE_DEF_SIZE > 0
lv_cache_entry_t * lv_image_decoder_add_to_cache(lv_image_decoder_t * decoder,
                                                 lv_image_cache_data_t * search_key,
                                                 const lv_draw_buf_t * decoded, void * user_data)
{
    lv_cache_entry_t * cache_entry = lv_cache_add(img_cache_p, search_key, NULL);
    if(cache_entry == NULL) {
        return NULL;
    }

    lv_image_cache_data_t * cached_data;
    cached_data = lv_cache_entry_get_data(cache_entry);

    /*Set the cache entry to decoder data*/
    cached_data->decoded = decoded;
    if(cached_data->src_type == LV_IMAGE_SRC_FILE) {
        cached_data->src = lv_strdup(cached_data->src);
    }
    cached_data->user_data = user_data; /*Need to free data on cache invalidate instead of decoder_close*/
    cached_data->decoder = decoder;

    return cache_entry;
}
#endif

lv_draw_buf_t * lv_image_decoder_post_process(lv_image_decoder_dsc_t * dsc, lv_draw_buf_t * decoded)
{
    if(decoded == NULL) return NULL; /*No need to adjust*/

    lv_image_decoder_args_t * args = &dsc->args;
    if(args->stride_align && decoded->header.cf != LV_COLOR_FORMAT_RGB565A8) {
        uint32_t stride_expect = lv_draw_buf_width_to_stride(decoded->header.w, decoded->header.cf);
        if(decoded->header.stride != stride_expect) {
            LV_LOG_TRACE("Stride mismatch");
            lv_draw_buf_t * aligned = lv_draw_buf_adjust_stride(decoded, stride_expect);
            if(aligned == NULL) {
                LV_LOG_ERROR("No memory for Stride adjust.");
                return NULL;
            }

            decoded = aligned;
        }
    }

    /*Premultiply alpha channel*/
    if(args->premultiply
       && !lv_draw_buf_has_flag(decoded, LV_IMAGE_FLAGS_PREMULTIPLIED) /*Hasn't done yet*/
      ) {
        LV_LOG_TRACE("Alpha premultiply.");
        if(lv_draw_buf_has_flag(decoded, LV_IMAGE_FLAGS_MODIFIABLE)) {
            /*Do it directly*/
            lv_draw_buf_premultiply(decoded);
        }
        else {
            decoded = lv_draw_buf_dup(decoded);
            if(decoded == NULL) {
                LV_LOG_ERROR("No memory for premulitplying.");
                return NULL;
            }

            lv_draw_buf_premultiply(decoded);
        }
    }

    return decoded;
}

static uint32_t img_width_to_stride(lv_image_header_t * header)
{
    if(header->cf == LV_COLOR_FORMAT_RGB565A8) {
        return header->w * 2;
    }
    else {
        return ((uint32_t)header->w * lv_color_format_get_bpp(header->cf) + 7) >> 3;
    }
}

#if LV_CACHE_DEF_SIZE > 0
static lv_cache_compare_res_t image_decoder_cache_compare_cb(
    const lv_image_cache_data_t * lhs,
    const lv_image_cache_data_t * rhs)
{
    if(lhs->src_type == rhs->src_type) {
        if(lhs->src_type == LV_IMAGE_SRC_FILE) {
            int32_t cmp_res = lv_strcmp(lhs->src, rhs->src);
            if(cmp_res != 0) {
                return cmp_res > 0 ? 1 : -1;
            }
        }
        else if(lhs->src_type == LV_IMAGE_SRC_VARIABLE) {
            if(lhs->src != rhs->src) {
                return lhs->src > rhs->src ? 1 : -1;
            }
        }
        return 0;
    }
    return lhs->src_type > rhs->src_type ? 1 : -1;
}

static void image_decoder_cache_free_cb(lv_image_cache_data_t * entry, void * user_data)
{
    LV_UNUSED(user_data); /*Unused*/

    const lv_image_decoder_t * decoder = entry->decoder;
    if(decoder && decoder->cache_free_cb) {
        decoder->cache_free_cb(entry, user_data);
    }
}

static lv_result_t try_cache(lv_image_decoder_dsc_t * dsc)
{
    lv_cache_t * cache = dsc->cache;

    lv_image_cache_data_t search_key;
    search_key.src_type = dsc->src_type;
    search_key.src = dsc->src;

    lv_cache_entry_t * entry = lv_cache_acquire(cache, &search_key, NULL);

    if(entry) {
        lv_image_cache_data_t * cached_data = lv_cache_entry_get_data(entry);
        dsc->decoded = cached_data->decoded;
        dsc->cache_entry = entry;     /*Save the cache to release it in decoder_close*/
        return LV_RESULT_OK;
    }

    return LV_RESULT_INVALID;
}
#endif
