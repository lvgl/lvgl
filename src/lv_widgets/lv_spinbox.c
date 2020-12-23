/**
 * @file lv_spinbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinbox.h"

#if LV_USE_SPINBOX != 0
#include "../lv_misc/lv_debug.h"
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"
#include "../lv_misc/lv_utils.h"

/*********************
 *      DEFINES
 *********************/
#define LV_OBJX_NAME "lv_spinbox"

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_spinbox_signal(lv_obj_t * spinbox, lv_signal_t sign, void * param);
static lv_style_list_t * lv_spinbox_get_style(lv_obj_t * ta, uint8_t part);
static void lv_spinbox_updatevalue(lv_obj_t * spinbox);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_cb_t ancestor_signal;
static lv_design_cb_t ancestor_design;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create a spinbox object
 * @param par pointer to an object, it will be the parent of the new spinbox
 * @param copy pointer to a spinbox object, if not NULL then the new object will be copied from it
 * @return pointer to the created spinbox
 */
lv_obj_t * lv_spinbox_create(lv_obj_t * par, const lv_obj_t * copy)
{
    LV_LOG_TRACE("spinbox create started");

    /*Create the ancestor of spinbox*/
    lv_obj_t * spinbox = lv_textarea_create(par, copy);
    LV_ASSERT_MEM(spinbox);
    if(spinbox == NULL) return NULL;

    /*Allocate the spinbox type specific extended data*/
    lv_spinbox_ext_t * ext = lv_obj_allocate_ext_attr(spinbox, sizeof(lv_spinbox_ext_t));
    LV_ASSERT_MEM(ext);
    if(ext == NULL) {
        lv_obj_del(spinbox);
        return NULL;
    }

    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_cb(spinbox);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_cb(spinbox);

    /*Initialize the allocated 'ext'*/
    ext->value              = 0;
    ext->dec_point_pos      = 0;
    ext->digit_count        = 5;
    ext->digit_padding_left = 0;
    ext->step               = 1;
    ext->range_max          = 99999;
    ext->range_min          = -99999;
    ext->rollover           = false;


    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_cb(spinbox, lv_spinbox_signal);
    lv_obj_set_design_cb(spinbox, ancestor_design); /*Leave the Text area's design function*/

    /*Init the new spinbox*/
    if(copy == NULL) {
        /* No scrolling will happen here so make the scrollable non-clickable
         * It allows to handle input events in the bg object only.*/
        lv_obj_set_click(lv_page_get_scrollable(spinbox), false);
        lv_textarea_set_one_line(spinbox, true);
        lv_textarea_set_cursor_click_pos(spinbox, true);
        lv_obj_set_width(spinbox, LV_DPI);
        lv_theme_apply(spinbox, LV_THEME_SPINBOX);
    }
    /*Copy an existing spinbox*/
    else {
        lv_spinbox_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        lv_spinbox_set_value(spinbox, copy_ext->value);
        lv_spinbox_set_digit_format(spinbox, (uint8_t)copy_ext->digit_count, (uint8_t)copy_ext->dec_point_pos);
        lv_spinbox_set_range(spinbox, copy_ext->range_min, copy_ext->range_max);
        lv_spinbox_set_step(spinbox, copy_ext->step);
        lv_spinbox_set_rollover(spinbox, copy_ext->rollover);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(spinbox, LV_OBJ_PART_ALL, LV_STYLE_PROP_ALL);
    }

    lv_spinbox_updatevalue(spinbox);

    LV_LOG_INFO("spinbox created");

    return spinbox;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set spinbox rollover function
 * @param spinbox pointer to spinbox
 * @param b true or false to enable or disable (default)
 */
void lv_spinbox_set_rollover(lv_obj_t * spinbox, bool b)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    ext->rollover = b;
}

/**
 * Set spinbox value
 * @param spinbox pointer to spinbox
 * @param i value to be set
 */
void lv_spinbox_set_value(lv_obj_t * spinbox, int32_t i)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    if(i > ext->range_max) i = ext->range_max;
    if(i < ext->range_min) i = ext->range_min;

    ext->value = i;

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not
 * shown
 */
