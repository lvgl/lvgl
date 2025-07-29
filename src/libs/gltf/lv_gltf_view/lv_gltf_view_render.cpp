/**
 * @file lv_gltf_view_render.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_view_internal.hpp"
#if LV_USE_GLTF
#include "../lv_gltf_data/lv_gltf_data_internal.hpp"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
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

static GLuint lv_gltf_view_render_model(lv_gltf_view_t * viewer, lv_gltf_data_t * model,
                                        bool prepare_bg, uint32_t crop_left,  uint32_t crop_right,
                                        uint32_t crop_top,  uint32_t crop_bottom);
static void lv_gltf_view_push_opengl_state(lv_gl_state_t * state);
static void lv_gltf_view_pop_opengl_state(const lv_gl_state_t * state);
static void setup_finish_frame(void);
static void setup_compile_and_load_bg_shader(lv_gl_shader_manager_t * manager);
static void render_materials(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, const MaterialIndexMap & map);

static void draw_primitive(int32_t prim_num,
                           lv_gltf_view_desc_t * view_desc,
                           lv_gltf_view_t * viewer,
                           lv_gltf_data_t * gltf_data,
                           fastgltf::Node & node,
                           std::size_t mesh_index,
                           const fastgltf::math::fmat4x4 & matrix,
                           const lv_gltf_view_env_textures_t * env_tex,
                           bool is_transmission_pass);

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
    for(size_t i = 1; i < n ; ++i) {
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
    GL_CALL(glGetBooleanv(GL_BLEND, &state->blendEnabled));
    GL_CALL(glGetIntegerv(GL_BLEND_SRC_ALPHA, &state->blendSrc));
    GL_CALL(glGetIntegerv(GL_BLEND_DST_ALPHA, &state->blendDst));
    GL_CALL(glGetIntegerv(GL_BLEND_EQUATION, &state->blendEquation));
    GL_CALL(glGetFloatv(GL_COLOR_CLEAR_VALUE, state->clearColor));
    GL_CALL(glGetFloatv(GL_DEPTH_CLEAR_VALUE, &state->clearDepth));
}

static void lv_gltf_view_pop_opengl_state(const lv_gl_state_t * state)
{
    GL_CALL(glDisable(GL_CULL_FACE));
    if(state->blendEnabled) {
        GL_CALL(glEnable(GL_BLEND));
    }
    else {
        GL_CALL(glDisable(GL_BLEND));
    }
    GL_CALL(glBlendFunc(state->blendSrc, state->blendDst));
    GL_CALL(glBlendEquation(state->blendEquation));
    GL_CALL(glDepthMask(GL_TRUE));
    GL_CALL(glClearColor(state->clearColor[0], state->clearColor[1], state->clearColor[2], state-> clearColor[3]));
    GL_CALL(glClearDepth(state->clearDepth));
}

static GLuint lv_gltf_view_render_model(lv_gltf_view_t * viewer, lv_gltf_data_t * model, bool prepare_bg,
                                        uint32_t crop_left,  uint32_t crop_right,  uint32_t crop_top,  uint32_t crop_bottom)
{
    const auto & asset = lv_gltf_data_get_asset(model);
    lv_gltf_view_state_t * vstate = &viewer->state;
    lv_gltf_view_desc_t * view_desc = &viewer->desc;
    bool opt_draw_bg = prepare_bg && (view_desc->bg_mode == BG_ENVIRONMENT);
    bool opt_aa_this_frame = (view_desc->aa_mode == ANTIALIAS_CONSTANT) || (view_desc->aa_mode == ANTIALIAS_NOT_MOVING &&
                                                                            model->_last_frame_no_motion == true);
    if(prepare_bg == false) {
        // If this data object is a secondary render pass, inherit the anti-alias setting for this frame from the first gltf_data drawn
        opt_aa_this_frame = view_desc->frame_was_antialiased;
    }

    lv_gl_state_t opengl_state;
    lv_gltf_view_push_opengl_state(&opengl_state);
    uint32_t sceneIndex = 0;
    gl_renwin_state_t _output;
    gl_renwin_state_t _opaque;

    view_desc->frame_was_cached = true;
    view_desc->render_width = lv_obj_get_width((lv_obj_t *)viewer) * (opt_aa_this_frame ? 2 : 1);
    view_desc->render_height = lv_obj_get_height((lv_obj_t *)viewer)  * (opt_aa_this_frame ? 2 : 1);

    if(opt_aa_this_frame != model->last_frame_was_antialiased) {
        // Antialiasing state has changed since the last render
        if(prepare_bg == true) {
            if(vstate->render_state_ready) {
                setup_cleanup_opengl_output(&vstate->render_state);
                vstate->render_state = setup_primary_output((uint32_t)view_desc->render_width, (uint32_t)view_desc->render_height,
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

    if(!model->nodes_parsed) {
        model->nodes_parsed = true;
        std::vector<int64_t> _used = std::vector<int64_t>();
        int64_t _max_index = 0;
        fastgltf::iterateSceneNodes(*asset, sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node & node,
        fastgltf::math::fmat4x4 matrix) {
            // TO-DO: replace this iterate with one that doesn't bother with any matrix math.  Since this is a one time loop at start up, it's ok for now.
            LV_UNUSED(matrix);
            if(node.meshIndex) {
                auto & mesh_index = node.meshIndex.value();
                if(node.skinIndex) {
                    auto skin_index = node.skinIndex.value();
                    if(!lv_gltf_data_validated_skins_contains(model, skin_index)) {
                        lv_gltf_data_validate_skin(model, skin_index);
                        auto skin = asset->skins[skin_index];
                        if(skin.inverseBindMatrices) {
                            auto & _ibmVal = skin.inverseBindMatrices.value();
                            auto & _ibmAccessor = asset->accessors[_ibmVal];
                            if(_ibmAccessor.bufferViewIndex) {  // To-do: test if this gets confused when bufferViewIndex == 0
                                fastgltf::iterateAccessorWithIndex<fastgltf::math::fmat4x4>(*asset, _ibmAccessor, [&](fastgltf::math::fmat4x4 _matrix,
                                std::size_t idx) {
                                    auto & _jointNode = asset->nodes[skin.joints[idx]];
                                    viewer->ibm_by_skin_the_node[skin_index][&_jointNode] = _matrix;
                                });
                            }
                        }
                    }
                }
                for(size_t mp = 0; mp < asset->meshes[mesh_index].primitives.size(); mp++) {
                    auto & _prim_gltf_data = asset->meshes[mesh_index].primitives[mp];
                    auto & mappings = _prim_gltf_data.mappings;
                    int64_t materialIndex = (!mappings.empty() &&
                                             mappings[vstate->material_variant]) ?  mappings[vstate->material_variant].value() + 1 : ((
                                                                                                                                          _prim_gltf_data.materialIndex) ?
                                                                                                                                      (_prim_gltf_data.materialIndex.value() + 1) : 0);
                    if(materialIndex < 0) {
                        lv_gltf_data_add_opaque_node_primitive(model, 0, &node, mp);
                    }
                    else {
                        auto & material = asset->materials[materialIndex - 1];
                        if(material.transmission) vstate->render_opaque_buffer = true;
                        if(material.alphaMode == fastgltf::AlphaMode::Blend || (material.transmission != NULL)) {
                            lv_gltf_data_add_blended_node_primitive(model, materialIndex + 1, &node, mp);
                        }
                        else {
                            lv_gltf_data_add_opaque_node_primitive(model, materialIndex + 1, &node, mp);
                        }
                        _max_index = std::max(_max_index, materialIndex);
                    }
                }
            }
        });

        lv_gltf_data_init_shaders(model, _max_index);
        setup_compile_and_load_bg_shader(viewer->shader_manager);
        fastgltf::iterateSceneNodes(*asset, sceneIndex, fastgltf::math::fmat4x4(), [&](fastgltf::Node & node,
        fastgltf::math::fmat4x4 matrix) {
            LV_UNUSED(matrix);
            if(!node.meshIndex) {
                return;
            }
            auto & mesh_index = node.meshIndex.value();
            for(uint64_t mp = 0; mp < asset->meshes[mesh_index].primitives.size(); mp++) {
                auto & prim_gltf_data = asset->meshes[mesh_index].primitives[mp];
                auto & mappings = prim_gltf_data.mappings;
                int64_t material_index = (!mappings.empty() &&
                                          mappings[vstate->material_variant]) ?  mappings[vstate->material_variant].value() + 1 : ((
                                                                                                                                       prim_gltf_data.materialIndex) ?
                                                                                                                                   (prim_gltf_data.materialIndex.value() + 1) : 0);
                const auto & shaderset = lv_gltf_data_get_shader_set(model, material_index);
                if(material_index > -1 && !shaderset->ready) {
                    lv_array_t defines;
                    lv_array_init(&defines, 64, sizeof(lv_gl_shader_t));
                    lv_result_t result = lv_gltf_view_shader_injest_discover_defines(&defines, model, &node, &prim_gltf_data);
                    LV_ASSERT_MSG(result == LV_RESULT_OK, "Couldn't injest shader defines");
                    lv_gltf_renwin_shaderset_t shaderset = lv_gltf_view_shader_compile_program(viewer, (lv_gl_shader_t*)defines.data, lv_array_size(&defines));
                    lv_gltf_uniform_locations_t uniform_locations = lv_gltf_uniform_locations_create(shaderset.program);
                    lv_gltf_data_set_shader(model, material_index, uniform_locations, shaderset);
                }
            }
        });
    }

    if(!vstate->render_state_ready) {
        if(prepare_bg == true) {
            vstate->render_state_ready = true;
            if(vstate->render_opaque_buffer) {
                _opaque = setup_opaque_output(vstate->opaque_frame_buffer_width, vstate->opaque_frame_buffer_height);
                vstate->opaque_render_state = _opaque;
                setup_finish_frame();
            }
        }
    }
    bool motion_dirty = false;
    if(view_desc->dirty) {
        //std::cout << "DIRTY VIEW TRIGGER WINDOW MOTION\n";
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
        if(model->local_timestamp >= model->cur_anim_maxtime) model->local_timestamp = 0.05f;
        else if(model->local_timestamp < 0.0f) model->local_timestamp = model->cur_anim_maxtime - 0.05f;
        //std::cout << "Animation #" << std::to_string(anim_num) << " | Time = " << std::to_string(local_timestamp) << "\n";
    }

    // TODO: check if the override actually affects the transform and that the affected object is visible in the scene

    lv_memcpy(&(viewer->last_desc), view_desc, sizeof(*view_desc));

    bool ___lastFrameNoMotion = model->__last_frame_no_motion;
    model->__last_frame_no_motion = model->_last_frame_no_motion;
    model->_last_frame_no_motion = true;
    int32_t pref_cam_num = LV_MIN(view_desc->camera, (int32_t)lv_gltf_data_get_camera_count(model) - 1);
    if(motion_dirty || (pref_cam_num != model->last_camera_index) ||
       lv_gltf_data_transform_cache_is_empty(model))  {
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

    uint32_t _ss = lv_gltf_data_get_skins_size(model);
    if(_ss > 0) {
        lv_gltf_data_destroy_textures(model);
        uint64_t i = 0u;
        uint32_t SIZEOF_16FLOATS = sizeof(float) * 16;
        while(i < _ss) {
            auto skinIndex = lv_gltf_data_get_skin(model, i);
            auto skin = asset->skins[skinIndex];
            auto _ibm = viewer->ibm_by_skin_the_node[skinIndex];

            std::size_t num_joints = skin.joints.size();
            std::size_t _tex_width = std::ceil(std::sqrt((float)num_joints * 8.0f));

            GLuint rtex = lv_gltf_data_create_texture(model);
            GL_CALL(glBindTexture(GL_TEXTURE_2D, rtex));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
            GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
            /* TODO: perf: Avoid doing memory allocations inside loops */
            float * _data = new float[_tex_width * _tex_width * 4];
            std::size_t _dpos = 0;
            for(uint64_t j = 0; j < num_joints; j++) {
                auto & _jointNode = asset->nodes[skin.joints[j]];
                fastgltf::math::fmat4x4 _finalJointMat = lv_gltf_data_get_cached_transform(model,
                                                                                           &_jointNode) * _ibm[&_jointNode];  // _ibmBySkinThenNode[skinIndex][&_jointNode];
                std::memcpy(&_data[_dpos], _finalJointMat.data(), SIZEOF_16FLOATS); // Copy final joint matrix
                std::memcpy(&_data[_dpos + 16], fastgltf::math::transpose(fastgltf::math::invert(_finalJointMat)).data(),
                            SIZEOF_16FLOATS);   // Copy normal matrix
                _dpos += 32;
            }
            GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _tex_width, _tex_width, 0, GL_RGBA, GL_FLOAT, _data));
            GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
            delete[] _data;
            ++i;
        }
    }

    NodeDistanceVector distance_sort_nodes;

    for(const auto & kv : model->blended_nodes_by_material_index) {
        for(const auto & pair : kv.second) {
            auto node = pair.first;
            auto new_node = NodeIndexDistancePair(fastgltf::math::length(model->view_pos - lv_gltf_data_get_centerpoint(
                                                                             model, lv_gltf_data_get_cached_transform(model, node), node->meshIndex.value(), pair.second)),
                                                  NodeIndexPair(node, pair.second));
            distance_sort_nodes.push_back(new_node);
        }
    }
    // Sort __distance_sort_nodes by the first member (distance)
    /*std::sort(gltf_data->distance_sort_nodes.begin(), gltf_data->distance_sort_nodes.end(),*/
    std::sort(distance_sort_nodes.begin(), distance_sort_nodes.end(),
    [](const NodeIndexDistancePair & a, const NodeIndexDistancePair & b) {
        return a.first < b.first;
    });

    model->last_material_index = 99999;  // Reset the last material index to an unused value once per frame at the start
    if(vstate->render_opaque_buffer) {
        if(model->has_any_cameras) setup_view_proj_matrix_from_camera(viewer, model->current_camera_index, view_desc,
                                                                          model->view_mat, model->view_pos, model, true);
        else setup_view_proj_matrix(viewer, view_desc, model, true);
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
            draw_primitive(node_element.second, view_desc, viewer, model, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(model, node), &viewer->env_textures, true);
        }

        GL_CALL(glBindTexture(GL_TEXTURE_2D, _opaque.texture));
        GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
        GL_CALL(glBindTexture(GL_TEXTURE_2D, GL_NONE));
        setup_finish_frame();
    }

    if(model->has_any_cameras) setup_view_proj_matrix_from_camera(viewer, model->current_camera_index, view_desc,
                                                                      model->view_mat, model->view_pos, model, false);
    else setup_view_proj_matrix(viewer, view_desc, model, false);
    viewer->env_rotation_angle = viewer->env_textures.angle;

    {
        lv_result_t result = setup_restore_primary_output(view_desc, _output,
                                                          (uint32_t)view_desc->render_width - (crop_left + crop_right),
                                                          (uint32_t)view_desc->render_height - (crop_top + crop_bottom), crop_left, crop_bottom, prepare_bg);

        LV_ASSERT_MSG(result == LV_RESULT_OK, "Failed to restore primary output which should never happen");
        if(result != LV_RESULT_OK) {
            lv_gltf_view_pop_opengl_state(&opengl_state);
            return _output.texture;
        }
        if(opt_draw_bg) setup_draw_environment_background(viewer->shader_manager, viewer, view_desc->blur_bg);
        render_materials(viewer, model, model->opaque_nodes_by_material_index);

        for(const auto & node_distance_pair : distance_sort_nodes) {
            const auto & node_element = node_distance_pair.second; // Access the second member (NodeIndexPair)
            const auto & node = node_element.first;
            draw_primitive(node_element.second, view_desc, viewer, model, *node, node->meshIndex.value(),
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
static void setup_compile_and_load_bg_shader(lv_gl_shader_manager_t * manager)
{
    lv_gl_shader_t frag_defs[1] = { { "TONEMAP_KHR_PBR_NEUTRAL", NULL } };

    uint32_t frag_shader_hash = lv_gl_shader_manager_select_shader(manager, "cubemap.frag", frag_defs, 1);
    uint32_t vert_shader_hash = lv_gl_shader_manager_select_shader(manager, "cubemap.vert", nullptr, 0);

    lv_gl_shader_program_t * program = lv_gl_shader_manager_get_program(manager, frag_shader_hash, vert_shader_hash);

    manager->bg_program = lv_gl_shader_program_get_id(program);
    setup_background_environment(manager->bg_program, &manager->bg_vao, &manager->bg_index_buf, &manager->bg_vertex_buf);
}

static void render_materials(lv_gltf_view_t * viewer, lv_gltf_data_t * gltf_data, const MaterialIndexMap & map)
{

    for(const auto & kv : map) {
        for(const auto & pair : kv.second) {
            auto node = pair.first;
            draw_primitive(pair.second, &viewer->desc, viewer, gltf_data, *node, node->meshIndex.value(),
                           lv_gltf_data_get_cached_transform(gltf_data, node), &viewer->env_textures, true);
        }
    }
}
static void draw_primitive(int32_t prim_num,
                           lv_gltf_view_desc_t * view_desc,
                           lv_gltf_view_t * viewer,
                           lv_gltf_data_t * gltf_data,
                           fastgltf::Node & node,
                           std::size_t mesh_index,
                           const fastgltf::math::fmat4x4 & matrix,
                           const lv_gltf_view_env_textures_t * env_tex,
                           bool is_transmission_pass)
{
    lv_gltf_mesh_data_t * mesh = lv_gltf_data_get_mesh(gltf_data, mesh_index);
    const auto & asset = lv_gltf_data_get_asset(gltf_data);
    const auto & _prim_data = lv_gltf_data_get_primitive_from_mesh(mesh, prim_num);
    auto & _prim_gltf_data = asset->meshes[mesh_index].primitives[prim_num];
    auto & mappings = _prim_gltf_data.mappings;
    std::size_t materialIndex = (!mappings.empty() && mappings[viewer->state.material_variant].has_value())
                                ? mappings[viewer->state.material_variant].value() + 1
                                : ((_prim_gltf_data.materialIndex.has_value()) ? (_prim_gltf_data.materialIndex.value() + 1) : 0);

    GL_CALL(glBindVertexArray(_prim_data->vertexArray));
    if((gltf_data->last_material_index == materialIndex) &&
       (gltf_data->last_pass_was_transmission == is_transmission_pass)) {
        GL_CALL(glUniformMatrix4fv(get_uniform_ids(gltf_data, materialIndex)->model_matrix, 1, GL_FALSE, &matrix[0][0]));
    }
    else {
        gltf_data->last_material_index = materialIndex;
        gltf_data->last_pass_was_transmission = is_transmission_pass;
        auto program = lv_gltf_data_get_shader_program(gltf_data, materialIndex);
        const auto & uniforms = get_uniform_ids(gltf_data, materialIndex);

        GL_CALL(glUseProgram(program));

        GL_CALL(glUniformMatrix4fv(uniforms->model_matrix, 1, GL_FALSE, &matrix[0][0]));
        GL_CALL(glUniformMatrix4fv(uniforms->view_matrix, 1, false, viewer->view_matrix.data()));
        GL_CALL(glUniformMatrix4fv(uniforms->projection_matrix, 1, false, viewer->projection_matrix.data()));
        GL_CALL(glUniformMatrix4fv(uniforms->view_projection_matrix, 1, false, viewer->view_projection_matrix.data()));
        const auto & _campos = viewer->camera_pos;
        GL_CALL(glUniform3f(uniforms->camera,  _campos[0], _campos[1], _campos[2]));

        GL_CALL(glUniform1f(uniforms->exposure, view_desc->exposure));
        GL_CALL(glUniform1f(uniforms->env_intensity, view_desc->env_pow));
        GL_CALL(glUniform1i(uniforms->env_mip_count, (int32_t)env_tex->mipCount));
        setup_environment_rotation_matrix(viewer->env_rotation_angle, program);
        GL_CALL(glEnable(GL_CULL_FACE));
        GL_CALL(glDisable(GL_BLEND));
        GL_CALL(glEnable(GL_DEPTH_TEST));
        GL_CALL(glDepthMask(GL_TRUE));
        GL_CALL(glCullFace(GL_BACK));
        uint32_t _texnum = 0;

        bool has_material = asset->materials.size() > (materialIndex - 1);
        if(!has_material) {
            setup_uniform_color_alpha(uniforms->base_color_factor, fastgltf::math::fvec4(1.0f));
            GL_CALL(glUniform1f(uniforms->roughness_factor, 0.5f));
            GL_CALL(glUniform1f(uniforms->metallic_factor,  0.5f));
            GL_CALL(glUniform1f(uniforms->ior, 1.5f));
            GL_CALL(glUniform1f(uniforms->dispersion, 0.0f));
            GL_CALL(glUniform1f(uniforms->thickness, 0.01847f));
        }
        else {
            auto & gltfMaterial = asset->materials[materialIndex - 1];
            if(is_transmission_pass && (gltfMaterial.transmission != NULL)) {
                return;
            }
            if(gltfMaterial.doubleSided) GL_CALL(glDisable(GL_CULL_FACE));
            if(gltfMaterial.alphaMode == fastgltf::AlphaMode::Blend) {
                GL_CALL(glEnable(GL_BLEND));
                //GL_CALL(glDisable(GL_DEPTH_TEST));
                //GL_CALL(glCullFace(GL_FRONT));
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

            // Update any scene lights present

            if(gltf_data->node_by_light_index.size() > 0) {
                size_t max_light_nodes = gltf_data->node_by_light_index.size();
                size_t max_scene_lights = asset->lights.size();
                if(max_scene_lights != max_light_nodes) {
                    LV_LOG_ERROR("Scene light count (%zu) != scene light node count (%zu)\n", max_scene_lights, max_light_nodes);
                }
                else {
                    // Scene contains lights and they may have just moved so update their position within the current shaders
                    // Update any lights present in the scene file
                    for(size_t ii = 0; ii < max_scene_lights; ii++) {
                        size_t i = ii + 1;
                        // Update each field of the light struct
                        std::string _prefix = "u_Lights[" + std::to_string(i) + "].";
                        auto & lightNode = gltf_data->node_by_light_index[ii];
                        fastgltf::math::fmat4x4 lightNodeMat = lv_gltf_data_get_cached_transform(gltf_data, lightNode);
                        const auto & m = lightNodeMat.data();
                        char _targ1[100];

                        strncpy(_targ1, (_prefix + "position").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform3fv(glGetUniformLocation(program, _targ1), 1, &lightNodeMat[3][0]);

                        strncpy(_targ1, (_prefix + "direction").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        fastgltf::math::fvec3 tlight_dir = fastgltf::math::fvec3(-lightNodeMat[2][0], -lightNodeMat[2][1], -lightNodeMat[2][2]);
                        glUniform3fv(glGetUniformLocation(program, _targ1), 1, &tlight_dir[0]);

                        strncpy(_targ1, (_prefix + "range").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        if(asset->lights[ii].range.has_value()) {
                            float light_scale = fastgltf::math::length(fastgltf::math::fvec3(m[0], m[4], m[8]));
                            glUniform1f(glGetUniformLocation(program, _targ1), asset->lights[ii].range.value() * light_scale);
                        }
                        else glUniform1f(glGetUniformLocation(program, _targ1), 9999.f);

                        strncpy(_targ1, (_prefix + "color").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform3fv(glGetUniformLocation(program, _targ1), 1, &(asset->lights[ii].color.data()[0]));

                        strncpy(_targ1, (_prefix + "intensity").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1f(glGetUniformLocation(program, _targ1), asset->lights[ii].intensity);

                        strncpy(_targ1, (_prefix + "innerConeCos").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1f(glGetUniformLocation(program, _targ1),
                                    asset->lights[ii].innerConeAngle.has_value() ? std::cos(asset->lights[ii].innerConeAngle.value()) : -1.0f);

                        strncpy(_targ1, (_prefix + "outerConeCos").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1f(glGetUniformLocation(program, _targ1),
                                    asset->lights[ii].outerConeAngle.has_value() ? std::cos(asset->lights[ii].outerConeAngle.value()) : -1.0f);

                        strncpy(_targ1, (_prefix + "type").c_str(), sizeof(_targ1) - 1);
                        _targ1[sizeof(_targ1) - 1] = '\0';
                        glUniform1i(glGetUniformLocation(program, _targ1), (int)asset->lights[ii].type);
                    }
                }
            }

            setup_uniform_color_alpha(uniforms->base_color_factor, gltfMaterial.pbrData.baseColorFactor);
            setup_uniform_color(uniforms->emissive_factor, gltfMaterial.emissiveFactor);
            GL_CALL(glUniform1f(uniforms->emissive_strength, gltfMaterial.emissiveStrength));
            GL_CALL(glUniform1f(uniforms->roughness_factor, gltfMaterial.pbrData.roughnessFactor));
            GL_CALL(glUniform1f(uniforms->metallic_factor,  gltfMaterial.pbrData.metallicFactor));

            GL_CALL(glUniform1f(uniforms->ior, gltfMaterial.ior));
            GL_CALL(glUniform1f(uniforms->dispersion, gltfMaterial.dispersion));

            if(gltfMaterial.pbrData.baseColorTexture.has_value()) _texnum = setup_texture(_texnum, _prim_data->albedoTexture,
                                                                                              _prim_data->baseColorTexcoordIndex, gltfMaterial.pbrData.baseColorTexture->transform,  uniforms->base_color_sampler,
                                                                                              uniforms->base_color_uv_set, uniforms->base_color_uv_transform);
            if(gltfMaterial.emissiveTexture.has_value()) _texnum = setup_texture(_texnum, _prim_data->emissiveTexture,
                                                                                     _prim_data->emissiveTexcoordIndex, gltfMaterial.emissiveTexture->transform, uniforms->emissive_sampler,
                                                                                     uniforms->emissive_uv_set, uniforms->emissive_uv_transform);
            if(gltfMaterial.pbrData.metallicRoughnessTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                      _prim_data->metalRoughTexture, _prim_data->metallicRoughnessTexcoordIndex,
                                                                                                      gltfMaterial.pbrData.metallicRoughnessTexture->transform, uniforms->metallic_roughness_sampler,
                                                                                                      uniforms->metallic_roughness_uv_set, uniforms->metallic_roughness_uv_transform);
            if(gltfMaterial.occlusionTexture.has_value()) {
                GL_CALL(glUniform1f(uniforms->occlusion_strength, static_cast<float>(gltfMaterial.occlusionTexture->strength)));
                _texnum = setup_texture(_texnum, _prim_data->occlusionTexture, _prim_data->occlusionTexcoordIndex,
                                        gltfMaterial.occlusionTexture->transform, uniforms->occlusion_sampler, uniforms->occlusion_uv_set,
                                        uniforms->occlusion_uv_transform);
            }

            if(gltfMaterial.normalTexture.has_value()) {
                GL_CALL(glUniform1f(uniforms->normal_scale, static_cast<float>(gltfMaterial.normalTexture->scale)));
                _texnum = setup_texture(_texnum, _prim_data->normalTexture, _prim_data->normalTexcoordIndex,
                                        gltfMaterial.normalTexture->transform, uniforms->normal_sampler, uniforms->normal_uv_set,
                                        uniforms->normal_uv_transform);
            }

            if(gltfMaterial.clearcoat) {
                GL_CALL(glUniform1f(uniforms->clearcoat_factor, static_cast<float>(gltfMaterial.clearcoat->clearcoatFactor)));
                GL_CALL(glUniform1f(uniforms->clearcoat_roughness_factor,
                                    static_cast<float>(gltfMaterial.clearcoat->clearcoatRoughnessFactor)));

                if(gltfMaterial.clearcoat->clearcoatTexture.has_value()) _texnum = setup_texture(_texnum, _prim_data->clearcoatTexture,
                                                                                                     _prim_data->clearcoatTexcoordIndex, gltfMaterial.clearcoat->clearcoatTexture->transform, uniforms->clearcoat_sampler,
                                                                                                     uniforms->clearcoat_uv_set, uniforms->clearcoat_uv_transform);
                if(gltfMaterial.clearcoat->clearcoatRoughnessTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                              _prim_data->clearcoatRoughnessTexture, _prim_data->clearcoatRoughnessTexcoordIndex,
                                                                                                              gltfMaterial.clearcoat->clearcoatRoughnessTexture->transform, uniforms->clearcoat_roughness_sampler,
                                                                                                              uniforms->clearcoat_roughness_uv_set, uniforms->clearcoat_roughness_uv_transform);
                if(gltfMaterial.clearcoat->clearcoatNormalTexture.has_value()) {
                    GL_CALL(glUniform1f(uniforms->clearcoat_normal_scale,
                                        static_cast<float>(gltfMaterial.clearcoat->clearcoatNormalTexture->scale)));
                    _texnum = setup_texture(_texnum, _prim_data->clearcoatNormalTexture, _prim_data->clearcoatNormalTexcoordIndex,
                                            gltfMaterial.clearcoat->clearcoatNormalTexture->transform, uniforms->clearcoat_normal_sampler,
                                            uniforms->clearcoat_normal_uv_set, uniforms->clearcoat_normal_uv_transform);
                }
            }

            if(gltfMaterial.volume) {
                GL_CALL(glUniform1f(uniforms->attenuation_distance, gltfMaterial.volume->attenuationDistance));
                setup_uniform_color(uniforms->attenuation_color, gltfMaterial.volume->attenuationColor);
                GL_CALL(glUniform1f(uniforms->thickness, gltfMaterial.volume->thicknessFactor));
                if(gltfMaterial.volume->thicknessTexture.has_value()) {
                    _texnum = setup_texture(_texnum, _prim_data->thicknessTexture, _prim_data->thicknessTexcoordIndex,
                                            gltfMaterial.volume->thicknessTexture->transform, uniforms->thickness_sampler, uniforms->thickness_uv_set,
                                            uniforms->thickness_uv_transform);
                }
            }

            if(gltfMaterial.transmission) {
                GL_CALL(glUniform1f(uniforms->transmission_factor, gltfMaterial.transmission->transmissionFactor));
                GL_CALL(glUniform2i(uniforms->screen_size, (int32_t)view_desc->render_width, (int32_t)view_desc->render_height));
                if(gltfMaterial.transmission->transmissionTexture.has_value()) _texnum = setup_texture(_texnum,
                                                                                                           _prim_data->transmissionTexture, _prim_data->transmissionTexcoordIndex,
                                                                                                           gltfMaterial.transmission->transmissionTexture->transform, uniforms->transmission_sampler,
                                                                                                           uniforms->transmission_uv_set,
                                                                                                           uniforms->transmission_uv_transform);
            }

            if(gltfMaterial.sheen) {
                setup_uniform_color(uniforms->sheen_color_factor, gltfMaterial.sheen->sheenColorFactor);
                GL_CALL(glUniform1f(uniforms->sheen_roughness_factor, static_cast<float>(gltfMaterial.sheen->sheenRoughnessFactor)));
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
                LV_LOG_WARN("Model uses outdated legacy mode pbr_speculargloss. Please update this model to a new shading model ");
                setup_uniform_color_alpha(uniforms->diffuse_factor, gltfMaterial.specularGlossiness->diffuseFactor);
                setup_uniform_color(uniforms->specular_factor, gltfMaterial.specularGlossiness->specularFactor);
                GL_CALL(glUniform1f(uniforms->glossiness_factor,
                                    static_cast<float>(gltfMaterial.specularGlossiness->glossinessFactor)));
                if(gltfMaterial.specularGlossiness->diffuseTexture.has_value()) {
                    _texnum = setup_texture(_texnum, _prim_data->diffuseTexture, _prim_data->diffuseTexcoordIndex,
                                            gltfMaterial.specularGlossiness->diffuseTexture->transform, uniforms->diffuse_sampler,
                                            uniforms->diffuse_uv_set, uniforms->diffuse_uv_transform);
                }
                if(gltfMaterial.specularGlossiness->specularGlossinessTexture.has_value()) {
                    _texnum = setup_texture(_texnum, _prim_data->specularGlossinessTexture, _prim_data->specularGlossinessTexcoordIndex,
                                            gltfMaterial.specularGlossiness->specularGlossinessTexture->transform, uniforms->specular_glossiness_sampler,
                                            uniforms->specular_glossiness_uv_set, uniforms->specular_glossiness_uv_transform);
                }
            }

            if(gltfMaterial.diffuseTransmission) {
                setup_uniform_color(uniforms->diffuse_transmission_color_factor,
                                    gltfMaterial.diffuseTransmission->diffuseTransmissionColorFactor);
                GL_CALL(glUniform1f(uniforms->diffuse_transmission_factor,
                                    static_cast<float>(gltfMaterial.diffuseTransmission->diffuseTransmissionFactor)));
                if(gltfMaterial.diffuseTransmission->diffuseTransmissionTexture.has_value()) {
                    _texnum = setup_texture(_texnum,
                                            _prim_data->diffuseTransmissionTexture, _prim_data->diffuseTransmissionTexcoordIndex,
                                            gltfMaterial.diffuseTransmission->diffuseTransmissionTexture->transform, uniforms->diffuse_transmission_sampler,
                                            uniforms->diffuse_transmission_uv_set, uniforms->diffuse_transmission_uv_transform);
                }
                if(gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture.has_value()) {
                    _texnum = setup_texture(_texnum, _prim_data->diffuseTransmissionColorTexture,
                                            _prim_data->diffuseTransmissionColorTexcoordIndex,
                                            gltfMaterial.diffuseTransmission->diffuseTransmissionColorTexture->transform,
                                            uniforms->diffuse_transmission_color_sampler,
                                            uniforms->diffuse_transmission_color_uv_set, uniforms->diffuse_transmission_color_uv_transform);
                }
            }
        }
        const lv_gltf_view_state_t * vstate = &viewer->state;
        if(!is_transmission_pass && vstate->render_opaque_buffer) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, vstate->opaque_render_state.texture));
            GL_CALL(glUniform1i(uniforms->transmission_framebuffer_sampler, _texnum));
            GL_CALL(glUniform2i(uniforms->transmission_framebuffer_size, (int32_t)vstate->opaque_frame_buffer_width,
                                (int32_t)vstate->opaque_frame_buffer_height));
            _texnum++;
        }

        if(node.skinIndex.has_value()) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, lv_gltf_data_get_skin_texture_at(gltf_data, node.skinIndex.value())));
            GL_CALL(glUniform1i(uniforms->joints_sampler, _texnum));
            _texnum++;
        }
        if(env_tex->diffuse != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex->diffuse));
            GL_CALL(glUniform1i(uniforms->env_diffuse_sampler, _texnum++));
        }
        if(env_tex->specular != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex->specular));
            GL_CALL(glUniform1i(uniforms->env_specular_sampler, _texnum++));
        }
        if(env_tex->sheen != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, env_tex->sheen));
            GL_CALL(glUniform1i(uniforms->env_sheen_sampler, _texnum++));
        }
        if(env_tex->ggxLut != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, env_tex->ggxLut));
            GL_CALL(glUniform1i(uniforms->env_ggx_lut_sampler, _texnum++));
        }
        if(env_tex->charlieLut != GL_NONE) {
            GL_CALL(glActiveTexture(GL_TEXTURE0 + _texnum));
            GL_CALL(glBindTexture(GL_TEXTURE_2D, env_tex->charlieLut));
            GL_CALL(glUniform1i(uniforms->env_charlie_lut_sampler, _texnum++));
        }

    }

    std::size_t index_count = 0;
    auto & indexAccessor = asset->accessors[asset->meshes[mesh_index].primitives[prim_num].indicesAccessor.value()];
    if(indexAccessor.bufferViewIndex.has_value()) {
        index_count = (uint32_t)indexAccessor.count;
    }
    if(index_count > 0) {
        GL_CALL(glDrawElements(_prim_data->primitiveType, index_count, _prim_data->indexType, 0));
    }
}
#endif /*LV_USE_GLTF*/
