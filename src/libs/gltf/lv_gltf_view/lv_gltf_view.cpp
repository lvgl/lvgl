/**
 * @file lv_gltf_view.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_view_internal.hpp"

#if LV_USE_GLTF

#include <optional>
#include "../lv_gltf_data/lv_gltf_data.h"
#include "../lv_gltf_data/lv_gltf_data_internal.hpp"
#include "../math/lv_gltf_math.hpp"
#include "../../../draw/lv_draw_3d.h"
#include "lv_gltf_view_internal.hpp"
#include "../fastgltf/lv_fastgltf.hpp"
#include "../../../core/lv_obj_class_private.h"
#include "../../../misc/lv_types.h"
#include "../../../widgets/3dtexture/lv_3dtexture.h"
#include "ibl/lv_gltf_view_ibl_sampler.hpp"
#include "assets/lv_gltf_view_shader.h"
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>

/*********************
 *      DEFINES
 *********************/

#define MY_CLASS (&lv_gltf_view_class)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void lv_gltf_view_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gltf_view_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gltf_view_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_gltf_view_state_init(lv_gltf_view_state_t * state);
static void lv_gltf_view_desc_init(lv_gltf_view_desc_t * state);
static void destroy_environment(lv_gltf_view_env_textures_t * env);
static void set_time_cb(lv_timer_t * timer);
const lv_obj_class_t lv_gltf_view_class = {
    .base_class = &lv_3dtexture_class,
    .constructor_cb = lv_gltf_view_constructor,
    .destructor_cb = lv_gltf_view_destructor,
    .event_cb = lv_gltf_view_event,
    .name = "lv_gltf_viewer",
    .width_def = LV_DPI_DEF * 2,
    .height_def = LV_DPI_DEF / 10,
    .instance_size = sizeof(lv_gltf_view_t),
};

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

extern "C" {

    lv_obj_t * lv_gltf_view_create(lv_obj_t * parent)
    {
        lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
        lv_obj_class_init_obj(obj);
        return obj;
    }

    lv_gltf_data_t * lv_gltf_load_model_from_file(lv_obj_t * obj, const char * path)
    {
        LV_ASSERT_NULL(obj);
        LV_ASSERT_OBJ(obj, MY_CLASS);
        lv_gltf_view_t * viewer = (lv_gltf_view_t *)obj;
        lv_gltf_data_t * model = lv_gltf_data_load_from_file(path, viewer->shader_manager);
        if(!model) {
            return NULL;
        }
        if(lv_array_push_back(&viewer->models, &model) == LV_RESULT_INVALID) {
            lv_gltf_data_destroy(model);
            return NULL;
        }
        const size_t animation_count = lv_gltf_data_get_animation_count(model);

        if(animation_count > 0) {
            lv_timer_create(set_time_cb, LV_DEF_REFR_PERIOD, viewer);
            viewer->desc.timestep = LV_DEF_REFR_PERIOD / 1000.;
        }
        return model;
    }
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void set_time_cb(lv_timer_t * timer)
{
    lv_obj_t * obj = (lv_obj_t *)lv_timer_get_user_data(timer);
    lv_obj_invalidate(obj);
}

static void lv_gltf_view_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_gltf_view_t * view = (lv_gltf_view_t *)obj;
    lv_gltf_view_state_init(&view->state);
    lv_gltf_view_desc_init(&view->desc);
    view->view_matrix = fastgltf::math::fmat4x4(1.0f);
    view->projection_matrix = fastgltf::math::fmat4x4(1.0f);
    view->view_projection_matrix = fastgltf::math::fmat4x4(1.0f);
    view->camera_pos = fastgltf::math::fvec3(0.0f);
    view->env_rotation_angle = 0.0f;
    view->texture.h_flip = false;
    view->texture.v_flip = false;

    lv_gltf_view_shader shaders;
    lv_gltf_view_shader_get_src(&shaders);
    char * vertex_shader = lv_gltf_view_shader_get_vertex();
    char * frag_shader = lv_gltf_view_shader_get_fragment();
    /*LV_LOG_USER("Vertex shader: %s", vertex_shader);*/
    //LV_LOG_USER("Frag shader: %s", frag_shader);
    //
    view->shader_manager = lv_gl_shader_manager_create(shaders.shader_list, shaders.count, vertex_shader, frag_shader);
    /*view->shader_manager = lv_gl_shader_manager_create(src_includes, sizeof(src_includes) / sizeof(src_includes[0]),*/
    /*                                                   PREPROCESS(src_vertexShader), PREPROCESS(src_fragmentShader));*/
    lv_free(vertex_shader);
    lv_free(frag_shader);

    view->env_textures = lv_gltf_view_ibl_sampler_setup(NULL, NULL, 0);

    lv_array_init(&view->models, 1, sizeof(lv_gltf_data_t *));
    new(&view->ibm_by_skin_the_node) std::map<int32_t, std::map<fastgltf::Node *, fastgltf::math::fmat4x4> >;

    LV_TRACE_OBJ_CREATE("end");
}

