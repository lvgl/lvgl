/**
 * @file lv_gltf_data_injest.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_gltf_data_internal.hpp"

#if LV_USE_GLTF

#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include "../fastgltf/lv_fastgltf.hpp"
#include "../../../misc/lv_assert.h"
#include "../../../misc/lv_log.h"
#include "../../../misc/lv_math.h"
#include "../../../stdlib/lv_sprintf.h"
#include "../../../misc/lv_fs.h"

#include "../stb_image/stb_image.h"
#include <webp/decode.h>

/*********************
 *      DEFINES
 *********************/
constexpr auto SUPPORTED_EXTENSIONS =
    //fastgltf::Extensions::KHR_draco_mesh_compression |
    //fastgltf::Extensions::EXT_meshopt_compression |
    fastgltf::Extensions::KHR_mesh_quantization | fastgltf::Extensions::KHR_texture_transform |
    fastgltf::Extensions::KHR_lights_punctual | fastgltf::Extensions::KHR_materials_anisotropy |
    fastgltf::Extensions::KHR_materials_clearcoat | fastgltf::Extensions::KHR_materials_dispersion |
    fastgltf::Extensions::KHR_materials_emissive_strength | fastgltf::Extensions::KHR_materials_ior |
    fastgltf::Extensions::KHR_materials_iridescence | fastgltf::Extensions::KHR_materials_sheen |
    fastgltf::Extensions::KHR_materials_specular |
    fastgltf::Extensions::
    KHR_materials_pbrSpecularGlossiness
    | // Depreciated, to enable support make sure to define FASTGLTF_ENABLE_DEPRECATED_EXT
    fastgltf::Extensions::KHR_materials_transmission |
    fastgltf::Extensions::KHR_materials_volume | fastgltf::Extensions::KHR_materials_unlit |
    fastgltf::Extensions::EXT_texture_webp |
    //fastgltf::Extensions::KHR_materials_diffuse_transmission |
    fastgltf::Extensions::KHR_materials_variants;

constexpr auto GLTF_OPTIONS = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble |
                              fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages |
                              fastgltf::Options::GenerateMeshIndices;

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    fastgltf::math::fvec3 position;
    fastgltf::math::fvec3 normal;
    fastgltf::math::fvec4 tangent;
    fastgltf::math::fvec2 uv;
    fastgltf::math::fvec2 uv2;
    fastgltf::math::fvec4 joints;
    fastgltf::math::fvec4 joints2;
    fastgltf::math::fvec4 weights;
    fastgltf::math::fvec4 weights2;
} vertex_t;

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void set_bounds_info(lv_gltf_model_t * data, fastgltf::math::fvec3 v_min, fastgltf::math::fvec3 v_max,
                            fastgltf::math::fvec3 v_cen, float radius);
static lv_gltf_model_t * create_data_from_bytes(const uint8_t * bytes, size_t data_size);

static lv_gltf_model_t * create_data_from_file(const char * path);

static void injest_grow_bounds_to_include(lv_gltf_model_t * data, const fastgltf::math::fmat4x4 & matrix,
                                          const fastgltf::Mesh & mesh);

static void injest_set_initial_bounds(lv_gltf_model_t * data, const fastgltf::math::fmat4x4 & matrix,
                                      const fastgltf::Mesh & mesh);

static bool injest_image(lv_opengl_shader_manager_t * shader_manager, lv_gltf_model_t * data, fastgltf::Image & image,
                         uint32_t index);

static bool injest_image_from_buffer_view(lv_gltf_model_t * data, fastgltf::sources::BufferView & view,
                                          GLuint texture_id);
static void injest_light(lv_gltf_model_t * data, size_t light_index, fastgltf::Light & light, size_t scene_index);
static bool injest_mesh(lv_gltf_model_t * data, fastgltf::Mesh & mesh);

static void make_small_magenta_texture(uint32_t new_magenta_tex);

template <typename T, typename Func>
static size_t injest_vec_attribute(uint8_t vec_size, int32_t current_attrib_index, lv_gltf_model_t * data,
                                   const fastgltf::Primitive * prim, const char * attrib_id, GLuint primitive_vertex_buffer,
                                   size_t offset, Func &&functor);

static int32_t injest_get_any_image_index(fastgltf::Optional<fastgltf::Texture> tex);
static bool injest_check_any_image_index_valid(fastgltf::Optional<fastgltf::Texture> tex);

static inline GLsizei get_level_count(int32_t width, int32_t height)
{
    return static_cast<GLsizei>(1 + floor(log2(width > height ? width : height)));
}

