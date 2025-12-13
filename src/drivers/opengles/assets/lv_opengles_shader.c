#include "lv_opengles_shader.h"

#if LV_USE_OPENGLES

#include "../opengl_shader/lv_opengl_shader_internal.h"
#include "../../../misc/lv_types.h"

static const lv_opengl_shader_t src_includes_v100[] = {{
        "hsv_adjust.glsl", R"(
        
        uniform float u_Hue;
        uniform float u_Saturation;
        uniform float u_Value;

        // Convert RGB to HSV
        vec3 rgb2hsv(vec3 c) {
            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;
            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }

        // Convert HSV to RGB
        vec3 hsv2rgb(vec3 c) {
            vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
            vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
            return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
        }

        vec3 adjustHSV(vec3 color){
            vec3 hsv = rgb2hsv(color);
            hsv.x = fract(hsv.x + u_Hue);
            hsv.y = clamp(hsv.y * u_Saturation, 0.0, 1.0);
            hsv.z = clamp(hsv.z * u_Value, 0.0, 1.0);
            return hsv2rgb(hsv);
        }
    )"
    }, {
        "brightness_adjust.glsl", R"(
        uniform float u_Brightness; // add/subtract in [ -1.0 .. +1.0 ], 0.0 = no change

        vec3 adjustBrightness(vec3 color){
            return clamp(color + vec3(u_Brightness), 0.0, 1.0);
        }

    )"
    }, {
        "contrast_adjust.glsl", R"(
        uniform float u_Contrast; // 0.0 = mid-gray, 1.0 = no change, >1.0 increases contrast

        vec3 adjustContrast(vec3 color){
            // shift to [-0.5..0.5], scale, shift back
            return clamp(((color - 0.5) * u_Contrast) + 0.5, 0.0, 1.0);
        }
    )"
    },
};

static const char * src_vertex_shader_v100 = R"(
    precision mediump float;
    
    attribute vec4 position;
    attribute vec2 texCoord;
    
    varying vec2 v_TexCoord;
    
    uniform mat3 u_VertexTransform;
    
    void main()
    {
        gl_Position = vec4((u_VertexTransform * vec3(position.xy, 1.0)).xy, position.zw);
        v_TexCoord = texCoord;
    }
)";

static const char *src_fragment_shader_v100 = R"(
    precision lowp float;
    
    varying vec2 v_TexCoord;
    
    uniform sampler2D u_Texture;
    uniform float u_ColorDepth;
    uniform float u_Opa;
    uniform bool u_IsFill;
    uniform vec3 u_FillColor;
    uniform bool u_SwapRB;
    
    #ifdef HSV_ADJUST
#include <hsv_adjust.glsl>
    #endif
    
    void main()
    {
        vec4 texColor;
        if (u_IsFill) {
            texColor = vec4(u_FillColor, 1.0);
        } else {
            texColor = texture2D(u_Texture, v_TexCoord);
        }
        if (abs(u_ColorDepth - 8.0) < 0.1) {
            float gray = texColor.r;
            gl_FragColor = vec4(vec3(gray * u_Opa), u_Opa);
        } else {
            float combinedAlpha = texColor.a * u_Opa;
            gl_FragColor = vec4(texColor.rgb * combinedAlpha, combinedAlpha);
        }
        if (u_SwapRB) {
            gl_FragColor.bgr = gl_FragColor.rgb;
        }
        #ifdef HSV_ADJUST
        gl_FragColor.rgb = adjustHSV(gl_FragColor.rgb);
        #endif
    }
)";

static const lv_opengl_shader_t src_includes_v300es[] = {{
        "hsv_adjust.glsl", R"(
        uniform float u_Hue;
        uniform float u_Saturation;
        uniform float u_Value;

        // Convert RGB to HSV
        vec3 rgb2hsv(vec3 c) {
            vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
            vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
            vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
            float d = q.x - min(q.w, q.y);
            float e = 1.0e-10;
            return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
        }

        // Convert HSV to RGB
        vec3 hsv2rgb(vec3 c) {
            vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
            vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
            return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
        }

        vec3 adjustHSV(vec3 color){
            vec3 hsv = rgb2hsv(color);
            hsv.x = fract(hsv.x + u_Hue);
            hsv.y = clamp(hsv.y * u_Saturation, 0.0, 1.0);
            hsv.z = clamp(hsv.z * u_Value, 0.0, 1.0);
            return hsv2rgb(hsv);
        }
    )"
    }, {
        "brightness_adjust.glsl", R"(
        uniform float u_Brightness; // add/subtract in [ -1.0 .. +1.0 ], 0.0 = no change

        vec3 adjustBrightness(vec3 color){
            return clamp(color + vec3(u_Brightness), 0.0, 1.0);
        }

    )"
    }, {
        "contrast_adjust.glsl", R"(
        uniform float u_Contrast; // 0.0 = mid-gray, 1.0 = no change, >1.0 increases contrast

        vec3 adjustContrast(vec3 color){
            // shift to [-0.5..0.5], scale, shift back
            return clamp(((color - 0.5) * u_Contrast) + 0.5, 0.0, 1.0);
        }
    )"
    },
};

