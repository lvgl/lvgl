/**
 * @file lv_btn.h
 * 
 */

#ifndef LV_BTN_H
#define LV_BTN_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_BTN != 0

#include "lv_rect.h"
#include "../lv_obj/lv_dispi.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
    LV_BTN_PR,
    LV_BTN_REL,
    LV_BTN_TGL_PR,
    LV_BTN_TGL_REL,
    LV_BTN_INA,
}lv_btn_state_t;

typedef struct
{
    lv_rects_t rel;
    lv_rects_t pr;
    lv_rects_t tgl_rel;
    lv_rects_t tgl_pr;
    lv_rects_t ina;
}lv_btns_t;

typedef enum
{
	LV_BTNS_DEF,
	LV_BTNS_TRANSP,
	LV_BTNS_BORDER,
}lv_btns_builtin_t;

typedef struct
{       
    bool (*pr_action)(lv_obj_t*, lv_dispi_t *);
    bool  (*rel_action)(lv_obj_t*, lv_dispi_t *);
    bool (*lpr_action)(lv_obj_t*, lv_dispi_t *);
    
    lv_btn_state_t state;
    uint8_t tgl :1;      /*1: Toggle enabled*/
    uint8_t lpr_exec :1; /*1: long press action executed (Not for user)*/
}lv_btn_ext_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/*Create function*/
lv_obj_t* lv_btn_create(lv_obj_t* par_dp, lv_obj_t * c_dp);

bool lv_btn_signal(lv_obj_t * obj_dp, lv_signal_t sign, void * param);
lv_btns_t * lv_btns_get(lv_btns_builtin_t style, lv_btns_t * copy_p);

void lv_btn_set_tgl(lv_obj_t* obj_dp, bool tgl);
void lv_btn_set_state(lv_obj_t* obj_dp, lv_btn_state_t state);
void lv_btn_set_pr_action(lv_obj_t* obi_p, bool (*pr_action)(lv_obj_t*, lv_dispi_t *));
void lv_btn_set_rel_action(lv_obj_t* obj_dp, bool (*rel_action)(lv_obj_t*, lv_dispi_t *));
void lv_btn_set_lpr_action(lv_obj_t* obj_dp, bool (*lpr_action)(lv_obj_t*, lv_dispi_t *));

bool lv_btn_get_tgl(lv_obj_t* obj_dp);
lv_btn_state_t lv_btn_get_state(lv_obj_t* obj_dp);

/**********************
 *      MACROS
 **********************/

#endif

#endif
