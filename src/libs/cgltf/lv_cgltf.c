/**
 * @file lv_cgltf.c
 * LVGL wrapper for cgltf - using LVGL file system and memory allocation
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_cgltf.h"

#if LV_USE_CGLTF

#include "../../stdlib/lv_mem.h"
#include "../../stdlib/lv_string.h"
#include "../../misc/lv_fs.h"
#include "../../misc/lv_log.h"

/* Define cgltf implementation with LVGL memory functions */
#define CGLTF_MALLOC(size) lv_malloc(size)
#define CGLTF_FREE(ptr) lv_free(ptr)
#define CGLTF_IMPLEMENTATION
#include "cgltf.h"

/*********************
 *      DEFINES
 *********************/

#define LV_CGLTF_LOG_LEVEL LV_LOG_LEVEL_INFO

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * cgltf_alloc_cb(void * user, cgltf_size size);
static void cgltf_free_cb(void * user, void * ptr);
static cgltf_result cgltf_read_cb(const cgltf_memory_options * memory_options,
                                  const cgltf_file_options * file_options,
                                  const char * path, cgltf_size * size, void ** data);
static void cgltf_release_cb(const cgltf_memory_options * memory_options,
                             const cgltf_file_options * file_options,
                             void * data, cgltf_size size);
static char * get_directory_path(const char * path);
static char * combine_path(const char * dir, const char * file);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_cgltf_init_options(cgltf_options * options)
{
    if(options == NULL) return;

    lv_memzero(options, sizeof(cgltf_options));

    /* Set up LVGL memory callbacks */
    options->memory.alloc_func = cgltf_alloc_cb;
    options->memory.free_func = cgltf_free_cb;
    options->memory.user_data = NULL;

    /* Set up LVGL file callbacks */
    options->file.read = cgltf_read_cb;
    options->file.release = cgltf_release_cb;
    options->file.user_data = NULL;
}

cgltf_data * lv_cgltf_parse_file(const char * path)
{
    if(path == NULL) {
        LV_LOG_ERROR("NULL path");
        return NULL;
    }

    cgltf_options options;
    lv_cgltf_init_options(&options);

    cgltf_data * data = NULL;

    /* Read file using LVGL file system */
    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_ERROR("Failed to open file: %s", path);
        return NULL;
    }

    /* Get file size */
    uint32_t file_size = 0;
    res = lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    if(res == LV_FS_RES_OK) {
        lv_fs_tell(&file, &file_size);
        lv_fs_seek(&file, 0, LV_FS_SEEK_SET);
    }

    if(file_size == 0) {
        LV_LOG_ERROR("Empty file or seek failed: %s", path);
        lv_fs_close(&file);
        return NULL;
    }

    /* Allocate buffer and read file */
    void * buffer = lv_malloc(file_size);
    if(buffer == NULL) {
        LV_LOG_ERROR("Failed to allocate %u bytes", file_size);
        lv_fs_close(&file);
        return NULL;
    }

    uint32_t bytes_read = 0;
    res = lv_fs_read(&file, buffer, file_size, &bytes_read);
    lv_fs_close(&file);

    if(res != LV_FS_RES_OK || bytes_read != file_size) {
        LV_LOG_ERROR("Failed to read file: %s", path);
        lv_free(buffer);
        return NULL;
    }

    /* Parse the data
     * NOTE: We must NOT free the buffer here because cgltf keeps references
     * to it for binary data (GLB). The buffer will be freed when cgltf_free()
     * is called, as cgltf tracks it internally via data->bin pointer.
     */
    cgltf_result result = cgltf_parse(&options, buffer, file_size, &data);

    if(result != cgltf_result_success) {
        LV_LOG_ERROR("Failed to parse glTF: %d", result);
        lv_free(buffer);
        return NULL;
    }

    /* For GLB files, cgltf references the buffer directly.
     * For GLTF files, we need to keep buffer alive until load_buffers is called.
     * Store the buffer pointer so it can be freed later.
     * Note: cgltf_free will free all memory allocated through the memory callbacks,
     * but the original parse buffer needs special handling.
     */
    data->file_data = buffer;

    LV_LOG_INFO("Parsed glTF: %s (meshes: %zu, nodes: %zu)",
                path, data->meshes_count, data->nodes_count);

    return data;
}

cgltf_data * lv_cgltf_parse_memory(const void * data, size_t size)
{
    if(data == NULL || size == 0) {
        LV_LOG_ERROR("Invalid data or size");
        return NULL;
    }

    cgltf_options options;
    lv_cgltf_init_options(&options);

    cgltf_data * gltf = NULL;
    cgltf_result result = cgltf_parse(&options, data, size, &gltf);

    if(result != cgltf_result_success) {
        LV_LOG_ERROR("Failed to parse glTF from memory: %d", result);
        return NULL;
    }

    return gltf;
}

