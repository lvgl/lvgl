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

static const uint8_t * lv_img_decoder_open(const void * src);
static lv_res_t lv_img_decoder_read_line(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);
static void lv_img_decoder_close(void);
static lv_res_t lv_img_built_in_decoder_line_alpha(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);

/**********************
 *  STATIC VARIABLES
 **********************/
static const void * decoder_src;
static lv_img_src_t decoder_src_type;
static lv_img_header_t decoder_header;
static lv_fs_file_t decoder_file;

lv_res_t (*lv_img_dsc_get_info_custom)(const char * src, lv_img_header_t * header);
const uint8_t * (*lv_img_decoder_open_custom)(const char * src);
lv_res_t (*lv_img_decoder_read_line_custom)(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf);
lv_res_t (*lv_img_decoder_close_custom)(void);

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
    if(lv_img_dsc_get_info_custom) {
        lv_res_t custom_res;
        custom_res = lv_img_dsc_get_info_custom(src, header);
        if(custom_res == LV_RES_OK) return LV_RES_OK;       /*Custom info supported this source*/
    }

    lv_img_src_t src_type = lv_img_src_get_type(src);
    if(src_type == LV_IMG_SRC_VARIABLE) {
        header->color_format =
                header->w = ((lv_img_dsc_t *)src)->header.w;
        header->h = ((lv_img_dsc_t *)src)->header.h;
        header->color_format = ((lv_img_dsc_t *)src)->header.color_format;
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
            header->color_format = LV_IMG_FORMAT_UNKOWN;
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
        header->color_format  = LV_IMG_FORMAT_ALPHA_1BIT;
    } else {
        LV_LOG_WARN("Image get info found unknown src type");
        return false;
    }
    return true;

}

uint8_t lv_img_color_format_get_px_size(lv_img_color_format_t cf)
{
	switch(cf) {
		case LV_IMG_FORMAT_UNKOWN:
		case LV_IMG_FORMAT_RAW:
			return 0;
		case LV_IMG_FORMAT_TRUE_COLOR:
		case LV_IMG_FORMAT_TRUE_COLOR_CHROMA_KEYED:
			return LV_COLOR_SIZE;
		case LV_IMG_FORMAT_TRUE_COLOR_ALPHA:
#if LV_COLOR_DEPTH != 24
			return LV_COLOR_SIZE;
#else
			return LV_COLOR_SIZE + 1;
#endif
		case LV_IMG_FORMAT_INDEXED_1BIT:
		case LV_IMG_FORMAT_ALPHA_1BIT:
			return 1;

		case LV_IMG_FORMAT_INDEXED_2BIT:
		case LV_IMG_FORMAT_ALPHA_2BIT:
			return 2;

		case LV_IMG_FORMAT_INDEXED_4BIT:
		case LV_IMG_FORMAT_ALPHA_4BIT:
			return 4;

		case LV_IMG_FORMAT_INDEXED_8BIT:
		case LV_IMG_FORMAT_ALPHA_8BIT:
			return 8;

		default:
			return 0;
	}

	return 0;
}

bool lv_img_color_format_is_chroma_keyed(lv_img_color_format_t cf)
{
	switch(cf) {
		case LV_IMG_FORMAT_TRUE_COLOR_CHROMA_KEYED:
		case LV_IMG_FORMAT_INDEXED_1BIT:
		case LV_IMG_FORMAT_INDEXED_2BIT:
		case LV_IMG_FORMAT_INDEXED_4BIT:
		case LV_IMG_FORMAT_INDEXED_8BIT:
			return true;
		default:
			return false;
	}

	return false;
}


