/**
 * @file lv_etc2.c
 *
 */
/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"

#if LV_USE_ETC2

/*********************
 *      DEFINES
 *********************/
#define ETC2_RGB_NO_MIPMAPS             1
#define ETC2_RGBA_NO_MIPMAPS            3
#define ETC2_PKM_HEADER_SIZE            16
#define ETC2_PKM_FORMAT_OFFSET          6
#define ETC2_PKM_ENCODED_WIDTH_OFFSET   8
#define ETC2_PKM_ENCODED_HEIGHT_OFFSET  10
#define ETC2_PKM_WIDTH_OFFSET           12
#define ETC2_PKM_HEIGHT_OFFSET          14

#define ETC2_EAC_FORMAT_CODE            3

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

static uint16_t read_big_endian_uint16(const uint8_t * buf);
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_draw_buf_t * decode_etc2_file(const char * filename, lv_image_decoder_dsc_t * dsc);
static void etc2_cache_free_cb(lv_image_cache_data_t * cached_data, void * user_data);

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_etc2_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);
    lv_image_decoder_set_cache_free_cb(dec, (lv_cache_free_cb_t)etc2_cache_free_cb);
}

void lv_etc2_deinit(void)
{
    lv_image_decoder_t * dec = NULL;
    while((dec = lv_image_decoder_get_next(dec)) != NULL) {
        if(dec->info_cb == decoder_info) {
            lv_image_decoder_delete(dec);
            break;
        }
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint16_t read_big_endian_uint16(const uint8_t * buf)
{
    return (buf[0] << 8) | buf[1];
}

static lv_res_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    LV_UNUSED(decoder); /*Unused*/

    lv_image_src_t src_type = lv_image_src_get_type(src);

    if(src_type == LV_IMAGE_SRC_FILE) {
        const char * ext = lv_fs_get_ext(src);
        if(!(lv_strcmp(ext, "pkm") == 0)) {
            return LV_RES_INV;
        }

        lv_fs_file_t f;
        lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            LV_LOG_WARN("open %s failed", (const char *)src);
            return LV_RES_INV;
        }

        uint32_t rn;
        uint8_t pkm_header[ETC2_PKM_HEADER_SIZE];
        static const char pkm_magic[] = { 'P', 'K', 'M', ' ', '2', '0' };

        res = lv_fs_read(&f, pkm_header, ETC2_PKM_HEADER_SIZE, &rn);
        lv_fs_close(&f);

        if(res != LV_FS_RES_OK || rn != ETC2_PKM_HEADER_SIZE) {
            LV_LOG_WARN("Image get info read file magic number failed");
            return LV_RES_INV;
        }

        if(memcmp(pkm_header, pkm_magic, sizeof(pkm_magic)) != 0) {
            LV_LOG_WARN("Image get info magic number invalid");
            return LV_RES_INV;
        }

        uint16_t pkm_format = read_big_endian_uint16(pkm_header + ETC2_PKM_FORMAT_OFFSET);
        if(pkm_format != ETC2_EAC_FORMAT_CODE) {
            LV_LOG_WARN("Image header format invalid : %d", pkm_format);
            return LV_RES_INV;
        }

        header->cf = LV_COLOR_FORMAT_ETC2_EAC;
        header->w = read_big_endian_uint16(pkm_header + ETC2_PKM_WIDTH_OFFSET);
        header->h = read_big_endian_uint16(pkm_header + ETC2_PKM_HEIGHT_OFFSET);
        header->stride = read_big_endian_uint16(pkm_header + ETC2_PKM_ENCODED_WIDTH_OFFSET);

        return LV_RES_OK;
    }

    return LV_RES_INV;
}

static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;
        lv_draw_buf_t * decoded = decode_etc2_file(fn, dsc);
        if(decoded == NULL) {
            return LV_RESULT_INVALID;
        }

        dsc->decoded = decoded;

#if LV_CACHE_DEF_SIZE > 0
        lv_image_cache_data_t search_key;
        search_key.src_type = dsc->src_type;
        search_key.src = dsc->src;
        search_key.slot.size = decoded->data_size;

        lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, decoded, NULL);

        if(entry == NULL) {
            lv_draw_buf_destroy(decoded);
            return LV_RESULT_INVALID;
        }
        dsc->cache_entry = entry;
#endif
        return LV_RESULT_OK;
    }

    return LV_RES_INV;
}

static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder); /*Unused*/

#if LV_CACHE_DEF_SIZE > 0
    lv_cache_release(dsc->cache, dsc->cache_entry, NULL);
#else
    lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
#endif
}

static lv_draw_buf_t * decode_etc2_file(const char * filename, lv_image_decoder_dsc_t * dsc)
{
    lv_image_header_t * header = &dsc->header;
    lv_draw_buf_t * decoded;

    decoded = lv_draw_buf_create(header->w, header->h, header->cf, header->stride);

    if(decoded == NULL) {
        LV_LOG_ERROR("etc2 draw buff alloc %zu failed: %s", (size_t)(header->stride * header->h), filename);
        return NULL;
    }

    lv_fs_file_t f;
    lv_fs_res_t res = lv_fs_open(&f, filename, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("etc2 decoder open %s failed", (const char *)filename);
        goto failed;
    }

    res = lv_fs_seek(&f, ETC2_PKM_HEADER_SIZE, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("etc2 file seek %s failed", (const char *)filename);
        lv_fs_close(&f);
        goto failed;
    }

    uint32_t rn;
    res = lv_fs_read(&f, decoded->data, decoded->data_size, &rn);
    lv_fs_close(&f);

    if(res != LV_FS_RES_OK || rn != decoded->data_size) {
        LV_LOG_WARN("etc2 read data failed, size:%zu", (size_t)decoded->data_size);
        goto failed;
    }

    return decoded;

failed:
    lv_draw_buf_destroy(decoded);
    return NULL;
}

static void etc2_cache_free_cb(lv_image_cache_data_t * cached_data, void * user_data)
{
    LV_UNUSED(user_data);

    if(cached_data->src_type == LV_IMAGE_SRC_FILE) lv_free((void *)cached_data->src);
    lv_draw_buf_destroy((lv_draw_buf_t *)cached_data->decoded);
}
#endif /*LV_USE_ETC2*/