static void lv_gltf_view_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DRAW_MAIN) {
        lv_obj_t * obj = (lv_obj_t *)lv_event_get_current_target(e);
        lv_gltf_view_t * viewer = (lv_gltf_view_t *)obj;
        GLuint texture_id = lv_gltf_view_render(viewer);
        lv_3dtexture_set_src((lv_obj_t *)&viewer->texture, (lv_3dtexture_id_t)texture_id);
    }

    lv_result_t res;

    /*Call the ancestor's event handler*/
    res = lv_obj_event_base(MY_CLASS, e);
    if(res != LV_RESULT_OK) {
        return;
    }

}
static void lv_gltf_view_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_gltf_view_t * view = (lv_gltf_view_t *)obj;
    lv_gl_shader_manager_destroy(view->shader_manager);
    const size_t n = lv_array_size(&view->models);
    for(size_t i = 0; i < n; ++i) {
        lv_gltf_data_destroy(*(lv_gltf_data_t **)lv_array_at(&view->models, i));
    }
    destroy_environment(&view->env_textures);
}

static void lv_gltf_view_state_init(lv_gltf_view_state_t * state)
{
    lv_memset(state, 0, sizeof(*state));
    state->opaque_frame_buffer_width = 256;
    state->opaque_frame_buffer_height = 256;
    state->material_variant = 0;
    state->render_state_ready = false;
    state->render_opaque_buffer = false;
    /* TODO: Do this during the intialization phase
        state->opaque_render_state = setup_opaque_output(state->opaque_frame_buffer_width, state->opaque_frame_buffer_height);
    */
}
static void lv_gltf_view_desc_init(lv_gltf_view_desc_t * desc)
{
    lv_memset(desc, 0, sizeof(*desc));
    desc->pitch = 0.f;
    desc->yaw = 0.f;
    desc->distance = 1.f;
    desc->focal_x = 0.f;
    desc->focal_y = 0.f;
    desc->focal_z = 0.f;
    desc->exposure = 0.8f;
    desc->env_pow = 1.8f;
    desc->blur_bg = 0.2f;
    desc->bg_mode = 0;
    desc->aa_mode = 2;
    desc->camera = 0;
    desc->fov = 45.f;
    desc->anim = 0;
    desc->spin_degree_offset = 0.f;
    desc->timestep = 0.f;
    desc->dirty = true;
    desc->recenter_flag = true;
    desc->frame_was_cached = false;
    desc->frame_was_antialiased = false;
    desc->bg_r = 230;
    desc->bg_g = 230;
    desc->bg_b = 230;
    desc->bg_a = 255;
}

