/**
 * @file lv_btn.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_btn.h"
#if LV_USE_BTN != 0

#include <string.h>
#include "../lv_core/lv_group.h"
#include "../lv_misc/lv_debug.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_btn"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_btn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy);
static void lv_btn_destructor(void * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_btn_class_t lv_btn;


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a button object
 * @param parent pointer to an object, it will be the parent of the new button
 * @param copy DEPRECATED, will be removed in v9.
 *             Pointer to an other button to copy.
 * @return pointer to the created button
 */
lv_obj_t * lv_btn_create(lv_obj_t * parent, const lv_obj_t * copy)
{


    if(!lv_btn._inited) {
         LV_CLASS_INIT(lv_btn, lv_obj);
         lv_btn.constructor = lv_btn_constructor;
         lv_btn.destructor = lv_btn_destructor;
     }

     lv_obj_t * obj = lv_class_new(&lv_btn);
     lv_btn.constructor(obj, parent, copy);

     lv_obj_create_finish(obj, parent, copy);

     LV_LOG_TRACE("button create started");
     return obj;
}

/*=====================
 * Setter functions
 *====================*/

/*=====================
 * Getter functions
 *====================*/

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_btn_constructor(lv_obj_t * obj, lv_obj_t * parent, const lv_obj_t * copy)
{
    LV_LOG_TRACE("lv_btn create started");

    LV_CLASS_CONSTRUCTOR_BEGIN(obj, lv_btn)
    lv_btn.base_p->constructor(obj, parent, copy);

    lv_obj_set_size(obj, LV_DPI, LV_DPI / 3);
    lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);


    LV_CLASS_CONSTRUCTOR_END(obj, lv_btn)
    LV_LOG_INFO("btn created");
}

static void lv_btn_destructor(void * obj)
{
//    lv_bar_t * bar = obj;
//
//    _lv_obj_reset_style_list_no_refr(obj, LV_BAR_PART_INDIC);
//#if LV_USE_ANIMATION
//    lv_anim_del(&bar->cur_value_anim, NULL);
//    lv_anim_del(&bar->start_value_anim, NULL);
//#endif

    lv_btn.base_p->destructor(obj);
}

#endif
