#include "lv_opengles_shader.h"

#if LV_USE_OPENGLES

#include "../opengl_shader/lv_opengl_shader_internal.h"

static const lv_opengl_shader_t src_includes_v100[] = {{
        "hsv_adjust.glsl", "\n"
        "        \n"
        "        uniform float u_Hue;\n"
        "        uniform float u_Saturation;\n"
        "        uniform float u_Value;\n"
        "\n"
        "        // Convert RGB to HSV\n"
        "        vec3 rgb2hsv(vec3 c) {\n"
        "            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
        "            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
        "            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"
        "            float d = q.x - min(q.w, q.y);\n"
        "            float e = 1.0e-10;\n"
        "            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
        "        }\n"
        "\n"
        "        // Convert HSV to RGB\n"
        "        vec3 hsv2rgb(vec3 c) {\n"
        "            vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
        "            vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
        "            return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
        "        }\n"
        "\n"
        "        vec3 adjustHSV(vec3 color){\n"
        "            vec3 hsv = rgb2hsv(color);\n"
        "            hsv.x = fract(hsv.x + u_Hue);\n"
        "            hsv.y = clamp(hsv.y * u_Saturation, 0.0, 1.0);\n"
        "            hsv.z = clamp(hsv.z * u_Value, 0.0, 1.0);\n"
        "            return hsv2rgb(hsv);\n"
        "        }\n"
        "    "
    }, {
        "brightness_adjust.glsl", "\n"
        "        uniform float u_Brightness; // add/subtract in [ -1.0 .. +1.0 ], 0.0 = no change\n"
        "\n"
        "        vec3 adjustBrightness(vec3 color){\n"
        "            return clamp(color + vec3(u_Brightness), 0.0, 1.0);\n"
        "        }\n"
        "\n"
        "    "
    }, {
        "contrast_adjust.glsl", "\n"
        "        uniform float u_Contrast; // 0.0 = mid-gray, 1.0 = no change, >1.0 increases contrast\n"
        "\n"
        "        vec3 adjustContrast(vec3 color){\n"
        "            // shift to [-0.5..0.5], scale, shift back\n"
        "            return clamp(((color - 0.5) * u_Contrast) + 0.5, 0.0, 1.0);\n"
        "        }\n"
        "    "
    },
};

static const char * src_vertex_shader_v100 = "\n"
                                             "    precision mediump float;\n"
                                             "    \n"
                                             "    attribute vec4 position;\n"
                                             "    attribute vec2 texCoord;\n"
                                             "    \n"
                                             "    varying vec2 v_TexCoord;\n"
                                             "    \n"
                                             "    uniform mat3 u_VertexTransform;\n"
                                             "    \n"
                                             "    void main()\n"
                                             "    {\n"
                                             "        gl_Position = vec4((u_VertexTransform * vec3(position.xy, 1.0)).xy, position.zw);\n"
                                             "        v_TexCoord = texCoord;\n"
                                             "    }\n";

static const char * src_fragment_shader_v100 = "\n"
                                               "    precision lowp float;\n"
                                               "    \n"
                                               "    varying vec2 v_TexCoord;\n"
                                               "    \n"
                                               "    uniform sampler2D u_Texture;\n"
                                               "    uniform float u_ColorDepth;\n"
                                               "    uniform float u_Opa;\n"
                                               "    uniform bool u_IsFill;\n"
                                               "    uniform vec3 u_FillColor;\n"
                                               "    uniform bool u_SwapRB;\n"
                                               "    uniform bool u_PremultipliedSrc;\n"
                                               "    \n"
                                               "    #ifdef HSV_ADJUST\n"
                                               "#include <hsv_adjust.glsl>\n"
                                               "    #endif\n"
                                               "    \n"
                                               "    void main()\n"
                                               "    {\n"
                                               "        vec4 texColor;\n"
                                               "        if (u_IsFill) {\n"
                                               "            texColor = vec4(u_FillColor, 1.0);\n"
                                               "        } else {\n"
                                               "            texColor = texture2D(u_Texture, v_TexCoord);\n"
                                               "        }\n"
                                               "        if (abs(u_ColorDepth - 8.0) < 0.1) {\n"
                                               "            float gray = texColor.r;\n"
                                               "            gl_FragColor = vec4(vec3(gray * u_Opa), u_Opa);\n"
                                               "        } else {\n"
                                               "            float combinedAlpha = texColor.a * u_Opa;\n"
                                               "            float rgbScale = u_PremultipliedSrc ? u_Opa : combinedAlpha;\n"
                                               "            gl_FragColor = vec4(texColor.rgb * rgbScale, combinedAlpha);\n"
                                               "        }\n"
                                               "        if (u_SwapRB) {\n"
                                               "            gl_FragColor.bgr = gl_FragColor.rgb;\n"
                                               "        }\n"
                                               "        #ifdef HSV_ADJUST\n"
                                               "        gl_FragColor.rgb = adjustHSV(gl_FragColor.rgb);\n"
                                               "        #endif\n"
                                               "    }\n";

