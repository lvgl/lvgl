/**
 * @file lv_spinbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinbox.h"

#if USE_LV_SPINBOX != 0
#include "../lv_themes/lv_theme.h"
#include "../lv_misc/lv_math.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_res_t lv_spinbox_signal(lv_obj_t * spinbox, lv_signal_t sign, void * param);
static void lv_spinbox_updatevalue(lv_obj_t * spinbox);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_signal_func_t ancestor_signal;
static lv_design_func_t ancestor_design;

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
    lv_obj_t * new_spinbox = lv_ta_create(par, copy);
    lv_mem_assert(new_spinbox);
    if(new_spinbox == NULL) return NULL;

    /*Allocate the spinbox type specific extended data*/
    lv_spinbox_ext_t * ext = lv_obj_allocate_ext_attr(new_spinbox, sizeof(lv_spinbox_ext_t));
    lv_mem_assert(ext);
    if(ext == NULL) return NULL;
    if(ancestor_signal == NULL) ancestor_signal = lv_obj_get_signal_func(new_spinbox);
    if(ancestor_design == NULL) ancestor_design = lv_obj_get_design_func(new_spinbox);

    /*Initialize the allocated 'ext'*/
    ext->ta.one_line = 1;
    ext->ta.pwd_mode = 0;
    ext->ta.accapted_chars = "1234567890+-. ";

    ext->value = 0;
    ext->dec_point_pos = 0;
    ext->digit_count = 5;
    ext->digit_padding_left = 0;
    ext->step = 1;
    ext->range_max = 99999;
    ext->range_min = -99999;
    ext->value_changed_cb = NULL;

    lv_ta_set_cursor_type(new_spinbox, LV_CURSOR_BLOCK | LV_CURSOR_HIDDEN); /*hidden by default*/

    /*The signal and design functions are not copied so set them here*/
    lv_obj_set_signal_func(new_spinbox, lv_spinbox_signal);
    lv_obj_set_design_func(new_spinbox, ancestor_design);        /*Leave the Text area's design function*/

    /*Init the new spinbox spinbox*/
    if(copy == NULL) {
        /*Set the default styles*/
        lv_theme_t * th = lv_theme_get_current();
        if(th) {
            lv_spinbox_set_style(new_spinbox, LV_SPINBOX_STYLE_BG, th->spinbox.bg);
            lv_spinbox_set_style(new_spinbox, LV_SPINBOX_STYLE_CURSOR, th->spinbox.cursor);
            lv_spinbox_set_style(new_spinbox, LV_SPINBOX_STYLE_SB, th->spinbox.sb);
        }
    }
    /*Copy an existing spinbox*/
    else {
        lv_spinbox_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

        lv_spinbox_set_value(new_spinbox, copy_ext->value);
        lv_spinbox_set_digit_format(new_spinbox, copy_ext->digit_count, copy_ext->dec_point_pos);
        lv_spinbox_set_range(new_spinbox, copy_ext->range_min, copy_ext->range_max);
        lv_spinbox_set_step(new_spinbox, copy_ext->step);

        /*Refresh the style with new signal function*/
        lv_obj_refresh_style(new_spinbox);
    }

    lv_spinbox_updatevalue(new_spinbox);

    LV_LOG_INFO("spinbox created");

    return new_spinbox;
}


/*=====================
 * Setter functions
 *====================*/

/**
 * Set spinbox value
 * @param spinbox pointer to spinbox
 * @param i value to be set
 */
void lv_spinbox_set_value(lv_obj_t * spinbox, int32_t i)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL)
        return;

    if(i > ext->range_max)
        i = ext->range_max;
    if(i < ext->range_min)
        i = ext->range_min;

    ext->value = i;

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not shown
 */
void lv_spinbox_set_digit_format(lv_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL)
        return;

    if(digit_count > LV_SPINBOX_MAX_DIGIT_COUNT)
        digit_count = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(separator_position > LV_SPINBOX_MAX_DIGIT_COUNT)
        separator_position = LV_SPINBOX_MAX_DIGIT_COUNT;

    ext->digit_count = digit_count;
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
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    ext->step = step;
}

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void lv_spinbox_set_range(lv_obj_t * spinbox, int32_t range_min, int32_t range_max)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    if(ext == NULL) return;

    ext->range_max = range_max;
    ext->range_min = range_min;

    if(ext->value > ext->range_max)  {
        ext->value = ext->range_max;
        lv_obj_invalidate(spinbox);
    }
    if(ext->value < ext->range_min)  {
        ext->value = ext->range_min;
        lv_obj_invalidate(spinbox);
    }
}

/**
 * Set spinbox callback on calue change
 * @param spinbox pointer to spinbox
 * @param cb Callback function called on value change event
 */
void lv_spinbox_set_value_changed_cb(lv_obj_t * spinbox, lv_spinbox_value_changed_cb_t cb)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    ext->value_changed_cb = cb;
}

/**
 * Set spinbox left padding in digits count (added between sign and first digit)
 * @param spinbox pointer to spinbox
 * @param cb Callback function called on value change event
 */
void lv_spinbox_set_padding_left(lv_obj_t * spinbox, uint8_t padding)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    ext->digit_padding_left = padding;
    lv_spinbox_updatevalue(spinbox);
}