gl_renwin_state_t setup_opaque_output(uint32_t texture_width, uint32_t texture_height)
{
    gl_renwin_state_t _ret;

    GLuint rtex;
    GL_CALL(glGenTextures(1, &rtex));
    _ret.texture = rtex;

    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GLuint rdepth;
    GL_CALL(glGenTextures(1, &rdepth));
    _ret.renderbuffer = rdepth;
    GL_CALL(glBindTexture(GL_TEXTURE_2D, _ret.renderbuffer));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
#ifdef __EMSCRIPTEN__ // Check if compiling for Emscripten (WebGL)
    // For WebGL2
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_INT, NULL));
#else
    // For Desktop OpenGL
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_SHORT, NULL));
#endif
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));

    GL_CALL(glGenFramebuffers(1, &_ret.framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));

    return _ret;
}

void setup_cleanup_opengl_output(gl_renwin_state_t * state)
{
    if(state) {
        // Delete the framebuffer
        if(state->framebuffer) {
            GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER,
                                      0)); // Unbind the framebuffer
            GL_CALL(glDeleteFramebuffers(1, &state->framebuffer));
            state->framebuffer = 0; // Reset to avoid dangling pointer
        }

        // Delete the color texture
        if(state->texture) {
            GL_CALL(glDeleteTextures(1, &state->texture));
            state->texture = 0; // Reset to avoid dangling pointer
        }

        // Delete the depth texture
        if(state->renderbuffer) {
            GL_CALL(glDeleteTextures(1, &state->renderbuffer));
            state->renderbuffer = 0; // Reset to avoid dangling pointer
        }
    }
}

gl_renwin_state_t setup_primary_output(uint32_t texture_width, uint32_t texture_height, bool mipmaps_enabled)
{
    gl_renwin_state_t result;

    GLuint rtex;
    GL_CALL(glGenTextures(1, &rtex));
    result.texture = rtex;
    GL_CALL(glBindTexture(GL_TEXTURE_2D, result.texture));
    //GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, mipmaps_enabled ? GL_LINEAR_MIPMAP_LINEAR : GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                            mipmaps_enabled ? GL_NEAREST_MIPMAP_NEAREST : GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    GLuint rdepth;
    GL_CALL(glGenTextures(1, &rdepth));
    result.renderbuffer = rdepth;
    GL_CALL(glBindTexture(GL_TEXTURE_2D, result.renderbuffer));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1));
#ifdef __EMSCRIPTEN__ // Check if compiling for Emscripten (WebGL)
    // For WebGL2
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_INT, NULL));
#else
    // For Desktop OpenGL
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_SHORT, NULL));
#endif
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));

    GL_CALL(glGenFramebuffers(1, &result.framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, result.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.renderbuffer, 0));

    return result;
}

void setup_view_proj_matrix_from_camera(lv_gltf_view_t * viewer, int32_t _cur_cam_num, lv_gltf_view_desc_t * view_desc,
                                        const fastgltf::math::fmat4x4 view_mat, const fastgltf::math::fvec3 view_pos,
                                        lv_gltf_data_t * gltf_data, bool transmission_pass)
{
    // The following matrix math is for the projection matrices as defined by the glTF spec:
    // https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#projection-matrices

    fastgltf::math::fmat4x4 projection;
    const auto & asset = lv_gltf_data_get_asset(gltf_data);

    auto width = view_desc->render_width;
    auto height = view_desc->render_height;
    // It's possible the transmission pass should simply use the regular passes aspect despite having different metrics itself.  Testing both ways to see which has less distortion
    float aspect = (float)width / (float)height;
    if(transmission_pass) {
        width = 256;
        height = 256;
    }

    std::visit(fastgltf::visitor{
        [&](fastgltf::Camera::Perspective & perspective)
        {
            projection = fastgltf::math::fmat4x4(0.0f);
            projection[0][0] = 1.f / (aspect * tan(0.5f * perspective.yfov));
            projection[1][1] = 1.f / (tan(0.5f * perspective.yfov));
            projection[2][3] = -1;

            if(perspective.zfar.has_value()) {
                // Finite projection matrix
                projection[2][2] = (*perspective.zfar + perspective.znear) /
                (perspective.znear - *perspective.zfar);
                projection[3][2] = (2 * *perspective.zfar * perspective.znear) /
                (perspective.znear - *perspective.zfar);
            }
            else {
                // Infinite projection matrix
                projection[2][2] = -1;
                projection[3][2] = -2 * perspective.znear;
            }
        },
        [&](fastgltf::Camera::Orthographic & orthographic)
        {
            projection = fastgltf::math::fmat4x4(1.0f);
            projection[0][0] = (1.f / orthographic.xmag) * aspect;
            projection[1][1] = 1.f / orthographic.ymag;
            projection[2][2] = 2.f / (orthographic.znear - orthographic.zfar);
            projection[3][2] =
                (orthographic.zfar + orthographic.znear) / (orthographic.znear - orthographic.zfar);
        },
    },
    asset->cameras[_cur_cam_num].camera);

    viewer->view_matrix = view_mat;
    viewer->projection_matrix = projection;
    viewer->view_projection_matrix = projection * view_mat;
    viewer->camera_pos = view_pos;
}

