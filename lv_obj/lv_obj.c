/**
 * @file lv_base_obj.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/

#include <lv_conf.h>
#include <lvgl/lv_draw/lv_draw_rbasic.h>
#include <lvgl/lv_draw/lv_draw_vbasic.h>
#include <lvgl/lv_misc/anim.h>
#include <lvgl/lv_obj/lv_dispi.h>
#include <lvgl/lv_obj/lv_obj.h>
#include <lvgl/lv_obj/lv_refr.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#ifdef LV_IMG_DEF_WALLPAPER
#include "../lv_objx/lv_img.h"
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_pos_child_refr(lv_obj_t * obj, cord_t x_diff, cord_t y_diff);
static void lv_style_refr_core(void * style_p, lv_obj_t * obj);
static void lv_obj_del_child(lv_obj_t * obj);
static bool lv_obj_design(lv_obj_t * obj, const  area_t * mask_p, lv_design_mode_t mode);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * def_scr = NULL;
static lv_obj_t * act_scr = NULL;
static ll_dsc_t scr_ll;

static lv_objs_t lv_objs_def = {.color = COLOR_MAKE(0xa0, 0xc0, 0xe0), .transp = 0};
static lv_objs_t lv_objs_scr = {.color = LV_OBJ_DEF_SCR_COLOR, .transp = 0};
static lv_objs_t lv_objs_transp = {.transp = 1};


#ifdef LV_IMG_DEF_WALLPAPER
LV_IMG_DECLARE(LV_IMG_DEF_WALLPAPER);
#endif

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Init. the 'lv' library.  
 */
void lv_init(void)
{
    /*Clear the screen*/
    area_t scr_area;
    area_set(&scr_area, 0, 0, LV_HOR_RES, LV_VER_RES);
    lv_rfill(&scr_area, NULL, COLOR_BLACK, OPA_COVER);
    
    /*Init. the screen refresh system*/
    lv_refr_init();
    
    /*Init. the animations*/
    anim_init();

    /*Create the default screen*/
    ll_init(&scr_ll, sizeof(lv_obj_t));
#ifdef LV_IMG_DEF_WALLPAPER
    lv_img_create_file("def_wp", LV_IMG_DEF_WALLPAPER);
    def_scr = lv_img_create(NULL, NULL);
    lv_img_set_auto_size(def_scr, false);
    lv_img_set_file(def_scr, "U:/def_wp");
    lv_img_set_upscale(def_scr, true);
#else
    def_scr = lv_obj_create(NULL, NULL);
#endif
    act_scr = def_scr;
    
    /*Refresh the screen*/
    lv_obj_inv(act_scr);
    
#if LV_DISPI_READ_PERIOD != 0
    /*Init the display input handling*/
    lv_dispi_init();
#endif

    /*Initialize the application level*/
#if LV_APP_ENABLE != 0
    lv_app_init();
#endif

}

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object 
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, lv_obj_t * copy)
{
    lv_obj_t * new_obj = NULL;
    /*Create a screen if the parent is NULL*/
    if(parent == NULL) {
        new_obj = ll_ins_head(&scr_ll);
        
        new_obj->par = NULL; /*Screens has no a parent*/
        ll_init(&(new_obj->child_ll), sizeof(lv_obj_t));
        
		/*Set coordinates to full screen size*/
		new_obj->cords.x1 = 0;
		new_obj->cords.y1 = 0;
		new_obj->cords.x2 = LV_HOR_RES - 1;
		new_obj->cords.y2 = LV_VER_RES - 1;
		new_obj->ext_size = 0;

		/*Set appearance*/
		new_obj->style_p = lv_objs_get(LV_OBJS_SCR, NULL);
		new_obj->opa = OPA_COVER;

		/*Set virtual functions*/
		lv_obj_set_signal_f(new_obj, lv_obj_signal);
		lv_obj_set_design_f(new_obj, lv_obj_design);

		/*Set free data*/
		new_obj->free_num = 0;
#if LV_OBJ_FREE_P != 0
        new_obj->free_p = NULL;
#endif

		/*Set attributes*/
		new_obj->click_en = 0;
		new_obj->drag_en = 0;
		new_obj->drag_throw_en = 0;
		new_obj->drag_parent = 0;
		new_obj->style_iso = 0;
		new_obj->hidden = 0;
		new_obj->top_en = 0;
        new_obj->protect = LV_PROTECT_NONE;

		new_obj->ext = NULL;
	 }
    /*parent != NULL create normal obj. on a parent*/
    else
    {   
        new_obj = ll_ins_head(&(parent)->child_ll);

        new_obj->par = parent; /*Set the parent*/
        ll_init(&(new_obj->child_ll), sizeof(lv_obj_t));
        
        /*Set coordinates left top corner of parent*/
        new_obj->cords.x1 = parent->cords.x1;
        new_obj->cords.y1 = parent->cords.y1;
        new_obj->cords.x2 = parent->cords.x1 +
                                   LV_OBJ_DEF_WIDTH;
        new_obj->cords.y2 = parent->cords.y1 +
                                   LV_OBJ_DEF_HEIGHT;
        new_obj->ext_size = 0;

        /*Set appearance*/
        new_obj->style_p = lv_objs_get(LV_OBJS_DEF, NULL);
        new_obj->opa = OPA_COVER;
        
        /*Set virtual functions*/
        lv_obj_set_signal_f(new_obj, lv_obj_signal);
        lv_obj_set_design_f(new_obj, lv_obj_design);

        /*Set free data*/
        new_obj->free_num = 0;
#if LV_OBJ_FREE_P != 0
        new_obj->free_p = NULL;
#endif
        
        /*Set attributes*/
        new_obj->click_en = 1;
        new_obj->drag_en = 0;
        new_obj->drag_throw_en = 0;
        new_obj->drag_parent = 0;
        new_obj->style_iso = 0;
        new_obj->hidden = 0;
        new_obj->top_en = 0;
        new_obj->protect = LV_PROTECT_NONE;
        
        new_obj->ext = NULL;
        
    }

    if(copy != NULL) {
    	area_cpy(&new_obj->cords, &copy->cords);
    	new_obj->ext_size = copy->ext_size;

        new_obj->opa = copy->opa;

        /*Set free data*/
        new_obj->free_num = copy->free_num;
#if LV_OBJ_FREE_P != 0
        new_obj->free_p = copy->free_p;
#endif
    	/*Set attributes*/
        new_obj->click_en = copy->click_en;
        new_obj->drag_en = copy->drag_en;
        new_obj->drag_throw_en = copy->drag_throw_en;
        new_obj->drag_parent = copy->drag_parent;
        new_obj->hidden = copy->hidden;
        new_obj->top_en = copy->top_en;
        new_obj->protect = copy->protect;

        new_obj->style_p = copy->style_p;

        if(copy->style_iso != 0) {
            lv_obj_iso_style(new_obj, dm_get_size(copy->style_p));
        }

    	lv_obj_set_pos(new_obj, lv_obj_get_x(copy), lv_obj_get_y(copy));
    }


    /*Send a signal to the parent to notify it about the new child*/
    if(parent != NULL) {
        parent->signal_f(parent, LV_SIGNAL_CHILD_CHG, new_obj);

        /*Invalidate the area if not screen created*/
        lv_obj_inv(new_obj);
    }
    
    return new_obj;
}