void lv_spinbox_set_digit_format(lv_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    if(digit_count > LV_SPINBOX_MAX_DIGIT_COUNT) digit_count = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(separator_position >= digit_count) separator_position = 0;
    if(separator_position > LV_SPINBOX_MAX_DIGIT_COUNT) separator_position = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(digit_count < LV_SPINBOX_MAX_DIGIT_COUNT) {
        int64_t max_val = _lv_pow(10, digit_count);
        if(ext->range_max > max_val - 1) ext->range_max = max_val - 1;
        if(ext->range_min < - max_val  + 1) ext->range_min = - max_val  + 1;
    }

    ext->digit_count   = digit_count;
    ext->dec_point_pos = separator_position;

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Set spinbox step
 * @param spinbox pointer to spinbox
 * @param step steps on increment/decrement
 */
void lv_spinbox_set_step(lv_obj_t * spinbox, uint32_t step)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    ext->step = step;

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void lv_spinbox_set_range(lv_obj_t * spinbox, int32_t range_min, int32_t range_max)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    ext->range_max = range_max;
    ext->range_min = range_min;

    if(ext->value > ext->range_max) {
        ext->value = ext->range_max;
        lv_obj_invalidate(spinbox);
    }
    if(ext->value < ext->range_min) {
        ext->value = ext->range_min;
        lv_obj_invalidate(spinbox);
    }

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Set spinbox left padding in digits count (added between sign and first digit)
 * @param spinbox pointer to spinbox
 * @param cb Callback function called on value change event
 */
void lv_spinbox_set_padding_left(lv_obj_t * spinbox, uint8_t padding)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext  = lv_obj_get_ext_attr(spinbox);
    ext->digit_padding_left = padding;
    lv_spinbox_updatevalue(spinbox);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get spinbox rollover function status
 * @param spinbox pointer to spinbox
 */
bool lv_spinbox_get_rollover(lv_obj_t * spinbox)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    return ext->rollover;
}

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t lv_spinbox_get_value(lv_obj_t * spinbox)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    return ext->value;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_next(lv_obj_t * spinbox)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    int32_t new_step = ext->step / 10;
    if((new_step) > 0)
        ext->step = new_step;
    else
        ext->step = 1;

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Select next higher digit for edition
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_prev(lv_obj_t * spinbox)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    int32_t step_limit;
    step_limit       = LV_MATH_MAX(ext->range_max, (ext->range_min < 0 ? (-ext->range_min) : ext->range_min));
    int32_t new_step = ext->step * 10;
    if(new_step <= step_limit) ext->step = new_step;

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Increment spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_increment(lv_obj_t * spinbox)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    if(ext->value + ext->step <= ext->range_max) {
        /*Special mode when zero crossing*/
        if((ext->value + ext->step) > 0 && ext->value < 0) ext->value = -ext->value;
        ext->value += ext->step;

    }
    else {
        // Rollover?
        if((ext->rollover) && (ext->value == ext->range_max))
            ext->value = ext->range_min;
        else
            ext->value = ext->range_max;
    }

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Decrement spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_decrement(lv_obj_t * spinbox)
{
    LV_ASSERT_OBJ(spinbox, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    if(ext->value - ext->step >= ext->range_min) {
        /*Special mode when zero crossing*/
        if((ext->value - ext->step) < 0 && ext->value > 0) ext->value = -ext->value;
        ext->value -= ext->step;
    }
    else {
        // Rollover?
        if((ext->rollover) && (ext->value == ext->range_min))
            ext->value = ext->range_max;
        else
            ext->value = ext->range_min;
    }

    lv_spinbox_updatevalue(spinbox);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Signal function of the spinbox
 * @param spinbox pointer to a spinbox object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_spinbox_signal(lv_obj_t * spinbox, lv_signal_t sign, void * param)
{

    lv_res_t res = LV_RES_OK;
    if(sign == LV_SIGNAL_GET_STYLE) {
        lv_get_style_info_t * info = param;
        info->result = lv_spinbox_get_style(spinbox, info->part);
        if(info->result != NULL) return LV_RES_OK;
        else return ancestor_signal(spinbox, sign, param);
    }

    /* Include the ancient signal function */
    if(sign != LV_SIGNAL_CONTROL) {
        res = ancestor_signal(spinbox, sign, param);
        if(res != LV_RES_OK) return res;
    }
    if(sign == LV_SIGNAL_GET_TYPE) return lv_obj_handle_get_type_signal(param, LV_OBJX_NAME);


    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    }
    else if(sign == LV_SIGNAL_GET_TYPE) {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++) { /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_spinbox";
    }
    else if(sign == LV_SIGNAL_RELEASED) {
        /*If released with an ENCODER then move to the next digit*/
        lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
        lv_indev_t * indev = lv_indev_get_act();
        if(lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
#if LV_USE_GROUP
            if(lv_group_get_editing(lv_obj_get_group(spinbox))) {
                if(ext->step > 1) {
                    lv_spinbox_step_next(spinbox);
                }
                else {
                    /*Restart from the MSB*/
                    ext->step = 1;
                    uint32_t i;
                    for(i = 0; i < ext->digit_count; i++) {
                        int32_t new_step = ext->step * 10;
                        if(new_step >= ext->range_max) break;
                        ext->step = new_step;
                    }
                    lv_spinbox_step_prev(spinbox);
                }
            }
#endif
        }
        else {
            /*The cursor has been positioned to a digit.
             * Set `step` accordingly*/
            const char * txt = lv_textarea_get_text(spinbox);
            size_t txt_len = strlen(txt);

            if(txt[ext->ta.cursor.pos] == '.') {
                lv_textarea_cursor_left(spinbox);
            }
            else if(ext->ta.cursor.pos == (uint32_t)txt_len) {
                lv_textarea_set_cursor_pos(spinbox, txt_len - 1);
            }
            else if(ext->ta.cursor.pos == 0 && ext->range_min < 0) {
                lv_textarea_set_cursor_pos(spinbox, 1);
            }

            size_t len = ext->digit_count - 1;
            uint16_t cp = ext->ta.cursor.pos;

            if(ext->ta.cursor.pos > ext->dec_point_pos && ext->dec_point_pos != 0) cp--;
            uint32_t pos = len - cp;

            if(ext->range_min < 0) pos++;

            ext->step = 1;
            uint16_t i;
            for(i = 0; i < pos; i++) ext->step *= 10;



        }
    }
    else if(sign == LV_SIGNAL_CONTROL) {
#if LV_USE_GROUP
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        uint32_t c = *((uint32_t *)param); /*uint32_t because can be UTF-8*/
        if(c == LV_KEY_RIGHT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER)
                lv_spinbox_increment(spinbox);
            else
                lv_spinbox_step_next(spinbox);
        }
        else if(c == LV_KEY_LEFT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER)
                lv_spinbox_decrement(spinbox);
            else
                lv_spinbox_step_prev(spinbox);
        }
        else if(c == LV_KEY_UP) {
            lv_spinbox_increment(spinbox);
        }
        else if(c == LV_KEY_DOWN) {
            lv_spinbox_decrement(spinbox);
        }
        else {
            lv_textarea_add_char(spinbox, c);
        }
#endif
    }

    return res;
}

