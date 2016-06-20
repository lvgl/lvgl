/**
 * @file lv_base_obj.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../lv_draw/lv_draw_rbasic.h"
#include "../lv_draw/lv_draw_vbasic.h"
#include "lv_dispi.h"
#include "lv_refr.h"
#include "misc/math/math_base.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void lv_obj_pos_child_refr(lv_obj_t* obj_dp, cord_t x_diff, cord_t y_diff);
static void lv_style_refr_core(void * style_p, lv_obj_t* obj_dp);
static bool lv_obj_design(lv_obj_t* obj_dp, const  area_t * mask_p, lv_design_mode_t mode);
static void lv_obj_refr_layout(lv_obj_t * obj_dp);
static void lv_layout_col(lv_obj_t * obj_dp);
static void lv_layout_row(lv_obj_t * obj_dp);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_obj_t* def_scr_dp = NULL;
lv_obj_t* act_scr_dp = NULL;
ll_dsc_t scr_ll;

lv_objs_t lv_objs_def = {.color = COLOR_MAKE(0x20, 0x30, 0x40)};
lv_objs_t lv_objs_scr = {.color = LV_OBJ_DEF_SCR_COLOR};

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
    
    /*Create the default screen*/
    ll_init(&scr_ll, sizeof(lv_obj_t));
    def_scr_dp = lv_obj_create(NULL, NULL);
    act_scr_dp = def_scr_dp;
    
    /*Refresh the screen*/
    lv_obj_inv(act_scr_dp);
    
#if LV_DISPI_READ_PERIOD != 0
    /*Init the display input handling*/
    lv_dispi_init();
#endif
}

/**
 * Mark the object as invalid therefor it will be redrawn by 'lv_refr_task'
 * @param obj_dp pointer to an object
 */
void lv_obj_inv(lv_obj_t* obj_dp)
{
    /*Invalidate the object only if it belongs to the 'act_scr'*/
    lv_obj_t* act_scr_p = lv_scr_act(); 
    if(lv_obj_get_scr(obj_dp) == act_scr_p) {
        /*Truncate the recursively on the parents*/
        area_t area_trunc;
        lv_obj_t* par_dp = lv_obj_get_parent(obj_dp);
        bool union_ok = true;
        /*Start with the original coordinates*/
        area_cpy(&area_trunc, &obj_dp->cords);
        
        /*Check through all parents*/
        while(par_dp != NULL) {
            union_ok = area_union(&area_trunc, &area_trunc, &par_dp->cords);
            if(union_ok == false) break; /*If no common parts with parent break;*/
            
            par_dp = lv_obj_get_parent(par_dp);
        }
        
        if(union_ok != false)  lv_inv_area(&area_trunc);
    }
}

/**
 * Notify an object if its style is modified
 * @param obj_dp pointer to an object 
 */
void lv_obj_refr_style(lv_obj_t* obj_dp)
{ 
    lv_obj_inv(obj_dp);
    obj_dp->signal_f(obj_dp, LV_SIGNAL_STYLE_CHG, NULL);
    lv_obj_inv(obj_dp);

}

/**
 * Notify all object if a style is modified
 * @param style_p pinter to a style. Only objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_style_refr_all(void * style_p)
{
    lv_obj_t* i;
    LL_READ(scr_ll, i) {
        lv_style_refr_core(style_p, i);
    }
}

/*--------------------
 * Create and delete
 *-------------------*/