/**
 * Delete 'obj' and all of its children
 * @param obj
 */
void lv_obj_del(lv_obj_t * obj)
{
    lv_obj_inv(obj);
    
    /*Recursively delete the children*/
    lv_obj_t * i;
    lv_obj_t * i_next;
    i = ll_get_head(&(obj->child_ll));
    while(i != NULL) {
        /*Get the next object before delete this*/
        i_next = ll_get_next(&(obj->child_ll), i);
        
        /*Call the recursive del to the child too*/
        lv_obj_del_child(i);
        
        /*Set i to the next node*/
        i = i_next;
    }
    
    /*Remove the animations from this object*/
    anim_del(obj, NULL);

    /*Remove the object from parent's children list*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par == NULL) { /*It is a screen*/
    	ll_rem(&scr_ll, obj);
    } else {
    	ll_rem(&(par->child_ll), obj);
    }

    /* All children deleted.
     * Now clean up the object specific data*/
    obj->signal_f(obj, LV_SIGNAL_CLEANUP, NULL);
    
    /*Delete the base objects*/
    if(obj->ext != NULL)  dm_free(obj->ext);
    if(obj->style_iso != 0) dm_free(obj->style_p);
    dm_free(obj); /*Free the object itself*/
    
    /* Reset all display input (dispi) because 
     * the deleted object can be being pressed*/
    lv_dispi_reset();
    
    /*Send a signal to the parent to notify it about the child delete*/
    if(par != NULL) {
    	par->signal_f(par, LV_SIGNAL_CHILD_CHG, NULL);
    }
}

/**
 * Signal function of the basic object
 * @param obj pointer to an object
 * @param sign signal type
 * @param param parameter for the signal (depends on signal type)
 * @return false: the object become invalid (e.g. deleted) 
 */
bool lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    bool valid = true;

    switch(sign) {
    case LV_SIGNAL_CHILD_CHG:
    	/*Return 'invalid' if the child change  signal is not enabled*/
    	if(lv_obj_is_protected(obj, LV_PROTECT_CHILD_CHG) != false) valid = false;
    	break;
    	default:
    		break;
    }

    return valid;
}

