/**
 * @file lv_tjpgd.c
 *
 */

/*----------------------------------------------------------------------------------------------------------------------------------
/    Added normal JPG support [7/10/2020]
/    ----------
/    SJPEG is a custom created modified JPEG file format for small embedded platforms.
/    It will contain multiple JPEG fragments all embedded into a single file with a custom header.
/    This makes JPEG decoding easier using any JPEG library. Overall file size will be almost
/    similar to the parent jpeg file. We can generate sjpeg from any jpeg using a python script
/    provided along with this project.
/                                                                                     (by vinodstanur | 2020 )
/    SJPEG FILE STRUCTURE
/    --------------------------------------------------------------------------------------------------------------------------------
/    Bytes                       |   Value                                                                                           |
/    --------------------------------------------------------------------------------------------------------------------------------
/
/    0 - 7                       |   "_SJPG__" followed by '\0'
/
/    8 - 13                      |   "V1.00" followed by '\0'       [VERSION OF SJPG FILE for future compatibiliby]
/
/    14 - 15                     |   X_RESOLUTION (width)            [little endian]
/
/    16 - 17                     |   Y_RESOLUTION (height)           [little endian]
/
/    18 - 19                     |   TOTAL_FRAMES inside sjpeg       [little endian]
/
/    20 - 21                     |   JPEG BLOCK WIDTH (16 normally)  [little endian]
/
/    22 - [(TOTAL_FRAMES*2 )]    |   SIZE OF EACH JPEG SPLIT FRAGMENTS   (FRAME_INFO_ARRAY)
/
/   SJPEG data                   |   Each JPEG frame can be extracted from SJPEG data by parsing the FRAME_INFO_ARRAY one time.
/
/----------------------------------------------------------------------------------------------------------------------------------
/                   JPEG DECODER
/                   ------------
/   We are using TJpgDec - Tiny JPEG Decompressor library from ELM-CHAN for decoding each split-jpeg fragments.
/   The tjpgd.c and tjpgd.h is not modified and those are used as it is. So if any update comes for the tiny-jpeg,
/   just replace those files with updated files.
/---------------------------------------------------------------------------------------------------------------------------------*/

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl.h"
#if LV_USE_TJPGD

#include "tjpgd.h"
#include "lv_tjpgd.h"
#include "../../misc/lv_fs.h"

/*********************
 *      DEFINES
 *********************/
#define TJPGD_WORKBUFF_SIZE             4096    //Recommended by TJPGD library

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

static lv_result_t decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area);
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);
static size_t input_func(JDEC * jd, uint8_t * buff, size_t ndata);
static int is_jpg(const uint8_t * raw_data, size_t len);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_tjpgd_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_get_area_cb(dec, decoder_get_area);
    lv_image_decoder_set_close_cb(dec, decoder_close);
}

void lv_tjpgd_deinit(void)
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

static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    LV_UNUSED(decoder);

    lv_image_src_t src_type = lv_image_src_get_type(src);

    if(src_type == LV_IMAGE_SRC_VARIABLE) {
        const lv_image_dsc_t * img_dsc = src;
        uint8_t * raw_data = (uint8_t *)img_dsc->data;
        const uint32_t raw_sjpeg_data_size = img_dsc->data_size;

        if(is_jpg(raw_data, raw_sjpeg_data_size) == true) {
#if LV_USE_FS_MEMFS
            header->always_zero = 0;
            header->cf = LV_COLOR_FORMAT_RAW;
            header->w = img_dsc->header.w;
            header->h = img_dsc->header.h;
            header->stride = img_dsc->header.w * 3;
            return LV_RESULT_OK;
#else
            LV_LOG_WARN("LV_USE_FS_MEMFS needs to enabled to decode from data");
            return LV_RESULT_INVALID;
#endif
        }
    }
    else if(src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = src;
        if((strcmp(lv_fs_get_ext(fn), "jpg") == 0) || (strcmp(lv_fs_get_ext(fn), "jpeg") == 0)) {
            lv_fs_file_t f;
            lv_fs_res_t res;
            res = lv_fs_open(&f, fn, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) return LV_RESULT_INVALID;

            uint8_t workb[TJPGD_WORKBUFF_SIZE];
            JDEC jd;
            JRESULT rc = jd_prepare(&jd, input_func, workb, TJPGD_WORKBUFF_SIZE, &f);
            if(rc) {
                LV_LOG_WARN("jd_prepare error: %d", rc);
                lv_fs_close(&f);
                return LV_RESULT_INVALID;
            }
            header->always_zero = 0;
            header->cf = LV_COLOR_FORMAT_RAW;
            header->w = jd.width;
            header->h = jd.height;
            header->stride = jd.width * 3;

            lv_fs_close(&f);
            return LV_RESULT_OK;
        }
    }
    return LV_RESULT_INVALID;
}

