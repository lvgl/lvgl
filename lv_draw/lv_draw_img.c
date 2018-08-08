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
static bool lv_img_draw_built_in(const lv_area_t * coords, const lv_area_t * mask,
                         const void * src, const lv_style_t * style, lv_opa_t opa_scale);
static bool lv_img_dsc_get_info_built_in(const char * src, lv_img_header_t * header, const lv_style_t * style);

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
        lv_draw_rect(coords, mask, &lv_style_plain, LV_OPA_COVER);
        lv_draw_label(coords, mask, &lv_style_plain, LV_OPA_COVER, "No\ndata", LV_TXT_FLAG_NONE, NULL);
        return;
    }

    lv_img_draw_built_in(coords, mask, src, style, opa_scale);
}


/**
 *
 * @param src
 * @param header
 * @param style
 * @return
 */
bool lv_img_dsc_get_info(const char * src, lv_img_header_t * header, const lv_style_t * style)
{
	header->always_zero = 0;

	return lv_img_dsc_get_info_built_in(src, header, style);

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
		case LV_IMG_FORMAT_INDEXED_ALPHA_2BIT:
			return 2;

		case LV_IMG_FORMAT_INDEXED_4BIT:
		case LV_IMG_FORMAT_ALPHA_4BIT:
		case LV_IMG_FORMAT_INDEXED_ALPHA_4BIT:
			return 4;

		case LV_IMG_FORMAT_INDEXED_8BIT:
		case LV_IMG_FORMAT_ALPHA_8BIT:
		case LV_IMG_FORMAT_INDEXED_ALPHA_8BIT:
			return 8;

		case LV_IMG_FORMAT_INDEXED_ALPHA_16BIT:
			return 16;
		default:
			return 0;
	}

	return 0;
}

bool lv_img_color_format_is_chroma_key(lv_img_color_format_t cf)
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
		case LV_IMG_FORMAT_INDEXED_ALPHA_2BIT:
		case LV_IMG_FORMAT_INDEXED_ALPHA_4BIT:
		case LV_IMG_FORMAT_INDEXED_ALPHA_8BIT:
		case LV_IMG_FORMAT_INDEXED_ALPHA_16BIT:
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

static bool lv_img_dsc_get_info_built_in(const char * src, lv_img_header_t * header, const lv_style_t * style)
{
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
        lv_point_t size;
        lv_txt_get_size(&size, src, style->text.font, style->text.letter_space, style->text.line_space, LV_COORD_MAX, LV_TXT_FLAG_NONE);
        header->w = size.x;
        header->h = size.y;
        header->color_format  = LV_IMG_FORMAT_ALPHA_1BIT;    /*Symbols always have transparent parts, Important because of cover check in the design function*/
    }

    return true;
}


static bool lv_img_draw_built_in(const lv_area_t * coords, const lv_area_t * mask,
                         const void * src, const lv_style_t * style, lv_opa_t opa_scale)
{
    lv_opa_t opa = opa_scale == LV_OPA_COVER ? style->image.opa : (uint16_t)((uint16_t) style->image.opa * opa_scale) >> 8;

    lv_img_src_t src_type = lv_img_src_get_type(src);

    if(src_type == LV_IMG_SRC_FILE) { /*It will be a path to a file*/
#if USE_LV_FILESYSTEM
        lv_fs_file_t file;
        lv_fs_res_t res = lv_fs_open(&file, src, LV_FS_MODE_RD);
        if(res != LV_FS_RES_OK ) return false;

		lv_img_header_t header;
		uint32_t br;
		res = lv_fs_read(&file, &header, sizeof(header), &br);

		lv_area_t mask_com;    /*Common area of mask and cords. Only these pixels are required now.*/
		bool union_ok;
		union_ok = lv_area_intersect(&mask_com, mask, coords);
		if(union_ok == false) {
			lv_fs_close(&file);
            return true;         /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
		}

		uint8_t px_size = lv_img_color_format_get_px_size(header.color_format);
		if(px_size == 0) return false;

		/* Move the file pointer to the start address according to mask*/
		uint32_t start_offset = sizeof(lv_img_header_t);
		start_offset += (lv_area_get_width(coords)) * (mask_com.y1 - coords->y1) * px_size;      /*First row*/
		start_offset += (mask_com.x1 - coords->x1) * px_size;                                    /*First col*/
		lv_fs_seek(&file, start_offset);

		uint32_t useful_data = lv_area_get_width(&mask_com) * px_size;
		uint32_t next_row = lv_area_get_width(coords) * px_size - useful_data;

		lv_area_t line;
		lv_area_copy(&line, &mask_com);
		lv_area_set_height(&line, 1);

		lv_coord_t row;
		uint32_t act_pos;

#if LV_COMPILER_VLA_SUPPORTED
		uint8_t buf[lv_area_get_width(&mask_com) * px_size];
#else
# if LV_HOR_RES > LV_VER_RES
		uint8_t buf[LV_HOR_RES * ((LV_COLOR_DEPTH >> 8) + 1)];  /*+1 because of the possible alpha byte*/
# else
		uint8_t buf[LV_VER_RES * ((LV_COLOR_DEPTH >> 8) + 1)];
# endif
#endif

		bool chroma_keyed = lv_img_color_format_is_chroma_key(header.color_format);
		bool alpha_byte = lv_img_color_format_has_alpha(header.color_format);

		for(row = mask_com.y1; row <= mask_com.y2; row ++) {
			res = lv_fs_read(&file, buf, useful_data, &br);

			map_fp(&line, &mask_com, (uint8_t *)buf, opa, chroma_keyed, alpha_byte,
				   style->image.color, style->image.intense);

			lv_fs_tell(&file, &act_pos);
			lv_fs_seek(&file, act_pos + next_row);
			line.y1++;    /*Go down a line*/
			line.y2++;
		}

		lv_fs_close(&file);
#endif
    } else {
        const lv_img_dsc_t * img_var = src;
        lv_area_t mask_com;    /*Common area of mask and coords*/
        bool union_ok;
        union_ok = lv_area_intersect(&mask_com, mask, coords);
        if(union_ok == false) {
            return true;         /*Out of mask. There is nothing to draw so the image is drawn successfully.*/
        }

		bool chroma_keyed = lv_img_color_format_is_chroma_key(img_var->header.color_format);
		bool alpha_byte = lv_img_color_format_has_alpha(img_var->header.color_format);

        map_fp(coords, mask, img_var->data, opa, chroma_keyed, alpha_byte, style->image.color, style->image.intense);
    }
}
