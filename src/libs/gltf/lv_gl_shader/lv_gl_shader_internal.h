/**
  @file lv_gl_shader_internal.h
 *
 */

#ifndef LV_GL_SHADER_INTERNAL_H
#define LV_GL_SHADER_INTERNAL_H

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF
#include "../../../drivers/glfw/lv_opengles_debug.h"
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
} lv_gl_shader_t;

typedef struct {
    const char * name;
    const char * value;
    bool value_allocated;
} lv_gl_shader_define_t;

typedef struct {
    lv_gl_shader_t data;
    bool src_allocated;
} lv_gl_shader_source_t;

typedef struct {
    uint32_t hash;
    GLuint id;
} lv_gl_compiled_shader_t;

typedef struct lv_opengl_shader_cache_struct {
    lv_rb_t sources_map;
    lv_rb_t textures_map;
    lv_rb_t compiled_shaders_map;
    lv_rb_t programs_map;
    GLuint bg_index_buf;
    GLuint bg_vertex_buf;
    GLuint bg_program;
    GLuint bg_vao;
} lv_gl_shader_manager_t;

struct _lv_shader_program;

typedef void (*lv_gl_shader_program_update_uniformi_t)(struct _lv_shader_program *, const char *, int);

typedef void (*lv_gl_shader_program_update_uniformf_t)(struct _lv_shader_program *, const char *, float);

typedef struct _lv_shader_program {
    lv_gl_shader_program_update_uniformi_t update_uniform_1i;
    lv_gl_shader_program_update_uniformf_t update_uniform_1f;
    uint32_t id;
} lv_gl_shader_program_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_gl_shader_program_t * lv_gl_shader_program_create(uint32_t id);
void lv_gl_shader_program_destroy(lv_gl_shader_program_t * program);
GLuint lv_gl_shader_program_get_id(lv_gl_shader_program_t * program);

lv_gl_shader_manager_t * lv_gl_shader_manager_create(const lv_gl_shader_t * sources,
                                                     size_t len, const char * vert_src,
                                                     const char * frag_src);
void lv_gl_shader_manager_destroy(lv_gl_shader_manager_t * manager);
uint32_t lv_gl_shader_hash(const char * value);
GLuint lv_gl_shader_manager_get_texture(lv_gl_shader_manager_t * manager, uint32_t hash);
void lv_gl_shader_manager_store_texture(lv_gl_shader_manager_t * manager, uint32_t hash, GLuint id);
uint32_t lv_gl_shader_manager_select_shader(lv_gl_shader_manager_t * shader,
                                            const char * shader_identifier,
                                            const lv_gl_shader_define_t * permutations,
                                            size_t permutations_len);
lv_gl_shader_program_t * lv_gl_shader_manager_get_program(lv_gl_shader_manager_t * manager,
                                                          uint32_t fragment_shader_hash,
                                                          uint32_t vertex_shader_hash);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_USE_GLTF*/
#endif /*LV_GL_SHADER_INTERNAL_H*/
