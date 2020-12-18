/**
 * @file lv_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include "../lv_misc/lv_mem.h"
#include "../lv_misc/lv_anim.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value);
static bool get_prop(const lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value);
static bool remove_prop(lv_style_t * style, lv_style_prop_t prop);

/**********************
 *  GLOBAL VARIABLES
 **********************/
lv_style_class_t lv_style;

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void _lv_style_system_init(void)
{
    LV_CLASS_INIT(lv_style, lv_base);
    lv_style.constructor = NULL;
    lv_style.remove_prop = remove_prop;
    lv_style.set_prop = set_prop;
    lv_style.get_prop = get_prop;
}

/**
 * Initialize a style
 * @param style pointer to a style to initialize
 */
void lv_style_init(lv_style_t * style)
{
    _lv_memset_00(style, sizeof(lv_style_t));
    style->class_p = &lv_style;
#if LV_USE_ASSERT_STYLE
    style->sentinel = LV_DEBUG_STYLE_SENTINEL_VALUE;
#endif

}

uint16_t lv_style_register_prop(bool inherit)
{
    static uint16_t act_id = (uint16_t)_LV_STYLE_LAST_BUIL_IN_PROP;
    act_id++;
    if(inherit) return act_id | LV_STYLE_PROP_INHERIT;
    else return act_id;
}

/**
 * Remove a property from a style
 * @param style pointer to a style
 * @param prop  a style property ORed with a state.
 * E.g. `LV_STYLE_BORDER_WIDTH | (LV_STATE_PRESSED << LV_STYLE_STATE_POS)`
 * @return true: the property was found and removed; false: the property wasn't found
 */
bool lv_style_remove_prop(lv_style_t * style, lv_style_prop_t prop)
{
    return style->class_p->remove_prop(style, prop);
}

/**
 * Clear all properties from a style and all allocated memories.
 * @param style pointer to a style
 */
void lv_style_reset(lv_style_t * style)
{
    LV_ASSERT_STYLE(style);
    lv_style_init(style);
}

void _alloc_ext(lv_style_t * style)
{
    if(style->ext) return;
    style->ext = lv_mem_alloc(sizeof(lv_style_ext_t));
    LV_ASSERT_MEM(style->ext);
    _lv_memset_00(style->ext, sizeof(lv_style_ext_t));
}

void lv_style_set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value)
{
    style->class_p->set_prop(style, prop, value);
}

bool lv_style_get_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value)
{
    return style->class_p->get_prop(style, prop, value);
}

lv_style_value_t lv_style_prop_get_default(lv_style_prop_t prop)
{
    lv_style_value_t value;
    switch(prop) {
            break;
        case LV_STYLE_TRANSFORM_ZOOM:
            value._int = LV_IMG_ZOOM_NONE;
            break;
        case LV_STYLE_BG_COLOR:
            value._color = LV_COLOR_WHITE;
            break;
        case LV_STYLE_OPA:
        case LV_STYLE_BORDER_OPA:
        case LV_STYLE_TEXT_OPA:
        case LV_STYLE_IMG_OPA:
        case LV_STYLE_OUTLINE_OPA:
        case LV_STYLE_SHADOW_OPA:
            value._int = LV_OPA_COVER;
            break;
        case LV_STYLE_BG_GRAD_STOP:
            value._int = 255;
            break;
        case LV_STYLE_BORDER_SIDE:
            value._int = LV_BORDER_SIDE_FULL;
            break;
        case LV_STYLE_TEXT_FONT:
            value._ptr = LV_THEME_DEFAULT_FONT_NORMAL;
            break;
        case LV_STYLE_TRANSITION_PATH:
            value._ptr = &lv_anim_path_def;
            break;
        default:
            value._ptr = NULL;
            value._int = 0;
            break;
    }

    return value;

}

/**
 * Check whether a style is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style(const lv_style_t * style)
{
    if(style == NULL) return true;  /*NULL style is still valid*/

#if LV_USE_ASSERT_STYLE
    if(style->sentinel != LV_DEBUG_STYLE_SENTINEL_VALUE) {
        LV_LOG_WARN("Invalid style (local variable or not initialized?)");
        return false;
    }
#endif

    return true;
}

/**
 * Check whether a style list is valid (initialized correctly)
 * @param style pointer to a style
 * @return true: valid
 */
