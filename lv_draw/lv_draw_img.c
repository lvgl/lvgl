/**
 * @file lv_draw_img.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_draw_img.h"
#include "../lv_misc/lv_fs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * mask,
                                 const void * src, const lv_style_t * style, lv_opa_t opa_scale);

static const uint8_t * lv_img_decoder_open(const void * src, const lv_style_t * style);
static lv_res_t lv_img_decoder_read_line(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);
static void lv_img_decoder_close(void);
static lv_res_t lv_img_built_in_decoder_line_alpha(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);
static lv_res_t lv_img_built_in_decoder_line_indexed(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool decoder_custom;
static const void * decoder_src;
static lv_img_src_t decoder_src_type;
static lv_img_header_t decoder_header;
static const lv_style_t * decoder_style;
#if USE_LV_FILESYSTEM
static lv_fs_file_t decoder_file;
#endif
#if LV_IMG_CF_INDEXED
static lv_color_t decoder_index_map[256];
#endif

static lv_img_decoder_info_f_t lv_img_decoder_info_custom;
static lv_img_decoder_open_f_t lv_img_decoder_open_custom;
static lv_img_decoder_read_line_f_t lv_img_decoder_read_line_custom;
static lv_img_decoder_close_f_t lv_img_decoder_close_custom;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Draw an image
 * @param coords the coordinates of the image
 * @param mask the image will be drawn only in this area
 * @param src pointer to a lv_color_t array which contains the pixels of the image
 * @param style style of the image
 * @param opa_scale scale down all opacities by the factor
 */
void lv_draw_img(const lv_area_t * coords, const lv_area_t * mask,
                 const void * src, const lv_style_t * style, lv_opa_t opa_scale)
{
    if(src == NULL) {
        LV_LOG_WARN("Image draw: src is NULL");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL);
        return;
    }

    lv_res_t res;
    res = lv_img_draw_core(coords, mask, src, style, opa_scale);

    if(res ==  LV_RES_INV) {
        LV_LOG_WARN("Image draw error");
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL);
        return;
    }
}


/**
 *
 * @param src
 * @param header
 * @param style
 * @return
 */
lv_res_t lv_img_dsc_get_info(const char * src, lv_img_header_t * header)
{
    header->always_zero = 0;
    /*Try to get info with the custom functions first*/
    if(lv_img_decoder_info_custom) {
        lv_res_t custom_res;
        custom_res = lv_img_decoder_info_custom(src, header);
        if(custom_res == LV_RES_OK) return LV_RES_OK;       /*Custom info has supported this source*/
    }

    lv_img_src_t src_type = lv_img_src_get_type(src);
    if(src_type == LV_IMG_SRC_VARIABLE) {
        header->w = ((lv_img_dsc_t *)src)->header.w;
        header->h = ((lv_img_dsc_t *)src)->header.h;
        header->cf = ((lv_img_dsc_t *)src)->header.cf;
    }
#if USE_LV_FILESYSTEM
    else if(src_type == LV_IMG_SRC_FILE) {
        lv_fs_file_t file;
        lv_fs_res_t res;
        uint32_t rn;
        res = lv_fs_open(&file, src, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            res = lv_fs_read(&file, header, sizeof(lv_img_header_t), &rn);
        }

        /*Create a dummy header on fs error*/
        if(res != LV_FS_RES_OK || rn != sizeof(lv_img_header_t)) {
            header->w = LV_DPI;
            header->h = LV_DPI;
            header->cf = LV_IMG_CF_UNKOWN;
        }

        lv_fs_close(&file);
    }
#endif
    else if(src_type == LV_IMG_SRC_SYMBOL) {
        /*The size depend on the font but it is unknown here. It should be handled outside of the function*/
        header->w = 1;
        header->h = 1;
        /* Symbols always have transparent parts. Important because of cover check in the design function.
         * The actual value doesn't matter because lv_draw_label will draw it*/
        header->cf  = LV_IMG_CF_ALPHA_1BIT;
    } else {
        LV_LOG_WARN("Image get info found unknown src type");
        return false;
    }
    return true;

}

