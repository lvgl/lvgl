#include "lv_opengles_standard_shader.h"

#if LV_USE_OPENGLES

#include "../../../stdlib/lv_sprintf.h"
#include <string.h>

static const lv_opengl_shader_t src_includes[] = {
    {
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
    },
    {
        "brightness_adjust.glsl", R"(
        uniform float u_Brightness; // add/subtract in [ -1.0 .. +1.0 ], 0.0 = no change

        vec3 adjustBrightness(vec3 color){
            return clamp(color + vec3(u_Brightness), 0.0, 1.0);
        }

    )"
    },
    {
        "contrast_adjust.glsl", R"(
        uniform float u_Contrast; // 0.0 = mid-gray, 1.0 = no change, >1.0 increases contrast

        vec3 adjustContrast(vec3 color){
            // shift to [-0.5..0.5], scale, shift back
            return clamp(((color - 0.5) * u_Contrast) + 0.5, 0.0, 1.0);
        }
    )"
    },
};

static const char * src_vertex_shader = R"(
    precision mediump float;
    
    in vec4 position;
    in vec2 texCoord;
    
    out vec2 v_TexCoord;
    
    uniform mat3 u_VertexTransform;
    
    void main()
    {
        gl_Position = vec4((u_VertexTransform * vec3(position.xy, 1)).xy, position.zw);
        v_TexCoord = texCoord;
    }
)";

static const char *src_fragment_shader = R"(
    precision lowp float;
    
    out vec4 color;
    
    in vec2 v_TexCoord;
    
    uniform sampler2D u_Texture;
    uniform float u_ColorDepth;
    uniform float u_Opa;
    uniform bool u_IsFill;
    uniform vec3 u_FillColor;
    
    #ifdef HSV_ADJUST
#include <hsv_adjust.glsl>
    #endif

    void main()
    {
        vec4 texColor;
        if (u_IsFill) {
            texColor = vec4(u_FillColor, 1.0);
        } else {
            //texColor = texture(u_Texture, v_TexCoord);
            texColor = textureLod(u_Texture, v_TexCoord, 0.0);  // If the vertices have been transformed, and mipmaps have not been generated, some rotation angles (notably 90 and 270) require using textureLod() to mitigate derivative calculation errors from increments flipping direction
        }
        if (abs(u_ColorDepth - 8.0) < 0.1) {
            float gray = texColor.r;
            color = vec4(gray, gray, gray, u_Opa);
        } else {
            float combinedAlpha = texColor.a * u_Opa;
            color = vec4(texColor.rgb * combinedAlpha, combinedAlpha);
        }
        #ifdef HSV_ADJUST
        color.rgb = adjustHSV(color.rgb);
        #endif
    }
)";

static const size_t src_includes_count = sizeof src_includes / sizeof src_includes[0];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

char* lv_opengles_standard_shader_get_vertex(void) {
    return lv_opengl_shader_manager_process_includes(src_vertex_shader, GLSL_VERSION_PREFIX, src_includes, src_includes_count );
}

char* lv_opengles_standard_shader_get_fragment(void) {
    return lv_opengl_shader_manager_process_includes(src_fragment_shader, GLSL_VERSION_PREFIX, src_includes, src_includes_count);
}

void lv_opengles_standard_shader_get_src(lv_opengl_shader_portions_t *portions)
{
    portions->all = src_includes;
    portions->count = sizeof(src_includes) / sizeof(src_includes[0]);
}

#endif /*LV_USE_OPENGLES*/