/**
 * Create a basic object 
 * @param parent_dp pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy_dp pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
lv_obj_t* lv_obj_create(lv_obj_t* parent_dp, lv_obj_t * copy_dp)
{
    lv_obj_t* new_obj_dp = NULL;
    /*Create a screen if the parent is NULL*/
    if(parent_dp == NULL) {
        new_obj_dp = ll_ins_head(&scr_ll);
        
        new_obj_dp->par_dp = NULL; /*Screens has no a parent*/
        ll_init(&(new_obj_dp->child_ll), sizeof(lv_obj_t));
        
		/*Set coordinates to full screen size*/
		new_obj_dp->cords.x1 = 0;
		new_obj_dp->cords.y1 = 0;
		new_obj_dp->cords.x2 = LV_HOR_RES - 1;
		new_obj_dp->cords.y2 = LV_VER_RES - 1;

		/*Set appearance*/
		new_obj_dp->style_p = lv_objs_get(LV_OBJS_SCR, NULL);
		new_obj_dp->opa = OPA_COVER;

		/*Set virtual functions*/
		lv_obj_set_signal_f(new_obj_dp, lv_obj_signal);
		lv_obj_set_design_f(new_obj_dp, lv_obj_design);

		/*Set attributes*/
		new_obj_dp->click_en = 0;
		new_obj_dp->drag_en = 0;
		new_obj_dp->drag_throw_en = 0;
		new_obj_dp->drag_parent = 0;
		new_obj_dp->style_iso = 0;
		new_obj_dp->hidden = 0;
		new_obj_dp->top_en = 0;
		new_obj_dp->ext_dp = NULL;
	 }
    /*parent_dp != NULL create normal obj. on parent*/
    else
    {   
        new_obj_dp = ll_ins_head(&(parent_dp)->child_ll);
        
        new_obj_dp->par_dp = parent_dp; /*Set the parent*/
        ll_init(&(new_obj_dp->child_ll), sizeof(lv_obj_t));
        
        /*Set coordinates left top corner of parent*/
        new_obj_dp->cords.x1 = parent_dp->cords.x1;
        new_obj_dp->cords.y1 = parent_dp->cords.y1;
        new_obj_dp->cords.x2 = parent_dp->cords.x1 +
                                   LV_OBJ_DEF_WIDTH;
        new_obj_dp->cords.y2 = parent_dp->cords.y1 +
                                   LV_OBJ_DEF_HEIGHT;

        /*Set appearance*/
        new_obj_dp->style_p = lv_objs_get(LV_OBJS_DEF, NULL);
        new_obj_dp->opa = OPA_COVER;
        
        /*Set virtual functions*/
        lv_obj_set_signal_f(new_obj_dp, lv_obj_signal);
        lv_obj_set_design_f(new_obj_dp, lv_obj_design);
        
        /*Set attributes*/
        new_obj_dp->click_en = 1;
        new_obj_dp->drag_en = 0;
        new_obj_dp->drag_throw_en = 0;
        new_obj_dp->drag_parent = 0;
        new_obj_dp->style_iso = 0;
        new_obj_dp->hidden = 0;
        new_obj_dp->top_en = 0;
        
        new_obj_dp->ext_dp = NULL;
        
    }

    if(copy_dp != NULL) {
    	area_cpy(&new_obj_dp->cords, &copy_dp->cords);

        /*Set attributes*/
        new_obj_dp->click_en = copy_dp->click_en;
        new_obj_dp->drag_en = copy_dp->drag_en;
        new_obj_dp->drag_throw_en = copy_dp->drag_throw_en;
        new_obj_dp->drag_parent = copy_dp->drag_parent;
        new_obj_dp->hidden = copy_dp->hidden;
        new_obj_dp->top_en = copy_dp->top_en;

    	new_obj_dp->style_p = copy_dp->style_p;

        if(copy_dp->style_iso != 0) {
        	lv_obj_iso_style(new_obj_dp, dm_get_size(copy_dp->style_p));
        }

    	lv_obj_set_pos(new_obj_dp, lv_obj_get_x(copy_dp), lv_obj_get_y(copy_dp));

        new_obj_dp->opa = copy_dp->opa;
    }


    /*Send a signal to the parent to notify it about the new child*/
    if(parent_dp != NULL) {
        parent_dp->signal_f(parent_dp, LV_SIGNAL_CHILD_CHG, new_obj_dp);

        /*Invalidate the area if not screen created*/
        lv_obj_inv(new_obj_dp);
    }
    
    return new_obj_dp;
}

/**
 * Delete 'obj_dp' and all of its children
 * @param obj_dp
 */
void lv_obj_del(lv_obj_t* obj_dp)
{
    lv_obj_inv(obj_dp);
    
    /*Recursively delete the children*/
    lv_obj_t* i;
    lv_obj_t* i_next;
    i = ll_get_head(&(obj_dp->child_ll));
    while(i != NULL) {
        /*Get the next object before delete this*/
        i_next = ll_get_next(&(obj_dp->child_ll), i);
        
        /*Call the recursive del to the child too*/
        lv_obj_del(i);
        
        /*Set i to the next node*/
        i = i_next;
    }
    
    /*Remove the object from parent's children list*/
    lv_obj_t* par_dp = lv_obj_get_parent(obj_dp);
    if(par_dp == NULL) { /*It is a screen*/
    	ll_rem(&scr_ll, obj_dp);
    } else {
    	ll_rem(&(par_dp->child_ll), obj_dp);
    }

    /* All children deleted.
     * Now clean up the object specific data*/
    obj_dp->signal_f(obj_dp, LV_SIGNAL_CLEANUP, NULL);
    
    /*Delete the base objects*/
    if(obj_dp->ext_dp != NULL)  dm_free(obj_dp->ext_dp);
    if(obj_dp->style_iso != 0) dm_free(obj_dp->style_p);
    dm_free(obj_dp); /*Free the object itself*/
    
    /* Reset all display input (dispi) because 
     * the deleted object can be being pressed*/
    lv_dispi_reset();
    
    /*Send a signal to the parent to notify it about the child delete*/
    if(par_dp != NULL) {
    	par_dp->signal_f(par_dp, LV_SIGNAL_CHILD_CHG, NULL);
    }
}

/**
 * Signal function of the basic object
 * @param obj_dp pointer to an object
 * @param sign signal type
 * @param param parameter for the signal (depends on signal type)
 * @return false: the object become invalid (e.g. deleted) 
 */