/**
 * Return with a pointer to built-in style and/or copy it to a variable
 * @param style a style name from lv_objs_builtin_t enum
 * @param copy_p copy the style to this variable. (NULL if unused)
 * @return pointer to an lv_objs_t style
 */
lv_objs_t * lv_objs_get(lv_objs_builtin_t style, lv_objs_t * copy_p)
{
	lv_objs_t  *style_p;

	switch(style) {
		case LV_OBJS_DEF:
			style_p = &lv_objs_def;
			break;
		case LV_OBJS_SCR:
			style_p = &lv_objs_scr;
			break;
		case LV_OBJS_TRANSP:
			style_p = &lv_objs_transp;
			break;
		default:
			style_p = NULL;
	}

	if(copy_p != NULL) {
		if(style_p != NULL) memcpy(copy_p, style_p, sizeof(lv_objs_t));
		else memcpy(copy_p, &lv_objs_def, sizeof(lv_objs_t));
	}

	return style_p;
}


/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_inv(lv_obj_t * obj)
{
    /*Invalidate the object only if it belongs to the 'act_scr'*/
    lv_obj_t * act_scr_p = lv_scr_act();
    if(lv_obj_get_scr(obj) == act_scr_p) {
        /*Truncate recursively to the parents*/
        area_t area_trunc;
        lv_obj_t * par = lv_obj_get_parent(obj);
        bool union_ok = true;
        /*Start with the original coordinates*/
        cord_t ext_size = obj->ext_size;
        area_cpy(&area_trunc, &obj->cords);
        area_trunc.x1 -= ext_size;
        area_trunc.y1 -= ext_size;
        area_trunc.x2 += ext_size;
        area_trunc.y2 += ext_size;

        /*Check through all parents*/
        while(par != NULL) {
            union_ok = area_union(&area_trunc, &area_trunc, &par->cords);
            if(union_ok == false) break; /*If no common parts with parent break;*/

            par = lv_obj_get_parent(par);
        }

        if(union_ok != false)  lv_inv_area(&area_trunc);
    }
}


/*=====================
 * Setter functions 
 *====================*/

/*--------------
 * Screen set 
 *--------------*/

/**
 * Load a new screen
 * @param scr pointer to a screen
 */
void lv_scr_load(lv_obj_t * scr)
{
    act_scr = scr;
    
    lv_obj_inv(act_scr);
}

/*--------------------
 * Parent/children set 
 *--------------------*/

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object
 * @param parent pointer to the new parent object
 */
void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent)
{
    lv_obj_inv(obj);
    
    point_t old_pos;
    old_pos.x = lv_obj_get_x(obj);
    old_pos.y = lv_obj_get_y(obj);
    
    ll_chg_list(&obj->par->child_ll, &parent->child_ll, obj);
    obj->par = parent;
    lv_obj_set_pos(obj, old_pos.x, old_pos.y);

    /*Notify the original parent because one of its children is lost*/
    obj->par->signal_f(obj->par, LV_SIGNAL_CHILD_CHG, NULL);

    /*Notify the new parent about the child*/
    parent->signal_f(parent, LV_SIGNAL_CHILD_CHG, obj);

    lv_obj_inv(obj);
}

/*--------------------
 * Coordinate set
 * ------------------*/

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t * obj, cord_t x, cord_t y)
{
    /*Convert x and y to absolute coordinates*/
    lv_obj_t * par = obj->par;
    x = x + par->cords.x1;
    y = y + par->cords.y1;
    
    /*Calculate and set the movement*/
    point_t diff;
    diff.x =  x - obj->cords.x1;
    diff.y =  y - obj->cords.y1;

    /* Do nothing if the position is not changed */
    /* It is very important else recursive positioning can
     * occur without position change*/
    if(diff.x == 0 && diff.y == 0) return;
        
    /*Invalidate the original area*/
    lv_obj_inv(obj);

    /*Save the original coordinates*/
    area_t ori;
    lv_obj_get_cords(obj, &ori);

    obj->cords.x1 += diff.x;
    obj->cords.y1 += diff.y;
    obj->cords.x2 += diff.x;
    obj->cords.y2 += diff.y;
    
    lv_obj_pos_child_refr(obj, diff.x, diff.y);
    
    /*Inform the object about its new coordinates*/
    obj->signal_f(obj, LV_SIGNAL_CORD_CHG, &ori);
    
    /*Send a signal to the parent too*/
    par->signal_f(par, LV_SIGNAL_CHILD_CHG, obj);
    
    /*Invalidate the new area*/
    lv_obj_inv(obj);
}

