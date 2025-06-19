/**
 * @file lv_xml_style.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "../../lvgl.h"
#if LV_USE_XML

#include "lv_xml_base_types.h"
#include "lv_xml_parser.h"
#include "lv_xml_style.h"
#include "lv_xml_utils.h"
#include "lv_xml_component_private.h"
#include <string.h>

/*********************
 *      DEFINES
 *********************/
#ifdef _MSC_VER
    #define strtok_r strtok_s  // Use strtok_s as an equivalent to strtok_r in Visual Studio
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static lv_style_prop_t style_prop_text_to_enum(const char * txt);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/*Expands to e.g.
  if(lv_streq(name, "height")) lv_style_set_height(style, lv_xml_to_size(value));
 */
#define SET_STYLE_IF(prop, value) if(lv_streq(name, #prop)) lv_style_set_##prop(style, value)

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_state_t lv_xml_style_state_to_enum(const char * txt)
{
    if(lv_streq("default", txt)) return LV_STATE_DEFAULT;
    else if(lv_streq("pressed", txt)) return LV_STATE_PRESSED;
    else if(lv_streq("checked", txt)) return LV_STATE_CHECKED;
    else if(lv_streq("scrolled", txt)) return LV_STATE_SCROLLED;
    else if(lv_streq("focused", txt)) return LV_STATE_FOCUSED;
    else if(lv_streq("focus_key", txt)) return LV_STATE_FOCUS_KEY;
    else if(lv_streq("edited", txt)) return LV_STATE_EDITED;
    else if(lv_streq("hovered", txt)) return LV_STATE_HOVERED;
    else if(lv_streq("disabled", txt)) return LV_STATE_DISABLED;
    else if(lv_streq("user_1", txt)) return LV_STATE_USER_1;
    else if(lv_streq("user_2", txt)) return LV_STATE_USER_2;
    else if(lv_streq("user_3", txt)) return LV_STATE_USER_3;
    else if(lv_streq("user_4", txt)) return LV_STATE_USER_4;

    return 0; /*Return 0 in lack of a better option. */
}

lv_part_t lv_xml_style_part_to_enum(const char * txt)
{
    if(lv_streq("main", txt)) return LV_PART_MAIN;
    else if(lv_streq("scrollbar", txt)) return LV_PART_SCROLLBAR;
    else if(lv_streq("indicator", txt)) return LV_PART_INDICATOR;
    else if(lv_streq("knob", txt)) return LV_PART_KNOB;
    else if(lv_streq("selected", txt)) return LV_PART_SELECTED;
    else if(lv_streq("items", txt)) return LV_PART_ITEMS;
    else if(lv_streq("cursor", txt)) return LV_PART_CURSOR;

    return 0; /*Return 0 in lack of a better option. */
}

