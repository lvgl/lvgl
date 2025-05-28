/**
 * @file lv_types.h
 *
 */

#ifndef LV_TYPES_H
#define LV_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_conf_internal.h"

#ifndef __ASSEMBLY__
#include LV_STDINT_INCLUDE
#include LV_STDDEF_INCLUDE
#include LV_STDBOOL_INCLUDE
#include LV_INTTYPES_INCLUDE
#include LV_LIMITS_INCLUDE
#include LV_STDARG_INCLUDE
#endif

/*********************
 *      DEFINES
 *********************/

/*If __UINTPTR_MAX__ or UINTPTR_MAX are available, use them to determine arch size*/
#if defined(__UINTPTR_MAX__) && __UINTPTR_MAX__ > 0xFFFFFFFF
#define LV_ARCH_64

#elif defined(UINTPTR_MAX) && UINTPTR_MAX > 0xFFFFFFFF
#define LV_ARCH_64

/*Otherwise use compiler-dependent means to determine arch size*/
#elif defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined (__aarch64__)
#define LV_ARCH_64

#endif

#if LV_USE_3DTEXTURE
#if LV_USE_OPENGLES
#define LV_3DTEXTURE_ID_NULL 0u
#endif

#ifndef LV_3DTEXTURE_ID_NULL
#error enable LV_USE_OPENGLES to use LV_USE_3DTEXTURE
#endif
#endif /*LV_USE_3DTEXTURE*/

/**********************
 *      TYPEDEFS
 **********************/

/* Exclude C enum and struct definitions when included by assembly code */
#ifndef __ASSEMBLY__

/**
 * LVGL error codes.
 */
typedef enum {
    LV_RESULT_INVALID = 0, /*Typically indicates that the object is deleted (become invalid) in the action
                      function or an operation was failed*/
    LV_RESULT_OK,      /*The object is valid (no deleted) after the action*/
} lv_result_t;

#if defined(__cplusplus) || __STDC_VERSION__ >= 199901L
/*If c99 or newer,  use the definition of uintptr_t directly from <stdint.h>*/
typedef uintptr_t lv_uintptr_t;
typedef intptr_t lv_intptr_t;

#else

/*Otherwise, use the arch size determination*/
#ifdef LV_ARCH_64
typedef uint64_t lv_uintptr_t;
typedef int64_t lv_intptr_t;
#else
typedef uint32_t lv_uintptr_t;
typedef int32_t lv_intptr_t;
#endif

#endif

#if LV_USE_FLOAT
typedef float lv_value_precise_t;
#else
typedef int32_t lv_value_precise_t;
#endif

#if LV_USE_3DTEXTURE
#if LV_USE_OPENGLES
typedef unsigned int lv_3dtexture_id_t;
#endif
#endif

/**
 * Typedefs from various lvgl modules.
 * They are defined here to avoid circular dependencies.
 */

typedef struct _lv_obj_t lv_obj_t;

typedef lv_obj_t * (*lv_screen_create_cb_t)(void);

typedef uint16_t lv_state_t;
typedef uint32_t lv_part_t;

typedef uint8_t lv_opa_t;

typedef uint8_t lv_style_prop_t;

typedef struct _lv_obj_class_t lv_obj_class_t;

typedef struct _lv_group_t lv_group_t;

typedef struct _lv_display_t lv_display_t;

typedef struct _lv_layer_t lv_layer_t;
typedef struct _lv_draw_unit_t lv_draw_unit_t;
typedef struct _lv_draw_task_t lv_draw_task_t;

typedef struct _lv_indev_t lv_indev_t;

typedef struct _lv_event_t lv_event_t;

typedef struct _lv_timer_t lv_timer_t;

typedef struct _lv_theme_t lv_theme_t;

typedef struct _lv_anim_t lv_anim_t;

typedef struct _lv_font_t lv_font_t;
typedef struct _lv_font_class_t lv_font_class_t;
typedef struct _lv_font_info_t lv_font_info_t;

typedef struct _lv_font_manager_t lv_font_manager_t;

typedef struct _lv_image_decoder_t lv_image_decoder_t;

typedef struct _lv_image_decoder_dsc_t lv_image_decoder_dsc_t;

typedef struct _lv_draw_image_dsc_t lv_draw_image_dsc_t;

typedef struct _lv_fragment_t lv_fragment_t;
typedef struct _lv_fragment_class_t lv_fragment_class_t;
typedef struct _lv_fragment_managed_states_t lv_fragment_managed_states_t;

typedef struct _lv_profiler_builtin_config_t lv_profiler_builtin_config_t;

typedef struct _lv_rb_node_t lv_rb_node_t;

typedef struct _lv_rb_t lv_rb_t;

typedef struct _lv_color_filter_dsc_t lv_color_filter_dsc_t;

typedef struct _lv_event_dsc_t lv_event_dsc_t;

typedef struct _lv_cache_t lv_cache_t;

typedef struct _lv_cache_entry_t lv_cache_entry_t;