/**
 * Set relative the position of an object (relative to the parent).
 * The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent. (will be multiplied with LV_DOWNSCALE)
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_pos_us(lv_obj_t * obj, cord_t x, cord_t y)
{
	lv_obj_set_pos(obj, x * LV_DOWNSCALE, y * LV_DOWNSCALE);
}

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, cord_t x)
{
    lv_obj_set_pos(obj, x, lv_obj_get_y(obj));
}

/**
 * Set the x coordinate of a object.
 * The coordinate will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_x_us(lv_obj_t * obj, cord_t x)
{
    lv_obj_set_pos(obj, x * LV_DOWNSCALE, lv_obj_get_y(obj));
}

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, cord_t y)
{
    lv_obj_set_pos(obj, lv_obj_get_x(obj), y);
}

/**
 * Set the y coordinate of a object.
 * The coordinate will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_y_us(lv_obj_t * obj, cord_t y)
{
    lv_obj_set_pos(obj, lv_obj_get_x(obj), y * LV_DOWNSCALE);
}

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width 
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, cord_t w, cord_t h)
{

    /* Do nothing if the size is not changed */
    /* It is very important else recursive resizing can
     * occur without size change*/
	if(lv_obj_get_width(obj) == w && lv_obj_get_height(obj) == h) {
		return;
	}

    /*Invalidate the original area*/
    lv_obj_inv(obj);
    
    /*Save the original coordinates*/
    area_t ori;
    lv_obj_get_cords(obj, &ori);
    
    //Set the length and height
    obj->cords.x2 = obj->cords.x1 + w - 1;
    obj->cords.y2 = obj->cords.y1 + h - 1;


    /*Send a signal to the object with its new coordinates*/
    obj->signal_f(obj, LV_SIGNAL_CORD_CHG, &ori);
    
    /*Send a signal to the parent too*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par != NULL) par->signal_f(par, LV_SIGNAL_CHILD_CHG, obj);
    
    /*Invalidate the new area*/
    lv_obj_inv(obj);
}

/**
 * Set the size of an object. The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_size_us(lv_obj_t * obj, cord_t w, cord_t h)
{
	lv_obj_set_size(obj, w * LV_DOWNSCALE, h * LV_DOWNSCALE);
}

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, cord_t w)
{
    lv_obj_set_size(obj, w, lv_obj_get_height(obj));
}

/**
 * Set the width of an object.  The width will be upscaled to compensate LV_DOWNSCALE
 * @param obj pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_width_us(lv_obj_t * obj, cord_t w)
{
    lv_obj_set_size(obj, w * LV_DOWNSCALE, lv_obj_get_height(obj));
}

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, cord_t h)
{
    lv_obj_set_size(obj, lv_obj_get_width(obj), h);
}

/**
 * Set the height of an object.  The height will be upscaled to compensate LV_DOWNSCALE
 * @param obj pointer to an object
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_height_us(lv_obj_t * obj, cord_t h)
{
    lv_obj_set_size(obj, lv_obj_get_width(obj), h * LV_DOWNSCALE);
}

/**
 * Align an object to an other object. 
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align(lv_obj_t * obj,lv_obj_t * base, lv_align_t align, cord_t x_mod, cord_t y_mod)
{
    cord_t new_x = lv_obj_get_x(obj);
    cord_t new_y = lv_obj_get_y(obj);

    if(base == NULL) {
        base = lv_obj_get_parent(obj);
    }
    
    switch(align)
    {
        case LV_ALIGN_CENTER:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
            break;

        case LV_ALIGN_IN_TOP_LEFT:
            new_x = 0;
            new_y = 0;
        	break;
        case LV_ALIGN_IN_TOP_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = 0;
            break;

        case LV_ALIGN_IN_TOP_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = 0;
            break;

        case LV_ALIGN_IN_BOTTOM_LEFT:
            new_x = 0;
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
        	break;
        case LV_ALIGN_IN_BOTTOM_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;

        case LV_ALIGN_IN_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
            break;

        case LV_ALIGN_IN_LEFT_MID:
            new_x = 0;
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
        	break;

        case LV_ALIGN_IN_RIGHT_MID:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
        	break;

        case LV_ALIGN_OUT_TOP_LEFT:
            new_x = 0;
            new_y = -lv_obj_get_height(obj);
        	break;

        case LV_ALIGN_OUT_TOP_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = - lv_obj_get_height(obj);
        	break;

        case LV_ALIGN_OUT_TOP_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = - lv_obj_get_height(obj);
        	break;

        case LV_ALIGN_OUT_BOTTOM_LEFT:
            new_x = 0;
            new_y = lv_obj_get_height(base);
        	break;

        case LV_ALIGN_OUT_BOTTOM_MID:
            new_x = lv_obj_get_width(base) / 2 - lv_obj_get_width(obj) / 2;
            new_y = lv_obj_get_height(base);
        	break;

        case LV_ALIGN_OUT_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base) - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base);
        	break;

        case LV_ALIGN_OUT_LEFT_TOP:
            new_x = - lv_obj_get_width(obj);
            new_y = 0;
        	break;

        case LV_ALIGN_OUT_LEFT_MID:
            new_x = - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
        	break;

        case LV_ALIGN_OUT_LEFT_BOTTOM:
            new_x = - lv_obj_get_width(obj);
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
        	break;

        case LV_ALIGN_OUT_RIGHT_TOP:
            new_x = lv_obj_get_width(base);
            new_y = 0;
        	break;

        case LV_ALIGN_OUT_RIGHT_MID:
            new_x = lv_obj_get_width(base);
            new_y = lv_obj_get_height(base) / 2 - lv_obj_get_height(obj) / 2;
        	break;

        case LV_ALIGN_OUT_RIGHT_BOTTOM:
            new_x = lv_obj_get_width(base);
            new_y = lv_obj_get_height(base) - lv_obj_get_height(obj);
        	break;
    }

    /*Bring together the coordination system of base and obj*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    cord_t base_abs_x = base->cords.x1;
    cord_t base_abs_y = base->cords.y1;
    cord_t par_abs_x = par->cords.x1;
    cord_t par_abs_y = par->cords.y1;
    new_x += x_mod + base_abs_x;
    new_y += y_mod + base_abs_y;
	new_x -= par_abs_x;
	new_y -= par_abs_y;

	lv_obj_set_pos(obj, new_x, new_y);
}


/**
 * Align an object to an other object. The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 * @param y_mod y coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_align_us(lv_obj_t * obj,lv_obj_t * base, lv_align_t align, cord_t x_mod, cord_t y_mod)
{
	lv_obj_align(obj, base, align, x_mod * LV_DOWNSCALE, y_mod * LV_DOWNSCALE);
}

/**
 * Set the extended size of an object
 * @param obj pointer to an object
 * @param ext_size the extended size
 */
