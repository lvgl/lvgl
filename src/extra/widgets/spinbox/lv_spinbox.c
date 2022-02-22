/**
 * @file lv_spinbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinbox.h"
#if LV_USE_SPINBOX

#include "../../../misc/lv_assert.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS    &lv_spinbox_class

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_spinbox_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_spinbox_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_spinbox_updatevalue(lv_obj_t * obj);

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_spinbox_class = {
    .constructor_cb = lv_spinbox_constructor,
    .event_cb = lv_spinbox_event,
    .width_def = LV_DPI_DEF,
    .instance_size = sizeof(lv_spinbox_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &lv_textarea_class
};
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t * lv_spinbox_create(lv_obj_t * parent)
{
    LV_LOG_INFO("begin");
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

/*=====================
 * Setter functions
 *====================*/

/**
 * Set spinbox value
 * @param obj pointer to spinbox
 * @param i value to be set
 */
void lv_spinbox_set_value(lv_obj_t * obj, int32_t i)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    if(i > spinbox->range_max) i = spinbox->range_max;
    if(i < spinbox->range_min) i = spinbox->range_min;

    spinbox->value = i;

    lv_spinbox_updatevalue(obj);
}

/**
 * Set spinbox rollover function
 * @param spinbox pointer to spinbox
 * @param b true or false to enable or disable (default)
 */
void lv_spinbox_set_rollover(lv_obj_t * obj, bool b)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->rollover = b;
}

/**
 * Set spinbox digit format (digit count and decimal format)
 * @param spinbox pointer to spinbox
 * @param digit_count number of digit excluding the decimal separator and the sign
 * @param separator_position number of digit before the decimal point. If 0, decimal point is not
 * shown
 */
void lv_spinbox_set_digit_format(lv_obj_t * obj, uint8_t digit_count, uint8_t separator_position)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    if(digit_count > LV_SPINBOX_MAX_DIGIT_COUNT) digit_count = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(separator_position >= digit_count) separator_position = 0;
    if(separator_position > LV_SPINBOX_MAX_DIGIT_COUNT) separator_position = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(digit_count < LV_SPINBOX_MAX_DIGIT_COUNT) {
        int64_t max_val = lv_pow(10, digit_count);
        if(spinbox->range_max > max_val - 1) spinbox->range_max = max_val - 1;
        if(spinbox->range_min < - max_val  + 1) spinbox->range_min = - max_val  + 1;
    }

    spinbox->digit_count   = digit_count;
    spinbox->dec_point_pos = separator_position;

    lv_spinbox_updatevalue(obj);
}

/**
 * Set spinbox step
 * @param spinbox pointer to spinbox
 * @param step steps on increment/decrement
 */
void lv_spinbox_set_step(lv_obj_t * obj, uint32_t step)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->step = step;
    lv_spinbox_updatevalue(obj);
}

/**
 * Set spinbox value range
 * @param spinbox pointer to spinbox
 * @param range_min maximum value, inclusive
 * @param range_max minimum value, inclusive
 */
void lv_spinbox_set_range(lv_obj_t * obj, int32_t range_min, int32_t range_max)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->range_max = range_max;
    spinbox->range_min = range_min;

    if(spinbox->value > spinbox->range_max) spinbox->value = spinbox->range_max;
    if(spinbox->value < spinbox->range_min) spinbox->value = spinbox->range_min;

    lv_spinbox_updatevalue(obj);
}

/**
 * Set cursor position to a specific digit for edition
 * @param spinbox pointer to spinbox
 * @param pos selected position in spinbox
 */
void lv_spinbox_set_pos(lv_obj_t * obj, uint8_t pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;
    int32_t step_limit;
    step_limit       = LV_MAX(spinbox->range_max, (spinbox->range_min < 0 ? (-spinbox->range_min) : spinbox->range_min));
    int32_t new_step = spinbox->step * lv_pow(10, pos);
    if(pos <= 0) spinbox->step = 1;
    else if(new_step <= step_limit) spinbox->step = new_step;

    lv_spinbox_updatevalue(obj);
}

/**
 * Set direction of digit step when clicking an encoder button while in editing mode
 * @param spinbox pointer to spinbox
 * @param direction the direction (LV_DIR_RIGHT or LV_DIR_LEFT)
 */
void lv_spinbox_set_digit_step_direction(lv_obj_t * obj, lv_dir_t direction)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;
    spinbox->digit_step_dir = direction;

    lv_spinbox_updatevalue(obj);
}
/*=====================
 * Getter functions
 *====================*/

