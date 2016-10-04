/**
 * @file lv_img.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_IMG != 0

#include "lv_img.h"
#include "../lv_draw/lv_draw.h"
#include "misc/fs/fsint.h"
#include "misc/fs/ufs/ufs.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_img_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode);
static void lv_imgs_init(void);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_imgs_t lv_imgs_def;
static lv_imgs_t lv_imgs_light;
static lv_imgs_t lv_imgs_dark;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create an image objects
 * @param par_dp pointer to an object, it will be the parent of the new button
 * @param copy_dp pointer to a rectangle object, if not NULL then the new object will be copied from it
 * @return pointer to the created image
 */
lv_obj_t* lv_img_create(lv_obj_t* par_dp, lv_obj_t * copy_dp)
{
    lv_obj_t* new_obj_dp = NULL;
    
    /*Create a basic object*/
    new_obj_dp = lv_obj_create(par_dp, NULL);
    
    /*Extend the basic object to image object*/
    lv_obj_alloc_ext(new_obj_dp, sizeof(lv_img_ext_t));

    /*Init the new object*/    
    lv_obj_set_signal_f(new_obj_dp, lv_img_signal);
    lv_obj_set_design_f(new_obj_dp, lv_img_design);
    
    lv_img_ext_t * img_ext_dp = lv_obj_get_ext(new_obj_dp);

    if(copy_dp == NULL) {
		img_ext_dp->fn_dp = NULL;
		img_ext_dp->w = lv_obj_get_width(new_obj_dp);
		img_ext_dp->h = lv_obj_get_height(new_obj_dp);
		img_ext_dp->transp = 0;

		/*Enable auto size for non screens*/
		if(par_dp != NULL) {
			img_ext_dp->auto_size = 1;
		} else {
			img_ext_dp->auto_size = 0;
		}
	    lv_obj_set_style(new_obj_dp, lv_imgs_get(LV_IMGS_DEF, NULL));
    } else {
    	img_ext_dp->auto_size = LV_EA(copy_dp, lv_img_ext_t)->auto_size;
    	lv_img_set_file(new_obj_dp, LV_EA(copy_dp, lv_img_ext_t)->fn_dp);
    }


    return new_obj_dp;
}

/**
 * Signal function of the image
 * @param obj_dp pointer to animage object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 */
bool lv_img_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid = true;

    /* Include the ancient signal function */
    valid = lv_obj_signal(obj_dp, sign, param); 

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
        lv_img_ext_t * img_p = lv_obj_get_ext(obj_dp);
        switch(sign) {
        	case LV_SIGNAL_CLEANUP:
        		dm_free(img_p->fn_dp);
        		break;
			default:
				break;
        }
    }
    
    return valid;
}

/**
 * Return with a pointer to built-in style and/or copy it to a variable
 * @param style a style name from lv_imgs_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_imgs_t style
 */
lv_imgs_t * lv_imgs_get(lv_imgs_builtin_t style, lv_imgs_t * copy_p)
{
	static bool style_inited = false;

	/*Make the style initialization if it is not done yet*/
	if(style_inited == false) {
		lv_imgs_init();
		style_inited = true;
	}

	lv_imgs_t * style_p;

	switch(style) {
		case LV_IMGS_DEF:
			style_p = &lv_imgs_def;
			break;
		case LV_IMGS_LIGHT:
			style_p = &lv_imgs_light;
			break;
		case LV_IMGS_DARK:
			style_p = &lv_imgs_dark;
			break;
		default:
			style_p = &lv_imgs_def;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_imgs_t));
		else memcpy(copy_p, &lv_imgs_def, sizeof(lv_imgs_t));
	}

	return style_p;
}

/**
 * Create a file to the RAMFS from a picture data
 * @param fn file name of the new file (e.g. "pic1", will be available at "U:/pic1")
 * @param data_p pointer to a color map with lv_img_raw_header_t header
 * @return result of the file operation. FS_RES_OK or any error from fs_res_t
 */
fs_res_t lv_img_create_file(const char * fn, const color_int_t * data_p)
{
	const lv_img_raw_header_t * raw_p = (lv_img_raw_header_t *) data_p;
	fs_res_t res;
	res = ufs_create_const(fn, data_p, raw_p->w * raw_p->h * sizeof(color_t));

	return res;
}

/*=====================
 * Setter functions 
 *====================*/

/**
 * Set a file to the image
 * @param obj_dp pointer to an image object
 * @param fn file name in the RAMFS to set as picture (e.g. "U:/pic1"). 
 */