typedef struct _lv_fs_file_cache_t lv_fs_file_cache_t;

typedef struct _lv_fs_path_ex_t lv_fs_path_ex_t;

typedef struct _lv_image_decoder_args_t lv_image_decoder_args_t;

typedef struct _lv_image_cache_data_t lv_image_cache_data_t;

typedef struct _lv_image_header_cache_data_t lv_image_header_cache_data_t;

typedef struct _lv_draw_mask_t lv_draw_mask_t;

typedef struct _lv_draw_label_hint_t lv_draw_label_hint_t;

typedef struct _lv_draw_glyph_dsc_t lv_draw_glyph_dsc_t;

typedef struct _lv_draw_image_sup_t lv_draw_image_sup_t;

typedef struct _lv_draw_mask_rect_dsc_t lv_draw_mask_rect_dsc_t;

typedef struct _lv_obj_style_t lv_obj_style_t;

typedef struct _lv_obj_style_transition_dsc_t lv_obj_style_transition_dsc_t;

typedef struct _lv_hit_test_info_t lv_hit_test_info_t;

typedef struct _lv_cover_check_info_t lv_cover_check_info_t;

typedef struct _lv_obj_spec_attr_t lv_obj_spec_attr_t;

typedef struct _lv_image_t lv_image_t;

typedef struct _lv_animimg_t lv_animimg_t;

typedef struct _lv_arc_t lv_arc_t;

typedef struct _lv_arclabel_t lv_arclabel_t;

typedef struct _lv_label_t lv_label_t;

typedef struct _lv_bar_anim_t lv_bar_anim_t;

typedef struct _lv_bar_t lv_bar_t;

typedef struct _lv_button_t lv_button_t;

typedef struct _lv_buttonmatrix_t lv_buttonmatrix_t;

typedef struct _lv_calendar_t lv_calendar_t;

typedef struct _lv_canvas_t lv_canvas_t;

typedef struct _lv_chart_series_t lv_chart_series_t;

typedef struct _lv_chart_cursor_t lv_chart_cursor_t;

typedef struct _lv_chart_t lv_chart_t;

typedef struct _lv_checkbox_t lv_checkbox_t;

typedef struct _lv_dropdown_t lv_dropdown_t;

typedef struct _lv_dropdown_list_t lv_dropdown_list_t;

typedef struct _lv_imagebutton_src_info_t lv_imagebutton_src_info_t;

typedef struct _lv_imagebutton_t lv_imagebutton_t;

typedef struct _lv_keyboard_t lv_keyboard_t;

typedef struct _lv_led_t lv_led_t;

typedef struct _lv_line_t lv_line_t;

typedef struct _lv_menu_load_page_event_data_t lv_menu_load_page_event_data_t;

typedef struct _lv_menu_history_t lv_menu_history_t;

typedef struct _lv_menu_t lv_menu_t;

typedef struct _lv_menu_page_t lv_menu_page_t;

typedef struct _lv_msgbox_t lv_msgbox_t;

typedef struct _lv_roller_t lv_roller_t;

typedef struct _lv_scale_section_t lv_scale_section_t;

typedef struct _lv_scale_t lv_scale_t;

typedef struct _lv_slider_t lv_slider_t;

typedef struct _lv_span_t lv_span_t;

typedef struct _lv_spangroup_t lv_spangroup_t;

typedef struct _lv_textarea_t lv_textarea_t;

typedef struct _lv_spinbox_t lv_spinbox_t;

typedef struct _lv_switch_t lv_switch_t;

typedef struct _lv_table_cell_t lv_table_cell_t;

typedef struct _lv_table_t lv_table_t;

typedef struct _lv_tabview_t lv_tabview_t;

typedef struct _lv_tileview_t lv_tileview_t;

typedef struct _lv_tileview_tile_t lv_tileview_tile_t;

typedef struct _lv_win_t lv_win_t;

typedef struct _lv_3dtexture_t lv_3dtexture_t;

typedef struct _lv_observer_t lv_observer_t;

typedef struct _lv_monkey_config_t lv_monkey_config_t;

typedef struct _lv_ime_pinyin_t lv_ime_pinyin_t;

typedef struct _lv_file_explorer_t lv_file_explorer_t;

typedef struct _lv_barcode_t lv_barcode_t;

typedef struct _lv_gif_t lv_gif_t;

typedef struct _lv_qrcode_t lv_qrcode_t;

typedef struct _lv_freetype_outline_vector_t lv_freetype_outline_vector_t;

typedef struct _lv_freetype_outline_event_param_t lv_freetype_outline_event_param_t;

typedef struct _lv_fpoint_t lv_fpoint_t;

typedef struct _lv_matrix_t lv_matrix_t;

typedef struct _lv_vector_path_t lv_vector_path_t;

typedef struct _lv_vector_gradient_t lv_vector_gradient_t;

typedef struct _lv_vector_fill_dsc_t lv_vector_fill_dsc_t;

