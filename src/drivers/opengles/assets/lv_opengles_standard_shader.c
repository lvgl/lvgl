#include "lv_opengles_standard_shader.h"

#if LV_USE_OPENGLES

#include "../../../stdlib/lv_sprintf.h"
#include <string.h>

static const lv_opengl_shader_t src_includes[] = {
	{ "hsv_adjust.glsl", R"(
        
        uniform float u_Hue;
        uniform float u_Saturation;
        uniform float u_Value;

        vec3 rgb2hsv(vec3 c){
            float M = max(max(c.r,c.g),c.b);
            float m = min(min(c.r,c.g),c.b);
            float d = M - m;
            float h = 0.0;
            if(d > 0.00001){
                if(M == c.r) h = mod((c.g - c.b)/d, 6.0);
                else if(M == c.g) h = (c.b - c.r)/d + 2.0;
                else h = (c.r - c.g)/d + 4.0;
                h /= 6.0;
            }
            float s = (M <= 0.00001) ? 0.0 : d / M;
            return vec3(h, s, M);
        }

        vec3 hsv2rgb(vec3 c){
            float h = c.x * 6.0;
            float i = floor(h);
            float f = h - i;
            float p = c.z * (1.0 - c.y);
            float q = c.z * (1.0 - c.y * f);
            float t = c.z * (1.0 - c.y * (1.0 - f));
            if(i == 0.0) return vec3(c.z, t, p);
            if(i == 1.0) return vec3(q, c.z, p);
            if(i == 2.0) return vec3(p, c.z, t);
            if(i == 3.0) return vec3(p, q, c.z);
            if(i == 4.0) return vec3(t, p, c.z);
            return vec3(c.z, p, q);
        }

        vec3 adjustHSV(vec3 color){
            vec3 hsv = rgb2hsv(color);
            hsv.x = fract(hsv.x + u_Hue);
            hsv.y = clamp(hsv.y * u_Saturation, 0.0, 1.0);
            hsv.z = clamp(hsv.z * u_Value, 0.0, 1.0);
            return hsv2rgb(hsv);
        }
    )" },
	{ "brightness_adjust.glsl", R"(
        uniform float u_Brightness; // add/subtract in [ -1.0 .. +1.0 ], 0.0 = no change

        vec3 adjustBrightness(vec3 color){
            return clamp(color + vec3(u_Brightness), 0.0, 1.0);
        }

    )" },
	{ "contrast_adjust.glsl", R"(
        uniform float u_Contrast; // 0.0 = mid-gray, 1.0 = no change, >1.0 increases contrast

        vec3 adjustContrast(vec3 color){
            // shift to [-0.5..0.5], scale, shift back
            return clamp(((color - 0.5) * u_Contrast) + 0.5, 0.0, 1.0);
        }
    )" },
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
    
    void main()
    {
        vec4 texColor;
        if (u_IsFill) {
            texColor = vec4(u_FillColor, 1.0);
        } else {
            texColor = texture(u_Texture, v_TexCoord);
        }
        if (abs(u_ColorDepth - 8.0) < 0.1) {
            float gray = texColor.r;
            color = vec4(gray, gray, gray, u_Opa);
        } else {
            float combinedAlpha = texColor.a * u_Opa;
            color = vec4(texColor.rgb * combinedAlpha, combinedAlpha);
        }
    }
)";

const size_t src_includes_count = sizeof src_includes / sizeof src_includes[0];

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
