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

#include "../math/lv_3dmath.h"
#include "../../../misc/lv_types.h"
#include "../../../misc/lv_area.h"
#include "../gltf_data/lv_gltf_model.h"

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      DEFINES
 *********************/

#define LV_GLTF_ANIM_SPEED_TENTH 100
#define LV_GLTF_ANIM_SPEED_QUARTER 250
#define LV_GLTF_ANIM_SPEED_HALF 500
#define LV_GLTF_ANIM_SPEED_NORMAL 1000
#define LV_GLTF_ANIM_SPEED_2X 2000
#define LV_GLTF_ANIM_SPEED_3X 3000
#define LV_GLTF_ANIM_SPEED_4X 4000
#define LV_GLTF_DEFAULT_CAMERA 0

/**********************
 *      TYPEDEFS
 **********************/

typedef enum {
    LV_GLTF_AA_MODE_OFF = 0, /** Anti aliasing off*/
    LV_GLTF_AA_MODE_ON = 1,  /** Anti aliasing on*/
    LV_GLTF_AA_MODE_DYNAMIC = 2, /** Anti aliasing on only when frame has no movement*/
} lv_gltf_aa_mode_t;

typedef enum {
    LV_GLTF_BG_MODE_SOLID = 0, /** Solid background. Use `lv_obj_set_style_bg_color` to set the background color*/
    LV_GLTF_BG_MODE_ENVIRONMENT = 1, /** Environnement background*/
} lv_gltf_bg_mode_t;


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create a glTF object
 * @param parent pointer to the parent object
 * @return pointer to the created glTF object
 */
lv_obj_t * lv_gltf_create(lv_obj_t * parent);

/**
 * Assign an environment to a glTF object for IBL rendering
 * @param obj pointer to a glTF viewer object
 * @param environment pointer to the environment to use
 * @note The environment can be shared across multiple glTF objects
 * @note If no environment is set before attempting to load a file,
 *       a default one will be created for you
 */
void lv_gltf_set_environment(lv_obj_t * obj, lv_gltf_environment_t * environment);

/**
 * Load a glTF model from a file into the viewer
 * @param obj pointer to a glTF viewer object
 * @param path file path to the glTF model to load
 * @return pointer to the loaded glTF model, or NULL on failure
 */
lv_gltf_model_t * lv_gltf_load_model_from_file(lv_obj_t * obj, const char * path);

/**
 * Load a glTF model from a byte array into the viewer
 * @param obj pointer to a glTF viewer object
 * @param bytes glTF raw data
 * @param len glTF raw data length in bytes
 * @return pointer to the loaded glTF model, or NULL on failure
 */
lv_gltf_model_t * lv_gltf_load_model_from_bytes(lv_obj_t * obj, const uint8_t * bytes, size_t len);

/**
 * Get the number of models loaded in the glTF viewer
 * @param obj pointer to a glTF viewer object
 * @return the total number of models in the viewer
 */
size_t lv_gltf_get_model_count(lv_obj_t * obj);

/**
 * Get a specific model by its index
 * @param obj pointer to a glTF viewer object
 * @param id index of the model to retrieve (0-based)
 * @return pointer to the model at the specified index, or NULL if index is invalid
 */
lv_gltf_model_t * lv_gltf_get_model_by_index(lv_obj_t * obj, size_t id);

/**
 * Get the primary model from the glTF viewer
 * The primary model is the first model added to the viewer and can be used
 * for camera selection and other primary operations
 * @param obj pointer to a glTF viewer object
 * @return pointer to the primary model, or NULL if no models are loaded
 */
lv_gltf_model_t * lv_gltf_get_primary_model(lv_obj_t * obj);

/**
 * Set the yaw (horizontal rotation) of the camera
 * @param obj pointer to a glTF viewer object
 * @param yaw yaw angle in degrees
 */
void lv_gltf_set_yaw(lv_obj_t * obj, float yaw);

/**
 * Get the yaw (horizontal rotation) of the camera
 * @param obj pointer to a glTF viewer object
 * @return yaw angle in degrees
 */
float lv_gltf_get_yaw(const lv_obj_t * obj);

/**
 * Set the pitch (vertical rotation) of the camera
 * @param obj pointer to a glTF viewer object
 * @param pitch pitch angle in degrees
 */
void lv_gltf_set_pitch(lv_obj_t * obj, float pitch);

/**
 * Get the pitch (vertical rotation) of the camera
 * @param obj pointer to a glTF viewer object
 * @return pitch angle in degrees
 */