void lv_gltf_view_recache_all_transforms(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data)
{
    const lv_gltf_view_desc_t * view_desc = &viewer->desc;
    const auto & asset = lv_gltf_data_get_asset(gltf_data);
    int32_t PREF_CAM_NUM = std::min(view_desc->camera, (int32_t)lv_gltf_data_get_camera_count(gltf_data) - 1);
    int32_t anim_num = view_desc->anim;
    uint32_t sceneIndex = 0;

    gltf_data->last_camera_index = PREF_CAM_NUM;
    lv_gltf_data_clear_transform_cache(gltf_data);
    gltf_data->current_camera_index = -1;
    gltf_data->has_any_cameras = false;
    auto tmat = fastgltf::math::fmat4x4{};
    auto cammat = fastgltf::math::fmat4x4{};
    fastgltf::custom_iterate_scene_nodes(
        *asset, sceneIndex, &tmat,
    [&](fastgltf::Node & node, fastgltf::math::fmat4x4 & parentworldmatrix, fastgltf::math::fmat4x4 & localmatrix) {
        bool made_changes = false;
        bool made_rotation_changes = false;
        if(lv_gltf_data_animation_get_channel_set(anim_num, gltf_data, node)->size() > 0) {
            lv_gltf_data_animation_matrix_apply(gltf_data->local_timestamp, anim_num, gltf_data, node,
                                                localmatrix);
            made_changes = true;
        }
        if(gltf_data->node_binds.find(&node) != gltf_data->node_binds.end()) {
            lv_gltf_bind_t * current_override = gltf_data->node_binds[&node];
            fastgltf::math::fvec3 local_pos;
            fastgltf::math::fquat local_quat;
            fastgltf::math::fvec3 local_scale;
            fastgltf::math::decomposeTransformMatrix(localmatrix, local_scale, local_quat, local_pos);
            fastgltf::math::fvec3 local_rot = lv_gltf_math_quaternion_to_euler(local_quat);

            // Traverse through all linked overrides
            while(current_override != nullptr) {
                if(current_override->prop == LV_GLTF_BIND_PROP_ROTATION) {
                    if(current_override->dir) {
                        current_override->data[0] = local_rot[0];
                        current_override->data[1] = local_rot[1];
                        current_override->data[2] = local_rot[2];
                    }
                    else {
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_1)
                            local_rot[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_2)
                            local_rot[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_3)
                            local_rot[2] = current_override->data[2];
                        made_changes = true;
                        made_rotation_changes = true;
                    }
                }
                else if(current_override->prop == LV_GLTF_BIND_PROP_POSITION) {
                    if(current_override->dir) {
                        current_override->data[0] = local_pos[0];
                        current_override->data[1] = local_pos[1];
                        current_override->data[2] = local_pos[2];
                    }
                    else {
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_1)
                            local_pos[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_2)
                            local_pos[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_3)
                            local_pos[2] = current_override->data[2];
                        made_changes = true;
                    }
                }
                else if(current_override->prop == LV_GLTF_BIND_PROP_WORLD_POSITION) {
                    fastgltf::math::fvec3 world_pos;
                    fastgltf::math::fquat world_quat;
                    fastgltf::math::fvec3 world_scale;
                    fastgltf::math::decomposeTransformMatrix(parentworldmatrix * localmatrix,
                                                             world_scale, world_quat, world_pos);
                    //fastgltf::math::fvec3 world_rot = quaternionToEuler(world_quat);

                    if(current_override->dir) {
                        current_override->data[0] = world_pos[0];
                        current_override->data[1] = world_pos[1];
                        current_override->data[2] = world_pos[2];
                    } /*else {
                        if(current_override->data_mask & LV_GLTF_OVERRIDE_MASK_CHANNEL_1) world_pos[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_OVERRIDE_MASK_CHANNEL_2) world_pos[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_OVERRIDE_MASK_CHANNEL_3) world_pos[2] = current_override->data[2];
                        made_changes = true;
                    }*/
                }
                else if(current_override->prop == LV_GLTF_BIND_PROP_SCALE) {
                    if(current_override->dir) {
                        current_override->data[0] = local_scale[0];
                        current_override->data[1] = local_scale[1];
                        current_override->data[2] = local_scale[2];
                    }
                    else {
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_1)
                            local_scale[0] = current_override->data[0];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_2)
                            local_scale[1] = current_override->data[1];
                        if(current_override->data_mask & LV_GLTF_BIND_CHANNEL_3)
                            local_scale[2] = current_override->data[2];
                        made_changes = true;
                    }
                }

                // Move to the next override in the linked list
                current_override = current_override->next_bind;
            }

            // Rebuild the local matrix after applying all overrides
            localmatrix = fastgltf::math::scale(
                              fastgltf::math::rotate(fastgltf::math::translate(fastgltf::math::fmat4x4(), local_pos),
                                                     made_rotation_changes ?
                                                     lv_gltf_math_euler_to_quartenion(
                                                         local_rot[0], local_rot[1], local_rot[2]) :
                                                     local_quat),
                              local_scale);
        }

        if(made_changes || !lv_gltf_data_has_cached_transform(gltf_data, &node)) {
            lv_gltf_data_set_cached_transform(gltf_data, &node, parentworldmatrix * localmatrix);
        }
        if(node.cameraIndex.has_value() && (gltf_data->current_camera_index < PREF_CAM_NUM)) {
            gltf_data->has_any_cameras = true;
            gltf_data->current_camera_index += 1;
            if(gltf_data->current_camera_index == PREF_CAM_NUM) {
                cammat = (parentworldmatrix * localmatrix);
            }
        }
    });
    if(gltf_data->has_any_cameras) {
        gltf_data->view_pos[0] = cammat[3][0];
        gltf_data->view_pos[1] = cammat[3][1];
        gltf_data->view_pos[2] = cammat[3][2];
        gltf_data->view_mat = fastgltf::math::invert(cammat);
    }
    else
        gltf_data->current_camera_index = -1;
}
void setup_view_proj_matrix(lv_gltf_view_t * viewer, lv_gltf_view_desc_t * view_desc, lv_gltf_data_t * gltf_data,
                            bool transmission_pass)
{
    // Create Look-At Matrix

    if(view_desc->recenter_flag) {
        view_desc->recenter_flag = false;
        const auto & _autocenpos = lv_gltf_data_get_center(gltf_data);
        view_desc->focal_x = _autocenpos[0];
        view_desc->focal_y = _autocenpos[1];
        view_desc->focal_z = _autocenpos[2];
    }

    auto _bradius = lv_gltf_data_get_radius(gltf_data);
    float radius = _bradius * 2.5;
    radius *= view_desc->distance;

    fastgltf::math::fvec3 rcam_dir = fastgltf::math::fvec3(0.0f, 0.0f, 1.0f);

    // Note because we switched over to fastgltf math and it's right-hand focused, z axis is actually pitch (instead of x-axis), and x axis is yaw, instead of y-axis
    fastgltf::math::fmat3x3 rotation1 =
        fastgltf::math::asMatrix(lv_gltf_math_euler_to_quartenion(0.f, 0.f, fastgltf::math::radians(view_desc->pitch)));
    fastgltf::math::fmat3x3 rotation2 = fastgltf::math::asMatrix(lv_gltf_math_euler_to_quartenion(
                                                                     fastgltf::math::radians(view_desc->yaw + view_desc->spin_degree_offset), 0.f, 0.f));

    rcam_dir = rotation1 * rcam_dir;
    rcam_dir = rotation2 * rcam_dir;

    fastgltf::math::fvec3 ncam_dir = fastgltf::math::normalize(rcam_dir);
    fastgltf::math::fvec3 cam_target = fastgltf::math::fvec3(view_desc->focal_x, view_desc->focal_y, view_desc->focal_z);
    fastgltf::math::fvec3 cam_position = fastgltf::math::fvec3(cam_target[0] + (ncam_dir[0] * radius),
                                                               cam_target[1] + (ncam_dir[1] * radius),
                                                               cam_target[2] + (ncam_dir[2] * radius));

    fastgltf::math::fmat4x4 view_mat =
        lv_gltf_math_look_at_rh(cam_position, cam_target, fastgltf::math::fvec3(0.0f, 1.0f, 0.0f));

    // Create Projection Matrix
    fastgltf::math::fmat4x4 projection;
    float fov = view_desc->fov;

    float znear = _bradius * 0.05f;
    float zfar = _bradius * std::max(4.0, 8.0 * view_desc->distance);
    auto width = view_desc->render_width;
    auto height = view_desc->render_height;
    // It's possible the transmission pass should simply use the regular passes aspect despite having different metrics itself.  Testing both ways to see which has less distortion
    float aspect = (float)width / (float)height;
    if(transmission_pass) {
        width = 256;
        height = 256;
    }

    if(fov <= 0.0f) {
        // Isometric view: create an orthographic projection
        float orthoSize = view_desc->distance * _bradius; // Adjust as needed

        projection = fastgltf::math::fmat4x4(1.0f);
        projection[0][0] = -(orthoSize * aspect);
        projection[1][1] = (orthoSize);
        projection[2][2] = 2.f / (znear - zfar);
        projection[3][2] = (zfar + znear) / (znear - zfar);

    }
    else {
        // Perspective view
        projection = fastgltf::math::fmat4x4(0.0f);
        assert(width != 0 && height != 0);
        projection[0][0] = 1.f / (aspect * tan(0.5f * fastgltf::math::radians(fov)));
        projection[1][1] = 1.f / (tan(0.5f * fastgltf::math::radians(fov)));
        projection[2][3] = -1;

        // Finite projection matrix
        projection[2][2] = (zfar + znear) / (znear - zfar);
        projection[3][2] = (2.f * zfar * znear) / (znear - zfar);
    }

    viewer->view_matrix = view_mat;
    viewer->projection_matrix = projection;
    viewer->view_projection_matrix = projection * view_mat;
    viewer->camera_pos = cam_position;
}

