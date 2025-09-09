/**
 * @file lv_gltf_view.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_view_internal.h"

#if LV_USE_GLTF

#include "../gltf_data/lv_gltf_model.h"
#include "../gltf_data/lv_gltf_data_internal.hpp"
#include "../../../draw/lv_draw_3d.h"
#include "../fastgltf/lv_fastgltf.hpp"
#include "../../../core/lv_obj_class_private.h"
#include "../../../misc/lv_types.h"
#include "../../../widgets/3dtexture/lv_3dtexture.h"
#include "ibl/lv_gltf_ibl_sampler.h"
#include "assets/lv_gltf_view_shader.h"
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>

/*********************
 *      DEFINES
 *********************/


#define MY_CLASS (&lv_gltf_class)

#ifndef LV_GLTF_INITIAL_MODEL_CAPACITY
    #define LV_GLTF_INITIAL_MODEL_CAPACITY 1
#endif /*LV_GLTF_INITIAL_MODEL_CAPACITY*/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_gltf_model_t * lv_gltf_add_model(lv_gltf_t * viewer, lv_gltf_model_t * model);
static void lv_gltf_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gltf_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj);
static void lv_gltf_event(const lv_obj_class_t * class_p, lv_event_t * e);
static void lv_gltf_view_state_init(lv_gltf_t * state);
static void lv_gltf_view_desc_init(lv_gltf_view_desc_t * state);
static void lv_gltf_parse_model(lv_gltf_t * viewer, lv_gltf_model_t * model);
static void destroy_environment(lv_gltf_view_env_textures_t * env);
static void setup_compile_and_load_bg_shader(lv_opengl_shader_manager_t * manager);
static void setup_background_environment(GLuint program, GLuint * vao, GLuint * indexBuffer, GLuint * vertexBuffer);


const lv_obj_class_t lv_gltf_class = {
    &lv_3dtexture_class,
    lv_gltf_constructor,
    lv_gltf_destructor,
    lv_gltf_event,
#if LV_USE_OBJ_PROPERTY
    0,
    0,
    NULL,
    0,
    NULL,
    0,
#endif
    NULL,
    "lv_gltf",
    LV_DPI_DEF * 2,
    LV_DPI_DEF / 10,
    LV_OBJ_CLASS_EDITABLE_INHERIT,
    LV_OBJ_CLASS_GROUP_DEF_INHERIT,
    sizeof(lv_gltf_t),
    LV_OBJ_CLASS_THEME_INHERITABLE_FALSE
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

lv_obj_t * lv_gltf_create(lv_obj_t * parent)
{
    lv_obj_t * obj = lv_obj_class_create_obj(MY_CLASS, parent);
    lv_obj_class_init_obj(obj);
    return obj;
}

lv_gltf_model_t * lv_gltf_load_model_from_file(lv_obj_t * obj, const char * path)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    lv_gltf_model_t * model = lv_gltf_data_load_from_file(path, viewer->shader_manager);
    return lv_gltf_add_model(viewer, model);
}

lv_gltf_model_t * lv_gltf_load_model_from_bytes(lv_obj_t * obj, const uint8_t * bytes, size_t len)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    lv_gltf_model_t * model = lv_gltf_data_load_from_bytes(bytes, len, viewer->shader_manager);
    return lv_gltf_add_model(viewer, model);
}

size_t lv_gltf_get_model_count(lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);

    return lv_array_size(&((lv_gltf_t *)obj)->models);
}

lv_gltf_model_t * lv_gltf_get_model_by_index(lv_obj_t * obj, size_t id)
{

    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *) obj;

    if(id >= lv_array_size(&viewer->models)) {
        return NULL;
    }
    return *(lv_gltf_model_t **)lv_array_at(&((lv_gltf_t *)obj)->models, id);

}
lv_gltf_model_t * lv_gltf_get_primary_model(lv_obj_t * obj)
{

    return lv_gltf_get_model_by_index(obj, 0);
}

void lv_gltf_set_yaw(lv_obj_t * obj, float yaw)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.yaw = yaw;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_yaw(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.yaw;
}

void lv_gltf_set_pitch(lv_obj_t * obj, float pitch)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.pitch = pitch;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_pitch(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.pitch;
}

