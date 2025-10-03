/**
 * @file lv_gltf_view_render.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_view_internal.h"

#if LV_USE_GLTF

#include "../gltf_data/lv_gltf_data_internal.hpp"

#include "../fastgltf/lv_fastgltf.hpp"
#include "../../../misc/lv_types.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../drivers/opengles/lv_opengles_private.h"
#include "../../../drivers/opengles/lv_opengles_debug.h"
#include "../math/lv_gltf_math.hpp"

#include <algorithm>

#include <fastgltf/types.hpp>
#include <fastgltf/tools.hpp>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static GLuint lv_gltf_view_render_model(lv_gltf_t * viewer, lv_gltf_model_t * model, bool prepare_bg);
static void lv_gltf_view_push_opengl_state(lv_opengl_state_t * state);
static void lv_gltf_view_pop_opengl_state(const lv_opengl_state_t * state);
static void setup_finish_frame(void);
static void render_materials(lv_gltf_t * viewer, lv_gltf_model_t * gltf_data, const MaterialIndexMap & map);
static void render_skins(lv_gltf_t * viewer, lv_gltf_model_t * gltf_data);
static lv_result_t render_primary_output(lv_gltf_t * viewer, const lv_gltf_renwin_state_t * state,
                                         int32_t texture_w,
                                         int32_t texture_h, bool prepare_bg);

static void lv_gltf_view_recache_all_transforms(lv_gltf_model_t * gltf_data);
static fastgltf::math::fmat3x3 create_texture_transform_matrix(std::unique_ptr<fastgltf::TextureTransform> & transform);
static void render_uniform_color_alpha(GLint uniform_loc, fastgltf::math::nvec4 color);
static void render_uniform_color(GLint uniform_loc, fastgltf::math::nvec3 color);
static uint32_t render_texture(uint32_t tex_unit, uint32_t tex_name, int32_t tex_coord_index,
                               std::unique_ptr<fastgltf::TextureTransform> & tex_transform, GLint sampler, GLint uv_set,
                               GLint uv_transform);
static void draw_primitive(int32_t prim_num, lv_gltf_t * viewer, lv_gltf_model_t * gltf_data, fastgltf::Node & node,
                           std::size_t mesh_index, const fastgltf::math::fmat4x4 & matrix,
                           const lv_gltf_view_env_textures_t * env_tex, bool is_transmission_pass);

static void setup_primitive(int32_t prim_num, lv_gltf_t * viewer, lv_gltf_model_t * gltf_data,
                            fastgltf::Node & node,
                            std::size_t mesh_index, const fastgltf::math::fmat4x4 & matrix,
                            const lv_gltf_view_env_textures_t * env_tex, bool is_transmission_pass);

static void draw_material(lv_gltf_t * viewer, const lv_gltf_uniform_locations_t * uniforms, lv_gltf_model_t * model,
                          lv_gltf_primitive_t * _prim_data, size_t materialIndex, bool is_transmission_pass, GLuint program,
                          uint32_t * tex_num);

static void draw_lights(lv_gltf_model_t * model, GLuint program);

static lv_gltf_renwin_state_t setup_opaque_output(uint32_t texture_width, uint32_t texture_height);
static void setup_cleanup_opengl_output(lv_gltf_renwin_state_t * state);
static lv_gltf_renwin_state_t setup_primary_output(int32_t texture_width, int32_t texture_height, bool mipmaps_enabled);

static void setup_view_proj_matrix_from_camera(lv_gltf_t * viewer, uint32_t camera,
                                               lv_gltf_view_desc_t * view_desc,
                                               fastgltf::math::fmat4x4 view_mat, fastgltf::math::fvec3 view_pos,
                                               lv_gltf_model_t * gltf_data, bool transmission_pass);

static void setup_view_proj_matrix(lv_gltf_t * viewer, lv_gltf_view_desc_t * view_desc, lv_gltf_model_t * gltf_data,
                                   bool transmission_pass);
static lv_result_t setup_restore_opaque_output(lv_gltf_t * viewer, const lv_gltf_renwin_state_t * _ret,
                                               uint32_t texture_w,
                                               uint32_t texture_h, bool prepare_bg);
static void setup_draw_environment_background(lv_opengl_shader_manager_t * manager, lv_gltf_t * viewer, float blur);
static void setup_environment_rotation_matrix(float env_rotation_angle, uint32_t shader_program);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

GLuint lv_gltf_view_render(lv_gltf_t * viewer)
{
    const size_t n = lv_array_size(&viewer->models);

    if(n == 0) {
        return GL_NONE;
    }
    lv_gltf_model_t * model = *(lv_gltf_model_t **)lv_array_at(&viewer->models, 0);

    GLuint texture_id = GL_NONE;
    texture_id = lv_gltf_view_render_model(viewer, model, true);
    for(size_t i = 1; i < n; ++i) {
        lv_gltf_model_t * model = *(lv_gltf_model_t **)lv_array_at(&viewer->models, i);
        lv_gltf_view_render_model(viewer, model, false);
    }
    return texture_id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gltf_view_push_opengl_state(lv_opengl_state_t * state)
{
    GL_CALL(glGetBooleanv(GL_BLEND, &state->blend_enabled));
    GL_CALL(glGetIntegerv(GL_BLEND_SRC_ALPHA, &state->blend_src));
    GL_CALL(glGetIntegerv(GL_BLEND_DST_ALPHA, &state->blend_dst));
    GL_CALL(glGetIntegerv(GL_BLEND_EQUATION, &state->blend_equation));
    GL_CALL(glGetFloatv(GL_COLOR_CLEAR_VALUE, state->clear_color));
    GL_CALL(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &state->clear_depth));
}

static void lv_gltf_view_pop_opengl_state(const lv_opengl_state_t * state)
{
    GL_CALL(glDisable(GL_CULL_FACE));
    if(state->blend_enabled) {
        GL_CALL(glEnable(GL_BLEND));
    }
    else {
        GL_CALL(glDisable(GL_BLEND));
    }
    GL_CALL(glBlendFunc(state->blend_src, state->blend_dst));
    GL_CALL(glBlendEquation(state->blend_equation));
    GL_CALL(glDepthMask(GL_TRUE));
    GL_CALL(glClearColor(state->clear_color[0], state->clear_color[1], state->clear_color[2], state->clear_color[3]));
    GL_CALL(glClearDepthf(state->clear_depth));
}

static GLuint lv_gltf_view_render_model(lv_gltf_t * viewer, lv_gltf_model_t * model, bool prepare_bg)
{
    lv_gltf_view_state_t * vstate = &viewer->state;
    lv_gltf_view_desc_t * view_desc = &viewer->desc;
    bool opt_draw_bg = prepare_bg && (view_desc->bg_mode == LV_GLTF_BG_MODE_ENVIRONMENT);
    bool opt_aa_this_frame = (view_desc->aa_mode == LV_GLTF_AA_MODE_ON) ||
                             (view_desc->aa_mode == LV_GLTF_AA_MODE_DYNAMIC && model->last_frame_no_motion == true);
    if(prepare_bg == false) {
        /* If this data object is a secondary render pass, inherit the anti-alias setting for this frame from the first gltf_data drawn*/
        opt_aa_this_frame = view_desc->frame_was_antialiased;
    }

    lv_opengl_state_t opengl_state;
    lv_gltf_view_push_opengl_state(&opengl_state);

    int32_t last_render_w = view_desc->render_width;
    int32_t last_render_h = view_desc->render_height;
    view_desc->render_width = lv_obj_get_width((lv_obj_t *)viewer) * (opt_aa_this_frame ? 2 : 1);
    view_desc->render_height = lv_obj_get_height((lv_obj_t *)viewer) * (opt_aa_this_frame ? 2 : 1);

    bool new_size = last_render_h != view_desc->render_height || last_render_w != view_desc->render_width;

    if(opt_aa_this_frame != model->last_frame_was_antialiased) {
        /* Antialiasing state has changed since the last render */
        if(prepare_bg == true) {
            if(vstate->render_state_ready) {
                setup_cleanup_opengl_output(&vstate->render_state);
                vstate->render_state = setup_primary_output((uint32_t)view_desc->render_width,
                                                            (uint32_t)view_desc->render_height,
                                                            opt_aa_this_frame);
            }
        }
        model->last_frame_was_antialiased = opt_aa_this_frame;
    }

    view_desc->frame_was_antialiased = opt_aa_this_frame;

    if(new_size || !vstate->render_state_ready) {
        vstate->render_state_ready = true;
        vstate->render_state =
            setup_primary_output(view_desc->render_width, view_desc->render_height, opt_aa_this_frame);
        setup_finish_frame();
    }
    if(vstate->render_opaque_buffer) {
        vstate->opaque_render_state =
            setup_opaque_output(vstate->opaque_frame_buffer_width, vstate->opaque_frame_buffer_height);
        setup_finish_frame();
    }

    bool dirty = lv_memcmp(&viewer->last_desc, view_desc, sizeof(*view_desc)) != 0 || model->is_animation_enabled;

    lv_memcpy(&(viewer->last_desc), view_desc, sizeof(*view_desc));

    bool last_frame_no_motion = model->_last_frame_no_motion;
    model->_last_frame_no_motion = model->last_frame_no_motion;
    model->last_frame_no_motion = true;


    if(dirty || lv_gltf_data_transform_cache_is_empty(model) || (model->camera != model->last_camera_index)) {
        model->last_frame_no_motion = false;
        lv_gltf_view_recache_all_transforms(model);
    }
    else if(model->last_frame_no_motion && model->_last_frame_no_motion && last_frame_no_motion) {
        /* Nothing changed at all, return the previous output frame */
        setup_finish_frame();
        lv_gltf_view_pop_opengl_state(&opengl_state);
        return vstate->render_state.texture;
    }

    render_skins(viewer, model);

    NodeDistanceVector distance_sort_nodes;

    for(const auto & kv : model->blended_nodes_by_material_index) {
        for(const auto & pair : kv.second) {
            auto node = pair.first;
            auto new_node = NodeIndexDistancePair(
                                fastgltf::math::length(
                                    model->view_pos -
                                    lv_gltf_data_get_centerpoint(model, lv_gltf_data_get_cached_transform(model, node),
                                                                 node->meshIndex.value(), pair.second)),
                                NodeIndexPair(node, pair.second));
            distance_sort_nodes.push_back(new_node);
        }
    }
    std::sort(distance_sort_nodes.begin(), distance_sort_nodes.end(),
    [](const NodeIndexDistancePair & a, const NodeIndexDistancePair & b) {
        return a.first < b.first;
    });
    /* Reset the last material index to an unused value once per frame at the start*/
    model->last_material_index = 99999;
    if(vstate->render_opaque_buffer) {
        if(model->camera > 0) {
            setup_view_proj_matrix_from_camera(viewer, model->camera - 1, view_desc, model->view_mat,
                                               model->view_pos, model, true);
        }
        else {
            setup_view_proj_matrix(viewer, view_desc, model, true);
        }
        lv_result_t result = setup_restore_opaque_output(viewer, &vstate->opaque_render_state,
                                                         vstate->opaque_frame_buffer_width,
                                                         vstate->opaque_frame_buffer_height, prepare_bg);
        LV_ASSERT_MSG(result == LV_RESULT_OK, "Failed to setup opaque output which should never happen");
        if(result != LV_RESULT_OK) {
            lv_gltf_view_pop_opengl_state(&opengl_state);
            return vstate->render_state.texture;
        }

        if(opt_draw_bg) {
            setup_draw_environment_background(&viewer->shader_manager, viewer, view_desc->blur_bg);
        }

        render_materials(viewer, model, model->opaque_nodes_by_material_index);

        for(const auto & node_distance_pair : distance_sort_nodes) {
            const auto & node_element = node_distance_pair.second;
            const auto & node = node_element.first;
            draw_primitive(node_element.second, viewer, model, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(model, node), &viewer->env_textures, true);
        }

        GL_CALL(glBindTexture(GL_TEXTURE_2D, vstate->opaque_render_state.texture));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
        setup_finish_frame();
    }

    if(model->camera > 0) {
        setup_view_proj_matrix_from_camera(viewer, model->camera - 1, view_desc, model->view_mat,
                                           model->view_pos, model, false);
    }
    else {
        setup_view_proj_matrix(viewer, view_desc, model, false);
    }

    lv_result_t result = render_primary_output(viewer, &vstate->render_state, view_desc->render_width,
                                               view_desc->render_height, prepare_bg);

    LV_ASSERT_MSG(result == LV_RESULT_OK, "Failed to restore primary output which should never happen");
    if(result != LV_RESULT_OK) {
        lv_gltf_view_pop_opengl_state(&opengl_state);
        return vstate->render_state.texture;
    }
    if(opt_draw_bg)
        setup_draw_environment_background(&viewer->shader_manager, viewer, view_desc->blur_bg);
    render_materials(viewer, model, model->opaque_nodes_by_material_index);

    for(const auto & node_distance_pair : distance_sort_nodes) {
        const auto & node_element = node_distance_pair.second;
        const auto & node = node_element.first;
        draw_primitive(node_element.second, viewer, model, *node, node->meshIndex.value(),
                       lv_gltf_data_get_cached_transform(model, node), &viewer->env_textures, false);
    }
    if(opt_aa_this_frame) {
        GL_CALL(glBindTexture(GL_TEXTURE_2D, vstate->render_state.texture));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
    }
    setup_finish_frame();
    lv_gltf_view_pop_opengl_state(&opengl_state);
    return vstate->render_state.texture;
}

