/**
* @file lv_pull_win.c
*
*/

/*********************
*      INCLUDES
*********************/
#include "lv_pull_win.h"
#if LV_USE_PULL_WIN != 0

#include <stdbool.h>
#include "lv_cont.h"
#include "../lv_core/lv_debug.h"
#include "../lv_themes/lv_theme.h"

/*********************
*      DEFINES
*********************/
#if LV_USE_ANIMATION
#ifndef LV_PULL_WIN_DEF_ANIM_TIME
#define LV_PULL_WIN_DEF_ANIM_TIME 300 /*Animation time loading a tile [ms] (0: no animation)  */
#endif
#else
#undef LV_PULL_WIN_DEF_ANIM_TIME
#define LV_PULL_WIN_DEF_ANIM_TIME 0 /*No animations*/
#endif

/**********************
*      TYPEDEFS
**********************/

/**********************
*  STATIC PROTOTYPES
**********************/
static lv_res_t lv_pull_win_signal(lv_obj_t * pull_win, lv_signal_t sign, void * param);
static lv_res_t lv_pull_win_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param);
static void pull_win_scrl_event_cb(lv_obj_t * scrl, lv_event_t event);
static void drag_end_handler(lv_obj_t * pull_win);
static bool set_valid_drag_dirs(lv_obj_t * pull_win);
static void lv_pull_win_add_page(lv_obj_t * pull_win, lv_obj_t * element);
static void lv_pull_win_set_tile_act(lv_obj_t * pull_win, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim);
/**********************
*  STATIC VARIABLES
**********************/
static lv_signal_cb_t ancestor_signal;
static lv_signal_cb_t ancestor_scrl_signal;
static lv_design_cb_t ancestor_design;

/**********************
*      MACROS
**********************/

/**********************
*   GLOBAL FUNCTIONS
**********************/

