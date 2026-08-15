/**
 * @file lv_qoi.c
 * @brief LVGL image decoder for the Quite OK Image (QOI) format.
 *
 * This decoder provides support for the QOI image format within LVGL.
 * It handles both file and variable sources, integrates with LVGL's image
 * cache system, and supports stride-aligned rendering for hardware display
 * pipelines. The implementation is endian-independent and compatible with
 * LVGL v9's image decoder framework.
 *
 * ## Key Features
 *
 * - **Dual Source Support**: Decodes QOI images from both file paths
 *   (via lv_fs) and in-memory variable sources (lv_image_dsc_t).
 * - **Cache Integration**: Fully integrates with LVGL's image cache
 *   subsystem to avoid redundant decoding of frequently used images.
 * - **Stride-Aligned Rendering**: Performs a row-by-row copy from the
 *   raw QOI pixel buffer into an LVGL draw buffer with configurable
 *   stride, ensuring compatibility with hardware display pipelines that
 *   require aligned row pitches.
 * - **Endian-Independent Parsing**: The QOI header signature and
 *   dimension fields are parsed byte-by-byte, making the decoder safe
 *   on both little-endian and big-endian architectures.
 * - **Bounds Checking**: All memory accesses are guarded by size
 *   validation against QOI_HEADER_SIZE, QOI_MIN_FILE_SIZE, and
 *   LV_QOI_MAX_FILE_SIZE to prevent buffer overruns.
 * - **Post-Processing Support**: Passes decoded buffers through
 *   lv_image_decoder_post_process() for color format conversion,
 *   premultiplied alpha handling, or other LVGL pipeline transforms.
 * - **Memory Safety**: Overrides QOI library memory hooks (QOI_MALLOC,
 *   QOI_FREE, QOI_ZEROARR) to use LVGL's allocator, ensuring all
 *   allocations are tracked within the LVGL memory pool.
 *
 * ## Usage
 *
 * Call lv_qoi_init() during LVGL initialization to register the decoder.
 * Once registered, LVGL will automatically use this decoder when loading
 * QOI images via lv_image_create() or lv_image_set_src().
 *
 * ## Dependencies
 *
 * - Requires the QOI reference library (qoi.h / qoi.c) bundled in the
 *   LVGL qoi integration.
 * - Requires LV_USE_QOI to be enabled in lv_conf.h.
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_image_decoder_private.h"
#include "../../lvgl.h"
#if LV_USE_QOI

#include "../libs/qoi/lv_qoi.h"
#include "../core/lv_global.h"
#include LV_LIMITS_INCLUDE

/* Exclude stdio functions from qoi.h as LVGL uses its own VFS (lv_fs) */
#define QOI_NO_STDIO

/* Override QOI memory & array hooks to safely use LVGL's allocator memory pool */
#define QOI_MALLOC(sz) lv_malloc(sz)
#define QOI_FREE(p)    lv_free(p)
#define QOI_ZEROARR(a) lv_memset((a), 0, sizeof(a))

/* Enforce a user-configurable pixel limit to prevent excessive memory
 * allocation from untrusted QOI headers. This is checked before QOI_MALLOC. */
#define QOI_PIXELS_LIMIT LV_QOI_MAX_PIXELS

#define QOI_IMPLEMENTATION
#include "../libs/qoi/qoi.h"

/*********************
 *      DEFINES
 *********************/

#define DECODER_NAME            "QOI"
#define QOI_RGBA_BPP            4U    /* 4 bytes per pixel for RGBA8888 */
#define QOI_MIN_FILE_SIZE       (QOI_HEADER_SIZE + 8) /* 14 header bytes + 8 end-marker padding bytes */

/* LV_QOI_MAX_FILE_SIZE is defined in lv_conf_internal.h */

#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t parse_qoi_header(const uint8_t * buf, size_t buf_size, lv_image_header_t * header);
static lv_draw_buf_t * decode_qoi_memory(const void * data, size_t data_size);
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static lv_draw_buf_t * decode_qoi_file(lv_image_decoder_dsc_t * dsc);
static uint8_t * read_file(lv_fs_file_t * file, uint32_t * size);

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * @brief Register the QOI decoder functions in LVGL.
 *
 * Sets up the QOI image decoder and registers it with the LVGL image decoding subsystem.
 */
void lv_qoi_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_close_cb(dec, decoder_close);

    dec->name = DECODER_NAME;
}