static const lv_opengl_shader_t src_includes_v300es[] = {{
        "hsv_adjust.glsl", "\n"
        "        uniform float u_Hue;\n"
        "        uniform float u_Saturation;\n"
        "        uniform float u_Value;\n"
        "\n"
        "        // Convert RGB to HSV\n"
        "        vec3 rgb2hsv(vec3 c) {\n"
        "            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);\n"
        "            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));\n"
        "            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));\n"
        "            float d = q.x - min(q.w, q.y);\n"
        "            float e = 1.0e-10;\n"
        "            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);\n"
        "        }\n"
        "\n"
        "        // Convert HSV to RGB\n"
        "        vec3 hsv2rgb(vec3 c) {\n"
        "            vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);\n"
        "            vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);\n"
        "            return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);\n"
        "        }\n"
        "\n"
        "        vec3 adjustHSV(vec3 color){\n"
        "            vec3 hsv = rgb2hsv(color);\n"
        "            hsv.x = fract(hsv.x + u_Hue);\n"
        "            hsv.y = clamp(hsv.y * u_Saturation, 0.0, 1.0);\n"
        "            hsv.z = clamp(hsv.z * u_Value, 0.0, 1.0);\n"
        "            return hsv2rgb(hsv);\n"
        "        }\n"
        "    "
    }, {
        "brightness_adjust.glsl", "\n"
        "        uniform float u_Brightness; // add/subtract in [ -1.0 .. +1.0 ], 0.0 = no change\n"
        "\n"
        "        vec3 adjustBrightness(vec3 color){\n"
        "            return clamp(color + vec3(u_Brightness), 0.0, 1.0);\n"
        "        }\n"
        "\n"
        "    "
    }, {
        "contrast_adjust.glsl", "\n"
        "        uniform float u_Contrast; // 0.0 = mid-gray, 1.0 = no change, >1.0 increases contrast\n"
        "\n"
        "        vec3 adjustContrast(vec3 color){\n"
        "            // shift to [-0.5..0.5], scale, shift back\n"
        "            return clamp(((color - 0.5) * u_Contrast) + 0.5, 0.0, 1.0);\n"
        "        }\n"
        "    "
    },
};

static const char * src_vertex_shader_v300es = "\n"
                                               "    precision mediump float;\n"
                                               "    \n"
                                               "    in vec4 position;\n"
                                               "    in vec2 texCoord;\n"
                                               "    \n"
                                               "    out vec2 v_TexCoord;\n"
                                               "    flat out lowp vec4 fill_color_alpha;\n"
                                               "    flat out lowp int is_gray;\n"
                                               "\n"
                                               "    uniform lowp float u_Opa;\n"
                                               "    uniform bool u_IsFill;\n"
                                               "    uniform vec3 u_FillColor;\n"
                                               "    uniform mat3 u_VertexTransform;\n"
                                               "    uniform float u_ColorDepth;\n"
                                               "    \n"
                                               "    void main()\n"
                                               "    {\n"
                                               "        gl_Position = vec4((u_VertexTransform * vec3(position.xy, 1)).xy, position.zw);\n"
                                               "        v_TexCoord = texCoord;\n"
                                               "        is_gray = (abs(u_ColorDepth - 8.0) < 0.1) ? 1 : 0;\n"
                                               "\n"
                                               "        if (u_IsFill) {\n"
                                               "            if (is_gray == 1) {\n"
                                               "                fill_color_alpha = vec4(u_FillColor.rrr, 1.0) * u_Opa;\n"
                                               "            } else {\n"
                                               "                fill_color_alpha = vec4((u_FillColor.rgb * u_Opa), u_Opa);\n"
                                               "            }\n"
                                               "        } else {\n"
                                               "            fill_color_alpha = vec4(0.0, 0.0, 0.0, -1.0);\n"
                                               "        }\n"
                                               "    }\n";

