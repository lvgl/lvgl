/**
 * @file lv_ddlist.h
 *
 */

#ifndef LV_DDLIST_H
#define LV_DDLIST_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#if LV_USE_DDLIST != 0

/*Testing of dependencies*/
#if LV_USE_PAGE == 0
#error "lv_ddlist: lv_page is required. Enable it in lv_conf.h (LV_USE_PAGE  1) "
#endif

#if LV_USE_LABEL == 0
#error "lv_ddlist: lv_label is required. Enable it in lv_conf.h (LV_USE_LABEL  1) "
#endif

#include "../lv_core/lv_obj.h"
#include "../lv_objx/lv_page.h"
#include "../lv_objx/lv_label.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
/*Data of drop down list*/
typedef struct
{
    lv_page_ext_t page; /*Ext. of ancestor*/
    /*New data for this type */
    lv_obj_t * label;             /*Label for the options*/
    const lv_style_t * sel_style; /*Style of the selected option*/
    uint16_t option_cnt;          /*Number of options*/
    uint16_t sel_opt_id;          /*Index of the current option*/
    uint16_t sel_opt_id_ori;      /*Store the original index on focus*/
    uint8_t opened : 1;           /*1: The list is opened (handled by the library)*/
    uint8_t force_sel : 1;        /*1: Keep the selection highlight even if the list is closed*/
    uint8_t draw_arrow : 1;       /*1: Draw arrow*/
    uint8_t stay_open : 1;        /*1: Don't close the list when a new item is selected*/
    lv_coord_t fix_height;        /*Height of the ddlist when opened. (0: auto-size)*/
} lv_ddlist_ext_t;

enum {
    LV_DDLIST_STYLE_BG,
    LV_DDLIST_STYLE_SEL,
    LV_DDLIST_STYLE_SB,
};
typedef uint8_t lv_ddlist_style_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
/**
 * Create a drop down list objects
 * @param par pointer to an object, it will be the parent of the new drop down list
 * @param copy pointer to a drop down list object, if not NULL then the new object will be copied
 * from it
 * @return pointer to the created drop down list
 */
lv_obj_t * lv_ddlist_create(lv_obj_t * par, const lv_obj_t * copy);

/*=====================
 * Setter functions
 *====================*/

/**
 * Set the options in a drop down list from a string
 * @param ddlist pointer to drop down list object
 * @param options a string with '\n' separated options. E.g. "One\nTwo\nThree"
 */
void lv_ddlist_set_options(lv_obj_t * ddlist, const char * options);

/**
 * Set the selected option
 * @param ddlist pointer to drop down list object
 * @param sel_opt id of the selected option (0 ... number of option - 1);
 */
void lv_ddlist_set_selected(lv_obj_t * ddlist, uint16_t sel_opt);

/**
 * Set a fix height for the drop down list
 * If 0 then the opened ddlist will be auto. sized else the set height will be applied.
 * @param ddlist pointer to a drop down list
 * @param h the height when the list is opened (0: auto size)
 */
void lv_ddlist_set_fix_height(lv_obj_t * ddlist, lv_coord_t h);

/**
 * Set a fix width for the drop down list
 * @param ddlist pointer to a drop down list
 * @param w the width when the list is opened (0: auto size)
 */
void lv_ddlist_set_fix_width(lv_obj_t * ddlist, lv_coord_t w);

/**
 * Set arrow draw in a drop down list
 * @param ddlist pointer to drop down list object
 * @param en enable/disable a arrow draw. E.g. "true" for draw.
 */
void lv_ddlist_set_draw_arrow(lv_obj_t * ddlist, bool en);

/**
 * Leave the list opened when a new value is selected
 * @param ddlist pointer to drop down list object
 * @param en enable/disable "stay open" feature
 */
void lv_ddlist_set_stay_open(lv_obj_t * ddlist, bool en);

/**
 * Set the scroll bar mode of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param sb_mode the new mode from 'lv_page_sb_mode_t' enum
 */
