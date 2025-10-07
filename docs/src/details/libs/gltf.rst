.. _gltf:

====
glTF
====

**glTF** (GL Transmission Format) is a royalty-free specification for efficient transmission and loading of 3D scenes and models.

The glTF extension in LVGL provides 3D model loading, rendering, and animation capabilities. It supports the full glTF 2.0 specification with advanced rendering features.

For a detailed introduction to glTF, see: https://www.khronos.org/gltf/

Features
********

LVGL's glTF implementation provides comprehensive 3D rendering capabilities:

**File Format Support:**

* Loading glTF (.gltf) and binary GLB (.glb) files from local filesystem
* Support for hex-encoded bytes in source include files when filesystem is not available
* JPG, PNG, and WebP compressed texture support
* External texture files or textures embedded within the glTF source

**Rendering & Lighting:**

* Image-Based Lighting (IBL) for realistic environmental lighting
* Punctual lighting support with animated light sources
* Physically Based Rendering (PBR) materials with full texture support:
  
  * Diffuse/albedo textures
  * Roughness and metallic workflow textures  
  * Normal maps for surface detail
  * Ambient occlusion textures

**Advanced Materials:**

* Emissive materials for glowing effects and self-illuminated surfaces
* Refractive materials with realistic distortion effects revealing geometry behind surfaces
* Clearcoat material support for multi-layer surface effects (like automotive paint finishes)
* Full transparency support with alpha blending

**Animation System:**

* Keyframe-based animations for object transformations
* Single-skeleton skinned character animations
* Animated punctual lights with dynamic lighting effects
* Parameter binding for real-time monitoring and control of object properties (position, rotation, scale)
* Dynamic parameter override from host application

**Camera & Viewport:**

* Programmatic viewport control with full camera manipulation
* Support for cameras defined within 3D editor software
* Multiple camera switching for different scene perspectives
* Orthographic and perspective projection modes

**Rendering Quality:**

* Configurable antialiasing with multiple modes:
  
  * Always-on for consistent quality
  * Always-off for maximum performance  
  * Auto-on that activates when scene movement stops

* Flexible background rendering:
  
  * Environment-based backgrounds using IBL
  * Solid color backgrounds
  * Transparent backgrounds for overlay effects

What is glTF?
*************

glTF (Graphics Library Transmission Format) is a 3D file format that stores 3D model information in JSON format. It supports:

- **Geometry**: Meshes, primitives, and vertex data
- **Materials**: PBR (Physically Based Rendering) materials with textures
- **Animations**: Keyframe-based animations for nodes
- **Cameras**: Perspective and orthographic camera definitions  
- **Scenes**: Hierarchical scene graphs with node transformations

glTF files can be stored as:

- **.gltf**: JSON format with external binary and image files
- **.glb**: Binary format with all assets embedded in a single file

Features
********

LVGL's glTF implementation provides comprehensive 3D rendering capabilities:

**File Format Support:**

* Loading glTF (.gltf) and binary GLB (.glb) files from local filesystem
* Support for hex-encoded bytes in source include files when filesystem is not available
* JPG, PNG, and WebP compressed texture support
* External texture files or textures embedded within the glTF source

**Rendering & Lighting:**

* Image-Based Lighting (IBL) for realistic environmental lighting
* Punctual lighting support with animated light sources
* Physically Based Rendering (PBR) materials with full texture support:
  
  * Diffuse/albedo textures
  * Roughness and metallic workflow textures  
  * Normal maps for surface detail
  * Ambient occlusion textures

**Advanced Materials:**

* Emissive materials for glowing effects and self-illuminated surfaces
* Refractive materials with realistic distortion effects revealing geometry behind surfaces
* Clearcoat material support for multi-layer surface effects (like automotive paint finishes)
* Full transparency support with alpha blending

**Animation System:**

* Keyframe-based animations for object transformations
* Single-skeleton skinned character animations
* Animated punctual lights with dynamic lighting effects
* Parameter binding for real-time monitoring and control of object properties (position, rotation, scale)
* Dynamic parameter override from host application

**Camera & Viewport:**

* Programmatic viewport control with full camera manipulation
* Support for cameras defined within 3D editor software
* Multiple camera switching for different scene perspectives
* Orthographic and perspective projection modes

**Rendering Quality:**

* Configurable antialiasing with multiple modes:
  
  * Always-on for consistent quality
  * Always-off for maximum performance  
  * Auto-on that activates when scene movement stops

