
/**
 * @file lv_kb.c
 * 
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_conf.h"
#if USE_LV_KB != 0

#include "lv_kb.h"
#include "lv_ta.h"

/*********************
 *      DEFINES
 *********************/
/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
#if 0 /*Use Button matrix design*/
static bool lv_kb_design(lv_obj_t * kb, const area_t * mask, lv_design_mode_t mode);
#endif

static lv_action_res_t lv_app_kb_action(lv_obj_t * kb, uint16_t i);

/**********************
 *  STATIC VARIABLES
 **********************/
static const char * kb_map_lc[] = {
"\0051#", "\004q", "\004w", "\004e", "\004r", "\004t", "\004y", "\004u", "\004i", "\004o", "\004p", "\007Del", "\n",
"\006ABC", "\003a", "\003s", "\003d", "\003f", "\003g", "\003h", "\003j", "\003k", "\003l", "\010Enter", "\n",
"_", "-", "z", "x", "c", "v", "b", "n", "m", ".", ",", ":", "\n",
"\003Hide", "\003Left", "\006 ", "\003Right", "\003Ok", ""
};

static const char * kb_map_uc[] = {
"\0051#", "\004Q", "\004W", "\004E", "\004R", "\004T", "\004Y", "\004U", "\004I", "\004O", "\004P", "\007Del", "\n",
"\006abc", "\003A", "\003S", "\003D", "\003F", "\003G", "\003H", "\003J", "\003K", "\003L", "\010Enter", "\n",
"_", "-", "Z", "X", "C", "V", "B", "N", "M", ".", ",", ":", "\n",
"\003Hide", "\003Left", "\006 ", "\003Right", "\003Ok", ""
};

static const char * kb_map_spec[] = {
"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "\002Del", "\n",
"\002abc", "+", "-", "/", "*", "=", "%", "!", "?", "#", "<", ">", "\n",
"\\", "@", "$", "(", ")", "{", "}", "[", "]", ";", "\"", "'", "\n",
"\003Hide", "\003Left", "\006 ", "\003Right", "\003Ok", ""
};

static const char * kb_map_num[] = {
"1", "2", "3", "\002Hide","\n",
"4", "5", "6", "\002Ok", "\n",
"7", "8", "9", "\002Del", "\n",
"+/-", "0", ".", "Left", "Right", ""
};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/*----------------- 
 * Create function
 *-----------------*/

/**
 * Create a keyboard objects
 * @param par pointer to an object, it will be the parent of the new keyboard
 * @param copy pointer to a keyboard object, if not NULL then the new object will be copied from it
 * @return pointer to the created keyboard
 */
lv_obj_t * lv_kb_create(lv_obj_t * par, lv_obj_t * copy)
{
    /*Create the ancestor of keyboard*/
    lv_obj_t * new_kb = lv_btnm_create(par, copy);
    dm_assert(new_kb);
    
    /*Allocate the keyboard type specific extended data*/
    lv_kb_ext_t * ext = lv_obj_allocate_ext_attr(new_kb, sizeof(lv_kb_ext_t));
    dm_assert(ext);

    /*Initialize the allocated 'ext' */

    ext->ta = NULL;
    ext->mode = LV_KB_MODE_TXT;
    ext->cur_mng = 0;
    ext->close_action = NULL;
    ext->ok_action = NULL;

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_kb, lv_kb_signal);

    /*Init the new keyboard keyboard*/
    if(copy == NULL) {
        lv_obj_set_size(new_kb, LV_HOR_RES, LV_VER_RES / 2);
        lv_obj_align(new_kb, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
        lv_btnm_set_action(new_kb, lv_app_kb_action);
        lv_btnm_set_map(new_kb, kb_map_lc);
    }
    /*Copy an existing keyboard*/
    else {
    	lv_kb_ext_t * copy_ext = lv_obj_get_ext_attr(copy);ext->ta = NULL;
        ext->ta = copy_ext->ta;
        ext->mode = copy_ext->mode;
        ext->cur_mng = copy_ext->cur_mng;
        ext->close_action = copy_ext->close_action;
        ext->ok_action = copy_ext->ok_action;

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_kb);
    }
    
    return new_kb;
}

/**
 * Signal function of the keyboard
 * @param kb pointer to a keyboard object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return true: the object is still valid (not deleted), false: the object become invalid
 */
bool lv_kb_signal(lv_obj_t * kb, lv_signal_t sign, void * param)
{
    bool valid;

    /* Include the ancient signal function */
    valid = lv_btnm_signal(kb, sign, param);

    /* The object can be deleted so check its validity and then
     * make the object specific signal handling */
    if(valid != false) {
    	if(sign == LV_SIGNAL_CLEANUP) {
            /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    	}
    }
    
    return valid;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @param ta pointer to a Text Area object to write there
 */
void lv_kb_set_ta(lv_obj_t * kb, lv_obj_t * ta)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);

    if(ext->ta != NULL && ext->cur_mng != 0) lv_ta_set_cursor_show(ext->ta, false);
    ext->ta = ta;
    if(ext->cur_mng != 0) lv_ta_set_cursor_show(ext->ta, true);
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @param mode the mode from 'lv_kb_mode_t'
 */
void lv_kb_set_mode(lv_obj_t * kb, lv_kb_mode_t mode)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    ext->mode = mode;
    if(mode == LV_KB_MODE_TXT) lv_btnm_set_map(kb, kb_map_lc);
    else if(mode == LV_KB_MODE_NUM) lv_btnm_set_map(kb, kb_map_num);
}


/**
 * Automatically hide or show the cursor of Text Area
 * @param kb pointer to a Keyboard object
 * @param en true: show cursor on the current text area, false: hide cursor
 */