/*=====================
 * Getter functions
 *====================*/

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param spinbox pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t lv_spinbox_get_value(lv_obj_t * spinbox)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    return ext->value;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition by dividing the step by 10
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_next(lv_obj_t * spinbox)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    int32_t new_step = ext->step / 10;
    if((new_step) > 0) ext->step = new_step;
    else ext->step = 1;

    lv_spinbox_updatevalue(spinbox);
}

/**
 * Select next higher digit for edition by multiplying the step by 10
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_step_previous(lv_obj_t * spinbox)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
    int32_t step_limit;
    step_limit = LV_MATH_MAX(ext->range_max, (ext->range_min < 0 ? (-ext->range_min) : ext->range_min));
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
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    if(ext->value + ext->step <= ext->range_max) {
        /*Special mode when zero crossing*/
        if((ext->value + ext->step) > 0 && ext->value < 0) ext->value = -ext->value;
        ext->value += ext->step;

    } else {
        ext->value = ext->range_max;
    }

    if(ext->value_changed_cb != NULL)  ext->value_changed_cb(spinbox, ext->value);
    lv_spinbox_updatevalue(spinbox);
}

/**
 * Decrement spinbox value by one step
 * @param spinbox pointer to spinbox
 */
void lv_spinbox_decrement(lv_obj_t * spinbox)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    if(ext->value - ext->step >= ext->range_min) {
        /*Special mode when zero crossing*/
        if((ext->value - ext->step) < 0 && ext->value > 0) ext->value = -ext->value;
        ext->value -= ext->step;
    } else {
        ext->value = ext->range_min;
    }

    if(ext->value_changed_cb != NULL) ext->value_changed_cb(spinbox, ext->value);
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

    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    lv_res_t res = LV_RES_OK;

    /* Include the ancient signal function */
    if(sign != LV_SIGNAL_CONTROLL)
    {
        res = ancestor_signal(spinbox, sign, param);
        if(res != LV_RES_OK) return res;
    }

    if(sign == LV_SIGNAL_CLEANUP) {
        /*Nothing to cleanup. (No dynamically allocated memory in 'ext')*/
    } else if(sign == LV_SIGNAL_GET_TYPE)
    {
        lv_obj_type_t * buf = param;
        uint8_t i;
        for(i = 0; i < LV_MAX_ANCESTOR_NUM - 1; i++)
        {  /*Find the last set data*/
            if(buf->type[i] == NULL) break;
        }
        buf->type[i] = "lv_spinbox";
    }
    else if(sign == LV_SIGNAL_CONTROLL) {
        lv_hal_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        uint32_t c = *((uint32_t *)param);      /*uint32_t because can be UTF-8*/
        if(c == LV_GROUP_KEY_RIGHT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER) lv_spinbox_increment(spinbox);
            else lv_spinbox_step_next(spinbox);
        }
        else if(c == LV_GROUP_KEY_LEFT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER) lv_spinbox_decrement(spinbox);
            else lv_spinbox_step_previous(spinbox);
        }
        else if(c == LV_GROUP_KEY_UP) {
            lv_spinbox_increment(spinbox);
        }
        else if(c == LV_GROUP_KEY_DOWN)  {
            lv_spinbox_decrement(spinbox);
        }
        else if(c == LV_GROUP_KEY_ENTER) {

            if(ext->step > 1) {
                lv_spinbox_step_next(spinbox);
            } else {
                /*Restart from the MSB*/
                ext->step = 1;
                uint32_t i;
                for(i = 0; i < ext->digit_count; i++) {
                    int32_t new_step = ext->step * 10;
                    if(new_step >= ext->range_max) break;
                    ext->step = new_step;
                }
                lv_spinbox_step_previous(spinbox);
            }
        }
        else {
            lv_ta_add_char(spinbox, c);
        }
    }

    return res;
}

static void lv_spinbox_updatevalue(lv_obj_t * spinbox)
{
    lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

    char buf[LV_SPINBOX_MAX_DIGIT_COUNT + 8];
    memset(buf, 0, sizeof(buf));
    char * buf_p = buf;

    /*Add the sign*/
    (*buf_p) = ext->value >= 0 ? '+' : '-';
    buf_p++;

    int i;
    /*padding left*/
    for(i = 0; i < ext->digit_padding_left; i++) {
        (*buf_p) = ' ';
        buf_p++;
    }

    char digits[64];
    /*Convert the numbers to string (the sign is already handled so always covert positive number)*/
    lv_math_num_to_str(ext->value < 0 ? -ext->value : ext->value, digits);

    /*Add leading zeros*/
    int lz_cnt = ext->digit_count - (int)strlen(digits);
    if(lz_cnt > 0) {
        for(i = strlen(digits); i >= 0; i--) {
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

        for(/*Leave i*/ ;i < ext->digit_count && digits[i] != '\0'; i++) {
            (*buf_p) = digits[i];
            buf_p++;
        }
    }

    /*Refresh the text*/
    lv_ta_set_text(spinbox, (char*)buf);


    /*Set the cursor position*/
    int32_t step = ext->step;
    uint8_t cur_pos = ext->digit_count;
    while(step >= 10)
    {
        step /= 10;
        cur_pos--;
    }

    if(cur_pos > intDigits ) cur_pos ++;   /*Skip teh decimal point*/

    cur_pos += ext->digit_padding_left;

    lv_ta_set_cursor_pos(spinbox, cur_pos);
}

#endif