void lv_obj_set_ext_size(lv_obj_t * obj, cord_t ext_size)
{
	obj->ext_size = ext_size;

	lv_obj_inv(obj);
}

/*---------------------
 * Appearance set 
 *--------------------*/

/**
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
void lv_obj_set_style(lv_obj_t * obj, void * style)
{
    lv_obj_inv(obj);

	if(obj->style_iso != 0) {
		dm_free(obj->style_p);
		obj->style_iso = 0;
	}
    obj->style_p = style;

    /*Send a style change signal to the object*/
    lv_obj_refr_style(obj);

    lv_obj_inv(obj);
}

/**
 * Isolate the style of an object. In other words a unique style will be created
 * for this object which can be freely modified independently from the style of the
 * other objects.
 */
void * lv_obj_iso_style(lv_obj_t * obj, uint32_t style_size)
{
	if(obj->style_iso != 0) return obj->style_p;

	void * ori_style_p = lv_obj_get_style(obj);
	void * iso_style = dm_alloc(style_size);
	dm_assert(iso_style);
	memcpy(iso_style, ori_style_p, style_size);

	obj->style_iso = 1;
	obj->style_p = iso_style;

	lv_obj_refr_style(obj);

	return obj->style_p;
}

/**
 * Set the opacity of an object
 * @param obj pointer to an object
 * @param opa 0 (transparent) .. 255(fully cover)
 */
void lv_obj_set_opa(lv_obj_t * obj, uint8_t opa)
{
    obj->opa = opa;
    
    lv_obj_inv(obj);
}

/**
 * Set the opacity of an object and all of its children
 * @param obj pointer to an object
 * @param opa 0 (transparent) .. 255(fully cover)
 */
void lv_obj_set_opar(lv_obj_t * obj, uint8_t opa)
{
    lv_obj_t * i;
    
    LL_READ(obj->child_ll, i) {
        lv_obj_set_opar(i, opa);
    }
    
    /*Set the opacity is the object is not protected*/
    if(lv_obj_is_protected(obj, LV_PROTECT_OPA) == false) obj->opa = opa;
    
    lv_obj_inv(obj);
}


/**
 * Notify an object about its style is modified
 * @param obj pointer to an object
 */
void lv_obj_refr_style(lv_obj_t * obj)
{
    lv_obj_inv(obj);
    obj->signal_f(obj, LV_SIGNAL_STYLE_CHG, NULL);
    lv_obj_inv(obj);

}

/**
 * Notify all object if a style is modified
 * @param style pinter to a style. Only objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_style_refr_all(void * style)
{
    lv_obj_t * i;
    LL_READ(scr_ll, i) {
        lv_style_refr_core(style, i);
    }
}


/*-----------------
 * Attribute set
 *----------------*/

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj pointer to an object
 * @param en true: hide the object
 */
