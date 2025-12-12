.. _gltf:

====
glTF
====

**glTF** (GL Transmission Format) is a royalty-free specification for efficient transmission and loading of 3D scenes and models.

The glTF extension in LVGL provides 3D model loading, rendering, and animation capabilities. It supports the full glTF 2.0 specification with advanced rendering features.

For a detailed introduction to glTF, see: https://www.khronos.org/gltf/



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


File Format Support
-------------------

* Loading glTF (.gltf) and binary GLB (.glb) files from local filesystem
* Support for hex-encoded bytes in source include files when filesystem is not available
* JPG, PNG, and WebP compressed texture support
* External texture files or textures embedded within the glTF source


Rendering & Lighting
---------------------

* Image-Based Lighting (IBL) for realistic environmental lighting
* Punctual lighting support with animated light sources
* Physically Based Rendering (PBR) materials with full texture support:
  
  * Diffuse/albedo textures
  * Roughness and metallic workflow textures  
  * Normal maps for surface detail
  * Ambient occlusion textures


Advanced Materials
------------------

* Emissive materials for glowing effects and self-illuminated surfaces
* Refractive materials with realistic distortion effects revealing geometry behind surfaces
* Clearcoat material support for multi-layer surface effects (like automotive paint finishes)
* Full transparency support with alpha blending


Animation System
----------------

* Keyframe-based animations for object transformations
* Single-skeleton skinned character animations
* Animated punctual lights with dynamic lighting effects
* Parameter binding for real-time monitoring and control of object properties (position, rotation, scale)
* Dynamic parameter override from host application


Camera & Viewport
-----------------

* Programmatic viewport control with full camera manipulation
* Support for cameras defined within 3D editor software
* Multiple camera switching for different scene perspectives
* Orthographic and perspective projection modes


Rendering Quality
-----------------

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

The glTF extension relies on **OpenGL ES 2.0** with some extension support for 3D rendering. LVGL includes built-in support for OpenGL ES through the GLFW driver, which provides cross-platform window management and OpenGL context creation.

The renderer uses OpenGL ES 3.0 shaders (GLSL version 300 es) to provide modern PBR (Physically Based Rendering) capabilities.

The full list of extensions required can be found `here <https://gen.glad.sh/#generator=c&api=egl%3D1.5%2Cgles2%3D2.0&profile=gl%3Dcompatibility%2Cgles1%3Dcommon&extensions=EGL_EXT_image_dma_buf_import%2CEGL_EXT_image_dma_buf_import_modifiers%2CEGL_EXT_platform_base%2CEGL_EXT_platform_wayland%2CEGL_KHR_fence_sync%2CEGL_KHR_image_base%2CEGL_KHR_platform_gbm%2CEGL_KHR_platform_wayland%2CGL_APPLE_texture_max_level%2CGL_ARM_rgba8%2CGL_EXT_color_buffer_float%2CGL_EXT_color_buffer_half_float%2CGL_EXT_texture_format_BGRA8888%2CGL_EXT_texture_storage%2CGL_EXT_unpack_subimage%2CGL_OES_depth24%2CGL_OES_mapbuffer%2CGL_OES_rgb8_rgba8%2CGL_OES_texture_float%2CGL_OES_texture_half_float%2CGL_OES_texture_storage_multisample_2d_array%2CGL_OES_vertex_array_object&options=ALIAS>`__.


Dependencies
------------

The glTF extension requires the following external libraries:

:fastgltf:     A C++20 library for parsing glTF files (https://github.com/spnda/fastgltf)
:libwebp:      WebP image format support for textures (https://github.com/webmproject/libwebp)



Setup
*****

Install Dependencies with CMake
--------------------------------

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


Enable glTF Support
-------------------

Set :c:macro:`LV_USE_GLTF` to ``1`` in ``lv_conf.h``.

Also enable other required dependencies by setting the following defines to ``1``:

- :c:macro:`LV_USE_OPENGLES`
- :c:macro:`LV_USE_DRAW_OPENGLES`
- :c:macro:`LV_USE_3DTEXTURE` 


Setup OpenGL ES Driver
-----------------------

Follow the OpenGL ES driver setup documentation (:ref:`opengl_driver`) to configure a driver that supports OpenGL for your platform.


Basic Setup Example
-------------------

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
---------------------------

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


Runtime Node Manipulation
-------------------------

You can dynamically modify node properties (position, rotation, scale) at runtime and receive notifications when these values change.

Reading Node Properties
~~~~~~~~~~~~~~~~~~~~~~~

Change individual node properties using setter functions. Changes are queued and applied during the next render cycle:


.. code-block:: c

    /* Get a node from the model */
    lv_gltf_model_node_t * node = lv_gltf_model_node_get_by_index(model, 0);
    
    /* Set position */
    lv_gltf_model_node_set_position_x(node, 1.5f);
    lv_gltf_model_node_set_position_y(node, 0.0f);
    lv_gltf_model_node_set_position_z(node, -2.0f);
    
    /* Set rotation (Euler angles in radians) */
    lv_gltf_model_node_set_rotation_x(node, 0.0f);
    lv_gltf_model_node_set_rotation_y(node, 1.57f);  /* 90 degrees */
    lv_gltf_model_node_set_rotation_z(node, 0.0f);
    
    /* Set scale */
    lv_gltf_model_node_set_scale_x(node, 2.0f);
    lv_gltf_model_node_set_scale_y(node, 2.0f);
    lv_gltf_model_node_set_scale_z(node, 2.0f);


Reading Node Properties
~~~~~~~~~~~~~~~~~~~~~~~

For performance reasons, the node properties are only calculated during rendering. 
In order to read them, you must first register an event callback.

This event callback will be called every time the node properties change.

Property values can only be read from within the ``LV_EVENT_VALUE_CHANGED``
callback to ensure data validity.

.. code-block:: c

    static void node_value_cb(lv_event_t * e)
    {
        lv_3dpoint_t position, rotation, scale;
        
        /* Read current values - only valid inside this callback */
        lv_gltf_model_node_get_local_position(e, &position);
        lv_gltf_model_node_get_euler_rotation(e, &rotation);
        lv_gltf_model_node_get_scale(e, &scale);
        
        LV_LOG_USER("Local Position: %.2f, %.2f, %.2f\n", 
                    position.x, position.y, position.z);
    }
    
    /* Register the callback */
    lv_gltf_model_node_add_event_cb(node, node_value_cb, 
                                    LV_EVENT_VALUE_CHANGED, NULL);


If you need world-space coordinates (position in global scene coordinates rather than relative to parent),
use the world position variant. This involves complex matrix calculations during the rendering phase which 
will impact performance:

.. code-block:: c

    static void node_world_value_cb(lv_event_t * e)
    {
        lv_3dpoint_t local_pos, world_pos;
        /* Both local and world positions are available */
        lv_gltf_model_node_get_local_position(e, &local_pos);
        lv_gltf_model_node_get_world_position(e, &world_pos);
        
        LV_LOG_USER("Local: %.2f, %.2f, %.2f\n", 
                    local_pos.x, local_pos.y, local_pos.z);
        LV_LOG_USER("World: %.2f, %.2f, %.2f\n",
                    world_pos.x, world_pos.y, world_pos.z);
    }
    /* Register with world position calculation enabled */
    lv_gltf_model_node_add_event_cb_with_world_position(node, node_world_value_cb,
                                                        LV_EVENT_VALUE_CHANGED, NULL);


Note: World position registration incurs additional computational cost due to matrix transformations. Only use it when you specifically need global coordinates.

.. important::

    - The ``LV_EVENT_VALUE_CHANGED`` event fires only when node properties are modified, not every frame 
    - Getter functions return ``LV_RESULT_INVALID`` if called outside the event callback, ensuring you never read stale data
    - Calling :cpp:func:`lv_gltf_model_node_get_world_position` without world position registration will return ``LV_RESULT_INVALID``

.. tip::
   When using the root node, the local position will equal the world position, meaning you can subscribe to 
   :cpp:func:`lv_gltf_model_node_add_event_cb` instead of :cpp:func:`lv_gltf_model_node_add_event_cb_with_world_position`
   and use :cpp:func:`lv_gltf_model_node_get_local_position` to get the world position without the computational 
   overhead of matrix calculations.

Finding Nodes
-------------

Access nodes by index, numeric path or path:

.. code-block::c

    /* Get by index */
    lv_gltf_model_node_t * root = lv_gltf_model_node_get_by_index(model, 0);
    lv_gltf_model_node_t * last_node = lv_gltf_model_node_get_by_index(model, lv_gltf_model_get_node_count(model) - 1); 
    
    /* Get by path (hierarchical node names from your 3D editor) */
    lv_gltf_model_node_t * cursor = lv_gltf_model_node_get_by_path(model, "/cursor");
    lv_gltf_model_node_t * arm = lv_gltf_model_node_get_by_path(model, "/body/arm");

    /* Get by index path (useful for unnamed nodes) */
    lv_gltf_model_node_t * node = lv_gltf_model_node_get_by_numeric_path(model, ".0.1");


Image-Based Lighting (IBL)
---------------------------

IBL (Image-Based Lighting) uses 360° panoramic images to light and shade 3D models, creating realistic reflections and environmental effects.
Unlike traditional mathematical lighting (where light sources are calculated with approximation functions),
IBL captures the entire lighting environment in an image, producing natural-looking results with:

- **Realistic reflections** on shiny surfaces that mirror the environment
- **Ambient lighting** from all directions with proper color tinting
- **Subtle lighting effects** like under-lighting and color variation based on viewing angle
- **Visual cohesiveness** that makes models appear naturally placed in their environment

For example, using an High-Dynamic-Range (HDR) image of a forest will make your 3D model appear to be sitting in a forest.

Default Behavior
~~~~~~~~~~~~~~~~

By default, each glTF viewer automatically creates an environment using an embedded default image with a cube map resolution of 128.
This provides good visual quality for most use cases without any setup.

Custom Environments
~~~~~~~~~~~~~~~~~~~

For more control over lighting quality, to use custom HDR images, or to share environments across multiple viewers, you can create and manage environments manually:

.. code-block:: c

    /* Create an IBL sampler with custom resolution */
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();

    /* Set a custom cube map resolution. Higher resolution produces sharper results at a cost of longer setup time */
    lv_gltf_ibl_sampler_set_cube_map_pixel_resolution(256);
    
    /* Create environment from custom HDR/JPEG image (or NULL for default) */
    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, "/path/to/environment.hdr");
    
    /* Sampler can be deleted after environment creation */
    lv_gltf_ibl_sampler_delete(sampler);
    
    /* Apply environment to viewer */
    lv_gltf_set_environment(gltf, env);
    
    /* Optionally rotate the environment lighting */
    lv_gltf_environment_set_angle(env, 45.0f);


Sharing Environments
~~~~~~~~~~~~~~~~~~~~

The same environment can be shared across multiple glTF viewers for consistent lighting and reduced memory usage:

.. code-block:: c

    lv_obj_t * gltf1 = lv_gltf_create(lv_screen_active());
    lv_obj_t * gltf2 = lv_gltf_create(lv_screen_active());
    
    /* Create shared environment */
    lv_gltf_ibl_sampler_t * sampler = lv_gltf_ibl_sampler_create();
    lv_gltf_environment_t * env = lv_gltf_environment_create(sampler, NULL);
    lv_gltf_ibl_sampler_delete(sampler);
    
    /* Apply to multiple viewers */
    /* Note that the user owns the environment and is responsible for deleting it when
    all glTF objects are deleted */
    lv_gltf_set_environment(gltf1, env);
    lv_gltf_set_environment(gltf2, env);


Environment Images
~~~~~~~~~~~~~~~~~~

- Use equirectangular (360°) panoramic images in HDR or JPEG format
- Source images are converted to cube map format at the sampler's cube map resolution
- Higher resolution values (256-512) provide better quality but use more memory
- Lower resolution values (64-128) are more suitable for embedded systems
- Free HDR environment maps are widely available online. Choose environments that match the "look" you want (outdoor, studio, warehouse, etc.)


How It Works
~~~~~~~~~~~~

When an environment is created, the source image is processed into a cube map format (6 images representing each face of a cube).
During rendering, for each pixel of your 3D model, the renderer samples the appropriate point from this cube map data to determine lighting color and intensity.
The processing cost is paid once during environment creation.


Performance Notes
~~~~~~~~~~~~~~~~~

- Environment creation processes the source image into cube map data, which takes more time with larger resolution values
- Processing time increases with both source image resolution and cube map resolution


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

.. include:: /examples/libs/gltf/index.rst



.. _gltf_api:

API
***

.. API startswith:  lv_gltf_
