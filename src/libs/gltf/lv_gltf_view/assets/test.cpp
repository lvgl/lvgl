
#include "../../lv_gltf_data/lv_gltf_data_internal.hpp"
#if LV_USE_GLTF
#include "fastgltf/types.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strlen, strcpy: for replaceWord and lineCount */

#include "test.h"

#include <sstream>  /* for replaceWord */
#include <vector>   /* for addDefine, clearDefines */
#include <string>
#include <stdint.h>
#include "../../lv_gltf_data/lv_gltf_data_internal.h"
#include "../../lv_gl_shader/lv_gl_shader_internal.h"

bool _vdefines_initialized = false;
std::vector<lv_gl_shader_t> v_defines;

const  char * src_fragmentShader_override;
bool src_fragmentShader_has_override = false;
void lv_gltf_view_shader_fragment_override(const char * override_fragment_code)
{
    src_fragmentShader_has_override = true;
    src_fragmentShader_override = override_fragment_code;
    printf("Establishing fragment shader override. Override: %s\n", src_fragmentShader_has_override ? "true" : "false");
}

const char * src_vertexShader_override;
bool src_vertexShader_has_override = false;
void lv_gltf_view_shader_vertex_override(const char * override_vertex_code)
{
    src_vertexShader_has_override = true;
    src_vertexShader_override = override_vertex_code;
    printf("Establishing vertex shader override. Override: %s\n", src_vertexShader_has_override ? "true" : "false");
}

bool shader_fragment_is_overridden(void)
{
    return src_fragmentShader_has_override;
}
const char * get_shader_fragment_override(void)
{
    return src_fragmentShader_override;
}

bool shader_vertex_is_overridden(void)
{
    return src_vertexShader_has_override;
}
const char * get_shader_vertex_override(void)
{
    return src_vertexShader_override;
}

void __init_vdefines(void)
{
    if(_vdefines_initialized) return;
    _vdefines_initialized = true;
    v_defines = std::vector<lv_gl_shader_t>();
}

lv_gl_shader_t * all_defines(void)
{
    return &v_defines[0];
}
uint32_t all_defines_count(void)
{
    return v_defines.size();
}

void clear_defines(void)
{
    __init_vdefines();
    v_defines.erase(v_defines.begin(), v_defines.end());
    v_defines.clear();
    v_defines.shrink_to_fit();
}

void add_define(const char * defsymbol, const char * defvalue_or_null)
{
    __init_vdefines();
    for(auto & _kv : v_defines) {
        if(strcmp(_kv.name, defsymbol) == 0) {
            return;
        }
    }
    lv_gl_shader_t _newkv = {defsymbol, defvalue_or_null};
    v_defines.push_back(_newkv);
}

char * getDefineId(void)
{
    char * _tretstr = (char *)malloc(1024);
    _tretstr[0] = '\0';
    bool _firstpass = true;
    for(auto & _kv : v_defines) {
        if(!_firstpass) {
            strcat(_tretstr, "|");
        }
        _firstpass = false;
        strcat(_tretstr, _kv.name);
        if(_kv.source) {
            strcat(_tretstr, _kv.name);
        }
    }
    return _tretstr;
}

std::string replaceWord(std::string s, const char * c_f, const char * c_r)
{
    //std::string s = std::string(c_s);
    std::string f = std::string(c_f);
    std::string r = std::string(c_r);

    if(s.empty() || f.empty() || f == r || s.find(f) == std::string::npos) {
        return (s);
    }

    std::ostringstream build_it;
    size_t i = 0;
    for(size_t pos; (pos = s.find(f, i)) != std::string::npos;) {
        build_it.write(&s[i], pos - i);
        build_it << r;
        i = pos + f.size();
    }
    if(i != s.size()) {
        build_it.write(&s[i], s.size() - i);
    }
    return build_it.str();
}

size_t lineCount(const char * str)
{
    size_t count = 0;
    while(*str) {
        if(*str == '\n') {
            count++;
        }
        str++;
    }
    return count;
}

char * PREPROCESS(const char * x)
{
    char * _def = get_defines_str();
    char * _ret = process_includes(x, _def);
    free(_def);
    return _ret;
}

