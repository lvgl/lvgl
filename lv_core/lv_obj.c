/**
 * @file lv_base_obj.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "lv_indev.h"
#include "lv_refr.h"
#include "lv_group.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_draw/lv_draw.h"
#include "../lv_draw/lv_draw_rbasic.h"
#include "../lv_misc/lv_anim.h"
#include "../lv_misc/lv_task.h"
#include "../lv_misc/lv_fs.h"
#include "../lv_misc/lv_ufs.h"
#include <stdint.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#define LV_OBJ_DEF_WIDTH  (LV_DPI)
#define LV_OBJ_DEF_HEIGHT  (2 * LV_DPI / 3)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void refresh_childen_position(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff);
static void report_style_mod_core(void * style_p, lv_obj_t * obj);
static void refresh_childen_style(lv_obj_t * obj);
static void delete_children(lv_obj_t * obj);
static bool lv_obj_design(lv_obj_t * obj, const  lv_area_t * mask_p, lv_design_mode_t mode);
static lv_res_t lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * def_scr = NULL;
static lv_obj_t * act_scr = NULL;
static lv_obj_t * top_layer = NULL;
static lv_obj_t * sys_layer = NULL;
static lv_ll_t scr_ll;                 /*Linked list of screens*/

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
    /*Initialize the lv_misc modules*/
    lv_mem_init();
    lv_task_init();

#if USE_LV_FILESYSTEM
    lv_fs_init();
    lv_ufs_init();
#endif

    lv_font_init();
#if USE_LV_ANIMATION
    lv_anim_init();
#endif

    /*Init. the sstyles*/
    lv_style_init();
    
    /*Init. the screen refresh system*/
    lv_refr_init();

    /*Create the default screen*/
    lv_ll_init(&scr_ll, sizeof(lv_obj_t));
    def_scr = lv_obj_create(NULL, NULL);

    act_scr = def_scr;
    
    top_layer = lv_obj_create(NULL, NULL);
    lv_obj_set_style(top_layer, &lv_style_transp_fit);

    sys_layer = lv_obj_create(NULL, NULL);
    lv_obj_set_style(sys_layer, &lv_style_transp_fit);

    /*Refresh the screen*/
    lv_obj_invalidate(act_scr);
    
#if LV_INDEV_READ_PERIOD != 0
    /*Init the input device handling*/
    lv_indev_init();
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
        new_obj = lv_ll_ins_head(&scr_ll);
        
        new_obj->par = NULL; /*Screens has no a parent*/
        lv_ll_init(&(new_obj->child_ll), sizeof(lv_obj_t));
        
		/*Set coordinates to full screen size*/
		new_obj->coords.x1 = 0;
		new_obj->coords.y1 = 0;
		new_obj->coords.x2 = LV_HOR_RES - 1;
		new_obj->coords.y2 = LV_VER_RES - 1;
		new_obj->ext_size = 0;

        /*Set the default styles*/
        lv_theme_t *th = lv_theme_get_current();
        if(th) {
            new_obj->style_p = th->bg;
        } else {
            new_obj->style_p = &lv_style_scr;
        }
		/*Set virtual functions*/
		lv_obj_set_signal_func(new_obj, lv_obj_signal);
		lv_obj_set_design_func(new_obj, lv_obj_design);

		/*Set free data*/
#ifdef LV_OBJ_FREE_NUM_TYPE
		new_obj->free_num = 0;
#endif

#if LV_OBJ_FREE_PTR != 0
        new_obj->free_ptr = NULL;
#endif

#if USE_LV_GROUP
        new_obj->group_p = NULL;