bool lv_obj_signal(lv_obj_t* obj_dp, lv_signal_t sign, void * param)
{
    bool valid = true;

    switch(sign) {
    case LV_SIGNAL_CHILD_CHG:
    	lv_obj_refr_layout(obj_dp);
    	break;
    case LV_SIGNAL_CORD_CHG:
    	if(param == NULL)
    	{
    		break;
    	}

    	if(lv_obj_get_width(obj_dp) != area_get_width(param) ||
		  lv_obj_get_height(obj_dp) != area_get_height(param)) {
        	lv_obj_refr_layout(obj_dp);
    	}
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
/*=====================
 * Setter functions 
 *====================*/

/*--------------
 * Screen set 
 *--------------*/

/**
 * Load a new screen
 * @param scr_dp pointer to a screen
 */
void lv_scr_load(lv_obj_t* scr_dp)
{
    act_scr_dp = scr_dp;
    
    lv_obj_inv(act_scr_dp);
}

/*--------------------
 * Parent/children set 
 *--------------------*/

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj_dp pointer to an object
 * @param parent_dp pointer to the new parent object
 */
void lv_obj_set_parent(lv_obj_t* obj_dp, lv_obj_t* parent_dp)
{
    lv_obj_inv(obj_dp);
    
    point_t old_pos;
    old_pos.x = lv_obj_get_x(obj_dp);
    old_pos.y = lv_obj_get_y(obj_dp);
    
    ll_chg_list(&obj_dp->par_dp->child_ll, &parent_dp->child_ll, obj_dp);
    
    obj_dp->par_dp->signal_f(obj_dp->par_dp, LV_SIGNAL_CHILD_CHG, NULL);
    
    obj_dp->par_dp = parent_dp;
    
    lv_obj_set_pos(obj_dp, old_pos.x, old_pos.y);
    
    parent_dp->signal_f(parent_dp, LV_SIGNAL_CHILD_CHG, obj_dp);

    
    lv_obj_inv(obj_dp);
}

/*-------------------------------------------
 * Coordinate set (cord_chk_f will be called)
 * -----------------------------------------*/

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj_dp pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t* obj_dp, cord_t x, cord_t y)
{
    /*Invalidate the original area*/
    lv_obj_inv(obj_dp);
    
    /*Save the original coordinates*/
    area_t ori;
    lv_obj_get_cords(obj_dp, &ori);
    
    /*Convert x and y to absolute coordinates*/
    lv_obj_t* par_dp = obj_dp->par_dp;
    x = x + par_dp->cords.x1;
    y = y + par_dp->cords.y1;
    
    /*Calculate and set the movement*/
    point_t diff;
    diff.x =  x - obj_dp->cords.x1;
    diff.y =  y - obj_dp->cords.y1;

    /*Do nothing if the position is not changed*/
    if(diff.x == 0 && diff.y == 0) return;
        
    obj_dp->cords.x1 += diff.x;
    obj_dp->cords.y1 += diff.y;
    obj_dp->cords.x2 += diff.x;
    obj_dp->cords.y2 += diff.y;
    
    lv_obj_pos_child_refr(obj_dp, diff.x, diff.y);
    
    /*Send a signal*/
    obj_dp->signal_f(obj_dp, LV_SIGNAL_CORD_CHG, &ori);
    
    /*Send a signal to the parent too*/
    par_dp->signal_f(par_dp, LV_SIGNAL_CHILD_CHG, obj_dp);
    
    /*Invalidate the new area*/
    lv_obj_inv(obj_dp);
}

/**
 * Set relative the position of an object (relative to the parent).
 * The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj_dp pointer to an object
 * @param x new distance from the left side of the parent. (will be multiplied with LV_DOWNSCALE)
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_pos_us(lv_obj_t* obj_dp, cord_t x, cord_t y)
{
	lv_obj_set_pos(obj_dp, x * LV_DOWNSCALE, y * LV_DOWNSCALE);
}

/**
 * Set the x coordinate of a object
 * @param obj_dp pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t* obj_dp, cord_t x)
{
    lv_obj_set_pos(obj_dp, x, lv_obj_get_y(obj_dp));
}

/**
 * Set the x coordinate of a object.
 * The coordinate will be upscaled to compensate LV_DOWNSCALE.
 * @param obj_dp pointer to an object
 * @param x new distance from the left side from the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_x_us(lv_obj_t* obj_dp, cord_t x)
{
    lv_obj_set_pos(obj_dp, x * LV_DOWNSCALE, lv_obj_get_y(obj_dp));
}

/**
 * Set the y coordinate of a object
 * @param obj_dp pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t* obj_dp, cord_t y)
{
    lv_obj_set_pos(obj_dp, lv_obj_get_x(obj_dp), y);
}

/**
 * Set the y coordinate of a object.
 * The coordinate will be upscaled to compensate LV_DOWNSCALE.
 * @param obj_dp pointer to an object
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_y_us(lv_obj_t* obj_dp, cord_t y)
{
    lv_obj_set_pos(obj_dp, lv_obj_get_x(obj_dp), y * LV_DOWNSCALE);
}

/**
 * Set the size of an object
 * @param obj_dp pointer to an object
 * @param w new width 
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t* obj_dp, cord_t w, cord_t h)
{
    /*Invalidate the original area*/
    lv_obj_inv(obj_dp);
    
    /*Save the original coordinates*/
    area_t ori;
    lv_obj_get_cords(obj_dp, &ori);
    
    //Set the length and height
    obj_dp->cords.x2 = obj_dp->cords.x1 + w - 1;
    obj_dp->cords.y2 = obj_dp->cords.y1 + h - 1;
    
    /*Send a signal*/
    obj_dp->signal_f(obj_dp, LV_SIGNAL_CORD_CHG, &ori);
    
    /*Send a signal to the parent too*/
    lv_obj_t* par_dp = lv_obj_get_parent(obj_dp);
    if(par_dp != NULL) par_dp->signal_f(par_dp, LV_SIGNAL_CHILD_CHG, obj_dp);
    
    /*Invalidate the new area*/
    lv_obj_inv(obj_dp);
}