uint8_t lv_img_color_format_get_px_size(lv_img_cf_t cf)
{
    uint8_t px_size = 0;

    switch(cf) {
        case LV_IMG_CF_UNKOWN:
        case LV_IMG_CF_RAW:
            px_size = 0;
            break;
        case LV_IMG_CF_TRUE_COLOR:
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
            px_size = LV_COLOR_SIZE;
            break;
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
            px_size = LV_IMG_PX_SIZE_ALPHA_BYTE << 3;
            break;
        case LV_IMG_CF_INDEXED_1BIT:
        case LV_IMG_CF_ALPHA_1BIT:
            px_size = 1;
            break;
        case LV_IMG_CF_INDEXED_2BIT:
        case LV_IMG_CF_ALPHA_2BIT:
            px_size = 2;
            break;
        case LV_IMG_CF_INDEXED_4BIT:
        case LV_IMG_CF_ALPHA_4BIT:
            px_size = 4;
            break;
        case LV_IMG_CF_INDEXED_8BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            px_size = 8;
            break;
        default:
            px_size = 0;
            break;
    }

    return px_size;
}

bool lv_img_color_format_is_chroma_keyed(lv_img_cf_t cf)
{
    bool is_chroma_keyed = false;

    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED:
        case LV_IMG_CF_RAW_CHROMA_KEYED:
        case LV_IMG_CF_INDEXED_1BIT:
        case LV_IMG_CF_INDEXED_2BIT:
        case LV_IMG_CF_INDEXED_4BIT:
        case LV_IMG_CF_INDEXED_8BIT:
            is_chroma_keyed = true;
            break;
        default:
            is_chroma_keyed = false;
            break;
    }

    return is_chroma_keyed;
}


bool lv_img_color_format_has_alpha(lv_img_cf_t cf)
{
    bool has_alpha = false;

    switch(cf) {
        case LV_IMG_CF_TRUE_COLOR_ALPHA:
        case LV_IMG_CF_RAW_ALPHA:
        case LV_IMG_CF_ALPHA_1BIT:
        case LV_IMG_CF_ALPHA_2BIT:
        case LV_IMG_CF_ALPHA_4BIT:
        case LV_IMG_CF_ALPHA_8BIT:
            has_alpha = true;
            break;
        default:
            has_alpha = false;
            break;
    }

    return has_alpha;
}

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to a file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKOWN
 */
lv_img_src_t lv_img_src_get_type(const void * src)
{
    lv_img_src_t img_src_type = LV_IMG_SRC_UNKNOWN;

    if(src == NULL) return img_src_type;
    const uint8_t * u8_p = src;

    /*The first byte shows the type of the image source*/
    if(u8_p[0] >= 0x20 && u8_p[0] <= 0x7F) {
        img_src_type = LV_IMG_SRC_FILE;     /*If it's an ASCII character then it's file name*/
    } else if(u8_p[0] >= 0x80) {
        img_src_type = LV_IMG_SRC_SYMBOL;   /*Symbols begins after 0x7F*/
    } else {
        img_src_type = LV_IMG_SRC_VARIABLE; /*`lv_img_dsc_t` is design to the first byte < 0x20*/
    }

    if (LV_IMG_SRC_UNKNOWN == img_src_type) {
        LV_LOG_WARN("lv_img_src_get_type: unknown image type");
    }

    return img_src_type;
}

/**
 * Set custom decoder functions. See the typdefs of the function typed above for more info about them
 * @param info_fp info get function
 * @param open_fp open function
 * @param read_fp read line function
 * @param close_fp clode function
 */