* Flexible background rendering:
  
  * Environment-based backgrounds using IBL
  * Solid color backgrounds
  * Transparent backgrounds for overlay effects

Requirements
************

The glTF extension relies on **OpenGL ES 3.0** for 3D rendering. LVGL includes built-in support for OpenGL ES through the GLFW driver, which provides cross-platform window management and OpenGL context creation.

The renderer uses OpenGL ES 3.0 shaders (GLSL version 300 es) to provide modern PBR (Physically Based Rendering) capabilities.

Dependencies
------------

The glTF extension requires the following external libraries:

:fastgltf:     A C++20 library for parsing glTF files (https://github.com/spnda/fastgltf)
:libwebp:      WebP image format support for textures (https://github.com/webmproject/libwebp)

Setup
*****

1. **Install Dependencies with CMake**

   The recommended way to integrate the required libraries is using CMake's FetchContent:

.. code-block:: cmake

    include(FetchContent)
    # Set policy to allow to run the target_link_libraries cmd on targets that are
    # build in another directory. Currently, the linking is not handled by
    cmake_policy(SET CMP0079 NEW)
    # Fetch fastgltf library
    FetchContent_Declare(
      fastgltf
      GIT_REPOSITORY https://github.com/spnda/fastgltf
      GIT_TAG 4e2261350888bae7c35a1f39991f6233d57795f5)
    set(FASTGLTF_ENABLE_DEPRECATED_EXT
        ON
        CACHE BOOL "" FORCE)
    set(FASTGLTF_DIFFUSE_TRANSMISSION_SUPPORT
        ON
        CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(fastgltf)
    
    # Fetch libwebp library  
    FetchContent_Declare(
      webp
      GIT_REPOSITORY https://github.com/webmproject/libwebp
      GIT_TAG fa6f56496a442eed59b103250021e4b14ebf1427)
    FetchContent_MakeAvailable(webp)

    # Link libraries to LVGL
    target_link_libraries(lvgl PUBLIC webp fastgltf)

2. **Enable glTF Support**

   Set :c:macro:`LV_USE_GLTF` to ``1`` in ``lv_conf.h``.

   Also enable other required dependencies by setting the following defines to ``1``:

   - :c:macro:`LV_USE_OPENGLES`
   - :c:macro:`LV_USE_DRAW_OPENGLES`
   - :c:macro:`LV_USE_3DTEXTURE` 

3. **Setup OpenGL ES Driver** 

   Follow the OpenGL ES driver setup documentation (:ref:`opengl_driver`) to configure GLFW and OpenGL ES support for your platform.

4. **Basic Setup Example**

.. code-block:: c

    int main(void)
    {
        /* Initialize LVGL */
        lv_init();
        
        /* GLFW setup */
        lv_opengles_window_t *window = lv_opengles_glfw_window_create(WINDOW_WIDTH, WINDOW_HEIGHT, true);
        lv_display_t *display = lv_opengles_texture_create(WINDOW_WIDTH, WINDOW_HEIGHT);
        unsigned int texture_id = lv_opengles_texture_get_texture_id(display);
        lv_opengles_window_add_texture(window, texture_id, WINDOW_WIDTH, WINDOW_HEIGHT);
        
        /* Load and display glTF demo */
        lv_demo_gltf("A:<path/to/gltf>");
        
        while (1) {
            uint32_t time_until_next = lv_timer_handler();
            if (time_until_next == LV_NO_TIMER_READY) {
                time_until_next = LV_DEF_REFR_PERIOD;
            }
            lv_delay_ms(time_until_next);
        }
        return 0;
    }

Usage
*****

Demo Application
----------------

LVGL provides a complete glTF demo that showcases the capabilities of the 3D viewer:

.. code-block:: c

    lv_demo_gltf("A:<path/to/your/model.glb>");

This demo creates an interactive 3D viewer with:

- Mouse/touch controls for camera rotation
- Zoom controls
- Animation playback controls
- Camera switching
- Visual settings adjustment

Basic glTF Viewer Creation
--------------------------

Here's how to create a basic glTF viewer and load a model:

.. code-block:: c

    /* Create a glTF viewer object */
    lv_obj_t * gltf = lv_gltf_create(lv_screen_active());
    
    /* Load a glTF model from file */
    lv_gltf_model_t * model = lv_gltf_load_model_from_file(gltf,
        "A:path/to/your/model.glb");
    
    if (model == NULL) {
        LV_LOG_ERROR("Failed to load glTF model");
        return;
    }

Camera Controls
---------------

The glTF viewer provides comprehensive camera controls:

.. code-block:: c

    /* Set camera rotation */
    lv_gltf_set_yaw(gltf, 45.0f);      /* Horizontal rotation */
    lv_gltf_set_pitch(gltf, -30.0f);   /* Vertical rotation */
    
    /* Set camera distance */
    lv_gltf_set_distance(gltf, 5.0f);
    
    /* Set field of view (0 for orthographic) */
    lv_gltf_set_fov(gltf, 60.0f);
    
    /* Set the focal point - The position the camera is pointing at */
    lv_gltf_set_focal_x(gltf, 0.0f);
    lv_gltf_set_focal_y(gltf, 0.0f);
    lv_gltf_set_focal_z(gltf, 0.0f);
    
    /* Recenter camera on model */
    lv_gltf_recenter(gltf, model);

Animation Control
-----------------

Control model animations with these functions:

.. code-block:: c

    /* Get number of animations */
    size_t anim_count = lv_gltf_model_get_animation_count(model);
    
    /* Play a specific animation */
    lv_gltf_model_play_animation(model, 0);
    
    /* Control animation speed */
    lv_gltf_set_animation_speed(gltf, LV_GLTF_ANIM_SPEED_2X);
    
    /* Pause/resume animation */
    lv_gltf_model_pause_animation(model);
    bool is_paused = lv_gltf_model_is_animation_paused(model);

Visual Settings
---------------

Customize the visual appearance of your 3D scene:

.. code-block:: c

    /* Background settings */
    lv_gltf_set_background_mode(gltf, LV_GLTF_BG_ENVIRONMENT);
    lv_gltf_set_background_blur(gltf, 50);
    
    /* Lighting and exposure */
    lv_gltf_set_env_brightness(gltf, 150);
    lv_gltf_set_image_exposure(gltf, 1.2f);
    
    /* Anti-aliasing */
    lv_gltf_set_antialiasing_mode(gltf, LV_GLTF_AA_DYNAMIC);

Multi-Model Support
-------------------

Load and manage multiple glTF models in a single viewer:

.. code-block:: c

    /* Load multiple models */
    lv_gltf_model_t * model1 = lv_gltf_load_model_from_file(gltf, "A:model1.glb");
    lv_gltf_model_t * model2 = lv_gltf_load_model_from_file(gltf, "A:model2.glb");
    
    /* Get model information */
    size_t model_count = lv_gltf_get_model_count(gltf);
    lv_gltf_model_t * primary = lv_gltf_get_primary_model(gltf);
    lv_gltf_model_t * specific = lv_gltf_get_model_by_index(gltf, 1);

Model Inspection
----------------

Query model properties to understand its structure:

.. code-block:: c

    /* Get model component counts */
    size_t mesh_count = lv_gltf_model_get_mesh_count(model);
    size_t material_count = lv_gltf_model_get_material_count(model);
    size_t texture_count = lv_gltf_model_get_texture_count(model);
    size_t image_count = lv_gltf_model_get_image_count(model);
    size_t node_count = lv_gltf_model_get_node_count(model);
    size_t scene_count = lv_gltf_model_get_scene_count(model);
    size_t camera_count = lv_gltf_model_get_camera_count(model);

Widget Architecture
*******************

The glTF widget extends the ``lv_3dtexture`` widget, which means:

- All standard ``lv_obj`` functions work with glTF widgets (positioning, sizing, styling, etc.)
- All ``lv_3dtexture`` functions are also available for advanced 3D texture management

Animation Speed System
----------------------

Animation speeds use integer values to avoid floating-point arithmetic:

- The speed ratio is calculated as: ``speed_value / LV_GLTF_ANIM_SPEED_NORMAL``
- ``LV_GLTF_ANIM_SPEED_NORMAL`` equals 1000, representing 1.0x (normal) speed
- Values greater than 1000 speed up animations (e.g., 2000 = 2.0x speed)
- Values less than 1000 slow down animations (e.g., 500 = 0.5x speed)

.. _gltf_example:

Examples
********

.. include:: ../../examples/libs/gltf/index.rst

.. _gltf_api:

API
***

.. API startswith:  lv_gltf_