static void setup_finish_frame(void)
{
    GL_CALL(glDisable(GL_DEPTH_TEST));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_CALL(glUseProgram(0));
}

static void render_materials(lv_gltf_t * viewer, lv_gltf_model_t * gltf_data, const MaterialIndexMap & map)
{
    for(const auto & kv : map) {
        for(const auto & pair : kv.second) {
            auto node = pair.first;
            draw_primitive(pair.second, viewer, gltf_data, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(gltf_data, node), &viewer->env_textures, true);
        }
    }
}

static void render_skins(lv_gltf_t * viewer, lv_gltf_model_t * model)
{
    uint32_t skin_count = lv_gltf_data_get_skins_size(model);
    if(skin_count == 0) {
        return;
    }
    lv_gltf_data_destroy_textures(model);
    for(size_t i = 0; i < skin_count; ++i) {
        const auto & skin_index = lv_gltf_data_get_skin(model, i);
        const auto & skin = model->asset.skins[skin_index];
        auto & ibm = viewer->ibm_by_skin_then_node[skin_index];

        size_t num_joints = skin.joints.size();
        size_t tex_width = std::ceil(std::sqrt((float)num_joints * 8.0f));

        GLuint rtex = lv_gltf_data_create_texture(model);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, rtex));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        float * texture_data = (float *)lv_malloc(tex_width * tex_width * 4 * sizeof(*texture_data));
        LV_ASSERT_MALLOC(texture_data);
        size_t texture_data_index = 0;

        for(uint64_t j = 0; j < num_joints; j++) {
            auto & joint_node = model->asset.nodes[skin.joints[j]];
            fastgltf::math::fmat4x4 final_joint_matrix =
                lv_gltf_data_get_cached_transform(model, &joint_node) * ibm[&joint_node];

            lv_memcpy(&texture_data[texture_data_index], final_joint_matrix.data(), sizeof(float) * 16);
            lv_memcpy(&texture_data[texture_data_index + 16],
                      fastgltf::math::transpose(fastgltf::math::invert(final_joint_matrix)).data(),
                      sizeof(float) * 16);

            texture_data_index += 32;
        }
        GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, tex_width, tex_width, 0, GL_RGBA, GL_FLOAT, texture_data));
        GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
        lv_free(texture_data);
    }
}
static void draw_primitive(int32_t prim_num, lv_gltf_t * viewer, lv_gltf_model_t * gltf_data, fastgltf::Node & node,
                           std::size_t mesh_index, const fastgltf::math::fmat4x4 & matrix,
                           const lv_gltf_view_env_textures_t * env_tex, bool is_transmission_pass)
{
    lv_gltf_mesh_data_t * mesh = lv_gltf_data_get_mesh(gltf_data, mesh_index);
    const auto & asset = lv_gltf_data_get_asset(gltf_data);
    const auto & _prim_data = lv_gltf_data_get_primitive_from_mesh(mesh, prim_num);

    std::size_t index_count = 0;
    auto & indexAccessor = asset->accessors[asset->meshes[mesh_index].primitives[prim_num].indicesAccessor.value()];
    setup_primitive(prim_num, viewer, gltf_data, node, mesh_index, matrix, env_tex, is_transmission_pass);

    if(indexAccessor.bufferViewIndex.has_value()) {
        index_count = (uint32_t)indexAccessor.count;
    }
    if(index_count > 0) {
        GL_CALL(glDrawElements(_prim_data->primitiveType, index_count, _prim_data->indexType, 0));
    }
}
static void setup_primitive(int32_t prim_num, lv_gltf_t * viewer, lv_gltf_model_t * model, fastgltf::Node & node,
                            std::size_t mesh_index, const fastgltf::math::fmat4x4 & matrix,
                            const lv_gltf_view_env_textures_t * env_tex, bool is_transmission_pass)
{
    lv_gltf_view_desc_t * view_desc = &viewer->desc;
    lv_gltf_mesh_data_t * mesh = lv_gltf_data_get_mesh(model, mesh_index);
    const auto & _prim_data = lv_gltf_data_get_primitive_from_mesh(mesh, prim_num);
    auto & _prim_gltf_data = model->asset.meshes[mesh_index].primitives[prim_num];
    auto & mappings = _prim_gltf_data.mappings;
    std::size_t materialIndex =
        (!mappings.empty() && mappings[viewer->state.material_variant].has_value()) ?
        mappings[viewer->state.material_variant].value() + 1 :
        ((_prim_gltf_data.materialIndex.has_value()) ? (_prim_gltf_data.materialIndex.value() + 1) : 0);

    GL_CALL(glBindVertexArray(_prim_data->vertexArray));

    lv_gltf_compiled_shader_t * compiled_shader = lv_gltf_get_compiled_shader(model, materialIndex);
    const lv_gltf_uniform_locations_t * uniforms = &compiled_shader->uniforms;

    /* Fast path, primitive setup in the primitive draw render */
    if((model->last_material_index == materialIndex) && (model->last_pass_was_transmission == is_transmission_pass)) {
        GL_CALL(glUniformMatrix4fv(uniforms->model_matrix, 1, GL_FALSE, &matrix[0][0]));
        return;
    }

    model->last_material_index = materialIndex;
    model->last_pass_was_transmission = is_transmission_pass;

    const GLuint program = compiled_shader->shaderset.program;

    GL_CALL(glUseProgram(program));

    GL_CALL(glUniformMatrix4fv(uniforms->model_matrix, 1, GL_FALSE, &matrix[0][0]));
    GL_CALL(glUniformMatrix4fv(uniforms->view_matrix, 1, false, viewer->view_matrix.data()));
    GL_CALL(glUniformMatrix4fv(uniforms->projection_matrix, 1, false, viewer->projection_matrix.data()));
    GL_CALL(glUniformMatrix4fv(uniforms->view_projection_matrix, 1, false, viewer->view_projection_matrix.data()));
    const auto & _campos = viewer->camera_pos;
    GL_CALL(glUniform3f(uniforms->camera, _campos[0], _campos[1], _campos[2]));

    GL_CALL(glUniform1f(uniforms->exposure, view_desc->exposure));
    GL_CALL(glUniform1f(uniforms->env_intensity, view_desc->env_pow));
    GL_CALL(glUniform1i(uniforms->env_mip_count, (int32_t)env_tex->mip_count));
    setup_environment_rotation_matrix(viewer->env_textures.angle, program);
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glDisable(GL_BLEND));
    GL_CALL(glEnable(GL_DEPTH_TEST));
    GL_CALL(glDepthMask(GL_TRUE));
    GL_CALL(glCullFace(GL_BACK));
    uint32_t tex_num = 0;

    draw_material(viewer, uniforms, model, _prim_data, materialIndex, is_transmission_pass, program, &tex_num);

    const lv_gltf_view_state_t * vstate = &viewer->state;
    if(!is_transmission_pass && vstate->render_opaque_buffer) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_num));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, vstate->opaque_render_state.texture));
        GL_CALL(glUniform1i(uniforms->transmission_framebuffer_sampler, tex_num));
        GL_CALL(glUniform2i(uniforms->transmission_framebuffer_size, (int32_t)vstate->opaque_frame_buffer_width,
                            (int32_t)vstate->opaque_frame_buffer_height));
        tex_num++;
    }

    if(node.skinIndex.has_value()) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_num));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, lv_gltf_data_get_skin_texture_at(model, node.skinIndex.value())));
        GL_CALL(glUniform1i(uniforms->joints_sampler, tex_num));
        tex_num++;
    }
    if(env_tex->diffuse != GL_NONE) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_num));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex->diffuse));
        GL_CALL(glUniform1i(uniforms->env_diffuse_sampler, tex_num++));
    }
    if(env_tex->specular != GL_NONE) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_num));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex->specular));
        GL_CALL(glUniform1i(uniforms->env_specular_sampler, tex_num++));
    }
    if(env_tex->sheen != GL_NONE) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_num));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex->sheen));
        GL_CALL(glUniform1i(uniforms->env_sheen_sampler, tex_num++));
    }
    if(env_tex->ggxLut != GL_NONE) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_num));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, env_tex->ggxLut));
        GL_CALL(glUniform1i(uniforms->env_ggx_lut_sampler, tex_num++));
    }
    if(env_tex->charlie_lut != GL_NONE) {
        GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_num));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, env_tex->charlie_lut));
        GL_CALL(glUniform1i(uniforms->env_charlie_lut_sampler, tex_num++));
    }
}