/**
 * Get the style descriptor of a part of the object
 * @param page pointer the object
 * @param part the part from `lv_spinbox_part_t`. (LV_SPINBOX_PART_...)
 * @return pointer to the style descriptor of the specified part
 */
static lv_style_list_t * lv_spinbox_get_style(lv_obj_t * ta, uint8_t part)
{
    LV_ASSERT_OBJ(ta, LV_OBJX_NAME);

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(ta);
    lv_style_list_t * style_dsc_p;

    switch(part) {
        case LV_SPINBOX_PART_BG:
            style_dsc_p = &ta->style_list;
            break;
        case LV_SPINBOX_PART_CURSOR:
            style_dsc_p = &ext->ta.cursor.style;
            break;
        default:
            style_dsc_p = NULL;
    }

    return style_dsc_p;
}
static void lv_spinbox_updatevalue(lv_obj_t * spinbox)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    char buf[LV_SPINBOX_MAX_DIGIT_COUNT + 8];
    _lv_memset_00(buf, sizeof(buf));
    char * buf_p = buf;
    uint8_t cur_shift_left = 0;

    if(ext->range_min < 0) {  // hide sign if there are only positive values
        /*Add the sign*/
        (*buf_p) = ext->value >= 0 ? '+' : '-';
        buf_p++;
    }
    else {
        /*Cursor need shift to left*/
        cur_shift_left++;
    }

    int32_t i;
    /*padding left*/
    for(i = 0; i < ext->digit_padding_left; i++) {
        (*buf_p) = ' ';
        buf_p++;
    }

    char digits[64];
    /*Convert the numbers to string (the sign is already handled so always covert positive number)*/
    _lv_utils_num_to_str(ext->value < 0 ? -ext->value : ext->value, digits);

    /*Add leading zeros*/
    int lz_cnt = ext->digit_count - (int)strlen(digits);
    if(lz_cnt > 0) {
        for(i = (uint16_t)strlen(digits); i >= 0; i--) {
            digits[i + lz_cnt] = digits[i];
        }
        for(i = 0; i < lz_cnt; i++) {
            digits[i] = '0';
        }
    }

    int32_t intDigits;
    intDigits = (ext->dec_point_pos == 0) ? ext->digit_count : ext->dec_point_pos;

    /*Add the decimal part*/
    for(i = 0; i < intDigits && digits[i] != '\0'; i++) {
        (*buf_p) = digits[i];
        buf_p++;
    }

    if(ext->dec_point_pos != 0) {
        /*Insert the decimal point*/
        (*buf_p) = '.';
        buf_p++;

        for(/*Leave i*/; i < ext->digit_count && digits[i] != '\0'; i++) {
            (*buf_p) = digits[i];
            buf_p++;
        }
    }

    /*Refresh the text*/
    lv_textarea_set_text(spinbox, (char *)buf);

    /*Set the cursor position*/
    int32_t step    = ext->step;
    uint8_t cur_pos = (uint8_t)ext->digit_count;
    while(step >= 10) {
        step /= 10;
        cur_pos--;
    }

    if(cur_pos > intDigits) cur_pos++; /*Skip the decimal point*/

    cur_pos += (ext->digit_padding_left - cur_shift_left);

    lv_textarea_set_cursor_pos(spinbox, cur_pos);
}

#endif
