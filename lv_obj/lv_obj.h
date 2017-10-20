/**
 * @file lv_obj.h
 * 
 */

#ifndef LV_OBJ_H
#define LV_OBJ_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#include <stddef.h>
#include <stdbool.h>
#include "misc/gfx/area.h"
#include "misc/mem/dyn_mem.h"
#include "misc/mem/linked_list.h"
#include "misc/gfx/color.h"
#include "lv_style.h"

/*********************
 *      DEFINES
 *********************/

/*Error check of lv_conf.h*/
#if LV_HOR_RES == 0 || LV_VER_RES == 0
#error "LV: LV_HOR_RES and LV_VER_RES must be greater then 0"
#endif

#if LV_ANTIALIAS != 0 && LV_ANTIALIAS != 1
#error "LV: LV_ATIALIAS can be only 0 or 1"
#endif

#if LV_VDB_SIZE == 0 && LV_ANTIALIAS != 0
#error "LV: If LV_VDB_SIZE == 0 the antialaissing must be disabled"
#endif

#if LV_VDB_SIZE != 0 && LV_VDB_SIZE < LV_HOR_RES && LV_ANTIALIAS == 0
#error "LV: Small Virtual Display Buffer (lv_conf.h: LV_VDB_SIZE >= LV_HOR_RES)"
#endif

#if LV_VDB_SIZE != 0 && LV_VDB_SIZE < 2 *LV_HOR_RES && LV_ANTIALIAS != 0
#error "LV: Small Virtual Display Buffer (lv_conf.h: LV_VDB_SIZE >= (2 * LV_HOR_RES))"
#endif

/*New defines*/
#define LV_OBJ_DEF_WIDTH  (80 << LV_ANTIALIAS)
#define LV_OBJ_DEF_HEIGHT  (60 << LV_ANTIALIAS)

#define ANIM_IN					0x00	/*Animation to show an object. 'OR' it with lv_anim_builtin_t*/
#define ANIM_OUT				0x80    /*Animation to hide an object. 'OR' it with lv_anim_builtin_t*/
#define ANIM_DIR_MASK			0x80	/*ANIM_IN/ANIM_OUT mask*/

/**********************
 *      TYPEDEFS
 **********************/

struct __LV_OBJ_T;

typedef enum
{
    LV_DESIGN_DRAW_MAIN,
    LV_DESIGN_DRAW_POST,
    LV_DESIGN_COVER_CHK,
}lv_design_mode_t;

typedef bool (* lv_design_func_t) (struct __LV_OBJ_T * obj, const area_t * mask_p, lv_design_mode_t mode);

typedef enum
{
    /*General signals*/
	LV_SIGNAL_CLEANUP,
    LV_SIGNAL_CHILD_CHG,
    LV_SIGNAL_CORD_CHG,
    LV_SIGNAL_STYLE_CHG,
	LV_SIGNAL_REFR_EXT_SIZE,

	/*Input device related*/
    LV_SIGNAL_PRESSED,
    LV_SIGNAL_PRESSING,
    LV_SIGNAL_PRESS_LOST,
    LV_SIGNAL_RELEASED,
    LV_SIGNAL_LONG_PRESS,
    LV_SIGNAL_LONG_PRESS_REP,
    LV_SIGNAL_DRAG_BEGIN,
    LV_SIGNAL_DRAG_END,

	/*Group related*/
    LV_SIGNAL_FOCUS,
    LV_SIGNAL_DEFOCUS,
    LV_SIGNAL_CONTROLL,
}lv_signal_t;

typedef bool (* lv_signal_func_t) (struct __LV_OBJ_T * obj, lv_signal_t sign, void * param);

typedef struct __LV_OBJ_T
{
    struct __LV_OBJ_T * par;    /*Pointer to the parent object*/
    ll_dsc_t child_ll;          /*Linked list to store the children objects*/
    
    area_t coords;               /*Coordinates of the object (x1, y1, x2, y2)*/

    lv_signal_func_t signal_func;     /*Object type specific signal function*/
    lv_design_func_t design_func;     /*Object type specific design function*/
    
    void * ext_attr;                 /*Object type specific extended data*/
    lv_style_t * style_p;       /*Pointer to the object's style*/

#if LV_OBJ_FREE_PTR != 0
    void * free_ptr;              /*Application specific pointer (set it freely)*/
#endif

    void * group_p;             /*Pointer to the group of the object*/

    /*Attributes and states*/
    uint8_t click     :1;    /*1: Can be pressed by an input device*/
    uint8_t drag      :1;    /*1: Enable the dragging*/
    uint8_t drag_throw:1;    /*1: Enable throwing with drag*/
    uint8_t drag_parent  :1;    /*1: Parent will be dragged instead*/
    uint8_t hidden       :1;    /*1: Object is hidden*/
    uint8_t top       :1;    /*1: If the object or its children is clicked it goes to the foreground*/
    uint8_t reserved     :1;

    uint8_t protect;            /*Automatically happening actions can be prevented. 'OR'ed values from lv_obj_prot_t*/

    cord_t ext_size;			/*EXTtend the size of the object in every direction. E.g. for shadow drawing*/

#if LV_OBJ_FREE_NUM != 0
    uint8_t free_num; 		    /*Application specific identifier (set it freely)*/
#endif
}lv_obj_t;