void lv_obj_set_hidden(lv_obj_t * obj, bool en)
{
    obj->hidden = en == false ? 0 : 1;
    
    lv_obj_t * par = lv_obj_get_parent(obj);
    par->signal_f(par, LV_SIGNAL_CHILD_CHG, obj);

    lv_obj_inv(obj);
}

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void lv_obj_set_click(lv_obj_t * obj, bool en)
{
    obj->click_en = (en == true ? 1 : 0);
    
}

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void lv_obj_set_top(lv_obj_t * obj, bool en)
{
    obj->top_en= (en == true ? 1 : 0);
}

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void lv_obj_set_drag(lv_obj_t * obj, bool en)
{
    obj->drag_en = (en == true ? 1 : 0);
}

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void lv_obj_set_drag_throw(lv_obj_t * obj, bool en)
{
    obj->drag_throw_en = (en == true ? 1 : 0);
}

/**
 * Enable to use parent for drag related operations. 
 * If trying to drag the object the parent will be moved instead 
 * @param obj pointer to an object
 * @param en true: enable the 'drag parent' for the object
 */
void lv_obj_set_drag_parent(lv_obj_t * obj, bool en)
{
    obj->drag_parent = (en == true ? 1 : 0);
}

/**
 * Set a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from lv_obj_prot_t
 */
void lv_obj_set_protect(lv_obj_t * obj, uint8_t prot)
{
    obj->protect |= prot;
}

/**
 * Clear a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from lv_obj_prot_t
 */
void lv_obj_clr_protect(lv_obj_t * obj, uint8_t prot)
{
    prot = (~prot) & 0xFF;
    obj->protect &= prot;
}

/**
 * Set the signal function of an object. 
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param fp the new signal function
 */
void lv_obj_set_signal_f(lv_obj_t * obj, lv_signal_f_t fp)
{
    obj->signal_f = fp;
}

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param fp the new design function
 */
void lv_obj_set_design_f(lv_obj_t * obj, lv_design_f_t fp)
{
    obj->design_f = fp;
}

/*----------------
 * Other set
 *--------------*/

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return Normal pointer to the allocated ext
 */
void * lv_obj_alloc_ext(lv_obj_t * obj, uint16_t ext_size)
{
    obj->ext = dm_realloc(obj->ext, ext_size); 
    
   return (void*)obj->ext;
}

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void lv_obj_refr_ext_size(lv_obj_t * obj)
{
	obj->ext_size = 0;
	obj->signal_f(obj, LV_SIGNAL_REFR_EXT_SIZE, NULL);

	lv_obj_inv(obj);
}

/**
 * Set an application specific number for an object.
 * It can help to identify objects in the application. 
 * @param obj pointer to an object
 * @param free_num the new free number
 */
void lv_obj_set_free_num(lv_obj_t * obj, uint8_t free_num)
{
    obj->free_num = free_num;
}

#if LV_OBJ_FREE_P != 0
/**
 * Set an application specific  pointer for an object.
 * It can help to identify objects in the application.
 * @param obj pointer to an object
 * @param free_p the new free pinter
 */
void lv_obj_set_free_p(lv_obj_t * obj, void * free_p)
{
    obj->free_p = free_p;
}
#endif

/**
 * Animate an object
 * @param obj pointer to an object to animate
 * @param type type of animation from 'lv_anim_builtin_t'. 'OR' it with ANIM_IN or ANIM_OUT
 * @param time time of animation in milliseconds
 * @param delay delay before the animation in milliseconds
 * @param cb a function to call when the animation is ready
 */
void lv_obj_anim(lv_obj_t * obj, lv_anim_builtin_t type, uint16_t time, uint16_t delay, void (*cb) (lv_obj_t *))
{
	lv_obj_t * par = lv_obj_get_parent(obj);

	/*Get the direction*/
	bool out = (type & ANIM_DIR_MASK) == ANIM_IN ? false : true;
	type = type & (~ANIM_DIR_MASK);

	anim_t a;
	a.var = obj;
	a.time = time;
	a.act_time = (int32_t)-delay;
	a.end_cb = (void(*)(void*))cb;
	a.path = anim_get_path(ANIM_PATH_LIN);
	a.playback_pause = 0;
	a.repeat_pause = 0;
	a.playback = 0;
	a.repeat = 0;

	/*Init to ANIM_IN*/
	switch(type) {
		case LV_ANIM_FLOAT_LEFT:
			a.fp = (void(*)(void *, int32_t))lv_obj_set_x;
			a.start = -lv_obj_get_width(obj);
			a.end = lv_obj_get_x(obj);
			break;
		case LV_ANIM_FLOAT_RIGHT:
			a.fp = (void(*)(void *, int32_t))lv_obj_set_x;
			a.start = lv_obj_get_width(par);
			a.end = lv_obj_get_x(obj);
			break;
		case LV_ANIM_FLOAT_TOP:
			a.fp = (void(*)(void * , int32_t))lv_obj_set_y;
			a.start = -lv_obj_get_height(obj);
			a.end = lv_obj_get_y(obj);
			break;
		case LV_ANIM_FLOAT_BOTTOM:
			a.fp = (void(*)(void * , int32_t))lv_obj_set_y;
			a.start = lv_obj_get_height(par);
			a.end = lv_obj_get_y(obj);
			break;
		case LV_ANIM_FADE:
			a.fp = (void(*)(void * , int32_t))lv_obj_set_opar;
			a.start = OPA_TRANSP;
			a.end = OPA_COVER;
			break;
		case LV_ANIM_GROW_H:
			a.fp = (void(*)(void * , int32_t))lv_obj_set_width;
			a.start = 0;
			a.end = lv_obj_get_width(obj);
			break;
		case LV_ANIM_GROW_V:
			a.fp = (void(*)(void * , int32_t))lv_obj_set_height;
			a.start = 0;
			a.end = lv_obj_get_height(obj);
			break;
        case LV_ANIM_NONE:
            a.fp = NULL;
            a.start = 0;
            a.end = 0;
            break;
		default:
			break;
	}

	/*Swap start and end in case of ANIM OUT*/
	if(out != false) {
		int32_t tmp = a.start;
		a.start = a.end;
		a.end = tmp;
	}

	anim_create(&a);
}
/*=======================
 * Getter functions
 *======================*/