static void draw_material(lv_gltf_t * viewer, const lv_gltf_uniform_locations_t * uniforms, lv_gltf_model_t * model,
                          lv_gltf_primitive_t * _prim_data, size_t materialIndex, bool is_transmission_pass, GLuint program,
                          uint32_t * tex_num)
{
    const auto & asset = lv_gltf_data_get_asset(model);

    bool has_material = asset->materials.size() > (materialIndex - 1);

    if(!has_material) {
        render_uniform_color_alpha(uniforms->base_color_factor, fastgltf::math::fvec4(1.0f));
        GL_CALL(glUniform1f(uniforms->roughness_factor, 0.5f));
        GL_CALL(glUniform1f(uniforms->metallic_factor, 0.5f));
        GL_CALL(glUniform1f(uniforms->ior, 1.5f));
        GL_CALL(glUniform1f(uniforms->dispersion, 0.0f));
        GL_CALL(glUniform1f(uniforms->thickness, 0.01847f));
        return;
    }

    auto & gltfMaterial = asset->materials[materialIndex - 1];

    if(is_transmission_pass && (gltfMaterial.transmission != NULL)) {
        return;
    }

    if(gltfMaterial.doubleSided)
        GL_CALL(glDisable(GL_CULL_FACE));
    if(gltfMaterial.alphaMode == fastgltf::AlphaMode::Blend) {
        GL_CALL(glEnable(GL_BLEND));
        GL_CALL(glDepthMask(GL_FALSE));
        GL_CALL(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA));
        GL_CALL(glBlendEquation(GL_FUNC_ADD));
        GL_CALL(glEnable(GL_CULL_FACE));
    }
    else {
        if(gltfMaterial.alphaMode == fastgltf::AlphaMode::Mask) {
            GL_CALL(glUniform1f(uniforms->alpha_cutoff, gltfMaterial.alphaCutoff));
            GL_CALL(glDisable(GL_CULL_FACE));
        }
    }

    draw_lights(model, program);
    render_uniform_color_alpha(uniforms->base_color_factor, gltfMaterial.pbrData.baseColorFactor);
    render_uniform_color(uniforms->emissive_factor, gltfMaterial.emissiveFactor);

    GL_CALL(glUniform1f(uniforms->emissive_strength, gltfMaterial.emissiveStrength));
    GL_CALL(glUniform1f(uniforms->roughness_factor, gltfMaterial.pbrData.roughnessFactor));
    GL_CALL(glUniform1f(uniforms->metallic_factor, gltfMaterial.pbrData.metallicFactor));
    GL_CALL(glUniform1f(uniforms->ior, gltfMaterial.ior));
    GL_CALL(glUniform1f(uniforms->dispersion, gltfMaterial.dispersion));

    if(gltfMaterial.pbrData.baseColorTexture.has_value())
        *tex_num = render_texture(*tex_num, _prim_data->albedoTexture, _prim_data->baseColorTexcoordIndex,
                                  gltfMaterial.pbrData.baseColorTexture->transform, uniforms->base_color_sampler,
                                  uniforms->base_color_uv_set, uniforms->base_color_uv_transform);
    if(gltfMaterial.emissiveTexture.has_value())
        *tex_num = render_texture(*tex_num, _prim_data->emissiveTexture, _prim_data->emissiveTexcoordIndex,
                                  gltfMaterial.emissiveTexture->transform, uniforms->emissive_sampler,
                                  uniforms->emissive_uv_set, uniforms->emissive_uv_transform);
    if(gltfMaterial.pbrData.metallicRoughnessTexture.has_value())
        *tex_num = render_texture(*tex_num, _prim_data->metalRoughTexture, _prim_data->metallicRoughnessTexcoordIndex,
                                  gltfMaterial.pbrData.metallicRoughnessTexture->transform,
                                  uniforms->metallic_roughness_sampler, uniforms->metallic_roughness_uv_set,
                                  uniforms->metallic_roughness_uv_transform);
    if(gltfMaterial.occlusionTexture.has_value()) {
        GL_CALL(glUniform1f(uniforms->occlusion_strength, static_cast<float>(gltfMaterial.occlusionTexture->strength)));
        *tex_num = render_texture(*tex_num, _prim_data->occlusionTexture, _prim_data->occlusionTexcoordIndex,
                                  gltfMaterial.occlusionTexture->transform, uniforms->occlusion_sampler,
                                  uniforms->occlusion_uv_set, uniforms->occlusion_uv_transform);
    }

    if(gltfMaterial.normalTexture.has_value()) {
        GL_CALL(glUniform1f(uniforms->normal_scale, static_cast<float>(gltfMaterial.normalTexture->scale)));
        *tex_num = render_texture(*tex_num, _prim_data->normalTexture, _prim_data->normalTexcoordIndex,
                                  gltfMaterial.normalTexture->transform, uniforms->normal_sampler,
                                  uniforms->normal_uv_set, uniforms->normal_uv_transform);
    }

    if(gltfMaterial.clearcoat) {
        GL_CALL(glUniform1f(uniforms->clearcoat_factor, static_cast<float>(gltfMaterial.clearcoat->clearcoatFactor)));
        GL_CALL(glUniform1f(uniforms->clearcoat_roughness_factor,
                            static_cast<float>(gltfMaterial.clearcoat->clearcoatRoughnessFactor)));

        if(gltfMaterial.clearcoat->clearcoatTexture.has_value())
            *tex_num = render_texture(*tex_num, _prim_data->clearcoatTexture, _prim_data->clearcoatTexcoordIndex,
                                      gltfMaterial.clearcoat->clearcoatTexture->transform,
                                      uniforms->clearcoat_sampler, uniforms->clearcoat_uv_set,
                                      uniforms->clearcoat_uv_transform);
        if(gltfMaterial.clearcoat->clearcoatRoughnessTexture.has_value())
            *tex_num = render_texture(*tex_num, _prim_data->clearcoatRoughnessTexture,
                                      _prim_data->clearcoatRoughnessTexcoordIndex,
                                      gltfMaterial.clearcoat->clearcoatRoughnessTexture->transform,
                                      uniforms->clearcoat_roughness_sampler, uniforms->clearcoat_roughness_uv_set,
                                      uniforms->clearcoat_roughness_uv_transform);
        if(gltfMaterial.clearcoat->clearcoatNormalTexture.has_value()) {
            GL_CALL(glUniform1f(uniforms->clearcoat_normal_scale,
                                static_cast<float>(gltfMaterial.clearcoat->clearcoatNormalTexture->scale)));
            *tex_num = render_texture(*tex_num, _prim_data->clearcoatNormalTexture,
                                      _prim_data->clearcoatNormalTexcoordIndex,
                                      gltfMaterial.clearcoat->clearcoatNormalTexture->transform,
                                      uniforms->clearcoat_normal_sampler, uniforms->clearcoat_normal_uv_set,
                                      uniforms->clearcoat_normal_uv_transform);
        }
    }

    if(gltfMaterial.volume) {
        GL_CALL(glUniform1f(uniforms->attenuation_distance, gltfMaterial.volume->attenuationDistance));
        render_uniform_color(uniforms->attenuation_color, gltfMaterial.volume->attenuationColor);
        GL_CALL(glUniform1f(uniforms->thickness, gltfMaterial.volume->thicknessFactor));
        if(gltfMaterial.volume->thicknessTexture.has_value()) {
            *tex_num = render_texture(*tex_num, _prim_data->thicknessTexture, _prim_data->thicknessTexcoordIndex,
                                      gltfMaterial.volume->thicknessTexture->transform, uniforms->thickness_sampler,
                                      uniforms->thickness_uv_set, uniforms->thickness_uv_transform);
        }
    }

    if(gltfMaterial.transmission) {
        GL_CALL(glUniform1f(uniforms->transmission_factor, gltfMaterial.transmission->transmissionFactor));
        GL_CALL(glUniform2i(uniforms->screen_size, viewer->desc.render_width, viewer->desc.render_height));
        if(gltfMaterial.transmission->transmissionTexture.has_value())
            *tex_num = render_texture(*tex_num, _prim_data->transmissionTexture,
                                      _prim_data->transmissionTexcoordIndex,
                                      gltfMaterial.transmission->transmissionTexture->transform,
                                      uniforms->transmission_sampler, uniforms->transmission_uv_set,
                                      uniforms->transmission_uv_transform);
    }

    if(gltfMaterial.sheen) {
        render_uniform_color(uniforms->sheen_color_factor, gltfMaterial.sheen->sheenColorFactor);
        GL_CALL(glUniform1f(uniforms->sheen_roughness_factor,
                            static_cast<float>(gltfMaterial.sheen->sheenRoughnessFactor)));
        if(gltfMaterial.sheen->sheenColorTexture.has_value()) {
            LV_LOG_WARN("Material has unhandled sheen texture");
        }
    }
    if(gltfMaterial.specular) {
        render_uniform_color(uniforms->specular_color_factor, gltfMaterial.specular->specularColorFactor);
        GL_CALL(glUniform1f(uniforms->specular_factor, static_cast<float>(gltfMaterial.specular->specularFactor)));
        if(gltfMaterial.specular->specularTexture.has_value()) {
            LV_LOG_WARN("Material has unhandled specular texture");
        }
        if(gltfMaterial.specular->specularColorTexture.has_value()) {
            LV_LOG_WARN("Material has unhandled specular color texture");
        }
    }

