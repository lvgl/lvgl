/**
 * @file lv_examples_api_map.h
 * Backward-compatible aliases for pre-rework example entry points.
 *
 * The example set was reworked from numbered names (`lv_example_arc_1`) to
 * descriptive ones (`lv_example_arc_set_angles`). Code/docs still calling the
 * old names keep building: every retired `lv_example_<topic>_<N>` is aliased
 * to a valid current example from the same topic. Which specific example a
 * number resolves to is not significant.
 */

#ifndef LV_EXAMPLES_API_MAP_H
#define LV_EXAMPLES_API_MAP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

/* animimg */
#define lv_example_animimg_1       lv_example_animimg_play

/* arc */
#define lv_example_arc_1           lv_example_arc_bind_value
#define lv_example_arc_2           lv_example_arc_change_rate
#define lv_example_arc_3           lv_example_arc_event

/* arclabel */
#define lv_example_arclabel_1      lv_example_arclabel_curved_text

/* bar */
#define lv_example_bar_1           lv_example_bar_bind_value
#define lv_example_bar_2           lv_example_bar_img_indicator
#define lv_example_bar_3           lv_example_bar_modes
#define lv_example_bar_4           lv_example_bar_orientation
#define lv_example_bar_5           lv_example_bar_styling
#define lv_example_bar_6           lv_example_bar_value_range
#define lv_example_bar_7           lv_example_bar_bind_value

/* button */
#define lv_example_button_1        lv_example_button_basic
#define lv_example_button_2        lv_example_button_event
#define lv_example_button_3        lv_example_button_states

/* buttonmatrix */
#define lv_example_buttonmatrix_1  lv_example_buttonmatrix_button_ctrl
#define lv_example_buttonmatrix_2  lv_example_buttonmatrix_button_width
#define lv_example_buttonmatrix_3  lv_example_buttonmatrix_event

/* calendar */
#define lv_example_calendar_1      lv_example_calendar_basic
#define lv_example_calendar_2      lv_example_calendar_chinese

/* canvas */
#define lv_example_canvas_1        lv_example_canvas_alpha
#define lv_example_canvas_2        lv_example_canvas_arc
#define lv_example_canvas_3        lv_example_canvas_blur
#define lv_example_canvas_4        lv_example_canvas_image
#define lv_example_canvas_5        lv_example_canvas_label
#define lv_example_canvas_6        lv_example_canvas_line
#define lv_example_canvas_7        lv_example_canvas_rectangle
#define lv_example_canvas_8        lv_example_canvas_rotate
#define lv_example_canvas_9        lv_example_canvas_triangle
#define lv_example_canvas_10       lv_example_canvas_vector
#define lv_example_canvas_11       lv_example_canvas_wave_text
#define lv_example_canvas_12       lv_example_canvas_windstorm_text

/* chart */
#define lv_example_chart_1         lv_example_chart_area_gradient
#define lv_example_chart_2         lv_example_chart_circular_gap
#define lv_example_chart_3         lv_example_chart_cursor
#define lv_example_chart_4         lv_example_chart_pressed_tooltip
#define lv_example_chart_5         lv_example_chart_recolor_bars
#define lv_example_chart_6         lv_example_chart_scatter
#define lv_example_chart_7         lv_example_chart_scrollable
#define lv_example_chart_8         lv_example_chart_series

/* checkbox */
#define lv_example_checkbox_1      lv_example_checkbox_bind_checked
#define lv_example_checkbox_2      lv_example_checkbox_event

/* dropdown */
#define lv_example_dropdown_1      lv_example_dropdown_bind_value
#define lv_example_dropdown_2      lv_example_dropdown_direction
#define lv_example_dropdown_3      lv_example_dropdown_event

/* flex */
#define lv_example_flex_1          lv_example_flex_align
#define lv_example_flex_2          lv_example_flex_flow
#define lv_example_flex_3          lv_example_flex_grow
#define lv_example_flex_4          lv_example_flex_ignore_layout
#define lv_example_flex_5          lv_example_flex_internal_padding
#define lv_example_flex_6          lv_example_flex_new_track

/* gif */
#define lv_example_gif_1           lv_example_gif_src

/* grid */
#define lv_example_grid_1          lv_example_grid_align
#define lv_example_grid_2          lv_example_grid_cell_span
#define lv_example_grid_3          lv_example_grid_descriptors
#define lv_example_grid_4          lv_example_grid_fr
#define lv_example_grid_5          lv_example_grid_ignore_layout
#define lv_example_grid_6          lv_example_grid_internal_padding

/* image */
#define lv_example_image_1         lv_example_image_inner_align
#define lv_example_image_2         lv_example_image_recolor
#define lv_example_image_3         lv_example_image_src
#define lv_example_image_4         lv_example_image_svg
#define lv_example_image_5         lv_example_image_transformations

/* imagebutton */
#define lv_example_imagebutton_1   lv_example_imagebutton_transition

/* keyboard */
#define lv_example_keyboard_1      lv_example_keyboard_custom_draw
#define lv_example_keyboard_2      lv_example_keyboard_custom_map
#define lv_example_keyboard_3      lv_example_keyboard_modes

/* label */
#define lv_example_label_1         lv_example_label_bind_text
#define lv_example_label_2         lv_example_label_long_mode
#define lv_example_label_3         lv_example_label_recolor
#define lv_example_label_4         lv_example_label_set_text_newline
#define lv_example_label_5         lv_example_label_styling
#define lv_example_label_6         lv_example_label_text_align
#define lv_example_label_7         lv_example_label_translation