void lv_img_decoder_set_custom(lv_img_decoder_info_f_t  info_fp, lv_img_decoder_open_f_t  open_fp,
                               lv_img_decoder_read_line_f_t read_fp, lv_img_decoder_close_f_t close_fp)
{
    lv_img_decoder_info_custom = info_fp;
    lv_img_decoder_open_custom = open_fp;
    lv_img_decoder_read_line_custom = read_fp;
    lv_img_decoder_close_custom = close_fp;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/


static lv_res_t lv_img_draw_core(const lv_area_t * coords, const lv_area_t * mask,
                                 const void * src, const lv_style_t * style, lv_opa_t opa_scale)
{

    lv_area_t mask_com;    /*Common area of mask and coords*/
    bool union_ok;
    union_ok = lv_area_intersect(&mask_com, mask, coords);
    if(union_ok == false) {
        return LV_RES_OK;         /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
    }

    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->image.opa : (uint16_t)((uint16_t) style->image.opa * opa_scale) >> 8;

    lv_img_header_t header;
    lv_res_t header_res;
    header_res = lv_img_dsc_get_info(src, &header);
    if(header_res != LV_RES_OK) {
        LV_LOG_WARN("Image draw can't get image info");
        lv_img_decoder_close();
        return LV_RES_INV;
    }

    bool chroma_keyed = lv_img_color_format_is_chroma_keyed(header.cf);
    bool alpha_byte = lv_img_color_format_has_alpha(header.cf);

    const uint8_t * img_data = lv_img_decoder_open(src, style);
    if(img_data == LV_IMG_DECODER_OPEN_FAIL) {
        LV_LOG_WARN("Image draw cannot open the image resource");
        lv_img_decoder_close();
        return LV_RES_INV;
    }

    /* The decoder open could open the image and gave the entire uncompressed image.
     * Just draw it!*/
    if(img_data) {
        map_fp(coords, mask, img_data, opa, chroma_keyed, alpha_byte, style->image.color, style->image.intense);
    }
    /* The whole uncompressed image is not available. Try to read it line-by-line*/
    else {
        lv_coord_t width = lv_area_get_width(&mask_com);

#if LV_COMPILER_VLA_SUPPORTED
        uint8_t buf[(lv_area_get_width(&mask_com) * ((LV_COLOR_DEPTH >> 3) + 1))];
#else
        uint8_t buf[LV_HOR_RES * ((LV_COLOR_DEPTH >> 3) + 1)];  /*+1 because of the possible alpha byte*/
#endif
        lv_area_t line;
        lv_area_copy(&line, &mask_com);
        lv_area_set_height(&line, 1);
        lv_coord_t x = mask_com.x1 - coords->x1;
        lv_coord_t y = mask_com.y1 - coords->y1;
        lv_coord_t row;
        lv_res_t read_res;
        for(row = mask_com.y1; row <= mask_com.y2; row++) {
            read_res = lv_img_decoder_read_line(x, y, width, buf);
            if(read_res != LV_RES_OK) {
                lv_img_decoder_close();
                LV_LOG_WARN("Image draw can't read the line");
                return LV_RES_INV;
            }
            map_fp(&line, mask, buf, opa, chroma_keyed, alpha_byte, style->image.color, style->image.intense);
            line.y1++;
            line.y2++;
            y++;
        }
    }

    lv_img_decoder_close();

    return LV_RES_OK;
}


static const uint8_t * lv_img_decoder_open(const void * src, const lv_style_t * style)
{
    decoder_custom = false;

    /*Try to open with the custom functions first*/
    if(lv_img_decoder_open_custom) {
        const uint8_t * custom_res;
        custom_res = lv_img_decoder_open_custom(src, style);
        if(custom_res != LV_IMG_DECODER_OPEN_FAIL) {
            decoder_custom = true;  /*Mark that custom decoder function should be used for this img source.*/
            return custom_res;      /*Custom open supported this source*/
        }
    }

    decoder_src = src;
    decoder_style = style;
    decoder_src_type = lv_img_src_get_type(src);

    lv_res_t header_res;
    header_res = lv_img_dsc_get_info(src, &decoder_header);
    if(header_res == LV_RES_INV) {
        decoder_src = NULL;
        decoder_src_type = LV_IMG_SRC_UNKNOWN;
        LV_LOG_WARN("Built-in image decoder can't get the header info");
        return LV_IMG_DECODER_OPEN_FAIL;
    }

    /*Open the file if it's a file*/
    if(decoder_src_type == LV_IMG_SRC_FILE) {
#if USE_LV_FILESYSTEM
        lv_fs_res_t res = lv_fs_open(&decoder_file, src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            LV_LOG_WARN("Built-in image decoder can't open the file");
            return LV_IMG_DECODER_OPEN_FAIL;
        }
#else
        LV_LOG_WARN("Image built-in decoder can read file because USE_LV_FILESYSTEM = 0");
        return LV_IMG_DECODER_OPEN_FAIL;
#endif
    }


    /*Process the different color formats*/
    lv_img_cf_t cf = decoder_header.cf;
    if(cf == LV_IMG_CF_TRUE_COLOR ||
            cf == LV_IMG_CF_TRUE_COLOR_ALPHA ||
            cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
        if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
            /*In case of uncompressed formats if the image stored in the ROM/RAM simply give it's pointer*/
            return ((lv_img_dsc_t *)decoder_src)->data;
        } else {
            /*If it's file it need to be read line by line later*/
            return NULL;
        }
    } else if(cf == LV_IMG_CF_INDEXED_1BIT ||
              cf == LV_IMG_CF_INDEXED_2BIT ||
              cf == LV_IMG_CF_INDEXED_4BIT ||
              cf == LV_IMG_CF_INDEXED_8BIT) {

#if LV_IMG_CF_INDEXED
#if USE_LV_FILESYSTEM
        lv_color32_t palette_file[256];
#endif

        lv_color32_t * palette_p = NULL;
        uint8_t px_size = lv_img_color_format_get_px_size(cf);
        uint32_t palette_size = 1 << px_size;

        if(decoder_src_type == LV_IMG_SRC_FILE) {
            /*Read the palette from file*/
#if USE_LV_FILESYSTEM
            lv_fs_seek(&decoder_file, 4);   /*Skip the header*/
            lv_fs_read(&decoder_file, palette_file, palette_size * sizeof(lv_color32_t), NULL);
            palette_p = palette_file;
#else
            LV_LOG_WARN("Image built-in decoder can read the palette because USE_LV_FILESYSTEM = 0");
            return LV_IMG_DECODER_OPEN_FAIL;
#endif
        } else {
            /*The palette begins in the beginning of the image data. Just point to it.*/
            palette_p = (lv_color32_t *)((lv_img_dsc_t *)decoder_src)->data;
        }

        uint32_t i;
        for(i = 0; i < palette_size; i++) {
            decoder_index_map[i] = LV_COLOR_MAKE(palette_p[i].red, palette_p[i].green, palette_p[i].blue);
        }
        return NULL;
#else
        LV_LOG_WARN("Indexed (palette) images are not enabled in lv_conf.h. See LV_IMG_CF_INDEXED");
        return LV_IMG_DECODER_OPEN_FAIL;
#endif
    } else if(cf == LV_IMG_CF_ALPHA_1BIT ||
              cf == LV_IMG_CF_ALPHA_2BIT ||
              cf == LV_IMG_CF_ALPHA_4BIT ||
              cf == LV_IMG_CF_ALPHA_8BIT) {
#if LV_IMG_CF_ALPHA
        return NULL;   /*Nothing to process*/
#else
        LV_LOG_WARN("Alpha indexed images are not enabled in lv_conf.h. See LV_IMG_CF_ALPHA");
        return LV_IMG_DECODER_OPEN_FAIL;
#endif
    } else {
        LV_LOG_WARN("Image decoder open: unknown color format")
        return LV_IMG_DECODER_OPEN_FAIL;
    }
}


