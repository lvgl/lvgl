.. _lv_3dtexture:

=========================
3D Texture (lv_3dtexture)
=========================

Overview
********

3D texture widgets are used to embed an external 3D graphics library's "texture" primitive
into an LVGL UI.


.. _lv_3dtexture_parts_and_styles:

Parts and Styles
****************

The size should be set to the actual size of the texture primitive.
The opacity may also be changed.


.. _lv_3dtexture_usage:

Usage
*****

Requires a draw unit to be enabled which can draw the
:cpp:enumerator:`LV_DRAW_TASK_TYPE_3D` draw task type.

OpenGL is the first supported 3D graphics back-end. The following must be enabled.

- :c:macro:`LV_USE_3DTEXTURE`
- :c:macro:`LV_USE_OPENGLES`
- :c:macro:`LV_USE_DRAW_OPENGLES`

See :ref:`LVGL's OpenGLES driver docs <opengl_driver>` to create a window and a
display texture.

The `lv_example_3dtexture <https://github.com/lvgl/lv_example_3dtexture>` repository is a
demonstration of how to use the ``3dtexture`` widget to load glTF models with OpenGL
and display them in LVGL.

.. code-block:: c

    lv_obj_t * tex = lv_3dtexture_create(parent);
    /* Render something to the texture. You can replace it with your code. */
    lv_3dtexture_id_t gltf_texture = render_gltf_model_to_opengl_texture(path, w, h, color);
    lv_3dtexture_set_src(tex, gltf_texture);
    lv_obj_set_size(tex, w, h);
    lv_obj_set_style_opa(tex, opa, 0);

The real type of :cpp:type:`lv_3dtexture_id_t` depends on the active 3D graphics back-end.
With OpenGL, the type is ``unsigned int``.


.. _lv_3dtexture_events:

Events
******

3D Texture has no special event handling.


.. _lv_3dtexture_example:

Example
*******

See the `lv_example_3dtexture <https://github.com/lvgl/lv_example_3dtexture>`__ repo.


.. _lv_3dtexture_api:

API
***