/**
 * Set the size of an object. The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj_dp pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_size_us(lv_obj_t* obj_dp, cord_t w, cord_t h)
{
	lv_obj_set_size(obj_dp, w * LV_DOWNSCALE, h * LV_DOWNSCALE);
}

/**
 * Set the width of an object
 * @param obj_dp pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t* obj_dp, cord_t w)
{
    lv_obj_set_size(obj_dp, w, lv_obj_get_height(obj_dp));
}

/**
 * Set the width of an object.  The width will be upscaled to compensate LV_DOWNSCALE
 * @param obj_dp pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_width_us(lv_obj_t* obj_dp, cord_t w)
{
    lv_obj_set_size(obj_dp, w * LV_DOWNSCALE, lv_obj_get_height(obj_dp));
}

/**
 * Set the height of an object
 * @param obj_dp pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t* obj_dp, cord_t h)
{
    lv_obj_set_size(obj_dp, lv_obj_get_width(obj_dp), h);
}

/**
 * Set the height of an object.  The height will be upscaled to compensate LV_DOWNSCALE
 * @param obj_dp pointer to an object
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_height_us(lv_obj_t* obj_dp, cord_t h)
{
    lv_obj_set_size(obj_dp, lv_obj_get_width(obj_dp), h * LV_DOWNSCALE);
}


/**
 * Set a layout for an object.
 * @param obj_dp pointer to an object
 * @param layout type of the layout (an element from lv_layout_t)
 */
void lv_obj_set_layout(lv_obj_t* obj_dp, lv_layout_t layout)
{
	obj_dp->layout_type = layout;
	obj_dp->signal_f(obj_dp, LV_SIGNAL_CHILD_CHG, NULL);
}

/**
 * Set the layout spacing for an object.
 * @param obj_dp pointer to an object
 * @param space space between object on the layout (space / 2 on edges)
 */
void lv_obj_set_layout_space(lv_obj_t * obj_dp, cord_t space)
{
	obj_dp->layout_space = space;
	obj_dp->signal_f(obj_dp, LV_SIGNAL_CHILD_CHG, NULL);
}

/**
 * Set the layout spacing for an object.
 * The space will be upscaled to compensate LV_DOWNSCALE
 * @param obj_dp pointer to an object
 * @param space space between object on the layout (space / 2 on edges)
 */
void lv_obj_set_layout_space_us(lv_obj_t * obj_dp, cord_t space)
{
	lv_obj_set_layout_space(obj_dp, space * LV_DOWNSCALE);
}

/**
 * Align an object to an other object. 
 * @param obj_dp pointer to an object to align
 * @param base_dp pointer to an object (if NULL the parent is used). 'obj_dp' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align(lv_obj_t* obj_dp,lv_obj_t* base_dp, lv_align_t align, cord_t x_mod, cord_t y_mod)
{
    cord_t new_x = lv_obj_get_x(obj_dp);
    cord_t new_y = lv_obj_get_y(obj_dp);

    if(base_dp == NULL) {
        base_dp = lv_obj_get_parent(obj_dp);
    }
    
    switch(align)
    {
        case LV_ALIGN_CENTER:
            new_x = lv_obj_get_width(base_dp) / 2 - lv_obj_get_width(obj_dp) / 2;
            new_y = lv_obj_get_height(base_dp) / 2 - lv_obj_get_height(obj_dp) / 2;
            break;

        case LV_ALIGN_IN_TOP_LEFT:
            new_x = 0;
            new_y = 0;
        	break;
        case LV_ALIGN_IN_TOP_MID:
            new_x = lv_obj_get_width(base_dp) / 2 - lv_obj_get_width(obj_dp) / 2;
            new_y = 0;
            break;

        case LV_ALIGN_IN_TOP_RIGHT:
            new_x = lv_obj_get_width(base_dp) - lv_obj_get_width(obj_dp);
            new_y = 0;
            break;

        case LV_ALIGN_IN_BOTTOM_LEFT:
            new_x = 0;
            new_y = lv_obj_get_height(base_dp) - lv_obj_get_height(obj_dp);
        	break;
        case LV_ALIGN_IN_BOTTOM_MID:
            new_x = lv_obj_get_width(base_dp) / 2 - lv_obj_get_width(obj_dp) / 2;
            new_y = lv_obj_get_height(base_dp) - lv_obj_get_height(obj_dp);
            break;

        case LV_ALIGN_IN_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base_dp) - lv_obj_get_width(obj_dp);
            new_y = lv_obj_get_height(base_dp) - lv_obj_get_height(obj_dp);
            break;

        case LV_ALIGN_IN_LEFT_MID:
            new_x = 0;
            new_y = lv_obj_get_height(base_dp) / 2 - lv_obj_get_height(obj_dp) / 2;
        	break;

        case LV_ALIGN_IN_RIGHT_MID:
            new_x = lv_obj_get_width(base_dp) - lv_obj_get_width(obj_dp);
            new_y = lv_obj_get_height(base_dp) / 2 - lv_obj_get_height(obj_dp) / 2;
        	break;

        case LV_ALIGN_OUT_TOP_LEFT:
            new_x = 0;
            new_y = -lv_obj_get_height(obj_dp);
        	break;

        case LV_ALIGN_OUT_TOP_MID:
            new_x = lv_obj_get_width(base_dp) / 2 - lv_obj_get_width(obj_dp) / 2;
            new_y = - lv_obj_get_height(obj_dp);
        	break;

        case LV_ALIGN_OUT_TOP_RIGHT:
            new_x = lv_obj_get_width(base_dp) - lv_obj_get_width(obj_dp);
            new_y = - lv_obj_get_height(obj_dp);
        	break;

        case LV_ALIGN_OUT_BOTTOM_LEFT:
            new_x = 0;
            new_y = lv_obj_get_height(base_dp);
        	break;

        case LV_ALIGN_OUT_BOTTOM_MID:
            new_x = lv_obj_get_width(base_dp) / 2 - lv_obj_get_width(obj_dp) / 2;
            new_y = lv_obj_get_height(base_dp);
        	break;

        case LV_ALIGN_OUT_BOTTOM_RIGHT:
            new_x = lv_obj_get_width(base_dp) - lv_obj_get_width(obj_dp);
            new_y = lv_obj_get_height(base_dp);
        	break;

        case LV_ALIGN_OUT_LEFT_TOP:
            new_x = - lv_obj_get_width(obj_dp);
            new_y = 0;
        	break;

        case LV_ALIGN_OUT_LEFT_MID:
            new_x = - lv_obj_get_width(obj_dp);
            new_y = lv_obj_get_height(base_dp) / 2 - lv_obj_get_height(obj_dp) / 2;
        	break;

        case LV_ALIGN_OUT_LEFT_BOTTOM:
            new_x = - lv_obj_get_width(obj_dp);
            new_y = lv_obj_get_height(base_dp) - lv_obj_get_height(obj_dp);
        	break;

        case LV_ALIGN_OUT_RIGHT_TOP:
            new_x = lv_obj_get_width(base_dp);
            new_y = 0;
        	break;

        case LV_ALIGN_OUT_RIGHT_MID:
            new_x = lv_obj_get_width(base_dp);
            new_y = lv_obj_get_height(base_dp) / 2 - lv_obj_get_height(obj_dp) / 2;
        	break;

        case LV_ALIGN_OUT_RIGHT_BOTTOM:
            new_x = lv_obj_get_width(base_dp);
            new_y = lv_obj_get_height(base_dp) - lv_obj_get_height(obj_dp);
        	break;
    }

    /*Bring together the coordination system of base_dp and obj_dp*/
    lv_obj_t* par_dp = lv_obj_get_parent(obj_dp);
    cord_t base_abs_x = base_dp->cords.x1;
    cord_t base_abs_y = base_dp->cords.y1;
    cord_t par_abs_x = par_dp->cords.x1;
    cord_t par_abs_y = par_dp->cords.y1;
    new_x += x_mod + base_abs_x;
    new_y += y_mod + base_abs_y;

    if(new_x != obj_dp->cords.x1 || new_y != obj_dp->cords.y1) {

		new_x -= par_abs_x;
		new_y -= par_abs_y;

		lv_obj_set_pos(obj_dp, new_x, new_y);
    }
}