static lv_res_t lv_img_decoder_read_line(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    /*Try to read the line with the custom functions*/
    if(decoder_custom) {
        if(lv_img_decoder_read_line_custom) {
            lv_res_t custom_res;
            custom_res = lv_img_decoder_read_line_custom(x, y, len, buf);
            return custom_res;
        } else {
            LV_LOG_WARN("Image open with custom decoder but read not supported")
        }
        return LV_RES_INV;  /*It"s an error if not returned earlier*/
    }

    if(decoder_src_type == LV_IMG_SRC_FILE) {
#if USE_LV_FILESYSTEM
        uint8_t px_size = lv_img_color_format_get_px_size(decoder_header.cf);

        lv_fs_res_t res;

        if(decoder_header.cf == LV_IMG_CF_TRUE_COLOR ||
                decoder_header.cf == LV_IMG_CF_TRUE_COLOR_ALPHA ||
                decoder_header.cf == LV_IMG_CF_TRUE_COLOR_CHROMA_KEYED) {
            uint32_t pos = ((y * decoder_header.w + x) * px_size) >> 3;
            pos += 4;    /*Skip the header*/
            res = lv_fs_seek(&decoder_file, pos);
            if(res != LV_FS_RES_OK) {
                LV_LOG_WARN("Built-in image decoder seek failed");
                return false;
            }
            uint32_t btr = len * (px_size >> 3);
            uint32_t br = 0;
            lv_fs_read(&decoder_file, buf, btr, &br);
            if(res != LV_FS_RES_OK || btr != br) {
                LV_LOG_WARN("Built-in image decoder read failed");
                return false;
            }
        } else if(decoder_header.cf == LV_IMG_CF_ALPHA_1BIT ||
                  decoder_header.cf == LV_IMG_CF_ALPHA_2BIT ||
                  decoder_header.cf == LV_IMG_CF_ALPHA_4BIT ||
                  decoder_header.cf == LV_IMG_CF_ALPHA_8BIT) {

            lv_img_built_in_decoder_line_alpha(x, y, len, buf);
        } else if(decoder_header.cf == LV_IMG_CF_INDEXED_1BIT ||
                  decoder_header.cf == LV_IMG_CF_INDEXED_2BIT ||
                  decoder_header.cf == LV_IMG_CF_INDEXED_4BIT ||
                  decoder_header.cf == LV_IMG_CF_INDEXED_8BIT) {
            lv_img_built_in_decoder_line_indexed(x, y, len, buf);
        } else {
            LV_LOG_WARN("Built-in image decoder read not supports the color format");
            return false;
        }
#else
        LV_LOG_WARN("Image built-in decoder can't read file because USE_LV_FILESYSTEM = 0");
        return false;
#endif
    } else if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = decoder_src;

        if(img_dsc->header.cf == LV_IMG_CF_ALPHA_1BIT ||
                img_dsc->header.cf == LV_IMG_CF_ALPHA_2BIT ||
                img_dsc->header.cf == LV_IMG_CF_ALPHA_4BIT ||
                img_dsc->header.cf == LV_IMG_CF_ALPHA_8BIT) {
            lv_img_built_in_decoder_line_alpha(x, y, len, buf);
        } else if(img_dsc->header.cf == LV_IMG_CF_INDEXED_1BIT ||
                  img_dsc->header.cf == LV_IMG_CF_INDEXED_2BIT ||
                  img_dsc->header.cf == LV_IMG_CF_INDEXED_4BIT ||
                  img_dsc->header.cf == LV_IMG_CF_INDEXED_8BIT) {
            lv_img_built_in_decoder_line_indexed(x, y, len, buf);
        } else {
            LV_LOG_WARN("Built-in image decoder not supports the color format");
            return false;
        }
    }

    return true;
}