lv_result_t lv_cgltf_load_buffers(cgltf_data * gltf, const char * gltf_path)
{
    if(gltf == NULL) return LV_RESULT_INVALID;

    cgltf_options options;
    lv_cgltf_init_options(&options);

    cgltf_result result = cgltf_load_buffers(&options, gltf, gltf_path);

    if(result != cgltf_result_success) {
        LV_LOG_ERROR("Failed to load buffers: %d", result);
        return LV_RESULT_INVALID;
    }

    return LV_RESULT_OK;
}

void lv_cgltf_free(cgltf_data * gltf)
{
    if(gltf == NULL) return;

    /* cgltf_free will call file_release (our cgltf_release_cb) to free file_data,
     * so we don't need to free it manually here.
     */
    cgltf_free(gltf);
}

uint32_t lv_cgltf_get_mesh_count(const cgltf_data * gltf)
{
    if(gltf == NULL) return 0;
    return (uint32_t)gltf->meshes_count;
}

lv_result_t lv_cgltf_get_mesh_data(const cgltf_data * gltf, uint32_t mesh_index,
                                   uint32_t primitive_index, lv_cgltf_mesh_data_t * out_data)
{
    if(gltf == NULL || out_data == NULL) return LV_RESULT_INVALID;
    if(mesh_index >= gltf->meshes_count) return LV_RESULT_INVALID;

    const cgltf_mesh * mesh = &gltf->meshes[mesh_index];
    if(primitive_index >= mesh->primitives_count) return LV_RESULT_INVALID;

    const cgltf_primitive * prim = &mesh->primitives[primitive_index];

    lv_memzero(out_data, sizeof(lv_cgltf_mesh_data_t));

    /* Extract positions, normals, texcoords */
    for(cgltf_size i = 0; i < prim->attributes_count; i++) {
        const cgltf_attribute * attr = &prim->attributes[i];
        const cgltf_accessor * accessor = attr->data;

        cgltf_size float_count = cgltf_accessor_unpack_floats(accessor, NULL, 0);
        if(float_count == 0) continue;

        float * buffer = lv_malloc(float_count * sizeof(float));
        if(buffer == NULL) {
            lv_cgltf_free_mesh_data(out_data);
            return LV_RESULT_INVALID;
        }

        cgltf_accessor_unpack_floats(accessor, buffer, float_count);

        switch(attr->type) {
            case cgltf_attribute_type_position:
                out_data->positions = buffer;
                out_data->vertex_count = (uint32_t)(float_count / 3);
                break;
            case cgltf_attribute_type_normal:
                out_data->normals = buffer;
                break;
            case cgltf_attribute_type_texcoord:
                if(out_data->texcoords == NULL) {  /* Only use first texcoord set */
                    out_data->texcoords = buffer;
                }
                else {
                    lv_free(buffer);
                }
                break;
            default:
                lv_free(buffer);
                break;
        }
    }

    /* Extract indices */
    if(prim->indices != NULL) {
        const cgltf_accessor * indices_accessor = prim->indices;
        out_data->index_count = (uint32_t)indices_accessor->count;

        out_data->indices = lv_malloc(out_data->index_count * sizeof(uint32_t));
        if(out_data->indices == NULL) {
            lv_cgltf_free_mesh_data(out_data);
            return LV_RESULT_INVALID;
        }

        for(cgltf_size i = 0; i < indices_accessor->count; i++) {
            out_data->indices[i] = (uint32_t)cgltf_accessor_read_index(indices_accessor, i);
        }
    }

    return LV_RESULT_OK;
}

void lv_cgltf_free_mesh_data(lv_cgltf_mesh_data_t * data)
{
    if(data == NULL) return;

    if(data->positions) lv_free(data->positions);
    if(data->normals) lv_free(data->normals);
    if(data->texcoords) lv_free(data->texcoords);
    if(data->indices) lv_free(data->indices);

    lv_memzero(data, sizeof(lv_cgltf_mesh_data_t));
}

lv_result_t lv_cgltf_get_material(const cgltf_data * gltf, uint32_t mesh_index,
                                  uint32_t primitive_index, lv_cgltf_material_t * out_material)
{
    if(gltf == NULL || out_material == NULL) return LV_RESULT_INVALID;
    if(mesh_index >= gltf->meshes_count) return LV_RESULT_INVALID;

    const cgltf_mesh * mesh = &gltf->meshes[mesh_index];
    if(primitive_index >= mesh->primitives_count) return LV_RESULT_INVALID;

    const cgltf_primitive * prim = &mesh->primitives[primitive_index];

    /* Default material values */
    out_material->base_color[0] = 1.0f;
    out_material->base_color[1] = 1.0f;
    out_material->base_color[2] = 1.0f;
    out_material->base_color[3] = 1.0f;
    out_material->metallic = 0.0f;
    out_material->roughness = 1.0f;
    out_material->has_texture = false;
    out_material->texture_data = NULL;

    if(prim->material == NULL) return LV_RESULT_OK;

    const cgltf_material * mat = prim->material;

    if(mat->has_pbr_metallic_roughness) {
        const cgltf_pbr_metallic_roughness * pbr = &mat->pbr_metallic_roughness;
        out_material->base_color[0] = pbr->base_color_factor[0];
        out_material->base_color[1] = pbr->base_color_factor[1];
        out_material->base_color[2] = pbr->base_color_factor[2];
        out_material->base_color[3] = pbr->base_color_factor[3];
        out_material->metallic = pbr->metallic_factor;
        out_material->roughness = pbr->roughness_factor;

        /* Check for base color texture */
        if(pbr->base_color_texture.texture != NULL) {
            const cgltf_texture * tex = pbr->base_color_texture.texture;
            if(tex->image != NULL && tex->image->buffer_view != NULL) {
                out_material->has_texture = true;
                out_material->texture_data = (const uint8_t *)tex->image->buffer_view->buffer->data +
                                             tex->image->buffer_view->offset;
            }
        }
    }

    return LV_RESULT_OK;
}