bool lv_debug_check_style_list(const void * list)
{
    return true;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void set_prop(lv_style_t * style, lv_style_prop_t prop, lv_style_value_t value)
{
    if(style == NULL) return;
     LV_ASSERT_STYLE(style);

     switch(prop) {

     case LV_STYLE_RADIUS:
         style->radius = value._int;
         style->has_radius = 1;
         break;
     case LV_STYLE_CLIP_CORNER:
         _alloc_ext(style);
         style->ext->clip_corner = value._int;
         style->ext->has_clip_corner = 1;
         break;
     case LV_STYLE_TRANSFORM_WIDTH:
         _alloc_ext(style);
         style->ext->transform_width = value._int;
         style->ext->has_transform_width = 1;
         break;
     case LV_STYLE_TRANSFORM_HEIGHT:
         _alloc_ext(style);
         style->ext->transform_height = value._int;
         style->ext->has_transform_height = 1;
         break;
     case LV_STYLE_TRANSFORM_ANGLE:
         _alloc_ext(style);
         style->ext->transform_angle = value._int;
         style->ext->has_transform_angle = 1;
         break;
     case LV_STYLE_TRANSFORM_ZOOM:
         _alloc_ext(style);
         style->ext->transform_zoom = value._int;
         style->ext->has_transform_zoom = 1;
         break;
     case LV_STYLE_OPA:
         _alloc_ext(style);
         style->ext->opa = value._int;
         style->ext->has_opa = 1;
         break;

     case LV_STYLE_PAD_TOP:
         style->pad_top = value._int;
         style->has_pad_top = 1;
         break;
     case LV_STYLE_PAD_BOTTOM:
         style->pad_bottom = value._int;
         style->has_pad_bottom = 1;
         break;
     case LV_STYLE_PAD_LEFT:
         style->pad_left = value._int;
         style->has_pad_left = 1;
         break;
     case LV_STYLE_PAD_RIGHT:
         style->pad_right = value._int;
         style->has_pad_right = 1;
         break;
     case LV_STYLE_MARGIN_TOP:
         _alloc_ext(style);
         style->ext->margin_top = value._int;
         style->ext->has_margin_top = 1;
         break;
     case LV_STYLE_MARGIN_BOTTOM:
         _alloc_ext(style);
         style->ext->margin_bottom = value._int;
         style->ext->has_margin_bottom = 1;
         break;
     case LV_STYLE_MARGIN_LEFT:
         _alloc_ext(style);
         style->ext->margin_left = value._int;
         style->ext->has_margin_left = 1;
         break;
     case LV_STYLE_MARGIN_RIGHT:
         _alloc_ext(style);
         style->ext->margin_right = value._int;
         style->ext->has_margin_right = 1;
         break;

     case LV_STYLE_BG_COLOR:
         style->bg_color = value._color;
         style->has_bg_color = 1;
         break;
     case LV_STYLE_BG_OPA:
         style->bg_opa = value._int;
         style->has_bg_opa = 1;
         break;
     case LV_STYLE_BG_GRAD_COLOR:
         _alloc_ext(style);
         style->ext->bg_grad_color = value._color;
         style->ext->has_bg_grad_color = 1;
         break;
     case LV_STYLE_BG_GRAD_DIR:
         _alloc_ext(style);
         style->ext->bg_grad_dir = value._int;
         style->ext->has_bg_grad_dir = 1;
         break;
     case LV_STYLE_BG_BLEND_MODE:
         _alloc_ext(style);
         style->ext->bg_blend_mode = value._int;
         style->ext->has_bg_blend_mode = 1;
         break;
     case LV_STYLE_BG_MAIN_STOP:
         _alloc_ext(style);
         style->ext->bg_main_stop = value._int;
         style->ext->has_bg_main_stop = 1;
         break;
     case LV_STYLE_BG_GRAD_STOP:
         _alloc_ext(style);
         style->ext->bg_grad_stop = value._int;
         style->ext->has_bg_grad_stop = 1;
         break;

     case LV_STYLE_BORDER_COLOR:
         style->border_color = value._color;
         style->has_border_color = 1;
         break;
     case LV_STYLE_BORDER_OPA:
         style->border_opa = value._int;
         style->has_border_opa = 1;
         break;
     case LV_STYLE_BORDER_WIDTH:
         style->border_width = value._int;
         style->has_border_width = 1;
         break;
     case LV_STYLE_BORDER_SIDE:
         style->border_side = value._int;
         style->has_border_side = 1;
         break;
     case LV_STYLE_BORDER_POST:
         style->border_post = value._int;
         style->has_border_post = 1;
         break;
     case LV_STYLE_BORDER_BLEND_MODE:
         _alloc_ext(style);
         style->ext->border_blend_mode = value._int;
         style->ext->has_border_blend_mode = 1;
         break;

     case LV_STYLE_TEXT_COLOR:
         style->text_color = value._color;
         style->has_text_color = 1;
         break;
     case LV_STYLE_TEXT_OPA:
         style->text_opa = value._int;
         style->has_text_opa = 1;
         break;
     case LV_STYLE_TEXT_LETTER_SPACE:
         _alloc_ext(style);
         style->ext->text_letter_space = value._int;
         style->ext->text_letter_space = 1;
         break;
     case LV_STYLE_TEXT_LINE_SPACE:
         _alloc_ext(style);
         style->ext->text_line_space = value._int;
         style->ext->has_text_line_space = 1;
         break;
     case LV_STYLE_TEXT_DECOR:
         _alloc_ext(style);
         style->ext->text_decor = value._int;
         style->ext->has_text_decor = 1;
         break;
     case LV_STYLE_TEXT_BLEND_MODE:
         _alloc_ext(style);
         style->ext->text_blend_mode = value._int;
         style->ext->has_text_blend_mode = 1;
         break;

     case LV_STYLE_IMG_OPA:
         style->img_opa = value._int;
         style->has_img_opa = 1;
         break;
     case LV_STYLE_IMG_BLEND_MODE:
         _alloc_ext(style);
         style->ext->img_blend_mode = value._int;
         style->ext->has_img_blend_mode = 1;
         break;
     case LV_STYLE_IMG_RECOLOR:
         _alloc_ext(style);
         style->ext->img_recolor = value._color;
         style->ext->has_img_recolor = 1;
         break;
     case LV_STYLE_IMG_RECOLOR_OPA:
         _alloc_ext(style);
         style->ext->img_recolor_opa = value._int;
         style->ext->has_img_recolor_opa= 1;
         break;

     case LV_STYLE_OUTLINE_WIDTH:
         _alloc_ext(style);
         style->ext->outline_width = value._int;
         style->ext->has_outline_width = 1;
         break;
     case LV_STYLE_OUTLINE_COLOR:
         _alloc_ext(style);
         style->ext->outline_color = value._color;
         style->ext->has_outline_color = 1;
         break;
     case LV_STYLE_OUTLINE_OPA:
         _alloc_ext(style);
         style->ext->outline_opa = value._int;
         style->ext->has_outline_opa = 1;
         break;
     case LV_STYLE_OUTLINE_PAD:
         _alloc_ext(style);
         style->ext->outline_pad = value._int;
         style->ext->has_outline_pad = 1;
         break;
     case LV_STYLE_OUTLINE_BLEND_MODE:
         _alloc_ext(style);
         style->ext->outline_blend_mode = value._int;
         style->ext->has_outline_blend_mode = 1;
         break;

     case LV_STYLE_SHADOW_WIDTH:
         _alloc_ext(style);
         style->ext->shadow_width = value._int;
         style->ext->has_shadow_width = 1;
         break;
     case LV_STYLE_SHADOW_OFS_X:
         _alloc_ext(style);
         style->ext->shadow_ofs_x = value._int;
         style->ext->has_shadow_ofs_x = 1;
         break;
     case LV_STYLE_SHADOW_OFS_Y:
         _alloc_ext(style);
         style->ext->shadow_ofs_y = value._int;
         style->ext->has_shadow_ofs_y = 1;
         break;
     case LV_STYLE_SHADOW_SPREAD:
         _alloc_ext(style);
         style->ext->shadow_spread = value._int;
         style->ext->has_shadow_spread = 1;
         break;
     case LV_STYLE_SHADOW_BLEND_MODE:
         _alloc_ext(style);
         style->ext->shadow_blend_mode = value._int;
         style->ext->has_shadow_blend_mode = 1;
         break;
     case LV_STYLE_SHADOW_COLOR:
         _alloc_ext(style);
         style->ext->shadow_color = value._color;
         style->ext->has_shadow_color = 1;
         break;
     case LV_STYLE_SHADOW_OPA:
         _alloc_ext(style);
         style->ext->shadow_opa = value._int;
         style->ext->has_shadow_opa = 1;
         break;

     case LV_STYLE_LINE_WIDTH:
         _alloc_ext(style);
         style->ext->line_width = value._int;
         style->ext->has_line_width = 1;
         break;
     case LV_STYLE_LINE_BLEND_MODE:
         _alloc_ext(style);
         style->ext->line_blend_mode = value._int;
         style->ext->has_line_blend_mode = 1;
         break;
     case LV_STYLE_LINE_DASH_WIDTH:
         _alloc_ext(style);
         style->ext->line_dash_width = value._int;
         style->ext->has_line_dash_width = 1;
         break;
     case LV_STYLE_LINE_DASH_GAP:
         _alloc_ext(style);
         style->ext->line_dash_gap = value._int;
         style->ext->has_line_dash_gap = 1;
         break;
     case LV_STYLE_LINE_ROUNDED:
         _alloc_ext(style);
         style->ext->line_rounded = value._int;
         style->ext->has_line_rounded = 1;
         break;
     case LV_STYLE_LINE_COLOR:
         _alloc_ext(style);
         style->ext->line_color = value._color;
         style->ext->has_line_color = 1;
         break;
     case LV_STYLE_LINE_OPA:
         _alloc_ext(style);
         style->ext->line_color = value._color;
         style->ext->has_line_opa = 1;
         break;

     case LV_STYLE_CONTENT_SRC:
         _alloc_ext(style);
         style->ext->content_text = value._ptr;
         style->ext->has_content_src = 1;
         break;
     case LV_STYLE_CONTENT_ALIGN:
         _alloc_ext(style);
         style->ext->content_text = value._ptr;
         style->ext->has_content_align = 1;
         break;
     case LV_STYLE_CONTENT_OFS_X:
         _alloc_ext(style);
         style->ext->content_ofs_x = value._int;
         style->ext->has_content_ofs_x = 1;
         break;
     case LV_STYLE_CONTENT_OFS_Y:
         _alloc_ext(style);
         style->ext->content_ofs_y = value._int;
         style->ext->has_content_ofs_y = 1;
         break;

     case LV_STYLE_TRANSITION_TIME:
         _alloc_ext(style);
         style->ext->transition_time = value._int;
         style->ext->has_transition_time = 1;
         break;
     case LV_STYLE_TRANSITION_DELAY:
         _alloc_ext(style);
         style->ext->transition_delay = value._int;
         style->ext->has_transition_delay = 1;
         break;
     case LV_STYLE_TRANSITION_PATH:
         _alloc_ext(style);
         style->ext->transition_path = value._ptr;
         style->ext->has_transition_path = 1;
         break;
     case LV_STYLE_TRANSITION_PROP_1:
         _alloc_ext(style);
         style->ext->transition_prop_1 = value._int;
         style->ext->has_transition_prop_1 = 1;
         break;
     case LV_STYLE_TRANSITION_PROP_2:
         _alloc_ext(style);
         style->ext->transition_prop_2 = value._int;
         style->ext->has_transition_prop_2 = 1;
         break;
     case LV_STYLE_TRANSITION_PROP_3:
         _alloc_ext(style);
         style->ext->transition_prop_3 = value._int;
         style->ext->has_transition_prop_3 = 1;
         break;
     case LV_STYLE_TRANSITION_PROP_4:
         _alloc_ext(style);
         style->ext->transition_prop_4 = value._int;
         style->ext->has_transition_prop_4 = 1;
         break;
     case LV_STYLE_TRANSITION_PROP_5:
         _alloc_ext(style);
         style->ext->transition_prop_5 = value._int;
         style->ext->has_transition_prop_5 = 1;
         break;
     case LV_STYLE_TRANSITION_PROP_6:
         _alloc_ext(style);
         style->ext->transition_prop_6 = value._int;
         style->ext->has_transition_prop_6 = 1;
         break;
     default:
         break;
     }
}


static bool get_prop(const lv_style_t * style, lv_style_prop_t prop, lv_style_value_t * value)
{
    switch(prop) {
      case LV_STYLE_RADIUS:
          if(style->has_radius) { value->_int = style->radius; return true; }
          break;
      case LV_STYLE_CLIP_CORNER:
          if(style->ext && style->ext->has_clip_corner) { value->_int = style->ext->clip_corner; return true; }
          break;
      case LV_STYLE_TRANSFORM_WIDTH:
          if(style->ext && style->ext->has_transform_width) { value->_int = style->ext->transform_width; return true; }
          break;
      case LV_STYLE_TRANSFORM_HEIGHT:
          if(style->ext && style->ext->has_transform_height) { value->_int = style->ext->transform_height; return true; }
          break;
      case LV_STYLE_TRANSFORM_ZOOM:
          if(style->ext && style->ext->has_transform_zoom) { value->_int = style->ext->transform_zoom; return true; }
          break;
      case LV_STYLE_TRANSFORM_ANGLE:
          if(style->ext && style->ext->has_transform_angle) { value->_int = style->ext->transform_angle; return true; }
          break;
      case LV_STYLE_OPA:
          if(style->ext && style->ext->has_opa) { value->_int = style->ext->opa; return true; }
          break;

      case LV_STYLE_PAD_TOP:
          if(style->has_pad_top) { value->_int = style->pad_top; return true; }
          break;
      case LV_STYLE_PAD_BOTTOM:
          if(style->has_pad_bottom) { value->_int = style->pad_bottom; return true; }
          break;
      case LV_STYLE_PAD_LEFT:
          if(style->has_pad_left) { value->_int = style->pad_left; return true; }
          break;
      case LV_STYLE_PAD_RIGHT:
          if(style->has_pad_right) { value->_int = style->pad_right; return true; }
          break;
      case LV_STYLE_MARGIN_TOP:
          if(style->ext && style->ext->has_margin_top) { value->_int = style->ext->margin_top; return true; }
          break;
      case LV_STYLE_MARGIN_BOTTOM:
          if(style->ext && style->ext->has_margin_bottom) { value->_int = style->ext->margin_left; return true; }
          break;
      case LV_STYLE_MARGIN_LEFT:
          if(style->ext && style->ext->has_margin_left) { value->_int = style->ext->margin_left; return true; }
          break;
      case LV_STYLE_MARGIN_RIGHT:
          if(style->ext && style->ext->has_margin_right) { value->_int = style->ext->margin_right; return true; }
          break;

      case LV_STYLE_BG_COLOR:
          if(style->has_bg_color) { value->_color = style->bg_color; return true; }
          break;
      case LV_STYLE_BG_OPA:
          if(style->has_bg_opa) { value->_int = style->bg_opa; return true; }
          break;
      case LV_STYLE_BG_GRAD_COLOR:
          if(style->ext && style->ext->has_bg_grad_color) { value->_color = style->ext->bg_grad_color; return true; }
          break;
      case LV_STYLE_BG_GRAD_DIR:
          if(style->ext && style->ext->has_bg_grad_dir) { value->_int = style->ext->bg_grad_dir; return true; }
          break;
      case LV_STYLE_BG_BLEND_MODE:
          if(style->ext && style->ext->has_bg_blend_mode) { value->_int = style->ext->bg_blend_mode; return true; }
          break;
      case LV_STYLE_BG_MAIN_STOP:
          if(style->ext && style->ext->has_bg_main_stop) { value->_int = style->ext->bg_main_stop; return true; }
          break;
      case LV_STYLE_BG_GRAD_STOP:
          if(style->ext && style->ext->has_bg_grad_stop) { value->_int = style->ext->bg_grad_stop; return true; }
          break;

      case LV_STYLE_BORDER_COLOR:
          if(style->has_border_color) { value->_color = style->border_color; return true; }
          break;
      case LV_STYLE_BORDER_OPA:
          if(style->has_border_opa) { value->_int = style->border_opa; return true; }
          break;
      case LV_STYLE_BORDER_WIDTH:
          if(style->has_border_width) { value->_int = style->border_width; return true; }
          break;
      case LV_STYLE_BORDER_SIDE:
          if(style->has_border_side) { value->_int = style->border_side; return true; }
          break;
      case LV_STYLE_BORDER_POST:
          if(style->has_border_post) { value->_int = style->border_post; return true; }
          break;
      case LV_STYLE_BORDER_BLEND_MODE:
          if(style->ext && style->ext->has_border_blend_mode) { value->_int = style->ext->border_blend_mode; return true; }
          break;

      case LV_STYLE_TEXT_COLOR:
          if(style->has_text_color) { value->_color = style->text_color; return true; }
          break;
      case LV_STYLE_TEXT_OPA:
          if(style->has_text_opa) { value->_int = style->text_opa; return true; }
          break;
      case LV_STYLE_TEXT_FONT:
          if(style->has_text_font) { value->_ptr = style->text_font; return true; }
          break;
      case LV_STYLE_TEXT_LETTER_SPACE:
          if(style->ext && style->ext->has_text_letter_space) { value->_int = style->ext->text_letter_space; return true; }
          break;
      case LV_STYLE_TEXT_LINE_SPACE:
          if(style->ext && style->ext->has_text_letter_space) { value->_int = style->ext->text_line_space; return true; }
          break;
      case LV_STYLE_TEXT_DECOR:
          if(style->ext && style->ext->has_text_letter_space) { value->_int = style->ext->text_decor; return true; }
          break;
      case LV_STYLE_TEXT_BLEND_MODE:
          if(style->ext && style->ext->has_text_blend_mode) { value->_int = style->ext->text_blend_mode; return true; }
          break;

      case LV_STYLE_IMG_OPA:
          if(style->has_img_opa) { value->_int = style->img_opa; return true; }
          break;
      case LV_STYLE_IMG_BLEND_MODE:
          if(style->ext && style->ext->has_img_blend_mode) { value->_int = style->ext->img_blend_mode; return true; }
          break;
      case LV_STYLE_IMG_RECOLOR:
          if(style->ext && style->ext->has_img_recolor) { value->_color = style->ext->img_recolor; return true; }
          break;
      case LV_STYLE_IMG_RECOLOR_OPA:
          if(style->ext && style->ext->has_img_recolor_opa) { value->_int = style->ext->img_recolor_opa; return true; }
          break;


      case LV_STYLE_OUTLINE_WIDTH:
          if(style->ext && style->ext->has_outline_width) { value->_int = style->ext->outline_width; return true; }
          break;
      case LV_STYLE_OUTLINE_COLOR:
          if(style->ext && style->ext->has_outline_color) { value->_color = style->ext->outline_color; return true; }
          break;
      case LV_STYLE_OUTLINE_OPA:
          if(style->ext && style->ext->has_outline_opa) { value->_int = style->ext->outline_opa; return true; }
          break;
      case LV_STYLE_OUTLINE_PAD:
          if(style->ext && style->ext->has_outline_pad) { value->_int = style->ext->outline_pad; return true; }
          break;
      case LV_STYLE_OUTLINE_BLEND_MODE:
          if(style->ext && style->ext->has_outline_blend_mode) { value->_int = style->ext->outline_blend_mode; return true; }
          break;

      case LV_STYLE_SHADOW_WIDTH:
          if(style->ext && style->ext->has_shadow_width) { value->_int = style->ext->shadow_width; return true; }
          break;
      case LV_STYLE_SHADOW_OFS_X:
          if(style->ext && style->ext->has_shadow_ofs_x) { value->_int = style->ext->shadow_ofs_x; return true; }
          break;
      case LV_STYLE_SHADOW_OFS_Y:
          if(style->ext && style->ext->has_shadow_ofs_y) { value->_int = style->ext->shadow_ofs_y; return true; }
          break;
      case LV_STYLE_SHADOW_SPREAD:
          if(style->ext && style->ext->has_shadow_spread) { value->_int = style->ext->shadow_spread; return true; }
          break;
      case LV_STYLE_SHADOW_BLEND_MODE:
          if(style->ext && style->ext->has_shadow_blend_mode) { value->_int = style->ext->shadow_blend_mode; return true; }
          break;
      case LV_STYLE_SHADOW_COLOR:
          if(style->ext && style->ext->has_shadow_color) { value->_color = style->ext->shadow_color; return true; }
          break;
      case LV_STYLE_SHADOW_OPA:
          if(style->ext && style->ext->has_shadow_opa) { value->_int = style->ext->shadow_opa; return true; }
          break;

      case LV_STYLE_LINE_WIDTH:
          if(style->ext && style->ext->has_line_width) { value->_int = style->ext->line_width; return true; }
          break;
      case LV_STYLE_LINE_BLEND_MODE:
          if(style->ext && style->ext->has_line_blend_mode) { value->_int = style->ext->line_blend_mode; return true; }
          break;
      case LV_STYLE_LINE_DASH_GAP:
          if(style->ext && style->ext->has_line_dash_gap) { value->_int = style->ext->line_dash_gap; return true; }
          break;
      case LV_STYLE_LINE_DASH_WIDTH:
          if(style->ext && style->ext->has_line_dash_width) { value->_int = style->ext->line_dash_width; return true; }
          break;
      case LV_STYLE_LINE_ROUNDED:
          if(style->ext && style->ext->has_line_rounded) { value->_int = style->ext->line_rounded; return true; }
          break;
      case LV_STYLE_LINE_COLOR:
          if(style->ext && style->ext->has_line_color) { value->_color = style->ext->line_color; return true; }
          break;
      case LV_STYLE_LINE_OPA:
          if(style->ext && style->ext->has_line_opa) { value->_int = style->ext->line_opa; return true; }
          break;

      case LV_STYLE_CONTENT_SRC:
             if(style->ext && style->ext->has_content_src) { value->_ptr = style->ext->content_text; return true; }
             break;
      case LV_STYLE_CONTENT_ALIGN:
             if(style->ext && style->ext->has_content_align) { value->_int = style->ext->content_align; return true; }
             break;
      case LV_STYLE_CONTENT_OFS_X:
             if(style->ext && style->ext->has_content_ofs_x) { value->_int = style->ext->content_ofs_x; return true; }
             break;
      case LV_STYLE_CONTENT_OFS_Y:
             if(style->ext && style->ext->has_content_ofs_y) { value->_int = style->ext->content_ofs_x; return true; }
             break;

      case LV_STYLE_TRANSITION_TIME:
             if(style->ext && style->ext->has_transition_time) { value->_int = style->ext->transition_time; return true; }
             break;
      case LV_STYLE_TRANSITION_DELAY:
             if(style->ext && style->ext->has_transition_delay) { value->_int = style->ext->transition_delay; return true; }
             break;
      case LV_STYLE_TRANSITION_PATH:
             if(style->ext && style->ext->has_transition_path) { value->_ptr = style->ext->transition_path; return true; }
             break;
      case LV_STYLE_TRANSITION_PROP_1:
             if(style->ext && style->ext->has_transition_prop_1) { value->_int = style->ext->transition_prop_1; return true; }
             break;
      case LV_STYLE_TRANSITION_PROP_2:
             if(style->ext && style->ext->has_transition_prop_2) { value->_int = style->ext->transition_prop_2; return true; }
             break;
      case LV_STYLE_TRANSITION_PROP_3:
             if(style->ext && style->ext->has_transition_prop_3) { value->_int = style->ext->transition_prop_3; return true; }
             break;
      case LV_STYLE_TRANSITION_PROP_4:
             if(style->ext && style->ext->has_transition_prop_4) { value->_int = style->ext->transition_prop_4; return true; }
             break;
      case LV_STYLE_TRANSITION_PROP_5:
             if(style->ext && style->ext->has_transition_prop_5) { value->_int = style->ext->transition_prop_5; return true; }
             break;
      case LV_STYLE_TRANSITION_PROP_6:
             if(style->ext && style->ext->has_transition_prop_6) { value->_int = style->ext->transition_prop_6; return true; }
             break;
      default:
          break;
      }

      return false;
}

static bool remove_prop(lv_style_t * style, lv_style_prop_t prop)
{
    if(style == NULL) return false;
    LV_ASSERT_STYLE(style);

    switch(prop) {

    case LV_STYLE_RADIUS:
        style->has_radius = 0;
        break;
    case LV_STYLE_CLIP_CORNER:
        if(style->ext) style->ext->has_clip_corner = 0;
        break;
    case LV_STYLE_TRANSFORM_WIDTH:
        if(style->ext) style->ext->has_transform_width = 0;
        break;
    case LV_STYLE_TRANSFORM_HEIGHT:
        if(style->ext) style->ext->has_transform_height = 0;
        break;
    case LV_STYLE_TRANSFORM_ZOOM:
        if(style->ext) style->ext->has_transform_zoom = 0;
        break;
    case LV_STYLE_TRANSFORM_ANGLE:
        if(style->ext) style->ext->has_transform_angle = 0;
        break;

    case LV_STYLE_OPA:
        if(style->ext) style->ext->has_opa = 0;
        break;
    case LV_STYLE_PAD_TOP:
        style->has_pad_top = 0;
        break;
    case LV_STYLE_PAD_BOTTOM:
        style->has_pad_bottom = 0;
        break;
    case LV_STYLE_PAD_LEFT:
        style->has_pad_left = 0;
        break;
    case LV_STYLE_PAD_RIGHT:
        style->has_pad_right = 0;
        break;
    case LV_STYLE_MARGIN_TOP:
        if(style->ext) style->ext->has_margin_top = 0;
        break;
    case LV_STYLE_MARGIN_BOTTOM:
        if(style->ext) style->ext->has_margin_bottom = 0;
        break;
    case LV_STYLE_MARGIN_LEFT:
        if(style->ext) style->ext->has_margin_left = 0;
        break;
    case LV_STYLE_MARGIN_RIGHT:
        if(style->ext) style->ext->has_margin_right = 0;
        break;

    case LV_STYLE_BG_COLOR:
        style->has_bg_color = 0;
        break;
    case LV_STYLE_BG_OPA:
        style->has_bg_opa = 0;
        break;
    case LV_STYLE_BG_GRAD_COLOR:
        if(style->ext) style->ext->has_bg_grad_color = 0;
        break;
    case LV_STYLE_BG_GRAD_DIR:
        if(style->ext) style->ext->has_bg_grad_dir = 0;
        break;
    case LV_STYLE_BG_BLEND_MODE:
        if(style->ext) style->ext->has_bg_blend_mode = 0;
        break;
    case LV_STYLE_BG_MAIN_STOP:
        if(style->ext) style->ext->has_bg_main_stop = 0;
        break;
    case LV_STYLE_BG_GRAD_STOP:
        if(style->ext) style->ext->has_bg_grad_stop = 0;
        break;

    case LV_STYLE_BORDER_COLOR:
        style->has_border_color = 0;
        break;
    case LV_STYLE_BORDER_OPA:
        style->has_border_opa = 0;
        break;
    case LV_STYLE_BORDER_WIDTH:
        style->has_border_width = 0;
        break;
    case LV_STYLE_BORDER_SIDE:
        style->has_border_side = 0;
        break;
    case LV_STYLE_BORDER_POST:
        style->has_border_post = 0;
        break;
    case LV_STYLE_BORDER_BLEND_MODE:
        if(style->ext) style->ext->has_border_blend_mode = 0;
        break;

    case LV_STYLE_TEXT_COLOR:
        style->has_text_color = 0;
        break;
    case LV_STYLE_TEXT_OPA:
        style->has_text_opa = 0;
        break;
    case LV_STYLE_TEXT_FONT:
        style->has_text_font = 0;
        break;
    case LV_STYLE_TEXT_LETTER_SPACE:
        if(style->ext) style->ext->has_text_letter_space = 0;
        break;
    case LV_STYLE_TEXT_LINE_SPACE:
        if(style->ext) style->ext->has_text_line_space = 0;
        break;
    case LV_STYLE_TEXT_DECOR:
        if(style->ext) style->ext->has_text_decor = 0;
        break;
    case LV_STYLE_TEXT_BLEND_MODE:
        if(style->ext) style->ext->has_text_blend_mode = 0;
        break;

    case LV_STYLE_IMG_OPA:
        style->has_img_opa = 0;
        break;
    case LV_STYLE_IMG_BLEND_MODE:
        style->ext->has_img_blend_mode = 0;
        break;
    case LV_STYLE_IMG_RECOLOR:
        if(style->ext) style->ext->has_img_recolor = 0;
        break;
    case LV_STYLE_IMG_RECOLOR_OPA:
        if(style->ext) style->ext->has_img_recolor_opa = 0;
        break;

    case LV_STYLE_OUTLINE_OPA:
        if(style->ext) style->ext->has_outline_opa = 0;
        break;
    case LV_STYLE_OUTLINE_COLOR:
        if(style->ext) style->ext->has_outline_color = 0;
        break;
    case LV_STYLE_OUTLINE_WIDTH:
        if(style->ext) style->ext->has_outline_width = 0;
        break;
    case LV_STYLE_OUTLINE_PAD:
        if(style->ext) style->ext->has_outline_pad = 0;
        break;
    case LV_STYLE_OUTLINE_BLEND_MODE:
        if(style->ext) style->ext->has_outline_blend_mode = 0;
        break;

    case LV_STYLE_SHADOW_WIDTH:
        if(style->ext) style->ext->has_shadow_width = 0;
        break;
    case LV_STYLE_SHADOW_OFS_X:
        if(style->ext) style->ext->has_shadow_ofs_x = 0;
        break;
    case LV_STYLE_SHADOW_OFS_Y:
        if(style->ext) style->ext->has_shadow_ofs_y = 0;
        break;
    case LV_STYLE_SHADOW_SPREAD:
        if(style->ext) style->ext->has_shadow_spread = 0;
        break;
    case LV_STYLE_SHADOW_BLEND_MODE:
        if(style->ext) style->ext->has_shadow_blend_mode = 0;
        break;
    case LV_STYLE_SHADOW_COLOR:
        if(style->ext) style->ext->has_shadow_color = 0;
        break;
    case LV_STYLE_SHADOW_OPA:
        if(style->ext) style->ext->has_shadow_opa = 0;
        break;

    case LV_STYLE_LINE_WIDTH:
        if(style->ext) style->ext->has_line_width = 0;
        break;
    case LV_STYLE_LINE_BLEND_MODE:
        if(style->ext) style->ext->has_line_blend_mode = 0;
        break;
    case LV_STYLE_LINE_DASH_GAP:
        if(style->ext) style->ext->has_line_dash_gap = 0;
        break;
    case LV_STYLE_LINE_DASH_WIDTH:
        if(style->ext) style->ext->has_line_dash_width = 0;
        break;
    case LV_STYLE_LINE_ROUNDED:
        if(style->ext) style->ext->has_line_rounded = 0;
        break;
    case LV_STYLE_LINE_COLOR:
        if(style->ext) style->ext->has_line_color = 0;
        break;
    case LV_STYLE_LINE_OPA:
        if(style->ext) style->ext->has_line_opa = 0;
        break;

    case LV_STYLE_CONTENT_ALIGN:
        if(style->ext) style->ext->has_content_align = 0;
        break;
    case LV_STYLE_CONTENT_OFS_X:
        if(style->ext) style->ext->has_content_ofs_x = 0;
        break;
    case LV_STYLE_CONTENT_OFS_Y:
        if(style->ext) style->ext->has_content_ofs_y = 0;
        break;
    case LV_STYLE_CONTENT_SRC:
        if(style->ext) style->ext->has_content_src = 0;
        break;

    case LV_STYLE_TRANSITION_TIME:
        if(style->ext) style->ext->has_transition_time = 0;
        break;
    case LV_STYLE_TRANSITION_DELAY:
        if(style->ext) style->ext->has_transition_delay = 0;
        break;
    case LV_STYLE_TRANSITION_PATH:
        if(style->ext) style->ext->has_transition_path = 0;
        break;
    case LV_STYLE_TRANSITION_PROP_1:
        if(style->ext) style->ext->has_transition_prop_1 = 0;
        break;
    case LV_STYLE_TRANSITION_PROP_2:
        if(style->ext) style->ext->has_transition_prop_2 = 0;
        break;
    case LV_STYLE_TRANSITION_PROP_3:
        if(style->ext) style->ext->has_transition_prop_3 = 0;
        break;
    case LV_STYLE_TRANSITION_PROP_4:
        if(style->ext) style->ext->has_transition_prop_4 = 0;
        break;
    case LV_STYLE_TRANSITION_PROP_5:
        if(style->ext) style->ext->has_transition_prop_5 = 0;
        break;
    case LV_STYLE_TRANSITION_PROP_6:
        if(style->ext) style->ext->has_transition_prop_6 = 0;
        break;

    default:
        return false;
    }

    return true;
}