typedef struct _lv_vector_stroke_dsc_t lv_vector_stroke_dsc_t;

typedef struct _lv_vector_draw_dsc_t lv_vector_draw_dsc_t;

typedef struct _lv_draw_vector_task_dsc_t lv_draw_vector_task_dsc_t;

typedef struct _lv_vector_dsc_t lv_vector_dsc_t;

typedef struct _lv_xkb_t lv_xkb_t;

typedef struct _lv_libinput_event_t lv_libinput_event_t;

typedef struct _lv_libinput_t lv_libinput_t;

typedef struct _lv_draw_sw_unit_t lv_draw_sw_unit_t;

typedef struct _lv_draw_sw_mask_common_dsc_t lv_draw_sw_mask_common_dsc_t;

typedef struct _lv_draw_sw_mask_line_param_t lv_draw_sw_mask_line_param_t;

typedef struct _lv_draw_sw_mask_angle_param_t lv_draw_sw_mask_angle_param_t;

typedef struct _lv_draw_sw_mask_radius_param_t lv_draw_sw_mask_radius_param_t;

typedef struct _lv_draw_sw_mask_fade_param_t lv_draw_sw_mask_fade_param_t;

typedef struct _lv_draw_sw_mask_map_param_t lv_draw_sw_mask_map_param_t;

typedef struct _lv_draw_sw_blend_dsc_t lv_draw_sw_blend_dsc_t;

typedef struct _lv_draw_sw_blend_fill_dsc_t lv_draw_sw_blend_fill_dsc_t;

typedef struct _lv_draw_sw_blend_image_dsc_t lv_draw_sw_blend_image_dsc_t;

typedef struct _lv_draw_buf_handlers_t lv_draw_buf_handlers_t;

typedef struct _lv_rlottie_t lv_rlottie_t;

typedef struct _lv_ffmpeg_player_t lv_ffmpeg_player_t;

typedef struct _lv_glfw_window_t lv_glfw_window_t;
typedef struct _lv_glfw_texture_t lv_glfw_texture_t;

typedef uint32_t lv_prop_id_t;

typedef struct _lv_array_t lv_array_t;

typedef struct _lv_iter_t lv_iter_t;

typedef struct _lv_circle_buf_t lv_circle_buf_t;

typedef struct _lv_draw_buf_t lv_draw_buf_t;

#if LV_USE_OBJ_PROPERTY
typedef struct _lv_property_name_t lv_property_name_t;
#endif

#if LV_USE_SYSMON

typedef struct _lv_sysmon_backend_data_t lv_sysmon_backend_data_t;

#if LV_USE_PERF_MONITOR
typedef struct _lv_sysmon_perf_info_t lv_sysmon_perf_info_t;
#endif /*LV_USE_PERF_MONITOR*/

#endif /*LV_USE_SYSMON*/


typedef struct _lv_xml_component_scope_t lv_xml_component_scope_t;

typedef struct _lv_xml_parser_state_t lv_xml_parser_state_t;

#if LV_USE_EVDEV
typedef struct _lv_evdev_discovery_t lv_evdev_discovery_t;
#endif

#if LV_USE_TRANSLATION
typedef struct _lv_translation_tag_dsc_t lv_translation_tag_dsc_t;

typedef struct _lv_translation_pack_t lv_translation_pack_t;
#endif

#endif /*__ASSEMBLY__*/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#define LV_UNUSED(x) ((void)x)

#define _LV_CONCAT(x, y) x ## y
#define LV_CONCAT(x, y) _LV_CONCAT(x, y)
#undef _LV_CONCAT

#define _LV_CONCAT3(x, y, z) x ## y ## z
#define LV_CONCAT3(x, y, z) _LV_CONCAT3(x, y, z)
#undef _LV_CONCAT3

#if defined(PYCPARSER) || defined(__CC_ARM)
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg)
#elif defined(__GNUC__) && ((__GNUC__ == 4 && __GNUC_MINOR__ >= 4) || __GNUC__ > 4)
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg) __attribute__((format(gnu_printf, fmtstr, vararg)))
#elif (defined(__clang__) || defined(__GNUC__) || defined(__GNUG__) || defined(__IAR_SYSTEMS_ICC__))
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg) __attribute__((format(printf, fmtstr, vararg)))
#else
#define LV_FORMAT_ATTRIBUTE(fmtstr, vararg)
#endif

#ifndef LV_NORETURN
#if defined(PYCPARSER)
#define LV_NORETURN
#elif defined(__GNUC__)
#define LV_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define LV_NORETURN __declspec(noreturn)
#else
#define LV_NORETURN
#endif
#endif /* LV_NORETURN not defined */

#ifndef LV_UNREACHABLE
#if defined(__GNUC__)
#define LV_UNREACHABLE() __builtin_unreachable()
#elif defined(_MSC_VER)
#define LV_UNREACHABLE() __assume(0)
#else
#define LV_UNREACHABLE() while(1)
#endif
#endif /* LV_UNREACHABLE not defined */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_TYPES_H*/
