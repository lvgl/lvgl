/**
 * @file lv_page.h
 * 
 */

#ifndef LV_PAGE_H
#define LV_PAGE_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_PAGE != 0

#include "../lv_obj/lv_obj.h"
#include "lv_rect.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum
{
	LV_PAGE_SB_MODE_OFF,
	LV_PAGE_SB_MODE_ON,
	LV_PAGE_SB_MODE_AUTO,
}lv_page_sb_mode_t;

typedef struct
{
    lv_rects_t bg_rects;
    lv_rects_t sb_rects;
    cord_t sb_width;
    cord_t margin_hor;	/*Extra size between the parent and the page horizontally*/
    cord_t margin_ver;  /*Extra size between the parent and the page vertically*/
    cord_t padding_hor; /*Extra size on page horizontally*/
    cord_t padding_ver; /*Extra size on page vertically*/
    lv_page_sb_mode_t sb_mode;
    uint8_t sb_opa;
}lv_pages_t;


typedef struct
{
    lv_obj_t* sbh_dp;  /*Horizontal scrollbar*/
    lv_obj_t* sbv_dp;  /*Vertical scrollbar*/
}lv_page_t;

typedef enum
{
	LV_PAGES_DEF,
	LV_PAGES_PAPER,
	LV_PAGES_TRANSP,
}lv_pages_builtin_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t* lv_page_create(lv_obj_t* par_dp, lv_obj_t * ori_dp);
void lv_page_glue_obj(lv_obj_t* page_p, bool en);
lv_pages_t * lv_pages_get(lv_pages_builtin_t style, lv_pages_t * to_copy);
bool lv_page_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param);

/**********************
 *      MACROS
 **********************/

#endif

#endif