/**
* Create a pull_win object
* @param par pointer to an object, it will be the parent of the new pull_win
* @param copy pointer to a pull_win object, if not NULL then the new object will be copied from it
* @return pointer to the created pull_win
*/
lv_obj_t * lv_pull_win_create(lv_obj_t * par, const lv_obj_t * copy)
{
	LV_LOG_TRACE("pull_win create started");

	/*Create the ancestor of pull_win*/
	lv_obj_t * new_pull_win = lv_page_create(par, copy);
	LV_ASSERT_MEM(new_pull_win);
	if (new_pull_win == NULL) return NULL;

	/*Allocate the pull_win type specific extended data*/
	lv_pull_win_ext_t * ext = lv_obj_allocate_ext_attr(new_pull_win, sizeof(lv_pull_win_ext_t));
	LV_ASSERT_MEM(ext);
	if (ext == NULL) return NULL;
	if (ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(new_pull_win);
	if (ancestor_scrl_signal == NULL) ancestor_scrl_signal = lv_obj_get_signal_cb(lv_page_get_scrl(new_pull_win));
	if (ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(new_pull_win);

	/*Initialize the allocated 'ext' */
#if LV_USE_ANIMATION
	ext->anim_time = LV_PULL_WIN_DEF_ANIM_TIME;
#endif
	ext->act_id.x = 0;
	ext->act_id.y = 0;
	ext->valid_pos_cnt = 0;
	ext->page0 = NULL;
	ext->page1 = NULL;

	/*The signal and design functions are not copied so set them here*/
	lv_obj_set_signal_cb(new_pull_win, lv_pull_win_signal);
	lv_obj_set_signal_cb(lv_page_get_scrl(new_pull_win), lv_pull_win_scrl_signal);

	/*Init the new pull_win*/
	if (copy == NULL) {
		/* Set a size which fits into the parent.
		* Don't use `par` directly because if the pull_win is created on a page it is moved to the
		* scrollable so the parent has changed */
		lv_obj_set_size(new_pull_win, lv_obj_get_width_fit(lv_obj_get_parent(new_pull_win)),
						lv_obj_get_height_fit(lv_obj_get_parent(new_pull_win)));

		lv_obj_set_drag_throw(lv_page_get_scrl(new_pull_win), false);
		lv_page_set_scrl_fit(new_pull_win, LV_FIT_TIGHT);
		lv_obj_set_event_cb(ext->page.scrl, pull_win_scrl_event_cb);
		
		lv_page_set_sb_mode(new_pull_win, LV_SB_MODE_OFF);

		lv_theme_t * th = lv_theme_get_current();
		if (th) {
			lv_page_set_style(new_pull_win, LV_PAGE_STYLE_BG, th->style.pull_win.bg);
			lv_page_set_style(new_pull_win, LV_PAGE_STYLE_SCRL, th->style.pull_win.scrl);
			lv_page_set_style(new_pull_win, LV_PAGE_STYLE_SB, th->style.pull_win.sb);
		}
		else {
			lv_page_set_style(new_pull_win, LV_PAGE_STYLE_BG, &lv_style_transp_tight);
			lv_page_set_style(new_pull_win, LV_PAGE_STYLE_SCRL, &lv_style_transp_tight);
		}
		//lv_page_set_style(new_pull_win, LV_PAGE_STYLE_BG, &lv_style_transp);
	//	lv_page_set_style(new_pull_win, LV_PAGE_STYLE_SCRL, &lv_style_transp);
	}
	/*Copy an existing pull_win*/
	else {
		lv_pull_win_ext_t * copy_ext = lv_obj_get_ext_attr(copy);
		ext->act_id.x = copy_ext->act_id.x;
		ext->act_id.y = copy_ext->act_id.y;
		ext->valid_pos[0] = copy_ext->valid_pos[0];
		ext->valid_pos[1] = copy_ext->valid_pos[1];
		ext->valid_pos_cnt = copy_ext->valid_pos_cnt;
		ext->dir = copy_ext->dir;
#if LV_USE_ANIMATION
		ext->anim_time = copy_ext->anim_time;
#endif

		/*Refresh the style with new signal function*/
		lv_obj_refresh_style(new_pull_win);
	}

	lv_pull_win_set_dir(new_pull_win, LV_PULL_WIN_DIR_TOP);

	ext->page0 = lv_obj_create(new_pull_win, NULL);
	lv_obj_set_click(ext->page0, true);
	lv_obj_set_style(ext->page0, &lv_style_transp);
	lv_pull_win_add_page(new_pull_win, ext->page0);

	ext->page1 = lv_obj_create(new_pull_win, NULL);
	lv_obj_set_click(ext->page1, true);
	lv_obj_set_style(ext->page1, &lv_style_transp);
	lv_pull_win_add_page(new_pull_win, ext->page1);

	lv_pull_win_set_align(new_pull_win);

	lv_page_set_edge_flash(new_pull_win, false);
	lv_page_set_sb_mode(new_pull_win, LV_SB_MODE_OFF);
	LV_LOG_INFO("pull_win created");

	return new_pull_win;
}

/*======================
* Add/remove functions
*=====================*/

/**
* Register an object on the pull_win. The register object will able to slide the pull_win
* @param pull_win pointer to a Pull window object
* @param element pointer to an object
*/
static void lv_pull_win_add_page(lv_obj_t * pull_win, lv_obj_t * element)
{
	/* Let the objects event to propagate to the scrollable part of the pull_win.
	* It is required the handle dargging of the pull_win with the element.*/
	element->parent_event = 1;
	lv_obj_set_drag_parent(element, true);

	/* When adding a new element the coordinates may shift.
	* For example y=1 can become y=1 if an element is added to the top.
	* So be sure the current tile is correctly shown*/
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	lv_pull_win_set_tile_act(pull_win, ext->act_id.x, ext->act_id.y, false);
}

/*=====================
* Setter functions
*====================*/

/**
* Set the size of an object
* @param obj pointer to an object
* @param w new width
* @param h new height
*/
void lv_pull_win_set_size(lv_obj_t * pull_win, lv_coord_t w, lv_coord_t h)
{
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	lv_obj_set_size(pull_win, w, h);

	lv_obj_set_size(ext->page0, lv_obj_get_width(pull_win), lv_obj_get_height(pull_win));
	lv_obj_set_size(ext->page1, lv_obj_get_width(pull_win), lv_obj_get_height(pull_win));

	lv_pull_win_set_align(pull_win);
}
/**
* Set the pull_win direct.
* @param pull_win pointer to a Pull window object
* @param dir pull direction
*/
void lv_pull_win_set_dir(lv_obj_t * pull_win, lv_pull_win_dir_t dir)
{
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	ext->dir = dir;

	ext->valid_pos_cnt = 2;
	switch (ext->dir)
	{
	case LV_PULL_WIN_DIR_TOP:
		ext->valid_pos[0].x = 0; ext->valid_pos[0].y = 1;
		ext->valid_pos[1].x = 0; ext->valid_pos[1].y = 0;
		break;
	case LV_PULL_WIN_DIR_BOTTOM:
		ext->valid_pos[0].x = 0; ext->valid_pos[0].y = 0;
		ext->valid_pos[1].x = 0; ext->valid_pos[1].y = 1;
		break;
	case LV_PULL_WIN_DIR_LEFT:
		ext->valid_pos[0].x = 1; ext->valid_pos[0].y = 0;
		ext->valid_pos[1].x = 0; ext->valid_pos[1].y = 0;
		break;
	case LV_PULL_WIN_DIR_RIGHT:
		ext->valid_pos[0].x = 0; ext->valid_pos[0].y = 0;
		ext->valid_pos[1].x = 1; ext->valid_pos[1].y = 0;
		break;
	}
}

/**
* Set the pull_win align.
* @param pull_win pointer to a Pull window object
*/
void lv_pull_win_set_align(lv_obj_t * pull_win)
{
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);

	switch (ext->dir)
	{
	case LV_PULL_WIN_DIR_TOP:
		lv_obj_align(ext->page1, ext->page0, LV_ALIGN_OUT_TOP_MID, 0, 0);
		break;
	case LV_PULL_WIN_DIR_BOTTOM:
		lv_obj_align(ext->page1, ext->page0, LV_ALIGN_OUT_BOTTOM_MID, 0, 0);
		break;
	case LV_PULL_WIN_DIR_LEFT:
		lv_obj_align(ext->page1, ext->page0, LV_ALIGN_OUT_LEFT_MID, 0, 0);
		break;
	case LV_PULL_WIN_DIR_RIGHT:
		lv_obj_align(ext->page1, ext->page0, LV_ALIGN_OUT_RIGHT_MID, 0, 0);
		break;
	}
}
/**
* Set the tile to be shown
* @param pull_win pointer to a pull_win object
* @param isShow show or not show
* @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
*/
void lv_pull_win_set_show(lv_obj_t * pull_win, bool isShow, lv_anim_enable_t anim)
{
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);

	if (isShow)
		lv_pull_win_set_tile_act(pull_win, ext->valid_pos[1].x, ext->valid_pos[1].y, anim);
	else
		lv_pull_win_set_tile_act(pull_win, ext->valid_pos[0].x, ext->valid_pos[0].y, anim);
}
/**
* Set the tile to be shown
* @param pull_win pointer to a pull_win object
* @param x column id (0, 1, 2...)
* @param y line id (0, 1, 2...)
* @param anim LV_ANIM_ON: set the value with an animation; LV_ANIM_OFF: change the value immediately
*/
static void lv_pull_win_set_tile_act(lv_obj_t * pull_win, lv_coord_t x, lv_coord_t y, lv_anim_enable_t anim)
{
#if LV_USE_ANIMATION == 0
	anim = LV_ANIM_OFF;
#endif
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);

	uint32_t tile_id;
	bool valid = false;

	for (tile_id = 0; tile_id < ext->valid_pos_cnt; tile_id++) {
		if (ext->valid_pos[tile_id].x == x && ext->valid_pos[tile_id].y == y) {
			valid = true;
		}
	}

	if (valid == false) return; /*Don't load not valid tiles*/

	if ((x == ext->valid_pos[0].x) && (y == ext->valid_pos[0].y))
		lv_obj_move_background(pull_win);/*if on page0£¬move object to background*/
	ext->act_id.x = x;
	ext->act_id.y = y;

	lv_coord_t x_coord = -x * lv_obj_get_width(pull_win);
	lv_coord_t y_coord = -y * lv_obj_get_height(pull_win);
	lv_obj_t * scrl = lv_page_get_scrl(pull_win);
	if (anim) {
#if LV_USE_ANIMATION
		lv_coord_t x_act = lv_obj_get_x(scrl);
		lv_coord_t y_act = lv_obj_get_y(scrl);

		lv_anim_t a;
		a.var = scrl;
		a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
		a.path_cb = lv_anim_path_linear;
		a.ready_cb = NULL;
		a.act_time = 0;
		a.time = ext->anim_time;
		a.playback = 0;
		a.playback_pause = 0;
		a.repeat = 0;
		a.repeat_pause = 0;

		if (x_coord != x_act) {
			a.start = x_act;
			a.end = x_coord;
			lv_anim_create(&a);
		}

		if (y_coord != y_act) {
			a.start = y_act;
			a.end = y_coord;
			a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
			lv_anim_create(&a);
		}
#endif
	}
	else {
		lv_obj_set_pos(scrl, x_coord, y_coord);
	}

	lv_res_t res = LV_RES_OK;
	res = lv_event_send(pull_win, LV_EVENT_VALUE_CHANGED, &tile_id);
	if (res != LV_RES_OK) return; /*Prevent the tile loading*/
}