static const char * src_fragment_shader_v300es = "\n"
                                                 "    precision lowp float;\n"
                                                 "    \n"
                                                 "    out vec4 color;\n"
                                                 "    \n"
                                                 "    in vec2 v_TexCoord;\n"
                                                 "    flat in lowp vec4 fill_color_alpha;\n"
                                                 "    flat in lowp int is_gray;\n"
                                                 "    \n"
                                                 "    uniform sampler2D u_Texture;\n"
                                                 "    uniform lowp float u_Opa;\n"
                                                 "    uniform bool u_SwapRB;\n"
                                                 "    uniform bool u_PremultipliedSrc;\n"
                                                 "    \n"
                                                 "    #ifdef HSV_ADJUST\n"
                                                 "#include <hsv_adjust.glsl>\n"
                                                 "    #endif\n"
                                                 "\n"
                                                 "    void main()\n"
                                                 "    {\n"
                                                 "        if (fill_color_alpha.a != -1.0) {\n"
                                                 "            color = fill_color_alpha;\n"
                                                 "        } else {\n"
                                                 "            color = texture(u_Texture, v_TexCoord);\n"
                                                 "            /* If the vertices have been transformed, and mipmaps have not been generated, \n"
                                                 "             * some rotation angles (notably 90 and 270) require using textureLod() to mitigate \n"
                                                 "             * derivative calculation errors from interpolator increments flipping direction.\n"
                                                 "             * color = textureLod(u_Texture, v_TexCoord, u_LodLevel);\n"
                                                 "             */\n"
                                                 "            if (is_gray != 0) {\n"
                                                 "                color.r *= u_Opa;\n"
                                                 "                color.gba = vec3(color.rr, u_Opa);\n"
                                                 "            } else {\n"
                                                 "                color.a *= u_Opa;\n"
                                                 "                color.rgb *= u_PremultipliedSrc ? u_Opa : color.a;\n"
                                                 "            }\n"
                                                 "        }\n"
                                                 "        if (u_SwapRB) {\n"
                                                 "            color.bgr = color.rgb;\n"
                                                 "        }\n"
                                                 "        #ifdef HSV_ADJUST\n"
                                                 "        color.rgb = adjustHSV(color.rgb);\n"
                                                 "        #endif\n"
                                                 "    }\n";

static const size_t src_includes_v100_count = sizeof src_includes_v100 / sizeof src_includes_v100[0];
static const size_t src_includes_v300es_count = sizeof src_includes_v300es / sizeof src_includes_v300es[0];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

char * lv_opengles_shader_get_vertex(lv_opengl_glsl_version_t version)
{
    switch(version) {
        case LV_OPENGL_GLSL_VERSION_330:
        case LV_OPENGL_GLSL_VERSION_300ES:
            return lv_opengl_shader_manager_process_includes(src_vertex_shader_v300es, src_includes_v300es,
                                                             src_includes_v300es_count);
        case LV_OPENGL_GLSL_VERSION_100:
            return lv_opengl_shader_manager_process_includes(src_vertex_shader_v100, src_includes_v100, src_includes_v100_count);
        case LV_OPENGL_GLSL_VERSION_LAST:
            LV_LOG_ERROR("Invalid glsl version %d", version);
            return NULL;
    }
    LV_UNREACHABLE();
}

char * lv_opengles_shader_get_fragment(lv_opengl_glsl_version_t version)
{
    switch(version) {
        case LV_OPENGL_GLSL_VERSION_330:
        case LV_OPENGL_GLSL_VERSION_300ES:
            return lv_opengl_shader_manager_process_includes(src_fragment_shader_v300es, src_includes_v300es,
                                                             src_includes_v300es_count);
        case LV_OPENGL_GLSL_VERSION_100:
            return lv_opengl_shader_manager_process_includes(src_fragment_shader_v100, src_includes_v100, src_includes_v100_count);
        case LV_OPENGL_GLSL_VERSION_LAST:
            LV_LOG_ERROR("Invalid glsl version %d", version);
            return NULL;
    }
    LV_UNREACHABLE();
}

void lv_opengles_shader_get_source(lv_opengl_shader_portions_t * portions, lv_opengl_glsl_version_t version)
{
    switch(version) {
        case LV_OPENGL_GLSL_VERSION_330:
        case LV_OPENGL_GLSL_VERSION_300ES:
            portions->all = src_includes_v300es;
            portions->count = src_includes_v300es_count;
            return;
        case LV_OPENGL_GLSL_VERSION_100:
            portions->all = src_includes_v100;
            portions->count = src_includes_v100_count;
            return;
        case LV_OPENGL_GLSL_VERSION_LAST:
            LV_LOG_ERROR("Invalid glsl version %d", version);
            portions->count = 0;
            return;
    }

    LV_UNREACHABLE();
}

#endif /*LV_USE_OPENGLES*/
