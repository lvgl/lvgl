/**
 * @file lv_spinbox.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinbox_private.h"
#include "../../core/lv_obj_class_private.h"
#if LV_USE_SPINBOX

#include "../../misc/lv_assert.h"
#include "../../indev/lv_indev.h"
#include "../../stdlib/lv_string.h"
#include "../../core/lv_observer_private.h"

/*********************
 *      DEFINES
 *********************/
#define MY_CLASS (&lv_spinbox_class)
#define LV_SPINBOX_MAX_DIGIT_COUNT_WITH_8BYTES (LV_SPINBOX_MAX_DIGIT_COUNT + 8U)
#define LV_SPINBOX_MAX_DIGIT_COUNT_WITH_4BYTES (LV_SPINBOX_MAX_DIGIT_COUNT + 4U)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_spinbox_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_spinbox_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_spinbox_updatevalue(lv_obj_t * obj);

#if LV_USE_OBSERVER
    static void spinbox_value_changed_event_cb(lv_event_t * e);
    static void spinbox_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject);
#endif /*LV_USE_OBSERVER*/

/**********************
 *  STATIC VARIABLES
 **********************/
const lv_obj_class_t lv_spinbox_class = {
    .constructor_cb = lv_spinbox_constructor,
    .event_cb = lv_spinbox_event,
    .width_def = LV_DPI_DEF,
    .instance_size = sizeof(lv_spinbox_t),
    .editable = LV_OBJ_CLASS_EDITABLE_TRUE,
    .base_class = &lv_textarea_class,
    .name = "lv_spinbox",
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

void lv_spinbox_set_value(lv_obj_t * obj, int32_t v)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    if(v > spinbox->range_max) v = spinbox->range_max;
    if(v < spinbox->range_min) v = spinbox->range_min;

    spinbox->value = v;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_rollover(lv_obj_t * obj, bool rollover)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->rollover = rollover;
}