/*Protect some attributes (max. 8 bit)*/
typedef enum
{
    LV_PROTECT_NONE      = 0x00,
    LV_PROTECT_CHILD_CHG = 0x01, /*Disable the child change signal. Used by the library*/
    LV_PROTECT_PARENT    = 0x02, /*Prevent automatic parent change (e.g. in lv_page)*/
    LV_PROTECT_POS       = 0x04, /*Prevent automatic positioning (e.g. in lv_cont layout)*/
    LV_PROTECT_FOLLOW    = 0x08, /*Prevent the object be followed in automatic ordering (e.g. in lv_cont PRETTY layout)*/
}lv_protect_t;

typedef enum
{
    LV_ALIGN_CENTER = 0,
    LV_ALIGN_IN_TOP_LEFT,
    LV_ALIGN_IN_TOP_MID,
    LV_ALIGN_IN_TOP_RIGHT,
    LV_ALIGN_IN_BOTTOM_LEFT,
    LV_ALIGN_IN_BOTTOM_MID,
    LV_ALIGN_IN_BOTTOM_RIGHT,
    LV_ALIGN_IN_LEFT_MID,
    LV_ALIGN_IN_RIGHT_MID,
    LV_ALIGN_OUT_TOP_LEFT,
    LV_ALIGN_OUT_TOP_MID,
    LV_ALIGN_OUT_TOP_RIGHT,
    LV_ALIGN_OUT_BOTTOM_LEFT,
    LV_ALIGN_OUT_BOTTOM_MID,
    LV_ALIGN_OUT_BOTTOM_RIGHT,
    LV_ALIGN_OUT_LEFT_TOP,
    LV_ALIGN_OUT_LEFT_MID,
    LV_ALIGN_OUT_LEFT_BOTTOM,
    LV_ALIGN_OUT_RIGHT_TOP,
    LV_ALIGN_OUT_RIGHT_MID,
    LV_ALIGN_OUT_RIGHT_BOTTOM,
}lv_align_t;

typedef enum
{
	LV_ANIM_NONE = 0,
	LV_ANIM_FLOAT_TOP, 		/*Float from/to the top*/
	LV_ANIM_FLOAT_LEFT,		/*Float from/to the left*/
	LV_ANIM_FLOAT_BOTTOM,	/*Float from/to the bottom*/
	LV_ANIM_FLOAT_RIGHT,	/*Float from/to the right*/
	LV_ANIM_GROW_H,			/*Grow/shrink  horizontally*/
	LV_ANIM_GROW_V,			/*Grow/shrink  vertically*/
}lv_anim_builtin_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Init. the 'lv' library.
 */
void lv_init(void);

/**
 * Create a basic object
 * @param parent pointer to a parent object.
 *                  If NULL then a screen will be created
 * @param copy pointer to a base object, if not NULL then the new object will be copied from it
 * @return pointer to the new object
 */
lv_obj_t * lv_obj_create(lv_obj_t * parent, lv_obj_t * copy);

/**
 * Delete 'obj' and all of its children
 * @param obj pointer to an object to delete
 */
void lv_obj_del(lv_obj_t * obj);

/**
 * Signal function of the basic object
 * @param obj pointer to an object
 * @param sign signal type
 * @param param parameter for the signal (depends on signal type)
 * @return false: the object become invalid (e.g. deleted)
 */
bool lv_obj_signal(lv_obj_t * obj, lv_signal_t sign, void * param);

/**
 * Mark the object as invalid therefore its current position will be redrawn by 'lv_refr_task'
 * @param obj pointer to an object
 */
void lv_obj_invalidate(lv_obj_t * obj);

/**
 * Load a new screen
 * @param scr pointer to a screen
 */