/*------------------
 * Screen get
 *-----------------*/

/**
 * Return with the actual screen
 * @return pointer to to the actual screen object
 */
lv_obj_t * lv_scr_act(void)
{
    return act_scr;
}

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_scr(lv_obj_t * obj)
{
    lv_obj_t * par = obj;
    lv_obj_t * act_p;
    
    do {
        act_p = par;
        par = lv_obj_get_parent(act_p);    
    }
    while(par != NULL);
    
    return act_p;
}


/*---------------------
 * Parent/children get
 *--------------------*/

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj' 
 */
lv_obj_t * lv_obj_get_parent(lv_obj_t * obj)
{
    return obj->par;
}

/**
 * Iterate through the children of an object
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(lv_obj_t * obj, lv_obj_t * child)
{
	if(child == NULL) {
		return ll_get_head(&obj->child_ll);
	} else {
		return ll_get_next(&obj->child_ll, child);
	}

	return NULL;
}

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_get_child_num(lv_obj_t * obj)
{
	lv_obj_t * i;
	uint16_t cnt = 0;

	LL_READ(obj->child_ll, i) cnt++;

	return cnt;
}

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates 
 */
void lv_obj_get_cords(lv_obj_t * obj, area_t * cords_p)
{
    area_cpy(cords_p, &obj->cords);
}


/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent 
 */
cord_t lv_obj_get_x(lv_obj_t * obj)
{
    cord_t rel_x;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    rel_x = obj->cords.x1 - parent->cords.x1;
    
    return rel_x;
}

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent 
 */
cord_t lv_obj_get_y(lv_obj_t * obj)
{
    cord_t rel_y;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    rel_y = obj->cords.y1 - parent->cords.y1;
    
    return rel_y;
}

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
cord_t lv_obj_get_width(lv_obj_t * obj)
{
    return area_get_width(&obj->cords);
}

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
cord_t lv_obj_get_height(lv_obj_t * obj)
{
    return area_get_height(&obj->cords);
}

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
cord_t lv_obj_getext_size(lv_obj_t * obj)
{
    return obj->ext_size;
}

/*-----------------
 * Appearance get
 *---------------*/

/**
 * Get the style pointer of an object
 * @param obj pointer to an object
 * @return pointer to a style
 */
void * lv_obj_get_style(lv_obj_t * obj)
{
    return obj->style_p;
}

/**
 * Get the opacity of an object
 * @param obj pointer to an object
 * @return 0 (transparent) .. 255 (fully cover)
 */
opa_t lv_obj_get_opa(lv_obj_t * obj)
{
    return obj->opa;
}

/*-----------------
 * Attribute get
 *----------------*/

/**
 * Get the hidden attribute of an object
 * @param obj pointer to an object
 * @return true: the object is hidden
 */
bool lv_obj_get_hidden(lv_obj_t * obj)
{
    return obj->hidden == 0 ? false : true;
}

/**
 * Get the click enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is clickable
 */
bool lv_obj_get_click(lv_obj_t * obj)
{
    return obj->click_en == 0 ? false : true;;
}

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feture is enabled
 */
bool lv_obj_get_top(lv_obj_t * obj)
{
    return obj->top_en == 0 ? false : true;;
}

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool lv_obj_get_drag(lv_obj_t * obj)
{
    return obj->drag_en == 0 ? false : true;   
}

/**
 * Get the drag thow enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool lv_obj_get_drag_throw(lv_obj_t * obj)
{
    return obj->drag_throw_en == 0 ? false : true;
}

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_drag_parent(lv_obj_t * obj)
{
    return obj->drag_parent == 0 ? false : true;
}

/**
 * Get the style isolation attribute of an object
 * @param obj pointer to an object
 * @return pointer to a style
 */