static void load_mesh_texture_impl(lv_gltf_model_t * data, const fastgltf::TextureInfo & material_prop,
                                   GLuint * primitive_tex_prop,
                                   GLint * primitive_tex_uv_id);
static void load_mesh_texture(lv_gltf_model_t * data, const fastgltf::Optional<fastgltf::TextureInfo> & material_prop,
                              GLuint * primitive_tex_prop, GLint * primitive_tex_uv_id);

static void load_mesh_texture(lv_gltf_model_t * data,
                              const fastgltf::Optional<fastgltf::NormalTextureInfo> & material_prop,
                              GLuint * primitive_tex_prop, GLint * primitive_tex_uv_id);

static void load_mesh_texture(lv_gltf_model_t * data,
                              const fastgltf::Optional<fastgltf::OcclusionTextureInfo> & material_prop,
                              GLuint * primitive_tex_prop, GLint * primitive_tex_uv_id);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_gltf_model_t * lv_gltf_data_load_internal(const void * data_source, size_t data_size,
                                             lv_opengl_shader_manager_t * shaders)
{
    lv_gltf_model_t * data = NULL;
    if(data_size > 0) {
        data = create_data_from_bytes((const uint8_t *)data_source, data_size);
    }
    else {
        data = create_data_from_file((const char *)data_source);
    }

    LV_ASSERT_MSG(data, "Failed to create gltf data");
    if(!data) {
        return NULL;
    }

    // Parse the visible node structure to get a world transform matrix for each mesh component
    // instance per node, and apply that matrix to the min/max of the untransformed mesh, then
    // grow a bounding volume to include those transformed points

    int32_t scene_index = 0;
    bool first_visible_mesh = true;
    fastgltf::iterateSceneNodes(
    data->asset, scene_index, fastgltf::math::fmat4x4(), [&](fastgltf::Node & node, fastgltf::math::fmat4x4 matrix) {
        if(!node.meshIndex.has_value()) {
            return;
        }
        if(first_visible_mesh) {
            injest_set_initial_bounds(data, matrix, data->asset.meshes[node.meshIndex.value()]);
        }
        else {
            injest_grow_bounds_to_include(data, matrix, data->asset.meshes[node.meshIndex.value()]);
        }
        first_visible_mesh = false;
    });
    lv_gltf_data_nodes_init(data, data->asset.nodes.size());

    fastgltf::namegen_iterate_scene_nodes(data->asset, scene_index,
                                          [&](fastgltf::Node & node, const std::string & node_path, const std::string & node_ip,
    size_t node_index, std::size_t child_index) {
        LV_UNUSED(node_index);
        LV_UNUSED(child_index);
        lv_gltf_data_node_t data_node;
        lv_gltf_data_node_init(&data_node, &node, node_path.c_str(), node_ip.c_str());
        lv_gltf_data_node_add(data, &data_node);
    });

    {
        uint32_t i = 0;
        for(auto & image : data->asset.images) {
            injest_image(shaders, data, image, i);
            i++;
        }
    }
    uint16_t lightnum = 0;
    for(auto & light : data->asset.lights) {
        injest_light(data, lightnum, light, 0);
        lightnum += 1;
    }
    for(auto & mesh : data->asset.meshes) {
        injest_mesh(data, mesh);
    }

    if(data->asset.defaultScene.has_value()) {
        LV_LOG_INFO("Default scene = #%d", data->asset.defaultScene.value());
    }

    return data;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static lv_gltf_model_t * create_data_from_file(const char * path)
{
#if !FASTGLTF_HAS_MEMORY_MAPPED_FILE
#error This version of fastgltf can not open GLTF files from filesystem. Either encode your GLB into a source file and create or build fastgltf with FASTGLTF_HAS_MEMORY_MAPPED_FILE set to '1'
#endif

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to open file '%s': %d", path, res);
        return NULL;
    }
    res = lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to seek end of file '%s': %d", path, res);
        return NULL;
    }
    uint32_t file_size;
    res = lv_fs_tell(&file, &file_size);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to get file count size '%s': %d", path, res);
        return NULL;
    }

    res = lv_fs_seek(&file, 0, LV_FS_SEEK_SET);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to seek start of file '%s': %d", path, res);
        return NULL;
    }

    uint8_t * bytes = (uint8_t *) lv_malloc(file_size);

    uint32_t bytes_read;
    res = lv_fs_read(&file, bytes, file_size, &bytes_read);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to seek start of file '%s': %d", path, res);
        return NULL;
    }
    if(bytes_read != file_size) {
        LV_LOG_ERROR("Failed to read the entire gltf file '%s': %d", path, res);
        return NULL;
    }
    lv_fs_close(&file);

    lv_gltf_model_t * model = create_data_from_bytes(bytes, file_size);
    lv_free(bytes);

    model->filename = path;
    return model;
}