/**
 * Align an object to an other object. The coordinates will be upscaled to compensate LV_DOWNSCALE.
 * @param obj_dp pointer to an object to align
 * @param base_dp pointer to an object (if NULL the parent is used). 'obj_dp' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 * @param y_mod y coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_align_us(lv_obj_t* obj_dp,lv_obj_t* base_dp, lv_align_t align, cord_t x_mod, cord_t y_mod)
{
	lv_obj_align(obj_dp, base_dp, align, x_mod * LV_DOWNSCALE, y_mod * LV_DOWNSCALE);
}

/*---------------------
 * Appearance set 
 *--------------------*/

/**
 * Set a new style for an object
 * @param obj_dp pointer to an object
 * @param style_p pointer to the new style
 */
void lv_obj_set_style(lv_obj_t* obj_dp, void * style_p)
{

	if(obj_dp->style_iso != 0) {
		dm_free(obj_dp->style_p);
	}

    obj_dp->style_p = style_p;
    obj_dp->signal_f(obj_dp, LV_SIGNAL_STYLE_CHG, NULL);
    //obj_dp->signal_f(lv_obj_get_parent(obj_dp, )
    lv_obj_inv(obj_dp);
}

/**
 * Isolate the style of an object. In other words a unique style will be created
 * for this object which can be freely modified independently from the style of the
 * other objects.
 */
void * lv_obj_iso_style(lv_obj_t * obj_dp, uint32_t style_size)
{
	if(obj_dp->style_iso != 0) return obj_dp->style_p;

	void * ori_style_p = lv_obj_get_style(obj_dp);
	void * iso_style_dp = dm_alloc(style_size);
	dm_assert(iso_style_dp);
	memcpy(iso_style_dp, ori_style_p, style_size);

	obj_dp->style_iso = 1;
	obj_dp->style_p = iso_style_dp;

	lv_obj_refr_style(obj_dp);

	return obj_dp->style_p;
}

/**
 * Set the opacity of an object
 * @param obj_dp pointer to an object
 * @param opa 0 (transparent) .. 255(fully cover)
 */
void lv_obj_set_opa(lv_obj_t* obj_dp, uint8_t opa)
{
    obj_dp->opa = opa;
    
    lv_obj_inv(obj_dp);
}

/**
 * Set the opacity of an object and all of its children
 * @param obj_dp pointer to an object
 * @param opa 0 (transparent) .. 255(fully cover)
 */
void lv_obj_set_opar(lv_obj_t* obj_dp, uint8_t opa)
{
    lv_obj_t* i;
    
    LL_READ(obj_dp->child_ll, i) {
        lv_obj_set_opar(i, opa);
    }
    
    obj_dp->opa = opa;
    
    lv_obj_inv(obj_dp);
}

/*-----------------
 * Attribute set
 *----------------*/

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj_dp pointer to an object
 * @param hidden_en true: hide the object
 */