void lv_gltf_set_fov(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.fov = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_fov(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.fov;
}

void lv_gltf_set_distance(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.distance = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_distance(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.distance;
}

void lv_gltf_set_animation_speed(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.animation_speed_ratio = value;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_animation_speed(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.animation_speed_ratio;
}

void lv_gltf_set_focal_x(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.focal_x = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_focal_x(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.focal_x;
}

void lv_gltf_set_focal_y(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.focal_y = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_focal_y(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.focal_y;
}

void lv_gltf_set_focal_z(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.focal_z = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_focal_z(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.focal_z;
}

void lv_gltf_set_camera(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(lv_array_is_empty(&viewer->models)) {
        return;
    }

    lv_gltf_model_t * model = *(lv_gltf_model_t **) lv_array_at(&viewer->models, 0);

    if(value > model->asset.cameras.size()) {
        return;
    }

    model->camera = value;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_camera(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(lv_array_is_empty(&viewer->models)) {
        return 0;
    }
    const lv_gltf_model_t * model = *(const lv_gltf_model_t **)lv_array_at(&viewer->models, 0);
    return model->camera;
}

uint32_t lv_gltf_get_camera_count(const lv_obj_t * obj)
{

    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;

    if(lv_array_is_empty(&viewer->models)) {
        return 0;
    }
    const lv_gltf_model_t * model = *(const lv_gltf_model_t **) lv_array_at(&viewer->models, 0);
    return lv_gltf_model_get_camera_count(model);
}

void lv_gltf_set_antialiasing_mode(lv_obj_t * obj, lv_gltf_aa_mode_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.aa_mode = value;
    lv_obj_invalidate(obj);
}

lv_gltf_aa_mode_t lv_gltf_get_antialiasing_mode(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.aa_mode;
}

void lv_gltf_set_background_mode(lv_obj_t * obj, lv_gltf_bg_mode_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.bg_mode = value;
    lv_obj_invalidate(obj);
}

lv_gltf_bg_mode_t lv_gltf_get_background_mode(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.bg_mode;
}

void lv_gltf_set_background_blur(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    if(value > 100) {
        value = 100;
    }
    viewer->desc.blur_bg = value / 100.f;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_background_blur(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.blur_bg * 100;
}

void lv_gltf_set_env_brightness(lv_obj_t * obj, uint32_t value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.env_pow = value / 100.;
    lv_obj_invalidate(obj);
}

uint32_t lv_gltf_get_env_brightness(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.env_pow * 100;
}

void lv_gltf_set_image_exposure(lv_obj_t * obj, float value)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    viewer->desc.exposure = value;
    lv_obj_invalidate(obj);
}

float lv_gltf_get_image_exposure(const lv_obj_t * obj)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    return viewer->desc.exposure;
}
void lv_gltf_recenter(lv_obj_t * obj, lv_gltf_model_t * model)
{
    LV_ASSERT_NULL(obj);
    LV_ASSERT_OBJ(obj, MY_CLASS);
    lv_gltf_t * viewer = (lv_gltf_t *)obj;
    if(model == NULL) {
        LV_ASSERT(lv_array_size(&viewer->models) > 0);
        model = *(lv_gltf_model_t **)lv_array_at(&viewer->models, 0);
    }

    const auto & center_position = lv_gltf_data_get_center(model);
    viewer->desc.focal_x = center_position[0];
    viewer->desc.focal_y = center_position[1];
    viewer->desc.focal_z = center_position[2];
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_gltf_model_t * lv_gltf_add_model(lv_gltf_t * viewer, lv_gltf_model_t * model)
{

    if(!model) {
        return NULL;
    }
    if(lv_array_push_back(&viewer->models, &model) == LV_RESULT_INVALID) {
        lv_gltf_data_destroy(model);
        return NULL;
    }
    model->viewer = viewer;
    lv_gltf_parse_model(viewer, model);


    if(lv_array_size(&viewer->models) == 1) {
        lv_gltf_recenter((lv_obj_t *)viewer, model);
    }

    return model;
}


static void lv_gltf_constructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    LV_TRACE_OBJ_CREATE("begin");
    lv_gltf_t * view = (lv_gltf_t *)obj;
    lv_gltf_view_state_init(view);
    lv_gltf_view_desc_init(&view->desc);
    view->view_matrix = fastgltf::math::fmat4x4(1.0f);
    view->projection_matrix = fastgltf::math::fmat4x4(1.0f);
    view->view_projection_matrix = fastgltf::math::fmat4x4(1.0f);
    view->camera_pos = fastgltf::math::fvec3(0.0f);
    view->texture.h_flip = false;
    view->texture.v_flip = false;
    new(&view->ibm_by_skin_then_node) std::map<int32_t, std::map<fastgltf::Node *, fastgltf::math::fmat4x4>>;

    lv_gltf_view_shader_t shaders;
    lv_gltf_view_shader_get_src(&shaders);
    char * vertex_shader = lv_gltf_view_shader_get_vertex();
    char * frag_shader = lv_gltf_view_shader_get_fragment();
    view->shader_manager = lv_opengl_shader_manager_create(shaders.shader_list, shaders.count, vertex_shader, frag_shader);
    lv_free(vertex_shader);
    lv_free(frag_shader);

    lv_gltf_ibl_generate_env_textures(&view->env_textures, NULL, 0);

    lv_array_init(&view->models, LV_GLTF_INITIAL_MODEL_CAPACITY, sizeof(lv_gltf_model_t *));

    LV_TRACE_OBJ_CREATE("end");
}

static void lv_gltf_event(const lv_obj_class_t * class_p, lv_event_t * e)
{
    LV_UNUSED(class_p);
    lv_event_code_t code = lv_event_get_code(e);

    if(code == LV_EVENT_DRAW_MAIN) {
        lv_obj_t * obj = (lv_obj_t *)lv_event_get_current_target(e);
        lv_gltf_t * viewer = (lv_gltf_t *)obj;
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
static void lv_gltf_destructor(const lv_obj_class_t * class_p, lv_obj_t * obj)
{
    LV_UNUSED(class_p);
    lv_gltf_t * view = (lv_gltf_t *)obj;
    lv_opengl_shader_manager_destroy(view->shader_manager);
    using IbmBySkinThenNodeMap = std::map<int32_t, std::map<fastgltf::Node *, fastgltf::math::fmat4x4>>;

    view->ibm_by_skin_then_node.~IbmBySkinThenNodeMap();
    const size_t n = lv_array_size(&view->models);
    for(size_t i = 0; i < n; ++i) {
        lv_gltf_data_destroy(*(lv_gltf_model_t **)lv_array_at(&view->models, i));
    }
    destroy_environment(&view->env_textures);
}

static void lv_gltf_view_state_init(lv_gltf_t * view)
{
    lv_memset(&view->state, 0, sizeof(view->state));
    view->state.opaque_frame_buffer_width = 256;
    view->state.opaque_frame_buffer_height = 256;
    view->state.material_variant = 0;
    view->state.render_state_ready = false;
    view->state.render_opaque_buffer = false;
}
static void lv_gltf_view_desc_init(lv_gltf_view_desc_t * desc)
{
    lv_memset(desc, 0, sizeof(*desc));
    desc->distance = 2.f;
    desc->exposure = 1.0f;
    desc->env_pow = 1.8f;
    desc->blur_bg = 0.5f;
    desc->bg_mode = LV_GLTF_BG_MODE_ENVIRONMENT;
    desc->aa_mode = LV_GLTF_AA_MODE_OFF;
    desc->fov = 45.f;
    desc->animation_speed_ratio = LV_GLTF_ANIM_SPEED_NORMAL;
    desc->frame_was_antialiased = false;
}
static void lv_gltf_parse_model(lv_gltf_t * viewer, lv_gltf_model_t * model)
{
    const auto & iterate_callback = [&](fastgltf::Node & node, const fastgltf::math::fmat4x4 & matrix) {
        LV_UNUSED(matrix);
        if(!node.meshIndex) {
            return;
        }
        auto & mesh_index = node.meshIndex.value();
        if(node.skinIndex) {
            auto skin_index = node.skinIndex.value();
            if(!lv_gltf_data_validated_skins_contains(model, skin_index)) {
                lv_gltf_data_validate_skin(model, skin_index);
                auto skin = model->asset.skins[skin_index];
                if(skin.inverseBindMatrices) {
                    auto & ibm_value = skin.inverseBindMatrices.value();
                    auto & ibm_accessor = model->asset.accessors[ibm_value];
                    if(ibm_accessor.bufferViewIndex) {
                        fastgltf::iterateAccessorWithIndex<fastgltf::math::fmat4x4>(
                            model->asset, ibm_accessor,
                        [&](fastgltf::math::fmat4x4 _matrix, std::size_t idx) {
                            auto & joint_node = model->asset.nodes[skin.joints[idx]];
                            viewer->ibm_by_skin_then_node[skin_index][&joint_node] = _matrix;
                        });
                    }
                }
            }
        }
        for(size_t mp = 0; mp < model->asset.meshes[mesh_index].primitives.size(); mp++) {
            auto & model_primitive = model->asset.meshes[mesh_index].primitives[mp];
            const auto & mappings = model_primitive.mappings;
            ssize_t material_index =
                (!mappings.empty() && mappings[viewer->state.material_variant]) ?
                mappings[viewer->state.material_variant].value() + 1 :
                ((model_primitive.materialIndex) ? (model_primitive.materialIndex.value() + 1) : 0);
            if(material_index < 0) {
                lv_gltf_data_add_opaque_node_primitive(model, 0, &node, mp);
                continue;
            }
            const fastgltf::Material & material = model->asset.materials[material_index - 1];

            viewer->state.render_opaque_buffer |= material.transmission != NULL;

            if(material.alphaMode == fastgltf::AlphaMode::Blend || material.transmission != NULL) {
                lv_gltf_data_add_blended_node_primitive(model, material_index + 1, &node, mp);
            }
            else {
                lv_gltf_data_add_opaque_node_primitive(model, material_index + 1, &node, mp);
            }

            lv_array_t defines;
            lv_array_init(&defines, 64, sizeof(lv_opengl_shader_define_t));
            lv_result_t result =
                lv_gltf_view_shader_injest_discover_defines(&defines, model, &node, &model_primitive);

            LV_ASSERT_MSG(result == LV_RESULT_OK, "Couldn't injest shader defines");
            lv_gltf_compiled_shader_t compiled_shader;
            compiled_shader.shaderset = lv_gltf_view_shader_compile_program(viewer, (lv_opengl_shader_define_t *)defines.data,
                                                                            lv_array_size(&defines));
            compiled_shader.uniforms = lv_gltf_uniform_locations_create(compiled_shader.shaderset.program);
            lv_gltf_store_compiled_shader(model, material_index, &compiled_shader);
            const size_t n = lv_array_size(&defines);
            for(size_t i = 0; i < n; ++i) {
                lv_opengl_shader_define_t * define = (lv_opengl_shader_define_t *) lv_array_at(&defines, i);
                if(define->value_allocated) {
                    lv_free((void *)define->value);
                }
            }
            lv_array_deinit(&defines);
        }
    };

    setup_compile_and_load_bg_shader(viewer->shader_manager);
    fastgltf::iterateSceneNodes(model->asset, 0, fastgltf::math::fmat4x4(), iterate_callback);
}

static void setup_compile_and_load_bg_shader(lv_opengl_shader_manager_t * manager)
{
    lv_opengl_shader_define_t frag_defs[1] = { { "TONEMAP_KHR_PBR_NEUTRAL", NULL, false} };

    uint32_t frag_shader_hash = lv_opengl_shader_manager_select_shader(manager, "cubemap.frag", frag_defs, 1);
    uint32_t vert_shader_hash = lv_opengl_shader_manager_select_shader(manager, "cubemap.vert", nullptr, 0);

    lv_opengl_shader_program_t * program = lv_opengl_shader_manager_get_program(manager, frag_shader_hash,
                                                                                vert_shader_hash);

    manager->bg_program = lv_opengl_shader_program_get_id(program);
    setup_background_environment(manager->bg_program, &manager->bg_vao, &manager->bg_index_buf, &manager->bg_vertex_buf);
}


static void setup_background_environment(GLuint program, GLuint * vao, GLuint * indexBuffer, GLuint * vertexBuffer)
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

static void destroy_environment(lv_gltf_view_env_textures_t * env)
{
    const unsigned int d[3] = { env->diffuse, env->specular, env->sheen };
    GL_CALL(glDeleteTextures(3, d));
}

#endif /*LV_USE_GLTF*/