static lv_gltf_model_t * create_data_from_bytes(const uint8_t * bytes, size_t data_size)
{
    fastgltf::Parser parser(SUPPORTED_EXTENSIONS);
    auto gltf_buffer = fastgltf::GltfDataBuffer::FromBytes(reinterpret_cast<const std::byte *>(bytes), data_size);
    if(!gltf_buffer) {
        LV_LOG_ERROR("Failed to create glTF buffer from bytes: %s",
                     std::string(fastgltf::getErrorMessage(gltf_buffer.error())).c_str());
        return NULL;
    }
    auto asset = parser.loadGltf(gltf_buffer.get(), ".", GLTF_OPTIONS);
    if(!asset) {
        LV_LOG_ERROR("Failed to decode glTF bytes: %s", std::string(fastgltf::getErrorMessage(asset.error())).c_str());
        return NULL;
    }
    return lv_gltf_data_create_internal("from_bytes", std::move(asset.get()));
}

static void make_small_magenta_texture(uint32_t new_magenta_tex)
{
    GL_CALL(glBindTexture(GL_TEXTURE_2D, new_magenta_tex));
    unsigned char clearBytes[4] = { 255, 0, 255, 255 }; // RGBA format
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, clearBytes));
    // Set texture parameters (optional but recommended)
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_2D));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    return;
}

static void load_mesh_texture_impl(lv_gltf_model_t * data, const fastgltf::TextureInfo & material_prop,
                                   GLuint * primitive_tex_prop,
                                   GLint * primitive_tex_uv_id)
{
    const auto & texture = data->asset.textures[material_prop.textureIndex];
    if(!injest_check_any_image_index_valid(texture)) {
        return;
    }
    *primitive_tex_prop = data->textures[injest_get_any_image_index(texture)];
    if(material_prop.transform && material_prop.transform->texCoordIndex.has_value()) {
        *primitive_tex_uv_id = material_prop.transform->texCoordIndex.value();
    }
    else {
        *primitive_tex_uv_id = material_prop.texCoordIndex;
    }
    LV_LOG_TRACE("Prim tex prop: %d Prim tex uv id %d", *primitive_tex_prop, *primitive_tex_uv_id);
}

static void load_mesh_texture(lv_gltf_model_t * data,
                              const fastgltf::Optional<fastgltf::NormalTextureInfo> & material_prop,
                              GLuint * primitive_tex_prop, GLint * primitive_tex_uv_id)
{
    if(!material_prop) {
        return;
    }
    load_mesh_texture_impl(data, material_prop.value(), primitive_tex_prop, primitive_tex_uv_id);
}

static void load_mesh_texture(lv_gltf_model_t * data, const fastgltf::Optional<fastgltf::TextureInfo> & material_prop,
                              GLuint * primitive_tex_prop, GLint * primitive_tex_uv_id)
{
    if(!material_prop) {
        return;
    }
    load_mesh_texture_impl(data, material_prop.value(), primitive_tex_prop, primitive_tex_uv_id);
}

static void load_mesh_texture(lv_gltf_model_t * data,
                              const fastgltf::Optional<fastgltf::OcclusionTextureInfo> & material_prop,
                              GLuint * primitive_tex_prop, GLint * primitive_tex_uv_id)
{
    if(!material_prop) {
        return;
    }
    load_mesh_texture_impl(data, material_prop.value(), primitive_tex_prop, primitive_tex_uv_id);
}

static int32_t injest_get_any_image_index(fastgltf::Optional<fastgltf::Texture> tex)
{
    if(tex->imageIndex.has_value()) {
        return tex->imageIndex.value();
    }

    if(tex->webpImageIndex.has_value()) {
        return tex->webpImageIndex.value();
    }
    return 0;
}
static bool injest_check_any_image_index_valid(fastgltf::Optional<fastgltf::Texture> tex)
{
    if(tex->imageIndex.has_value())
        return true;
    if(tex->webpImageIndex.has_value())
        return true;
    return false;
}