void lv_scr_load(lv_obj_t * scr);

/**
 * Set a new parent for an object. Its relative position will be the same.
 * @param obj pointer to an object
 * @param parent pointer to the new parent object
 */
void lv_obj_set_parent(lv_obj_t * obj, lv_obj_t * parent);

/**
 * Set relative the position of an object (relative to the parent)
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent
 * @param y new distance from the top of the parent
 */
void lv_obj_set_pos(lv_obj_t * obj, cord_t x, cord_t y);

/**
 * Set relative the position of an object (relative to the parent).
 * The coordinates will be upscaled with LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param x new distance from the left side of the parent. (will be multiplied with LV_DOWNSCALE)
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_pos_scale(lv_obj_t * obj, cord_t x, cord_t y);

/**
 * Set the x coordinate of a object
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent
 */
void lv_obj_set_x(lv_obj_t * obj, cord_t x);

/**
 * Set the x coordinate of a object.
 * The coordinate will be upscaled with  LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param x new distance from the left side from the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_x_scale(lv_obj_t * obj, cord_t x);

/**
 * Set the y coordinate of a object
 * @param obj pointer to an object
 * @param y new distance from the top of the parent
 */
void lv_obj_set_y(lv_obj_t * obj, cord_t y);

/**
 * Set the y coordinate of a object.
 * The coordinate will be upscaled with LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param y new distance from the top of the parent. (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_y_scale(lv_obj_t * obj, cord_t y);

/**
 * Set the size of an object
 * @param obj pointer to an object
 * @param w new width
 * @param h new height
 */
void lv_obj_set_size(lv_obj_t * obj, cord_t w, cord_t h);

/**
 * Set the size of an object. The coordinates will be upscaled with  LV_DOWNSCALE.
 * @param obj pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_size_scale(lv_obj_t * obj, cord_t w, cord_t h);

/**
 * Set the width of an object
 * @param obj pointer to an object
 * @param w new width
 */
void lv_obj_set_width(lv_obj_t * obj, cord_t w);

/**
 * Set the width of an object.  The width will be upscaled with  LV_DOWNSCALE
 * @param obj pointer to an object
 * @param w new width (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_width_scale(lv_obj_t * obj, cord_t w);

/**
 * Set the height of an object
 * @param obj pointer to an object
 * @param h new height
 */
void lv_obj_set_height(lv_obj_t * obj, cord_t h);

/**
 * Set the height of an object.  The height will be upscaled with  LV_DOWNSCALE
 * @param obj pointer to an object
 * @param h new height (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_set_height_scale(lv_obj_t * obj, cord_t h);

/**
 * Align an object to an other object.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment
 * @param y_mod y coordinate shift after alignment
 */
void lv_obj_align(lv_obj_t * obj,lv_obj_t * base, lv_align_t align, cord_t x_mod, cord_t y_mod);

/**
 * Align an object to an other object. The coordinates will be upscaled with  LV_DOWNSCALE.
 * @param obj pointer to an object to align
 * @param base pointer to an object (if NULL the parent is used). 'obj' will be aligned to it.
 * @param align type of alignment (see 'lv_align_t' enum)
 * @param x_mod x coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 * @param y_mod y coordinate shift after alignment (will be multiplied with LV_DOWNSCALE)
 */
void lv_obj_align_scale(lv_obj_t * obj,lv_obj_t * base, lv_align_t align, cord_t x_mod, cord_t y_mod);

/**
 * Set the extended size of an object
 * @param obj pointer to an object
 * @param ext_size the extended size
 */
void lv_obj_set_ext_size(lv_obj_t * obj, cord_t ext_size);

/**
 * Set a new style for an object
 * @param obj pointer to an object
 * @param style_p pointer to the new style
 */
void lv_obj_set_style(lv_obj_t * obj, lv_style_t * style);

/**
 * Notify an object about its style is modified
 * @param obj pointer to an object
 */
void lv_obj_refresh_style(lv_obj_t * obj);

/**
 * Notify all object if a style is modified
 * @param style pointer to a style. Only the objects with this style will be notified
 *               (NULL to notify all objects)
 */
void lv_style_refr_objs(void * style);

/**
 * Hide an object. It won't be visible and clickable.
 * @param obj pointer to an object
 * @param en true: hide the object
 */
void lv_obj_set_hidden(lv_obj_t * obj, bool en);

/**
 * Enable or disable the clicking of an object
 * @param obj pointer to an object
 * @param en true: make the object clickable
 */
