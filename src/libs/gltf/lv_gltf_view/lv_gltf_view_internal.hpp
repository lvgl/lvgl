/**
 * @file lv_gltf_view_internal.h
 *
 */

#ifndef LV_GLTF_VIEW_INTERNAL_HPP
#define LV_GLTF_VIEW_INTERNAL_HPP

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../../../misc/lv_types.h"
#include "../lv_gl_shader/lv_gl_shader_internal.h"
#include "../../../widgets/3dtexture/lv_3dtexture_private.h"
#include "../lv_gltf_data/lv_gltf_data.h"
#include "../lv_gltf_data/lv_gltf_data_internal.h"

#include <fastgltf/math.hpp>
#include <fastgltf/types.hpp>
#include <map>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    ANTIALIAS_OFF = 0,
    ANTIALIAS_CONSTANT = 1,
    ANTIALIAS_NOT_MOVING = 2,
    ANTIALIAS_UNKNOWN = 999
} AntialiasingMode;

typedef enum {
    BG_CLEAR = 0,
    BG_SOLID = 1,
    BG_ENVIRONMENT = 2,
    BG_UNKNOWN = 999
} BackgroundMode;

typedef struct {
    uint32_t texture;
    uint32_t renderbuffer;
    unsigned framebuffer;
} gl_renwin_state_t;

typedef struct  {
    gl_renwin_state_t render_state;
    gl_renwin_state_t opaque_render_state;

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
    bool recenter_flag;
    float spin_degree_offset;   // This amount is added to the yaw and can be incremented overtime for a spinning platter effect
    float focal_x;
    float focal_y;
    float focal_z;
    uint8_t bg_r;               // The background color r/g/b/a - note the rgb components have affect on antialiased edges that border on empty space, even when alpha is zero.
    uint8_t bg_g;
    uint8_t bg_b;
    uint8_t bg_a;
    bool frame_was_cached;
    bool frame_was_antialiased;
    bool dirty;
    int32_t camera;             // -1 for default (first scene camera if available or platter if not), 0 = platter, 1+ = Camera index in the order it appeared within the current scene render.  Any value higher than the scene's camera count will be limited to the scene's camera count.
    int32_t anim;               // -1 for no animations, 0+ = Animation index.  Any value higher than the scene's animation count will be limited to the scene's animation count.
    float timestep;             // How far to step the current animation in seconds
    int32_t aa_mode;            // The anti-aliasing mode: 0 = None, 1 = Always, 2 = When Moving or Animated
    int32_t bg_mode;            // The background mode: 0 = Clear, 1 = Solid Color, 2 = The Environment
    float blur_bg;              // How much to blur the environment background, between 0.0 and 1.0
    float env_pow;              // Environmental brightness, 1.0 default
    float exposure;             // Image exposure level, 1.0 default
} lv_gltf_view_desc_t;

typedef struct {
    GLboolean blendEnabled;
    GLint blendSrc;
    GLint blendDst;
    GLint blendEquation;
    GLfloat clearDepth;
    GLfloat clearColor[4];
}lv_gl_state_t;

typedef struct {
    uint32_t diffuse;
    uint32_t specular;
    uint32_t sheen;
    uint32_t ggxLut;
    uint32_t charlieLut;
    uint32_t mipCount;
    float iblIntensityScale;
    float angle;
    bool loaded;
} lv_gltf_view_env_textures_t;


struct _lv_gltf_view_t {
    lv_3dtexture_t texture;
    lv_array_t models;
    lv_gltf_view_state_t state;
    fastgltf::math::fmat4x4 view_matrix;
    fastgltf::math::fmat4x4 projection_matrix;
    fastgltf::math::fmat4x4 view_projection_matrix;
    fastgltf::math::fvec3 camera_pos;

    std::map<int32_t, std::map<fastgltf::Node*, fastgltf::math::fmat4x4>> ibm_by_skin_the_node;
    float env_rotation_angle;
    float bound_radius;

    lv_gltf_view_desc_t desc;
    lv_gltf_view_desc_t last_desc;
    lv_gl_shader_manager_t* shader_manager;
    lv_gltf_view_env_textures_t env_textures;
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

GLuint lv_gltf_view_render(lv_gltf_view_t * viewer);
lv_result_t lv_gltf_view_shader_injest_discover_defines(lv_array_t *result, lv_gltf_data_t *data, fastgltf::Node *node,
						 fastgltf::Primitive *prim);

lv_gltf_renwin_shaderset_t lv_gltf_view_shader_compile_program(lv_gltf_view_t *view, const lv_gl_shader_t *defines, size_t n);

/* TODO: These are currently used during the rendering phase but could probably be used during the initialization phase
 * Once we start using them in the initialization function they can be defined as static inside `lv_gltf_view.cpp`
 */
gl_renwin_state_t setup_opaque_output(uint32_t texture_width, uint32_t texture_height);
void setup_cleanup_opengl_output(gl_renwin_state_t *state);
gl_renwin_state_t setup_primary_output(uint32_t texture_width, uint32_t texture_height, bool mipmaps_enabled);
void setup_view_proj_matrix_from_camera(lv_gltf_view_t *viewer, int32_t _cur_cam_num, lv_gltf_view_desc_t *view_desc,
					const fastgltf::math::fmat4x4 view_mat, const fastgltf::math::fvec3 view_pos,
					lv_gltf_data_t *gltf_data, bool transmission_pass);

void setup_view_proj_matrix(lv_gltf_view_t *viewer, lv_gltf_view_desc_t *view_desc, lv_gltf_data_t *gltf_data,
			    bool transmission_pass);
lv_result_t setup_restore_opaque_output(lv_gltf_view_desc_t *view_desc, gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h,
				 bool prepare_bg);
void setup_draw_environment_background(lv_gl_shader_manager_t *manager, lv_gltf_view_t *viewer, float blur);
void setup_environment_rotation_matrix(float env_rotation_angle, uint32_t shader_program);
void setup_background_environment(GLuint program, GLuint *vao, GLuint *indexBuffer, GLuint *vertexBuffer);
void setup_uniform_color_alpha(GLint uniform_loc, fastgltf::math::nvec4 color);
lv_result_t setup_restore_primary_output(lv_gltf_view_desc_t *view_desc, gl_renwin_state_t _ret, uint32_t texture_w, uint32_t texture_h,
				  uint32_t texture_offset_w, uint32_t texture_offset_h, bool prepare_bg);
void setup_uniform_color(GLint uniform_loc, fastgltf::math::nvec3 color);
uint32_t setup_texture(uint32_t tex_unit, uint32_t tex_name, int32_t tex_coord_index,
		       std::unique_ptr<fastgltf::TextureTransform> &tex_transform, GLint sampler, GLint uv_set,
		       GLint uv_transform);
fastgltf::math::fmat3x3 setup_texture_transform_matrix(fastgltf::TextureTransform transform);
void lv_gltf_view_recache_all_transforms(lv_gltf_view_t *viewer, lv_gltf_data_t *gltf_data);


lv_obj_t *lv_gltf_view_create_internal(lv_obj_t *parent);
lv_gltf_data_t *lv_gltf_load_model_from_file_internal(lv_obj_t *obj, const char *path);

/**********************
 *      MACROS
 **********************/


#endif /*LV_USE_GLTF*/

#endif /*LV_GLTF_VIEW_INTERNAL_HPP*/
