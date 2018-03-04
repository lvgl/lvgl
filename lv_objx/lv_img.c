/**
 * @file lv_img.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lv_conf.h"
#if USE_LV_IMG != 0

/*Testing of dependencies*/
#if USE_LV_LABEL == 0
#error "lv_img: lv_label is required. Enable it in lv_conf.h (USE_LV_LABEL  1) "
#endif

#include "lv_img.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_ufs.h"
#include "../lv_misc/lv_txt.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_img_design(lv_obj_t * img, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_img_signal(lv_obj_t * img, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create an image objects
 * @param par pointer to an object, it will be the parent of the new button
 * @param copy pointer to a image object, if not NULL then the new object will be copied from it
 * @return pointer to the created image
 */
lv_obj_t * lv_img_create(lv_obj_t * par, lv_obj_t * copy)
{
    lv_obj_t * new_img = NULL;
    
    /*Create a basic object*/
    new_img = lv_obj_create(par, copy);
    lv_mem_assert(new_img);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_img);
    
    /*Extend the basic object to image object*/
    lv_img_ext_t * ext = lv_obj_allocate_ext_attr(new_img, sizeof(lv_img_ext_t));
    lv_mem_assert(ext);
    ext->src = NULL;
    ext->src_type = LV_IMG_SRC_UNKNOWN;
    ext->w = lv_obj_get_width(new_img);
    ext->h = lv_obj_get_height(new_img);
    ext->chroma_keyed = 0;
    ext->alpha_byte = 0;
    ext->auto_size = 1;

    /*Init the new object*/    
    lv_obj_set_signal_func(new_img, lv_img_signal);
    lv_obj_set_design_func(new_img, lv_img_design);
    
    if(copy == NULL) {
		/* Enable auto size for non screens
		 * because image screens are wallpapers
		 * and must be screen sized*/
		if(par != NULL) ext->auto_size = 1;
		else ext->auto_size = 0;
		if(par != NULL) lv_obj_set_style(new_img, NULL);            /*Inherit the style  by default*/
		else lv_obj_set_style(new_img, &lv_style_plain);            /*Set a style for screens*/
    } else {
        lv_img_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->auto_size = copy_ext->auto_size;
    	lv_img_set_src(new_img, copy_ext->src);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_img);
    }

    return new_img;
}


/*=====================
 * Setter functions 
 *====================*/


/**
 * Set the pixel map to display by the image
 * @param img pointer to an image object
 * @param data the image data
 */
void lv_img_set_src(lv_obj_t * img, const void * src_img)
{
    lv_img_src_t src_type = lv_img_get_src_type(src_img);
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);


#if USE_LV_FILESYSTEM == 0
    if(src_type == LV_IMG_SRC_FILE) src_type = LV_IMG_SRC_UNKNOWN;
#endif


    if(src_type == LV_IMG_SRC_UNKNOWN) {
        if(ext->src_type == LV_IMG_SRC_SYMBOL || ext->src_type == LV_IMG_SRC_FILE) {
            lv_mem_free(ext->src);
        }
        ext->src = NULL;
        ext->src_type = LV_IMG_SRC_UNKNOWN;
        return;
    }

    ext->src_type = src_type;

    if(src_type == LV_IMG_SRC_VARIABLE) {
        ext->src = src_img;
        ext->w = ((lv_img_t*)src_img)->header.w;
        ext->h = ((lv_img_t*)src_img)->header.h;
        ext->chroma_keyed = ((lv_img_t*)src_img)->header.chroma_keyed;
        ext->alpha_byte = ((lv_img_t*)src_img)->header.alpha_byte;
        lv_obj_set_size(img, ext->w, ext->h);
    }
#if USE_LV_FILESYSTEM
    else if(src_type == LV_IMG_SRC_FILE) {
        lv_fs_file_t file;
        lv_fs_res_t res;
        lv_img_t img_file_data;
        uint32_t rn;
        res = lv_fs_open(&file, src_img, LV_FS_MODE_RD);
        if(res == LV_FS_RES_OK) {
            res = lv_fs_read(&file, &img_file_data, sizeof(img_file_data), &rn);
        }

        /*Create a dummy header on fs error*/
        if(res != LV_FS_RES_OK || rn != sizeof(img_file_data)) {
            img_file_data.header.w = lv_obj_get_width(img);
            img_file_data.header.h = lv_obj_get_height(img);
            img_file_data.header.chroma_keyed = 0;
            img_file_data.header.alpha_byte = 0;
        }

        lv_fs_close(&file);

        ext->w = img_file_data.header.w;
        ext->h = img_file_data.header.h;
        ext->chroma_keyed = img_file_data.header.chroma_keyed;
        ext->alpha_byte = img_file_data.header.alpha_byte;

        /* If the new and the old src are the same then it was only a refresh.*/
        if(ext->src != src_img) {
            lv_mem_free(ext->src);
            char * new_fn = lv_mem_alloc(strlen(src_img) + 1);
            strcpy(new_fn, src_img);
            ext->src = new_fn;

        }
    }