static void lv_img_decoder_close(void)
{
    /*Try to close with the custom functions*/
    if(decoder_custom) {
        if(lv_img_decoder_close_custom) lv_img_decoder_close_custom();
        return;
    }

    /*It was opened with built-in decoder*/
    if(decoder_src) {
#if USE_LV_FILESYSTEM
        if(decoder_src_type == LV_IMG_SRC_FILE) {
            lv_fs_close(&decoder_file);
        }
#endif
        decoder_src_type = LV_IMG_SRC_UNKNOWN;
        decoder_src = NULL;
    }
}

static lv_res_t lv_img_built_in_decoder_line_alpha(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{

#if LV_IMG_CF_ALPHA
    const lv_opa_t alpha1_opa_table[2] =  {0, 255};                   /*Opacity mapping with bpp = 1 (Just for compatibility)*/
    const lv_opa_t alpha2_opa_table[4] =  {0, 85, 170, 255};          /*Opacity mapping with bpp = 2*/
    const lv_opa_t alpha4_opa_table[16] = {0,   17,  34,  51,         /*Opacity mapping with bpp = 4*/
                                           68,  85,  102, 119,
                                           136, 153, 170, 187,
                                           204, 221, 238, 255
                                          };

    /*Simply fill the buffer with the color. Later only the alpha value will be modified.*/
    lv_color_t bg_color = decoder_style->image.color;
    lv_coord_t i;
    for(i = 0; i < len; i++) {
#if LV_COLOR_DEPTH == 8 || LV_COLOR_DEPTH == 1
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full;
#elif LV_COLOR_DEPTH == 16
        /*Because of Alpha byte 16 bit color can start on odd address which can cause crash*/
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full & 0xFF;
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + 1] = (bg_color.full >> 8) & 0xFF;
#elif LV_COLOR_DEPTH == 32
        *((uint32_t *)&buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE]) = bg_color.full;
#else
#error "Invalid LV_COLOR_DEPTH. Check it in lv_conf.h"
#endif
    }

    const lv_opa_t * opa_table = NULL;
    uint8_t px_size = lv_img_color_format_get_px_size(decoder_header.cf);
    uint16_t mask = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t w = 0;
    uint32_t ofs = 0;
    int8_t pos = 0;
    switch(decoder_header.cf) {
        case LV_IMG_CF_ALPHA_1BIT:
            w = (decoder_header.w >> 3);        /*E.g. w = 20 -> w = 2 + 1*/
            if(decoder_header.w & 0x7) w++;
            ofs += w * y + (x >> 3);      /*First pixel*/
            pos = 7 - (x & 0x7);
            opa_table = alpha1_opa_table;
            break;
        case LV_IMG_CF_ALPHA_2BIT:
            w = (decoder_header.w >> 2);       /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            if(decoder_header.w & 0x3) w++;
            ofs += w * y + (x >> 2);      /*First pixel*/
            pos = 6 - ((x & 0x3) * 2);
            opa_table = alpha2_opa_table;
            break;
        case LV_IMG_CF_ALPHA_4BIT:
            w = (decoder_header.w >> 1);       /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            if(decoder_header.w & 0x1) w++;
            ofs += w * y + (x >> 1);      /*First pixel*/
            pos = 4 - ((x & 0x1) * 4);
            opa_table = alpha4_opa_table;
            break;
        case LV_IMG_CF_ALPHA_8BIT:
            w = decoder_header.w;              /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;      /*First pixel*/
            pos = 0;
            break;
    }