#if FASTGLTF_ENABLE_DEPRECATED_EXT
    if(gltfMaterial.specularGlossiness) {
        LV_LOG_WARN(
            "Model uses outdated legacy mode pbr_speculargloss. Please update this model to a new shading model ");
        render_uniform_color_alpha(uniforms->diffuse_factor, gltfMaterial.specularGlossiness->diffuseFactor);
        render_uniform_color(uniforms->specular_factor, gltfMaterial.specularGlossiness->specularFactor);
        GL_CALL(glUniform1f(uniforms->glossiness_factor,
                            static_cast<float>(gltfMaterial.specularGlossiness->glossinessFactor)));
        if(gltfMaterial.specularGlossiness->diffuseTexture.has_value()) {
            *tex_num = render_texture(*tex_num, _prim_data->diffuseTexture, _prim_data->diffuseTexcoordIndex,
                                      gltfMaterial.specularGlossiness->diffuseTexture->transform,
                                      uniforms->diffuse_sampler, uniforms->diffuse_uv_set,
                                      uniforms->diffuse_uv_transform);
        }
        if(gltfMaterial.specularGlossiness->specularGlossinessTexture.has_value()) {
            *tex_num = render_texture(*tex_num, _prim_data->specularGlossinessTexture,
                                      _prim_data->specularGlossinessTexcoordIndex,
                                      gltfMaterial.specularGlossiness->specularGlossinessTexture->transform,
                                      uniforms->specular_glossiness_sampler, uniforms->specular_glossiness_uv_set,
                                      uniforms->specular_glossiness_uv_transform);
        }
    }
