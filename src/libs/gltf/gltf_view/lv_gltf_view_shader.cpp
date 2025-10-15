/**
 * @file lv_gltf_view_shader.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gltf_view_internal.h"
#if LV_USE_GLTF

#include "fastgltf/types.hpp"
#include "../gltf_data/lv_gltf_data_internal.hpp"
#include "../gltf_data/lv_gltf_data_internal.h"
#include "../../../drivers/opengles/opengl_shader/lv_opengl_shader_internal.h"
#include "../../../misc/lv_array.h"
#include "../../../misc/lv_assert.h"
#include "../../../misc/lv_types.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../stdlib/lv_string.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static lv_result_t add_define(lv_array_t * array, const char * defsymbol, const char * value, bool value_allocated);
static lv_result_t add_define_if_primitive_attribute_exists(lv_array_t * array, const fastgltf::Asset & asset,
                                                            const fastgltf::Primitive * primitive, const char * attribute,
                                                            const char * define);

static lv_result_t add_texture_defines_impl(lv_array_t * array, const fastgltf::TextureInfo & material_prop,
                                            const char * define,
                                            const char * uv_define);

static lv_result_t add_texture_defines(lv_array_t * array,
                                       const fastgltf::Optional<fastgltf::TextureInfo> & material_prop,
                                       const char * define, const char * uv_define);

static lv_result_t add_texture_defines(lv_array_t * array,
                                       const fastgltf::Optional<fastgltf::NormalTextureInfo> & material_prop,
                                       const char * define, const char * uv_define);

static lv_result_t add_texture_defines(lv_array_t * array,
                                       const fastgltf::Optional<fastgltf::OcclusionTextureInfo> & material_prop,
                                       const char * define, const char * uv_define);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_gltf_view_shader_injest_discover_defines(lv_array_t * result, lv_gltf_model_t * data,
                                                        fastgltf::Node * node,
                                                        fastgltf::Primitive * prim)
{
    const auto & asset = data->asset;

    if(add_define(result, "_OPAQUE", "0", false) == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define(result, "_MASK", "1", false) == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define(result, "_BLEND", "2", false) == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }

    LV_ASSERT_MSG(prim->findAttribute("POSITION") != prim->attributes.end(),
                  "A mesh primitive is required to hold the POSITION attribute");
    LV_ASSERT_MSG(prim->indicesAccessor.has_value(),
                  "We specify fastgltf::Options::GenerateMeshIndices, so we should always have indices");

    if(!prim->materialIndex.has_value()) {
        if(add_define(result, "ALPHAMODE", "_OPAQUE", false) == LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
    }
    else {
        const auto & material = asset.materials[prim->materialIndex.value()];
        if(add_define(result, "TONEMAP_KHR_PBR_NEUTRAL", NULL, false) == LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
        if(material.unlit) {
            if(add_define(result, "MATERIAL_UNLIT", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_define(result, "LINEAR_OUTPUT", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }
        else {
            if(add_define(result, "MATERIAL_METALLICROUGHNESS", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_define(result, "LINEAR_OUTPUT", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }
        const size_t light_count = data->node_by_light_index.size();
        if(add_define(result, "USE_IBL", NULL, false) == LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
        if(light_count > 10) {
            LV_LOG_ERROR("Too many scene lights, max is 10");
        }
        else if(light_count > 0) {
            if(add_define(result, "USE_PUNCTUAL", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            char * count = (char *) lv_zalloc(5);
            lv_snprintf(count, 5, "%zu", light_count);
            if(add_define(result, "LIGHT_COUNT", count, true) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }
        else {
            if(add_define(result, "LIGHT_COUNT", "0", false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }

        // only set cutoff value for mask material
        if(material.alphaMode == fastgltf::AlphaMode::Mask) {
            if(add_define(result, "ALPHAMODE", "_MASK", false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }
        else if(material.alphaMode == fastgltf::AlphaMode::Opaque) {
            if(add_define(result, "ALPHAMODE", "_OPAQUE", false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }
        else {
            if(add_define(result, "ALPHAMODE", "_BLEND", false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }
        if(add_texture_defines(result, material.pbrData.baseColorTexture, "HAS_BASE_COLOR_MAP",
                               "HAS_BASECOLOR_UV_TRANSFORM") == LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
        if(add_texture_defines(result, material.pbrData.metallicRoughnessTexture, "HAS_METALLIC_ROUGHNESS_MAP",
                               "HAS_METALLICROUGHNESS_UV_TRANSFORM") == LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
        if(add_texture_defines(result, material.occlusionTexture, "HAS_OCCLUSION_MAP", "HAS_OCCLUSION_UV_TRANSFORM") ==
           LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
        if(add_texture_defines(result, material.normalTexture, "HAS_NORMAL_MAP", "HAS_NORMAL_UV_TRANSFORM") ==
           LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
        if(add_texture_defines(result, material.emissiveTexture, "HAS_EMISSIVE_MAP", "HAS_EMISSIVE_UV_TRANSFORM") ==
           LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }

        if(add_define(result, "MATERIAL_EMISSIVE_STRENGTH", NULL, false) == LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
        if(material.sheen)
            if(add_define(result, "MATERIAL_SHEEN", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        if(material.specular)
            if(add_define(result, "MATERIAL_SPECULAR", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        if(material.specularGlossiness) {
            if(add_define(result, "MATERIAL_SPECULARGLOSSINESS", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_texture_defines(result, material.specularGlossiness->diffuseTexture, "HAS_DIFFUSE_MAP",
                                   "HAS_DIFFUSE_UV_TRANSFORM")) {
                return LV_RESULT_INVALID;
            }
            if(add_texture_defines(result, material.specularGlossiness->specularGlossinessTexture,
                                   "HAS_SPECULARGLOSSINESS_MAP", "HAS_SPECULARGLOSSINESS_UV_TRANSFORM")) {
                return LV_RESULT_INVALID;
            }
        }
        if(material.transmission) {
            if(add_define(result, "MATERIAL_TRANSMISSION", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_define(result, "MATERIAL_DISPERSION", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_define(result, "MATERIAL_VOLUME", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(material.transmission->transmissionTexture.has_value())
                if(add_define(result, "HAS_TRANSMISSION_MAP", NULL, false) == LV_RESULT_INVALID) {
                    return LV_RESULT_INVALID;
                }
            if(material.volume) {
                add_texture_defines(result, material.volume->thicknessTexture, "HAS_THICKNESS_MAP",
                                    "HAS_THICKNESS_UV_TRANSFORM");
            }
        }
        if(material.clearcoat) {
            if(add_define(result, "MATERIAL_CLEARCOAT", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_texture_defines(result, material.clearcoat->clearcoatTexture, "HAS_CLEARCOAT_MAP",
                                   "HAS_CLEARCOAT_UV_TRANSFORM") == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_texture_defines(result, material.clearcoat->clearcoatRoughnessTexture,
                                   "HAS_CLEARCOAT_ROUGHNESS_MAP",
                                   "HAS_CLEARCOATROUGHNESS_UV_TRANSFORM") == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(add_texture_defines(result, material.clearcoat->clearcoatNormalTexture, "HAS_CLEARCOAT_NORMAL_MAP",
                                   "HAS_CLEARCOATNORMAL_UV_TRANSFORM") == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
        }
        if(material.diffuseTransmission) {
            if(add_define(result, "MATERIAL_DIFFUSE_TRANSMISSION", NULL, false) == LV_RESULT_INVALID) {
                return LV_RESULT_INVALID;
            }
            if(material.diffuseTransmission->diffuseTransmissionTexture.has_value()) {
                if(add_define(result, "HAS_DIFFUSE_TRANSMISSION_MAP", NULL, false) == LV_RESULT_INVALID) {
                    return LV_RESULT_INVALID;
                }
            }
            if(material.diffuseTransmission->diffuseTransmissionColorTexture.has_value()) {
                if(add_define(result, "HAS_DIFFUSE_TRANSMISSION_COLOR_MAP", NULL, false) == LV_RESULT_INVALID) {
                    return LV_RESULT_INVALID;
                }
            }
        }
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "NORMAL", "HAS_NORMAL_VEC3") == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "TANGENT", "HAS_TANGENT_VEC4") == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "TEXCOORD_0", "HAS_TEXCOORD_0_VEC2") ==
       LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "TEXCOORD_1", "HAS_TEXCOORD_1_VEC2") ==
       LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "JOINTS_0",
                                                "HAS_JOINTS_0_VEC4") == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "JOINTS_1",
                                                "HAS_JOINTS_1_VEC4") == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "WEIGHTS_0", "HAS_WEIGHTS_0_VEC4") ==
       LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(add_define_if_primitive_attribute_exists(result, asset, prim, "WEIGHTS_1", "HAS_WEIGHTS_1_VEC4") ==
       LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }

    const auto * joints0it = prim->findAttribute("JOINTS_0");
    const auto * weights0it = prim->findAttribute("WEIGHTS_0");
    if((node->skinIndex.has_value()) && (joints0it != prim->attributes.end()) && (weights0it != prim->attributes.end())) {
        if(add_define(result, "USE_SKINNING", NULL, false) == LV_RESULT_INVALID) {
            return LV_RESULT_INVALID;
        }
    }
    return LV_RESULT_OK;
}

/**
 * @brief Compile and load shaders.
 *
 * This function compiles and loads the shaders from the specified shader cache, preparing them
 * for use in rendering operations. It returns a structure containing the shader set information.
 *
 * @param shaders Pointer to the lv_opengl_shader_cache_t structure containing the shader cache.
 * @return A gl_renwin_shaderset_t structure representing the compiled and loaded shaders.
 */