#endif
		/*Set attributes*/
		new_obj->click = 0;
		new_obj->drag = 0;
		new_obj->drag_throw = 0;
		new_obj->drag_parent = 0;
		new_obj->hidden = 0;
		new_obj->top = 0;
        new_obj->protect = LV_PROTECT_NONE;

		new_obj->ext_attr = NULL;
	 }
    /*parent != NULL create normal obj. on a parent*/
    else
    {   
        new_obj = lv_ll_ins_head(&(parent)->child_ll);

        new_obj->par = parent; /*Set the parent*/
        lv_ll_init(&(new_obj->child_ll), sizeof(lv_obj_t));
        
        /*Set coordinates left top corner of parent*/
        new_obj->coords.x1 = parent->coords.x1;
        new_obj->coords.y1 = parent->coords.y1;
        new_obj->coords.x2 = parent->coords.x1 +
                                   LV_OBJ_DEF_WIDTH;
        new_obj->coords.y2 = parent->coords.y1 +
                                   LV_OBJ_DEF_HEIGHT;
        new_obj->ext_size = 0;

        /*Set appearance*/
        new_obj->style_p = &lv_style_plain_color;
        
        /*Set virtual functions*/
        lv_obj_set_signal_func(new_obj, lv_obj_signal);
        lv_obj_set_design_func(new_obj, lv_obj_design);

        /*Set free data*/
#ifdef LV_OBJ_FREE_NUM_TYPE
        new_obj->free_num = 0;
#endif
#if LV_OBJ_FREE_PTR != 0
        new_obj->free_ptr = NULL;
#endif
#if USE_LV_GROUP
        new_obj->group_p = NULL;
#endif
        
        /*Set attributes*/
        new_obj->click = 1;
        new_obj->drag = 0;
        new_obj->drag_throw = 0;
        new_obj->drag_parent = 0;
        new_obj->hidden = 0;
        new_obj->top = 0;
        new_obj->protect = LV_PROTECT_NONE;
        
        new_obj->ext_attr = NULL;
    }

    if(copy != NULL) {
    	lv_area_copy(&new_obj->coords, &copy->coords);
    	new_obj->ext_size = copy->ext_size;

        /*Set free data*/
#ifdef LV_OBJ_FREE_NUM_TYPE
        new_obj->free_num = copy->free_num;
#endif
#if LV_OBJ_FREE_PTR != 0
        new_obj->free_ptr = copy->free_ptr;
#endif
    	/*Set attributes*/
        new_obj->click = copy->click;
        new_obj->drag = copy->drag;
        new_obj->drag_throw = copy->drag_throw;
        new_obj->drag_parent = copy->drag_parent;
        new_obj->hidden = copy->hidden;
        new_obj->top = copy->top;
        new_obj->protect = copy->protect;

        new_obj->style_p = copy->style_p;

#if USE_LV_GROUP
        /*Add to the same group*/
        if(copy->group_p != NULL) {
            lv_group_add_obj(copy->group_p, new_obj);
        }
#endif

    	lv_obj_set_pos(new_obj, lv_obj_get_x(copy), lv_obj_get_y(copy));
    }


    /*Send a signal to the parent to notify it about the new child*/
    if(parent != NULL) {
        parent->signal_func(parent, LV_SIGNAL_CHILD_CHG, new_obj);

        /*Invalidate the area if not screen created*/
        lv_obj_invalidate(new_obj);
    }
    
    return new_obj;
}

/**
 * Delete 'obj' and all of its children
 * @param obj pointer to an object to delete
 * @return LV_RES_INV because the object is deleted
 */