bool lv_img_color_format_has_alpha(lv_img_color_format_t cf)
{
	switch(cf) {
		case LV_IMG_FORMAT_TRUE_COLOR_ALPHA:
		case LV_IMG_FORMAT_ALPHA_1BIT:
		case LV_IMG_FORMAT_ALPHA_2BIT:
		case LV_IMG_FORMAT_ALPHA_4BIT:
		case LV_IMG_FORMAT_ALPHA_8BIT:
			return true;
		default:
			return false;
	}

	return false;
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
    if(src == NULL) return LV_IMG_SRC_UNKNOWN;
    const uint8_t * u8_p = src;

    /*The first byte shows the type of the image source*/
    if(u8_p[0] >= 0x20 && u8_p[0] <= 0x7F) return LV_IMG_SRC_FILE;    /*If it's an ASCII character then it's file name*/
    else if(u8_p[0] >= 0x80) return LV_IMG_SRC_SYMBOL;                /*Symbols begins after 0x7F*/
    else return LV_IMG_SRC_VARIABLE;      							  /*`lv_img_dsc_t` is design to the first byte < 0x20*/

    return LV_IMG_SRC_UNKNOWN;
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

    bool chroma_keyed = lv_img_color_format_is_chroma_keyed(header.color_format);
    bool alpha_byte = lv_img_color_format_has_alpha(header.color_format);

    const uint8_t * img_data = lv_img_decoder_open(src);
    if(img_data == LV_IMG_DECODER_OPEN_FAIL) {
        LV_LOG_WARN("Image draw cannot open the image resource");
        lv_img_decoder_close();
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
        uint8_t buf[(lv_area_get_width(&mask_com) * (LV_COLOR_SIZE + 1))];
#else
# if LV_HOR_RES > LV_VER_RES
        uint8_t buf[LV_HOR_RES * ((LV_COLOR_DEPTH >> 8) + 1)];  /*+1 because of the possible alpha byte*/
# else
        uint8_t buf[LV_VER_RES * ((LV_COLOR_DEPTH >> 8) + 1)];
# endif
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


static const uint8_t * lv_img_decoder_open(const void * src)
{
    /*Try to open with the custom functions first*/
    if(lv_img_decoder_open_custom) {
        const uint8_t * custom_res;
        custom_res = lv_img_decoder_open_custom(src);
        if(custom_res != LV_IMG_DECODER_OPEN_FAIL) return custom_res;   /*Custom open supported this source*/
    }

    decoder_src = src;
    lv_img_src_t decoder_src_type = lv_img_src_get_type(src);

    /*It will be a path to a file*/
    if(decoder_src_type == LV_IMG_SRC_FILE) {
#if USE_LV_FILESYSTEM
        lv_fs_res_t res = lv_fs_open(&decoder_file, src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK) {
            decoder_src = NULL;
            decoder_src_type = LV_IMG_SRC_UNKNOWN;
            LV_LOG_WARN("Built-in image decoder can't open the src");
            return NULL;
        }

        lv_res_t header_res;
        header_res = lv_img_dsc_get_info(src, &decoder_header);
        if(header_res == LV_RES_INV) {
            lv_fs_close(&decoder_file);
            decoder_src = NULL;
            decoder_src_type = LV_IMG_SRC_UNKNOWN;
            LV_LOG_WARN("Built-in image decoder can't get the header info");
            return NULL;
        }
#endif
    }
    /*It will be a variable in the RAM/ROM*/
    else if(decoder_src_type == LV_IMG_SRC_VARIABLE) {
        const lv_img_dsc_t * img_dsc = src;
        switch(img_dsc->header.color_format) {
            /*In case of uncompressed formats return the whole array*/
            case LV_IMG_FORMAT_TRUE_COLOR:
            case LV_IMG_FORMAT_TRUE_COLOR_ALPHA:
            case LV_IMG_FORMAT_TRUE_COLOR_CHROMA_KEYED:
                return img_dsc->data;
            default:
                /*Else return NULL because the pixel are need to be processed first*/
                return NULL;
        }
    } else {
        decoder_src = NULL;
        decoder_src_type = LV_IMG_SRC_UNKNOWN;
        LV_LOG_WARN("Built-in image decoder don't know the src type");
        return NULL;
    }

    return NULL;
}


static lv_res_t lv_img_decoder_read_line(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    /*Try to read the line with the custom functions first*/
    if(lv_img_decoder_read_line_custom) {
        lv_res_t custom_res;
        custom_res = lv_img_decoder_read_line_custom(x, y, len, buf);
        if(custom_res == LV_RES_OK) return LV_RES_OK;       /*Custom read line supported this source*/
    }

     if(decoder_src_type == LV_IMG_SRC_FILE) {
 #if USE_LV_FILESYSTEM

         uint8_t px_size = lv_img_color_format_get_px_size(decoder_header.color_format);

         lv_fs_res_t res;
         uint32_t pos = ((y * decoder_header.w + x) * px_size) >> 3;
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
 #endif
     } else {
         const lv_img_dsc_t * img_dsc = decoder_src;

         if(img_dsc->header.color_format == LV_IMG_FORMAT_ALPHA_1BIT ||
            img_dsc->header.color_format == LV_IMG_FORMAT_ALPHA_2BIT ||
            img_dsc->header.color_format == LV_IMG_FORMAT_ALPHA_4BIT ||
            img_dsc->header.color_format == LV_IMG_FORMAT_ALPHA_8BIT)
         {
             lv_img_built_in_decoder_line_alpha(x, y, len, buf);
         } else {
             LV_LOG_WARN("Built-in image decoder not supports the color format");
             return false;
         }
     }

     return true;

}

static void lv_img_decoder_close(void)
{
    /*Try to close with the custom functions first*/
    if(lv_img_decoder_close_custom) {
        lv_res_t custom_res;
        custom_res = lv_img_decoder_close_custom();
        if(custom_res == LV_RES_OK) return;             /*Custom close supported this source*/
    }

    if(decoder_src) {
        if(decoder_src_type == LV_IMG_SRC_FILE) {
            lv_fs_close(&decoder_file);
        }
        decoder_src_type = LV_IMG_SRC_UNKNOWN;
        decoder_src = NULL;
    }
}


static lv_res_t lv_img_built_in_decoder_line_alpha(lv_coord_t x, lv_coord_t y, lv_coord_t len, uint8_t * buf)
{
    const lv_opa_t alpha1_opa_table[2] =  {0, 255};                   /*Opacity mapping with bpp = 1 (Just for compatibility)*/
    const lv_opa_t alpha2_opa_table[4] =  {0, 85, 170, 255};          /*Opacity mapping with bpp = 2*/
    const lv_opa_t alpha4_opa_table[16] = {0,   17,  34,  51,         /*Opacity mapping with bpp = 4*/
                                          68,  85,  102, 119,
                                          136, 153, 170, 187,
                                          204, 221, 238, 255
                                         };

    /*Simply fill the buffer with the color. Later only the alpha value will be modified.*/
    lv_color_t bg_color = LV_COLOR_RED;
    lv_coord_t i;
    for(i = 0; i < len; i++) {
#if LV_COLOR_DEPTH == 8
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full;
#elif LV_COLOR_DEPTH == 16
        /*Because of Alpha byte 16 bit color can start on odd address which can cause crash*/
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE] = bg_color.full & 0xFF;
        buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + 1] = (bg_color.full >> 8) & 0xFF;
#elif LV_COLOR_DEPTH == 24
        *((uint32_t*)&buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE]) = bg_color.full;
#endif
    }

    const lv_img_dsc_t * img_dsc = decoder_src;
	const uint8_t * data_tmp = img_dsc->data;
	const lv_opa_t * opa_table = NULL;
	uint8_t px_size = lv_img_color_format_get_px_size(img_dsc->header.color_format);
	uint16_t mask = (1 << px_size) - 1; /*E.g. px_size = 2; mask = 0x03*/

	lv_coord_t w = 0;
	int8_t pos = 0;
	switch(img_dsc->header.color_format) {
		case LV_IMG_FORMAT_ALPHA_1BIT:
			w = (img_dsc->header.w >> 3);		/*E.g. w = 20 -> w = 2 + 1*/
			if(img_dsc->header.w & 0x7) w++;
			data_tmp += w * y + (x >> 3);      /*First pixel*/
			pos = 7 - (x & 0x7);
			opa_table = alpha1_opa_table;
		break;
        case LV_IMG_FORMAT_ALPHA_2BIT:
            w = (img_dsc->header.w >> 2);       /*E.g. w = 13 -> w = 3 + 1 (bytes)*/
            if(img_dsc->header.w & 0x3) w++;
            data_tmp += w * y + (x >> 2);      /*First pixel*/
            pos = 6 - ((x & 0x3) * 2);
            opa_table = alpha2_opa_table;
        break;
        case LV_IMG_FORMAT_ALPHA_4BIT:
            w = (img_dsc->header.w >> 1);       /*E.g. w = 13 -> w = 6 + 1 (bytes)*/
            if(img_dsc->header.w & 0x1) w++;
            data_tmp += w * y + (x >> 1);      /*First pixel*/
            pos = 4 - ((x & 0x1) * 4);
            opa_table = alpha4_opa_table;
        break;
        case LV_IMG_FORMAT_ALPHA_8BIT:
            w = img_dsc->header.w;              /*E.g. x = 7 -> w = 7 (bytes)*/
            data_tmp += w * y + x;      /*First pixel*/
            pos = 0;
        break;
	}

	uint8_t byte_act = 0;
	uint8_t val_act;
	for(i = 0; i < len; i ++) {
		val_act = (data_tmp[byte_act] & (mask << pos)) >> pos;

		buf[i * LV_IMG_PX_SIZE_ALPHA_BYTE + LV_IMG_PX_SIZE_ALPHA_BYTE - 1] =
		         img_dsc->header.color_format == LV_IMG_FORMAT_ALPHA_8BIT ? val_act : opa_table[val_act];

		pos -= px_size;
		if(pos < 0) {
			pos = 8 - px_size;
			data_tmp++;
		}
	}

	return LV_RES_OK;
}