void lv_obj_set_hidden(lv_obj_t* obj_dp, bool hidden_en)
{
    obj_dp->hidden = hidden_en == false ? 0 : 1;
    
    lv_obj_inv(obj_dp);
}

/**
 * Enable or disable the clicking of an object
 * @param obj_dp pointer to an object
 * @param click_en true: make the object clickable
 */
void lv_obj_set_click(lv_obj_t* obj_dp, bool click_en)
{
    obj_dp->click_en = (click_en == true ? 1 : 0);
    
}

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj_dp pointer to an object
 * @param top_en true: enable the auto top feature
 */
void lv_obj_set_top(lv_obj_t* obj_dp, bool top_en)
{
    obj_dp->top_en= (top_en == true ? 1 : 0);
}

/**
 * Enable the dragging of an object
 * @param obj_dp pointer to an object
 * @param drag_en true: make the object dragable
 */
void lv_obj_set_drag(lv_obj_t* obj_dp, bool drag_en)
{
    obj_dp->drag_en = (drag_en == true ? 1 : 0);
}

/**
 * Enable the throwing of an object after is is dragged
 * @param obj_dp pointer to an object
 * @param dragthr_en true: enable the drag throw
 */
void lv_obj_set_drag_throw(lv_obj_t* obj_dp, bool dragthr_en)
{
    obj_dp->drag_throw_en = (dragthr_en == true ? 1 : 0);
}

/**
 * Enable to use parent for drag related operations. 
 * If trying to drag the object the parent will be moved instead 
 * @param obj_dp pointer to an object
 * @param dragpar_en true: enable the 'drag parent' for the object
 */
void lv_obj_set_drag_parent(lv_obj_t* obj_dp, bool dragpar_en)
{
    obj_dp->drag_parent = (dragpar_en == true ? 1 : 0);
}

/**
 * Set the signal function of an object. 
 * Always call the previous signal function in the new.
 * @param obj_dp pointer to an object
 * @param fp the new signal function
 */
void lv_obj_set_signal_f(lv_obj_t* obj_dp, lv_signal_f_t fp)
{
    obj_dp->signal_f = fp;
}

/**
 * Set a new design function for an object
 * @param obj_dp pointer to an object
 * @param fp the new design function
 */
void lv_obj_set_design_f(lv_obj_t* obj_dp, lv_design_f_t fp)
{
    obj_dp->design_f = fp;
}

/*----------------
 * Other set
 *--------------*/

/**
 * Allocate a new ext. data for an object
 * @param obj_dp pointer to an object
 * @param ext_size the size of the new ext. data
 * @return Normal pointer to the allocated ext
 */
void * lv_obj_alloc_ext(lv_obj_t* obj_dp, uint16_t ext_size)
{
    obj_dp->ext_dp = dm_realloc(obj_dp->ext_dp, ext_size); 
    
   return (void*)obj_dp->ext_dp;
}

/**
 * Set an application specific number for an object.
 * It can help to identify objects in the application. 
 * @param obj_dp pointer to an object
 * @param free_num the new free number
 */
void lv_obj_set_free_num(lv_obj_t* obj_dp, uint8_t free_num)
{
    obj_dp->free_num = free_num;
}

#if LV_OBJ_FREE_P != 0
/**
 * Set an application specific  pointer for an object.
 * It can help to identify objects in the application.
 * @param obj_dp pointer to an object
 * @param free_p the new free pinter
 */
void lv_obj_set_free_p(lv_obj_t* obj_dp, void * free_p)
{
    obj_dp->free_p = free_p;
}
#endif
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
lv_obj_t* lv_scr_act(void)
{
    return act_scr_dp;
}

/**
 * Return with the screen of an object
 * @param obj_dp pointer to an object
 * @return pointer to a screen
 */
lv_obj_t* lv_obj_get_scr(lv_obj_t* obj_dp)
{
    lv_obj_t* par_dp = obj_dp;
    lv_obj_t* act_p;
    
    do {
        act_p = par_dp;
        par_dp = lv_obj_get_parent(act_p);    
    }
    while(par_dp != NULL);
    
    return act_p;
}


/*---------------------
 * Parent/children get
 *--------------------*/

/**
 * Returns with the parent of an object
 * @param obj_dp pointer to an object
 * @return pointer to the parent of  'obj_dp' 
 */
lv_obj_t* lv_obj_get_parent(lv_obj_t * obj_dp)
{
    return obj_dp->par_dp;
}

/**
 * Iterate through the children of an object
 * @param obj_dp pointer to an object
 * @param child_dp NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child_dp' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(lv_obj_t * obj_dp, lv_obj_t * child_dp)
{
	if(child_dp == NULL) {
		return ll_get_head(&obj_dp->child_ll);
	} else {
		return ll_get_next(&obj_dp->child_ll, child_dp);
	}

	return NULL;
}

/*---------------------
 * Coordinate get
 *--------------------*/

/**
 * Copy the coordinates of an object to an area
 * @param obj_dp pointer to an object
 * @param cords_p pointer to an area to store the coordinates 
 */
void lv_obj_get_cords(lv_obj_t* obj_dp, area_t * cords_p)
{
    area_cpy(cords_p, &obj_dp->cords);
}


/**
 * Get the x coordinate of object
 * @param obj_dp pointer to an object
 * @return distance of 'obj_dp' from the left side of its parent 
 */
cord_t lv_obj_get_x(lv_obj_t* obj_dp)
{
    cord_t rel_x;
    lv_obj_t* parent_dp = lv_obj_get_parent(obj_dp);
    rel_x = obj_dp->cords.x1 - parent_dp->cords.x1;
    
    return rel_x;
}

