/**
 * @file lv_templ.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_spinbox.h"

#if USE_LV_SPINBOX != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static bool lv_spinbox_design(lv_obj_t * templ, const lv_area_t * mask, lv_design_mode_t mode);
static lv_res_t lv_spinbox_signal(lv_obj_t * spinbox, lv_signal_t sign, void * param);
static void lv_spinbox_updatevalue(lv_obj_t * spinbox);
static int32_t lv_spinbox_double_to_int(lv_obj_t * spinbox, double d);
static double lv_spinbox_int_to_double(lv_obj_t * spinbox, int32_t i);

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
	ext->ta.accapted_chars = "1234567890+-.";

	ext->value = 0.0;
	ext->valueDigit = 0;
	ext->decPointPos = 2;
	ext->digitCount = 5;
	ext->step = 100;
	ext->rangeMax = 99999;
	ext->rangeMin = -99999;

	lv_ta_set_cursor_type(new_spinbox, LV_CURSOR_BLOCK | LV_CURSOR_HIDDEN); /*hidden by default*/
	lv_ta_set_cursor_pos(new_spinbox, 4);


	/*The signal and design functions are not copied so set them here*/
	lv_obj_set_signal_func(new_spinbox, lv_spinbox_signal);
	lv_obj_set_design_func(new_spinbox, lv_spinbox_design);

	/*Init the new spinbox spinbox*/
	if(copy == NULL) {

	}
	/*Copy an existing spinbox*/
	else {
		lv_spinbox_ext_t * copy_ext = lv_obj_get_ext_attr(copy);

		/*Refresh the style with new signal function*/
		lv_obj_refresh_style(new_spinbox);
	}

	lv_spinbox_updatevalue(new_spinbox);

	LV_LOG_INFO("spinbox created");

	return new_spinbox;
}


void lv_spinbox_step_next(lv_obj_t * spinbox)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);


	if((ext->step / 10) < ext->rangeMax && (ext->step / 10) > ext->rangeMin && (ext->step / 10) > 0)
	{
		ext->step /= 10;
	}

	lv_spinbox_updatevalue(spinbox);
}

void lv_spinbox_step_previous(lv_obj_t * spinbox)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);


	if((ext->step * 10) <= ext->rangeMax && (ext->step * 10) > ext->rangeMin && (ext->step * 10) > 0)
	{
		ext->step *= 10;
	}

	lv_spinbox_updatevalue(spinbox);
}

/*======================
 * Add/remove functions
 *=====================*/

/*
 * New object specific "add" or "remove" functions come here
 */


/*=====================
 * Setter functions
 *====================*/

/*
 * New object specific "set" functions come here
 */
void lv_spinbox_set_double(const lv_obj_t * spinbox, double d)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
	if(ext == NULL)
		return;
}

void lv_spinbox_set_int(const lv_obj_t * spinbox, int32_t i)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
	if(ext == NULL)
		return;

	if(i > ext->rangeMax)
		i = ext->rangeMax;
	if(i < ext->rangeMin)
		i = ext->rangeMin;

	ext->valueDigit = i;
}

void lv_spinbox_set_digit_format(const lv_obj_t * spinbox, uint8_t digit_count, uint8_t separator_position)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
	if(ext == NULL)
		return;

	if(digit_count > LV_SPINBOX_MAX_DIGIT_COUNT)
		digit_count = LV_SPINBOX_MAX_DIGIT_COUNT;

	if(separator_position < LV_SPINBOX_MAX_DIGIT_COUNT)
		separator_position = LV_SPINBOX_MAX_DIGIT_COUNT;

	ext->digitCount = digit_count;
	ext->decPointPos = separator_position;

	lv_spinbox_updatevalue(spinbox);
}

void lv_spinbox_set_range_int(const lv_obj_t * spinbox, int32_t rangeMin, int32_t rangeMax)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
	if(ext == NULL)
		return;

	ext->rangeMax = rangeMax;
	ext->rangeMin = rangeMin;

	if(ext->valueDigit > ext->rangeMax)
	{
		ext->valueDigit = ext->rangeMax;
		lv_obj_invalidate(spinbox);
	}
	if(ext->valueDigit < ext->rangeMin)
	{
		ext->valueDigit = ext->rangeMin;
		lv_obj_invalidate(spinbox);
	}
}

void lv_spinbox_set_range_double(const lv_obj_t * spinbox, double rangeMin, double rangeMax)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
	if(ext == NULL)
		return;

	double rMinDouble = lv_spinbox_double_to_int(spinbox, rangeMin);
	double rMaxDouble = lv_spinbox_double_to_int(spinbox, rangeMax);

	lv_spinbox_set_range_int(spinbox, rMinDouble, rMaxDouble);
}


/**
 * Set a style of a spinbox.
 * @param templ pointer to spinbox object
 * @param type which style should be set
 * @param style pointer to a style
 */
