/**
 * @file lv_opengles_driver.h
 *
 */

#ifndef LV_OPENGLES_DRIVER_H
#define LV_OPENGLES_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

void lv_opengles_vertex_buffer_init(const void * data, unsigned int size);

void lv_opengles_vertex_buffer_deinit();

void lv_opengles_vertex_buffer_bind();

void lv_opengles_vertex_buffer_unbind();

void lv_opengles_vertex_array_init();

void lv_opengles_vertex_array_deinit();

void lv_opengles_vertex_array_bind();

void lv_opengles_vertex_array_unbind();

void lv_opengles_vertex_array_add_buffer();

void lv_opengles_index_buffer_init(const unsigned int * data, unsigned int count);

void lv_opengles_index_buffer_deinit();

unsigned int lv_opengles_index_buffer_get_count();

void lv_opengles_index_buffer_bind();

void lv_opengles_index_buffer_unbind();

void lv_opengles_render_clear();

unsigned int lv_opengles_shader_compile(unsigned int type, const char * source);

unsigned int lv_opengles_shader_create(const char * vertexShader, const char * fragmentShader);

void lv_opengles_shader_init();

void lv_opengles_shader_deinit();

void lv_opengles_shader_bind();

void lv_opengles_shader_unbind();

int lv_opengles_shader_get_uniform_location(const char * name);

void lv_opengles_shader_set_uniform1i(const char * name, int value);

void lv_opengles_shader_set_uniform1f(const char * name, float value);

void lv_opengles_shader_set_uniform4f(const char * name, float v0, float v1, float v2, float v3);

void lv_opengles_render_draw();

void lv_opengles_texture_init(void * buffer, int width, int height);

void lv_opengles_texture_deinit();

void lv_opengles_texture_bind(unsigned int slot);

void lv_opengles_texture_unbind();

void lv_opengles_texture_update(void * buffer, int width, int height);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* LV_OPENGLES_DRIVER_H */