void lv_obj_set_click(lv_obj_t * obj, bool en);

/**
 * Enable to bring this object to the foreground if it
 * or any of its children is clicked
 * @param obj pointer to an object
 * @param en true: enable the auto top feature
 */
void lv_obj_set_top(lv_obj_t * obj, bool en);

/**
 * Enable the dragging of an object
 * @param obj pointer to an object
 * @param en true: make the object dragable
 */
void lv_obj_set_drag(lv_obj_t * obj, bool en);

/**
 * Enable the throwing of an object after is is dragged
 * @param obj pointer to an object
 * @param en true: enable the drag throw
 */
void lv_obj_set_drag_throw(lv_obj_t * obj, bool en);

/**
 * Enable to use parent for drag related operations.
 * If trying to drag the object the parent will be moved instead
 * @param obj pointer to an object
 * @param en true: enable the 'drag parent' for the object
 */
void lv_obj_set_drag_parent(lv_obj_t * obj, bool en);

/**
 * Set a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from lv_obj_prot_t
 */
void lv_obj_set_protect(lv_obj_t * obj, uint8_t prot);

/**
 * Clear a bit or bits in the protect filed
 * @param obj pointer to an object
 * @param prot 'OR'-ed values from lv_obj_prot_t
 */
void lv_obj_clear_protect(lv_obj_t * obj, uint8_t prot);

/**
 * Set the signal function of an object.
 * Always call the previous signal function in the new.
 * @param obj pointer to an object
 * @param fp the new signal function
 */
void lv_obj_set_signal_func(lv_obj_t * obj, lv_signal_func_t fp);

/**
 * Set a new design function for an object
 * @param obj pointer to an object
 * @param fp the new design function
 */
void lv_obj_set_design_func(lv_obj_t * obj, lv_design_func_t fp);

/**
 * Allocate a new ext. data for an object
 * @param obj pointer to an object
 * @param ext_size the size of the new ext. data
 * @return Normal pointer to the allocated ext
 */
void * lv_obj_allocate_ext_attr(lv_obj_t * obj, uint16_t ext_size);

/**
 * Send a 'LV_SIGNAL_REFR_EXT_SIZE' signal to the object
 * @param obj pointer to an object
 */
void lv_obj_refresh_ext_size(lv_obj_t * obj);

#if LV_OBJ_FREE_NUM != 0
/**
 * Set an application specific number for an object.
 * It can help to identify objects in the application.
 * @param obj pointer to an object
 * @param free_num the new free number
 */
void lv_obj_set_free_number(lv_obj_t * obj, uint8_t free_number);
#endif

#if LV_OBJ_FREE_PTR != 0
/**
 * Set an application specific  pointer for an object.
 * It can help to identify objects in the application.
 * @param obj pointer to an object
 * @param free_p the new free pinter
 */
void lv_obj_set_free_pointer(lv_obj_t * obj, void * free_pointer);
#endif

/**
 * Animate an object
 * @param obj pointer to an object to animate
 * @param type type of animation from 'lv_anim_builtin_t'. 'OR' it with ANIM_IN or ANIM_OUT
 * @param time time of animation in milliseconds
 * @param delay delay before the animation in milliseconds
 * @param cb a function to call when the animation is ready
 */
void lv_obj_animate(lv_obj_t * obj, lv_anim_builtin_t type, uint16_t time, uint16_t delay, void (*cb) (lv_obj_t *));

/**
 * Return with the actual screen
 * @return pointer to to the actual screen object
 */
lv_obj_t * lv_scr_act(void);lv_obj_t * lv_layer_top(void);

/**
 * Return with the system layer. (Same on every screen and it is above the all other layers)
 * It is used for example by the cursor
 * @return pointer to the system layer object (transparent screen sized lv_obj)
 */
lv_obj_t * lv_layer_sys(void);

/**
 * Return with the screen of an object
 * @param obj pointer to an object
 * @return pointer to a screen
 */
lv_obj_t * lv_obj_get_screen(lv_obj_t * obj);

/**
 * Returns with the parent of an object
 * @param obj pointer to an object
 * @return pointer to the parent of  'obj'
 */
lv_obj_t * lv_obj_get_parent(lv_obj_t * obj);

/**
 * Iterate through the children of an object
 * @param obj pointer to an object
 * @param child NULL at first call to get the next children
 *                  and the previous return value later
 * @return the child after 'act_child' or NULL if no more child
 */
lv_obj_t * lv_obj_get_child(lv_obj_t * obj, lv_obj_t * child);