lv_res_t lv_obj_del(lv_obj_t * obj)
{
    lv_obj_invalidate(obj);
    
    /*Recursively delete the children*/
    lv_obj_t * i;
    lv_obj_t * i_next;
    i = lv_ll_get_head(&(obj->child_ll));
    while(i != NULL) {
        /*Get the next object before delete this*/
        i_next = lv_ll_get_next(&(obj->child_ll), i);
        
        /*Call the recursive del to the child too*/
        delete_children(i);
        
        /*Set i to the next node*/
        i = i_next;
    }
#if USE_LV_ANIMATION
    /*Remove the animations from this object*/
    lv_anim_del(obj, NULL);
#endif
    
    /*Delete from the group*/
 #if USE_LV_GROUP
    if(obj->group_p != NULL) lv_group_remove_obj(obj);
 #endif

    /* Reset all input devices if
     * the currently pressed object is deleted*/
    lv_indev_t * indev = lv_indev_next(NULL);
    while(indev) {
        if(indev->proc.act_obj == obj || indev->proc.last_obj == obj) {
            lv_indev_reset(indev);
        }
        indev = lv_indev_next(indev);
    }

    /*Remove the object from parent's children list*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par == NULL) { /*It is a screen*/
    	lv_ll_rem(&scr_ll, obj);
    } else {
    	lv_ll_rem(&(par->child_ll), obj);
    }

    /* All children deleted.
     * Now clean up the object specific data*/
    obj->signal_func(obj, LV_SIGNAL_CLEANUP, NULL);

    /*Delete the base objects*/
    if(obj->ext_attr != NULL)  lv_mem_free(obj->ext_attr);
    lv_mem_free(obj); /*Free the object itself*/

    /*Send a signal to the parent to notify it about the child delete*/
    if(par != NULL) {
    	par->signal_func(par, LV_SIGNAL_CHILD_CHG, NULL);
    }

    return LV_RES_INV;
}

/**
 * Delete all children of an object
 * @param obj pointer to an object
 */
void lv_obj_clean(lv_obj_t *obj)
{
    lv_obj_t *child = lv_obj_get_child(obj, NULL);

    while(child) {
        lv_obj_del(child);
        child = lv_obj_get_child(obj, child);
    }
}

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(lv_obj_t * obj)
{
	if(lv_obj_get_hidden(obj)) return;

    /*Invalidate the object only if it belongs to the 'act_scr'*/
    lv_obj_t * obj_scr = lv_obj_get_screen(obj);
    if(obj_scr == lv_scr_act() ||
       obj_scr == lv_layer_top() ||
       obj_scr == lv_layer_sys())
    {
        /*Truncate recursively to the parents*/
        lv_area_t area_trunc;
        lv_obj_t * par = lv_obj_get_parent(obj);
        bool union_ok = true;
        /*Start with the original coordinates*/
        lv_coord_t ext_size = obj->ext_size;
        lv_area_copy(&area_trunc, &obj->coords);
        area_trunc.x1 -= ext_size;
        area_trunc.y1 -= ext_size;
        area_trunc.x2 += ext_size;
        area_trunc.y2 += ext_size;

        /*Check through all parents*/
        while(par != NULL) {
            union_ok = lv_area_union(&area_trunc, &area_trunc, &par->coords);
            if(union_ok == false) break; 		/*If no common parts with parent break;*/
        	if(lv_obj_get_hidden(par)) return;	/*If the parent is hidden then the child is hidden and won't be drawn*/

            par = lv_obj_get_parent(par);
        }

        if(union_ok != false) lv_inv_area(&area_trunc);
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
    
    lv_obj_invalidate(act_scr);
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
    lv_obj_invalidate(obj);
    
    lv_point_t old_pos;
    old_pos.x = lv_obj_get_x(obj);
    old_pos.y = lv_obj_get_y(obj);
    
    lv_obj_t * old_par = obj->par;

    lv_ll_chg_list(&obj->par->child_ll, &parent->child_ll, obj);
    obj->par = parent;
    lv_obj_set_pos(obj, old_pos.x, old_pos.y);

    /*Notify the original parent because one of its children is lost*/
    old_par->signal_func(old_par, LV_SIGNAL_CHILD_CHG, NULL);

    /*Notify the new parent about the child*/
    parent->signal_func(parent, LV_SIGNAL_CHILD_CHG, obj);

    lv_obj_invalidate(obj);
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
void lv_obj_set_pos(lv_obj_t * obj, lv_coord_t x, lv_coord_t y)
{
    /*Convert x and y to absolute coordinates*/
    lv_obj_t * par = obj->par;
    x = x + par->coords.x1;
    y = y + par->coords.y1;
    
    /*Calculate and set the movement*/
    lv_point_t diff;
    diff.x =  x - obj->coords.x1;
    diff.y =  y - obj->coords.y1;

    /* Do nothing if the position is not changed */
    /* It is very important else recursive positioning can
     * occur without position change*/
    if(diff.x == 0 && diff.y == 0) return;
        
    /*Invalidate the original area*/
    lv_obj_invalidate(obj);

    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);

    obj->coords.x1 += diff.x;
    obj->coords.y1 += diff.y;
    obj->coords.x2 += diff.x;
    obj->coords.y2 += diff.y;
    
    refresh_childen_position(obj, diff.x, diff.y);
    
    /*Inform the object about its new coordinates*/
    obj->signal_func(obj, LV_SIGNAL_CORD_CHG, &ori);
    
    /*Send a signal to the parent too*/
    par->signal_func(par, LV_SIGNAL_CHILD_CHG, obj);
    
    /*Invalidate the new area*/
    lv_obj_invalidate(obj);
}


/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, lv_coord_t x)
{
    lv_obj_set_pos(obj, x, lv_obj_get_y(obj));
}