#endif
    else if(src_type == LV_IMG_SRC_SYMBOL) {
        lv_style_t * style = lv_obj_get_style(img);
        lv_point_t size;
        lv_txt_get_size(&size, src_img, style->text.font, style->text.letter_space, style->text.line_space, LV_COORD_MAX, LV_TXT_FLAG_NONE);
        ext->w = size.x;
        ext->h = size.y;
        ext->chroma_keyed = 1;    /*Symbols always have transparent parts, Important because of cover check in the design function*/

        /* If the new and the old src are the same then it was only a refresh.*/
        if(ext->src != src_img) {
            lv_mem_free(ext->src);
            char * new_txt = lv_mem_alloc(strlen(src_img) + 1);
            strcpy(new_txt, src_img);
            ext->src = new_txt;
        }
    }

    if(lv_img_get_auto_size(img) != false) {
        lv_obj_set_size(img, ext->w, ext->h);
    }

    lv_obj_invalidate(img);
}

/**
 * Enable the auto size feature.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param autosize_en true: auto size enable, false: auto size disable
 */
void lv_img_set_auto_size(lv_obj_t * img, bool autosize_en)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    ext->auto_size = (autosize_en == false ? 0 : 1);
}


/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the type of an image source
 * @param src pointer to an image source:
 *  - pointer to an 'lv_img_t' variable (image stored internally and compiled into the code)
 *  - a path to an file (e.g. "S:/folder/image.bin")
 *  - or a symbol (e.g. SYMBOL_CLOSE)
 * @return type of the image source LV_IMG_SRC_VARIABLE/FILE/SYMBOL/UNKOWN
 */
lv_img_src_t lv_img_get_src_type(const void * src)
{
    if(src == NULL) return LV_IMG_SRC_UNKNOWN;
    const uint8_t * u8_p = src;

    /*The first byte shows the type of the image source*/
    if(u8_p[0] >= 'A' && u8_p[0] <= 'Z') return LV_IMG_SRC_FILE;    /*It's a driver letter*/
    else if(((u8_p[0] & 0xFC) >> 2) == LV_IMG_FORMAT_INTERNAL_RAW) return LV_IMG_SRC_VARIABLE;      /*Mask the file format part og of lv_img_t header. IT should be 0 which means C array */
    else if(u8_p[0] >= ' ') return LV_IMG_SRC_SYMBOL;               /*Other printable characters are considered symbols*/

    else return LV_IMG_SRC_UNKNOWN;
}

/**
 * Get the name of the file set for an image
 * @param img pointer to an image
 * @return file name
 */
const char * lv_img_get_file_name(lv_obj_t * img)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    if(ext->src_type == LV_IMG_SRC_FILE) return ext->src;
    else return "";
}


/**
 * Get the auto size enable attribute
 * @param img pointer to an image
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool lv_img_get_auto_size(lv_obj_t * img)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    return ext->auto_size == 0 ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the images
 * @param img pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'        
 */
static bool lv_img_design(lv_obj_t * img, const lv_area_t * mask, lv_design_mode_t mode)
{
    lv_style_t * style = lv_obj_get_style(img);
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    if(mode == LV_DESIGN_COVER_CHK) {
        bool cover = false;
        if(ext->src_type == LV_IMG_SRC_UNKNOWN || ext->src_type == LV_IMG_SRC_SYMBOL) return false;

        if(ext->chroma_keyed == 0 && ext->alpha_byte == 0) cover = lv_area_is_in(mask, &img->coords);
        return cover;

    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        if(ext->h == 0 || ext->w == 0) return true;
		lv_area_t coords;

		lv_obj_get_coords(img, &coords);

		if(ext->src_type == LV_IMG_SRC_FILE || ext->src_type == LV_IMG_SRC_VARIABLE) {
		    lv_area_t cords_tmp;
            cords_tmp.y1 = coords.y1;
            cords_tmp.y2 = coords.y1 + ext->h - 1;

            for(; cords_tmp.y1 < coords.y2; cords_tmp.y1 += ext->h, cords_tmp.y2 += ext->h) {
                cords_tmp.x1 = coords.x1;
                cords_tmp.x2 = coords.x1 + ext->w - 1;
                for(; cords_tmp.x1 < coords.x2; cords_tmp.x1 += ext->w, cords_tmp.x2 += ext->w) {
                    lv_draw_img(&cords_tmp, mask, style, ext->src);
                }
            }
		} else if(ext->src_type == LV_IMG_SRC_SYMBOL) {
            lv_draw_label(&coords, mask, style, ext->src, LV_TXT_FLAG_NONE, NULL);

		} else {

		    /*Trigger the error handler of image drawer*/
            lv_draw_img(&img->coords, mask, style, NULL);

		}
    }
    
    return true;
}


/**
 * Signal function of the image
 * @param img pointer to an image object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_img_signal(lv_obj_t * img, lv_signal_t sign, void * param)
{
    lv_res_t res;

    /* Include the ancient signal function */
    res = ancestor_signal(img, sign, param);
    if(res != LV_RES_OK) return res;

    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);
    if(sign == LV_SIGNAL_CLEANUP) {
        if(ext->src_type == LV_IMG_SRC_FILE || ext->src_type == LV_IMG_SRC_SYMBOL) {
            lv_mem_free(ext->src);
            ext->src = NULL;
            ext->src_type = LV_IMG_SRC_UNKNOWN;
        }
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Refresh the file name to refresh the symbol text size*/
        if(ext->src_type == LV_IMG_SRC_SYMBOL) {
            lv_img_set_src(img, ext->src);

        }
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_img";
    }

    return res;
}

#endif
