/**
 * @file lv_img.h
 * 
 */

#ifndef LV_IMG_H
#define LV_IMG_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include "misc_conf.h"
#if USE_LV_IMG != 0 && USE_FSINT != 0 && USE_UFS != 0

#include "../lv_obj/lv_obj.h"
#include "misc/fs/fsint.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/*Style of template*/
typedef struct
{
	lv_objs_t objs;	/*Style of ancestor*/
	/*New style element for this type */
	opa_t recolor_opa;
}lv_imgs_t;

/*Built-in styles of template*/
typedef enum
{
	LV_IMGS_DEF,
	LV_IMGS_LIGHT,
	LV_IMGS_DARK,
}lv_imgs_builtin_t;

/*Data of template*/
typedef struct
{
	/*No ext. because inherited from the base object*/ /*Ext. of ancestor*/
	/*New data for this type */
    char* fn;	/*Image file name. E.g. "U:/my_image"*/
    cord_t w;	/*Width of the image (doubled when upscaled)*/
    cord_t h;	/*Height of the image (doubled when upscaled)*/
    uint8_t auto_size 	:1;		/*1: automatically set the object size to the image size*/
    uint8_t upscale     :1;     /*1: upscale to double size*/
    uint8_t transp 	    :1; 	/*Transp. bit in the images header (library handles this)*/
}lv_img_ext_t;

/*Image header*/
typedef struct
{
	uint16_t w;			/*Width of the image map*/
	uint16_t h;     	/*Height of the image map*/
	uint16_t cd;		/*Color depth (8/16 or 24)*/
	uint16_t transp :1;	/*1: Do not draw LV_IMG_TRANSP_COLOR pixels*/
}lv_img_raw_header_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t * lv_img_create(lv_obj_t * par, lv_obj_t * copy);

bool lv_img_signal(lv_obj_t * img, lv_signal_t sign, void * param);
void lv_img_set_file(lv_obj_t * img, const char * fn);
fs_res_t lv_img_create_file(const char * fn, const color_int_t * data_p);
void lv_img_set_auto_size(lv_obj_t * img, bool en);
void lv_img_set_upscale(lv_obj_t * img, bool en);

bool lv_img_get_auto_size(lv_obj_t * img);
bool lv_img_get_upscale(lv_obj_t * img);
lv_imgs_t * lv_imgs_get(lv_imgs_builtin_t style, lv_imgs_t * copy);

/**********************
 *      MACROS
 **********************/
#define LV_IMG_DECLARE(var_name) extern const color_int_t var_name[];

#endif

#endif