void lv_img_set_file(lv_obj_t* obj_dp, const char * fn)
{
    lv_img_ext_t * img_ext_p = lv_obj_get_ext(obj_dp);
    
    fs_file_t file;
    fs_res_t res;
	lv_img_raw_header_t header;
	uint32_t rn;
    
    res = fs_open(&file, fn, FS_MODE_RD);
    if(res == FS_RES_OK) {
		res = fs_read(&file, &header, sizeof(header), &rn);
    }

    if(res != FS_RES_OK || rn != sizeof(header)) {
		/*Create a dummy header*/
		header.w = lv_obj_get_width(obj_dp);
		header.h = lv_obj_get_height(obj_dp);
		header.transp = 0;
	}

	fs_close(&file);

	img_ext_p->w = header.w;
	img_ext_p->h = header.h;
	img_ext_p->transp = header.transp;
#if LV_UPSCALE_MAP != 0
	img_ext_p->w *= LV_DOWNSCALE;
	img_ext_p->h *= LV_DOWNSCALE;
#endif
	if(fn != NULL) {
		img_ext_p->fn_dp = dm_realloc(img_ext_p->fn_dp, strlen(fn) + 1);
		strcpy(img_ext_p->fn_dp, fn);
	} else {
		img_ext_p->fn_dp = NULL;
	}


    if(lv_img_get_auto_size(obj_dp) != false) {
    	lv_obj_set_size(obj_dp, img_ext_p->w, img_ext_p->h);
    }
}

/**
 * Enable the auto size feature.  If enabled the object size will be same as the picture size.
 * @param obj_dp pointer to an image
 * @param en true: auto size enable, false: auto size disable
 */
void lv_img_set_auto_size(lv_obj_t* obj_dp, bool en)
{
    lv_img_ext_t * img_ext_p = lv_obj_get_ext(obj_dp);

    img_ext_p->auto_size = (en == false ? 0 : 1);
}

/*=====================
 * Getter functions 
 *====================*/

/**
 * Get the auto size enable attribute
 * @param obj_dp pinter to an image
 * @return true: auto size is enabled, false: auto size is disabled
 */
bool lv_img_get_auto_size(lv_obj_t* obj_dp)
{
    lv_img_ext_t * img_ext_p = lv_obj_get_ext(obj_dp);

    return img_ext_p->auto_size == 0 ? false : true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the images
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'        
 */
static bool lv_img_design(lv_obj_t* obj_dp, const area_t * mask_p, lv_design_mode_t mode)
{
    lv_imgs_t * imgs_p = lv_obj_get_style(obj_dp);
    lv_img_ext_t * ext_p = lv_obj_get_ext(obj_dp);

    if(mode == LV_DESIGN_COVER_CHK) {
        if(ext_p->transp == 0) {
        	bool cover;
        	cover = area_is_in(mask_p, &obj_dp->cords);
        	return cover;
        }
        else return false;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		area_t cords;

		lv_obj_get_cords(obj_dp, &cords);
		opa_t opa = lv_obj_get_opa(obj_dp);

		area_t cords_tmp;
		cords_tmp.y1 = cords.y1;
		cords_tmp.y2 = cords.y1 + ext_p->h - 1;

		for(; cords_tmp.y1 < cords.y2; cords_tmp.y1 += ext_p->h, cords_tmp.y2 += ext_p->h) {
			cords_tmp.x1 = cords.x1;
			cords_tmp.x2 = cords.x1 + ext_p->w - 1;
			for(; cords_tmp.x1 < cords.x2; cords_tmp.x1 += ext_p->w, cords_tmp.x2 += ext_p->w) {
				lv_draw_img(&cords_tmp, mask_p, imgs_p, opa, ext_p->fn_dp);
			}
		}
    }
    
    return true;
}
/**
 * Initialize the image styles
 */
static void lv_imgs_init(void)
{
	/*Default style*/
	lv_imgs_def.objs.color = COLOR_BLACK;
	lv_imgs_def.recolor_opa = OPA_TRANSP;

	/*Dark style*/
	memcpy(&lv_imgs_dark, &lv_imgs_def, sizeof(lv_imgs_t));
	lv_imgs_dark.objs.color = COLOR_WHITE; lv_imgs_dark.recolor_opa = OPA_50;

	/*Light style*/
	memcpy(&lv_imgs_light, &lv_imgs_dark, sizeof(lv_imgs_t));
	lv_imgs_light.objs.color = COLOR_WHITE; lv_imgs_light.recolor_opa = OPA_50;

}

#endif