/**
* Set a style of a pull_win.
* @param pull_win pointer to pull_win object
* @param type which style should be set
* @param style pointer to a style
*/
void lv_pull_win_set_style(lv_obj_t * pull_win, lv_pull_win_style_t type, const lv_style_t * style)
{
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	switch (type) {
	case LV_PULL_WIN_STYLE_MAIN: 
		lv_obj_set_style(pull_win, style); 
		break;
	case LV_PULL_WIN_STYLE_BG:
		lv_obj_set_style(ext->page1, style);
		break;
	}
}

/*=====================
* Getter functions
*====================*/

/*
* New object specific "get" functions come here
*/
/**
* Get show page object,for adding object and set object prarent
* @param pull_win pointer to a Pull window object
* @return 
*/
lv_obj_t * lv_pull_win_get_obj(lv_obj_t * pull_win)
{
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	return ext->page1;
}
/**
* Get style of a pull_win.
* @param pull_win pointer to pull_win object
* @param type which style should be get
* @return style pointer to the style
*/
const lv_style_t * lv_pull_win_get_style(const lv_obj_t * pull_win, lv_pull_win_style_t type)
{
	const lv_style_t * style = NULL;
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	switch (type) {
	case LV_PULL_WIN_STYLE_MAIN: style = lv_obj_get_style(pull_win); break;
	case LV_PULL_WIN_STYLE_BG: style = lv_obj_get_style(ext->page1); break;
	default: style = NULL;
	}

	return style;
}