lv_gltf_shaderset_t lv_gltf_view_shader_compile_program(lv_gltf_t * view, const lv_opengl_shader_define_t * defines,
                                                        size_t n)
{
    uint32_t frag_shader_hash;
    uint32_t vert_shader_hash;
    lv_result_t res = lv_opengl_shader_manager_select_shader(&view->shader_manager, "__MAIN__.frag",
                                                             defines, n, LV_OPENGL_GLSL_VERSION_300ES, &frag_shader_hash);
    LV_ASSERT(res == LV_RESULT_OK);
    res = lv_opengl_shader_manager_select_shader(&view->shader_manager, "__MAIN__.vert",
                                                 defines, n, LV_OPENGL_GLSL_VERSION_300ES, &vert_shader_hash);
    LV_ASSERT(res == LV_RESULT_OK);
    lv_opengl_shader_program_t * program =
        lv_opengl_shader_manager_get_program(&view->shader_manager, frag_shader_hash, vert_shader_hash);

    LV_ASSERT_MSG(program != NULL,
                  "Failed to link program. This probably means your platform doesn't support GLSL version 300 es");

    GLuint program_id = lv_opengl_shader_program_get_id(program);

    GL_CALL(glUseProgram(program_id));
    lv_gltf_shaderset_t shader_prog;
    shader_prog.program = program_id;

    return shader_prog;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_result_t add_define(lv_array_t * array, const char * name, const char * value, bool value_allocated)
{
    const size_t n = lv_array_size(array);
    for(size_t i = 0; i < n; ++i) {
        lv_opengl_shader_define_t * define = (lv_opengl_shader_define_t *)lv_array_at(array, i);
        if(lv_streq(define->name, name)) {
            return LV_RESULT_OK;
        }
    }

    lv_opengl_shader_define_t entry = { name, value, value_allocated };
    return lv_array_push_back(array, &entry);
}

static lv_result_t add_define_if_primitive_attribute_exists(lv_array_t * array, const fastgltf::Asset & asset,
                                                            const fastgltf::Primitive * primitive, const char * attribute,
                                                            const char * define)
{
    const auto & it = primitive->findAttribute(attribute);
    if(it == primitive->attributes.end() || !asset.accessors[it->accessorIndex].bufferViewIndex.has_value()) {
        return LV_RESULT_OK;
    }
    return add_define(array, define, NULL, false);
}

static lv_result_t add_texture_defines_impl(lv_array_t * array, const fastgltf::TextureInfo & material_prop,
                                            const char * define,
                                            const char * uv_define)
{
    if(add_define(array, define, NULL, false) == LV_RESULT_INVALID) {
        return LV_RESULT_INVALID;
    }
    if(!material_prop.transform) {
        return LV_RESULT_OK;
    }
    return add_define(array, uv_define, NULL, false);
}

static lv_result_t add_texture_defines(lv_array_t * array,
                                       const fastgltf::Optional<fastgltf::TextureInfo> & material_prop,
                                       const char * define, const char * uv_define)
{
    if(!material_prop.has_value()) {
        return LV_RESULT_OK;
    }
    return add_texture_defines_impl(array, material_prop.value(), define, uv_define);
}

static lv_result_t add_texture_defines(lv_array_t * array,
                                       const fastgltf::Optional<fastgltf::NormalTextureInfo> & material_prop,
                                       const char * define, const char * uv_define)
{
    if(!material_prop.has_value()) {
        return LV_RESULT_OK;
    }
    return add_texture_defines_impl(array, material_prop.value(), define, uv_define);
}

static lv_result_t add_texture_defines(lv_array_t * array,
                                       const fastgltf::Optional<fastgltf::OcclusionTextureInfo> & material_prop,
                                       const char * define, const char * uv_define)
{
    if(!material_prop.has_value()) {
        return LV_RESULT_OK;
    }
    return add_texture_defines_impl(array, material_prop.value(), define, uv_define);
}

#endif /*LV_USE_GLTF*/