static void injest_grow_bounds_to_include(lv_gltf_model_t * data, const fastgltf::math::fmat4x4 & matrix,
                                          const fastgltf::Mesh & mesh)
{
    /* Grow the bounds to include the specified mesh. */
    fastgltf::math::fvec3 v_min{ data->vertex_min[0], data->vertex_min[1], data->vertex_min[2] };

    fastgltf::math::fvec3 v_max{
        data->vertex_max[0],
        data->vertex_max[1],
        data->vertex_max[2],
    };
    fastgltf::math::fvec3 v_cen{ data->vertex_cen[0], data->vertex_cen[1], data->vertex_cen[2] };

    float new_bound_radius = data->bound_radius;
    if(mesh.primitives.size() > 0) {
        set_bounds_info(data, v_min, v_max, v_cen, new_bound_radius);
        return;
    }
    size_t accessor_index = mesh.primitives[0].findAttribute("POSITION")->accessorIndex;
    const auto & accessor = data->asset.accessors[accessor_index];

    if(!accessor.bufferViewIndex.has_value() || !(accessor.min.has_value() && accessor.max.has_value())) {
        set_bounds_info(data, v_min, v_max, v_cen, new_bound_radius);
        return;
    }

    fastgltf::math::fvec4 t_min{ (float)(accessor.min.value().get<double>(0)), (float)(accessor.min.value().get<double>(1)),
                                 (float)(accessor.min.value().get<double>(2)), 1.f };
    fastgltf::math::fvec4 t_max{ (float)(accessor.max.value().get<double>(0)), (float)(accessor.max.value().get<double>(1)),
                                 (float)(accessor.max.value().get<double>(2)), 1.f };

    t_min = matrix * t_min;
    t_max = matrix * t_max;
    v_max[0] = LV_MAX(LV_MAX(v_max[0], t_min.x()), t_max.x());
    v_max[1] = LV_MAX(LV_MAX(v_max[1], t_min.y()), t_max.y());
    v_max[2] = LV_MAX(LV_MAX(v_max[2], t_min.z()), t_max.z());
    v_min[0] = LV_MIN(LV_MIN(v_min[0], t_min.x()), t_max.x());
    v_min[1] = LV_MIN(LV_MIN(v_min[1], t_min.y()), t_max.y());
    v_min[2] = LV_MIN(LV_MIN(v_min[2], t_min.z()), t_max.z());
    v_cen[0] = (v_max[0] + v_min[0]) / 2.0f;
    v_cen[1] = (v_max[1] + v_min[1]) / 2.0f;
    v_cen[2] = (v_max[2] + v_min[2]) / 2.0f;

    float size_x = v_max[0] - v_min[0];
    float size_y = v_max[1] - v_min[1];
    float size_z = v_max[2] - v_min[2];
    new_bound_radius = std::sqrt((size_x * size_x) + (size_y * size_y) + (size_z * size_z)) / 2.0f;

    set_bounds_info(data, v_min, v_max, v_cen, new_bound_radius);
}
static void injest_set_initial_bounds(lv_gltf_model_t * data, const fastgltf::math::fmat4x4 & matrix,
                                      const fastgltf::Mesh & mesh)
{
    fastgltf::math::fvec3 v_min, v_max, v_cen;
    float radius = 0.f;
    if(mesh.primitives.size() == 0) {
        set_bounds_info(data, v_min, v_max, v_cen, radius);
        return;
    }

    size_t accessor_index = mesh.primitives[0].findAttribute("POSITION")->accessorIndex;
    const auto & accessor = data->asset.accessors[accessor_index];

    if(!accessor.bufferViewIndex.has_value() || !(accessor.min.has_value() && accessor.max.has_value())) {
        set_bounds_info(data, v_min, v_max, v_cen, radius);
        return;
    }

    fastgltf::math::fvec4 t_min{ (float)(accessor.min.value().get<double>(0)), (float)(accessor.min.value().get<double>(1)),
                                 (float)(accessor.min.value().get<double>(2)), 1.f };

    fastgltf::math::fvec4 t_max{ (float)(accessor.max.value().get<double>(0)), (float)(accessor.max.value().get<double>(1)),
                                 (float)(accessor.max.value().get<double>(2)), 1.f };

    t_min = matrix * t_min;
    t_max = matrix * t_max;

    v_max[0] = LV_MAX(t_min.x(), t_max.x());
    v_max[1] = LV_MAX(t_min.y(), t_max.y());
    v_max[2] = LV_MAX(t_min.z(), t_max.z());
    v_min[0] = LV_MIN(t_min.x(), t_max.x());
    v_min[1] = LV_MIN(t_min.y(), t_max.y());
    v_min[2] = LV_MIN(t_min.z(), t_max.z());
    v_cen[0] = (v_max[0] + v_min[0]) / 2.0f;
    v_cen[1] = (v_max[1] + v_min[1]) / 2.0f;
    v_cen[2] = (v_max[2] + v_min[2]) / 2.0f;
    const float size_x = v_max[0] - v_min[0];
    const float size_y = v_max[1] - v_min[1];
    const float size_z = v_max[2] - v_min[2];
    radius = std::sqrt((size_x * size_x) + (size_y * size_y) + (size_z * size_z)) / 2.0f;

    set_bounds_info(data, v_min, v_max, v_cen, radius);
}