static const char * src_vertex_shader_v300es = R"(
    precision mediump float;
    
    in vec4 position;
    in vec2 texCoord;
    
    out vec2 v_TexCoord;
    flat out lowp vec4 fill_color_alpha;
    flat out lowp int is_gray;

    uniform lowp float u_Opa;
    uniform bool u_IsFill;
    uniform vec3 u_FillColor;
    uniform mat3 u_VertexTransform;
    uniform float u_ColorDepth;
    
    void main()
    {
        gl_Position = vec4((u_VertexTransform * vec3(position.xy, 1)).xy, position.zw);
        v_TexCoord = texCoord;
        is_gray = (abs(u_ColorDepth - 8.0) < 0.1) ? 1 : 0;

        if (u_IsFill) {
            if (is_gray == 1) {
                fill_color_alpha = vec4(u_FillColor.rrr, 1.0) * u_Opa;
            } else {
                fill_color_alpha = vec4((u_FillColor.rgb * u_Opa), u_Opa);
            }
        } else {
            fill_color_alpha = vec4(0.0, 0.0, 0.0, -1.0);
        }
    }
)";

static const char *src_fragment_shader_v300es = R"(
    precision lowp float;
    
    out vec4 color;
    
    in vec2 v_TexCoord;
    flat in lowp vec4 fill_color_alpha;
    flat in lowp int is_gray;
    
    uniform sampler2D u_Texture;
    uniform lowp float u_Opa;
    uniform bool u_SwapRB;
    
    #ifdef HSV_ADJUST
#include <hsv_adjust.glsl>
    #endif

    void main()
    {
        if (fill_color_alpha.a != -1.0) {
            color = fill_color_alpha;
        } else {
            color = texture(u_Texture, v_TexCoord);
            /* If the vertices have been transformed, and mipmaps have not been generated, 
             * some rotation angles (notably 90 and 270) require using textureLod() to mitigate 
             * derivative calculation errors from interpolator increments flipping direction.
             * color = textureLod(u_Texture, v_TexCoord, u_LodLevel);
             */
            if (is_gray != 0) {
                color.r *= u_Opa;
                color.gba = vec3(color.rr, u_Opa);
            } else {
                color.a *= u_Opa;
                color.rgb *= color.a;
            }
        }
        if (u_SwapRB) {
            color.bgr = color.rgb;
        }
        #ifdef HSV_ADJUST
        color.rgb = adjustHSV(color.rgb);
        #endif
    }
)";

static const size_t src_includes_v100_count = sizeof src_includes_v100 / sizeof src_includes_v100[0];
static const size_t src_includes_v300es_count = sizeof src_includes_v300es / sizeof src_includes_v300es[0];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

char * lv_opengles_shader_get_vertex(lv_opengl_glsl_version version) {
    switch (version){
        case LV_OPENGL_GLSL_VERSION_300ES:
            return lv_opengl_shader_manager_process_includes(src_vertex_shader_v300es, src_includes_v300es, src_includes_v300es_count);
        case LV_OPENGL_GLSL_VERSION_100:
            return lv_opengl_shader_manager_process_includes(src_vertex_shader_v100, src_includes_v100, src_includes_v100_count);
        case LV_OPENGL_GLSL_VERSION_LAST:
            LV_LOG_ERROR("Invalid glsl version %d", version);
            return NULL;
    }
    LV_UNREACHABLE();
}

char * lv_opengles_shader_get_fragment(lv_opengl_glsl_version version) {
    switch (version){
        case LV_OPENGL_GLSL_VERSION_300ES:
            return lv_opengl_shader_manager_process_includes(src_fragment_shader_v300es, src_includes_v300es, src_includes_v300es_count);
        case LV_OPENGL_GLSL_VERSION_100:
            return lv_opengl_shader_manager_process_includes(src_fragment_shader_v100, src_includes_v100, src_includes_v100_count);
        case LV_OPENGL_GLSL_VERSION_LAST:
            LV_LOG_ERROR("Invalid glsl version %d", version);
            return NULL;
    }
    LV_UNREACHABLE();
}

void lv_opengles_shader_get_source(lv_opengl_shader_portions_t *portions, lv_opengl_glsl_version version)
{
    switch (version){
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