char * get_defines_str(void)
{
    __init_vdefines();
    std::string _ret = std::string(GLSL_VERSION_PREFIX) + std::string("\n");

    for(auto & _kv : v_defines) {
        _ret += "#define " + std::string(_kv.name);
        if(_kv.source != NULL) {
            _ret += " " + std::string(_kv.source);
        }
        _ret += "\n";
    }
    char * _retcstr = (char *)malloc(_ret.length() + 1);
    _retcstr[0] = '\0';
    strcpy(_retcstr, _ret.c_str());
    return _retcstr;
}

char * process_defines(const lv_gl_shader_t * __define_set, size_t _num_items)
{
    uint32_t _reqlength = strlen(GLSL_VERSION_PREFIX) + 1;
    for(size_t i = 0; i < _num_items; i++) {
        _reqlength += strlen("#define ");
        _reqlength += strlen(__define_set[i].name);
        if(__define_set[i].source != NULL) {
            _reqlength += strlen(" ");
            _reqlength += strlen(__define_set[i].source);
        }
        _reqlength += strlen("\n");
    }
    char * ret = (char *)malloc(_reqlength + 1);
    ret[0] = '\0';
    strcat(ret, GLSL_VERSION_PREFIX);
    strcat(ret, "\n");
    for(size_t i = 0; i < _num_items; i++) {
        strcat(ret, "#define ");
        strcat(ret, __define_set[i].name);
        if(__define_set[i].source != NULL) {
            strcat(ret, " ");
            strcat(ret, __define_set[i].source);
        }
        strcat(ret, "\n");
    }
    //std::cout << "SHADER DEFINES:\n==============\n" << ret << "==============\n";
    return ret;
}

char * process_includes(const char * c_src, const char * _defines)
{
    std::string _src = std::string(c_src);
    std::string  rep = replaceWord(_src, GLSL_VERSION_PREFIX, _defines);
    size_t num_items = sizeof(src_includes) / sizeof(lv_gl_shader_t);
    char * _srch = (char *)malloc(255);
    for(size_t i = 0; i < num_items; i++) {
        _srch[0] = '\0';
        strcat(_srch, "\n#include <");
        strcat(_srch, src_includes[i].name);
        strcat(_srch, ">");
        rep = replaceWord(rep, _srch, src_includes[i].source);
    }
    free(_srch);
    char * retval = (char *)malloc(rep.length() + 1);
    retval[0] = '\0';
    strcat(retval, rep.c_str());
    return retval;
}

lv_gltf_renwin_shaderset_t
setup_compile_and_load_shaders(lv_gl_shader_manager_t * manager)
{
    lv_gl_shader_t * all_defs = all_defines();
    uint32_t frag_shader_hash = lv_gl_shader_manager_select_shader(
                                    manager, "__MAIN__.frag", all_defs, all_defines_count());

    uint32_t vert_shader_hash = lv_gl_shader_manager_select_shader(
                                    manager, "__MAIN__.vert", all_defs, all_defines_count());

    LV_LOG_USER("%d %d", frag_shader_hash, vert_shader_hash);
    lv_gl_shader_program_t * program = lv_gl_shader_manager_get_program(
                                           manager, frag_shader_hash, vert_shader_hash);

    LV_ASSERT_NULL(program);

    GLuint program_id = lv_gl_shader_program_get_id(program);

    LV_LOG_USER("Program used %d", program_id);
    GL_CALL(glUseProgram(program_id));
    lv_gltf_renwin_shaderset_t _shader_prog;
    _shader_prog.program = program_id;
    _shader_prog.ready = true;

    return _shader_prog;
}

static void add_define_if_primitive_attribute_exists(
    const fastgltf::Asset & asset, const fastgltf::Primitive * primitive,
    const char * attribute, const char * define);

static void add_texture_defines_impl(const fastgltf::TextureInfo & material_prop,
                                     const char * define, const char * uv_define);

static void add_texture_defines(
    const fastgltf::Optional<fastgltf::TextureInfo> & material_prop,
    const char * define, const char * uv_define);

static void add_texture_defines(
    const fastgltf::Optional<fastgltf::NormalTextureInfo> & material_prop,
    const char * define, const char * uv_define);

static void add_texture_defines(
    const fastgltf::Optional<fastgltf::OcclusionTextureInfo> & material_prop,
    const char * define, const char * uv_define);