/**
 * Get the spinbox numeral value (user has to convert to float according to its digit format)
 * @param obj pointer to spinbox
 * @return value integer value of the spinbox
 */
int32_t lv_spinbox_get_value(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    return spinbox->value;
}
/**
 * Get the spinbox step value (user has to convert to float according to its digit format)
 * @param obj pointer to spinbox
 * @return value integer step value of the spinbox
 */
int32_t lv_spinbox_get_step(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    return spinbox->step;
}

/*=====================
 * Other functions
 *====================*/

/**
 * Select next lower digit for edition
 * @param obj pointer to spinbox
 */
void lv_spinbox_step_next(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    int32_t new_step = spinbox->step / 10;
    if((new_step) > 0)
        spinbox->step = new_step;
    else
        spinbox->step = 1;

    lv_spinbox_updatevalue(obj);
}

/**
 * Select next higher digit for edition
 * @param obj pointer to spinbox
 */
void lv_spinbox_step_prev(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;
    int32_t step_limit;
    step_limit       = LV_MAX(spinbox->range_max, (spinbox->range_min < 0 ? (-spinbox->range_min) : spinbox->range_min));
    int32_t new_step = spinbox->step * 10;
    if(new_step <= step_limit) spinbox->step = new_step;

    lv_spinbox_updatevalue(obj);
}

/**
 * Get spinbox rollover function status
 * @param obj pointer to spinbox
 */
bool lv_spinbox_get_rollover(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    return spinbox->rollover;
}

/**
 * Increment spinbox value by one step
 * @param obj pointer to spinbox
 */
void lv_spinbox_increment(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    if(spinbox->value + spinbox->step <= spinbox->range_max) {
        /*Special mode when zero crossing*/
        if((spinbox->value + spinbox->step) > 0 && spinbox->value < 0) spinbox->value = -spinbox->value;
        spinbox->value += spinbox->step;

    }
    else {
        // Rollover?
        if((spinbox->rollover) && (spinbox->value == spinbox->range_max))
            spinbox->value = spinbox->range_min;
        else
            spinbox->value = spinbox->range_max;
    }

    lv_spinbox_updatevalue(obj);
}

/**
 * Decrement spinbox value by one step
 * @param obj pointer to spinbox
 */
void lv_spinbox_decrement(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    if(spinbox->value - spinbox->step >= spinbox->range_min) {
        /*Special mode when zero crossing*/
        if((spinbox->value - spinbox->step) < 0 && spinbox->value > 0) spinbox->value = -spinbox->value;
        spinbox->value -= spinbox->step;
    }
    else {
        /*Rollover?*/
        if((spinbox->rollover) && (spinbox->value == spinbox->range_min))
            spinbox->value = spinbox->range_max;
        else
            spinbox->value = spinbox->range_min;
    }

    lv_spinbox_updatevalue(obj);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_spinbox_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_LOG_TRACE("begin");

    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    /*Initialize the allocated 'ext'*/
    spinbox->value              = 0;
    spinbox->dec_point_pos      = 0;
    spinbox->digit_count        = 5;
    spinbox->step               = 1;
    spinbox->range_max          = 99999;
    spinbox->range_min          = -99999;
    spinbox->rollover           = false;
    spinbox->digit_step_dir     = LV_DIR_RIGHT;

    lv_textarea_set_one_line(obj, true);
    lv_textarea_set_cursor_click_pos(obj, true);

    lv_spinbox_updatevalue(obj);

    LV_LOG_TRACE("Spinbox constructor finished");
}