void lv_spinbox_set_style(lv_obj_t * spinbox, lv_spinbox_style_t type, lv_style_t * style)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

	switch(type) {
	case LV_SPINBOX_STYLE_BG:
		lv_page_set_style(spinbox, LV_PAGE_STYLE_BG, style);
		break;
	case LV_SPINBOX_STYLE_SB:
		lv_page_set_style(spinbox, LV_PAGE_STYLE_SB, style);
		break;
	case LV_SPINBOX_STYLE_CURSOR:
		ext->ta.cursor.style = style;
		lv_obj_refresh_ext_size(lv_page_get_scrl(spinbox)); /*Refresh ext. size because of cursor drawing*/
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
 * Get style of a spinbox.
 * @param templ pointer to spinbox object
 * @param type which style should be get
 * @return style pointer to the style
 */
lv_style_t * lv_spinbox_get_style(const lv_obj_t * templ, lv_spinbox_style_t type)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(templ);

	switch(type) {
	default:
		return NULL;
	}

	/*To avoid warning*/
	return NULL;
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
 * Handle the drawing related tasks of the spinboxs
 * @param templ pointer to an object
 * @param mask the object will be drawn only in this area
 * @param mode LV_DESIGN_COVER_CHK: only check if the object fully covers the 'mask_p' area
 *                                  (return 'true' if yes)
 *             LV_DESIGN_DRAW: draw the object (always return 'true')
 *             LV_DESIGN_DRAW_POST: drawing after every children are drawn
 * @param return true/false, depends on 'mode'
 */
static bool lv_spinbox_design(lv_obj_t * templ, const lv_area_t * mask, lv_design_mode_t mode)
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

/**
 * Signal function of the spinbox
 * @param templ pointer to a spinbox object
 * @param sign a signal type from lv_signal_t enum
 * @param param pointer to a signal specific variable
 * @return LV_RES_OK: the object is not deleted in the function; LV_RES_INV: the object is deleted
 */
static lv_res_t lv_spinbox_signal(lv_obj_t * spinbox, lv_signal_t sign, void * param)
{

	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

	lv_res_t res;

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
	}else if(sign == LV_SIGNAL_CONTROLL)
	{
		uint32_t c = *((uint32_t *)param);      /*uint32_t because can be UTF-8*/
		if(c == LV_GROUP_KEY_RIGHT)
		{
			if(ext->valueDigit - ext->step >= ext->rangeMin)
			{
				/*Special mode when zero crossing*/
				if((ext->valueDigit - ext->step) < 0 && ext->valueDigit > 0)
				{
					ext->valueDigit = -ext->valueDigit;
				}/*end special mode*/

				ext->valueDigit -= ext->step;
			}

			lv_spinbox_updatevalue(spinbox);
			lv_label_set_text(ext->ta.label, (char*)ext->digits);
		}
		else if(c == LV_GROUP_KEY_LEFT)
		{
			if(ext->valueDigit + ext->step <= ext->rangeMax)
			{
				/*Special mode when zero crossing*/
				if((ext->valueDigit + ext->step) > 0 && ext->valueDigit < 0)
				{
					ext->valueDigit = -ext->valueDigit;
				}/*end special mode*/
				ext->valueDigit += ext->step;
			}
			lv_spinbox_updatevalue(spinbox);
			lv_label_set_text(ext->ta.label, (char*)ext->digits);
		}
		else if(c == LV_GROUP_KEY_UP)
		{
			lv_ta_cursor_up(spinbox);
		}
		else if(c == LV_GROUP_KEY_DOWN)
		{
			lv_ta_cursor_down(spinbox);
		}
		else
		{
			if(c == LV_GROUP_KEY_ENTER)
			{
				int p = lv_ta_get_cursor_pos(spinbox);
				if(p == ext->digitCount + 1)
				{
					for(int i = 0; i < ext->digitCount; i++)
						lv_spinbox_step_previous(spinbox);
				} else
				{
					lv_spinbox_step_next(spinbox);
				}


				lv_spinbox_updatevalue(spinbox);
				lv_label_set_text(ext->ta.label, (char*)ext->digits);
			}
			else
			{
				lv_ta_add_char(spinbox, c);
			}
		}
	}


	return res;
}

static void lv_spinbox_updatevalue(lv_obj_t * spinbox)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);
	int32_t v = ext->valueDigit;
	int32_t intDigits, decDigits;
	uint8_t dc = ext->digitCount;

	intDigits = (ext->decPointPos==0)?ext->digitCount:ext->decPointPos;
	decDigits = ext->digitCount - intDigits;

	ext->digits[0] = v>=0?'+':'-';

	int i = 0;
	uint8_t digits[16];

	if(v < 0)
		v = -v;
	for(i = 0; i < dc; i++)
	{
		digits[i] = v%10;
		v = v/10;
	}

	int k;
	for(k = 0; k < intDigits; k++)
	{
		ext->digits[1 + k] = '0' + digits[--i];
	}

	ext->digits[1 + intDigits] = '.';

	int d;

	for(d = 0; d < decDigits; d++)
	{
		ext->digits[1 + intDigits + 1 + d] = '0' + digits[--i];
	}

	ext->digits[1 + intDigits + 1 + decDigits] = '\0';

	lv_label_set_text(ext->ta.label, (char*)ext->digits);

	int32_t step = ext->step;
	uint8_t cPos = ext->digitCount;
	while(step >= 10)
	{
		step /= 10;
		cPos--;
	}
	if(cPos > intDigits)
	{
		cPos ++;
	}

	lv_ta_set_cursor_pos(spinbox, cPos);
}

static double lv_spinbox_int_to_double(lv_obj_t * spinbox, int32_t i)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

	/*
	if(i > ext->rangeMax)
		i = ext->rangeMax;
	if(i < ext->rangeMin)
		i = ext->rangeMin;
	*/

	return (double)( i / (10 * (ext->digitCount - ext->decPointPos)));
}

static int32_t lv_spinbox_double_to_int(lv_obj_t * spinbox, double d)
{
	lv_spinbox_ext_t * ext = lv_obj_get_ext_attr(spinbox);

	int32_t i = (int32_t)(d * (10 * (ext->digitCount - ext->decPointPos)));

	/*
	if(i > ext->rangeMax)
		i = ext->rangeMax;
	if(i < ext->rangeMin)
		i = ext->rangeMin;
	*/

	return i;
}

#endif
