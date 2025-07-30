/**
 * @file lv_gltf_view_render.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "fastgltf/types.hpp"
#include "lv_gltf_view_internal.h"
#include <src/libs/gltf/lv_gltf_data/lv_gltf_data_internal.h>
#if LV_USE_GLTF
#include "../lv_gltf_data/lv_gltf_data_internal.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "../../../misc/lv_types.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../drivers/glfw/lv_opengles_debug.h"

#include <algorithm>
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

static GLuint lv_gltf_view_render_model(lv_gltf_view_t * viewer, lv_gltf_data_t * model, bool prepare_bg,
                                        uint32_t crop_left,
                                        uint32_t crop_right, uint32_t crop_top, uint32_t crop_bottom);
static void lv_gltf_view_push_opengl_state(lv_gl_state_t * state);
static void lv_gltf_view_pop_opengl_state(const lv_gl_state_t * state);
static void setup_finish_frame(void);
static void render_materials(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, const MaterialIndexMap & map);
static void render_skins(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data);

static void draw_primitive(int32_t prim_num, lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, fastgltf::Node & node,
                           std::size_t mesh_index, const fastgltf::math::fmat4x4 & matrix,
                           const lv_gltf_view_env_textures_t * env_tex, bool is_transmission_pass);

static void setup_primitive(int32_t prim_num, lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data,
                            fastgltf::Node & node,
                            std::size_t mesh_index, const fastgltf::math::fmat4x4 & matrix,
                            const lv_gltf_view_env_textures_t * env_tex, bool is_transmission_pass);

static void draw_material(lv_gltf_view_t * viewer, const lv_gltf_uniform_locations_t * uniforms, lv_gltf_data_t * model,
                          lv_gltf_primitive_t * _prim_data, size_t materialIndex, bool is_transmission_pass, GLuint program,
                          uint32_t * tex_num);

static void draw_lights(lv_gltf_data_t * model, GLuint program);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

GLuint lv_gltf_view_render(lv_gltf_view_t * viewer)
{
    const size_t n = lv_array_size(&viewer->models);

    if(n == 0) {
        return GL_NONE;
    }
    lv_gltf_data_t * model = *(lv_gltf_data_t **)lv_array_at(&viewer->models, 0);
    GLuint texture_id = lv_gltf_view_render_model(viewer, model, true, 0, 0, 0, 0);
    for(size_t i = 1; i < n; ++i) {
        lv_gltf_data_t * model = *(lv_gltf_data_t **)lv_array_at(&viewer->models, i);
        lv_gltf_view_render_model(viewer, model, false, 0, 0, 0, 0);
    }
    return texture_id;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void lv_gltf_view_push_opengl_state(lv_gl_state_t * state)
{
    GL_CALL(glGetBooleanv(GL_BLEND, &state->blend_enabled));
    GL_CALL(glGetIntegerv(GL_BLEND_SRC_ALPHA, &state->blend_src));
    GL_CALL(glGetIntegerv(GL_BLEND_DST_ALPHA, &state->blend_dst));
    GL_CALL(glGetIntegerv(GL_BLEND_EQUATION, &state->blend_equation));
    GL_CALL(glGetFloatv(GL_COLOR_CLEAR_VALUE, state->clear_color));
    GL_CALL(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &state->clear_depth));
}

static void lv_gltf_view_pop_opengl_state(const lv_gl_state_t * state)
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
    GL_CALL(glClearDepth(state->clear_depth));
}

static GLuint lv_gltf_view_render_model(lv_gltf_view_t * viewer, lv_gltf_data_t * model, bool prepare_bg,
                                        uint32_t crop_left,
                                        uint32_t crop_right, uint32_t crop_top, uint32_t crop_bottom)
{
    lv_gltf_view_state_t * vstate = &viewer->state;
    lv_gltf_view_desc_t * view_desc = &viewer->desc;
    bool opt_draw_bg = prepare_bg && (view_desc->bg_mode == BG_ENVIRONMENT);
    bool opt_aa_this_frame = (view_desc->aa_mode == ANTIALIAS_CONSTANT) ||
                             (view_desc->aa_mode == ANTIALIAS_NOT_MOVING && model->_last_frame_no_motion == true);
    if(prepare_bg == false) {
        // If this data object is a secondary render pass, inherit the anti-alias setting for this frame from the first gltf_data drawn
        opt_aa_this_frame = view_desc->frame_was_antialiased;
    }

    lv_gl_state_t opengl_state;
    lv_gltf_view_push_opengl_state(&opengl_state);
    gl_renwin_state_t _output;
    gl_renwin_state_t _opaque;

    view_desc->frame_was_cached = true;
    view_desc->render_width = lv_obj_get_width((lv_obj_t *)viewer) * (opt_aa_this_frame ? 2 : 1);
    view_desc->render_height = lv_obj_get_height((lv_obj_t *)viewer) * (opt_aa_this_frame ? 2 : 1);

    if(opt_aa_this_frame != model->last_frame_was_antialiased) {
        // Antialiasing state has changed since the last render
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

    if(opt_aa_this_frame) {
        crop_left *= 2;
        crop_right *= 2;
        crop_top *= 2;
        crop_bottom *= 2;
    }
    view_desc->frame_was_antialiased = opt_aa_this_frame;
    if(prepare_bg == true) {
        if(!vstate->render_state_ready) {
            _output = setup_primary_output((uint32_t)view_desc->render_width, (uint32_t)view_desc->render_height,
                                           opt_aa_this_frame);
            setup_finish_frame();
            vstate->render_state = _output;
        }
    }

    if(!vstate->render_state_ready) {
        if(prepare_bg == true) {
            vstate->render_state_ready = true;
            if(vstate->render_opaque_buffer) {
                _opaque = setup_opaque_output(vstate->opaque_frame_buffer_width,
                                              vstate->opaque_frame_buffer_height);
                vstate->opaque_render_state = _opaque;
                setup_finish_frame();
            }
        }
    }
    bool motion_dirty = false;
    if(view_desc->dirty) {
        motion_dirty = true;
    }
    view_desc->dirty = false;

    _output = vstate->render_state;
    int32_t anim_num = view_desc->anim;
    if((anim_num >= 0) && ((int64_t)lv_gltf_data_get_animation_count(model) > anim_num)) {
        if(LV_ABS(view_desc->timestep) > 0.0001f) {
            //std::cout << "ACTIVE ANIMATION TRIGGER WINDOW MOTION\n";
            model->local_timestamp += view_desc->timestep;
            motion_dirty = true;
        }
        if(model->last_anim_num != anim_num) {
            model->cur_anim_maxtime = lv_gltf_data_get_animation_total_time(model, anim_num);
            model->last_anim_num = anim_num;
        }
        if(model->local_timestamp >= model->cur_anim_maxtime)
            model->local_timestamp = 0.05f;
        else if(model->local_timestamp < 0.0f)
            model->local_timestamp = model->cur_anim_maxtime - 0.05f;
        //std::cout << "Animation #" << std::to_string(anim_num) << " | Time = " << std::to_string(local_timestamp) << "\n";
    }

    // TODO: check if the override actually affects the transform and that the affected object is visible in the scene

    lv_memcpy(&(viewer->last_desc), view_desc, sizeof(*view_desc));

    bool ___lastFrameNoMotion = model->__last_frame_no_motion;
    model->__last_frame_no_motion = model->_last_frame_no_motion;
    model->_last_frame_no_motion = true;
    int32_t pref_cam_num = LV_MIN(view_desc->camera, (int32_t)lv_gltf_data_get_camera_count(model) - 1);
    if(motion_dirty || (pref_cam_num != model->last_camera_index) || lv_gltf_data_transform_cache_is_empty(model)) {
        model->_last_frame_no_motion = false;
        motion_dirty = false;
        lv_gltf_view_recache_all_transforms(viewer, model);
    }

    if((model->_last_frame_no_motion == true) && (model->__last_frame_no_motion == true) &&
        (___lastFrameNoMotion == true)) {
        // Nothing changed at all, return the previous output frame
        setup_finish_frame();
        lv_gltf_view_pop_opengl_state(&opengl_state);
        return _output.texture;
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

    model->last_material_index = 99999; // Reset the last material index to an unused value once per frame at the start
    if(vstate->render_opaque_buffer) {
        if(model->has_any_cameras)
            setup_view_proj_matrix_from_camera(viewer, model->current_camera_index, view_desc, model->view_mat,
                                               model->view_pos, model, true);
        else
            setup_view_proj_matrix(viewer, view_desc, model, true);
        _opaque = vstate->opaque_render_state;

        lv_result_t result = setup_restore_opaque_output(view_desc, _opaque, vstate->opaque_frame_buffer_width,
                                                         vstate->opaque_frame_buffer_height, prepare_bg);
        LV_ASSERT_MSG(result == LV_RESULT_OK, "Failed to setup opaque output which should never happen");
        if(result != LV_RESULT_OK) {
            lv_gltf_view_pop_opengl_state(&opengl_state);
            return _output.texture;
        }

        if(opt_draw_bg) {
            setup_draw_environment_background(viewer->shader_manager, viewer, view_desc->blur_bg * 0.4f);
        }

        render_materials(viewer, model, model->opaque_nodes_by_material_index);

        for(const auto & node_distance_pair : distance_sort_nodes) {
            const auto & node_element = node_distance_pair.second;
            const auto & node = node_element.first;
            draw_primitive(node_element.second, viewer, model, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(model, node), &viewer->env_textures, true);
        }

        GL_CALL(glBindTexture(GL_TEXTURE_2D, _opaque.texture));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
        setup_finish_frame();
    }

    if(model->has_any_cameras) {
        setup_view_proj_matrix_from_camera(viewer, model->current_camera_index, view_desc, model->view_mat, model->view_pos, model, false);
    }
    else{
        setup_view_proj_matrix(viewer, view_desc, model, false);
    }
    viewer->env_rotation_angle = viewer->env_textures.angle;

    {
        lv_result_t result = setup_restore_primary_output(view_desc, _output,
                                                          (uint32_t)view_desc->render_width - (crop_left + crop_right),
                                                          (uint32_t)view_desc->render_height - (crop_top + crop_bottom),
                                                          crop_left, crop_bottom, prepare_bg);

        LV_ASSERT_MSG(result == LV_RESULT_OK, "Failed to restore primary output which should never happen");
        if(result != LV_RESULT_OK) {
            lv_gltf_view_pop_opengl_state(&opengl_state);
            return _output.texture;
        }
        if(opt_draw_bg)
            setup_draw_environment_background(viewer->shader_manager, viewer, view_desc->blur_bg);
        render_materials(viewer, model, model->opaque_nodes_by_material_index);

        for(const auto & node_distance_pair : distance_sort_nodes) {
            const auto & node_element = node_distance_pair.second; // Access the second member (NodeIndexPair)
            const auto & node = node_element.first;
            draw_primitive(node_element.second, viewer, model, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(model, node), &viewer->env_textures, false);
        }
        if(opt_aa_this_frame) {
            GL_CALL(glBindTexture(GL_TEXTURE_2D, _output.texture));
            GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
        }
        setup_finish_frame();
    }
    lv_gltf_view_pop_opengl_state(&opengl_state);
    view_desc->frame_was_cached = false;

    return _output.texture;
}

static void setup_finish_frame(void)
{
    GL_CALL(glDisable(GL_DEPTH_TEST));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CALL(glBindRenderbuffer(GL_RENDERBUFFER, 0));
    GL_CALL(glUseProgram(0));
}

static void render_materials(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, const MaterialIndexMap & map)
{
    for(const auto & kv : map) {
        for(const auto & pair : kv.second) {
            auto node = pair.first;
            draw_primitive(pair.second, viewer, gltf_data, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(gltf_data, node), &viewer->env_textures, true);
        }
    }
}

static void render_skins(lv_gltf_view_t * viewer, lv_gltf_data_t * model){

    uint32_t skin_count = lv_gltf_data_get_skins_size(model);
    if(skin_count == 0) {
        return;
    }
    lv_gltf_data_destroy_textures(model);
    for(size_t i = 0; i < skin_count; ++i) {
        const auto &skin_index = lv_gltf_data_get_skin(model, i);
        const auto &skin = model->asset.skins[skin_index];
        auto &ibm = viewer->ibm_by_skin_the_node[skin_index];

        size_t num_joints = skin.joints.size();
        size_t tex_width = std::ceil(std::sqrt((float)num_joints * 8.0f));

        GLuint rtex = lv_gltf_data_create_texture(model);
        GL_CALL(glBindTexture(GL_TEXTURE_2D, rtex));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        float * texture_data = (float*)lv_malloc(tex_width * tex_width * 4 * sizeof(*texture_data));
        LV_ASSERT_MALLOC(texture_data);
        size_t texture_data_index = 0;

        for(uint64_t j = 0; j < num_joints; j++) {
            auto & joint_node = model->asset.nodes[skin.joints[j]];
            fastgltf::math::fmat4x4 final_joint_matrix = lv_gltf_data_get_cached_transform(model, &joint_node) * ibm[&joint_node]; 

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
static void draw_primitive(int32_t prim_num, lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, fastgltf::Node & node,
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
static void setup_primitive(int32_t prim_num, lv_gltf_view_t * viewer, lv_gltf_data_t * model, fastgltf::Node & node,
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
    setup_environment_rotation_matrix(viewer->env_rotation_angle, program);
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

static void draw_material(lv_gltf_view_t * viewer, const lv_gltf_uniform_locations_t * uniforms, lv_gltf_data_t * model,
                          lv_gltf_primitive_t * _prim_data, size_t materialIndex, bool is_transmission_pass, GLuint program,
                          uint32_t * tex_num)
{
    const auto & asset = lv_gltf_data_get_asset(model);

    bool has_material = asset->materials.size() > (materialIndex - 1);

    if(!has_material) {
        setup_uniform_color_alpha(uniforms->base_color_factor, fastgltf::math::fvec4(1.0f));
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
    setup_uniform_color_alpha(uniforms->base_color_factor, gltfMaterial.pbrData.baseColorFactor);
    setup_uniform_color(uniforms->emissive_factor, gltfMaterial.emissiveFactor);

    GL_CALL(glUniform1f(uniforms->emissive_strength, gltfMaterial.emissiveStrength));
    GL_CALL(glUniform1f(uniforms->roughness_factor, gltfMaterial.pbrData.roughnessFactor));
    GL_CALL(glUniform1f(uniforms->metallic_factor, gltfMaterial.pbrData.metallicFactor));
    GL_CALL(glUniform1f(uniforms->ior, gltfMaterial.ior));
    GL_CALL(glUniform1f(uniforms->dispersion, gltfMaterial.dispersion));

    if(gltfMaterial.pbrData.baseColorTexture.has_value())
        *tex_num = setup_texture(*tex_num, _prim_data->albedoTexture, _prim_data->baseColorTexcoordIndex,
                                 gltfMaterial.pbrData.baseColorTexture->transform, uniforms->base_color_sampler,
                                 uniforms->base_color_uv_set, uniforms->base_color_uv_transform);
    if(gltfMaterial.emissiveTexture.has_value())
        *tex_num = setup_texture(*tex_num, _prim_data->emissiveTexture, _prim_data->emissiveTexcoordIndex,
                                 gltfMaterial.emissiveTexture->transform, uniforms->emissive_sampler,
                                 uniforms->emissive_uv_set, uniforms->emissive_uv_transform);
    if(gltfMaterial.pbrData.metallicRoughnessTexture.has_value())
        *tex_num = setup_texture(*tex_num, _prim_data->metalRoughTexture, _prim_data->metallicRoughnessTexcoordIndex,
                                 gltfMaterial.pbrData.metallicRoughnessTexture->transform,
                                 uniforms->metallic_roughness_sampler, uniforms->metallic_roughness_uv_set,
                                 uniforms->metallic_roughness_uv_transform);
    if(gltfMaterial.occlusionTexture.has_value()) {
        GL_CALL(glUniform1f(uniforms->occlusion_strength, static_cast<float>(gltfMaterial.occlusionTexture->strength)));
        *tex_num = setup_texture(*tex_num, _prim_data->occlusionTexture, _prim_data->occlusionTexcoordIndex,
                                 gltfMaterial.occlusionTexture->transform, uniforms->occlusion_sampler,
                                 uniforms->occlusion_uv_set, uniforms->occlusion_uv_transform);
    }

    if(gltfMaterial.normalTexture.has_value()) {
        GL_CALL(glUniform1f(uniforms->normal_scale, static_cast<float>(gltfMaterial.normalTexture->scale)));
        *tex_num = setup_texture(*tex_num, _prim_data->normalTexture, _prim_data->normalTexcoordIndex,
                                 gltfMaterial.normalTexture->transform, uniforms->normal_sampler,
                                 uniforms->normal_uv_set, uniforms->normal_uv_transform);
    }

    if(gltfMaterial.clearcoat) {
        GL_CALL(glUniform1f(uniforms->clearcoat_factor, static_cast<float>(gltfMaterial.clearcoat->clearcoatFactor)));
        GL_CALL(glUniform1f(uniforms->clearcoat_roughness_factor,
                            static_cast<float>(gltfMaterial.clearcoat->clearcoatRoughnessFactor)));

        if(gltfMaterial.clearcoat->clearcoatTexture.has_value())
            *tex_num = setup_texture(*tex_num, _prim_data->clearcoatTexture, _prim_data->clearcoatTexcoordIndex,
                                     gltfMaterial.clearcoat->clearcoatTexture->transform,
                                     uniforms->clearcoat_sampler, uniforms->clearcoat_uv_set,
                                     uniforms->clearcoat_uv_transform);
        if(gltfMaterial.clearcoat->clearcoatRoughnessTexture.has_value())
            *tex_num = setup_texture(*tex_num, _prim_data->clearcoatRoughnessTexture,
                                     _prim_data->clearcoatRoughnessTexcoordIndex,
                                     gltfMaterial.clearcoat->clearcoatRoughnessTexture->transform,
                                     uniforms->clearcoat_roughness_sampler, uniforms->clearcoat_roughness_uv_set,
                                     uniforms->clearcoat_roughness_uv_transform);
        if(gltfMaterial.clearcoat->clearcoatNormalTexture.has_value()) {
            GL_CALL(glUniform1f(uniforms->clearcoat_normal_scale,
                                static_cast<float>(gltfMaterial.clearcoat->clearcoatNormalTexture->scale)));
            *tex_num = setup_texture(*tex_num, _prim_data->clearcoatNormalTexture,
                                     _prim_data->clearcoatNormalTexcoordIndex,
                                     gltfMaterial.clearcoat->clearcoatNormalTexture->transform,
                                     uniforms->clearcoat_normal_sampler, uniforms->clearcoat_normal_uv_set,
                                     uniforms->clearcoat_normal_uv_transform);
        }
    }

    if(gltfMaterial.volume) {
        GL_CALL(glUniform1f(uniforms->attenuation_distance, gltfMaterial.volume->attenuationDistance));
        setup_uniform_color(uniforms->attenuation_color, gltfMaterial.volume->attenuationColor);
        GL_CALL(glUniform1f(uniforms->thickness, gltfMaterial.volume->thicknessFactor));
        if(gltfMaterial.volume->thicknessTexture.has_value()) {
            *tex_num = setup_texture(*tex_num, _prim_data->thicknessTexture, _prim_data->thicknessTexcoordIndex,
                                     gltfMaterial.volume->thicknessTexture->transform, uniforms->thickness_sampler,
                                     uniforms->thickness_uv_set, uniforms->thickness_uv_transform);
        }
    }

    if(gltfMaterial.transmission) {
        GL_CALL(glUniform1f(uniforms->transmission_factor, gltfMaterial.transmission->transmissionFactor));
        GL_CALL(glUniform2i(uniforms->screen_size, viewer->desc.render_width, viewer->desc.render_height));
        if(gltfMaterial.transmission->transmissionTexture.has_value())
            *tex_num = setup_texture(*tex_num, _prim_data->transmissionTexture,
                                     _prim_data->transmissionTexcoordIndex,
                                     gltfMaterial.transmission->transmissionTexture->transform,
                                     uniforms->transmission_sampler, uniforms->transmission_uv_set,
                                     uniforms->transmission_uv_transform);
    }

    if(gltfMaterial.sheen) {
        setup_uniform_color(uniforms->sheen_color_factor, gltfMaterial.sheen->sheenColorFactor);
        GL_CALL(glUniform1f(uniforms->sheen_roughness_factor,
                            static_cast<float>(gltfMaterial.sheen->sheenRoughnessFactor)));
        if(gltfMaterial.sheen->sheenColorTexture.has_value()) {
            LV_LOG_WARN("Material has unhandled sheen texture");
        }
    }
    if(gltfMaterial.specular) {
        setup_uniform_color(uniforms->specular_color_factor, gltfMaterial.specular->specularColorFactor);
        GL_CALL(glUniform1f(uniforms->specular_factor, static_cast<float>(gltfMaterial.specular->specularFactor)));
        if(gltfMaterial.specular->specularTexture.has_value()) {
            LV_LOG_WARN("Material has unhandled specular texture");
        }
        if(gltfMaterial.specular->specularColorTexture.has_value()) {
            LV_LOG_WARN("Material has unhandled specular color texture");
        }
    }

    if(gltfMaterial.specularGlossiness) {
        LV_LOG_WARN(
            "Model uses outdated legacy mode pbr_speculargloss. Please update this model to a new shading model ");
        setup_uniform_color_alpha(uniforms->diffuse_factor, gltfMaterial.specularGlossiness->diffuseFactor);
        setup_uniform_color(uniforms->specular_factor, gltfMaterial.specularGlossiness->specularFactor);
        GL_CALL(glUniform1f(uniforms->glossiness_factor,
                            static_cast<float>(gltfMaterial.specularGlossiness->glossinessFactor)));
        if(gltfMaterial.specularGlossiness->diffuseTexture.has_value()) {
            *tex_num = setup_texture(*tex_num, _prim_data->diffuseTexture, _prim_data->diffuseTexcoordIndex,
                                     gltfMaterial.specularGlossiness->diffuseTexture->transform,
                                     uniforms->diffuse_sampler, uniforms->diffuse_uv_set,
                                     uniforms->diffuse_uv_transform);
        }
        if(gltfMaterial.specularGlossiness->specularGlossinessTexture.has_value()) {
            *tex_num = setup_texture(*tex_num, _prim_data->specularGlossinessTexture,
                                     _prim_data->specularGlossinessTexcoordIndex,
                                     gltfMaterial.specularGlossiness->specularGlossinessTexture->transform,
                                     uniforms->specular_glossiness_sampler, uniforms->specular_glossiness_uv_set,
                                     uniforms->specular_glossiness_uv_transform);
        }
    }

    if(gltfMaterial.diffuseTransmission) {
        setup_uniform_color(uniforms->diffuse_transmission_color_factor,
                            gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor);
        GL_CALL(glUniform1f(uniforms->diffuse_transmission_factor,
                            static_cast<float>(gltfMaterial.diffuseTransmission->diffuseTransmissionFactor)));
        if(gltfMaterial.diffuseTransmission->diffuseTransmissionTexture.has_value()) {
            *tex_num = setup_texture(*tex_num, _prim_data->diffuseTransmissionTexture,
                                     _prim_data->diffuseTransmissionTexcoordIndex,
                                     gltfMaterial.diffuseTransmission->diffuseTransmissionTexture->transform,
                                     uniforms->diffuse_transmission_sampler, uniforms->diffuse_transmission_uv_set,
                                     uniforms->diffuse_transmission_uv_transform);
        }
        if(gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture.has_value()) {
            *tex_num = setup_texture(*tex_num, _prim_data->diffuseTransmissionColorTexture,
                                     _prim_data->diffuseTransmissionColorTexcoordIndex,
                                     gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture->transform,
                                     uniforms->diffuse_transmission_color_sampler,
                                     uniforms->diffuse_transmission_color_uv_set,
                                     uniforms->diffuse_transmission_color_uv_transform);
        }
    }
}
static void draw_lights(lv_gltf_data_t * model, GLuint program)
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
#endif /*LV_USE_GLTF*/