lv_result_t lv_xml_style_register(lv_xml_component_scope_t * scope, const char ** attrs)
{
    const char * style_name =  lv_xml_get_value_of(attrs, "name");
    if(style_name == NULL) {
        LV_LOG_WARN("'name' is missing from a style");
        return LV_RESULT_INVALID;
    }

    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) return LV_RESULT_INVALID;


    lv_xml_style_t * xml_style;
    /*If a style with the same name is already created, use it */
    bool found = false;
    LV_LL_READ(&scope->style_ll, xml_style) {
        if(lv_streq(xml_style->name, style_name)) {
            found = true;
            LV_LOG_INFO("Style %s is already registered. Extending it with new properties.", style_name);
            break;
        }
    }

    if(!found) {
        xml_style = lv_ll_ins_tail(&scope->style_ll);
        xml_style->name = lv_strdup(style_name);
        lv_style_init(&xml_style->style);
        size_t long_name_len = lv_strlen(scope->name) + 1 + lv_strlen(style_name) + 1;
        xml_style->long_name = lv_malloc(long_name_len);
        lv_snprintf((char *)xml_style->long_name, long_name_len, "%s.%s", scope->name, style_name); /*E.g. my_button.style1*/
    }

    lv_style_t * style = &xml_style->style;

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
        if(lv_streq(name, "name")) continue;
        if(lv_streq(name, "help")) continue;
        if(lv_streq(name, "figma_node_id")) continue;

        if(value[0] == '#') {
            const char * value_clean = &value[1];
            lv_xml_const_t * c;
            LV_LL_READ(&scope->const_ll, c) {
                if(lv_streq(c->name, value_clean)) {
                    value = c->value;
                    break;
                }
            }
        }

        if(lv_streq(value, "remove")) {
            lv_style_prop_t prop = style_prop_text_to_enum(name);
            if(prop != LV_STYLE_PROP_INV) lv_style_remove_prop(style, prop);
            else if(lv_streq(name, "pad_all")) {
                lv_style_remove_prop(style, LV_STYLE_PAD_TOP);
                lv_style_remove_prop(style, LV_STYLE_PAD_BOTTOM);
                lv_style_remove_prop(style, LV_STYLE_PAD_LEFT);
                lv_style_remove_prop(style, LV_STYLE_PAD_RIGHT);
            }
            else if(lv_streq(name, "pad_hor")) {
                lv_style_remove_prop(style, LV_STYLE_PAD_LEFT);
                lv_style_remove_prop(style, LV_STYLE_PAD_RIGHT);
            }
            else if(lv_streq(name, "pad_ver")) {
                lv_style_remove_prop(style, LV_STYLE_PAD_TOP);
                lv_style_remove_prop(style, LV_STYLE_PAD_BOTTOM);
            }
            else if(lv_streq(name, "pad_gap")) {
                lv_style_remove_prop(style, LV_STYLE_PAD_COLUMN);
                lv_style_remove_prop(style, LV_STYLE_PAD_ROW);
            }
            else if(lv_streq(name, "margin_all")) {
                lv_style_remove_prop(style, LV_STYLE_MARGIN_TOP);
                lv_style_remove_prop(style, LV_STYLE_MARGIN_BOTTOM);
                lv_style_remove_prop(style, LV_STYLE_MARGIN_LEFT);
                lv_style_remove_prop(style, LV_STYLE_MARGIN_RIGHT);
            }
            else if(lv_streq(name, "margin_hor")) {
                lv_style_remove_prop(style, LV_STYLE_MARGIN_LEFT);
                lv_style_remove_prop(style, LV_STYLE_MARGIN_RIGHT);
            }
            else if(lv_streq(name, "margin_ver")) {
                lv_style_remove_prop(style, LV_STYLE_MARGIN_TOP);
                lv_style_remove_prop(style, LV_STYLE_MARGIN_BOTTOM);
            }
        }
        else SET_STYLE_IF(width, lv_xml_to_size(value));
        else SET_STYLE_IF(min_width, lv_xml_to_size(value));
        else SET_STYLE_IF(max_width, lv_xml_to_size(value));
        else SET_STYLE_IF(height, lv_xml_to_size(value));
        else SET_STYLE_IF(min_height, lv_xml_to_size(value));
        else SET_STYLE_IF(max_height, lv_xml_to_size(value));
        else SET_STYLE_IF(length, lv_xml_to_size(value));
        else SET_STYLE_IF(radius, lv_xml_to_size(value));

        else SET_STYLE_IF(pad_left, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_right, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_top, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_bottom, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_hor, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_ver, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_all, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_row, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_column, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_gap, lv_xml_atoi(value));
        else SET_STYLE_IF(pad_radial, lv_xml_atoi(value));

        else SET_STYLE_IF(margin_left, lv_xml_atoi(value));
        else SET_STYLE_IF(margin_right, lv_xml_atoi(value));
        else SET_STYLE_IF(margin_top, lv_xml_atoi(value));
        else SET_STYLE_IF(margin_bottom, lv_xml_atoi(value));
        else SET_STYLE_IF(margin_hor, lv_xml_atoi(value));
        else SET_STYLE_IF(margin_ver, lv_xml_atoi(value));
        else SET_STYLE_IF(margin_all, lv_xml_atoi(value));

        else SET_STYLE_IF(base_dir, lv_xml_base_dir_to_enum(value));
        else SET_STYLE_IF(clip_corner, lv_xml_to_bool(value));

        else SET_STYLE_IF(bg_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(bg_color, lv_xml_to_color(value));
        else SET_STYLE_IF(bg_grad_dir, lv_xml_grad_dir_to_enum(value));
        else SET_STYLE_IF(bg_grad_color, lv_xml_to_color(value));
        else SET_STYLE_IF(bg_main_stop, lv_xml_atoi(value));
        else SET_STYLE_IF(bg_grad_stop, lv_xml_atoi(value));
        else SET_STYLE_IF(bg_grad, lv_xml_component_get_grad(scope, value));

        else SET_STYLE_IF(bg_image_src, lv_xml_get_image(scope, value));
        else SET_STYLE_IF(bg_image_tiled, lv_xml_to_bool(value));
        else SET_STYLE_IF(bg_image_recolor, lv_xml_to_color(value));
        else SET_STYLE_IF(bg_image_recolor_opa, lv_xml_to_opa(value));

        else SET_STYLE_IF(border_color, lv_xml_to_color(value));
        else SET_STYLE_IF(border_width, lv_xml_atoi(value));
        else SET_STYLE_IF(border_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(border_side, lv_xml_border_side_to_enum(value));
        else SET_STYLE_IF(border_post, lv_xml_to_bool(value));

        else SET_STYLE_IF(outline_color, lv_xml_to_color(value));
        else SET_STYLE_IF(outline_width, lv_xml_atoi(value));
        else SET_STYLE_IF(outline_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(outline_pad, lv_xml_atoi(value));

        else SET_STYLE_IF(shadow_width, lv_xml_atoi(value));
        else SET_STYLE_IF(shadow_color, lv_xml_to_color(value));
        else SET_STYLE_IF(shadow_offset_x, lv_xml_atoi(value));
        else SET_STYLE_IF(shadow_offset_y, lv_xml_atoi(value));
        else SET_STYLE_IF(shadow_spread, lv_xml_atoi(value));
        else SET_STYLE_IF(shadow_opa, lv_xml_to_opa(value));

        else SET_STYLE_IF(text_color, lv_xml_to_color(value));
        else SET_STYLE_IF(text_font, lv_xml_get_font(scope, value));
        else SET_STYLE_IF(text_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(text_align, lv_xml_text_align_to_enum(value));
        else SET_STYLE_IF(text_letter_space, lv_xml_atoi(value));
        else SET_STYLE_IF(text_line_space, lv_xml_atoi(value));
        else SET_STYLE_IF(text_decor, lv_xml_text_decor_to_enum(value));

        else SET_STYLE_IF(image_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(image_recolor, lv_xml_to_color(value));
        else SET_STYLE_IF(image_recolor_opa, lv_xml_to_opa(value));

        else SET_STYLE_IF(line_color, lv_xml_to_color(value));
        else SET_STYLE_IF(line_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(line_width, lv_xml_atoi(value));
        else SET_STYLE_IF(line_dash_width, lv_xml_atoi(value));
        else SET_STYLE_IF(line_dash_gap, lv_xml_atoi(value));
        else SET_STYLE_IF(line_rounded, lv_xml_to_bool(value));

        else SET_STYLE_IF(arc_color, lv_xml_to_color(value));
        else SET_STYLE_IF(arc_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(arc_width, lv_xml_atoi(value));
        else SET_STYLE_IF(arc_rounded, lv_xml_to_bool(value));
        else SET_STYLE_IF(arc_image_src, lv_xml_get_image(scope, value));

        else SET_STYLE_IF(opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(opa_layered, lv_xml_to_opa(value));
        else SET_STYLE_IF(color_filter_opa, lv_xml_to_opa(value));
        else SET_STYLE_IF(anim_duration, lv_xml_atoi(value));
        else SET_STYLE_IF(blend_mode, lv_xml_blend_mode_to_enum(value));
        else SET_STYLE_IF(transform_width, lv_xml_atoi(value));
        else SET_STYLE_IF(transform_height, lv_xml_atoi(value));
        else SET_STYLE_IF(translate_x, lv_xml_atoi(value));
        else SET_STYLE_IF(translate_y, lv_xml_atoi(value));
        else SET_STYLE_IF(translate_radial, lv_xml_atoi(value));
        else SET_STYLE_IF(transform_scale_x, lv_xml_atoi(value));
        else SET_STYLE_IF(transform_scale_y, lv_xml_atoi(value));
        else SET_STYLE_IF(transform_rotation, lv_xml_atoi(value));
        else SET_STYLE_IF(transform_pivot_x, lv_xml_atoi(value));
        else SET_STYLE_IF(transform_pivot_y, lv_xml_atoi(value));
        else SET_STYLE_IF(transform_skew_x, lv_xml_atoi(value));
        else SET_STYLE_IF(bitmap_mask_src, lv_xml_get_image(scope, value));
        else SET_STYLE_IF(rotary_sensitivity, lv_xml_atoi(value));
        else SET_STYLE_IF(recolor, lv_xml_to_color(value));
        else SET_STYLE_IF(recolor_opa, lv_xml_to_opa(value));

        else SET_STYLE_IF(layout, lv_xml_layout_to_enum(value));

        else SET_STYLE_IF(flex_flow, lv_xml_flex_flow_to_enum(value));
        else SET_STYLE_IF(flex_grow, lv_xml_atoi(value));
        else SET_STYLE_IF(flex_main_place, lv_xml_flex_align_to_enum(value));
        else SET_STYLE_IF(flex_cross_place, lv_xml_flex_align_to_enum(value));
        else SET_STYLE_IF(flex_track_place, lv_xml_flex_align_to_enum(value));

        else SET_STYLE_IF(grid_column_align, lv_xml_grid_align_to_enum(value));
        else SET_STYLE_IF(grid_row_align, lv_xml_grid_align_to_enum(value));
        else SET_STYLE_IF(grid_cell_column_pos, lv_xml_atoi(value));
        else SET_STYLE_IF(grid_cell_column_span, lv_xml_atoi(value));
        else SET_STYLE_IF(grid_cell_x_align, lv_xml_grid_align_to_enum(value));
        else SET_STYLE_IF(grid_cell_row_pos, lv_xml_atoi(value));
        else SET_STYLE_IF(grid_cell_row_span, lv_xml_atoi(value));
        else SET_STYLE_IF(grid_cell_y_align, lv_xml_grid_align_to_enum(value));


        else {
            LV_LOG_WARN("%s style property is not supported", name);
        }
    }

    return LV_RESULT_OK;
}

const char * lv_xml_style_string_process(char * txt, lv_style_selector_t * selector)
{
    *selector = 0;

    char * style_name = lv_xml_split_str(&txt, ':');
    char * selector_str = lv_xml_split_str(&txt, ':');
    while(selector_str != NULL) {
        /* Handle different states and parts */
        *selector |= lv_xml_style_state_to_enum(selector_str);
        *selector |= lv_xml_style_part_to_enum(selector_str);

        /* Move to the next token */
        selector_str = lv_xml_split_str(&txt, ':');
    }

    return style_name;
}

lv_xml_style_t * lv_xml_get_style_by_name(lv_xml_component_scope_t * scope, const char * style_name_raw)
{
    const char * style_name = strrchr(style_name_raw, '.');

    if(style_name) {
        char component_name[256];
        size_t len = (size_t)(style_name - style_name_raw);
        lv_memcpy(component_name, style_name_raw, len);
        component_name[len] = '\0';
        scope = lv_xml_component_get_scope(component_name);
        if(scope == NULL) {
            LV_LOG_WARN("'%s' component or widget is not found", component_name);
        }
        style_name++; /*Skip the dot*/
    }
    else {
        style_name = style_name_raw;
    }

    /*Use the global scope is not specified*/
    if(scope == NULL) scope = lv_xml_component_get_scope("globals");
    if(scope == NULL) return NULL;

    lv_xml_style_t * xml_style;
    LV_LL_READ(&scope->style_ll, xml_style) {
        if(lv_streq(xml_style->name, style_name)) return xml_style;
    }

    /*If not found in the component check the global space*/
    if(!lv_streq(scope->name, "globals")) {
        scope = lv_xml_component_get_scope("globals");
        if(scope) {
            LV_LL_READ(&scope->style_ll, xml_style) {
                if(lv_streq(xml_style->name, style_name)) return xml_style;
            }
        }
    }

    LV_LOG_WARN("No style found with %s name", style_name_raw);

    return NULL;
}

lv_grad_dsc_t * lv_xml_component_get_grad(lv_xml_component_scope_t * scope, const char * name)
{
    lv_xml_grad_t * d;
    LV_LL_READ(&scope->gradient_ll, d) {
        if(lv_streq(d->name, name)) return &d->grad_dsc;
    }

    return NULL;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_style_prop_t style_prop_text_to_enum(const char * txt)
{
    if(lv_streq(txt, "width")) return LV_STYLE_WIDTH;
    if(lv_streq(txt, "min_width")) return LV_STYLE_MIN_WIDTH;
    if(lv_streq(txt, "max_width")) return LV_STYLE_MAX_WIDTH;
    else if(lv_streq(txt, "height")) return LV_STYLE_HEIGHT;
    else if(lv_streq(txt, "min_height")) return LV_STYLE_MIN_HEIGHT;
    else if(lv_streq(txt, "max_height")) return LV_STYLE_MAX_HEIGHT;
    else if(lv_streq(txt, "length")) return LV_STYLE_LENGTH;
    else if(lv_streq(txt, "radius")) return LV_STYLE_RADIUS;

    else if(lv_streq(txt, "pad_left")) return LV_STYLE_PAD_LEFT;
    else if(lv_streq(txt, "pad_right")) return LV_STYLE_PAD_RIGHT;
    else if(lv_streq(txt, "pad_top")) return LV_STYLE_PAD_TOP;
    else if(lv_streq(txt, "pad_bottom")) return LV_STYLE_PAD_BOTTOM;
    else if(lv_streq(txt, "pad_row")) return LV_STYLE_PAD_ROW;
    else if(lv_streq(txt, "pad_column")) return LV_STYLE_PAD_COLUMN;
    else if(lv_streq(txt, "pad_radial")) return LV_STYLE_PAD_RADIAL;

    else if(lv_streq(txt, "margin_left")) return LV_STYLE_MARGIN_LEFT;
    else if(lv_streq(txt, "margin_right")) return LV_STYLE_MARGIN_RIGHT;
    else if(lv_streq(txt, "margin_top")) return LV_STYLE_MARGIN_TOP;
    else if(lv_streq(txt, "margin_bottom")) return LV_STYLE_MARGIN_BOTTOM;

    else if(lv_streq(txt, "base_dir")) return LV_STYLE_BASE_DIR;
    else if(lv_streq(txt, "clip_corner")) return LV_STYLE_CLIP_CORNER;

    else if(lv_streq(txt, "bg_opa")) return LV_STYLE_BG_OPA;
    else if(lv_streq(txt, "bg_color")) return LV_STYLE_BG_COLOR;
    else if(lv_streq(txt, "bg_grad_dir")) return LV_STYLE_BG_GRAD_DIR;
    else if(lv_streq(txt, "bg_grad_color")) return LV_STYLE_BG_GRAD_COLOR;
    else if(lv_streq(txt, "bg_main_stop")) return LV_STYLE_BG_MAIN_STOP;
    else if(lv_streq(txt, "bg_grad_stop")) return LV_STYLE_BG_GRAD_STOP;
    else if(lv_streq(txt, "bg_grad")) return LV_STYLE_BG_GRAD;

    else if(lv_streq(txt, "bg_image_src")) return LV_STYLE_BG_IMAGE_SRC;
    else if(lv_streq(txt, "bg_image_tiled")) return LV_STYLE_BG_IMAGE_TILED;
    else if(lv_streq(txt, "bg_image_recolor")) return LV_STYLE_BG_IMAGE_RECOLOR;
    else if(lv_streq(txt, "bg_image_recolor_opa")) return LV_STYLE_BG_IMAGE_RECOLOR_OPA;

    else if(lv_streq(txt, "border_color")) return LV_STYLE_BORDER_COLOR;
    else if(lv_streq(txt, "border_width")) return LV_STYLE_BORDER_WIDTH;
    else if(lv_streq(txt, "border_opa")) return LV_STYLE_BORDER_OPA;
    else if(lv_streq(txt, "border_side")) return LV_STYLE_BORDER_SIDE;
    else if(lv_streq(txt, "border_post")) return LV_STYLE_BORDER_POST;

    else if(lv_streq(txt, "outline_color")) return LV_STYLE_OUTLINE_COLOR;
    else if(lv_streq(txt, "outline_width")) return LV_STYLE_OUTLINE_WIDTH;
    else if(lv_streq(txt, "outline_opa")) return LV_STYLE_OUTLINE_OPA;
    else if(lv_streq(txt, "outline_pad")) return LV_STYLE_OUTLINE_PAD;

    else if(lv_streq(txt, "shadow_width")) return LV_STYLE_SHADOW_WIDTH;
    else if(lv_streq(txt, "shadow_color")) return LV_STYLE_SHADOW_COLOR;
    else if(lv_streq(txt, "shadow_offset_x")) return LV_STYLE_SHADOW_OFFSET_X;
    else if(lv_streq(txt, "shadow_offset_y")) return LV_STYLE_SHADOW_OFFSET_Y;
    else if(lv_streq(txt, "shadow_spread")) return LV_STYLE_SHADOW_SPREAD;
    else if(lv_streq(txt, "shadow_opa")) return LV_STYLE_SHADOW_OPA;

    else if(lv_streq(txt, "text_color")) return LV_STYLE_TEXT_COLOR;
    else if(lv_streq(txt, "text_font")) return LV_STYLE_TEXT_FONT;
    else if(lv_streq(txt, "text_opa")) return LV_STYLE_TEXT_OPA;
    else if(lv_streq(txt, "text_align")) return LV_STYLE_TEXT_ALIGN;
    else if(lv_streq(txt, "text_letter_space")) return LV_STYLE_TEXT_LETTER_SPACE;
    else if(lv_streq(txt, "text_line_space")) return LV_STYLE_TEXT_LINE_SPACE;
    else if(lv_streq(txt, "text_decor")) return LV_STYLE_TEXT_DECOR;

    else if(lv_streq(txt, "image_opa")) return LV_STYLE_IMAGE_OPA;
    else if(lv_streq(txt, "image_recolor")) return LV_STYLE_IMAGE_RECOLOR;
    else if(lv_streq(txt, "image_recolor_opa")) return LV_STYLE_IMAGE_RECOLOR_OPA;

    else if(lv_streq(txt, "line_color")) return LV_STYLE_LINE_COLOR;
    else if(lv_streq(txt, "line_opa")) return LV_STYLE_LINE_OPA;
    else if(lv_streq(txt, "line_width")) return LV_STYLE_LINE_WIDTH;
    else if(lv_streq(txt, "line_dash_width")) return LV_STYLE_LINE_DASH_WIDTH;
    else if(lv_streq(txt, "line_dash_gap")) return LV_STYLE_LINE_DASH_GAP;
    else if(lv_streq(txt, "line_rounded")) return LV_STYLE_LINE_ROUNDED;

    else if(lv_streq(txt, "arc_color")) return LV_STYLE_ARC_COLOR;
    else if(lv_streq(txt, "arc_opa")) return LV_STYLE_ARC_OPA;
    else if(lv_streq(txt, "arc_width")) return LV_STYLE_ARC_WIDTH;
    else if(lv_streq(txt, "arc_rounded")) return LV_STYLE_ARC_ROUNDED;
    else if(lv_streq(txt, "arc_image_src")) return LV_STYLE_ARC_IMAGE_SRC;

    else if(lv_streq(txt, "opa")) return LV_STYLE_OPA;
    else if(lv_streq(txt, "opa_layered")) return LV_STYLE_OPA_LAYERED;
    else if(lv_streq(txt, "color_filter_opa")) return LV_STYLE_COLOR_FILTER_OPA;
    else if(lv_streq(txt, "anim_duration")) return LV_STYLE_ANIM_DURATION;
    else if(lv_streq(txt, "blend_mode")) return LV_STYLE_BLEND_MODE;
    else if(lv_streq(txt, "transform_width")) return LV_STYLE_TRANSFORM_WIDTH;
    else if(lv_streq(txt, "transform_height")) return LV_STYLE_TRANSFORM_HEIGHT;
    else if(lv_streq(txt, "translate_x")) return LV_STYLE_TRANSLATE_X;
    else if(lv_streq(txt, "translate_y")) return LV_STYLE_TRANSLATE_Y;
    else if(lv_streq(txt, "translate_radial")) return LV_STYLE_TRANSLATE_RADIAL;
    else if(lv_streq(txt, "transform_scale_x")) return LV_STYLE_TRANSFORM_SCALE_X;
    else if(lv_streq(txt, "transform_scale_y")) return LV_STYLE_TRANSFORM_SCALE_Y;
    else if(lv_streq(txt, "transform_rotation")) return LV_STYLE_TRANSFORM_ROTATION;
    else if(lv_streq(txt, "transform_pivot_x")) return LV_STYLE_TRANSFORM_PIVOT_X;
    else if(lv_streq(txt, "transform_pivot_y")) return LV_STYLE_TRANSFORM_PIVOT_Y;
    else if(lv_streq(txt, "transform_skew_x")) return LV_STYLE_TRANSFORM_SKEW_X;
    else if(lv_streq(txt, "bitmap_mask_src")) return LV_STYLE_BITMAP_MASK_SRC;
    else if(lv_streq(txt, "rotary_sensitivity")) return LV_STYLE_ROTARY_SENSITIVITY;
    else if(lv_streq(txt, "recolor")) return LV_STYLE_RECOLOR;
    else if(lv_streq(txt, "recolor_opa")) return LV_STYLE_RECOLOR_OPA;

    else if(lv_streq(txt, "layout")) return LV_STYLE_LAYOUT;

    else if(lv_streq(txt, "flex_flow")) return LV_STYLE_FLEX_FLOW;
    else if(lv_streq(txt, "flex_grow")) return LV_STYLE_FLEX_GROW;
    else if(lv_streq(txt, "flex_main_place")) return LV_STYLE_FLEX_MAIN_PLACE;
    else if(lv_streq(txt, "flex_cross_place")) return LV_STYLE_FLEX_CROSS_PLACE;
    else if(lv_streq(txt, "flex_track_place")) return LV_STYLE_FLEX_TRACK_PLACE;

    else if(lv_streq(txt, "grid_column_align")) return LV_STYLE_GRID_COLUMN_ALIGN;
    else if(lv_streq(txt, "grid_row_align")) return LV_STYLE_GRID_ROW_ALIGN;
    else if(lv_streq(txt, "grid_cell_column_pos")) return LV_STYLE_GRID_CELL_COLUMN_POS;
    else if(lv_streq(txt, "grid_cell_column_span")) return LV_STYLE_GRID_CELL_COLUMN_SPAN;
    else if(lv_streq(txt, "grid_cell_x_align")) return LV_STYLE_GRID_CELL_X_ALIGN;
    else if(lv_streq(txt, "grid_cell_row_pos")) return LV_STYLE_GRID_CELL_ROW_POS;
    else if(lv_streq(txt, "grid_cell_row_span")) return LV_STYLE_GRID_CELL_ROW_SPAN;
    else if(lv_streq(txt, "grid_cell_y_align")) return LV_STYLE_GRID_CELL_Y_ALIGN;

    return LV_STYLE_PROP_INV;

}

#endif /* LV_USE_XML */
