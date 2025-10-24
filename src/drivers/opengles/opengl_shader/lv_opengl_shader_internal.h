/**
  @file lv_opengl_shader_internal.h
 *
 */

#ifndef LV_OPENGL_SHADER_INTERNAL_H
#define LV_OPENGL_SHADER_INTERNAL_H

#include "../../../lv_conf_internal.h"

#if LV_USE_OPENGLES
#include "../lv_opengles_private.h"
#include "../lv_opengles_debug.h"
#include "../../../misc/lv_types.h"
#include "../../../misc/lv_rb_private.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef struct {
    const char * name;
    const char * source;
} lv_opengl_shader_t;

typedef struct {
    const lv_opengl_shader_t * all;
    uint32_t count;
} lv_opengl_shader_portions_t;

typedef struct {
    const char * name;
    const char * value;
    bool value_allocated;
} lv_opengl_shader_define_t;

typedef struct {
    lv_opengl_shader_t data;
    bool src_allocated;
} lv_opengl_shader_source_t;

typedef struct {
    uint32_t hash;
    GLuint id;
} lv_opengl_compiled_shader_t;

typedef struct lv_opengl_shader_cache_struct {
    lv_rb_t sources_map;
    lv_rb_t textures_map;
    lv_rb_t compiled_shaders_map;
    lv_rb_t programs_map;
    GLuint bg_index_buf;
    GLuint bg_vertex_buf;
    GLuint bg_program;
    GLuint bg_vao;
} lv_opengl_shader_manager_t;

struct _lv_shader_program;

typedef void (*lv_opengl_shader_program_update_uniformi_t)(struct _lv_shader_program *, const char *, int);

typedef void (*lv_opengl_shader_program_update_uniformf_t)(struct _lv_shader_program *, const char *, float);

typedef struct _lv_shader_program {
    lv_opengl_shader_program_update_uniformi_t update_uniform_1i;
    lv_opengl_shader_program_update_uniformf_t update_uniform_1f;
    uint32_t id;
} lv_opengl_shader_program_t;

typedef enum {
    LV_OPENGL_GLSL_VERSION_300ES,
    LV_OPENGL_GLSL_VERSION_100,
    LV_OPENGL_GLSL_VERSION_LAST,
} lv_opengl_glsl_version;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

char * lv_opengl_shader_manager_process_includes(const char * c_src, const lv_opengl_shader_t * src_includes,
                                                 size_t num_items);

lv_opengl_shader_program_t * lv_opengl_shader_program_create(uint32_t id);
void lv_opengl_shader_program_destroy(lv_opengl_shader_program_t * program);
GLuint lv_opengl_shader_program_get_id(lv_opengl_shader_program_t * program);

void lv_opengl_shader_manager_init(lv_opengl_shader_manager_t * manager, const lv_opengl_shader_t * sources,
                                   size_t len, const char * vert_src,
                                   const char * frag_src);
void lv_opengl_shader_manager_deinit(lv_opengl_shader_manager_t * manager);
uint32_t lv_opengl_shader_hash(const char * value);
GLuint lv_opengl_shader_manager_get_texture(lv_opengl_shader_manager_t * manager, uint32_t hash);
void lv_opengl_shader_manager_store_texture(lv_opengl_shader_manager_t * manager, uint32_t hash, GLuint id);
lv_result_t lv_opengl_shader_manager_select_shader(lv_opengl_shader_manager_t * shader, const char * shader_identifier,
                                                   const lv_opengl_shader_define_t * permutations, size_t permutations_len,
                                                   lv_opengl_glsl_version glsl_version, uint32_t * out_hash);
lv_opengl_shader_program_t * lv_opengl_shader_manager_get_program(lv_opengl_shader_manager_t * manager,
                                                                  uint32_t fragment_shader_hash,
                                                                  uint32_t vertex_shader_hash);

const char * lv_opengles_glsl_version_to_string(lv_opengl_glsl_version version);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_OPENGLES*/
#endif /*LV_OPENGL_SHADER_INTERNAL_H*/