static inline void lv_ddlist_set_sb_mode(lv_obj_t * ddlist, lv_sb_mode_t mode)
{
    lv_page_set_sb_mode(ddlist, mode);
}
/**
 * Set the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @param anim_time: open/close animation time [ms]
 */
static inline void lv_ddlist_set_anim_time(lv_obj_t * ddlist, uint16_t anim_time)
{
    lv_page_set_anim_time(ddlist, anim_time);
}

/**
 * Set a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be set
 * @param style pointer to a style
 *  */
void lv_ddlist_set_style(lv_obj_t * ddlist, lv_ddlist_style_t type, const lv_style_t * style);

/**
 * Set the alignment of the labels in a drop down list
 * @param ddlist pointer to a drop down list object
 * @param align alignment of labels
 */
void lv_ddlist_set_align(lv_obj_t * ddlist, lv_label_align_t align);

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the options of a drop down list
 * @param ddlist pointer to drop down list object
 * @return the options separated by '\n'-s (E.g. "Option1\nOption2\nOption3")
 */
const char * lv_ddlist_get_options(const lv_obj_t * ddlist);

/**
 * Get the selected option
 * @param ddlist pointer to drop down list object
 * @return id of the selected option (0 ... number of option - 1);
 */
uint16_t lv_ddlist_get_selected(const lv_obj_t * ddlist);

/**
 * Get the current selected option as a string
 * @param ddlist pointer to ddlist object
 * @param buf pointer to an array to store the string
 * @param buf_size size of `buf` in bytes. 0: to ignore it.
 */
void lv_ddlist_get_selected_str(const lv_obj_t * ddlist, char * buf, uint16_t buf_size);

/**
 * Get the fix height value.
 * @param ddlist pointer to a drop down list object
 * @return the height if the ddlist is opened (0: auto size)
 */
lv_coord_t lv_ddlist_get_fix_height(const lv_obj_t * ddlist);

/**
 * Get arrow draw in a drop down list
 * @param ddlist pointer to drop down list object
 */
bool lv_ddlist_get_draw_arrow(lv_obj_t * ddlist);

/**
 * Get whether the drop down list stay open after selecting a  value or not
 * @param ddlist pointer to drop down list object
 */
bool lv_ddlist_get_stay_open(lv_obj_t * ddlist);

/**
 * Get the scroll bar mode of a drop down list
 * @param ddlist pointer to a  drop down list object
 * @return scrollbar mode from 'lv_page_sb_mode_t' enum
 */
static inline lv_sb_mode_t lv_ddlist_get_sb_mode(const lv_obj_t * ddlist)
{
    return lv_page_get_sb_mode(ddlist);
}

/**
 * Get the open/close animation time.
 * @param ddlist pointer to a drop down list
 * @return open/close animation time [ms]
 */
static inline uint16_t lv_ddlist_get_anim_time(const lv_obj_t * ddlist)
{
    return lv_page_get_anim_time(ddlist);
}

/**
 * Get a style of a drop down list
 * @param ddlist pointer to a drop down list object
 * @param type which style should be get
 * @return style pointer to a style
 */
const lv_style_t * lv_ddlist_get_style(const lv_obj_t * ddlist, lv_ddlist_style_t type);

/**
 * Get the alignment of the labels in a drop down list
 * @param ddlist pointer to a drop down list object
 * @return alignment of labels
 */
lv_label_align_t lv_ddlist_get_align(const lv_obj_t * ddlist);

/*=====================
 * Other functions
 *====================*/

/**
 * Open the drop down list with or without animation
 * @param ddlist pointer to drop down list object
 * @param anim_en LV_ANIM_ON: use animation; LV_ANOM_OFF: not use animations
 */
void lv_ddlist_open(lv_obj_t * ddlist, lv_anim_enable_t anim);

/**
 * Close (Collapse) the drop down list
 * @param ddlist pointer to drop down list object
 * @param anim_en LV_ANIM_ON: use animation; LV_ANOM_OFF: not use animations
 */
void lv_ddlist_close(lv_obj_t * ddlist, lv_anim_enable_t anim);

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_DDLIST*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DDLIST_H*/