/* led */
#define lv_example_led_1           lv_example_led_brightness

/* line */
#define lv_example_line_1          lv_example_line_points

/* list */
#define lv_example_list_1          lv_example_list_reorder
#define lv_example_list_2          lv_example_list_sections

/* lottie */
#define lv_example_lottie_1        lv_example_lottie_from_data
#define lv_example_lottie_2        lv_example_lottie_from_file

/* menu */
#define lv_example_menu_1          lv_example_menu_custom_back_button
#define lv_example_menu_2          lv_example_menu_floating_button
#define lv_example_menu_3          lv_example_menu_root_back_button
#define lv_example_menu_4          lv_example_menu_sidebar
#define lv_example_menu_5          lv_example_menu_sub_page

/* msgbox */
#define lv_example_msgbox_1        lv_example_msgbox_blur
#define lv_example_msgbox_2        lv_example_msgbox_modal
#define lv_example_msgbox_3        lv_example_msgbox_settings

/* obj */
#define lv_example_obj_1           lv_example_obj_bind_style
#define lv_example_obj_2           lv_example_obj_draggable
#define lv_example_obj_3           lv_example_obj_transform

/* qrcode */
#define lv_example_qrcode_1        lv_example_qrcode_basic

/* roller */
#define lv_example_roller_1        lv_example_roller_bind_value
#define lv_example_roller_2        lv_example_roller_event
#define lv_example_roller_3        lv_example_roller_fade_mask

/* scale */
#define lv_example_scale_1         lv_example_scale_bind_section
#define lv_example_scale_2         lv_example_scale_compass
#define lv_example_scale_3         lv_example_scale_heart_rate
#define lv_example_scale_4         lv_example_scale_label_recolor
#define lv_example_scale_5         lv_example_scale_modes
#define lv_example_scale_6         lv_example_scale_needles
#define lv_example_scale_7         lv_example_scale_range
#define lv_example_scale_8         lv_example_scale_styling
#define lv_example_scale_9         lv_example_scale_sunset_sunrise
#define lv_example_scale_10        lv_example_scale_ticks
#define lv_example_scale_11        lv_example_scale_bind_section

/* scroll */
#define lv_example_scroll_1        lv_example_scroll_chain
#define lv_example_scroll_2        lv_example_scroll_circular
#define lv_example_scroll_3        lv_example_scroll_events
#define lv_example_scroll_4        lv_example_scroll_floating
#define lv_example_scroll_5        lv_example_scroll_infinite
#define lv_example_scroll_6        lv_example_scroll_one
#define lv_example_scroll_7        lv_example_scroll_overview
#define lv_example_scroll_8        lv_example_scroll_properties
#define lv_example_scroll_9        lv_example_scroll_rtl

/* slider */
#define lv_example_slider_1        lv_example_slider_bind_state
#define lv_example_slider_2        lv_example_slider_bind_value
#define lv_example_slider_3        lv_example_slider_event
#define lv_example_slider_4        lv_example_slider_img_indicator

/* span */
#define lv_example_span_1          lv_example_span_hittest

/* spinbox */
#define lv_example_spinbox_1       lv_example_spinbox_bind_value

/* spinner */
#define lv_example_spinner_1       lv_example_spinner_basic

/* styles */
#define lv_example_style_1         lv_example_style_arc
#define lv_example_style_2         lv_example_style_background
#define lv_example_style_3         lv_example_style_blur
#define lv_example_style_4         lv_example_style_border
#define lv_example_style_5         lv_example_style_drop_shadow
#define lv_example_style_6         lv_example_style_gradient_buttons
#define lv_example_style_7         lv_example_style_gradient_conic
#define lv_example_style_8         lv_example_style_gradient_radial
#define lv_example_style_9         lv_example_style_image
#define lv_example_style_10        lv_example_style_line
#define lv_example_style_11        lv_example_style_local
#define lv_example_style_12        lv_example_style_margin
#define lv_example_style_13        lv_example_style_modal
#define lv_example_style_14        lv_example_style_multiple
#define lv_example_style_15        lv_example_style_opacity_transform
#define lv_example_style_16        lv_example_style_outline
#define lv_example_style_17        lv_example_style_padding
#define lv_example_style_18        lv_example_style_parts_states
#define lv_example_style_19        lv_example_style_shadow
#define lv_example_style_20        lv_example_style_size_position
#define lv_example_style_21        lv_example_style_text

/* switch */
#define lv_example_switch_1        lv_example_switch_event
#define lv_example_switch_2        lv_example_switch_orientation

/* table */
#define lv_example_table_1         lv_example_table_cells
#define lv_example_table_2         lv_example_table_merge_cells

/* tabview */
#define lv_example_tabview_1       lv_example_tabview_active_tab
#define lv_example_tabview_2       lv_example_tabview_basic

/* textarea */
#define lv_example_textarea_1      lv_example_textarea_buttonmatrix
#define lv_example_textarea_2      lv_example_textarea_cursor
#define lv_example_textarea_3      lv_example_textarea_format
#define lv_example_textarea_4      lv_example_textarea_password

/* tileview */
#define lv_example_tileview_1      lv_example_tileview_l_shape

/* win */
#define lv_example_win_1           lv_example_win_toolbar

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /* LV_EXAMPLES_API_MAP_H */