float lv_gltf_get_pitch(const lv_obj_t * obj);

/**
 * Set the camera distance from the focal point
 * @param obj pointer to a glTF viewer object
 * @param value distance value
 */
void lv_gltf_set_distance(lv_obj_t * obj, float value);

/**
 * Get the camera distance scale factor from the focal point
 * @param obj pointer to a glTF viewer object
 * @return distance scaling factor value
 */
float lv_gltf_get_distance(const lv_obj_t * obj);

/**
 * Get the camera distance from the focal point in world units
 * @param obj pointer to a GLTF viewer object
 * @return world unit distance value
 */
float lv_gltf_get_world_distance(const lv_obj_t * obj);

/**********************
 * Viewport Functions
 **********************/

/**
 * Set the field of view
 * @param obj pointer to a glTF viewer object
 * @param value vertical FOV in degrees. If zero, the view will be orthographic (non-perspective)
 */
void lv_gltf_set_fov(lv_obj_t * obj, float value);

/**
 * Get the field of view
 * @param obj pointer to a glTF viewer object
 * @return vertical FOV in degrees
 */
float lv_gltf_get_fov(const lv_obj_t * obj);

/**********************
 * Focal Point Functions
 **********************/

/**
 * Set the X coordinate of the camera focal point
 * @param obj pointer to a glTF viewer object
 * @param value X coordinate
 */
void lv_gltf_set_focal_x(lv_obj_t * obj, float value);

/**
 * Get the X coordinate of the camera focal point
 * @param obj pointer to a glTF viewer object
 * @return X coordinate
 */
float lv_gltf_get_focal_x(const lv_obj_t * obj);

/**
 * Set the Y coordinate of the camera focal point
 * @param obj pointer to a glTF viewer object
 * @param value Y coordinate
 */
void lv_gltf_set_focal_y(lv_obj_t * obj, float value);

/**
 * Get the Y coordinate of the camera focal point
 * @param obj pointer to a glTF viewer object
 * @return Y coordinate
 */
float lv_gltf_get_focal_y(const lv_obj_t * obj);

/**
 * Set the Z coordinate of the camera focal point
 * @param obj pointer to a glTF viewer object
 * @param value Z coordinate
 */
void lv_gltf_set_focal_z(lv_obj_t * obj, float value);

/**
 * Get the Z coordinate of the camera focal point
 * @param obj pointer to a glTF viewer object
 * @return Z coordinate
 */
float lv_gltf_get_focal_z(const lv_obj_t * obj);

/**
 * Set the focal coordinates to the center point of the model object
 * @param obj pointer to a glTF viewer object
 * @param model a model attached to this viewer or NULL for the first model
 */
void lv_gltf_recenter(lv_obj_t * obj, lv_gltf_model_t * model);

/**********************
 * Scene Control Functions
 **********************/

/**
 * Set the active camera index
 * The camera is selected from the first glTF model added to the viewer
 *
 * @param obj pointer to a glTF viewer object
 * @param value camera index (0 for default camera, 1+ for scene camera index)
 * @note Values higher than the scene's camera count will be clamped to the maximum available camera index
 */
void lv_gltf_set_camera(lv_obj_t * obj, uint32_t value);

/**
 * Get the active camera index
 * @param obj pointer to a glTF viewer object
 * @return active camera index
 */
uint32_t lv_gltf_get_camera(const lv_obj_t * obj);

/**
 * Get the number of cameras in the first glTF model added to the viewer
 * This count represents the valid range for the camera index parameter
 * used with lv_gltf_set_camera()
 *
 * To get the camera count of other models, call
 * lv_gltf_model_get_camera_count(model) directly with the specific model
 *
 * @param obj pointer to a glTF viewer object
 * @return number of available cameras
 */
uint32_t lv_gltf_get_camera_count(const lv_obj_t * obj);

/**
 * Set the animation speed ratio
 *
 * The actual ratio is the value parameter / LV_GLTF_ANIM_SPEED_NORMAL
 * Values greater than LV_GLTF_ANIM_SPEED_NORMAL will speed-up the animation
 * Values less than LV_GLTF_ANIM_SPEED_NORMAL will slow down the animation
 *
 * @param obj pointer to a glTF viewer object
 * @param value speed-up ratio of the animation
 */
void lv_gltf_set_animation_speed(lv_obj_t * obj, uint32_t value);

