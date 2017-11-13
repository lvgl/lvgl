/**
 * @file lv_img.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "misc_conf.h"
#if USE_LV_IMG != 0 && USE_FSINT != 0

#include "lv_img.h"
#include "../lv_draw/lv_draw.h"
#include "misc/fs/fsint.h"
#include "misc/fs/ufs/ufs.h"
#include "misc/gfx/text.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_img_design(lv_obj_t * img, const area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_img_signal(lv_obj_t * img, lv_signal_t sign, void * param);
static bool lv_img_is_symbol(const char * txt);

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
    dm_assert(new_img);
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_img);
    
    /*Extend the basic object to image object*/
    lv_img_ext_t * ext = lv_obj_allocate_ext_attr(new_img, sizeof(lv_img_ext_t));
    dm_assert(ext);
    ext->fn = NULL;
    ext->w = lv_obj_get_width(new_img);
    ext->h = lv_obj_get_height(new_img);
    ext->transp = 0;
    ext->upscale = 0;
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
		if(par != NULL) lv_obj_set_style(new_img, NULL);                 /*Inherit the style  by default*/
		else lv_obj_set_style(new_img, &lv_style_plain);    /*Set style for screens*/
    } else {
        lv_img_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
    	ext->auto_size = copy_ext->auto_size;
        ext->upscale = copy_ext->upscale;
    	lv_img_set_file(new_img, copy_ext->fn);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_img);
    }

    return new_img;
}

/**
 * Create a file to the RAMFS from a picture data
 * @param fn file name of the new file (e.g. "pic1", will be available at "U:/pic1")
 * @param data pointer to a color map with lv_img_raw_header_t header
 * @return result of the file operation. FS_RES_OK or any error from fs_res_t
 */
fs_res_t lv_img_create_file(const char * fn, const color_int_t * data)
{
#if USE_UFS != 0
	const lv_img_raw_header_t * raw_p = (lv_img_raw_header_t *) data;
	fs_res_t res;
	res = ufs_create_const(fn, data, raw_p->w * raw_p->h * sizeof(color_t) + sizeof(lv_img_raw_header_t));

	return res;
#else
	return FS_RES_NOT_EX;
#endif
}

/*=====================
 * Setter functions 
 *====================*/

/**
 * Set a file to the image
 * @param img pointer to an image object
 * @param fn file name in the RAMFS to set as picture (e.g. "U:/pic1"). 
 */
void lv_img_set_file(lv_obj_t * img, const char * fn)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);
    
    /*Handle normal images*/
	if(lv_img_is_symbol(fn) == false) {
        fs_file_t file;
        fs_res_t res;
        lv_img_raw_header_t header;
        uint32_t rn;
        res = fs_open(&file, fn, FS_MODE_RD);
        if(res == FS_RES_OK) {
            res = fs_read(&file, &header, sizeof(header), &rn);
        }

        /*Create a dummy header on fs error*/
        if(res != FS_RES_OK || rn != sizeof(header)) {
            header.w = lv_obj_get_width(img);
            header.h = lv_obj_get_height(img);
            header.transp = 0;
            header.cd = 0;
        }

        fs_close(&file);

        ext->w = header.w;
        ext->h = header.h;
        ext->transp = header.transp;

#if LV_ANTIALIAS != 0
        if(ext->upscale != 0) {
            ext->w *=  2;
            ext->h *=  2;
        }
#endif
	}
	/*Handle symbol texts*/
	else {
        lv_style_t * style = lv_obj_get_style(img);
        point_t size;
        txt_get_size(&size, fn, style->text.font, style->text.letter_space, style->text.line_space, CORD_MAX, TXT_FLAG_NONE);
        ext->w = size.x;
        ext->h = size.y;
        ext->transp = 1;    /*Symbols always have transparent parts*/
	}

	if(fn != NULL) {
	    ext->fn = dm_realloc(ext->fn, strlen(fn) + 1);
		strcpy(ext->fn, fn);
	} else {
		ext->fn = NULL;
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

/**
 * Enable the upscaling if LV_ANTIALIAS is enabled.
 * If enabled the object size will be same as the picture size.
 * @param img pointer to an image
 * @param upscale_en true: upscale enable, false: upscale disable
 */
void lv_img_set_upscale(lv_obj_t * img, bool upscale_en)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);
    
    /*Upscale works only if antialiassing is enabled*/
#if LV_ANTIALIAS == 0
    upscale_en = false;
#else
    ext->upscale = (upscale_en == false ? 0 : 1);
#endif
    /*Refresh the image with the new size*/
    lv_img_set_file(img, ext->fn);
}

/*=====================
 * Getter functions 
 *====================*/


/**
 * Get the name of the file set for an image
 * @param img pointer to an image
 * @return file name
 */
const char * lv_img_get_file_name(lv_obj_t * img)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    return ext->fn;
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

/**
 * Get the upscale enable attribute
 * @param img pointer to an image
 * @return true: upscale is enabled, false: upscale is disabled
 */
bool lv_img_get_upscale(lv_obj_t * img)
{
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    return ext->upscale == 0 ? false : true;
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
static bool lv_img_design(lv_obj_t * img, const area_t * mask, lv_design_mode_t mode)
{
    lv_style_t * style = lv_obj_get_style(img);
    lv_img_ext_t * ext = lv_obj_get_ext_attr(img);

    if(mode == LV_DESIGN_COVER_CHK) {
        bool cover = false;
        if(ext->transp == 0) cover = area_is_in(mask, &img->coords);
        return cover;

    } else if(mode == LV_DESIGN_DRAW_MAIN) {
        if(ext->h == 0 || ext->w == 0) return true;
		area_t cords;
/*Create a default style for symbol texts*/
        bool sym = lv_img_is_symbol(ext->fn);

		lv_obj_get_coords(img, &cords);

		area_t cords_tmp;
		cords_tmp.y1 = cords.y1;
		cords_tmp.y2 = cords.y1 + ext->h - 1;

		for(; cords_tmp.y1 < cords.y2; cords_tmp.y1 += ext->h, cords_tmp.y2 += ext->h) {
			cords_tmp.x1 = cords.x1;
			cords_tmp.x2 = cords.x1 + ext->w - 1;
			for(; cords_tmp.x1 < cords.x2; cords_tmp.x1 += ext->w, cords_tmp.x2 += ext->w) {
			    if(sym == false) lv_draw_img(&cords_tmp, mask, style, ext->fn);
			    else lv_draw_label(&cords_tmp, mask, style, ext->fn, TXT_FLAG_NONE, NULL);

			}
		}
    }
    
    return true;
}


/**
 * Signal function of the image
 * @param img pointer to animage object
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
        dm_free(ext->fn);
    }
    else if(sign == LV_SIGNAL_STYLE_CHG) {
        /*Refresh the file name to refresh the symbol text size*/
        if(lv_img_is_symbol(ext->fn) != false) {
            lv_img_set_file(img, ext->fn);
        }
    }

    return LV_RES_OK;
}


/**
 * From the settings in lv_conf.h and the file name
 * tells it a filename or a symbol text.
 * @param txt a file name (e.g. "U:/file1") or a symbol (e.g. SYMBOL_OK)
 * @return true: 'txt' is a symbol text, false: 'txt' is a file name
 */
static bool lv_img_is_symbol(const char * txt)
{
    if(txt == NULL) return false;

    /* if txt begins with an upper case letter then it refers to a driver
     * so it is a file name*/
    if(txt[0] >= 'A' && txt[0] <= 'Z') return false;

    /*If not returned during the above tests then consider as text*/
    return true;
}


#endif
