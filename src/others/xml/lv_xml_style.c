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

void lv_xml_style_register(lv_xml_component_ctx_t * ctx, const char ** attrs)
{
    const char * style_name =  lv_xml_get_value_of(attrs, "name");
    if(style_name == NULL) {
        LV_LOG_WARN("'name' is missing from a style");
        return;
    }
    lv_xml_style_t * xml_style = lv_ll_ins_tail(&ctx->style_ll);
    lv_style_t * style = &xml_style->style;
    lv_style_init(style);
    xml_style->name = lv_strdup(style_name);

    size_t long_name_len = lv_strlen(ctx->name) + 1 + lv_strlen(style_name) + 1;
    xml_style->long_name = lv_malloc(long_name_len);
    lv_snprintf((char *)xml_style->long_name, long_name_len, "%s.%s", ctx->name, style_name); /*E.g. my_button.style1*/

    for(int i = 0; attrs[i]; i += 2) {
        const char * name = attrs[i];
        const char * value = attrs[i + 1];
        if(lv_streq(name, "name")) continue;
        if(lv_streq(name, "help")) continue;

        if(value[0] == '#') {
            const char * value_clean = &value[1];
            lv_xml_const_t * c;
            LV_LL_READ(&ctx->const_ll, c) {
                if(lv_streq(c->name, value_clean)) {
                    value = c->value;
                    break;
                }
            }
        }


        SET_STYLE_IF(width, lv_xml_to_size(value));
        else SET_STYLE_IF(height, lv_xml_to_size(value));
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

        else SET_STYLE_IF(bg_image_src, lv_xml_get_image(value));
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
        else SET_STYLE_IF(text_font, lv_xml_get_font(value));
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
        else SET_STYLE_IF(arc_image_src, lv_xml_get_image(value));

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
        else SET_STYLE_IF(bitmap_mask_src, lv_xml_get_image(value));
        else SET_STYLE_IF(rotary_sensitivity, lv_xml_atoi(value));

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

void lv_xml_style_add_to_obj(lv_xml_parser_state_t * state, lv_obj_t * obj, const char * text)
{
    char * str = lv_strdup(text);
    char * str_ori = str;

    /* Split the string based on space and colons */
    char * onestyle_str = lv_xml_split_str(&str, ' ');
    while(onestyle_str != NULL) {
        /* Parse the parts and states after the space */
        lv_style_selector_t selector = 0;
        const char * style_name = lv_xml_style_string_process(onestyle_str, &selector);
        if(style_name != NULL) {
            lv_xml_style_t * xml_style = NULL;
            /*Resolve parameters or just find the style*/
            if(style_name[0] == '$') {
                /*E.g. `$pr_style` style name means use the value
                 *coming from the parent's `pr_style` named attribute*/
                const char * name_clean = &style_name[1];
                const char * parent_style_name = lv_xml_get_value_of(state->parent_attrs, name_clean);
                if(parent_style_name) {
                    xml_style = lv_xml_get_style_by_name(state->parent_ctx, parent_style_name);
                }
            }
            else {
                xml_style = lv_xml_get_style_by_name(&state->ctx, style_name);
            }
            if(xml_style) {
                /* Apply with the selector */
                lv_obj_add_style(obj, &xml_style->style, selector);
            }
        }
        onestyle_str = lv_xml_split_str(&str, ' ');
    }

    lv_free(str_ori);
}

lv_xml_style_t * lv_xml_get_style_by_name(lv_xml_component_ctx_t * ctx, const char * style_name_raw)
{
    const char * style_name = strrchr(style_name_raw, '.');

    if(style_name) {
        char component_name[256];
        size_t len = (size_t)(style_name - style_name_raw);
        lv_memcpy(component_name, style_name_raw, len);
        component_name[len] = '\0';
        ctx = lv_xml_component_get_ctx(component_name);
        style_name++; /*Skip the dot*/
    }
    else {
        style_name = style_name_raw;
    }

    lv_xml_style_t * xml_style;
    LV_LL_READ(&ctx->style_ll, xml_style) {
        if(lv_streq(xml_style->name, style_name)) return xml_style;
    }

    LV_LOG_WARN("No style found with %s name", style_name_raw);

    return NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /* LV_USE_XML */