/**
 * Get the animation speed ratio
 *
 * The actual ratio is the return value / LV_GLTF_ANIM_SPEED_NORMAL
 *
 * @param obj pointer to a glTF viewer object
 */
uint32_t lv_gltf_get_animation_speed(const lv_obj_t * obj);

/**********************
 * Visual Settings Functions
 **********************/

/**
 * Set the background mode
 * @param obj pointer to a glTF viewer object
 * @param value background mode
 */
void lv_gltf_set_background_mode(lv_obj_t * obj, lv_gltf_bg_mode_t value);

/**
 * Get the background mode
 * @param obj pointer to a glTF viewer object
 * @return background mode
 */
lv_gltf_bg_mode_t lv_gltf_get_background_mode(const lv_obj_t * obj);

/**
 * Set the background blur amount
 * @param obj pointer to a glTF viewer object
 * @param value blur amount between 0 and 100
 */
void lv_gltf_set_background_blur(lv_obj_t * obj, uint32_t value);

/**
 * Get the background blur amount
 * @param obj pointer to a glTF viewer object
 * @return blur amount between 0 and 100
 */
uint32_t lv_gltf_get_background_blur(const lv_obj_t * obj);

/**
 * Set the environmental brightness/power
 * @param obj pointer to a glTF viewer object
 * @param value brightness multiplier
 */
void lv_gltf_set_env_brightness(lv_obj_t * obj, uint32_t value);

/**
 * Get the environmental brightness/power
 * @param obj pointer to a glTF viewer object
 * @return brightness multiplier
 */
uint32_t lv_gltf_get_env_brightness(const lv_obj_t * obj);

/**
 * Set the image exposure level
 * @param obj pointer to a glTF viewer object
 * @param value exposure level (1.0 is default)
 */
void lv_gltf_set_image_exposure(lv_obj_t * obj, float value);

/**
 * Get the image exposure level
 * @param obj pointer to a glTF viewer object
 * @return exposure level
 */
float lv_gltf_get_image_exposure(const lv_obj_t * obj);

/**********************
 * Rendering Functions
 **********************/

/**
 * Set the anti-aliasing mode
 * @param obj pointer to a glTF viewer object
 * @param value anti-aliasing mode
 */
void lv_gltf_set_antialiasing_mode(lv_obj_t * obj, lv_gltf_aa_mode_t value);

/**
 * Get the anti-aliasing mode
 * @param obj pointer to a glTF viewer object
 * @return anti-aliasing mode
 */
lv_gltf_aa_mode_t lv_gltf_get_antialiasing_mode(const lv_obj_t * obj);

/***********************
 * Raycasting Functions
 ***********************/

/**
 * Get the point that a given ray intersects with a specified plane at, if any
 * @param ray the intersection test ray
 * @param screen_y the plane to test ray intersection with
 * @param collision_point output lv_3dpoint_t holder, values are only valid if true is the return value
 * @return LV_RESULT_OK if intersection, LV_RESULT_INVALID if no intersection
 */
lv_result_t lv_intersect_ray_with_plane(const lv_3dray_t * ray, const lv_3dplane_t * plane,
                                        lv_3dpoint_t * collision_point);

/**
 * Get a plane that faces the current view camera, centered some units in front of it
 * @param obj pointer to a GLTF viewer object
 * @param distance distance in front of the camera to set the plane, in world units. see lv_gltf_get_world_distance to get the auto-distance
 * @return camera facing plane
 */
lv_3dplane_t lv_gltf_get_current_view_plane(lv_obj_t * obj, float distance);

/**
 * Calculates a ray originating from the camera and passing through the specified mouse position on the screen.
 * @param obj pointer to a GLTF viewer object
 * @param screen_pos screen co-ordinate, in pixels
 * @return mouse point ray
 */
lv_3dray_t lv_gltf_get_ray_from_2d_coordinate(lv_obj_t * obj, const lv_point_t * screen_pos);


/**
 * Get the screen position of a 3d point
 * @param obj pointer to a GLTF viewer object
 * @param world_pos world position to convert
 * @param lv_point_t the resulting point, in pixels. only valid if return value is true
 * @return LV_RESULT_OK if conversion valid, LV_RESULT_INVALID if no valid conversion
 */
lv_result_t lv_gltf_world_to_screen(lv_obj_t * obj, const lv_3dpoint_t world_pos, lv_point_t * screen_pos);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
}
#endif

#endif /*LV_USE_GLTF*/

#endif /*LV_GLTF_H*/