static size_t input_func(JDEC * jd, uint8_t * buff, size_t ndata)
{
    lv_fs_file_t * f = jd->device;
    if(!f) return 0;

    if(buff) {
        uint32_t rn = 0;
        lv_fs_read(f, buff, (uint32_t)ndata, &rn);
        return rn;
    }
    else {
        uint32_t pos;
        lv_fs_tell(f, &pos);
        lv_fs_seek(f, (uint32_t)(ndata + pos),  LV_FS_SEEK_SET);
        return ndata;
    }
    return 0;
}

static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    lv_fs_file_t * f = lv_malloc(sizeof(lv_fs_file_t));
    if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
#if LV_USE_FS_MEMFS
        const lv_image_dsc_t * img_dsc = dsc->src;
        if(is_jpg(img_dsc->data, img_dsc->data_size) == true) {
            lv_fs_path_ex_t path;
            lv_fs_make_path_from_buffer(&path, LV_FS_MEMFS_LETTER, img_dsc->data, img_dsc->data_size);
            lv_fs_res_t res;
            res = lv_fs_open(f, (const char *)&path, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) {
                lv_free(f);
                return LV_RESULT_INVALID;
            }
        }
#else
        LV_LOG_WARN("LV_USE_FS_MEMFS needs to enabled to decode from data");
        return LV_RESULT_INVALID;
#endif
    }
    else if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;
        if((strcmp(lv_fs_get_ext(fn), "jpg") == 0) || (strcmp(lv_fs_get_ext(fn), "jpeg") == 0)) {
            lv_fs_res_t res;
            res = lv_fs_open(f, fn, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) {
                lv_free(f);
                return LV_RESULT_INVALID;
            }
        }
    }

    uint8_t * workb_temp = lv_malloc(TJPGD_WORKBUFF_SIZE);
    JDEC * jd = lv_malloc(sizeof(JDEC));
    dsc->user_data = jd;
    JRESULT rc = jd_prepare(jd, input_func, workb_temp, (size_t)TJPGD_WORKBUFF_SIZE, f);
    if(rc) return rc;

    dsc->header.always_zero = 0;
    dsc->header.cf = LV_COLOR_FORMAT_RGB888;
    dsc->header.w = jd->width;
    dsc->header.h = jd->height;
    dsc->header.stride = jd->width * 3;

    if(rc != JDR_OK) {
        lv_free(workb_temp);
        lv_free(jd);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

static lv_result_t decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area)
{
    LV_UNUSED(decoder);
    LV_UNUSED(full_area);

    JDEC * jd = dsc->user_data;

    uint32_t  mx, my;
    mx = jd->msx * 8;
    my = jd->msy * 8;         /* Size of the MCU (pixel) */
    if(decoded_area->y1 == LV_COORD_MIN) {
        decoded_area->y1 = 0;
        decoded_area->y2 = 7;
        decoded_area->x1 = 0 - mx;
        decoded_area->x2 = 7 - mx;
        jd->scale = 0;
        jd->dcv[2] = jd->dcv[1] = jd->dcv[0] = 0;   /* Initialize DC values */
        dsc->img_data = jd->workbuf;
        jd->rst = 0;
        jd->rsc = 0;
        dsc->header.stride = mx * 3;
    }

    decoded_area->x1 += mx;
    decoded_area->x2 += mx;

    if(decoded_area->x1 >= jd->width) {
        decoded_area->x1 = 0;
        decoded_area->x2 = 7;
        decoded_area->y1 += my;
        decoded_area->y2 += my;
    }

    JRESULT rc;
    if(jd->nrst && jd->rst++ == jd->nrst) {     /* Process restart interval if enabled */
        rc = jd_restart(jd, jd->rsc++);
        if(rc != JDR_OK) return rc;
        jd->rst = 1;
    }
    rc = jd_mcu_load(jd);                  /* Load an MCU (decompress huffman coded stream, dequantize and apply IDCT) */
    if(rc != JDR_OK) return rc;
    rc = jd_mcu_output(jd, NULL, decoded_area->x1,
                       decoded_area->y1); /* Output the MCU (YCbCr to RGB, scaling and output) */
    if(rc != JDR_OK) return rc;

    return LV_RESULT_OK;
}

/**
 * Free the allocated resources
 * @param decoder pointer to the decoder where this function belongs
 * @param dsc pointer to a descriptor which describes this decoding session
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    JDEC * jd = dsc->user_data;
    lv_fs_close(jd->device);
    lv_free(jd->device);
    lv_free(jd->pool_original);
    lv_free(jd);
}

static int is_jpg(const uint8_t * raw_data, size_t len)
{
    const uint8_t jpg_signature[] = {0xFF, 0xD8, 0xFF,  0xE0,  0x00,  0x10, 0x4A,  0x46, 0x49, 0x46};
    if(len < sizeof(jpg_signature)) return false;
    return memcmp(jpg_signature, raw_data, sizeof(jpg_signature)) == 0;
}

#endif /*LV_USE_TJPGD*/