bool injest_image(lv_opengl_shader_manager_t * shader_manager, lv_gltf_model_t * data, fastgltf::Image & image,
                  uint32_t index)
{
    std::string _tex_id = std::string(lv_gltf_get_filename(data)) + "_IMG" + std::to_string(index);

    char tmp[512];
    lv_snprintf(tmp, sizeof(tmp), "%s_img_%u", data->filename, index);
    const uint32_t hash = lv_opengl_shader_hash(tmp);
    GLuint texture_id = lv_opengl_shader_manager_get_texture(shader_manager, hash);

    if(texture_id != GL_NONE) {
        LV_LOG_TRACE("Emplacing back already cached texture from previous injest iteration %u", texture_id);
        data->textures.emplace_back(texture_id);
        return true;
    }

    LV_LOG_TRACE("Image (%s) [%d] [%u]", image.name.c_str(), texture_id, hash);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);

    GL_CALL(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    bool image_invalidated = false;
    std::visit(fastgltf::visitor{
        [](auto & arg)
        {
            LV_UNUSED(arg);
            LV_LOG_ERROR("Unexpected image source");
        },
        [&](fastgltf::sources::URI & file_path)
        {
            LV_ASSERT_MSG(file_path.fileByteOffset == 0, "Offsets aren't supported with stbi");
            LV_ASSERT_MSG(file_path.uri.isLocalPath(), "We're only capable of loading local files.");

            int32_t width, height, nrChannels;
            LV_LOG_TRACE("Loading image: %s", image.name.c_str());
            const std::string path(file_path.uri.path().begin(), file_path.uri.path().end());
            unsigned char * data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);
            glTexStorage2D(GL_TEXTURE_2D, get_level_count(width, height), GL_RGBA8, width, height);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        },
        [&](fastgltf::sources::Array & vector)
        {
            int32_t width, height, nrChannels;
            LV_LOG_TRACE("Unpacking image data: %s", image.name.c_str());

            unsigned char * data = stbi_load_from_memory(
                reinterpret_cast<const stbi_uc *>(vector.bytes.data()),
                static_cast<int32_t>(vector.bytes.size()), &width, &height, &nrChannels, 4);
            glTexStorage2D(GL_TEXTURE_2D, get_level_count(width, height), GL_RGBA8, width, height);
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        },
        [&](fastgltf::sources::BufferView & view)
        {
            LV_LOG_TRACE("Injesting image from bufferview: %s", image.name.c_str());
            image_invalidated |= injest_image_from_buffer_view(data, view, texture_id);
        },
    }, image.data);

    if(!image_invalidated) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        LV_LOG_ERROR("Failed to load image %s", image.name.c_str());
    }
    LV_LOG_TRACE("Storing texture with hash: %u %u", hash, texture_id);
    lv_opengl_shader_manager_store_texture(shader_manager, hash, texture_id);
    GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
    data->textures.emplace_back(texture_id);
    return true;
}