void lv_spinbox_set_digit_format(lv_obj_t * obj, uint32_t digit_count, uint32_t sep_pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    if(digit_count > LV_SPINBOX_MAX_DIGIT_COUNT) digit_count = LV_SPINBOX_MAX_DIGIT_COUNT;

    if(sep_pos >= digit_count) sep_pos = 0;

    if(digit_count < LV_SPINBOX_MAX_DIGIT_COUNT) {
        const int64_t max_val = lv_pow(10, digit_count);
        if(spinbox->range_max > max_val - 1) spinbox->range_max = max_val - 1;
        if(spinbox->range_min < -max_val  + 1) spinbox->range_min = -max_val  + 1;
    }

    spinbox->digit_count   = digit_count;
    spinbox->dec_point_pos = sep_pos;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_digit_count(lv_obj_t * obj, uint32_t digit_count)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    if(digit_count > LV_SPINBOX_MAX_DIGIT_COUNT) digit_count = LV_SPINBOX_MAX_DIGIT_COUNT;

    spinbox->digit_count = digit_count;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_dec_point_pos(lv_obj_t * obj, uint32_t dec_point_pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->dec_point_pos = dec_point_pos;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_step(lv_obj_t * obj, uint32_t step)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->step = step;
    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_range(lv_obj_t * obj, int32_t min_value, int32_t max_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->range_max = max_value;
    spinbox->range_min = min_value;

    if(spinbox->value > spinbox->range_max) spinbox->value = spinbox->range_max;
    if(spinbox->value < spinbox->range_min) spinbox->value = spinbox->range_min;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_min_value(lv_obj_t * obj, int32_t min_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->range_min = min_value;

    if(spinbox->value < spinbox->range_min) spinbox->value = spinbox->range_min;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_max_value(lv_obj_t * obj, int32_t max_value)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    spinbox->range_max = max_value;

    if(spinbox->value > spinbox->range_max) spinbox->value = spinbox->range_max;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_set_cursor_pos(lv_obj_t * obj, uint32_t pos)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    const int32_t step_limit = LV_MAX(spinbox->range_max, LV_ABS(spinbox->range_min));
    const int32_t new_step = lv_pow(10, pos);

    if(pos <= 0) spinbox->step = 1;
    else if(new_step <= step_limit) spinbox->step = new_step;

    lv_spinbox_updatevalue(obj);
}

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

int32_t lv_spinbox_get_value(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    return spinbox->value;
}

int32_t lv_spinbox_get_step(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    return spinbox->step;
}

/*=====================
 * Other functions
 *====================*/

void lv_spinbox_step_next(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    const int32_t new_step = spinbox->step / 10;
    spinbox->step = (new_step > 0) ? new_step : 1;

    lv_spinbox_updatevalue(obj);
}

void lv_spinbox_step_prev(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    const int32_t step_limit = LV_MAX(spinbox->range_max, LV_ABS(spinbox->range_min));
    const int32_t new_step = spinbox->step * 10;
    if(new_step <= step_limit) spinbox->step = new_step;

    lv_spinbox_updatevalue(obj);
}

bool lv_spinbox_get_rollover(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    return spinbox->rollover;
}

void lv_spinbox_increment(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    int32_t v = spinbox->value;
    /* Special mode when zero crossing. E.g -3+10 should be 3, not 7.
     * Pretend we are on -7 now.*/
    if((spinbox->value < 0) && (spinbox->value + spinbox->step) > 0) {
        v = -(spinbox->step + spinbox->value);
    }

    if(v + spinbox->step <= spinbox->range_max) {
        v += spinbox->step;
    }
    else {
        /*Rollover?*/
        if((spinbox->rollover) && (spinbox->value == spinbox->range_max))
            v = spinbox->range_min;
        else
            v = spinbox->range_max;
    }

    if(v != spinbox->value) {
        spinbox->value = v;
        lv_spinbox_updatevalue(obj);
    }
}

void lv_spinbox_decrement(lv_obj_t * obj)
{
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;

    int32_t v = spinbox->value;
    /* Special mode when zero crossing. E.g 3-10 should be -3, not -7.
     * Pretend we are on 7 now.*/
    if((spinbox->value > 0) && (spinbox->value - spinbox->step) < 0) {
        v = spinbox->step - spinbox->value;
    }

    if(v - spinbox->step >= spinbox->range_min) {
        v -= spinbox->step;
    }
    else {
        /*Rollover?*/
        if((spinbox->rollover) && (spinbox->value == spinbox->range_min))
            v = spinbox->range_max;
        else
            v = spinbox->range_min;
    }

    if(v != spinbox->value) {
        spinbox->value = v;
        lv_spinbox_updatevalue(obj);
    }
}

#if LV_USE_OBSERVER
lv_observer_t * lv_spinbox_bind_value(lv_obj_t * obj, lv_subject_t * subject)
{
    LV_ASSERT_NULL(subject);
    LV_ASSERT_NULL(obj);

    if(subject->type != LV_SUBJECT_TYPE_INT && subject->type != LV_SUBJECT_TYPE_FLOAT) {
        LV_LOG_WARN("Incompatible subject type: %d", subject->type);
        return NULL;
    }

    lv_obj_add_event_cb(obj, spinbox_value_changed_event_cb, LV_EVENT_VALUE_CHANGED, subject);

    lv_observer_t * observer = lv_subject_add_observer_obj(subject, spinbox_value_observer_cb, obj, NULL);
    return observer;
}
#endif /*LV_USE_OBSERVER*/

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
    lv_result_t res = LV_RESULT_OK;
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) return;

    const lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_current_target(e);
    lv_spinbox_t * spinbox = (lv_spinbox_t *)obj;
    if(code == LV_EVENT_RELEASED) {
        /*If released with an ENCODER then move to the next digit*/
        lv_indev_t * indev = lv_indev_active();
        if(lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER && lv_group_get_editing(lv_obj_get_group(obj))) {
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
        /*The cursor has been positioned to a digit.
         * Set `step` accordingly*/
        else {
            const char * txt = lv_textarea_get_text(obj);
            const size_t txt_len = lv_strlen(txt);

            /* Check cursor position */
            /* Cursor is in '.' digit */
            if(txt[spinbox->ta.cursor.pos] == '.') {
                lv_textarea_cursor_left(obj);
            }
            /* Cursor is already in the right-most digit */
            else if(spinbox->ta.cursor.pos == (uint32_t)txt_len) {
                lv_textarea_set_cursor_pos(obj, txt_len - 1);
            }
            /* Cursor is already in the left-most digit AND range_min is negative */
            else if(spinbox->ta.cursor.pos == 0 && spinbox->range_min < 0) {
                lv_textarea_set_cursor_pos(obj, 1);
            }

            /* Handle spinbox with decimal point (spinbox->dec_point_pos != 0) */
            uint32_t cp = spinbox->ta.cursor.pos;
            if(spinbox->ta.cursor.pos > spinbox->dec_point_pos && spinbox->dec_point_pos != 0) cp--;

            const size_t len = spinbox->digit_count - 1;
            uint32_t pos = len - cp;

            if(spinbox->range_min < 0) pos++;

            spinbox->step = 1;
            uint32_t i;
            for(i = 0; i < pos; i++) spinbox->step *= 10;

            lv_spinbox_updatevalue(obj);
        }
    }
    else if(code == LV_EVENT_KEY) {
        lv_indev_type_t indev_type = lv_indev_get_type(lv_indev_active());

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

    /* LV_SPINBOX_MAX_DIGIT_COUNT_WITH_8BYTES (18): Max possible digit_count value (15) + sign + decimal point + NULL terminator */
    char textarea_txt[LV_SPINBOX_MAX_DIGIT_COUNT_WITH_8BYTES] = {0U};
    char * buf_p = textarea_txt;

    uint32_t cur_shift_left = 0;
    if(spinbox->range_min < 0) {  /*hide sign if there are only positive values*/
        /*Add the sign*/
        (*buf_p) = spinbox->value >= 0 ? '+' : '-';
        buf_p++;
    }
    else {
        /*Cursor need shift to left*/
        cur_shift_left++;
    }

    /*Convert the numbers to string (the sign is already handled so always convert positive number)*/
    char digits[LV_SPINBOX_MAX_DIGIT_COUNT_WITH_4BYTES];
    lv_snprintf(digits, LV_SPINBOX_MAX_DIGIT_COUNT_WITH_4BYTES, "%" LV_PRId32, LV_ABS(spinbox->value));

    /*Add leading zeros*/
    int32_t i;
    const size_t digits_len = lv_strlen(digits);

    const int leading_zeros_cnt = spinbox->digit_count - digits_len;
    if(leading_zeros_cnt > 0) {
        for(i = (int32_t) digits_len; i >= 0; i--) {
            digits[i + leading_zeros_cnt] = digits[i];
        }
        /* NOTE: Substitute with memset? */
        for(i = 0; i < leading_zeros_cnt; i++) {
            digits[i] = '0';
        }
    }

    /*Add the decimal part*/
    const uint32_t int_digits = (spinbox->dec_point_pos == 0) ? spinbox->digit_count : spinbox->dec_point_pos;
    for(i = 0; i < (int32_t)int_digits && digits[i] != '\0'; i++) {
        (*buf_p) = digits[i];
        buf_p++;
    }

    /*Insert the decimal point*/
    if(spinbox->dec_point_pos) {
        (*buf_p) = '.';
        buf_p++;

        for(/*Leave i*/; i < spinbox->digit_count && digits[i] != '\0'; i++) {
            (*buf_p) = digits[i];
            buf_p++;
        }
    }

    /*Refresh the text*/
    lv_textarea_set_text(obj, (char *)textarea_txt);

    /*Set the cursor position*/
    int32_t step = spinbox->step;
    uint32_t cur_pos = (uint32_t)spinbox->digit_count;
    while(step >= 10) {
        step /= 10;
        cur_pos--;
    }

    if(cur_pos > int_digits) cur_pos++; /*Skip the decimal point*/

    cur_pos -= cur_shift_left;

    lv_textarea_set_cursor_pos(obj, cur_pos);
}


#if LV_USE_OBSERVER

static void spinbox_value_changed_event_cb(lv_event_t * e)
{
    lv_obj_t * arc = lv_event_get_current_target(e);
    lv_subject_t * subject = lv_event_get_user_data(e);

    if(subject->type == LV_SUBJECT_TYPE_INT) {
        lv_subject_set_int(subject, lv_spinbox_get_value(arc));
    }
}

static void spinbox_value_observer_cb(lv_observer_t * observer, lv_subject_t * subject)
{
    if(subject->type == LV_SUBJECT_TYPE_INT) {
        lv_spinbox_set_value(observer->target, subject->value.num);
    }
}

#endif /*LV_USE_OBSERVER*/


#endif /*LV_USE_SPINBOX*/
