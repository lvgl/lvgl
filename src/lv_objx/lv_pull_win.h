/**
* @file lv_pull_win.h
*
*/

#ifndef LV_PULL_WIN_H
#define LV_PULL_WIN_H

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

#if LV_USE_PULL_WIN != 0

#include "../lv_objx/lv_page.h"

	/*********************
	*      DEFINES
	*********************/

	/**********************
	*      TYPEDEFS
	**********************/

	/*Styles*/
	enum {
		LV_PULL_WIN_DIR_TOP,
		LV_PULL_WIN_DIR_BOTTOM,
		LV_PULL_WIN_DIR_LEFT,
		LV_PULL_WIN_DIR_RIGHT,
	};
	typedef uint8_t lv_pull_win_dir_t;

	/*Styles*/
	enum {
		LV_PULL_WIN_STYLE_MAIN,
		LV_PULL_WIN_STYLE_BG,
	};
	typedef uint8_t lv_pull_win_style_t;

	/*Data of pull_win*/
	typedef struct
	{
		lv_page_ext_t page;
		/*New data for this type */
		lv_point_t valid_pos[2];
		uint16_t valid_pos_cnt;

		lv_obj_t *page0;
		lv_obj_t *page1;
		lv_pull_win_dir_t dir;
#if LV_USE_ANIMATION
		uint16_t anim_time;
#endif
		lv_point_t act_id;
		uint8_t drag_top_en : 1;
		uint8_t drag_bottom_en : 1;
		uint8_t drag_left_en : 1;
		uint8_t drag_right_en : 1;
		uint8_t drag_hor : 1;
		uint8_t drag_ver : 1;
	} lv_pull_win_ext_t;



	/**********************
	* GLOBAL PROTOTYPES
	**********************/

	/**
	* Create a pull_win objects
	* @param par pointer to an object, it will be the parent of the new pull_win
	* @param copy pointer to a pull_win object, if not NULL then the new object will be copied from it
	* @return pointer to the created pull_win
	*/
	lv_obj_t * lv_pull_win_create(lv_obj_t * par, const lv_obj_t * copy);

	/*======================
	* Add/remove functions
	*=====================*/

	/*=====================
	* Setter functions
	*====================*/
	/**
	* Set the size of an object
	* @param obj pointer to an object
	* @param w new width
	* @param h new height
	*/
	void lv_pull_win_set_size(lv_obj_t * pull_win, lv_coord_t w, lv_coord_t h);
	/**
	* Set the pull_win direct.
	* @param pull_win pointer to a Pull window object
	* @param dir pull direction
	*/
	void lv_pull_win_set_dir(lv_obj_t * pull_win, lv_pull_win_dir_t dir);
	/**
	* Set the pull_win align.
	* @param pull_win pointer to a Pull window object
	*/
	void lv_pull_win_set_align(lv_obj_t * pull_win);
	/**
	* Set the tile to be shown
	* @param pull_win pointer to a pull_win object
	* @param isShow show or not show
	* @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
	*/
	void lv_pull_win_set_show(lv_obj_t * pull_win, bool isShow, lv_anim_enable_t anim);

	/**
	* Set the animation time for the Tile view
	* @param pull_win pointer to a page object
	* @param anim_time animation time in milliseconds
	*/
	static inline void lv_pull_win_set_anim_time(lv_obj_t * pull_win, uint16_t anim_time)
	{
		lv_page_set_anim_time(pull_win, anim_time);
	}

	/**
	* Set a style of a pull_win.
	* @param pull_win pointer to pull_win object
	* @param type which style should be set
	* @param style pointer to a style
	*/
	void lv_pull_win_set_style(lv_obj_t * pull_win, lv_pull_win_style_t type, const lv_style_t * style);

	/*=====================
	* Getter functions
	*====================*/
	/**
	* Get show page object,for adding object and set object prarent
	* @param pull_win pointer to a Pull window object
	* @return
	*/
	lv_obj_t * lv_pull_win_get_obj(lv_obj_t * pull_win);
	/**
	* Get the animation time for the Tile view
	* @param pull_win pointer to a page object
	* @return animation time in milliseconds
	*/
	static inline uint16_t lv_pull_win_get_anim_time(lv_obj_t * pull_win)
	{
		return lv_page_get_anim_time(pull_win);
	}

	/**
	* Get style of a pull_win.
	* @param pull_win pointer to pull_win object
	* @param type which style should be get
	* @return style pointer to the style
	*/
	const lv_style_t * lv_pull_win_get_style(const lv_obj_t * pull_win, lv_pull_win_style_t type);

	/*=====================
	* Other functions
	*====================*/

	/**********************
	*      MACROS
	**********************/

#endif /*LV_USE_PULL_WIN*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_PULL_WIN_H*/