void injest_discover_defines(lv_gltf_data_t * data,
                                          fastgltf::Node * node,
                                          fastgltf::Primitive * prim)
{
    const auto & asset = data->asset;
    clear_defines();

    add_define("_OPAQUE", "0");
    add_define("_MASK", "1");
    add_define("_BLEND", "2");

    LV_ASSERT_MSG(
        prim->findAttribute("POSITION") != prim->attributes.end(),
        "A mesh primitive is required to hold the POSITION attribute");
    LV_ASSERT_MSG(
        prim->indicesAccessor.has_value(),
        "We specify fastgltf::Options::GenerateMeshIndices, so we should always have indices ");

    if(!prim->materialIndex.has_value()) {
        add_define("ALPHAMODE", "_OPAQUE");
    }
    else {
        const auto & material =
            asset.materials[prim->materialIndex.value()];
        add_define("TONEMAP_KHR_PBR_NEUTRAL", NULL);
        if(material.unlit) {
            add_define("MATERIAL_UNLIT", NULL);
            add_define("LINEAR_OUTPUT", NULL);
        }
        else {
            add_define("MATERIAL_METALLICROUGHNESS", NULL);
            add_define("LINEAR_OUTPUT", NULL);
        }
        const size_t light_count = data->node_by_light_index.size();
        add_define("USE_IBL", NULL);
        if(light_count > 10) {
            LV_LOG_ERROR("Too many scene lights, max is 10");
        } else if (light_count > 0) {
            add_define("USE_PUNCTUAL", NULL);
            char tmp[5];
            snprintf(tmp, sizeof(tmp), "%zu", light_count);
            add_define("LIGHT_COUNT", tmp);
            LV_LOG_INFO("Added %s lights to shader", tmp);
        }
        else {
            add_define("LIGHT_COUNT", "0");
        }

        // only set cutoff value for mask material
        if(material.alphaMode == fastgltf::AlphaMode::Mask) {
            add_define("ALPHAMODE", "_MASK");
        }
        else if(material.alphaMode == fastgltf::AlphaMode::Opaque) {
            add_define("ALPHAMODE", "_OPAQUE");
        }
        else {
            add_define("ALPHAMODE", "_BLEND");
        }
        add_texture_defines(material.pbrData.baseColorTexture,
                            "HAS_BASE_COLOR_MAP",
                            "HAS_BASECOLOR_UV_TRANSFORM");
        add_texture_defines(material.pbrData.metallicRoughnessTexture,
                            "HAS_METALLIC_ROUGHNESS_MAP",
                            "HAS_METALLICROUGHNESS_UV_TRANSFORM");
        add_texture_defines(material.occlusionTexture,
                            "HAS_OCCLUSION_MAP",
                            "HAS_OCCLUSION_UV_TRANSFORM");
        add_texture_defines(material.normalTexture, "HAS_NORMAL_MAP",
                            "HAS_NORMAL_UV_TRANSFORM");
        add_texture_defines(material.emissiveTexture,
                            "HAS_EMISSIVE_MAP",
                            "HAS_EMISSIVE_UV_TRANSFORM");
        add_define("MATERIAL_EMISSIVE_STRENGTH", NULL);
        if(material.sheen)
            add_define("MATERIAL_SHEEN", NULL);
        if(material.specular)
            add_define("MATERIAL_SPECULAR", NULL);
        if(material.specularGlossiness) {
            add_define("MATERIAL_SPECULARGLOSSINESS", NULL);
            add_texture_defines(
                material.specularGlossiness->diffuseTexture,
                "HAS_DIFFUSE_MAP", "HAS_DIFFUSE_UV_TRANSFORM");
            add_texture_defines(
                material.specularGlossiness
                ->specularGlossinessTexture,
                "HAS_SPECULARGLOSSINESS_MAP",
                "HAS_SPECULARGLOSSINESS_UV_TRANSFORM");
        }
        if(material.transmission) {
            add_define("MATERIAL_TRANSMISSION", NULL);
            add_define("MATERIAL_DISPERSION", NULL);
            add_define("MATERIAL_VOLUME", NULL);
            if(material.transmission->transmissionTexture
               .has_value())
                add_define("HAS_TRANSMISSION_MAP", NULL);
            if(material.volume) {
                add_texture_defines(
                    material.volume->thicknessTexture,
                    "HAS_THICKNESS_MAP",
                    "HAS_THICKNESS_UV_TRANSFORM");
            }
        }
        if(material.clearcoat) {
            add_define("MATERIAL_CLEARCOAT", NULL);
            add_texture_defines(
                material.clearcoat->clearcoatTexture,
                "HAS_CLEARCOAT_MAP",
                "HAS_CLEARCOAT_UV_TRANSFORM");
            add_texture_defines(
                material.clearcoat->clearcoatRoughnessTexture,
                "HAS_CLEARCOAT_ROUGHNESS_MAP",
                "HAS_CLEARCOATROUGHNESS_UV_TRANSFORM");
            add_texture_defines(
                material.clearcoat->clearcoatNormalTexture,
                "HAS_CLEARCOAT_NORMAL_MAP",
                "HAS_CLEARCOATNORMAL_UV_TRANSFORM");
        }
        if(material.diffuseTransmission) {
            add_define("MATERIAL_DIFFUSE_TRANSMISSION", NULL);
            if(material.diffuseTransmission
               ->diffuseTransmissionTexture.has_value()) {
                add_define("HAS_DIFFUSE_TRANSMISSION_MAP",
                           NULL);
            }
            if(material.diffuseTransmission
               ->diffuseTransmissionColorTexture
               .has_value()) {
                add_define("HAS_DIFFUSE_TRANSMISSION_COLOR_MAP",
                           NULL);
            }
        }
    }
    add_define_if_primitive_attribute_exists(asset, prim, "NORMAL",
                                             "HAS_NORMAL_VEC3");
    add_define_if_primitive_attribute_exists(asset, prim, "TANGENT",
                                             "HAS_TANGENT_VEC4");
    add_define_if_primitive_attribute_exists(asset, prim, "TEXCOORD_0",
                                             "HAS_TEXCOORD_0_VEC2");
    add_define_if_primitive_attribute_exists(asset, prim, "TEXCOORD_1",
                                             "HAS_TEXCOORD_1_VEC2");
    add_define_if_primitive_attribute_exists(asset, prim, "JOINTS_0",
                                             "HAS_JOINTS_0_VEC4");
    add_define_if_primitive_attribute_exists(asset, prim, "JOINTS_1",
                                             "HAS_JOINTS_1_VEC4");
    add_define_if_primitive_attribute_exists(asset, prim, "WEIGHTS_0",
                                             "HAS_WEIGHTS_0_VEC4");
    add_define_if_primitive_attribute_exists(asset, prim, "WEIGHTS_1",
                                             "HAS_WEIGHTS_1_VEC4");

    const auto * joints0it = prim->findAttribute("JOINTS_0");
    const auto * weights0it = prim->findAttribute("WEIGHTS_0");
    if((node->skinIndex.has_value()) &&
       (joints0it != prim->attributes.end()) &&
       (weights0it != prim->attributes.end())) {
        add_define("USE_SKINNING", NULL);
    }
}
static void add_define_if_primitive_attribute_exists(
    const fastgltf::Asset & asset, const fastgltf::Primitive * primitive,
    const char * attribute, const char * define)
{
    const auto & it = primitive->findAttribute(attribute);
    if(it == primitive->attributes.end() ||
       !asset.accessors[it->accessorIndex].bufferViewIndex.has_value()) {
        return;
    }
    add_define(define, NULL);
}

static void add_texture_defines_impl(const fastgltf::TextureInfo & material_prop,
                                     const char * define, const char * uv_define)
{
    add_define(define, NULL);
    if(material_prop.transform) {
        add_define(uv_define, NULL);
    }
}

static void add_texture_defines(
    const fastgltf::Optional<fastgltf::TextureInfo> & material_prop,
    const char * define, const char * uv_define)
{
    if(!material_prop.has_value()) {
        return;
    }
    add_texture_defines_impl(material_prop.value(), define, uv_define);
}

static void add_texture_defines(
    const fastgltf::Optional<fastgltf::NormalTextureInfo> & material_prop,
    const char * define, const char * uv_define)
{
    if(!material_prop.has_value()) {
        return;
    }
    add_texture_defines_impl(material_prop.value(), define, uv_define);
}

static void add_texture_defines(
    const fastgltf::Optional<fastgltf::OcclusionTextureInfo> & material_prop,
    const char * define, const char * uv_define)
{
    if(!material_prop.has_value()) {
        return;
    }
    add_texture_defines_impl(material_prop.value(), define, uv_define);
}

#endif /*LV_USE_GLTF*/