#if USE_LV_FILESYSTEM
# if LV_COMPILER_VLA_SUPPORTED
    uint8_t fs_buf[w];
# else
    uint8_t fs_buf[LV_HOR_RES];
# endif
#endif
    const uint8_t * data_tmp = NULL;
    if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = decoder_src;
        data_tmp = img_dsc->data + ofs;
    } else {
#if USE_LV_FILESYSTEM
        lv_fs_seek(&decoder_file, ofs + 4);     /*+4 to skip the header*/
        lv_fs_read(&decoder_file, fs_buf, w, NULL);
        data_tmp = fs_buf;
#else
        LV_LOG_WARN("Image built-in alpha line reader can't read file because USE_LV_FILESYSTEM = 0");
        data_tmp = NULL;        /*To avoid warnings*/
        return LV_RES_INV;
#endif
    }


    uint8_t byte_act = 0;
    uint8_t val_act;
    for(i = 0; i < len; i ++) {
        val_act = (data_tmp[byte_act] & (mask << pos)) >> pos;

        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + LV_IMG_PX_SIZE_ALPHA_BYTE - 1] =
            decoder_header.cf == LV_IMG_CF_ALPHA_8BIT ? val_act : opa_table[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }

    return LV_RES_OK;

#else
    LV_LOG_WARN("Image built-in alpha line reader failed because LV_IMG_CF_ALPHA is 0 in lv_conf.h");
    return LV_RES_INV;