lv_result_t setup_restore_opaque_output(lv_gltf_view_desc_t * view_desc, gl_renwin_state_t _ret, uint32_t texture_w,
                                        uint32_t texture_h,
                                        bool prepare_bg)
{
    LV_LOG_USER("Color texture ID: %u, Depth texture ID: %u", _ret.texture, _ret.renderbuffer);

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, _ret.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _ret.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _ret.renderbuffer, 0));
    GL_CALL(glViewport(0, 0, texture_w, texture_h));
    if(prepare_bg) {
        GL_CALL(glClearColor(view_desc->bg_r / 255.0f, view_desc->bg_g / 255.0f, view_desc->bg_b / 255.0f,
                             view_desc->bg_a / 255.0f));
        //GL_CALL(glClearColor(208.0/255.0, 220.0/255.0, 230.0/255.0, 0.0f));
        GL_CALL(glClearDepth(1.0f));
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    return glGetError() == GL_NO_ERROR ? LV_RESULT_OK : LV_RESULT_INVALID;
}

void setup_draw_environment_background(lv_gl_shader_manager_t * manager, lv_gltf_view_t * viewer, float blur)
{
    GL_CALL(glBindVertexArray(manager->bg_vao));

    GL_CALL(glUseProgram(manager->bg_program));
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glDisable(GL_BLEND));
    GL_CALL(glDisable(GL_DEPTH_TEST));
    GL_CALL(glUniformMatrix4fv(glGetUniformLocation(manager->bg_program, "u_ViewProjectionMatrix"), 1, false,
                               viewer->view_projection_matrix.data()));
    //GL_CALL(glBindTextureUnit(0, shaders->lastEnv->specular));

    // Bind the texture to the specified texture unit
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 0)); // Activate the texture unit
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP,
                          viewer->env_textures.specular)); // Bind the texture (assuming 2D texture)

    GL_CALL(glUniform1i(glGetUniformLocation(manager->bg_program, "u_GGXEnvSampler"), 0));

    GL_CALL(glUniform1i(glGetUniformLocation(manager->bg_program, "u_MipCount"), viewer->env_textures.mipCount));
    GL_CALL(glUniform1f(glGetUniformLocation(manager->bg_program, "u_EnvBlurNormalized"), blur));
    GL_CALL(glUniform1f(glGetUniformLocation(manager->bg_program, "u_EnvIntensity"), 1.0f));
    GL_CALL(glUniform1f(glGetUniformLocation(manager->bg_program, "u_Exposure"), 1.0f));

    setup_environment_rotation_matrix(viewer->env_textures.angle, manager->bg_program);

    // Bind the index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, manager->bg_index_buf);

    // Bind the vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, manager->bg_vertex_buf);

    // Draw the elements
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void *)0);

    GL_CALL(glBindVertexArray(0));
    return;
}
void setup_environment_rotation_matrix(float env_rotation_angle, uint32_t shader_program)
{
    fastgltf::math::fmat3x3 rotmat =
        fastgltf::math::asMatrix(lv_gltf_math_euler_to_quartenion(env_rotation_angle, 0.f, 3.14159f));

    // Get the uniform location and set the uniform
    int32_t u_loc;
    GL_CALL(u_loc = glGetUniformLocation(shader_program, "u_EnvRotation"));
    GL_CALL(glUniformMatrix3fv(u_loc, 1, GL_FALSE, (const GLfloat *)rotmat.data()));
}