#endif

    if(gltfMaterial.diffuseTransmission) {
        render_uniform_color(uniforms->diffuse_transmission_color_factor,
                             gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor);
        GL_CALL(glUniform1f(uniforms->diffuse_transmission_factor,
                            static_cast<float>(gltfMaterial.diffuseTransmission->diffuseTransmissionFactor)));
        if(gltfMaterial.diffuseTransmission->diffuseTransmissionTexture.has_value()) {
            *tex_num = render_texture(*tex_num, _prim_data->diffuseTransmissionTexture,
                                      _prim_data->diffuseTransmissionTexcoordIndex,
                                      gltfMaterial.diffuseTransmission->diffuseTransmissionTexture->transform,
                                      uniforms->diffuse_transmission_sampler, uniforms->diffuse_transmission_uv_set,
                                      uniforms->diffuse_transmission_uv_transform);
        }
        if(gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture.has_value()) {
            *tex_num = render_texture(*tex_num, _prim_data->diffuseTransmissionColorTexture,
                                      _prim_data->diffuseTransmissionColorTexcoordIndex,
                                      gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture->transform,
                                      uniforms->diffuse_transmission_color_sampler,
                                      uniforms->diffuse_transmission_color_uv_set,
                                      uniforms->diffuse_transmission_color_uv_transform);
        }
    }
}
static void draw_lights(lv_gltf_model_t * model, GLuint program)
{
    if(model->node_by_light_index.empty()) {
        return;
    }
    size_t max_light_nodes = model->node_by_light_index.size();
    size_t max_scene_lights = model->asset.lights.size();
    if(max_scene_lights != max_light_nodes) {
        LV_LOG_ERROR("Scene light count (%zu) != scene light node count (%zu)\n", max_scene_lights, max_light_nodes);
        return;
    }

    char tag[100];
    char prefix[20];
    for(size_t i = 0; i < max_scene_lights; i++) {
        // Update each field of the light struct
        lv_snprintf(prefix, sizeof(prefix), "u_Lights[%zu]", i + 1);
        auto & lightNode = model->node_by_light_index[i];
        const fastgltf::math::fmat4x4 & light_matrix = lv_gltf_data_get_cached_transform(model, lightNode);

        lv_snprintf(tag, sizeof(tag), "%s.position", prefix);
        glUniform3fv(glGetUniformLocation(program, tag), 1, &light_matrix[3][0]);

        lv_snprintf(tag, sizeof(tag), "%s.direction", prefix);
        float tlight_dir[3] = { -light_matrix[2][0], -light_matrix[2][1], -light_matrix[2][2] };

        glUniform3fv(glGetUniformLocation(program, tag), 1, &tlight_dir[0]);

        lv_snprintf(tag, sizeof(tag), "%s.range", prefix);
        const auto & m = light_matrix.data();

        if(model->asset.lights[i].range.has_value()) {
            float light_scale = fastgltf::math::length(fastgltf::math::fvec3(m[0], m[4], m[8]));
            glUniform1f(glGetUniformLocation(program, tag), model->asset.lights[i].range.value() * light_scale);
        }
        else {
            glUniform1f(glGetUniformLocation(program, tag), 9999.f);
        }

        lv_snprintf(tag, sizeof(tag), "%s.color", prefix);
        glUniform3fv(glGetUniformLocation(program, tag), 1, &(model->asset.lights[i].color.data()[0]));

        lv_snprintf(tag, sizeof(tag), "%s.intensity", prefix);
        glUniform1f(glGetUniformLocation(program, tag), model->asset.lights[i].intensity);

        lv_snprintf(tag, sizeof(tag), "%s.innerConeCos", prefix);
        if(model->asset.lights[i].innerConeAngle.has_value()) {
            glUniform1f(glGetUniformLocation(program, tag),
                        std::cos(model->asset.lights[i].innerConeAngle.value()));

        }
        else {
            glUniform1f(glGetUniformLocation(program, tag), -1.0f);
        }

        lv_snprintf(tag, sizeof(tag), "%s.outerConeCos", prefix);

        if(model->asset.lights[i].outerConeAngle.has_value()) {
            glUniform1f(glGetUniformLocation(program, tag),
                        std::cos(model->asset.lights[i].outerConeAngle.value()));

        }
        else {
            glUniform1f(glGetUniformLocation(program, tag), -1.0f);
        }
        lv_snprintf(tag, sizeof(tag), "%s.type", prefix);
        glUniform1i(glGetUniformLocation(program, tag), (GLint)model->asset.lights[i].type);
    }
}