/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, lv_coord_t y)
{
    lv_obj_set_pos(obj, lv_obj_get_x(obj), y);
}

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width 
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, lv_coord_t w, lv_coord_t h)
{

    /* Do nothing if the size is not changed */
    /* It is very important else recursive resizing can
     * occur without size change*/
	if(lv_obj_get_width(obj) == w && lv_obj_get_height(obj) == h) {
		return;
	}

    /*Invalidate the original area*/
    lv_obj_invalidate(obj);
    
    /*Save the original coordinates*/
    lv_area_t ori;
    lv_obj_get_coords(obj, &ori);
    
    //Set the length and height
    obj->coords.x2 = obj->coords.x1 + w - 1;
    obj->coords.y2 = obj->coords.y1 + h - 1;


    /*Send a signal to the object with its new coordinates*/
    obj->signal_func(obj, LV_SIGNAL_CORD_CHG, &ori);
    
    /*Send a signal to the parent too*/
    lv_obj_t * par = lv_obj_get_parent(obj);
    if(par != NULL) par->signal_func(par, LV_SIGNAL_CHILD_CHG, obj);
    
    /*Invalidate the new area*/
    lv_obj_invalidate(obj);
}

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, lv_coord_t w)
{
    lv_obj_set_size(obj, w, lv_obj_get_height(obj));
}

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, lv_coord_t h)
{
    lv_obj_set_size(obj, lv_obj_get_width(obj), h);
}

/**
 * Align an object to an other object. 
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align(lv_obj_t * obj,lv_obj_t * base, lv_align_t align, lv_coord_t x_mod, lv_coord_t y_mod)
{
    lv_coord_t new_x = lv_obj_get_x(obj);
    lv_coord_t new_y = lv_obj_get_y(obj);

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
    lv_coord_t base_abs_x = base->coords.x1;
    lv_coord_t base_abs_y = base->coords.y1;
    lv_coord_t par_abs_x = par->coords.x1;
    lv_coord_t par_abs_y = par->coords.y1;
    new_x += x_mod + base_abs_x;
    new_y += y_mod + base_abs_y;
	new_x -= par_abs_x;
	new_y -= par_abs_y;

	lv_obj_set_pos(obj, new_x, new_y);
}

/*---------------------
 * Appearance set 
 *--------------------*/

/**
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
void lv_obj_set_style(lv_obj_t * obj, lv_style_t * style)
{
    obj->style_p = style;

    /*Send a signal about style change to every children with NULL style*/
    refresh_childen_style(obj);

    /*Notify the object about the style change too*/
    lv_obj_refresh_style(obj);

}

/**
 * Notify an object about its style is modified
 * @param obj pointer to an object
 */