void setup_background_environment(GLuint program, GLuint * vao, GLuint * indexBuffer, GLuint * vertexBuffer)
{
    int32_t indices[] = { 1, 2, 0, 2, 3, 0, 6, 2, 1, 1, 5, 6, 6, 5, 4, 4, 7, 6,
                          6, 3, 2, 7, 3, 6, 3, 7, 0, 7, 4, 0, 5, 1, 0, 4, 5, 0
                        };
    float verts[] = { -1.0f, -1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f,
                      -1.0f, -1.0f, 1.0f,  1.0f, -1.0f, 1.0f,  1.0f, 1.0f, 1.0f,  -1.0f, 1.0f, 1.0f
                    };

    GL_CALL(glUseProgram(program));
    GL_CALL(glGenVertexArrays(1, vao));
    GL_CALL(glBindVertexArray(*vao));
    GL_CALL(glGenBuffers(1, indexBuffer));
    GL_CALL(glGenBuffers(1, vertexBuffer));

    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, *vertexBuffer));
    GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW));
    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *indexBuffer));
    GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

    GLint positionAttributeLocation = glGetAttribLocation(program, "a_position");

    // Specify the layout of the vertex data
    glVertexAttribPointer(positionAttributeLocation, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
    glEnableVertexAttribArray(positionAttributeLocation);

    GL_CALL(glBindVertexArray(0));
    GL_CALL(glUseProgram(0));
}