lv_result_t render_primary_output(lv_gltf_t * viewer, const lv_gltf_renwin_state_t * state, int32_t texture_w,
                                  int32_t texture_h, bool prepare_bg)
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, state->framebuffer));

    if(glGetError() != GL_NO_ERROR) {
        return LV_RESULT_INVALID;
    }
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, state->texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, state->renderbuffer, 0));
    GL_CALL(glViewport(0, 0, texture_w, texture_h));
    if(prepare_bg) {
        /* cast is safe because viewer is a lv_obj_t*/
        lv_color_t bg_color = lv_obj_get_style_bg_color((lv_obj_t *)viewer, LV_PART_MAIN);
        uint8_t alpha = lv_obj_get_style_bg_opa((lv_obj_t *)viewer, LV_PART_MAIN);
        GL_CALL(glClearColor(bg_color.red / 255.0f, bg_color.green / 255.0f, bg_color.blue / 255.0f, alpha / 255.0f));

        GL_CALL(glClearDepthf(1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }

    return glGetError() == GL_NO_ERROR ? LV_RESULT_OK : LV_RESULT_INVALID;
}

static void render_uniform_color_alpha(GLint uniform_loc, fastgltf::math::nvec4 color)
{
    GL_CALL(glUniform4f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]),
                        static_cast<float>(color[2]), static_cast<float>(color[3])));
}
static void render_uniform_color(GLint uniform_loc, fastgltf::math::nvec3 color)
{
    GL_CALL(glUniform3f(uniform_loc, static_cast<float>(color[0]), static_cast<float>(color[1]),
                        static_cast<float>(color[2])));
}