/**
 * Count the children of an object (only children directly on 'obj')
 * @param obj pointer to an object
 * @return children number of 'obj'
 */
uint16_t lv_obj_count_children(lv_obj_t * obj);

/**
 * Copy the coordinates of an object to an area
 * @param obj pointer to an object
 * @param cords_p pointer to an area to store the coordinates
 */
void lv_obj_get_coords(lv_obj_t * obj, area_t * cords_p);

/**
 * Get the x coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the left side of its parent
 */
cord_t lv_obj_get_x(lv_obj_t * obj);

/**
 * Get the y coordinate of object
 * @param obj pointer to an object
 * @return distance of 'obj' from the top of its parent
 */
cord_t lv_obj_get_y(lv_obj_t * obj);

/**
 * Get the width of an object
 * @param obj pointer to an object
 * @return the width
 */
cord_t lv_obj_get_width(lv_obj_t * obj);

/**
 * Get the height of an object
 * @param obj pointer to an object
 * @return the height
 */
cord_t lv_obj_get_height(lv_obj_t * obj);

/**
 * Get the extended size attribute of an object
 * @param obj pointer to an object
 * @return the extended size attribute
 */
cord_t lv_obj_get_ext_size(lv_obj_t * obj);

/**
 * Get the style pointer of an object (if NULL get style of the parent)
 * @param obj pointer to an object
 * @return pointer to a style
 */
lv_style_t * lv_obj_get_style(lv_obj_t * obj);

/**
 * Get the hidden attribute of an object
 * @param obj pointer to an object
 * @return true: the object is hidden
 */
bool lv_obj_get_hidden(lv_obj_t * obj);

/**
 * Get the click enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is clickable
 */
bool lv_obj_get_click(lv_obj_t * obj);

/**
 * Get the top enable attribute of an object
 * @param obj pointer to an object
 * @return true: the auto top feture is enabled
 */
bool lv_obj_get_top(lv_obj_t * obj);

/**
 * Get the drag enable attribute of an object
 * @param obj pointer to an object
 * @return true: the object is dragable
 */
bool lv_obj_get_drag(lv_obj_t * obj);

/**
 * Get the drag thow enable attribute of an object
 * @param obj pointer to an object
 * @return true: drag throw is enabled
 */
bool lv_obj_get_drag_throw(lv_obj_t * obj);

/**
 * Get the drag parent attribute of an object
 * @param obj pointer to an object
 * @return true: drag parent is enabled
 */
bool lv_obj_get_drag_parent(lv_obj_t * obj);

/**
 * Get the protect field of an object
 * @param obj pointer to an object
 * @return protect field ('OR'ed values of lv_obj_prot_t)
 */
uint8_t lv_obj_get_protect(lv_obj_t * obj);

/**
 * Check at least one bit of a given protect bitfield is set
 * @param obj pointer to an object
 * @param prot protect bits to test ('OR'ed values of lv_obj_prot_t)
 * @return false: none of the given bits are set, true: at least one bit is set
 */
bool lv_obj_is_protected(lv_obj_t * obj, uint8_t prot);

/**
 * Get the signal function of an object
 * @param obj pointer to an object
 * @return the signal function
 */
lv_signal_func_t   lv_obj_get_signal_func(lv_obj_t * obj);

/**
 * Get the design function of an object
 * @param obj pointer to an object
 * @return the design function
 */
lv_design_func_t lv_obj_get_design_func(lv_obj_t * obj);

/**
 * Get the ext pointer
 * @param obj pointer to an object
 * @return the ext pointer but not the dynamic version
 *         Use it as ext->data1, and NOT da(ext_attr)->data1
 */
void * lv_obj_get_ext_attr(lv_obj_t * obj);

#if LV_OBJ_FREE_NUM != 0
/**
 * Get the free number
 * @param obj pointer to an object
 * @return the free number
 */
uint8_t lv_obj_get_free_num(lv_obj_t * obj);
#endif

#if LV_OBJ_FREE_PTR != 0
/**
 * Get the free pointer
 * @param obj pointer to an object
 * @return the free pointer
 */
void * lv_obj_get_free_p(lv_obj_t * obj);
#endif

#if LV_OBJ_GROUP != 0
/**
 * Get the group of the object
 * @param obj pointer to an object
 * @return the pointer to group of the object
 */
void * lv_obj_get_group(lv_obj_t * obj);
#endif

/**********************
 *      MACROS
 **********************/
#define LV_SCALE(x) (x << LV_ANTIALIAS)


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_OBJ_H*/