void lv_kb_set_cur_mng(lv_obj_t * kb, bool en)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    ext->cur_mng = en == false? 0 : 1;

    if(ext->ta != NULL) lv_ta_set_cursor_show(ext->ta, true);
}

/**
 * Set call back to call when the "Ok" button is pressed
 * @param kb pointer to Keyboard object
 * @param action a callback with 'lv_action_t' type
 */
void lv_kb_set_ok_action(lv_obj_t * kb, lv_action_t action)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    ext->ok_action = action;
}

/**
 * Set call back to call when the "Hide" button is pressed
 * @param kb pointer to Keyboard object
 * @param action a callback with 'lv_action_t' type
 */
void lv_kb_set_close_action(lv_obj_t * kb, lv_action_t action)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    ext->close_action = action;
}


/*=====================
 * Getter functions
 *====================*/

/**
 * Assign a Text Area to the Keyboard. The pressed characters will be put there.
 * @param kb pointer to a Keyboard object
 * @return pointer to the assigned Text Area object
 */
lv_obj_t * lv_kb_get_ta(lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ta;
}

/**
 * Set a new a mode (text or number map)
 * @param kb pointer to a Keyboard object
 * @return the current mode from 'lv_kb_mode_t'
 */
lv_kb_mode_t lv_kb_get_mode(lv_obj_t * kb)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->mode;
}


/**
 * Get the current cursor manage mode.
 * @param kb pointer to a Keyboard object
 * @return true: show cursor on the current text area, false: hide cursor
 */
bool lv_kb_get_cur_mng(lv_obj_t * kb, bool en)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->cur_mng == 0 ? false : true;
}

/**
 * Get the callback to call when the "Ok" button is pressed
 * @param kb pointer to Keyboard object
 * @return the ok callback
 */
lv_action_t lv_kb_get_ok_action(lv_obj_t * kb, lv_action_t action)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->ok_action;
}

/**
 * Get the callback to call when the "Hide" button is pressed
 * @param kb pointer to Keyboard object
 * @return the close callback
 */
lv_action_t lv_kb_get_close_action(lv_obj_t * kb, lv_action_t action)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    return ext->close_action;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#if 0 /*Use Button matrix design*/
/**
 * Handle the drawing related tasks of the keyboards
 * @param kb pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_kb_design(lv_obj_t * kb, const area_t * mask, lv_design_mode_t mode)
{
    /*Return false if the object is not covers the mask_p area*/
    if(mode == LV_DESIGN_COVER_CHK) {
    	return false;
    }
    /*Draw the object*/
    else if(mode == LV_DESIGN_DRAW_MAIN) {

    }
    /*Post draw when the children are drawn*/
    else if(mode == LV_DESIGN_DRAW_POST) {

    }

    return true;
}
#endif

/**
 * Called when a button of 'kb_btnm' is released
 * @param btnm pointer to 'kb_btnm'
 * @param i the index of the released button from the current btnm map
 * @return LV_ACTION_RES_INV if the btnm is deleted else LV_ACTION_RES_OK
 */
static lv_action_res_t lv_app_kb_action(lv_obj_t * kb, uint16_t i)
{
    lv_kb_ext_t * ext = lv_obj_get_ext_attr(kb);
    if(ext->ta == NULL) return LV_ACTION_RES_OK;

    const char ** map = lv_btnm_get_map(kb);
    const char * txt = map[i];

    /*Ignore the unit size number of the text*/
    if(txt[0] <= '\011') txt++;     /*Ignore length specifier*/

    if(txt[0] == '\177') return LV_ACTION_RES_OK;    /*Don't care hidden buttons*/

    /*Do the corresponding action according to the text of the button*/
    if(strcmp(txt, "abc") == 0) {
        lv_btnm_set_map(kb, kb_map_lc);
    } else if(strcmp(txt, "ABC") == 0) {
        lv_btnm_set_map(kb, kb_map_uc);
    } else if(strcmp(txt, "1#") == 0) {
        lv_btnm_set_map(kb, kb_map_spec);
    }  else if(strcmp(txt, "Enter") == 0) {
        lv_ta_add_char(ext->ta, '\n');
    } else if(strcmp(txt, "Left") == 0) {
        lv_ta_cursor_left(ext->ta);
    } else if(strcmp(txt, "Right") == 0) {
        lv_ta_cursor_right(ext->ta);
    } else if(strcmp(txt, "Del") == 0) {
        lv_ta_del(ext->ta);
    } else if(strcmp(txt, "+/-") == 0) {
        uint16_t cur = lv_ta_get_cursor_pos(ext->ta);
        const char * ta_txt = lv_ta_get_txt(ext->ta);
        if(ta_txt[0] == '-') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del(ext->ta);
            lv_ta_add_char(ext->ta, '+');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else if(ta_txt[0] == '+') {
            lv_ta_set_cursor_pos(ext->ta, 1);
            lv_ta_del(ext->ta);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur);
        } else {
            lv_ta_set_cursor_pos(ext->ta, 0);
            lv_ta_add_char(ext->ta, '-');
            lv_ta_set_cursor_pos(ext->ta, cur + 1);
        }
    } else if(strcmp(txt, "Hide") == 0) {
        if(ext->close_action) ext->close_action(kb);
        return LV_ACTION_RES_INV;
    } else if(strcmp(txt, "Ok") == 0) {
        if(ext->ok_action) ext->ok_action(kb);
        return LV_ACTION_RES_INV;
    } else {
        lv_ta_add_text(ext->ta, txt);
    }
    return LV_ACTION_RES_OK;
}

#endif