#endif
}

static lv_res_t lv_img_built_in_decoder_line_indexed(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{

#if LV_IMG_CF_INDEXED
    uint8_t px_size = lv_img_color_format_get_px_size(decoder_header.cf);
    uint16_t mask = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

    lv_coord_t w = 0;
    int8_t pos = 0;
    uint32_t ofs = 0;
    switch(decoder_header.cf) {
        case LV_IMG_CF_INDEXED_1BIT:
            w = (decoder_header.w >> 3);       /*E.g. w = 20 -> w = 2 + 1*/
            if(decoder_header.w & 0x7) w++;
            ofs += w * y + (x >> 3);      /*First pixel*/
            ofs += 8;                    /*Skip the palette*/
            pos = 7 - (x & 0x7);
            break;
        case LV_IMG_CF_INDEXED_2BIT:
            w = (decoder_header.w >> 2);       /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            if(decoder_header.w & 0x3) w++;
            ofs += w * y + (x >> 2);      /*First pixel*/
            ofs += 16;                    /*Skip the palette*/
            pos = 6 - ((x & 0x3) * 2);
            break;
        case LV_IMG_CF_INDEXED_4BIT:
            w = (decoder_header.w >> 1);       /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            if(decoder_header.w & 0x1) w++;
            ofs += w * y + (x >> 1);      /*First pixel*/
            ofs += 64;                    /*Skip the palette*/
            pos = 4 - ((x & 0x1) * 4);
            break;
        case LV_IMG_CF_INDEXED_8BIT:
            w = decoder_header.w;              /*E.g. x = 7 -> w = 7 (bytes)*/
            ofs += w * y + x;              /*First pixel*/
            ofs += 1024;                    /*Skip the palette*/
            pos = 0;
            break;
    }

#if USE_LV_FILESYSTEM
# if LV_COMPILER_VLA_SUPPORTED
    uint8_t fs_buf[w];
# else
    uint8_t fs_buf[LV_HOR_RES];
# endif
#endif
    const uint8_t * data_tmp = NULL;
    if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = decoder_src;
        data_tmp = img_dsc->data + ofs;
    } else {
#if USE_LV_FILESYSTEM
        lv_fs_seek(&decoder_file, ofs + 4);     /*+4 to skip the header*/
        lv_fs_read(&decoder_file, fs_buf, w, NULL);
        data_tmp = fs_buf;
#else
        LV_LOG_WARN("Image built-in indexed line reader can't read file because USE_LV_FILESYSTEM = 0");
        data_tmp = NULL;        /*To avoid warnings*/
        return LV_RES_INV;
#endif
    }

    uint8_t byte_act = 0;
    uint8_t val_act;
    lv_coord_t i;
    lv_color_t * cbuf = (lv_color_t *) buf;
    for(i = 0; i < len; i ++) {
        val_act = (data_tmp[byte_act] & (mask << pos)) >> pos;
        cbuf[i] = decoder_index_map[val_act];

        pos -= px_size;
        if(pos < 0) {
            pos = 8 - px_size;
            data_tmp++;
        }
    }

    return LV_RES_OK;
#else
    LV_LOG_WARN("Image built-in indexed line reader failed because LV_IMG_CF_INDEXED is 0 in lv_conf.h");
    return LV_RES_INV;
#endif
}