/**
 * @brief Deinitialize the QOI decoder and remove it from LVGL.
 *
 * Cleans up resources and unregisters the QOI image decoder from the LVGL image decoding subsystem.
 */
void lv_qoi_deinit(void)
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

/**
 * @brief Parse and validate QOI header bytes.
 *
 * @param  buf       pointer to the raw header buffer.
 * @param  buf_size  size of the buffer (must be at least QOI_HEADER_SIZE).
 * @param  header    pointer to the LVGL image header structure to fill (optional).
 * @return LV_RESULT_OK on success, LV_RESULT_INVALID on header validation failure.
 */
static lv_result_t parse_qoi_header(const uint8_t * buf, size_t buf_size, lv_image_header_t * header)
{
    if(buf == NULL || buf_size < QOI_HEADER_SIZE) {
        return LV_RESULT_INVALID;
    }

    /* Endian-independent QOI signature check "qoif" */
    if(buf[0] != 'q' || buf[1] != 'o' || buf[2] != 'i' || buf[3] != 'f') {
        return LV_RESULT_INVALID;
    }

    uint32_t w = ((uint32_t)buf[4] << 24) | ((uint32_t)buf[5] << 16) | ((uint32_t)buf[6] << 8) | (uint32_t)buf[7];
    uint32_t h = ((uint32_t)buf[8] << 24) | ((uint32_t)buf[9] << 16) | ((uint32_t)buf[10] << 8) | (uint32_t)buf[11];
    uint8_t channels = buf[12];
    uint8_t colorspace = buf[13];

    /* Validate header fields according to qoi.h specs */
    if(w == 0 || h == 0 || channels < 3 || channels > 4 || colorspace > 1) {
        return LV_RESULT_INVALID;
    }

    /* Validate against qoi.h QOI_PIXELS_MAX guard */
    if(h >= QOI_PIXELS_MAX / w) {
        return LV_RESULT_INVALID;
    }

    /* Enforce the user-configurable pixel limit to prevent excessive
     * memory allocation from untrusted image headers. */
    if(h >= LV_QOI_MAX_PIXELS / w) {
        LV_LOG_WARN("QOI image dimensions %" LV_PRIu32 "x%" LV_PRIu32
                    " exceed pixel limit %" LV_PRIu32,
                    w, h, (uint32_t)LV_QOI_MAX_PIXELS);
        return LV_RESULT_INVALID;
    }

    if(header != NULL) {
        header->w = w;
        header->h = h;
        header->cf = LV_COLOR_FORMAT_ARGB8888;
    }

    return LV_RESULT_OK;
}

/**
 * @brief Common QOI memory decoding and draw buffer creation helper.
 *
 * @param  data       pointer to raw QOI file bytes in memory.
 * @param  data_size  length of the data in bytes.
 * @return pointer to decoded lv_draw_buf_t, or NULL on error.
 */
static lv_draw_buf_t * decode_qoi_memory(const void * data, size_t data_size)
{
    if(data == NULL || data_size < QOI_MIN_FILE_SIZE || data_size > INT_MAX) {
        return NULL;
    }

    qoi_desc desc;
    void * pixels = qoi_decode(data, (int)data_size, &desc, QOI_RGBA_BPP);
    if(pixels == NULL) {
        LV_LOG_WARN("QOI decode failed");
        return NULL;
    }

    if(desc.width == 0 || desc.height == 0 || desc.width > (UINT32_MAX / QOI_RGBA_BPP) ||
       desc.height >= QOI_PIXELS_MAX / desc.width ||
       desc.height >= LV_QOI_MAX_PIXELS / desc.width) {
        LV_LOG_WARN("QOI image dimensions %" LV_PRIu32 "x%" LV_PRIu32
                    " exceed pixel limit %" LV_PRIu32,
                    desc.width, desc.height, (uint32_t)LV_QOI_MAX_PIXELS);
        QOI_FREE(pixels);
        return NULL;
    }

    lv_draw_buf_t * decoded = lv_draw_buf_create_ex(image_cache_draw_buf_handlers, desc.width, desc.height,
                                                    LV_COLOR_FORMAT_ARGB8888,
                                                    LV_STRIDE_AUTO);
    if(decoded == NULL) {
        LV_LOG_WARN("create draw buf failed");
        QOI_FREE(pixels);
        return NULL;
    }

    /* Safe stride row-by-row copy prevents alignment buffer corruption */
    uint32_t row_size = desc.width * QOI_RGBA_BPP;
    uint8_t * dst_row = (uint8_t *)decoded->data;
    uint8_t * src_row = (uint8_t *)pixels;
    uint32_t y;

    for(y = 0; y < desc.height; y++) {
        lv_memcpy(dst_row, src_row, row_size);
        dst_row += decoded->header.stride;
        src_row += row_size;
    }
    QOI_FREE(pixels);

    return decoded;
}

