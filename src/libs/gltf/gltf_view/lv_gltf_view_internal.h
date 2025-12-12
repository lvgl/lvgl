/**
 * @file lv_gltf_view_internal.h
 *
 */

#ifndef LV_GLTF_VIEW_INTERNAL_H
#define LV_GLTF_VIEW_INTERNAL_H

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "lv_gltf.h"
#include "../../../misc/lv_types.h"
#include "../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"
#include "../../../widgets/3dtexture/lv_3dtexture_private.h"
#include "../gltf_data/lv_gltf_data_internal.h"


/*********************
 *      DEFINES
 *********************/

/* ::Gamma Presets::
 * Standard Gamma value is 2.2
 * Values range from 0.5 to 3.5, roughly speaking, with
 * reasonable results between the 1.5 and 2.8 levels.
 * The value must be enclosed with quotes, as a string literal.
 */
#define LV_GLTF_GAMMA_BRIGHTEST "3.5"
#define LV_GLTF_GAMMA_BRIGHTER  "3.0"
#define LV_GLTF_GAMMA_BRIGHT    "2.6"
#define LV_GLTF_GAMMA_STANDARD  "2.2"
#define LV_GLTF_GAMMA_DARK      "1.8"
#define LV_GLTF_GAMMA_DARKER    "1.3"
#define LV_GLTF_GAMMA_DARKEST   "0.8"

#define LV_GLTF_DISTANCE_SCALE_FACTOR 2.5f
#define LV_GLTF_TRANSMISSION_PASS_SIZE 256

/* Apply defaults below if not set explicitly */

/* Tone-mapping is not applied if linear output is enabled.
 * Linear output is the default.
 */
#ifndef LV_GLTF_LINEAR_OUTPUT
    #define LV_GLTF_LINEAR_OUTPUT 1
#endif

/* If tone-mapping is applied, this adjusts the brightness
 * and color range of the output. Use stringified values.
 */
#ifndef LV_GLTF_TONEMAP_GAMMA
    #define LV_GLTF_TONEMAP_GAMMA LV_GLTF_GAMMA_STANDARD
#endif

/**********************
 *      TYPEDEFS
 **********************/

#ifdef __cplusplus
extern "C" {
#endif/* __cplusplus*/


typedef struct {
    uint32_t texture;
    uint32_t renderbuffer;
    unsigned framebuffer;
} lv_gltf_renwin_state_t;

typedef struct  {
    lv_gltf_renwin_state_t render_state;
    lv_gltf_renwin_state_t opaque_render_state;

    uint64_t opaque_frame_buffer_width;
    uint64_t opaque_frame_buffer_height;
    uint32_t material_variant;
    bool render_state_ready;
    bool render_opaque_buffer;
} lv_gltf_view_state_t;

typedef struct {
    float pitch;
    float yaw;
    float distance;
    float fov;                  // The vertical FOV, in degrees.  If this is zero, the view will be orthographic (non-perspective)
    int32_t render_width;       // If anti-aliasing is not applied this frame, these are the same as width/height, if antialiasing
    int32_t render_height;      // is enabled, these are width/height * antialias upscale power (currently 2.0)
    float focal_x;
    float focal_y;
    float focal_z;
    bool frame_was_antialiased;
    int32_t animation_speed_ratio;
    lv_gltf_aa_mode_t aa_mode;
    lv_gltf_bg_mode_t bg_mode;
    float blur_bg;              /** How much to blur the environment background, between 0.0 and 1.0 */
    float env_pow;              /** Environmental brightness, 1.8 by default */
    float exposure;             /** Image exposure level, 1.0 default */
} lv_gltf_view_desc_t;

typedef struct {
    GLboolean blend_enabled;
    GLint blend_src;
    GLint blend_dst;
    GLint blend_equation;
    GLfloat clear_depth;
    GLfloat clear_color[4];
} lv_opengl_state_t;


#ifdef __cplusplus
}


#include <fastgltf/math.hpp>
#include <fastgltf/types.hpp>
#include <map>

struct _lv_gltf_t {
    lv_3dtexture_t texture;
    lv_array_t models;
    lv_gltf_view_state_t state;
    lv_gltf_view_desc_t desc;
    lv_gltf_view_desc_t last_desc;
    lv_opengl_shader_manager_t shader_manager;
    lv_gltf_environment_t * environment;
    fastgltf::math::fmat4x4 view_matrix;
    fastgltf::math::fmat4x4 projection_matrix;
    fastgltf::math::fmat4x4 view_projection_matrix;
    fastgltf::math::fvec3 camera_pos;

    std::map<int32_t, std::map<fastgltf::Node *, fastgltf::math::fmat4x4>> ibm_by_skin_then_node;
    bool owns_environment;
};

/**********************
 * GLOBAL PROTOTYPES
 **********************/

GLuint lv_gltf_view_render(lv_gltf_t * viewer);
lv_result_t lv_gltf_view_shader_injest_discover_defines(lv_array_t * result, lv_gltf_model_t * data,
                                                        fastgltf::Node * node,
                                                        fastgltf::Primitive * prim);

lv_gltf_shaderset_t lv_gltf_view_shader_compile_program(lv_gltf_t * view, const lv_opengl_shader_define_t * defines,
                                                        size_t n);

/**********************
 *      MACROS
 **********************/

#endif/* __cplusplus*/
#endif /*LV_USE_GLTF*/

#endif /*LV_GLTF_VIEW_INTERNAL_H*/