/**
 * Get the y coordinate of object
 * @param obj_dp pointer to an object
 * @return distance of 'obj_dp' from the top of its parent 
 */
cord_t lv_obj_get_y(lv_obj_t* obj_dp)
{
    cord_t rel_y;
    lv_obj_t* parent_dp = lv_obj_get_parent(obj_dp);
    rel_y = obj_dp->cords.y1 - parent_dp->cords.y1;
    
    return rel_y;
}

/**
 * Get the width of an object
 * @param obj_dp pointer to an object
 * @return the width
 */
cord_t lv_obj_get_width(lv_obj_t* obj_dp)
{
    return area_get_width(&obj_dp->cords);
}

/**
 * Get the height of an object
 * @param obj_dp pointer to an object
 * @return the height
 */
cord_t lv_obj_get_height(lv_obj_t* obj_dp)
{
    return area_get_height(&obj_dp->cords);
}

/**
 * Get the layout type of an object
 * @param obj_dp pointer to an object
 * @return type of the layout (from lv_layout_t)
 */
lv_layout_t lv_obj_get_layout(lv_obj_t * obj_dp)
{
	return obj_dp->layout_type;
}

/**
 * Get the layout space of an object
 * @param obj_dp pointer to an object
 * @return the layout space
 */
cord_t lv_obj_get_layout_space(lv_obj_t * obj_dp)
{
	return obj_dp->layout_space;
}

/*-----------------
 * Appearance get
 *---------------*/

/**
 * Get the style pointer of an object
 * @param obj_dp pointer to an object
 * @return pointer to a style
 */
void * lv_obj_get_style(lv_obj_t* obj_dp)
{
    return obj_dp->style_p;
}

/**
 * Get the opacity of an object
 * @param obj_dp pointer to an object
 * @return 0 (transparent) .. 255 (fully cover)
 */
opa_t lv_obj_get_opa(lv_obj_t* obj_dp)
{
    return obj_dp->opa;
}

/*-----------------
 * Attribute get
 *----------------*/

/**
 * Get the hidden attribute of an object
 * @param obj_dp pointer to an object
 * @return true: the object is hidden
 */
bool lv_obj_get_hidden(lv_obj_t* obj_dp)
{
    return obj_dp->hidden == 0 ? false : true;
}

/**
 * Get the click enable attribute of an object
 * @param obj_dp pointer to an object
 * @return true: the object is clickable
 */
bool lv_obj_get_click(lv_obj_t* obj_dp)
{
    return obj_dp->click_en == 0 ? false : true;;
}

/**
 * Get the top enable attribute of an object
 * @param obj_dp pointer to an object
 * @return true: the auto top feture is enabled
 */
bool lv_obj_get_top(lv_obj_t* obj_dp)
{
    return obj_dp->top_en == 0 ? false : true;;
}

/**
 * Get the drag enable attribute of an object
 * @param obj_dp pointer to an object
 * @return true: the object is dragable
 */
bool lv_obj_get_drag(lv_obj_t* obj_dp)
{
    return obj_dp->drag_en == 0 ? false : true;   
}

/**
 * Get the drag thow enable attribute of an object
 * @param obj_dp pointer to an object
 * @return true: drag throw is enabled
 */
bool lv_obj_get_drag_throw(lv_obj_t* obj_dp)
{
    return obj_dp->drag_throw_en == 0 ? false : true;
}

/**
 * Get the drag parent attribute of an object
 * @param obj_dp pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_drag_parent(lv_obj_t* obj_dp)
{
    return obj_dp->drag_parent == 0 ? false : true;
}

/**
 * Get the signal function of an object
 * @param obj_dp pointer to an object
 * @return the signal function
 */
lv_signal_f_t  lv_obj_get_signal_f(lv_obj_t* obj_dp)
{
    return obj_dp->signal_f;
}

/**
 * Get the design function of an object
 * @param obj_dp pointer to an object
 * @return the design function
 */
lv_design_f_t lv_obj_get_design_f(lv_obj_t* obj_dp)
{
    return obj_dp->design_f;
}


/*------------------
 * Other get
 *-----------------*/

/**
 * Get the ext pointer
 * @param obj_dp pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext)->data1
 */
void * lv_obj_get_ext(lv_obj_t* obj_dp)
{
   return obj_dp->ext_dp;
}


/**
 * Get the free number
 * @param obj_dp pointer to an object
 * @return the free number
 */
uint8_t lv_obj_get_free_num(lv_obj_t* obj_dp)
{
    return obj_dp->free_num;
}

#if LV_OBJ_FREE_P != 0
/**
 * Get the free pointer
 * @param obj_dp pointer to an object
 * @return the free pointer
 */
void * lv_obj_get_free_p(lv_obj_t* obj_dp)
{
    return obj_dp->free_p;
}
#endif

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Handle the drawing related tasks of the base objects.
 * @param obj_dp pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 * @param return true/false, depends on 'mode'        
 */
static bool lv_obj_design(lv_obj_t* obj_dp, const  area_t * mask_p, lv_design_mode_t mode)
{
    if(mode == LV_DESIGN_COVER_CHK) {
        bool cover;
    	cover = area_is_in(mask_p, &obj_dp->cords);
        return cover;
    }

    lv_objs_t * objs_p = lv_obj_get_style(obj_dp);
    
    opa_t opa = lv_obj_get_opa(obj_dp);
    color_t color = objs_p->color;

    /*Simply draw a rectangle*/
#if LV_VDB_SIZE == 0
    lv_rfill(&obj_dp->cords, mask_p, color, opa);
#else
    lv_vfill(&obj_dp->cords, mask_p, color, opa);
#endif
    
    return true;
}