/**
 * @brief Get info about a QOI image.
 *
 * @param  decoder  pointer to the decoder object (unused).
 * @param  dsc      image descriptor containing the source and type of the image.
 * @param  header   store the header info here.
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info.
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc, lv_image_header_t * header)
{
    LV_UNUSED(decoder);
    if(dsc == NULL || header == NULL) return LV_RESULT_INVALID;

    lv_image_src_t src_type = dsc->src_type;

    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = dsc->src;
        if(img_dsc == NULL || img_dsc->data == NULL) return LV_RESULT_INVALID;

        /* Filter out uncompressed native image formats */
        if(img_dsc->header.cf != LV_COLOR_FORMAT_RAW &&
           img_dsc->header.cf != LV_COLOR_FORMAT_RAW_ALPHA &&
           img_dsc->header.cf != LV_COLOR_FORMAT_UNKNOWN) {
            return LV_RESULT_INVALID;
        }

        return parse_qoi_header(img_dsc->data, img_dsc->data_size, header);
    }
    else if(src_type == LV_IMAGE_SRC_FILE) {
        const char * src = dsc->src;
        if(src == NULL) return LV_RESULT_INVALID;

        uint8_t buf[QOI_HEADER_SIZE];
        uint32_t rn;

        /* The file is opened by the LVGL core (image_decoder_get_info) before
         * calling this callback, and closed after. Only rewind it here. */
        lv_fs_res_t res = lv_fs_read(&dsc->file, buf, sizeof(buf), &rn);

        /* Rewind the file so subsequent decoders or the open callback can read from the start */
        (void)lv_fs_seek(&dsc->file, 0, LV_FS_SEEK_SET);

        if(res != LV_FS_RES_OK || rn != sizeof(buf)) {
            return LV_RESULT_INVALID;
        }

        return parse_qoi_header(buf, rn, header);
    }

    return LV_RESULT_INVALID;
}

/**
 * @brief Open a QOI image and return the decoded image draw buffer.
 *
 * @param  decoder  pointer to the decoder object.
 * @param  dsc      pointer to the decoder descriptor.
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image.
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    if(dsc == NULL) return LV_RESULT_INVALID;

    lv_draw_buf_t * decoded = NULL;

    if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = dsc->src;
        if(img_dsc == NULL || img_dsc->data == NULL) {
            return LV_RESULT_INVALID;
        }

        decoded = decode_qoi_memory(img_dsc->data, img_dsc->data_size);
        if(decoded == NULL) {
            LV_LOG_WARN("decode qoi variable failed");
            return LV_RESULT_INVALID;
        }
    }
    else if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        decoded = decode_qoi_file(dsc);
        if(decoded == NULL) {
            return LV_RESULT_INVALID;
        }
    }
    else {
        return LV_RESULT_INVALID;
    }

    /* Post processing handling */
    lv_draw_buf_t * adjusted = lv_image_decoder_post_process(dsc, decoded);
    if(adjusted == NULL) {
        lv_draw_buf_destroy(decoded);
        return LV_RESULT_INVALID;
    }

    /* The adjusted draw buffer is newly allocated if it differs from decoded */
    if(adjusted != decoded) {
        lv_draw_buf_destroy(decoded);
        decoded = adjusted;
    }

    dsc->decoded = decoded;

    if(dsc->args.no_cache || !lv_image_cache_is_enabled()) {
        return LV_RESULT_OK;
    }

    /* Cache Processing Handling */
    lv_image_cache_data_t search_key;
    search_key.src_type = dsc->src_type;
    search_key.src = dsc->src;
    search_key.slot.size = decoded->data_size;

    lv_cache_entry_t * entry = lv_image_decoder_add_to_cache(decoder, &search_key, decoded, NULL);

    if(entry == NULL) {
        /* Uncached fallback: open succeeds, decoder_close will destroy decoded buffer */
        dsc->cache_entry = NULL;
        return LV_RESULT_OK;
    }

    dsc->cache_entry = entry;
    return LV_RESULT_OK;
}

