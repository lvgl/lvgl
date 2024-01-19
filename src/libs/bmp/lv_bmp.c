/**
 * @file lv_bmp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../../lvgl.h"
#if LV_USE_BMP

#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    lv_fs_file_t f;
    unsigned int px_offset;
    int px_width;
    int px_height;
    unsigned int bpp;
    int row_size_bytes;
} bmp_dsc_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header);
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc);

static lv_result_t decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area);

static void decoder_close(lv_image_decoder_t * dec, lv_image_decoder_dsc_t * dsc);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/
void lv_bmp_init(void)
{
    lv_image_decoder_t * dec = lv_image_decoder_create();
    lv_image_decoder_set_info_cb(dec, decoder_info);
    lv_image_decoder_set_open_cb(dec, decoder_open);
    lv_image_decoder_set_get_area_cb(dec, decoder_get_area);
    lv_image_decoder_set_close_cb(dec, decoder_close);
}

void lv_bmp_deinit(void)
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
 * Get info about a BMP image
 * @param src can be file name or pointer to a C array
 * @param header store the info here
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't get the info
 */
static lv_result_t decoder_info(lv_image_decoder_t * decoder, const void * src, lv_image_header_t * header)
{
    LV_UNUSED(decoder);

    lv_image_src_t src_type = lv_image_src_get_type(src);          /*Get the source type*/

    /*If it's a BMP file...*/
    if(src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = src;
        if(lv_strcmp(lv_fs_get_ext(fn), "bmp") == 0) {              /*Check the extension*/
            /*Save the data in the header*/
            lv_fs_file_t f;
            lv_fs_res_t res = lv_fs_open(&f, src, LV_FS_MODE_RD);
            if(res != LV_FS_RES_OK) return LV_RESULT_INVALID;
            uint8_t headers[54];

            lv_fs_read(&f, headers, 54, NULL);
            uint32_t w;
            uint32_t h;
            lv_memcpy(&w, headers + 18, 4);
            lv_memcpy(&h, headers + 22, 4);
            header->w = w;
            header->h = h;
            lv_fs_close(&f);

            uint16_t bpp;
            lv_memcpy(&bpp, headers + 28, 2);
            switch(bpp) {
                case 16:
                    header->cf = LV_COLOR_FORMAT_RGB565;
                    break;
                case 24:
                    header->cf = LV_COLOR_FORMAT_RGB888;
                    break;
                case 32:
                    header->cf = LV_COLOR_FORMAT_ARGB8888;
                    break;
                default:
                    LV_LOG_WARN("Not supported bpp: %d", bpp);
                    return LV_RESULT_OK;
            }
            return LV_RESULT_OK;
        }
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(src_type == LV_IMAGE_SRC_VARIABLE) {
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_INVALID;         /*If didn't succeeded earlier then it's an error*/
}

/**
 * Open a BMP image and return the decided image
 * @param decoder pointer to the decoder
 * @param dsc     pointer to the decoder descriptor
 * @return LV_RESULT_OK: no error; LV_RESULT_INVALID: can't open the image
 */
static lv_result_t decoder_open(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);

    /*If it's a BMP file...*/
    if(dsc->src_type == LV_IMAGE_SRC_FILE) {
        const char * fn = dsc->src;

        if(lv_strcmp(lv_fs_get_ext(fn), "bmp") != 0) {
            return LV_RESULT_INVALID;       /*Check the extension*/
        }

        bmp_dsc_t b;
        lv_memset(&b, 0x00, sizeof(b));

        lv_fs_res_t res = lv_fs_open(&b.f, dsc->src, LV_FS_MODE_RD);
        if(res == LV_RESULT_OK) return LV_RESULT_INVALID;

        uint8_t header[54];
        lv_fs_read(&b.f, header, 54, NULL);

        if(0x42 != header[0] || 0x4d != header[1]) {
            lv_fs_close(&b.f);
            return LV_RESULT_INVALID;
        }

        lv_memcpy(&b.px_offset, header + 10, 4);
        lv_memcpy(&b.px_width, header + 18, 4);
        lv_memcpy(&b.px_height, header + 22, 4);
        lv_memcpy(&b.bpp, header + 28, 2);
        b.row_size_bytes = ((b.bpp * b.px_width + 31) / 32) * 4;

        dsc->user_data = lv_malloc(sizeof(bmp_dsc_t));
        LV_ASSERT_MALLOC(dsc->user_data);
        if(dsc->user_data == NULL) return LV_RESULT_INVALID;
        lv_memcpy(dsc->user_data, &b, sizeof(b));
        return LV_RESULT_OK;
    }
    /* BMP file as data not supported for simplicity.
     * Convert them to LVGL compatible C arrays directly. */
    else if(dsc->src_type == LV_IMAGE_SRC_VARIABLE) {
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_INVALID;    /*If not returned earlier then it failed*/
}

static lv_result_t decoder_get_area(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc,
                                    const lv_area_t * full_area, lv_area_t * decoded_area)
{
    LV_UNUSED(decoder);
    bmp_dsc_t * b = dsc->user_data;
    lv_draw_buf_t * decoded = (void *)dsc->decoded;

    if(decoded_area->y1 == LV_COORD_MIN) {
        *decoded_area = *full_area;
        decoded_area->y2 = decoded_area->y1;
        if(decoded == NULL) decoded = lv_draw_buf_create(lv_area_get_width(full_area), 1, dsc->header.cf, LV_STRIDE_AUTO);
        dsc->decoded = decoded;
    }
    else {
        decoded_area->y1++;
        decoded_area->y2++;
    }

    if(decoded_area->y1 > full_area->y2) {
        return LV_RESULT_INVALID;
    }
    else {
        int32_t y = (b->px_height - 1) - (decoded_area->y1); /*BMP images are stored upside down*/
        uint32_t p = b->px_offset + b->row_size_bytes * y;
        p += (decoded_area->x1) * (b->bpp / 8);
        lv_fs_seek(&b->f, p, LV_FS_SEEK_SET);
        uint32_t line_width_byte = lv_area_get_width(full_area) * (b->bpp / 8);
        lv_fs_read(&b->f, decoded->data, line_width_byte, NULL);

        return LV_RESULT_OK;
    }
}

/**
 * Free the allocated resources
 */
static void decoder_close(lv_image_decoder_t * decoder, lv_image_decoder_dsc_t * dsc)
{
    LV_UNUSED(decoder);
    bmp_dsc_t * b = dsc->user_data;
    lv_fs_close(&b->f);
    lv_free(dsc->user_data);
    if(dsc->decoded) lv_draw_buf_destroy((void *)dsc->decoded);

}

#endif /*LV_USE_BMP*/