static uint32_t render_texture(uint32_t tex_unit, uint32_t tex_name, int32_t tex_coord_index,
                               std::unique_ptr<fastgltf::TextureTransform> & tex_transform, GLint sampler, GLint uv_set,
                               GLint uv_transform)
{
    /* Activate the texture unit*/
    GL_CALL(glActiveTexture(GL_TEXTURE0 + tex_unit));
    /* Bind the texture (assuming 2D texture) */
    GL_CALL(glBindTexture(GL_TEXTURE_2D, tex_name));
    /* Set the sampler to use the texture unit */
    GL_CALL(glUniform1i(sampler, tex_unit));
    /* Set the UV set index */
    GL_CALL(glUniform1i(uv_set, tex_coord_index));
    if(tex_transform != NULL) {
        GL_CALL(glUniformMatrix3fv(uv_transform, 1, GL_FALSE, &(create_texture_transform_matrix(tex_transform)[0][0])));
    }

    tex_unit++;
    return tex_unit;
}

static fastgltf::math::fmat3x3 create_texture_transform_matrix(std::unique_ptr<fastgltf::TextureTransform> & transform)
{
    fastgltf::math::fmat3x3 rotation = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 scale = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 translation = fastgltf::math::fmat3x3(0.f);
    fastgltf::math::fmat3x3 result = fastgltf::math::fmat3x3(0.f);

    float s = std::sin(transform->rotation);
    float c = std::cos(transform->rotation);
    rotation[0][0] = c;
    rotation[1][1] = c;
    rotation[0][1] = s;
    rotation[1][0] = -s;
    rotation[2][2] = 1.0f;

    scale[0][0] = transform->uvScale[0];
    scale[1][1] = transform->uvScale[1];
    scale[2][2] = 1.0f;

    translation[0][0] = 1.0f;
    translation[1][1] = 1.0f;
    translation[0][2] = transform->uvOffset[0];
    translation[1][2] = transform->uvOffset[1];
    translation[2][2] = 1.0f;

    result = translation * rotation;
    result = result * scale;
    return result;
}

static lv_gltf_renwin_state_t setup_opaque_output(uint32_t texture_width, uint32_t texture_height)
{
    lv_gltf_renwin_state_t result;

    GLuint rtex;
    GL_CALL(glGenTextures(1, &rtex));
    result.texture = rtex;

    GL_CALL(glBindTexture(GL_TEXTURE_2D, result.texture));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
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
#ifdef __EMSCRIPTEN__
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, LV_GL_PREFERRED_DEPTH, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_INT, NULL));
#else
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, LV_GL_PREFERRED_DEPTH, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_SHORT, NULL));
#endif
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));

    GL_CALL(glGenFramebuffers(1, &result.framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, result.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.renderbuffer, 0));

    return result;
}

static lv_gltf_renwin_state_t setup_primary_output(int32_t texture_width, int32_t texture_height, bool mipmaps_enabled)
{
    lv_gltf_renwin_state_t result;

    GLuint rtex;
    GL_CALL(glGenTextures(1, &rtex));
    result.texture = rtex;
    GL_CALL(glBindTexture(GL_TEXTURE_2D, result.texture));
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
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, LV_GL_PREFERRED_DEPTH, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_INT, NULL));
#else
    // For Desktop OpenGL
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, LV_GL_PREFERRED_DEPTH, texture_width, texture_height, 0, GL_DEPTH_COMPONENT,
                         GL_UNSIGNED_SHORT, NULL));
#endif
    GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));

    GL_CALL(glGenFramebuffers(1, &result.framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, result.framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, result.texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, result.renderbuffer, 0));

    return result;
}

static void setup_cleanup_opengl_output(lv_gltf_renwin_state_t * state)
{
    if(!state) {
        return;
    }
    if(state->framebuffer) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
        GL_CALL(glDeleteFramebuffers(1, &state->framebuffer));
        state->framebuffer = 0;
    }
    if(state->texture) {
        GL_CALL(glDeleteTextures(1, &state->texture));
        state->texture = 0;
    }
    if(state->renderbuffer) {
        GL_CALL(glDeleteTextures(1, &state->renderbuffer));
        state->renderbuffer = 0;
    }
}
static void setup_view_proj_matrix_from_camera(lv_gltf_t * viewer, uint32_t camera,
                                               lv_gltf_view_desc_t * view_desc,
                                               fastgltf::math::fmat4x4 view_mat, fastgltf::math::fvec3 view_pos,
                                               lv_gltf_model_t * gltf_data, bool transmission_pass)
{
    /* The following matrix math is for the projection matrices as defined by the glTF spec:*/
    /* https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.html#projection-matrices*/

    fastgltf::math::fmat4x4 projection;
    const auto & asset = lv_gltf_data_get_asset(gltf_data);

    auto width = view_desc->render_width;
    auto height = view_desc->render_height;
    /* It's possible the transmission pass should simply use the regular passes aspect despite having different metrics itself. */
    /* TODO: test both ways to see which has less distortion*/

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
    asset->cameras[camera].camera);

    viewer->view_matrix = view_mat;
    viewer->projection_matrix = projection;
    viewer->view_projection_matrix = projection * view_mat;
    viewer->camera_pos = view_pos;
}

