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
#if USE_LV_IMG != 0

#include "../lv_obj/lv_obj.h"
#include "misc/fs/fsint.h"

/*********************
 *      DEFINES
 *********************/
#define LV_IMG_DECLARE(var_name) extern const color_int_t var_name[];

/**********************
 *      TYPEDEFS
 **********************/
typedef struct
{
	lv_objs_t objs;
	opa_t recolor_opa;
}lv_imgs_t;

typedef struct
{
    char* fn_dp;
    cord_t w;
    cord_t h;
    uint8_t auto_size 	:1;
    uint8_t transp 	    :1; 	/*Transp. bit in the images header (library handles this)*/
}lv_img_ext_t;

typedef enum
{
	LV_IMGS_DEF,
	LV_IMGS_LIGHT,
	LV_IMGS_DARK,
}lv_imgs_builtin_t;

typedef struct
{
	uint16_t w;
	uint16_t h;
	uint16_t cd;		/*Color depth*/
	uint16_t transp :1;	/*Do not draw LV_IMG_TRANSP_COLOR pixels*/
}lv_img_raw_header_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t* lv_img_create(lv_obj_t* par_dp, lv_obj_t * copy_dp);

bool lv_img_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);
void lv_img_set_file(lv_obj_t* obj_p, const char * fn);
fs_res_t lv_img_create_file(const char * fn, const color_int_t * data_p);
void lv_img_set_auto_size(lv_obj_t* obj_dp, bool en);

bool lv_img_get_auto_size(lv_obj_t* obj_dp);

lv_imgs_t * lv_imgs_get(lv_imgs_builtin_t style, lv_imgs_t * copy_p);

/**********************
 *      MACROS
 **********************/

#endif

#endif