void setup_uniform_color_alpha(GLint uniform_loc, fastgltf::math::nvec4 color)
{
    GL_CALL(glUniform4f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]),
                        static_cast<float>(color[2]),
                        static_cast<float>(color[3])));
}

lv_result_t setup_restore_primary_output(lv_gltf_view_desc_t * view_desc, gl_renwin_state_t state, uint32_t texture_w,
                                         uint32_t texture_h,
                                         uint32_t texture_offset_w, uint32_t texture_offset_h, bool prepare_bg)
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, state.framebuffer));

    if(glGetError() != GL_NO_ERROR) {
        return LV_RESULT_INVALID;
    }
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, state.renderbuffer, 0));
    GL_CALL(glViewport(texture_offset_w, texture_offset_h, texture_w, texture_h));
    if(prepare_bg) {
        GL_CALL(glClearColor(view_desc->bg_r / 255.0f, view_desc->bg_g / 255.0f, view_desc->bg_b / 255.0f,
                             view_desc->bg_a / 255.0f));
        GL_CALL(glClearDepth(1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    return glGetError() == GL_NO_ERROR ? LV_RESULT_OK : LV_RESULT_INVALID;
}
void setup_uniform_color(GLint uniform_loc, fastgltf::math::nvec3 color)
{
    GL_CALL(glUniform3f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]),
                        static_cast<float>(color[2])));
}