bool lv_obj_get_style_iso(lv_obj_t * obj)
{
    return obj->style_iso == 0 ? false : true;
}

/**
 * Get the protect field of an object
 * @param obj pointer to an object
 * @return protect field ('OR'ed values of lv_obj_prot_t)
 */
uint8_t lv_obj_get_protect(lv_obj_t * obj)
{
    return obj->protect ;
}

/**
 * Check at least one bit of a given protect bitfield is set
 * @param obj pointer to an object
 * @param prot protect bits to test ('OR'ed values of lv_obj_prot_t)
 * @return false: none of the given bits are set, true: at least one bit is set
 */
bool lv_obj_is_protected(lv_obj_t * obj, uint8_t prot)
{
    return (obj->protect & prot) == 0 ? false : true ;
}

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
lv_signal_f_t   lv_obj_get_signal_f(lv_obj_t * obj)
{
    return obj->signal_f;
}

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
lv_design_f_t lv_obj_get_design_f(lv_obj_t * obj)
{
    return obj->design_f;
}


/*------------------
 * Other get
 *-----------------*/

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * lv_obj_get_ext(lv_obj_t * obj)
{
   return obj->ext;
}


/**
 * Get the free number
 * @param obj pointer to an object
 * @return the free number
 */
uint8_t lv_obj_get_free_num(lv_obj_t * obj)
{
    return obj->free_num;
}

#if LV_OBJ_FREE_P != 0
/**
 * Get the free pointer
 * @param obj pointer to an object
 * @return the free pointer
 */
void * lv_obj_get_free_p(lv_obj_t * obj)
{
    return obj->free_p;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the base objects.
 * @param obj pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'        
 */
static bool lv_obj_design(lv_obj_t * obj, const  area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        bool cover;
    	cover = area_is_in(mask_p, &obj->cords);
        return cover;
    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		lv_objs_t * objs_p = lv_obj_get_style(obj);

		opa_t opa = lv_obj_get_opa(obj);
		color_t color = objs_p->color;

		/*Simply draw a rectangle*/
#if LV_VDB_SIZE == 0
		lv_rfill(&obj->cords, mask_p, color, opa);
#else
		lv_vfill(&obj->cords, mask_p, color, opa);
#endif
    }
    return true;
}

/**
 * Reposition the children of an object. (Called recursively)
 * @param obj pointer to an object which children will be repositioned
 * @param x_diff x coordinate shift
 * @param y_diff y coordinate shift
 */
static void lv_obj_pos_child_refr(lv_obj_t * obj, cord_t x_diff, cord_t y_diff)
{
    lv_obj_t * i;   
    LL_READ(obj->child_ll, i) {
        i->cords.x1 += x_diff;
        i->cords.y1 += y_diff;
        i->cords.x2 += x_diff;
        i->cords.y2 += y_diff;
        
        lv_obj_pos_child_refr(i, x_diff, y_diff);
    }
}

/**
 * Refresh the style of all children of an object. (Called recursively)
 * @param style_p refresh objects only with this style. (ignore is if NULL)
 * @param obj pointer to an object
 */
static void lv_style_refr_core(void * style_p, lv_obj_t * obj)
{
    lv_obj_t * i;
    LL_READ(obj->child_ll, i) {
        if(i->style_p == style_p || style_p == NULL) {
            i->signal_f(i, LV_SIGNAL_STYLE_CHG, NULL);
            lv_obj_inv(i);
        }
        
        lv_style_refr_core(style_p, i);
    }
}

/**
 * Called by 'lv_obj_del' to delete the children objects
 * @param obj pointer to an object (all of its children will be deleted)
 */
static void lv_obj_del_child(lv_obj_t * obj)
{
   lv_obj_t * i;
   lv_obj_t * i_next;
   i = ll_get_head(&(obj->child_ll));
   while(i != NULL) {
       /*Get the next object before delete this*/
       i_next = ll_get_next(&(obj->child_ll), i);

       /*Call the recursive del to the child too*/
       lv_obj_del_child(i);

       /*Set i to the next node*/
       i = i_next;
   }

   /*Remove the animations from this object*/
   anim_del(obj, NULL);

   /*Remove the object from parent's children list*/
   lv_obj_t * par = lv_obj_get_parent(obj);

   ll_rem(&(par->child_ll), obj);

   /* All children deleted.
    * Now clean up the object specific data*/
   obj->signal_f(obj, LV_SIGNAL_CLEANUP, NULL);

   /*Delete the base objects*/
   if(obj->ext != NULL)  dm_free(obj->ext);
   if(obj->style_iso != 0) dm_free(obj->style_p);
   dm_free(obj); /*Free the object itself*/

}