static bool injest_image_from_buffer_view(lv_gltf_model_t * data, fastgltf::sources::BufferView & view,
                                          GLuint texture_id)
{
    /* Yes, we've already loaded every buffer into some GL buffer. However, with GL it's simpler
       to just copy the buffer data again for the texture. Besides, this is just an example. */
    auto & buffer_view = data->asset.bufferViews[view.bufferViewIndex];
    auto & buffer = data->asset.buffers[buffer_view.bufferIndex];
    LV_LOG_INFO("Unpacking image bufferView: %s from %d bytes", image.name, bufferView.byteLenght);
    return std::visit(
    fastgltf::visitor{
        // We only care about VectorWithMime here, because we specify LoadExternalBuffers, meaning
        // all buffers are already loaded into a vector.
        [](auto & arg)
        {
            LV_UNUSED(arg);
            LV_LOG_ERROR("Unexpected image source");
            return false;
        },
        [&](fastgltf::sources::Array & vector)
        {
            LV_LOG_TRACE("[WEBP] width: %d height: %d", width, height);
            int32_t width, height, nrChannels;
            int32_t webpRes = WebPGetInfo(
                reinterpret_cast<const uint8_t *>(vector.bytes.data() + buffer_view.byteOffset),
                static_cast<std::size_t>(buffer_view.byteLength), &width, &height);

            if(!webpRes) {
                unsigned char * data = stbi_load_from_memory(
                    reinterpret_cast<const stbi_uc *>(vector.bytes.data() + buffer_view.byteOffset),
                    static_cast<int32_t>(buffer_view.byteLength), &width, &height, &nrChannels, 4);
                if((width <= 0) || (height <= 0)) {
                    LV_LOG_ERROR("Failed to load image from memory");
                    make_small_magenta_texture(texture_id);
                    return true;
                }
                LV_LOG_TRACE("[WEBP] width: %d height: %d", width, height);
                glTexStorage2D(GL_TEXTURE_2D, get_level_count(width, height), GL_RGBA8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                stbi_image_free(data);
                return false;
            }
            WebPBitstreamFeatures features = WebPBitstreamFeatures();
            auto status_code = WebPGetFeatures(
                                   reinterpret_cast<const uint8_t *>(vector.bytes.data() + buffer_view.byteOffset),
                                   static_cast<std::size_t>(buffer_view.byteLength), &features);
            if(status_code != VP8_STATUS_OK) {
                LV_LOG_ERROR("Failed to load webp image %d", status_code);
                make_small_magenta_texture(texture_id);
                return true;
            }
            if(features.has_alpha) {
                uint8_t * unpacked = WebPDecodeRGBA(
                                         reinterpret_cast<const uint8_t *>(vector.bytes.data() + buffer_view.byteOffset),
                                         static_cast<std::size_t>(buffer_view.byteLength), &width, &height);
                glTexStorage2D(GL_TEXTURE_2D, get_level_count(width, height), GL_RGBA8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE,
                                unpacked);
                WebPFree(unpacked);
            }
            else {
                uint8_t * unpacked = WebPDecodeRGB(
                                         reinterpret_cast<const uint8_t *>(vector.bytes.data() + buffer_view.byteOffset),
                                         static_cast<std::size_t>(buffer_view.byteLength), &width, &height);
                glTexStorage2D(GL_TEXTURE_2D, get_level_count(width, height), GL_RGB8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE,
                                unpacked);
                WebPFree(unpacked);
            }
            return false;
        } },
    buffer.data);
}
static void injest_light(lv_gltf_model_t * data, size_t light_index, fastgltf::Light & light, size_t scene_index)
{
    fastgltf::math::fmat4x4 tmat;
    // It would seem like we'd need this info but not really, just the index will do at the loading phase, the rest is pulled during frame updates.
    LV_UNUSED(light);

    fastgltf::findlight_iterate_scene_nodes(data->asset, scene_index, &tmat,
                                            [&](fastgltf::Node & node, const fastgltf::math::fmat4x4 & parentworldmatrix,
    const fastgltf::math::fmat4x4 & localmatrix) {
        LV_UNUSED(parentworldmatrix);
        LV_UNUSED(localmatrix);
        if(!node.lightIndex.has_value() ||
           node.lightIndex.value() != light_index) {
            return;
        }
        LV_LOG_INFO("SCENE LIGHT BEING ADDED #%d\n", light_index);
        data->node_by_light_index.push_back(&node);
    });
}

static bool injest_mesh(lv_gltf_model_t * data, fastgltf::Mesh & mesh)
{
    /*const auto &asset = GET_ASSET(data);*/
    const auto & outMesh = lv_gltf_get_new_meshdata(data);
    outMesh->primitives.resize(mesh.primitives.size());

    for(auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it) {
        if(it->dracoCompression) {
            LV_LOG_WARN("Unhandled draco compression");
        }
        auto * positionIt = it->findAttribute("POSITION");
        // A mesh primitive is required to hold the POSITION attribute.
        //
        assert(positionIt != it->attributes.end());
        assert(it->indicesAccessor.has_value()); // We specify GenerateMeshIndices, so we should always have indices

        auto index = std::distance(mesh.primitives.begin(), it);
        auto & primitive = outMesh->primitives[index];

        // Generate the VAO
        GLuint vao;
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        primitive.primitiveType = fastgltf::to_underlying(it->type);
        primitive.vertexArray = vao;

        if(it->materialIndex.has_value()) {
            // Adjust for default material
            primitive.materialUniformsIndex = it->materialIndex.value() + 1;
            auto & material = data->asset.materials[it->materialIndex.value()];
            load_mesh_texture(data, material.pbrData.baseColorTexture, &primitive.albedoTexture,
                              &primitive.baseColorTexcoordIndex);

            load_mesh_texture(data, material.pbrData.metallicRoughnessTexture, &primitive.metalRoughTexture,
                              &primitive.metallicRoughnessTexcoordIndex);
            load_mesh_texture(data, material.normalTexture, &primitive.normalTexture, &primitive.normalTexcoordIndex);
            load_mesh_texture(data, material.occlusionTexture, &primitive.occlusionTexture,
                              &primitive.occlusionTexcoordIndex);
            load_mesh_texture(data, material.emissiveTexture, &primitive.emissiveTexture,
                              &primitive.emissiveTexcoordIndex);
            if(material.volume)
                load_mesh_texture(data, material.volume->thicknessTexture, &primitive.thicknessTexture,
                                  &primitive.thicknessTexcoordIndex);
            if(material.transmission)
                load_mesh_texture(data, material.transmission->transmissionTexture,
                                  &primitive.transmissionTexture, (GLint *)&primitive.transmissionTexcoordIndex);
            if(material.clearcoat) {
                load_mesh_texture(data, material.clearcoat->clearcoatTexture,
                                  (GLuint *)&primitive.clearcoatTexture, &primitive.clearcoatTexcoordIndex);
                load_mesh_texture(data, material.clearcoat->clearcoatRoughnessTexture,
                                  (GLuint *)&primitive.clearcoatRoughnessTexture,
                                  &primitive.clearcoatRoughnessTexcoordIndex);
                load_mesh_texture(data, material.clearcoat->clearcoatNormalTexture,
                                  (GLuint *)&primitive.clearcoatNormalTexture,
                                  &primitive.clearcoatNormalTexcoordIndex);
            }
            if(material.diffuseTransmission) {
                load_mesh_texture(data, material.diffuseTransmission->diffuseTransmissionTexture,
                                  &primitive.diffuseTransmissionTexture,
                                  &primitive.diffuseTransmissionTexcoordIndex);
                load_mesh_texture(data, material.diffuseTransmission->diffuseTransmissionColorTexture,
                                  &primitive.diffuseTransmissionColorTexture,
                                  &primitive.diffuseTransmissionColorTexcoordIndex);
            }
        }
        else {
            primitive.materialUniformsIndex = 0;
        }

        auto & positionAccessor = data->asset.accessors[positionIt->accessorIndex];
        if(!positionAccessor.bufferViewIndex.has_value()) {
            continue;
        }

        // Create the vertex buffer for this primitive, and use the accessor tools to copy directly into the mapped buffer.
        GL_CALL(glGenBuffers(1, &primitive.vertexBuffer));
        GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, primitive.vertexBuffer));

        std::vector<vertex_t> vertices_vec(positionAccessor.count);
        vertex_t * vertices = vertices_vec.data();
        glBufferData(GL_ARRAY_BUFFER, positionAccessor.count * sizeof(*vertices), nullptr, GL_STATIC_DRAW);
        {
            int32_t attr_index = 0;
            attr_index = injest_vec_attribute<fastgltf::math::fvec3>(
                             3, attr_index, data, &(*it), "POSITION", primitive.vertexBuffer, offsetof(vertex_t, position),
            [&](fastgltf::math::fvec3 vec, size_t idx) {
                vertices[idx].position = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec4>(
                             4, attr_index, data, &(*it), "JOINTS_0", primitive.vertexBuffer, offsetof(vertex_t, joints),
            [&](fastgltf::math::fvec4 vec, size_t idx) {
                vertices[idx].joints = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec4>(
                             4, attr_index, data, &(*it), "JOINTS_1", primitive.vertexBuffer, offsetof(vertex_t, joints2),
            [&](fastgltf::math::fvec4 vec, std::size_t idx) {
                vertices[idx].joints2 = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec4>(
                             4, attr_index, data, &(*it), "WEIGHTS_0", primitive.vertexBuffer, offsetof(vertex_t, weights),
            [&](fastgltf::math::fvec4 vec, std::size_t idx) {
                vertices[idx].weights = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec4>(
                             4, attr_index, data, &(*it), "WEIGHTS_1", primitive.vertexBuffer, offsetof(vertex_t, weights2),
            [&](fastgltf::math::fvec4 vec, size_t idx) {
                vertices[idx].weights2 = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec3>(
                             3, attr_index, data, &(*it), "NORMAL", primitive.vertexBuffer, offsetof(vertex_t, normal),
            [&](fastgltf::math::fvec3 vec, std::size_t idx) {
                vertices[idx].normal = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec4>(
                             4, attr_index, data, &(*it), "TANGENT", primitive.vertexBuffer, offsetof(vertex_t, tangent),
            [&](fastgltf::math::fvec4 vec, size_t idx) {
                vertices[idx].tangent = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec2>(
                             2, attr_index, data, &(*it), "TEXCOORD_0", primitive.vertexBuffer, offsetof(vertex_t, uv),
            [&](fastgltf::math::fvec2 vec, size_t idx) {
                vertices[idx].uv = vec;
            });
            attr_index = injest_vec_attribute<fastgltf::math::fvec2>(
                             2, attr_index, data, &(*it), "TEXCOORD_1", primitive.vertexBuffer, offsetof(vertex_t, uv2),
            [&](fastgltf::math::fvec2 vec, size_t idx) {
                vertices[idx].uv2 = vec;
            });
        }
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, primitive.vertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, positionAccessor.count * sizeof(vertex_t), vertices_vec.data(), GL_STATIC_DRAW);

        // Generate the indirect draw command
        auto & draw = primitive.draw;
        draw.instanceCount = 1;
        draw.baseInstance = 0;
        draw.baseVertex = 0;
        draw.firstIndex = 0;

        auto & indexAccessor = data->asset.accessors[it->indicesAccessor.value()];
        if(!indexAccessor.bufferViewIndex.has_value())
            return false;
        draw.count = static_cast<std::uint32_t>(indexAccessor.count);

        // Create the index buffer and copy the indices into it.
        glGenBuffers(1, &primitive.indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.indexBuffer);
        if(indexAccessor.componentType == fastgltf::ComponentType::UnsignedByte ||
           indexAccessor.componentType == fastgltf::ComponentType::UnsignedShort) {
            primitive.indexType = GL_UNSIGNED_SHORT;
            std::uint16_t * tempIndices = new std::uint16_t[indexAccessor.count];
            fastgltf::copyFromAccessor<std::uint16_t>(data->asset, indexAccessor, tempIndices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(indexAccessor.count * sizeof(std::uint16_t)), tempIndices,
                         GL_STATIC_DRAW);
            delete[] tempIndices;
        }
        else {
            primitive.indexType = GL_UNSIGNED_INT;
            //std::uint32_t tempIndices[indexAccessor.count];
            std::uint32_t * tempIndices = new std::uint32_t[indexAccessor.count];
            fastgltf::copyFromAccessor<std::uint32_t>(data->asset, indexAccessor, tempIndices);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         static_cast<GLsizeiptr>(indexAccessor.count * sizeof(std::uint32_t)), tempIndices,
                         GL_STATIC_DRAW);
            delete[] tempIndices;
        }
    }

    glBindVertexArray(0);
    return true;
}
template <typename T, typename Func>
static size_t injest_vec_attribute(uint8_t vec_size, int32_t current_attrib_index, lv_gltf_model_t * data,
                                   const fastgltf::Primitive * prim, const char * attrib_id, GLuint primitive_vertex_buffer,
                                   size_t offset, Func &&functor

                                  )
{
    const auto & asset = data->asset;
    if(const auto * _attrib = prim->findAttribute(std::string(attrib_id)); _attrib != prim->attributes.end()) {
        auto & accessor = asset.accessors[_attrib->accessorIndex];
        if(accessor.bufferViewIndex.has_value()) {
            glBindBuffer(GL_ARRAY_BUFFER, primitive_vertex_buffer);
            fastgltf::iterateAccessorWithIndex<T>(asset, accessor, functor);
            // Specify the layout of the vertex data
            glVertexAttribPointer(current_attrib_index, // Attribute index
                                  vec_size, // Number of components per vertex
                                  GL_FLOAT, // Data type
                                  GL_FALSE, // Normalized
                                  sizeof(vertex_t), // Stride (size of one vertex)
                                  (void *)offset); // Offset in the buffer
            glEnableVertexAttribArray(current_attrib_index);
        }
        else {
            glDisableVertexAttribArray(current_attrib_index);
        }
        current_attrib_index++;
    }
    return current_attrib_index;
}

static void set_bounds_info(lv_gltf_model_t * data, fastgltf::math::fvec3 v_min, fastgltf::math::fvec3 v_max,
                            fastgltf::math::fvec3 v_cen, float radius)
{
    {
        auto _d = v_min.data();
        data->vertex_min[0] = _d[0];
        data->vertex_min[1] = _d[1];
        data->vertex_min[2] = _d[2];
    }
    {
        auto _d = v_max.data();
        data->vertex_max[0] = _d[0];
        data->vertex_max[1] = _d[1];
        data->vertex_max[2] = _d[2];
    }
    {
        auto _d = v_cen.data();
        data->vertex_cen[0] = _d[0];
        data->vertex_cen[1] = _d[1];
        data->vertex_cen[2] = _d[2];
    }
    data->bound_radius = radius;
}

#endif /*LV_USE_GLTF*/