static void lv_spinbox_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);

    /*Call the ancestor's event handler*/
    lv_res_t res = LV_RES_OK;
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RES_OK) return;

    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;
    if(code == LV_EVENT_RELEASED) {
        /*If released with an ENCODER then move to the next digit*/
        lv_indev_t * indev = lv_indev_get_act();
        if(lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
            if(lv_group_get_editing(lv_obj_get_group(obj))) {
                if(spinbox->digit_count > 1) {
                    if(spinbox->digit_step_dir == LV_DIR_RIGHT) {
                        if(spinbox->step > 1) {
                            lv_spinbox_step_next(obj);
                        }
                        else {
                            /*Restart from the MSB*/
                            spinbox->step = lv_pow(10, spinbox->digit_count - 2);
                            lv_spinbox_step_prev(obj);
                        }
                    }
                    else {
                        if(spinbox->step < lv_pow(10, spinbox->digit_count - 1)) {
                            lv_spinbox_step_prev(obj);
                        }
                        else {
                            /*Restart from the LSB*/
                            spinbox->step = 10;
                            lv_spinbox_step_next(obj);
                        }
                    }
                }
            }
        }
        /*The cursor has been positioned to a digit.
         * Set `step` accordingly*/
        else {
            const char * txt = lv_textarea_get_text(obj);
            size_t txt_len = strlen(txt);

            if(txt[spinbox->ta.cursor.pos] == '.') {
                lv_textarea_cursor_left(obj);
            }
            else if(spinbox->ta.cursor.pos == (uint32_t)txt_len) {
                lv_textarea_set_cursor_pos(obj, txt_len - 1);
            }
            else if(spinbox->ta.cursor.pos == 0 && spinbox->range_min < 0) {
                lv_textarea_set_cursor_pos(obj, 1);
            }

            size_t len = spinbox->digit_count - 1;
            uint16_t cp = spinbox->ta.cursor.pos;

            if(spinbox->ta.cursor.pos > spinbox->dec_point_pos && spinbox->dec_point_pos != 0) cp--;
            uint32_t pos = len - cp;

            if(spinbox->range_min < 0) pos++;

            spinbox->step = 1;
            uint16_t i;
            for(i = 0; i < pos; i++) spinbox->step *= 10;
        }
    }
    else if(code == LV_EVENT_KEY) {
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_get_act());

        uint32_t c = *((uint32_t *)lv_event_get_param(e)); /*uint32_t because can be UTF-8*/
        if(c == LV_KEY_RIGHT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER)
                lv_spinbox_increment(obj);
            else
                lv_spinbox_step_next(obj);
        }
        else if(c == LV_KEY_LEFT) {
            if(indev_type == LV_INDEV_TYPE_ENCODER)
                lv_spinbox_decrement(obj);
            else
                lv_spinbox_step_prev(obj);
        }
        else if(c == LV_KEY_UP) {
            lv_spinbox_increment(obj);
        }
        else if(c == LV_KEY_DOWN) {
            lv_spinbox_decrement(obj);
        }
        else {
            lv_textarea_add_char(obj, c);
        }
    }
}

static void lv_spinbox_updatevalue(lv_obj_t * obj)
{
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    char buf[LV_SPINBOX_MAX_DIGIT_COUNT + 8];
    lv_memset_00(buf, sizeof(buf));
    char * buf_p = buf;
    uint8_t cur_shift_left = 0;

    if(spinbox->range_min < 0) {  // hide sign if there are only positive values
        /*Add the sign*/
        (*buf_p) = spinbox->value >= 0 ? '+' : '-';
        buf_p++;
    }
    else {
        /*Cursor need shift to left*/
        cur_shift_left++;
    }

    int32_t i;
    char digits[LV_SPINBOX_MAX_DIGIT_COUNT + 4];
    /*Convert the numbers to string (the sign is already handled so always covert positive number)*/
    lv_snprintf(digits, sizeof(digits), "%" LV_PRId32, LV_ABS(spinbox->value));

    /*Add leading zeros*/
    int lz_cnt = spinbox->digit_count - (int)strlen(digits);
    if(lz_cnt > 0) {
        for(i = (uint16_t)strlen(digits); i >= 0; i--) {
            digits[i + lz_cnt] = digits[i];
        }
        for(i = 0; i < lz_cnt; i++) {
            digits[i] = '0';
        }
    }

    int32_t intDigits;
    intDigits = (spinbox->dec_point_pos == 0) ? spinbox->digit_count : spinbox->dec_point_pos;

    /*Add the decimal part*/
    for(i = 0; i < intDigits && digits[i] != '\0'; i++) {
        (*buf_p) = digits[i];
        buf_p++;
    }

    if(spinbox->dec_point_pos != 0) {
        /*Insert the decimal point*/
        (*buf_p) = '.';
        buf_p++;

        for(/*Leave i*/; i < spinbox->digit_count && digits[i] != '\0'; i++) {
            (*buf_p) = digits[i];
            buf_p++;
        }
    }

    /*Refresh the text*/
    lv_textarea_set_text(obj, (char *)buf);

    /*Set the cursor position*/
    int32_t step    = spinbox->step;
    uint8_t cur_pos = (uint8_t)spinbox->digit_count;
    while(step >= 10) {
        step /= 10;
        cur_pos--;
    }

    if(cur_pos > intDigits) cur_pos++; /*Skip the decimal point*/

    cur_pos -= cur_shift_left;

    lv_textarea_set_cursor_pos(obj, cur_pos);
}

#endif /*LV_USE_SPINBOX*/