static void setup_view_proj_matrix(lv_gltf_t * viewer, lv_gltf_view_desc_t * view_desc, lv_gltf_model_t * gltf_data,
                                   bool transmission_pass)
{
    auto b_radius = lv_gltf_data_get_radius(gltf_data);
    float radius = b_radius * 2.5;
    radius *= view_desc->distance;

    fastgltf::math::fvec3 rcam_dir = fastgltf::math::fvec3(0.0f, 0.0f, 1.0f);

    fastgltf::math::fmat3x3 rotation1 =
        fastgltf::math::asMatrix(lv_gltf_math_euler_to_quaternion(0.f, 0.f, fastgltf::math::radians(view_desc->pitch)));
    fastgltf::math::fmat3x3 rotation2 =
        fastgltf::math::asMatrix(lv_gltf_math_euler_to_quaternion(fastgltf::math::radians(view_desc->yaw), 0.f, 0.f));

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

    float znear = b_radius * 0.05f;
    float zfar = b_radius * std::max(4.0, 8.0 * view_desc->distance);
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
        float orthoSize = view_desc->distance * b_radius; // Adjust as needed

        projection = fastgltf::math::fmat4x4(1.0f);
        projection[0][0] = -(orthoSize * aspect);
        projection[1][1] = (orthoSize);
        projection[2][2] = 2.f / (znear - zfar);
        projection[3][2] = (zfar + znear) / (znear - zfar);

    }
    else {
        // Perspective view
        projection = fastgltf::math::fmat4x4(0.0f);
        LV_ASSERT(width != 0 && height != 0);
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


static lv_result_t setup_restore_opaque_output(lv_gltf_t * viewer, const lv_gltf_renwin_state_t * renwin_state,
                                               uint32_t texture_w,
                                               uint32_t texture_h, bool prepare_bg)
{
    LV_LOG_TRACE("Color texture ID: %u, Depth texture ID: %u", renwin_state->texture, renwin_state->renderbuffer);

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, renwin_state->framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, renwin_state->texture, 0));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, renwin_state->renderbuffer, 0));
    GL_CALL(glViewport(0, 0, texture_w, texture_h));
    if(prepare_bg) {
        /* cast is safe because viewer is a lv_obj_t*/
        lv_color_t bg_color = lv_obj_get_style_bg_color((lv_obj_t *)viewer, LV_PART_MAIN);
        uint8_t alpha = lv_obj_get_style_bg_opa((lv_obj_t *)viewer, LV_PART_MAIN);
        GL_CALL(glClearColor(bg_color.red / 255.0f, bg_color.green / 255.0f, bg_color.blue / 255.0f, alpha / 255.0f));
        GL_CALL(glClearDepthf(1.0f));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
    }
    return glGetError() == GL_NO_ERROR ? LV_RESULT_OK : LV_RESULT_INVALID;
}

static void setup_draw_environment_background(lv_opengl_shader_manager_t * manager, lv_gltf_t * viewer, float blur)
{
    GL_CALL(glBindVertexArray(manager->bg_vao));

    GL_CALL(glUseProgram(manager->bg_program));
    GL_CALL(glEnable(GL_CULL_FACE));
    GL_CALL(glDisable(GL_BLEND));
    GL_CALL(glDisable(GL_DEPTH_TEST));
    GL_CALL(glUniformMatrix4fv(glGetUniformLocation(manager->bg_program, "u_ViewProjectionMatrix"), 1, false,
                               viewer->view_projection_matrix.data()));

    /* Bind the texture to the specified texture unit*/
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, viewer->env_textures.specular));

    GL_CALL(glUniform1i(glGetUniformLocation(manager->bg_program, "u_GGXEnvSampler"), 0));

    GL_CALL(glUniform1i(glGetUniformLocation(manager->bg_program, "u_MipCount"), viewer->env_textures.mip_count));
    GL_CALL(glUniform1f(glGetUniformLocation(manager->bg_program, "u_EnvBlurNormalized"), blur));
    GL_CALL(glUniform1f(glGetUniformLocation(manager->bg_program, "u_EnvIntensity"), 1.0f));
    GL_CALL(glUniform1f(glGetUniformLocation(manager->bg_program, "u_Exposure"), 1.0f));

    setup_environment_rotation_matrix(viewer->env_textures.angle, manager->bg_program);

    GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, manager->bg_index_buf));
    GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, manager->bg_vertex_buf));
    GL_CALL(glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL));

    GL_CALL(glBindVertexArray(0));
    return;
}
static void lv_gltf_view_recache_all_transforms(lv_gltf_model_t * gltf_data)
{
    const auto & asset = lv_gltf_data_get_asset(gltf_data);
    int32_t anim_num = gltf_data->current_animation;
    uint32_t scene_index = 0;

    gltf_data->last_camera_index = gltf_data->camera;
    size_t current_camera_count = 0;

    lv_gltf_data_clear_transform_cache(gltf_data);

    auto tmat = fastgltf::math::fmat4x4{};
    fastgltf::custom_iterate_scene_nodes(
        *asset, scene_index, &tmat,
    [&](fastgltf::Node & node, fastgltf::math::fmat4x4 & parentworldmatrix, fastgltf::math::fmat4x4 & localmatrix) {
        bool made_changes = false;
        bool made_rotation_changes = false;
        if(lv_gltf_data_animation_get_channel_set(anim_num, gltf_data, node)->size() > 0) {
            lv_gltf_data_animation_matrix_apply(gltf_data->local_timestamp / 1000., anim_num, gltf_data, node,
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

                    if(current_override->dir) {
                        current_override->data[0] = world_pos[0];
                        current_override->data[1] = world_pos[1];
                        current_override->data[2] = world_pos[2];
                    }
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
                                                     lv_gltf_math_euler_to_quaternion(
                                                         local_rot[0], local_rot[1], local_rot[2]) :
                                                     local_quat),
                              local_scale);
        }

        if(made_changes || !lv_gltf_data_has_cached_transform(gltf_data, &node)) {
            lv_gltf_data_set_cached_transform(gltf_data, &node, parentworldmatrix * localmatrix);
        }

        if(node.cameraIndex.has_value()) {
            current_camera_count++;
            if(current_camera_count == gltf_data->camera) {
                fastgltf::math::fmat4x4 cammat = (parentworldmatrix * localmatrix);
                gltf_data->view_pos[0] = cammat[3][0];
                gltf_data->view_pos[1] = cammat[3][1];
                gltf_data->view_pos[2] = cammat[3][2];
                gltf_data->view_mat = fastgltf::math::invert(cammat);
            }
        }
    });
}

static void setup_environment_rotation_matrix(float env_rotation_angle, uint32_t shader_program)
{
    fastgltf::math::fmat3x3 rotmat =
        fastgltf::math::asMatrix(lv_gltf_math_euler_to_quaternion(env_rotation_angle, 0.f, 3.14159f));

    // Get the uniform location and set the uniform
    int32_t u_loc;
    GL_CALL(u_loc = glGetUniformLocation(shader_program, "u_EnvRotation"));
    GL_CALL(glUniformMatrix3fv(u_loc, 1, GL_FALSE, (const GLfloat *)rotmat.data()));
}

#endif /*LV_USE_GLTF*/