/**
 * @brief Free resources allocated for a QOI decoding session.
 *
 * @param  decoder  pointer to the decoder object (unused).
 * @param  dsc      pointer to the decoder descriptor.
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    if(dsc == NULL || dsc->decoded == NULL) return;

    /* Free buffer directly only if it is NOT owned by the cache system */
    if(dsc->cache_entry == NULL) {
        lv_draw_buf_destroy((lv_draw_buf_t *)dsc->decoded);
    }

    dsc->decoded = NULL;
}

/**
 * @brief Read the entire contents of a file into a newly allocated buffer.
 *
 * @param  file  pointer to the LVGL file stream.
 * @param  size  pointer to store the size of the read buffer.
 * @return pointer to the allocated buffer, or NULL on failure.
 */
static uint8_t * read_file(lv_fs_file_t * file, uint32_t * size)
{
    uint8_t * data = NULL;
    uint32_t data_size = 0;
    uint32_t rn = 0;

    *size = 0;

    if(lv_fs_seek(file, 0, LV_FS_SEEK_END) != LV_FS_RES_OK) {
        return NULL;
    }

    if(lv_fs_tell(file, &data_size) != LV_FS_RES_OK) {
        (void)lv_fs_seek(file, 0, LV_FS_SEEK_SET);
        return NULL;
    }

    /* Enforce strict bounds on file size to protect against corrupted filesystems */
    if(data_size < QOI_MIN_FILE_SIZE || data_size > LV_QOI_MAX_FILE_SIZE) {
        LV_LOG_WARN("file size %" LV_PRIu32 " out of bounds (min: %u, max: %u)",
                    data_size, QOI_MIN_FILE_SIZE, (uint32_t)LV_QOI_MAX_FILE_SIZE);
        (void)lv_fs_seek(file, 0, LV_FS_SEEK_SET);
        return NULL;
    }

    if(lv_fs_seek(file, 0, LV_FS_SEEK_SET) != LV_FS_RES_OK) {
        return NULL;
    }

    data = QOI_MALLOC(data_size);
    if(data == NULL) {
        LV_LOG_WARN("malloc failed for %" LV_PRIu32 " bytes", data_size);
        return NULL;
    }

    lv_fs_res_t res = lv_fs_read(file, data, data_size, &rn);

    if(res == LV_FS_RES_OK && rn == data_size) {
        *size = rn;
    }
    else {
        LV_LOG_WARN("read file failed: read %" LV_PRIu32 " of %" LV_PRIu32 " bytes", rn, data_size);
        QOI_FREE(data);
        data = NULL;
        (void)lv_fs_seek(file, 0, LV_FS_SEEK_SET);
    }

    return data;
}

/**
 * @brief Decode a QOI file source into an LVGL draw buffer.
 *
 * @param  dsc  pointer to the decoder descriptor.
 * @return pointer to the decoded lv_draw_buf_t, or NULL on failure.
 */
static lv_draw_buf_t * decode_qoi_file(lv_image_decoder_dsc_t * dsc)
{
    const char * filename = dsc->src;
    uint32_t data_size;
    bool close_file = false;

    if(dsc->file.drv == NULL) {
        if(lv_fs_open(&dsc->file, filename, LV_FS_MODE_RD) != LV_FS_RES_OK) {
            LV_LOG_WARN("can't open %s", filename);
            return NULL;
        }
        close_file = true;
    }
    else {
        (void)lv_fs_seek(&dsc->file, 0, LV_FS_SEEK_SET);
    }

    uint8_t * data = read_file(&dsc->file, &data_size);

    if(close_file) {
        (void)lv_fs_close(&dsc->file);
    }
    else if(data == NULL) {
        /* Rewind on read error if caller owns the file descriptor handle */
        (void)lv_fs_seek(&dsc->file, 0, LV_FS_SEEK_SET);
    }

    if(data == NULL) {
        LV_LOG_WARN("can't load file: %s", filename);
        return NULL;
    }

    lv_draw_buf_t * decoded = decode_qoi_memory(data, data_size);
    QOI_FREE(data);

    return decoded;
}

#endif /* LV_USE_QOI */