void lv_cgltf_get_node_transform(const cgltf_node * node, float * out_matrix)
{
    if(node == NULL || out_matrix == NULL) return;
    cgltf_node_transform_local(node, out_matrix);
}

void lv_cgltf_get_node_world_transform(const cgltf_node * node, float * out_matrix)
{
    if(node == NULL || out_matrix == NULL) return;
    cgltf_node_transform_world(node, out_matrix);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * cgltf_alloc_cb(void * user, cgltf_size size)
{
    LV_UNUSED(user);
    return lv_malloc(size);
}

static void cgltf_free_cb(void * user, void * ptr)
{
    LV_UNUSED(user);
    lv_free(ptr);
}

static cgltf_result cgltf_read_cb(const cgltf_memory_options * memory_options,
                                  const cgltf_file_options * file_options,
                                  const char * path, cgltf_size * size, void ** data)
{
    LV_UNUSED(memory_options);
    LV_UNUSED(file_options);

    lv_fs_file_t file;
    lv_fs_res_t res = lv_fs_open(&file, path, LV_FS_MODE_RD);
    if(res != LV_FS_RES_OK) {
        LV_LOG_WARN("cgltf_read_cb: Failed to open %s", path);
        return cgltf_result_file_not_found;
    }

    /* Get file size */
    uint32_t file_size = 0;
    res = lv_fs_seek(&file, 0, LV_FS_SEEK_END);
    if(res == LV_FS_RES_OK) {
        lv_fs_tell(&file, &file_size);
        lv_fs_seek(&file, 0, LV_FS_SEEK_SET);
    }

    if(file_size == 0) {
        lv_fs_close(&file);
        return cgltf_result_io_error;
    }

    /* Allocate buffer */
    void * buffer = lv_malloc(file_size);
    if(buffer == NULL) {
        lv_fs_close(&file);
        return cgltf_result_out_of_memory;
    }

    /* Read file */
    uint32_t bytes_read = 0;
    res = lv_fs_read(&file, buffer, file_size, &bytes_read);
    lv_fs_close(&file);

    if(res != LV_FS_RES_OK || bytes_read != file_size) {
        lv_free(buffer);
        return cgltf_result_io_error;
    }

    *size = file_size;
    *data = buffer;
    return cgltf_result_success;
}

static void cgltf_release_cb(const cgltf_memory_options * memory_options,
                             const cgltf_file_options * file_options,
                             void * data, cgltf_size size)
{
    LV_UNUSED(memory_options);
    LV_UNUSED(file_options);
    LV_UNUSED(size);
    lv_free(data);
}

static char * get_directory_path(const char * path)
{
    if(path == NULL) return NULL;

    const char * last_slash = NULL;
    const char * p = path;

    while(*p) {
        if(*p == '/' || *p == '\\') {
            last_slash = p;
        }
        p++;
    }

    if(last_slash == NULL) {
        /* No directory separator, return empty string */
        char * dir = lv_malloc(1);
        if(dir) dir[0] = '\0';
        return dir;
    }

    size_t len = last_slash - path + 1;
    char * dir = lv_malloc(len + 1);
    if(dir) {
        lv_memcpy(dir, path, len);
        dir[len] = '\0';
    }
    return dir;
}

static char * combine_path(const char * dir, const char * file)
{
    if(file == NULL) return NULL;
    if(dir == NULL || dir[0] == '\0') {
        size_t len = lv_strlen(file);
        char * result = lv_malloc(len + 1);
        if(result) {
            lv_memcpy(result, file, len + 1);
        }
        return result;
    }

    size_t dir_len = lv_strlen(dir);
    size_t file_len = lv_strlen(file);
    char * result = lv_malloc(dir_len + file_len + 1);
    if(result) {
        lv_memcpy(result, dir, dir_len);
        lv_memcpy(result + dir_len, file, file_len + 1);
    }
    return result;
}

#endif /* LV_USE_CGLTF */