/**
 * Reposition the children of an object. (Called recursively)
 * @param obj_dp pointer to an object which children will be repositioned
 * @param x_diff x coordinate shift
 * @param y_diff y coordinate shift
 */
static void lv_obj_pos_child_refr(lv_obj_t* obj_dp, cord_t x_diff, cord_t y_diff)
{
    lv_obj_t* i;   
    LL_READ(obj_dp->child_ll, i) {
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
 * @param obj_dp pointer to an object
 */
static void lv_style_refr_core(void * style_p, lv_obj_t* obj_dp)
{
    lv_obj_t* i;
    LL_READ(obj_dp->child_ll, i) {
        if(i->style_p == style_p || style_p == NULL) {
            i->signal_f(i, LV_SIGNAL_STYLE_CHG, NULL);
            lv_obj_inv(i);
        }
        
        lv_style_refr_core(style_p, i);
    }
}


/**
 * Refresh the layout of an object
 * @param obj_dp pointer to an object which layout should be refreshed
 */
static void lv_obj_refr_layout(lv_obj_t * obj_dp)
{
	lv_layout_t type = obj_dp->layout_type;

	if(type == LV_LAYOUT_OFF) return;

	if(type == LV_LAYOUT_COL_L || type == LV_LAYOUT_COL_M || type == LV_LAYOUT_COL_R) {
		lv_layout_col(obj_dp);
	} else if(type == LV_LAYOUT_ROW_T || type == LV_LAYOUT_ROW_M || type == LV_LAYOUT_ROW_B) {
		lv_layout_row(obj_dp);
	}
}


/**
 * Handle column type layouts
 * @param obj_dp pointer to an object which layout should be handled
 */
static void lv_layout_col(lv_obj_t * obj_dp)
{
	lv_layout_t type = lv_obj_get_layout(obj_dp);
	cord_t space = lv_obj_get_layout_space(obj_dp); /*Space between objects*/
	cord_t margin = abs(space); 					   /*Margin by the parent*/

	lv_obj_t * child;
	/*Recalculate space in justified mode*/
	if(space < 0) {
		uint32_t h_tot = 0;
		uint32_t obj_cnt = 0;
		LL_READ(obj_dp->child_ll, child) {
			h_tot += lv_obj_get_height(child);
			obj_cnt ++;
		}

		if(obj_cnt == 0) return;
		space = lv_obj_get_height(parent_dp) - h_tot;

		if(obj_cnt == 5) {
			obj_cnt = 5;
		}

		space = space / (cord_t)obj_cnt;
	}

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	switch(type) {
		case LV_LAYOUT_COL_L:
			align = LV_ALIGN_IN_TOP_LEFT;
			margin = margin / 2;
			break;
		case LV_LAYOUT_COL_M:
			align = LV_ALIGN_IN_TOP_MID;
			margin = 0;
			break;
		case LV_LAYOUT_COL_R:
			align = LV_ALIGN_IN_TOP_RIGHT;
			margin = -(margin / 2);
			break;
		default:
			align = LV_ALIGN_IN_TOP_LEFT;
			margin = 0;
			break;
	}

	cord_t last_cord = space / 2;
	LL_READ_BACK(obj_dp->child_ll, child) {
		lv_obj_align(child, parent_dp, align, margin , last_cord);
		last_cord += lv_obj_get_height(child) + space;
	}
}

/**
 * Handle row type layouts
 * @param obj_dp pointer to an object which layout should be handled
 */
static void lv_layout_row(lv_obj_t * obj_dp)
{
	lv_layout_t type = lv_obj_get_layout(obj_dp);
	cord_t space = lv_obj_get_layout_space(obj_dp); /*Space between objects*/
	cord_t margin = abs(space); 					   /*Margin by the parent*/

	lv_obj_t * child;
	/*Recalculate space in justified mode*/
	if(space < 0) {
		uint32_t w_tot = 0;
		uint32_t obj_cnt = 0;
		LL_READ(obj_dp->child_ll, child) {
			w_tot += lv_obj_get_width(child);
			obj_cnt ++;
		}

		if(obj_cnt == 0) return;
		space = lv_obj_get_width(obj_dp) - w_tot;
		space = space / (cord_t)obj_cnt;
	}

	/*Adjust margin and get the alignment type*/
	lv_align_t align;
	switch(type) {
		case LV_LAYOUT_ROW_T:
			align = LV_ALIGN_IN_TOP_LEFT;
			margin = margin / 2;
			break;
		case LV_LAYOUT_ROW_M:
			align = LV_ALIGN_IN_LEFT_MID;
			margin = 0;
			break;
		case LV_LAYOUT_ROW_B:
			align = LV_ALIGN_IN_BOTTOM_LEFT;
			margin = -(margin / 2);
			break;
		default:
			align = LV_ALIGN_IN_TOP_LEFT;
			margin = 0;
			break;
	}

	cord_t last_cord = space / 2;
	LL_READ_BACK(obj_dp->child_ll, child) {
		lv_obj_align(child, obj_dp, align, last_cord, margin);
		last_cord += lv_obj_get_width(child) + space;
	}
}

