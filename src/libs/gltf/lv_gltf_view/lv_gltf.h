/**
 * @file lv_gltf.h
 *
 */

#ifndef LV_GLTF_H
#define LV_GLTF_H

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

#if LV_USE_GLTF

#include "../../../misc/lv_types.h"
#include "../../../misc/lv_color.h"
#include "../lv_gltf_data/lv_gltf_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GLTF_AA_OFF = 0,
    LV_GLTF_AA_CONSTANT = 1,
    LV_GLTF_AA_NOT_MOVING = 2,
} lv_gltf_antialiasing_mode_t;

typedef enum {
    LV_GLTF_BG_CLEAR = 0,
    LV_GLTF_BG_SOLID = 1,
    LV_GLTF_BG_ENVIRONMENT = 2,
} lv_gltf_background_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

lv_obj_t * lv_gltf_create(lv_obj_t * parent);
lv_gltf_model_t * lv_gltf_load_model_from_file(lv_obj_t * obj, const char * path);

/**
 * Set the yaw (horizontal rotation) of the camera
 * @param obj pointer to a GLTF viewer object
 * @param yaw yaw angle in degrees
 */
void lv_gltf_set_yaw(lv_obj_t * obj, float yaw);

/**
 * Get the yaw (horizontal rotation) of the camera
 * @param obj pointer to a GLTF viewer object
 * @return yaw angle in degrees
 */
float lv_gltf_get_yaw(const lv_obj_t * obj);

/**
 * Set the pitch (vertical rotation) of the camera
 * @param obj pointer to a GLTF viewer object
 * @param pitch pitch angle in degrees
 */
void lv_gltf_set_pitch(lv_obj_t * obj, float pitch);

/**
 * Get the pitch (vertical rotation) of the camera
 * @param obj pointer to a GLTF viewer object
 * @return pitch angle in degrees
 */
float lv_gltf_get_pitch(const lv_obj_t * obj);

/**
 * Set the camera distance from the focal point
 * @param obj pointer to a GLTF viewer object
 * @param value distance value
 */
void lv_gltf_set_distance(lv_obj_t * obj, float value);

/**
 * Get the camera distance from the focal point
 * @param obj pointer to a GLTF viewer object
 * @return distance value
 */
float lv_gltf_get_distance(const lv_obj_t * obj);

/**********************
 * Viewport Functions
 **********************/

/**
 * Set the field of view
 * @param obj pointer to a GLTF viewer object
 * @param value vertical FOV in degrees. If zero, the view will be orthographic (non-perspective)
 */
void lv_gltf_set_fov(lv_obj_t * obj, float value);

/**
 * Get the field of view
 * @param obj pointer to a GLTF viewer object
 * @return vertical FOV in degrees
 */
float lv_gltf_get_fov(const lv_obj_t * obj);

/**********************
 * Focal Point Functions
 **********************/

/**
 * Set the X coordinate of the camera focal point
 * @param obj pointer to a GLTF viewer object
 * @param value X coordinate
 */
void lv_gltf_set_focal_x(lv_obj_t * obj, float value);

/**
 * Get the X coordinate of the camera focal point
 * @param obj pointer to a GLTF viewer object
 * @return X coordinate
 */
float lv_gltf_get_focal_x(const lv_obj_t * obj);

/**
 * Set the Y coordinate of the camera focal point
 * @param obj pointer to a GLTF viewer object
 * @param value Y coordinate
 */
void lv_gltf_set_focal_y(lv_obj_t * obj, float value);

/**
 * Get the Y coordinate of the camera focal point
 * @param obj pointer to a GLTF viewer object
 * @return Y coordinate
 */
float lv_gltf_get_focal_y(const lv_obj_t * obj);

/**
 * Set the Z coordinate of the camera focal point
 * @param obj pointer to a GLTF viewer object
 * @param value Z coordinate
 */
void lv_gltf_set_focal_z(lv_obj_t * obj, float value);

/**
 * Get the Z coordinate of the camera focal point
 * @param obj pointer to a GLTF viewer object
 * @return Z coordinate
 */
float lv_gltf_get_focal_z(const lv_obj_t * obj);

/**
 * Set the focal coordinates to the center point of the model object
 * @param obj pointer to a GLTF viewer object
 * @param model a model attached to this viewer or NULL for the first model
 */