uint32_t setup_texture(uint32_t tex_unit, uint32_t tex_name, int32_t tex_coord_index,
                       std::unique_ptr<fastgltf::TextureTransform> & tex_transform, GLint sampler, GLint uv_set,
                       GLint uv_transform)
{
    GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_unit)); // Activate the texture unit
    GL_CALL(glBindTexture(GL_TEXTURE_2D,
                          tex_name)); // Bind the texture (assuming 2D texture)
    GL_CALL(glUniform1i(sampler, tex_unit)); // Set the sampler to use the texture unit
    GL_CALL(glUniform1i(uv_set, tex_coord_index)); // Set the UV set index
    if(tex_transform != NULL)
        GL_CALL(glUniformMatrix3fv(uv_transform, 1, GL_FALSE, &(setup_texture_transform_matrix(*tex_transform)[0][0])));
    tex_unit++;
    return tex_unit;
}

fastgltf::math::fmat3x3 setup_texture_transform_matrix(fastgltf::TextureTransform transform)
{
    fastgltf::math::fmat3x3 rotation = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 scale = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 translation = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 result = fastgltf::math::fmat3x3(0.f);

    float s = std::sin(transform.rotation);
    float c = std::cos(transform.rotation);
    rotation[0][0] = c;
    rotation[1][1] = c;
    rotation[0][1] = s;
    rotation[1][0] = -s;
    rotation[2][2] = 1.0f;

    scale[0][0] = transform.uvScale[0];
    scale[1][1] = transform.uvScale[1];
    scale[2][2] = 1.0f;

    translation[0][0] = 1.0f;
    translation[1][1] = 1.0f;
    translation[0][2] = transform.uvOffset[0];
    translation[1][2] = transform.uvOffset[1];
    translation[2][2] = 1.0f;

    result = translation * rotation;
    result = result * scale;
    return result;
}

static void destroy_environment(lv_gltf_view_env_textures_t * env)
{
    const unsigned int d[3] = { env->diffuse, env->specular,
                                env->sheen
                              };
    GL_CALL(glDeleteTextures(3, d));
}


#endif /*LV_USE_GLTF*/