void lv_obj_refresh_style(lv_obj_t * obj)
{
    lv_obj_invalidate(obj);
    obj->signal_func(obj, LV_SIGNAL_STYLE_CHG, NULL);
    lv_obj_invalidate(obj);

}

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_obj_report_style_mod(lv_style_t * style)
{
    lv_obj_t * i;
    LL_READ(scr_ll, i) {
        report_style_mod_core(style, i);
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
	if(!obj->hidden) lv_obj_invalidate(obj);	/*Invalidate when not hidden (hidden objects are ignored) */

    obj->hidden = en == false ? 0 : 1;

	if(!obj->hidden) lv_obj_invalidate(obj);	/*Invalidate when not hidden (hidden objects are ignored) */

    lv_obj_t * par = lv_obj_get_parent(obj);
    par->signal_func(par, LV_SIGNAL_CHILD_CHG, obj);

}

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void lv_obj_set_click(lv_obj_t * obj, bool en)
{
    obj->click = (en == true ? 1 : 0);
}

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void lv_obj_set_top(lv_obj_t * obj, bool en)
{
    obj->top = (en == true ? 1 : 0);
}

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void lv_obj_set_drag(lv_obj_t * obj, bool en)
{
    if(en == true) lv_obj_set_click(obj, true);     /*Drag is useless without enabled clicking*/
    obj->drag = (en == true ? 1 : 0);
}

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void lv_obj_set_drag_throw(lv_obj_t * obj, bool en)
{
    obj->drag_throw = (en == true ? 1 : 0);
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
void lv_obj_clear_protect(lv_obj_t * obj, uint8_t prot)
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
void lv_obj_set_signal_func(lv_obj_t * obj, lv_signal_func_t fp)
{
    obj->signal_func = fp;
}

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param fp the new design function
 */
void lv_obj_set_design_func(lv_obj_t * obj, lv_design_func_t fp)
{
    obj->design_func = fp;
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
void * lv_obj_allocate_ext_attr(lv_obj_t * obj, uint16_t ext_size)
{
   obj->ext_attr = lv_mem_realloc(obj->ext_attr, ext_size);
    
   return (void*)obj->ext_attr;
}

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void lv_obj_refresh_ext_size(lv_obj_t * obj)
{
	obj->ext_size = 0;
	obj->signal_func(obj, LV_SIGNAL_REFR_EXT_SIZE, NULL);

	lv_obj_invalidate(obj);
}

#ifdef LV_OBJ_FREE_NUM_TYPE
/**
 * Set an application specific number for an object.
 * It can help to identify objects in the application. 
 * @param obj pointer to an object
 * @param free_num the new free number
 */
void lv_obj_set_free_num(lv_obj_t * obj, LV_OBJ_FREE_NUM_TYPE free_num)
{
    obj->free_num = free_num;
}
#endif

#if LV_OBJ_FREE_PTR != 0
/**
 * Set an application specific  pointer for an object.
 * It can help to identify objects in the application.
 * @param obj pointer to an object
 * @param free_p the new free pinter
 */
void lv_obj_set_free_ptr(lv_obj_t * obj, void * free_p)
{
    obj->free_ptr = free_p;
}
#endif

#if USE_LV_ANIMATION
/**
 * Animate an object
 * @param obj pointer to an object to animate
 * @param type type of animation from 'lv_anim_builtin_t'. 'OR' it with ANIM_IN or ANIM_OUT
 * @param time time of animation in milliseconds
 * @param delay delay before the animation in milliseconds
 * @param cb a function to call when the animation is ready
 */
void lv_obj_animate(lv_obj_t * obj, lv_anim_builtin_t type, uint16_t time, uint16_t delay, void (*cb) (lv_obj_t *))
{
	lv_obj_t * par = lv_obj_get_parent(obj);

	/*Get the direction*/
	bool out = (type & LV_ANIM_DIR_MASK) == LV_ANIM_IN ? false : true;
	type = type & (~LV_ANIM_DIR_MASK);

	lv_anim_t a;
	a.var = obj;
	a.time = time;
	a.act_time = (int32_t)-delay;
	a.end_cb = (void(*)(void*))cb;
	a.path = lv_anim_path_linear;
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

	lv_anim_create(&a);
}

#endif

/*=======================
 * Getter functions
 *======================*/

/*------------------
 * Screen get
 *-----------------*/

/**
 * Return with a pointer to the active screen
 * @return pointer to the active screen object (loaded by 'lv_scr_load()')
 */
lv_obj_t * lv_scr_act(void)
{
    return act_scr;
}

/**
 * Return with the top layer. (Same on every screen and it is above the normal screen layer)
 * @return pointer to the top layer object  (transparent screen sized lv_obj)
 */
lv_obj_t * lv_layer_top(void)
{
    return top_layer;
}

/**
 * Return with the system layer. (Same on every screen and it is above the all other layers)
 * It is used for example by the cursor
 * @return pointer to the system layer object (transparent screen sized lv_obj)
 */
lv_obj_t * lv_layer_sys(void)
{
    return sys_layer;
}

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_screen(lv_obj_t * obj)
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
 * Iterate through the children of an object (start from the "youngest")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(lv_obj_t * obj, lv_obj_t * child)
{
	if(child == NULL) {
		return lv_ll_get_head(&obj->child_ll);
	} else {
		return lv_ll_get_next(&obj->child_ll, child);
	}

	return NULL;
}

/**
 * Iterate through the children of an object (start from the "oldest")
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child_back(lv_obj_t * obj, lv_obj_t * child)
{
    if(child == NULL) {
        return lv_ll_get_tail(&obj->child_ll);
    } else {
        return lv_ll_get_prev(&obj->child_ll, child);
    }

    return NULL;
}

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children(lv_obj_t * obj)
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
void lv_obj_get_coords(lv_obj_t * obj, lv_area_t * cords_p)
{
    lv_area_copy(cords_p, &obj->coords);
}


/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent 
 */
lv_coord_t lv_obj_get_x(lv_obj_t * obj)
{
    lv_coord_t rel_x;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    rel_x = obj->coords.x1 - parent->coords.x1;
    
    return rel_x;
}

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent 
 */
lv_coord_t lv_obj_get_y(lv_obj_t * obj)
{
    lv_coord_t rel_y;
    lv_obj_t * parent = lv_obj_get_parent(obj);
    rel_y = obj->coords.y1 - parent->coords.y1;
    
    return rel_y;
}

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
lv_coord_t lv_obj_get_width(lv_obj_t * obj)
{
    return lv_area_get_width(&obj->coords);
}

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
lv_coord_t lv_obj_get_height(lv_obj_t * obj)
{
    return lv_area_get_height(&obj->coords);
}

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
lv_coord_t lv_obj_get_ext_size(lv_obj_t * obj)
{
    return obj->ext_size;
}

/*-----------------
 * Appearance get
 *---------------*/

/**
 * Get the style pointer of an object (if NULL get style of the parent)
 * @param obj pointer to an object
 * @return pointer to a style
 */
lv_style_t * lv_obj_get_style(lv_obj_t * obj)
{
    lv_style_t * style_act = obj->style_p;
    if(style_act == NULL) {
        lv_obj_t * par = obj->par;

        while(par) {
            if(par->style_p) {
                if(par->style_p->glass == 0) {
#if USE_LV_GROUP == 0
                    style_act = par->style_p;
#else
                    /*Is a parent is focused then use then focused style*/
                    lv_group_t *g = lv_obj_get_group(par);
                    if(lv_group_get_focused(g) == par) {
                        style_act = lv_group_mod_style(g, par->style_p);
                    } else {
                        style_act = par->style_p;
                    }
#endif
                    break;
                }
            }
            par = par->par;
        }
    }
#if USE_LV_GROUP
    if(obj->group_p) {
        if(lv_group_get_focused(obj->group_p) == obj) {
            style_act = lv_group_mod_style(obj->group_p, style_act);
        }
    }
#endif

    if(style_act == NULL) style_act = &lv_style_plain;

    return style_act;
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
    return obj->click == 0 ? false : true;
}

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feture is enabled
 */
bool lv_obj_get_top(lv_obj_t * obj)
{
    return obj->top == 0 ? false : true;
}

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool lv_obj_get_drag(lv_obj_t * obj)
{
    return obj->drag == 0 ? false : true;   
}

/**
 * Get the drag thow enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool lv_obj_get_drag_throw(lv_obj_t * obj)
{
    return obj->drag_throw == 0 ? false : true;
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
lv_signal_func_t   lv_obj_get_signal_func(lv_obj_t * obj)
{
    return obj->signal_func;
}

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
lv_design_func_t lv_obj_get_design_func(lv_obj_t * obj)
{
    return obj->design_func;
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
void * lv_obj_get_ext_attr(lv_obj_t * obj)
{
   return obj->ext_attr;
}

/**
 * Get object's and its ancestors type. Put their name in `type_buf` starting with the current type.
 * E.g. buf.type[0]="lv_btn", buf.type[1]="lv_cont", buf.type[2]="lv_obj"
 * @param obj pointer to an object which type should be get
 * @param buf pointer to an `lv_obj_type_t` buffer to store the types
 */
void lv_obj_get_type(lv_obj_t * obj, lv_obj_type_t * buf)
{
    lv_obj_type_t tmp;

    memset(buf, 0, sizeof(lv_obj_type_t));
    memset(&tmp, 0, sizeof(lv_obj_type_t));

    obj->signal_func(obj, LV_SIGNAL_GET_TYPE, &tmp);

    uint8_t cnt;
    for(cnt = 0; cnt < LV_MAX_ANCESTOR_NUM; cnt++) {
        if(tmp.type[cnt] == NULL) break;
    }


    /*Swap the order. The real type comes first*/
    uint8_t i;
    for(i = 0; i < cnt; i++) {
        buf->type[i] = tmp.type[cnt - 1 - i];
    }
}

#ifdef LV_OBJ_FREE_NUM_TYPE
/**
 * Get the free number
 * @param obj pointer to an object
 * @return the free number
 */
LV_OBJ_FREE_NUM_TYPE lv_obj_get_free_num(lv_obj_t * obj)
{
    return obj->free_num;
}
#endif

#if LV_OBJ_FREE_PTR != 0
/**
 * Get the free pointer
 * @param obj pointer to an object
 * @return the free pointer
 */
void * lv_obj_get_free_ptr(lv_obj_t * obj)
{
    return obj->free_ptr;
}
#endif


#if USE_LV_GROUP
/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * lv_obj_get_group(lv_obj_t * obj)
{
    return obj->group_p;
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
static bool lv_obj_design(lv_obj_t * obj, const  lv_area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {

        /* Because of the radius it is not sure the area is covered
         * Check the areas where there is no radius*/
        lv_style_t * style = lv_obj_get_style(obj);
        if(style->body.empty != 0) return false;

        uint16_t r = style->body.radius;

        if(r == LV_RADIUS_CIRCLE) return false;

        lv_area_t area_tmp;

        /*Check horizontally without radius*/
        lv_obj_get_coords(obj, &area_tmp);
        area_tmp.x1 += r;
        area_tmp.x2 -= r;
        if(lv_area_is_in(mask_p, &area_tmp) == false) return false;

        /*Check vertically without radius*/
        lv_obj_get_coords(obj, &area_tmp);
        area_tmp.y1 += r;
        area_tmp.y2 -= r;
        if(lv_area_is_in(mask_p, &area_tmp) == false) return false;

    } else if(mode == LV_DESIGN_DRAW_MAIN) {
		lv_style_t * style = lv_obj_get_style(obj);
		lv_draw_rect(&obj->coords, mask_p, style);
    }

    return true;
}

/**
 * Signal function of the basic object
 * @param obj pointer to an object
 * @param sign signal type
 * @param param parameter for the signal (depends on signal type)
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param)
{
    (void)param;

    lv_res_t res = LV_RES_OK;

    lv_style_t * style = lv_obj_get_style(obj);
    if(sign == LV_SIGNAL_CHILD_CHG) {
        /*Return 'invalid' if the child change signal is not enabled*/
        if(lv_obj_is_protected(obj, LV_PROTECT_CHILD_CHG) != false) res = LV_RES_INV;
    }
    else if(sign == LV_SIGNAL_REFR_EXT_SIZE) {
        if(style->body.shadow.width > obj->ext_size) obj->ext_size = style->body.shadow.width;
    }
    else if(sign ==  LV_SIGNAL_STYLE_CHG) {
        lv_obj_refresh_ext_size(obj);
    }
    else if(sign ==  LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        buf->type[0] = "lv_obj";
    }

    return res;
}

/**
 * Reposition the children of an object. (Called recursively)
 * @param obj pointer to an object which children will be repositioned
 * @param x_diff x coordinate shift
 * @param y_diff y coordinate shift
 */
static void refresh_childen_position(lv_obj_t * obj, lv_coord_t x_diff, lv_coord_t y_diff)
{
    lv_obj_t * i;   
    LL_READ(obj->child_ll, i) {
        i->coords.x1 += x_diff;
        i->coords.y1 += y_diff;
        i->coords.x2 += x_diff;
        i->coords.y2 += y_diff;
        
        refresh_childen_position(i, x_diff, y_diff);
    }
}

/**
 * Refresh the style of all children of an object. (Called recursively)
 * @param style_p refresh objects only with this style. (ignore is if NULL)
 * @param obj pointer to an object
 */
static void report_style_mod_core(void * style_p, lv_obj_t * obj)
{
    lv_obj_t * i;
    LL_READ(obj->child_ll, i) {
        if(i->style_p == style_p) {
            refresh_childen_style(i);
            lv_obj_refresh_style(i);
        }
        
        report_style_mod_core(style_p, i);
    }
}


/**
 * Recursively refresh the style of the children. Go deeper until a not NULL style is found
 * because the NULL styles are inherited from the parent
 * @param obj pointer to an object
 */
static void refresh_childen_style(lv_obj_t * obj)
{
    lv_obj_t * child = lv_obj_get_child(obj, NULL);
    while(child != NULL) {
        if(child->style_p == NULL) {
            refresh_childen_style(child);     /*Check children too*/
            lv_obj_refresh_style(child);       /*Notify the child about the style change*/
        } else if(child->style_p->glass) {
            /*Children with 'glass' parent might be effected if their style == NULL*/
            refresh_childen_style(child);
        }
        child = lv_obj_get_child(child, NULL);
    }
}

/**
 * Called by 'lv_obj_del' to delete the children objects
 * @param obj pointer to an object (all of its children will be deleted)
 */
static void delete_children(lv_obj_t * obj)
{
   lv_obj_t * i;
   lv_obj_t * i_next;
   i = lv_ll_get_head(&(obj->child_ll));
   while(i != NULL) {
       /*Get the next object before delete this*/
       i_next = lv_ll_get_next(&(obj->child_ll), i);

       /*Call the recursive del to the child too*/
       delete_children(i);

       /*Set i to the next node*/
       i = i_next;
   }

   /*Remove the animations from this object*/
#if USE_LV_ANIMATION
   lv_anim_del(obj, NULL);
#endif

   /*Delete from the group*/
#if USE_LV_GROUP
   if(obj->group_p != NULL) lv_group_remove_obj(obj);
#endif

   /* Reset the input devices if
    * the currently pressed object is deleted*/
   lv_indev_t * indev = lv_indev_next(NULL);
   while(indev) {
       if(indev->proc.act_obj == obj || indev->proc.last_obj == obj) {
           lv_indev_reset(indev);
       }
       indev = lv_indev_next(indev);
   }

   /*Remove the object from parent's children list*/
   lv_obj_t * par = lv_obj_get_parent(obj);
   lv_ll_rem(&(par->child_ll), obj);

   /* Clean up the object specific data*/
   obj->signal_func(obj, LV_SIGNAL_CLEANUP, NULL);

   /*Delete the base objects*/
   if(obj->ext_attr != NULL)  lv_mem_free(obj->ext_attr);
   lv_mem_free(obj); /*Free the object itself*/

}