void lv_gltf_recenter(lv_obj_t * obj, lv_gltf_model_t * model);

/**********************
 * Scene Control Functions
 **********************/

/**
 * Set the active camera index
 * @param obj pointer to a GLTF viewer object
 * @param value camera index (-1 for default, 0 = platter, 1+ = scene camera index)
 * @note Values higher than the scene's camera count will be limited to the scene's camera count
 */
void lv_gltf_set_camera(lv_obj_t * obj, int32_t value);

/**
 * Get the active camera index
 * @param obj pointer to a GLTF viewer object
 * @return camera index
 */
int32_t lv_gltf_get_camera(const lv_obj_t * obj);

/**
 * Set the active animation index
 * @param obj pointer to a GLTF viewer object
 * @param value animation index (-1 for no animations, 0+ = animation index)
 * @note Values higher than the scene's animation count will be limited to the scene's animation count
 */
void lv_gltf_set_animation(lv_obj_t * obj, int32_t value);

/**
 * Get the active animation index
 * @param obj pointer to a GLTF viewer object
 * @return animation index
 */
int32_t lv_gltf_get_animation(const lv_obj_t * obj);

/**
 * Set the animation timestep
 * @param obj pointer to a GLTF viewer object
 * @param value timestep in seconds to advance the current animation
 */
void lv_gltf_set_timestep(lv_obj_t * obj, float value);

/**
 * Get the animation timestep
 * @param obj pointer to a GLTF viewer object
 * @return timestep in seconds
 */
float lv_gltf_get_timestep(const lv_obj_t * obj);

/**********************
 * Visual Settings Functions
 **********************/

/**
 * Set the background color
 * @param obj pointer to a GLTF viewer object
 * @param value background color
 */
void lv_gltf_set_background_color(lv_obj_t * obj, lv_color32_t value);

/**
 * Get the background color
 * @param obj pointer to a GLTF viewer object
 * @return background color
 */
lv_color32_t lv_gltf_get_background_color(const lv_obj_t * obj);

/**
 * Set the background mode
 * @param obj pointer to a GLTF viewer object
 * @param value background mode
 */
void lv_gltf_set_background_mode(lv_obj_t * obj, lv_gltf_background_mode_t value);

/**
 * Get the background mode
 * @param obj pointer to a GLTF viewer object
 * @return background mode
 */
lv_gltf_background_mode_t lv_gltf_get_background_mode(const lv_obj_t * obj);

/**
 * Set the background blur amount
 * @param obj pointer to a GLTF viewer object
 * @param value blur amount between 0.0 and 1.0
 */
void lv_gltf_set_blur_bg(lv_obj_t * obj, float value);

/**
 * Get the background blur amount
 * @param obj pointer to a GLTF viewer object
 * @return blur amount between 0.0 and 1.0
 */
float lv_gltf_get_blur_bg(const lv_obj_t * obj);

/**
 * Set the environmental brightness/power
 * @param obj pointer to a GLTF viewer object
 * @param value brightness multiplier (1.0 is default)
 */
void lv_gltf_set_env_brightness(lv_obj_t * obj, float value);

/**
 * Get the environmental brightness/power
 * @param obj pointer to a GLTF viewer object
 * @return brightness multiplier
 */
float lv_gltf_get_env_brightness(const lv_obj_t * obj);

/**
 * Set the image exposure level
 * @param obj pointer to a GLTF viewer object
 * @param value exposure level (1.0 is default)
 */
void lv_gltf_set_image_exposure(lv_obj_t * obj, float value);

/**
 * Get the image exposure level
 * @param obj pointer to a GLTF viewer object
 * @return exposure level
 */
float lv_gltf_get_image_exposure(const lv_obj_t * obj);

/**********************
 * Rendering Functions
 **********************/

/**
 * Set the anti-aliasing mode
 * @param obj pointer to a GLTF viewer object
 * @param value anti-aliasing mode
 */
void lv_gltf_set_antialiasing_mode(lv_obj_t * obj, lv_gltf_antialiasing_mode_t value);

/**
 * Get the anti-aliasing mode
 * @param obj pointer to a GLTF viewer object
 * @return anti-aliasing mode
 */
lv_gltf_antialiasing_mode_t lv_gltf_get_antialiasing_mode(const lv_obj_t * obj);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LV_USE_GLTF*/

#endif /*LV_GLTF_H*/