/*=====================
* Other functions
*====================*/

/*
* New object specific "other" functions come here
*/

/**********************
*   STATIC FUNCTIONS
**********************/

/**
* Signal function of the pull_win
* @param pull_win pointer to a pull_win object
* @param sign a signal type from lv_signal_t enum
* @param param pointer to a signal specific variable
* @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
*/
static lv_res_t lv_pull_win_signal(lv_obj_t * pull_win, lv_signal_t sign, void * param)
{
	lv_res_t res;

	/* Include the ancient signal function */
	res = ancestor_signal(pull_win, sign, param);
	if (res != LV_RES_OK) return res;

	if (sign == LV_SIGNAL_CLEANUP) {
		/*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
	}
	else if (sign == LV_SIGNAL_GET_TYPE) {
		lv_obj_type_t * buf = param;
		uint8_t i;
		for (i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
			if (buf->type[i] == NULL) break;
		}
		buf->type[i] = "lv_pull_win";
	}
	return res;
}

/**
* Signal function of the pull_win scrollable
* @param pull_win pointer to the scrollable part of the pull_win object
* @param sign a signal type from lv_signal_t enum
* @param param pointer to a signal specific variable
* @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
*/
static lv_res_t lv_pull_win_scrl_signal(lv_obj_t * scrl, lv_signal_t sign, void * param)
{

	lv_res_t res;

	/* Include the ancient signal function */
	res = ancestor_scrl_signal(scrl, sign, param);
	if (res != LV_RES_OK) return res;

	lv_obj_t * pull_win = lv_obj_get_parent(scrl);
	const lv_style_t * style_bg = lv_pull_win_get_style(pull_win, LV_PULL_WIN_STYLE_MAIN);

	/*Apply constraint on moving of the pull_win*/
	if (sign == LV_SIGNAL_CORD_CHG) {
		lv_indev_t * indev = lv_indev_get_act();
		if (indev) {
			lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);

			/*Set horizontal drag constraint if no vertical constraint an dragged to valid x
			* direction */
			if (ext->drag_ver == 0 &&
				((ext->drag_right_en && indev->proc.types.pointer.drag_sum.x <= -LV_INDEV_DEF_DRAG_LIMIT) ||
				(ext->drag_left_en && indev->proc.types.pointer.drag_sum.x >= LV_INDEV_DEF_DRAG_LIMIT))) {
				ext->drag_hor = 1;
			}
			/*Set vertical drag constraint if no horizontal constraint an dragged to valid y
			* direction */
			if (ext->drag_hor == 0 &&
				((ext->drag_bottom_en && indev->proc.types.pointer.drag_sum.y <= -LV_INDEV_DEF_DRAG_LIMIT) ||
				(ext->drag_top_en && indev->proc.types.pointer.drag_sum.y >= LV_INDEV_DEF_DRAG_LIMIT))) {
				ext->drag_ver = 1;
			}

#if LV_USE_ANIMATION
			if (ext->drag_hor) {
				ext->page.edge_flash.top_ip = 0;
				ext->page.edge_flash.bottom_ip = 0;
			}

			if (ext->drag_ver) {
				ext->page.edge_flash.right_ip = 0;
				ext->page.edge_flash.left_ip = 0;
			}
#endif

			lv_coord_t x = lv_obj_get_x(scrl);
			lv_coord_t y = lv_obj_get_y(scrl);
			lv_coord_t h = lv_obj_get_height(pull_win);
			lv_coord_t w = lv_obj_get_width(pull_win);
			if (ext->drag_top_en == 0) {
				if (y > -(ext->act_id.y * h) && indev->proc.types.pointer.vect.y > 0 && ext->drag_hor == 0) {
#if LV_USE_ANIMATION
					if (ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
						ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
						ext->page.edge_flash.bottom_ip == 0) {
						ext->page.edge_flash.top_ip = 1;
						lv_page_start_edge_flash(pull_win, LV_PAGE_EDGE_TOP);
					}
#endif

					lv_obj_set_y(scrl, -ext->act_id.y * h + style_bg->body.padding.top);
				}
			}
			if (ext->drag_bottom_en == 0 && indev->proc.types.pointer.vect.y < 0 && ext->drag_hor == 0) {
				if (y < -(ext->act_id.y * h)) {
#if LV_USE_ANIMATION
					if (ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
						ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
						ext->page.edge_flash.bottom_ip == 0) {
						ext->page.edge_flash.bottom_ip = 1;
						lv_page_start_edge_flash(pull_win, LV_PAGE_EDGE_BOTTOM);
					}
#endif
				}

				lv_obj_set_y(scrl, -ext->act_id.y * h + style_bg->body.padding.top);
			}
			if (ext->drag_left_en == 0) {
				if (x > -(ext->act_id.x * w) && indev->proc.types.pointer.vect.x > 0 && ext->drag_ver == 0) {
#if LV_USE_ANIMATION
					if (ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
						ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
						ext->page.edge_flash.bottom_ip == 0) {
						ext->page.edge_flash.left_ip = 1;
						lv_page_start_edge_flash(pull_win, LV_PAGE_EDGE_LEFT);
					}
#endif

					lv_obj_set_x(scrl, -ext->act_id.x * w + style_bg->body.padding.left);
				}
			}
			if (ext->drag_right_en == 0 && indev->proc.types.pointer.vect.x < 0 && ext->drag_ver == 0) {
				if (x < -(ext->act_id.x * w)) {
#if LV_USE_ANIMATION
					if (ext->page.edge_flash.enabled && ext->page.edge_flash.left_ip == 0 &&
						ext->page.edge_flash.right_ip == 0 && ext->page.edge_flash.top_ip == 0 &&
						ext->page.edge_flash.bottom_ip == 0) {
						ext->page.edge_flash.right_ip = 1;
						lv_page_start_edge_flash(pull_win, LV_PAGE_EDGE_RIGHT);
					}
#endif
				}

				lv_obj_set_x(scrl, -ext->act_id.x * w + style_bg->body.padding.top);
			}

			/*Apply the drag constraints*/
			if (ext->drag_ver == 0)
				lv_obj_set_y(scrl, -ext->act_id.y * lv_obj_get_height(pull_win) + style_bg->body.padding.top);
			if (ext->drag_hor == 0)
				lv_obj_set_x(scrl, -ext->act_id.x * lv_obj_get_width(pull_win) + style_bg->body.padding.left);
		}
	}
	return res;
}

static void pull_win_scrl_event_cb(lv_obj_t * scrl, lv_event_t event)
{
	lv_obj_t * pull_win = lv_obj_get_parent(scrl);

	/*Initialize some variables on PRESS*/
	if (event == LV_EVENT_PRESSED) {
		lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
		ext->drag_hor = 0;
		ext->drag_ver = 0;
		set_valid_drag_dirs(pull_win);
	}
	/*Animate the tabview to the correct location on RELEASE*/
	else if (event == LV_EVENT_PRESS_LOST || event == LV_EVENT_RELEASED) {
		/* If the element was dragged and it moved the pull_win finish the drag manually to
		* let the pull_win to finish the move.*/
		lv_indev_t * indev = lv_indev_get_act();
		lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
		if (lv_indev_is_dragging(indev) && (ext->drag_hor || ext->drag_ver)) {
			indev->proc.types.pointer.drag_in_prog = 0;
		}

		drag_end_handler(pull_win);
	}
}

/**
* Called when the user releases an element of the pull_win after dragging it.
* @param pull_win pointer to a pull_win object
*/
static void drag_end_handler(lv_obj_t * pull_win)
{
	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	lv_indev_t * indev = lv_indev_get_act();
	lv_point_t point_act;
	lv_indev_get_point(indev, &point_act);
	lv_obj_t * scrl = lv_page_get_scrl(pull_win);
	lv_point_t p;

	p.x = -(scrl->coords.x1 - lv_obj_get_width(pull_win) / 2);
	p.y = -(scrl->coords.y1 - lv_obj_get_height(pull_win) / 2);

	/*From the drag vector (drag throw) predict the end position*/
	if (ext->drag_hor) {
		lv_point_t vect;
		lv_indev_get_vect(indev, &vect);
		lv_coord_t predict = 0;

		while (vect.x != 0) {
			predict += vect.x;
			vect.x = vect.x * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
		}

		p.x -= predict;
	}
	else if (ext->drag_ver) {
		lv_point_t vect;
		lv_indev_get_vect(indev, &vect);
		lv_coord_t predict = 0;

		while (vect.y != 0) {
			predict += vect.y;
			vect.y = vect.y * (100 - LV_INDEV_DEF_DRAG_THROW) / 100;
		}

		p.y -= predict;
	}

	/*Get the index of the tile*/
	p.x = p.x / lv_obj_get_width(pull_win);
	p.y = p.y / lv_obj_get_height(pull_win);

	/*Max +- move*/
	lv_coord_t x_move = p.x - ext->act_id.x;
	lv_coord_t y_move = p.y - ext->act_id.y;
	if (x_move < -1) x_move = -1;
	if (x_move > 1) x_move = 1;
	if (y_move < -1) y_move = -1;
	if (y_move > 1) y_move = 1;

	/*Set the new tile*/
	lv_pull_win_set_tile_act(pull_win, ext->act_id.x + x_move, ext->act_id.y + y_move, true);
}

static bool set_valid_drag_dirs(lv_obj_t * pull_win)
{

	lv_pull_win_ext_t * ext = lv_obj_get_ext_attr(pull_win);
	if (ext->valid_pos == NULL) return false;

	ext->drag_bottom_en = 0;
	ext->drag_top_en = 0;
	ext->drag_left_en = 0;
	ext->drag_right_en = 0;

	uint16_t i;
	for (i = 0; i < ext->valid_pos_cnt; i++) {
		if (ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y - 1) ext->drag_top_en = 1;
		if (ext->valid_pos[i].x == ext->act_id.x && ext->valid_pos[i].y == ext->act_id.y + 1) ext->drag_bottom_en = 1;
		if (ext->valid_pos[i].x == ext->act_id.x - 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_left_en = 1;
		if (ext->valid_pos[i].x == ext->act_id.x + 1 && ext->valid_pos[i].y == ext->act_id.y) ext->drag_right_en = 1;
	}

	return true;
}

#endif
