#include "lv_gltf_view_shader.h"

#if LV_USE_GLTF

#include "../../../../stdlib/lv_sprintf.h"
#include <string.h>

static const lv_opengl_shader_t src_includes[] = {
    {
        "tonemapping.glsl", R"(

        uniform float u_Exposure;


        const float STANDARD_GAMMA = 2.2;
        const float GAMMA = STANDARD_GAMMA;
        const float INV_GAMMA = 1.0 / GAMMA;


        // sRGB => XYZ => D65_2_D60 => AP1 => RRT_SAT
        const mat3 ACESInputMat = mat3
        (
            0.59719, 0.07600, 0.02840,
            0.35458, 0.90834, 0.13383,
            0.04823, 0.01566, 0.83777
        );


        // ODT_SAT => XYZ => D60_2_D65 => sRGB
        const mat3 ACESOutputMat = mat3
        (
            1.60475, -0.10208, -0.00327,
            -0.53108,  1.10813, -0.07276,
            -0.07367, -0.00605,  1.07602
        );


        // linear to sRGB approximation
        // see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
        vec3 linearTosRGB(vec3 color)
        {
            return pow(color, vec3(INV_GAMMA));
        }


        // sRGB to linear approximation
        // see http://chilliant.blogspot.com/2012/08/srgb-approximations-for-hlsl.html
        vec3 sRGBToLinear(vec3 srgbIn)
        {
            return vec3(pow(srgbIn.xyz, vec3(GAMMA)));
        }


        vec4 sRGBToLinear(vec4 srgbIn)
        {
            return vec4(sRGBToLinear(srgbIn.xyz), srgbIn.w);
        }


        // ACES tone map (faster approximation)
        // see: https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
        vec3 toneMapACES_Narkowicz(vec3 color)
        {
            const float A = 2.51;
            const float B = 0.03;
            const float C = 2.43;
            const float D = 0.59;
            const float E = 0.14;
            return clamp((color * (A * color + B)) / (color * (C * color + D) + E), 0.0, 1.0);
        }


        // ACES filmic tone map approximation
        // see https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
        vec3 RRTAndODTFit(vec3 color)
        {
            vec3 a = color * (color + 0.0245786) - 0.000090537;
            vec3 b = color * (0.983729 * color + 0.4329510) + 0.238081;
            return a / b;
        }


        // tone mapping
        vec3 toneMapACES_Hill(vec3 color)
        {
            color = ACESInputMat * color;

            // Apply RRT and ODT
            color = RRTAndODTFit(color);

            color = ACESOutputMat * color;

            // Clamp to [0, 1]
            color = clamp(color, 0.0, 1.0);

            return color;
        }

        // Khronos PBR neutral tone mapping
        #ifdef TONEMAP_KHR_PBR_NEUTRAL
        vec3 toneMap_KhronosPbrNeutral( vec3 color )
        {
            const float startCompression = 0.8 - 0.04;
            const float desaturation = 0.15;

            float x = min(color.r, min(color.g, color.b));
            float offset = x < 0.08 ? x - 6.25 * x * x : 0.04;
            color -= offset;

            float peak = max(color.r, max(color.g, color.b));
            if (peak < startCompression) return color;

            const float d = 1. - startCompression;
            float newPeak = 1. - d * d / (peak + d - startCompression);
            color *= newPeak / peak;

            float g = 1. - 1. / (desaturation * (peak - newPeak) + 1.);
            return mix(color, newPeak * vec3(1, 1, 1), g);
        }
        #endif

        vec3 toneMap(vec3 color)
        {
            color *= u_Exposure;

        #ifdef TONEMAP_ACES_NARKOWICZ
            color = toneMapACES_Narkowicz(color);
        #endif

        #ifdef TONEMAP_ACES_HILL
            color = toneMapACES_Hill(color);
        #endif

        #ifdef TONEMAP_ACES_HILL_EXPOSURE_BOOST
            // boost exposure as discussed in https://github.com/mrdoob/three.js/pull/19621
            // this factor is based on the exposure correction of Krzysztof Narkowicz in his
            // implemetation of ACES tone mapping
            color /= 0.6;
            color = toneMapACES_Hill(color);
        #endif

        #ifdef TONEMAP_KHR_PBR_NEUTRAL
            color = toneMap_KhronosPbrNeutral(color);
        #endif

            return linearTosRGB(color);
        }

    )"
    },
    {
        "textures1.glsl", R"(

        // IBL

        uniform int u_MipCount;
        uniform samplerCube u_LambertianEnvSampler;
        uniform samplerCube u_GGXEnvSampler;
        uniform sampler2D u_GGXLUT;
        uniform samplerCube u_CharlieEnvSampler;
        uniform sampler2D u_CharlieLUT;
        uniform sampler2D u_SheenELUT;
        uniform mat3 u_EnvRotation;


        // General Material


        uniform sampler2D u_NormalSampler;
        uniform float u_NormalScale;
        uniform int u_NormalUVSet;
        uniform mat3 u_NormalUVTransform;

        uniform vec3 u_EmissiveFactor;
        uniform sampler2D u_EmissiveSampler;
        uniform int u_EmissiveUVSet;
        uniform mat3 u_EmissiveUVTransform;

        uniform sampler2D u_OcclusionSampler;
        uniform int u_OcclusionUVSet;
        uniform float u_OcclusionStrength;
        uniform mat3 u_OcclusionUVTransform;


        in vec2 v_texcoord_0;
        in vec2 v_texcoord_1;


        vec2 getNormalUV()
        {
            vec3 uv = vec3(u_NormalUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

        #ifdef HAS_NORMAL_UV_TRANSFORM
            uv = u_NormalUVTransform * uv;
        #endif

            return uv.xy;
        }


        vec2 getEmissiveUV()
        {
            vec3 uv = vec3(u_EmissiveUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

        #ifdef HAS_EMISSIVE_UV_TRANSFORM
            uv = u_EmissiveUVTransform * uv;
        #endif

            return uv.xy;
        }


        vec2 getOcclusionUV()
        {
            vec3 uv = vec3(u_OcclusionUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

        #ifdef HAS_OCCLUSION_UV_TRANSFORM
            uv = u_OcclusionUVTransform * uv;
        #endif

            return uv.xy;
        }


        // MK TEMP - Added special optimized handling for unlit materials
        #ifdef MATERIAL_UNLIT
            uniform sampler2D u_BaseColorSampler;
            uniform int u_BaseColorUVSet;
            uniform mat3 u_BaseColorUVTransform;
            
            vec2 getBaseColorUV()
            {
                vec3 uv = vec3(u_BaseColorUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

            #ifdef HAS_BASECOLOR_UV_TRANSFORM
                uv = u_BaseColorUVTransform * uv;
            #endif

                return uv.xy;
            }
        #else
            // Metallic Roughness Material
            #ifdef MATERIAL_METALLICROUGHNESS

            uniform sampler2D u_BaseColorSampler;
            uniform int u_BaseColorUVSet;
            uniform mat3 u_BaseColorUVTransform;

            uniform sampler2D u_MetallicRoughnessSampler;
            uniform int u_MetallicRoughnessUVSet;
            uniform mat3 u_MetallicRoughnessUVTransform;

            vec2 getBaseColorUV()
            {
                vec3 uv = vec3(u_BaseColorUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

            #ifdef HAS_BASECOLOR_UV_TRANSFORM
                uv = u_BaseColorUVTransform * uv;
            #endif

                return uv.xy;
            }

            vec2 getMetallicRoughnessUV()
            {
                vec3 uv = vec3(u_MetallicRoughnessUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

            #ifdef HAS_METALLICROUGHNESS_UV_TRANSFORM
                uv = u_MetallicRoughnessUVTransform * uv;
            #endif

                return uv.xy;
            }

            #endif
        #endif

    )"
    },
    {
        "textures2.glsl", R"(
        // Specular Glossiness Material


        #ifdef MATERIAL_SPECULARGLOSSINESS

        uniform sampler2D u_DiffuseSampler;
        uniform int u_DiffuseUVSet;
        uniform mat3 u_DiffuseUVTransform;

        uniform sampler2D u_SpecularGlossinessSampler;
        uniform int u_SpecularGlossinessUVSet;
        uniform mat3 u_SpecularGlossinessUVTransform;


        vec2 getSpecularGlossinessUV()
        {
            vec3 uv = vec3(u_SpecularGlossinessUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

        #ifdef HAS_SPECULARGLOSSINESS_UV_TRANSFORM
            uv = u_SpecularGlossinessUVTransform * uv;
        #endif

            return uv.xy;
        }

        vec2 getDiffuseUV()
        {
            vec3 uv = vec3(u_DiffuseUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);

        #ifdef HAS_DIFFUSE_UV_TRANSFORM
            uv = u_DiffuseUVTransform * uv;
        #endif

            return uv.xy;
        }

        #endif


        // Clearcoat Material


        #ifdef MATERIAL_CLEARCOAT

        uniform sampler2D u_ClearcoatSampler;
        uniform int u_ClearcoatUVSet;
        uniform mat3 u_ClearcoatUVTransform;

        uniform sampler2D u_ClearcoatRoughnessSampler;
        uniform int u_ClearcoatRoughnessUVSet;
        uniform mat3 u_ClearcoatRoughnessUVTransform;

        uniform sampler2D u_ClearcoatNormalSampler;
        uniform int u_ClearcoatNormalUVSet;
        uniform mat3 u_ClearcoatNormalUVTransform;
        uniform float u_ClearcoatNormalScale;


        vec2 getClearcoatUV()
        {
            vec3 uv = vec3(u_ClearcoatUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_CLEARCOAT_UV_TRANSFORM
            uv = u_ClearcoatUVTransform * uv;
        #endif
            return uv.xy;
        }

        vec2 getClearcoatRoughnessUV()
        {
            vec3 uv = vec3(u_ClearcoatRoughnessUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_CLEARCOATROUGHNESS_UV_TRANSFORM
            uv = u_ClearcoatRoughnessUVTransform * uv;
        #endif
            return uv.xy;
        }

        vec2 getClearcoatNormalUV()
        {
            vec3 uv = vec3(u_ClearcoatNormalUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_CLEARCOATNORMAL_UV_TRANSFORM
            uv = u_ClearcoatNormalUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif


        // Sheen Material


        #ifdef MATERIAL_SHEEN

        uniform sampler2D u_SheenColorSampler;
        uniform int u_SheenColorUVSet;
        uniform mat3 u_SheenColorUVTransform;
        uniform sampler2D u_SheenRoughnessSampler;
        uniform int u_SheenRoughnessUVSet;
        uniform mat3 u_SheenRoughnessUVTransform;

    )"
    },
    {
        "textures3.glsl", R"(
        vec2 getSheenColorUV()
        {
            vec3 uv = vec3(u_SheenColorUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_SHEENCOLOR_UV_TRANSFORM
            uv = u_SheenColorUVTransform * uv;
        #endif
            return uv.xy;
        }

        vec2 getSheenRoughnessUV()
        {
            vec3 uv = vec3(u_SheenRoughnessUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_SHEENROUGHNESS_UV_TRANSFORM
            uv = u_SheenRoughnessUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif


        // Specular Material


        #ifdef MATERIAL_SPECULAR

        uniform sampler2D u_SpecularSampler;
        uniform int u_SpecularUVSet;
        uniform mat3 u_SpecularUVTransform;
        uniform sampler2D u_SpecularColorSampler;
        uniform int u_SpecularColorUVSet;
        uniform mat3 u_SpecularColorUVTransform;


        vec2 getSpecularUV()
        {
            vec3 uv = vec3(u_SpecularUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_SPECULAR_UV_TRANSFORM
            uv = u_SpecularUVTransform * uv;
        #endif
            return uv.xy;
        }

        vec2 getSpecularColorUV()
        {
            vec3 uv = vec3(u_SpecularColorUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_SPECULARCOLOR_UV_TRANSFORM
            uv = u_SpecularColorUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif


        // Transmission Material


        #ifdef MATERIAL_TRANSMISSION

        uniform sampler2D u_TransmissionSampler;
        uniform int u_TransmissionUVSet;
        uniform mat3 u_TransmissionUVTransform;
        uniform sampler2D u_TransmissionFramebufferSampler;
        uniform ivec2 u_TransmissionFramebufferSize;


        vec2 getTransmissionUV()
        {
            vec3 uv = vec3(u_TransmissionUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_TRANSMISSION_UV_TRANSFORM
            uv = u_TransmissionUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif


        // Volume Material


        #ifdef MATERIAL_VOLUME

        uniform sampler2D u_ThicknessSampler;
        uniform int u_ThicknessUVSet;
        uniform mat3 u_ThicknessUVTransform;


    )"
    },
    {
        "textures4.glsl", R"(
        vec2 getThicknessUV()
        {
            vec3 uv = vec3(u_ThicknessUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_THICKNESS_UV_TRANSFORM
            uv = u_ThicknessUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif


        // Iridescence


        #ifdef MATERIAL_IRIDESCENCE

        uniform sampler2D u_IridescenceSampler;
        uniform int u_IridescenceUVSet;
        uniform mat3 u_IridescenceUVTransform;

        uniform sampler2D u_IridescenceThicknessSampler;
        uniform int u_IridescenceThicknessUVSet;
        uniform mat3 u_IridescenceThicknessUVTransform;


        vec2 getIridescenceUV()
        {
            vec3 uv = vec3(u_IridescenceUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_IRIDESCENCE_UV_TRANSFORM
            uv = u_IridescenceUVTransform * uv;
        #endif
            return uv.xy;
        }

        vec2 getIridescenceThicknessUV()
        {
            vec3 uv = vec3(u_IridescenceThicknessUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_IRIDESCENCETHICKNESS_UV_TRANSFORM
            uv = u_IridescenceThicknessUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif


        // Diffuse Transmission

        #ifdef MATERIAL_DIFFUSE_TRANSMISSION

        uniform sampler2D u_DiffuseTransmissionSampler;
        uniform int u_DiffuseTransmissionUVSet;
        uniform mat3 u_DiffuseTransmissionUVTransform;

        uniform sampler2D u_DiffuseTransmissionColorSampler;
        uniform int u_DiffuseTransmissionColorUVSet;
        uniform mat3 u_DiffuseTransmissionColorUVTransform;


        vec2 getDiffuseTransmissionUV()
        {
            vec3 uv = vec3(u_DiffuseTransmissionUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_DIFFUSETRANSMISSION_UV_TRANSFORM
            uv = u_DiffuseTransmissionUVTransform * uv;
        #endif
            return uv.xy;
        }

        vec2 getDiffuseTransmissionColorUV()
        {
            vec3 uv = vec3(u_DiffuseTransmissionColorUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_DIFFUSETRANSMISSIONCOLOR_UV_TRANSFORM
            uv = u_DiffuseTransmissionColorUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif

        // Anisotropy

        #ifdef MATERIAL_ANISOTROPY

        uniform sampler2D u_AnisotropySampler;
        uniform int u_AnisotropyUVSet;
        uniform mat3 u_AnisotropyUVTransform;

        vec2 getAnisotropyUV()
        {
            vec3 uv = vec3(u_AnisotropyUVSet < 1 ? v_texcoord_0 : v_texcoord_1, 1.0);
        #ifdef HAS_ANISOTROPY_UV_TRANSFORM
            uv = u_AnisotropyUVTransform * uv;
        #endif
            return uv.xy;
        }

        #endif

    )"
    },
    {
        "functions.glsl", R"(

        const float M_PI = 3.141592653589793;


        in vec3 v_Position;


        #ifdef HAS_NORMAL_VEC3
        #ifdef HAS_TANGENT_VEC4
        in mat3 v_TBN;
        #else
        in vec3 v_Normal;
        #endif
        #endif


        #ifdef HAS_COLOR_0_VEC3
        in vec3 v_Color;
        #endif
        #ifdef HAS_COLOR_0_VEC4
        in vec4 v_Color;
        #endif


        vec4 getVertexColor()
        {
        vec4 color = vec4(1.0);

        #ifdef HAS_COLOR_0_VEC3
            color.rgb = v_Color.rgb;
        #endif
        #ifdef HAS_COLOR_0_VEC4
            color = v_Color;
        #endif

        return color;
        }


        struct NormalInfo {
            vec3 ng;   // Geometry normal
            vec3 t;    // Geometry tangent
            vec3 b;    // Geometry bitangent
            vec3 n;    // Shading normal
            vec3 ntex; // Normal from texture, scaling is accounted for.
        };


        float clampedDot(vec3 x, vec3 y)
        {
            return clamp(dot(x, y), 0.0, 1.0);
        }


        float max3(vec3 v)
        {
            return max(max(v.x, v.y), v.z);
        }


        float sq(float t)
        {
            return t * t;
        }

        vec2 sq(vec2 t)
        {
            return t * t;
        }

        vec3 sq(vec3 t)
        {
            return t * t;
        }

        vec4 sq(vec4 t)
        {
            return t * t;
        }


        float applyIorToRoughness(float roughness, float ior)
        {
            // Scale roughness with IOR so that an IOR of 1.0 results in no microfacet refraction and
            // an IOR of 1.5 results in the default amount of microfacet refraction.
            return roughness * clamp(ior * 2.0 - 2.0, 0.0, 1.0);
        }

        vec3 rgb_mix(vec3 base, vec3 layer, vec3 rgb_alpha)
        {
            float rgb_alpha_max = max(rgb_alpha.r, max(rgb_alpha.g, rgb_alpha.b));
            return (1.0 - rgb_alpha_max) * base + rgb_alpha * layer;
        }


    )"
    },
    {
        "brdf1.glsl", R"(
        //
        // Fresnel
        //
        // http://graphicrants.blogspot.com/2013/08/specular-brdf-reference.html
        // https://github.com/wdas/brdf/tree/master/src/brdfs
        // https://google.github.io/filament/Filament.md.html
        //

        // The following equation models the Fresnel reflectance term of the spec equation (aka F())
        // Implementation of fresnel from [4], Equation 15
        vec3 F_Schlick(vec3 f0, vec3 f90, float VdotH) 
        {
            return f0 + (f90 - f0) * pow(clamp(1.0 - VdotH, 0.0, 1.0), 5.0);
        }

        float F_Schlick(float f0, float f90, float VdotH)
        {
            float x = clamp(1.0 - VdotH, 0.0, 1.0);
            float x2 = x * x;
            float x5 = x * x2 * x2;
            return f0 + (f90 - f0) * x5;
        }

        float F_Schlick(float f0, float VdotH)
        {
            float f90 = 1.0; //clamp(50.0 * f0, 0.0, 1.0);
            return F_Schlick(f0, f90, VdotH);
        }

        vec3 F_Schlick(vec3 f0, float f90, float VdotH)
        {
            float x = clamp(1.0 - VdotH, 0.0, 1.0);
            float x2 = x * x;
            float x5 = x * x2 * x2;
            return f0 + (f90 - f0) * x5;
        }

        vec3 F_Schlick(vec3 f0, float VdotH)
        {
            float f90 = 1.0; //clamp(dot(f0, vec3(50.0 * 0.33)), 0.0, 1.0);
            return F_Schlick(f0, f90, VdotH);
        }

        vec3 Schlick_to_F0(vec3 f, vec3 f90, float VdotH) {
            float x = clamp(1.0 - VdotH, 0.0, 1.0);
            float x2 = x * x;
            float x5 = clamp(x * x2 * x2, 0.0, 0.9999);

            return (f - f90 * x5) / (1.0 - x5);
        }

        float Schlick_to_F0(float f, float f90, float VdotH) {
            float x = clamp(1.0 - VdotH, 0.0, 1.0);
            float x2 = x * x;
            float x5 = clamp(x * x2 * x2, 0.0, 0.9999);

            return (f - f90 * x5) / (1.0 - x5);
        }

        vec3 Schlick_to_F0(vec3 f, float VdotH) {
            return Schlick_to_F0(f, vec3(1.0), VdotH);
        }

        float Schlick_to_F0(float f, float VdotH) {
            return Schlick_to_F0(f, 1.0, VdotH);
        }

    )"
    },
    {
        "brdf2.glsl", R"(
        // Smith Joint GGX
        // Note: Vis = G / (4 * NdotL * NdotV)
        // see Eric Heitz. 2014. Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs. Journal of Computer Graphics Techniques, 3
        // see Real-Time Rendering. Page 331 to 336.
        // see https://google.github.io/filament/Filament.md.html#materialsystem/specularbrdf/geometricshadowing(specularg)
        float V_GGX(float NdotL, float NdotV, float alphaRoughness)
        {
            float alphaRoughnessSq = alphaRoughness * alphaRoughness;

            float GGXV = NdotL * sqrt(NdotV * NdotV * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);
            float GGXL = NdotV * sqrt(NdotL * NdotL * (1.0 - alphaRoughnessSq) + alphaRoughnessSq);

            float GGX = GGXV + GGXL;
            if (GGX > 0.0)
            {
                return 0.5 / GGX;
            }
            return 0.0;
        }


        // The following equation(s) model the distribution of microfacet normals across the area being drawn (aka D())
        // Implementation from "Average Irregularity Representation of a Roughened Surface for Ray Reflection" by T. S. Trowbridge, and K. P. Reitz
        // Follows the distribution function recommended in the SIGGRAPH 2013 course notes from EPIC Games [1], Equation 3.
        float D_GGX(float NdotH, float alphaRoughness)
        {
            float alphaRoughnessSq = alphaRoughness * alphaRoughness;
            float f = (NdotH * NdotH) * (alphaRoughnessSq - 1.0) + 1.0;
            return alphaRoughnessSq / (M_PI * f * f);
        }


        float lambdaSheenNumericHelper(float x, float alphaG)
        {
            float oneMinusAlphaSq = (1.0 - alphaG) * (1.0 - alphaG);
            float a = mix(21.5473, 25.3245, oneMinusAlphaSq);
            float b = mix(3.82987, 3.32435, oneMinusAlphaSq);
            float c = mix(0.19823, 0.16801, oneMinusAlphaSq);
            float d = mix(-1.97760, -1.27393, oneMinusAlphaSq);
            float e = mix(-4.32054, -4.85967, oneMinusAlphaSq);
            return a / (1.0 + b * pow(x, c)) + d * x + e;
        }


        float lambdaSheen(float cosTheta, float alphaG)
        {
            if (abs(cosTheta) < 0.5)
            {
                return exp(lambdaSheenNumericHelper(cosTheta, alphaG));
            }
            else
            {
                return exp(2.0 * lambdaSheenNumericHelper(0.5, alphaG) - lambdaSheenNumericHelper(1.0 - cosTheta, alphaG));
            }
        }


        float V_Sheen(float NdotL, float NdotV, float sheenRoughness)
        {
            sheenRoughness = max(sheenRoughness, 0.000001); //clamp (0,1]
            float alphaG = sheenRoughness * sheenRoughness;

            return clamp(1.0 / ((1.0 + lambdaSheen(NdotV, alphaG) + lambdaSheen(NdotL, alphaG)) *
                (4.0 * NdotV * NdotL)), 0.0, 1.0);
        }


        //Sheen implementation-------------------------------------------------------------------------------------
        // See  https://github.com/sebavan/glTF/tree/KHR_materials_sheen/extensions/2.0/Khronos/KHR_materials_sheen

        // Estevez and Kulla http://www.aconty.com/pdf/s2017_pbs_imageworks_sheen.pdf
        float D_Charlie(float sheenRoughness, float NdotH)
        {
            sheenRoughness = max(sheenRoughness, 0.000001); //clamp (0,1]
            float alphaG = sheenRoughness * sheenRoughness;
            float invR = 1.0 / alphaG;
            float cos2h = NdotH * NdotH;
            float sin2h = 1.0 - cos2h;
            return (2.0 + invR) * pow(sin2h, invR * 0.5) / (2.0 * M_PI);
        }
    )"
    },
    {
        "brdf3.glsl", R"(

        //https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
        vec3 BRDF_lambertian(vec3 diffuseColor)
        {
            // see https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/
            return (diffuseColor / M_PI);
        }

        //  https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
        vec3 BRDF_specularGGX(float alphaRoughness, float NdotL, float NdotV, float NdotH)
        {
            float Vis = V_GGX(NdotL, NdotV, alphaRoughness);
            float D = D_GGX(NdotH, alphaRoughness);

            return vec3(Vis * D);
        }


        #ifdef MATERIAL_ANISOTROPY
        // GGX Distribution Anisotropic (Same as Babylon.js)
        // https://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf Addenda
        float D_GGX_anisotropic(float NdotH, float TdotH, float BdotH, float anisotropy, float at, float ab)
        {
            float a2 = at * ab;
            vec3 f = vec3(ab * TdotH, at * BdotH, a2 * NdotH);
            float w2 = a2 / dot(f, f);
            return a2 * w2 * w2 / M_PI;
        }

        // GGX Mask/Shadowing Anisotropic (Same as Babylon.js - smithVisibility_GGXCorrelated_Anisotropic)
        // Heitz http://jcgt.org/published/0003/02/03/paper.pdf
        float V_GGX_anisotropic(float NdotL, float NdotV, float BdotV, float TdotV, float TdotL, float BdotL, float at, float ab)
        {
            float GGXV = NdotL * length(vec3(at * TdotV, ab * BdotV, NdotV));
            float GGXL = NdotV * length(vec3(at * TdotL, ab * BdotL, NdotL));
            float v = 0.5 / (GGXV + GGXL);
            return clamp(v, 0.0, 1.0);
        }

        vec3 BRDF_specularGGXAnisotropy(float alphaRoughness, float anisotropy, vec3 n, vec3 v, vec3 l, vec3 h, vec3 t, vec3 b)
        {
            // Roughness along the anisotropy bitangent is the material roughness, while the tangent roughness increases with anisotropy.
            float at = mix(alphaRoughness, 1.0, anisotropy * anisotropy);
            float ab = clamp(alphaRoughness, 0.001, 1.0);

            float NdotL = clamp(dot(n, l), 0.0, 1.0);
            float NdotH = clamp(dot(n, h), 0.001, 1.0);
            float NdotV = dot(n, v);

            float V = V_GGX_anisotropic(NdotL, NdotV, dot(b, v), dot(t, v), dot(t, l), dot(b, l), at, ab);
            float D = D_GGX_anisotropic(NdotH, dot(t, h), dot(b, h), anisotropy, at, ab);

            return vec3(V * D);
        }
        #endif


        // f_sheen
        vec3 BRDF_specularSheen(vec3 sheenColor, float sheenRoughness, float NdotL, float NdotV, float NdotH)
        {
            float sheenDistribution = D_Charlie(sheenRoughness, NdotH);
            float sheenVisibility = V_Sheen(NdotL, NdotV, sheenRoughness);
            return sheenColor * sheenDistribution * sheenVisibility;
        }
    )"
    },
    {
        "punctual1.glsl", R"(
        struct Light
        {
            vec3 direction;
            float range;

            vec3 color;
            float intensity;

            vec3 position;
            float innerConeCos;

            float outerConeCos;
            int type;
        };

        const int LightType_Directional = 0;
        const int LightType_Point = 1;
        const int LightType_Spot = 2;


        #ifdef USE_PUNCTUAL
        //Light u_Lights[LIGHT_COUNT + 1]; //Array [0] is not allowed
        uniform Light u_Lights[LIGHT_COUNT + 1]; //Array [0] is not allowed
        #endif

        float getRangeAttenuation(float range, float distance)
        {
            if (range <= 0.0)
            {
                // negative range means unlimited
                return 1.0 / pow(distance, 2.0);
            }
            return max(min(1.0 - pow(distance / range, 4.0), 1.0), 0.0) / pow(distance, 2.0);
        }
        float getSpotAttenuation(vec3 pointToLight, vec3 spotDirection, float outerConeCos, float innerConeCos)
        {
            float actualCos = dot(normalize(spotDirection), normalize(-pointToLight));
            if (actualCos > outerConeCos)
            {
                if (actualCos < innerConeCos)
                {
                    float angularAttenuation = (actualCos - outerConeCos) / (innerConeCos - outerConeCos);
                    return angularAttenuation * angularAttenuation;
                }
                return 1.0;
            }
            return 0.0;
        }
        vec3 getLighIntensity(Light light, vec3 pointToLight)
        {
            float rangeAttenuation = 1.0;
            float spotAttenuation = 1.0;

            if (light.type != LightType_Directional)
            {
                rangeAttenuation = getRangeAttenuation(light.range, length(pointToLight));
            }
            if (light.type == LightType_Spot)
            {
                spotAttenuation = getSpotAttenuation(pointToLight, light.direction, light.outerConeCos, light.innerConeCos);
            }

            return rangeAttenuation * spotAttenuation * light.intensity * light.color;
        }

    )"
    },
    {
        "punctual2.glsl", R"(
        vec3 getPunctualRadianceTransmission(vec3 normal, vec3 view, vec3 pointToLight, float alphaRoughness,
            vec3 baseColor, float ior)
        {
            float transmissionRougness = applyIorToRoughness(alphaRoughness, ior);

            vec3 n = normalize(normal);
            vec3 v = normalize(view);
            vec3 l = normalize(pointToLight);
            vec3 l_mirror = normalize(l + 2.0*n*dot(-l, n));
            vec3 h = normalize(l_mirror + v);

            float D = D_GGX(clamp(dot(n, h), 0.0, 1.0), transmissionRougness);
            float Vis = V_GGX(clamp(dot(n, l_mirror), 0.0, 1.0), clamp(dot(n, v), 0.0, 1.0), transmissionRougness);

            // Transmission BTDF
            return baseColor * D * Vis;
        }


        vec3 getPunctualRadianceClearCoat(vec3 clearcoatNormal, vec3 v, vec3 l, vec3 h, float VdotH, vec3 f0, vec3 f90, float clearcoatRoughness)
        {
            float NdotL = clampedDot(clearcoatNormal, l);
            float NdotV = clampedDot(clearcoatNormal, v);
            float NdotH = clampedDot(clearcoatNormal, h);
            return NdotL * BRDF_specularGGX(clearcoatRoughness * clearcoatRoughness, NdotL, NdotV, NdotH);
        }


        vec3 getPunctualRadianceSheen(vec3 sheenColor, float sheenRoughness, float NdotL, float NdotV, float NdotH)
        {
            return NdotL * BRDF_specularSheen(sheenColor, sheenRoughness, NdotL, NdotV, NdotH);
        }


        vec3 applyVolumeAttenuation(vec3 radiance, float transmissionDistance, vec3 attenuationColor, float attenuationDistance)
        {
            if (attenuationDistance == 0.0)
            {
                // Attenuation distance is +∞ (which we indicate by zero), i.e. the transmitted color is not attenuated at all.
                return radiance;
            }
            else
            {
                vec3 transmittance = pow(attenuationColor, vec3(transmissionDistance / attenuationDistance));
                return transmittance * radiance;
            }
        }


        vec3 getVolumeTransmissionRay(vec3 n, vec3 v, float thickness, float ior, mat4 modelMatrix)
        {
            vec3 refractionVector = refract(-v, normalize(n), 1.0 / ior);
            vec3 modelScale;
            modelScale.x = length(vec3(modelMatrix[0].xyz));
            modelScale.y = length(vec3(modelMatrix[1].xyz));
            modelScale.z = length(vec3(modelMatrix[2].xyz));
            return normalize(refractionVector) * thickness * modelScale;
        }

    )"
    },
    {
        "ibl1.glsl", R"(

        uniform float u_EnvIntensity;

        vec3 getDiffuseLight(vec3 n)
        {
            // MK TEMP (for some reason, this one still seems to have positive effect - check for inverted Y at IBL diffuse generation phase)
            // n.y *= -1.0;
            vec4 textureSample = texture(u_LambertianEnvSampler, u_EnvRotation * n);
            textureSample.rgb *= u_EnvIntensity;
            return textureSample.rgb;
        }

        vec4 getSpecularSample(vec3 reflection, float lod)
        {
            vec4 textureSample = textureLod(u_GGXEnvSampler, u_EnvRotation * reflection, lod);
            textureSample.rgb *= u_EnvIntensity;
            return textureSample;
        }

        vec4 getSheenSample(vec3 reflection, float lod)
        {
            vec4 textureSample =  textureLod(u_CharlieEnvSampler, u_EnvRotation * reflection, lod);
            textureSample.rgb *= u_EnvIntensity;
            return textureSample;
        }

        vec3 getIBLGGXFresnel(vec3 n, vec3 v, float roughness, vec3 F0, float specularWeight)
        {
            // see https://bruop.github.io/ibl/#single_scattering_results at Single Scattering Results
            // Roughness dependent fresnel, from Fdez-Aguera
            float NdotV = clampedDot(n, v);
            vec2 brdfSamplePoint = clamp(vec2(NdotV, roughness), vec2(0.0, 0.0), vec2(1.0, 1.0));
            vec2 f_ab = texture(u_GGXLUT, brdfSamplePoint).rg;
            vec3 Fr = max(vec3(1.0 - roughness), F0) - F0;
            vec3 k_S = F0 + Fr * pow(1.0 - NdotV, 5.0);
            vec3 FssEss = specularWeight * (k_S * f_ab.x + f_ab.y);

            // Multiple scattering, from Fdez-Aguera
            float Ems = (1.0 - (f_ab.x + f_ab.y));
            vec3 F_avg = specularWeight * (F0 + (1.0 - F0) / 21.0);
            vec3 FmsEms = Ems * FssEss * F_avg / (1.0 - F_avg * Ems);

            return FssEss + FmsEms;
        }

        vec3 getIBLRadianceGGX(vec3 n, vec3 v, float roughness)
        {
            float NdotV = clampedDot(n, v);
            float lod = roughness * float(u_MipCount - 1);

            vec3 reflection = normalize(reflect(-v, n));
            vec4 specularSample = getSpecularSample(reflection, lod);

            vec3 specularLight = specularSample.rgb;

            return specularLight;
        }


        #ifdef MATERIAL_TRANSMISSION
        vec3 getTransmissionSample(vec2 fragCoord, float roughness, float ior)
        {
            float framebufferLod = log2(float(u_TransmissionFramebufferSize.x)) * applyIorToRoughness(roughness, ior);
            vec3 transmittedLight = textureLod(u_TransmissionFramebufferSampler, fragCoord.xy, framebufferLod).rgb;

            return transmittedLight;
        }
        #endif

        #ifdef MATERIAL_TRANSMISSION
        vec3 getIBLVolumeRefraction(vec3 n, vec3 v, float perceptualRoughness, vec3 baseColor, vec3 position, mat4 modelMatrix,
            mat4 viewMatrix, mat4 projMatrix, float ior, float thickness, vec3 attenuationColor, float attenuationDistance, float dispersion)
        {
    )"
    },
    {
        "ibl2.glsl", R"(

        #ifdef MATERIAL_DISPERSION
            // Dispersion will spread out the ior values for each r,g,b channel
            float halfSpread = (ior - 1.0) * 0.025 * dispersion;
            vec3 iors = vec3(ior - halfSpread, ior, ior + halfSpread);

            vec3 transmittedLight;
            float transmissionRayLength;
            for (int i = 0; i < 3; i++)
            {
                vec3 transmissionRay = getVolumeTransmissionRay(n, v, thickness, iors[i], modelMatrix);
                // TODO: taking length of blue ray, ideally we would take the length of the green ray. For now overwriting seems ok
                transmissionRayLength = length(transmissionRay);
                vec3 refractedRayExit = position + transmissionRay;

                // Project refracted vector on the framebuffer, while mapping to normalized device coordinates.
                vec4 ndcPos = projMatrix * viewMatrix * vec4(refractedRayExit, 1.0);
                vec2 refractionCoords = ndcPos.xy / ndcPos.w;
                refractionCoords += 1.0;
                refractionCoords /= 2.0;

                // Sample framebuffer to get pixel the refracted ray hits for this color channel.
                transmittedLight[i] = getTransmissionSample(refractionCoords, perceptualRoughness, iors[i])[i];
            }
        #else
            vec3 transmissionRay = getVolumeTransmissionRay(n, v, thickness, ior, modelMatrix);
            float transmissionRayLength = length(transmissionRay);
            vec3 refractedRayExit = position + transmissionRay;

            // Project refracted vector on the framebuffer, while mapping to normalized device coordinates.
            vec4 ndcPos = projMatrix * viewMatrix * vec4(refractedRayExit, 1.0);
            vec2 refractionCoords = ndcPos.xy / ndcPos.w;
            refractionCoords += 1.0;
            refractionCoords /= 2.0;

            // Sample framebuffer to get pixel the refracted ray hits.
            vec3 transmittedLight = getTransmissionSample(refractionCoords, perceptualRoughness, ior);

        #endif // MATERIAL_DISPERSION
            vec3 attenuatedColor = applyVolumeAttenuation(transmittedLight, transmissionRayLength, attenuationColor, attenuationDistance);

            return attenuatedColor * baseColor;
        }
        #endif


        #ifdef MATERIAL_ANISOTROPY
        vec3 getIBLRadianceAnisotropy(vec3 n, vec3 v, float roughness, float anisotropy, vec3 anisotropyDirection)
        {
            float NdotV = clampedDot(n, v);

            float tangentRoughness = mix(roughness, 1.0, anisotropy * anisotropy);
            vec3  anisotropicTangent  = cross(anisotropyDirection, v);
            vec3  anisotropicNormal   = cross(anisotropicTangent, anisotropyDirection);
            float bendFactor          = 1.0 - anisotropy * (1.0 - roughness);
            float bendFactorPow4      = bendFactor * bendFactor * bendFactor * bendFactor;
            vec3  bentNormal          = normalize(mix(anisotropicNormal, n, bendFactorPow4));

            float lod = roughness * float(u_MipCount - 1);
            vec3 reflection = normalize(reflect(-v, bentNormal));

            vec4 specularSample = getSpecularSample(reflection, lod);

            vec3 specularLight = specularSample.rgb;

            return specularLight;
        }
        #endif


        vec3 getIBLRadianceCharlie(vec3 n, vec3 v, float sheenRoughness, vec3 sheenColor)
        {
            float NdotV = clampedDot(n, v);
            float lod = sheenRoughness * float(u_MipCount - 1);
            vec3 reflection = normalize(reflect(-v, n));

            vec2 brdfSamplePoint = clamp(vec2(NdotV, sheenRoughness), vec2(0.0, 0.0), vec2(1.0, 1.0));
            float brdf = texture(u_CharlieLUT, brdfSamplePoint).b;
            vec4 sheenSample = getSheenSample(reflection, lod);

            vec3 sheenLight = sheenSample.rgb;
            return sheenLight * sheenColor * brdf;
        }
            
    )"
    },
    {
        "material_info1.glsl", R"(

        // Metallic Roughness
        uniform float u_MetallicFactor;
        uniform float u_RoughnessFactor;
        uniform vec4 u_BaseColorFactor;

        // Sheen
        uniform float u_SheenRoughnessFactor;
        uniform vec3 u_SheenColorFactor;

        // Clearcoat
        uniform float u_ClearcoatFactor;
        uniform float u_ClearcoatRoughnessFactor;

        // Specular
        uniform vec3 u_KHR_materials_specular_specularColorFactor;
        uniform float u_KHR_materials_specular_specularFactor;

        // Transmission
        uniform float u_TransmissionFactor;

        // Volume
        uniform float u_ThicknessFactor;
        uniform vec3 u_AttenuationColor;
        uniform float u_AttenuationDistance;

        // Iridescence
        uniform float u_IridescenceFactor;
        uniform float u_IridescenceIor;
        uniform float u_IridescenceThicknessMinimum;
        uniform float u_IridescenceThicknessMaximum;

        // Diffuse Transmission
        uniform float u_DiffuseTransmissionFactor;
        uniform vec3 u_DiffuseTransmissionColorFactor;

        // Emissive Strength
        uniform float u_EmissiveStrength;

        // IOR
        uniform float u_Ior;

        // Anisotropy
        uniform vec3 u_Anisotropy;

        // Dispersion
        uniform float u_Dispersion;

        // Alpha mode
        uniform float u_AlphaCutoff;

        uniform vec3 u_Camera;

        #ifdef MATERIAL_TRANSMISSION
        uniform ivec2 u_ScreenSize;
        #endif

        uniform mat4 u_ModelMatrix;
        uniform mat4 u_ViewMatrix;
        uniform mat4 u_ProjectionMatrix;


        struct MaterialInfo
        {
            vec4 baseColorFactor;
            float alphaCutoff;
	        int flags;
            vec2 padding;  // Above props temporary from earlier shader version -mk
            
                float occlusionStrength;
                float normalScale;
    
            float ior;
            float perceptualRoughness;      // roughness value, as authored by the model creator (input to shader)
            vec3 f0_dielectric;

            float alphaRoughness;           // roughness mapped to a more linear change in the roughness (proposed by [2])

            float fresnel_w;

            vec3 f90;                       // reflectance color at grazing angle
            vec3 f90_dielectric;
            float metallic;

            vec3 baseColor;

            float sheenRoughnessFactor;
            vec3 sheenColorFactor;

            vec3 clearcoatF0;
            vec3 clearcoatF90;
            float clearcoatFactor;
            vec3 clearcoatNormal;
            float clearcoatRoughness;

            // KHR_materials_specular 
            float specularWeight; // product of specularFactor and specularTexture.a

            float transmissionFactor;
    )"
    },
    {
        "material_info2.glsl", R"(
            float thickness;
            vec3 attenuationColor;
            float attenuationDistance;

            // KHR_materials_iridescence
            float iridescenceFactor;
            float iridescenceIor;
            float iridescenceThickness;

            float diffuseTransmissionFactor;
            vec3 diffuseTransmissionColorFactor;

            // KHR_materials_anisotropy
            vec3 anisotropicT;
            vec3 anisotropicB;
            float anisotropyStrength;

            // KHR_materials_dispersion
            float dispersion;
        };


        // Get normal, tangent and bitangent vectors.
        NormalInfo getNormalInfo(vec3 v)
        {
            vec2 UV = getNormalUV();
            vec2 uv_dx = dFdx(UV);
            vec2 uv_dy = dFdy(UV);

            if (length(uv_dx) <= 1e-2) {
            uv_dx = vec2(1.0, 0.0);
            }

            if (length(uv_dy) <= 1e-2) {
            uv_dy = vec2(0.0, 1.0);
            }

            vec3 t_ = (uv_dy.t * dFdx(v_Position) - uv_dx.t * dFdy(v_Position)) /
                (uv_dx.s * uv_dy.t - uv_dy.s * uv_dx.t);

            vec3 n, t, b, ng;

            // Compute geometrical TBN:
        #ifdef HAS_NORMAL_VEC3
        #ifdef HAS_TANGENT_VEC4
            // Trivial TBN computation, present as vertex attribute.
            // Normalize eigenvectors as matrix is linearly interpolated.
            t = normalize(v_TBN[0]);
            b = normalize(v_TBN[1]);
            ng = normalize(v_TBN[2]);
        #else
            // Normals are either present as vertex attributes or approximated.
            ng = normalize(v_Normal);
            t = normalize(t_ - ng * dot(ng, t_));
            b = cross(ng, t);
        #endif
        #else
            ng = normalize(cross(dFdx(v_Position), dFdy(v_Position)));
            t = normalize(t_ - ng * dot(ng, t_));
            b = cross(ng, t);
        #endif

        #ifndef NOT_TRIANGLE
            // For a back-facing surface, the tangential basis vectors are negated.
            if (gl_FrontFacing == false)
            {
                t *= -1.0;
                b *= -1.0;
                ng *= -1.0;
            }
        #endif

            // Compute normals:
            NormalInfo info;
            info.ng = ng;
        #ifdef HAS_NORMAL_MAP
            info.ntex = texture(u_NormalSampler, UV).rgb * 2.0 - vec3(1.0);
            info.ntex *= vec3(u_NormalScale, u_NormalScale, 1.0);
            info.ntex = normalize(info.ntex);
            info.n = normalize(mat3(t, b, ng) * info.ntex);
        #else
            info.n = ng;
        #endif
            info.t = t;
            info.b = b;
            return info;
        }


        #ifdef MATERIAL_CLEARCOAT
        vec3 getClearcoatNormal(NormalInfo normalInfo)
        {
        #ifdef HAS_CLEARCOAT_NORMAL_MAP
            vec3 n = texture(u_ClearcoatNormalSampler, getClearcoatNormalUV()).rgb * 2.0 - vec3(1.0);
            n *= vec3(u_ClearcoatNormalScale, u_ClearcoatNormalScale, 1.0);
            n = mat3(normalInfo.t, normalInfo.b, normalInfo.ng) * normalize(n);
            return n;
        #else
            return normalInfo.ng;
        #endif
        }
        #endif

    )"
    },
    {
        "material_info3.glsl", R"(
        vec4 getBaseColor()
        {
            vec4 baseColor = u_BaseColorFactor;

            #ifdef MATERIAL_UNLIT
                #if defined(HAS_BASE_COLOR_MAP)
                    baseColor *= texture(u_BaseColorSampler, getBaseColorUV());
                #endif
                return baseColor;
            #else
                #ifdef MATERIAL_METALLICROUGHNESS
                    #if defined(HAS_BASE_COLOR_MAP)
                        baseColor *= texture(u_BaseColorSampler, getBaseColorUV());
                    #endif
                #endif
                return baseColor * getVertexColor();
            #endif

        }


        #ifdef MATERIAL_METALLICROUGHNESS
        MaterialInfo getMetallicRoughnessInfo(MaterialInfo info)
        {
            info.metallic = u_MetallicFactor;
            info.perceptualRoughness = u_RoughnessFactor;

        #ifdef HAS_METALLIC_ROUGHNESS_MAP
            // Roughness is stored in the 'g' channel, metallic is stored in the 'b' channel.
            // This layout intentionally reserves the 'r' channel for (optional) occlusion map data
            vec4 mrSample = texture(u_MetallicRoughnessSampler, getMetallicRoughnessUV());
            info.perceptualRoughness *= mrSample.g;
            info.metallic *= mrSample.b;
        #endif

            return info;
        }
        #endif


        #ifdef MATERIAL_SHEEN
        MaterialInfo getSheenInfo(MaterialInfo info)
        {
            info.sheenColorFactor = u_SheenColorFactor;
            info.sheenRoughnessFactor = u_SheenRoughnessFactor;

        #ifdef HAS_SHEEN_COLOR_MAP
            vec4 sheenColorSample = texture(u_SheenColorSampler, getSheenColorUV());
            info.sheenColorFactor *= sheenColorSample.rgb;
        #endif

        #ifdef HAS_SHEEN_ROUGHNESS_MAP
            vec4 sheenRoughnessSample = texture(u_SheenRoughnessSampler, getSheenRoughnessUV());
            info.sheenRoughnessFactor *= sheenRoughnessSample.a;
        #endif
            return info;
        }
        #endif


        #ifdef MATERIAL_SPECULAR
        MaterialInfo getSpecularInfo(MaterialInfo info)
        {   
            vec4 specularTexture = vec4(1.0);
        #ifdef HAS_SPECULAR_MAP
            specularTexture.a = texture(u_SpecularSampler, getSpecularUV()).a;
        #endif
        #ifdef HAS_SPECULAR_COLOR_MAP
            specularTexture.rgb = texture(u_SpecularColorSampler, getSpecularColorUV()).rgb;
        #endif

            info.f0_dielectric = min(info.f0_dielectric * u_KHR_materials_specular_specularColorFactor * specularTexture.rgb, vec3(1.0));
            info.specularWeight = u_KHR_materials_specular_specularFactor * specularTexture.a;
            info.f90_dielectric = vec3(info.specularWeight);
            return info;
        }
        #endif


        #ifdef MATERIAL_TRANSMISSION
        MaterialInfo getTransmissionInfo(MaterialInfo info)
        {
            info.transmissionFactor = u_TransmissionFactor;

        #ifdef HAS_TRANSMISSION_MAP
            vec4 transmissionSample = texture(u_TransmissionSampler, getTransmissionUV());
            info.transmissionFactor *= transmissionSample.r;
        #endif

        #ifdef MATERIAL_DISPERSION
            info.dispersion = u_Dispersion;
        #else
            info.dispersion = 0.0;
        #endif
            return info;
        }
        #endif
    )"
    },
    {
        "material_info4.glsl", R"(
        #ifdef MATERIAL_VOLUME
        MaterialInfo getVolumeInfo(MaterialInfo info)
        {
            info.thickness = u_ThicknessFactor;
            info.attenuationColor = u_AttenuationColor;
            info.attenuationDistance = u_AttenuationDistance;

        #ifdef HAS_THICKNESS_MAP
            vec4 thicknessSample = texture(u_ThicknessSampler, getThicknessUV());
            info.thickness *= thicknessSample.g;
        #endif
            return info;
        }
        #endif


        #ifdef MATERIAL_IRIDESCENCE
        MaterialInfo getIridescenceInfo(MaterialInfo info)
        {
            info.iridescenceFactor = u_IridescenceFactor;
            info.iridescenceIor = u_IridescenceIor;
            info.iridescenceThickness = u_IridescenceThicknessMaximum;

            #ifdef HAS_IRIDESCENCE_MAP
                info.iridescenceFactor *= texture(u_IridescenceSampler, getIridescenceUV()).r;
            #endif

            #ifdef HAS_IRIDESCENCE_THICKNESS_MAP
                float thicknessSampled = texture(u_IridescenceThicknessSampler, getIridescenceThicknessUV()).g;
                float thickness = mix(u_IridescenceThicknessMinimum, u_IridescenceThicknessMaximum, thicknessSampled);
                info.iridescenceThickness = thickness;
            #endif

            return info;
        }
        #endif


        #ifdef MATERIAL_DIFFUSE_TRANSMISSION
        MaterialInfo getDiffuseTransmissionInfo(MaterialInfo info)
        {
            info.diffuseTransmissionFactor = u_DiffuseTransmissionFactor;
            info.diffuseTransmissionColorFactor = u_DiffuseTransmissionColorFactor;

            #ifdef HAS_DIFFUSE_TRANSMISSION_MAP
                info.diffuseTransmissionFactor *= texture(u_DiffuseTransmissionSampler, getDiffuseTransmissionUV()).a;
            #endif

            #ifdef HAS_DIFFUSE_TRANSMISSION_COLOR_MAP
                info.diffuseTransmissionColorFactor *= texture(u_DiffuseTransmissionColorSampler, getDiffuseTransmissionColorUV()).rgb;
            #endif

            return info;
        }
        #endif
    )"
    },
    {
        "material_info5.glsl", R"(

        #ifdef MATERIAL_CLEARCOAT
        MaterialInfo getClearCoatInfo(MaterialInfo info, NormalInfo normalInfo)
        {
            info.clearcoatFactor = u_ClearcoatFactor;
            info.clearcoatRoughness = u_ClearcoatRoughnessFactor;
            info.clearcoatF0 = vec3(pow((info.ior - 1.0) / (info.ior + 1.0), 2.0));
            info.clearcoatF90 = vec3(1.0);

        #ifdef HAS_CLEARCOAT_MAP
            vec4 clearcoatSample = texture(u_ClearcoatSampler, getClearcoatUV());
            info.clearcoatFactor *= clearcoatSample.r;
        #endif

        #ifdef HAS_CLEARCOAT_ROUGHNESS_MAP
            vec4 clearcoatSampleRoughness = texture(u_ClearcoatRoughnessSampler, getClearcoatRoughnessUV());
            info.clearcoatRoughness *= clearcoatSampleRoughness.g;
        #endif

            info.clearcoatNormal = getClearcoatNormal(normalInfo);
            info.clearcoatRoughness = clamp(info.clearcoatRoughness, 0.0, 1.0);
            return info;
        }
        #endif


        #ifdef MATERIAL_IOR
        MaterialInfo getIorInfo(MaterialInfo info)
        {
            info.f0_dielectric = vec3(pow(( u_Ior - 1.0) /  (u_Ior + 1.0), 2.0));
            info.ior = u_Ior;
            return info;
        }
        #endif

        #ifdef MATERIAL_ANISOTROPY
        MaterialInfo getAnisotropyInfo(MaterialInfo info, NormalInfo normalInfo)
        {
            vec2 direction = vec2(1.0, 0.0);
            float strengthFactor = 1.0;
        #ifdef HAS_ANISOTROPY_MAP
            vec3 anisotropySample = texture(u_AnisotropySampler, getAnisotropyUV()).xyz;
            direction = anisotropySample.xy * 2.0 - vec2(1.0);
            strengthFactor = anisotropySample.z;
        #endif
            vec2 directionRotation = u_Anisotropy.xy; // cos(theta), sin(theta)
            mat2 rotationMatrix = mat2(directionRotation.x, directionRotation.y, -directionRotation.y, directionRotation.x);
            direction = rotationMatrix * direction.xy;

            info.anisotropicT = mat3(normalInfo.t, normalInfo.b, normalInfo.n) * normalize(vec3(direction, 0.0));
            info.anisotropicB = cross(normalInfo.ng, info.anisotropicT);
            info.anisotropyStrength = clamp(u_Anisotropy.z * strengthFactor, 0.0, 1.0);
            return info;
        }
        #endif


        float albedoSheenScalingLUT(float NdotV, float sheenRoughnessFactor)
        {
            //return NdotV;
            return texture(u_SheenELUT, vec2(NdotV, sheenRoughnessFactor)).r;
        }

    )"
    },
    {
        "iridescence.glsl", R"(
        const mat3 XYZ_TO_REC709 = mat3(
            3.2404542, -0.9692660,  0.0556434,
            -1.5371385,  1.8760108, -0.2040259,
            -0.4985314,  0.0415560,  1.0572252
        );

        vec3 Fresnel0ToIor(vec3 fresnel0) {
            vec3 sqrtF0 = sqrt(fresnel0);
            return (vec3(1.0) + sqrtF0) / (vec3(1.0) - sqrtF0);
        }

        vec3 IorToFresnel0(vec3 transmittedIor, float incidentIor) {
            return sq((transmittedIor - vec3(incidentIor)) / (transmittedIor + vec3(incidentIor)));
        }

        float IorToFresnel0(float transmittedIor, float incidentIor) {
            return sq((transmittedIor - incidentIor) / (transmittedIor + incidentIor));
        }

        vec3 evalSensitivity(float OPD, vec3 shift) {
            float phase = 2.0 * M_PI * OPD * 1.0e-9;
            vec3 val = vec3(5.4856e-13, 4.4201e-13, 5.2481e-13);
            vec3 pos = vec3(1.6810e+06, 1.7953e+06, 2.2084e+06);
            vec3 var = vec3(4.3278e+09, 9.3046e+09, 6.6121e+09);

            vec3 xyz = val * sqrt(2.0 * M_PI * var) * cos(pos * phase + shift) * exp(-sq(phase) * var);
            xyz.x += 9.7470e-14 * sqrt(2.0 * M_PI * 4.5282e+09) * cos(2.2399e+06 * phase + shift[0]) * exp(-4.5282e+09 * sq(phase));
            xyz /= 1.0685e-7;

            vec3 srgb = XYZ_TO_REC709 * xyz;
            return srgb;
        }

        vec3 evalIridescence(float outsideIOR, float eta2, float cosTheta1, float thinFilmThickness, vec3 baseF0) {
            vec3 I;

            // Force iridescenceIor -> outsideIOR when thinFilmThickness -> 0.0
            float iridescenceIor = mix(outsideIOR, eta2, smoothstep(0.0, 0.03, thinFilmThickness));
            // Evaluate the cosTheta on the base layer (Snell law)
            float sinTheta2Sq = sq(outsideIOR / iridescenceIor) * (1.0 - sq(cosTheta1));

            // Handle TIR:
            float cosTheta2Sq = 1.0 - sinTheta2Sq;
            if (cosTheta2Sq < 0.0) {
                return vec3(1.0);
            }

            float cosTheta2 = sqrt(cosTheta2Sq);

            // First interface
            float R0 = IorToFresnel0(iridescenceIor, outsideIOR);
            float R12 = F_Schlick(R0, cosTheta1);
            float R21 = R12;
            float T121 = 1.0 - R12;
            float phi12 = 0.0;
            if (iridescenceIor < outsideIOR) phi12 = M_PI;
            float phi21 = M_PI - phi12;

            // Second interface
            vec3 baseIOR = Fresnel0ToIor(clamp(baseF0, 0.0, 0.9999)); // guard against 1.0
            vec3 R1 = IorToFresnel0(baseIOR, iridescenceIor);
            vec3 R23 = F_Schlick(R1, cosTheta2);
            vec3 phi23 = vec3(0.0);
            if (baseIOR[0] < iridescenceIor) phi23[0] = M_PI;
            if (baseIOR[1] < iridescenceIor) phi23[1] = M_PI;
            if (baseIOR[2] < iridescenceIor) phi23[2] = M_PI;

            // Phase shift
            float OPD = 2.0 * iridescenceIor * thinFilmThickness * cosTheta2;
            vec3 phi = vec3(phi21) + phi23;

            // Compound terms
            vec3 R123 = clamp(R12 * R23, 1e-5, 0.9999);
            vec3 r123 = sqrt(R123);
            vec3 Rs = sq(T121) * R23 / (vec3(1.0) - R123);

            // Reflectance term for m = 0 (DC term amplitude)
            vec3 C0 = R12 + Rs;
            I = C0;

            // Reflectance term for m > 0 (pairs of diracs)
            vec3 Cm = Rs - T121;
            for (int m = 1; m <= 2; ++m)
            {
                Cm *= r123;
                vec3 Sm = 2.0 * evalSensitivity(float(m) * OPD, float(m) * phi);
                I += Cm * Sm;
            }

            // Since out of gamut colors might be produced, negative color values are clamped to 0.
            return max(I, vec3(0.0));
        }
    )"
    },
    {
        "animation1.glsl", R"(

        #ifdef HAS_MORPH_TARGETS
        uniform highp sampler2DArray u_MorphTargetsSampler;
        #endif

        #ifdef USE_MORPHING
        uniform float u_morphWeights[WEIGHT_COUNT];
        #endif

        #ifdef HAS_JOINTS_0_VEC4
        in vec4 a_joints_0;
        #endif

        #ifdef HAS_JOINTS_1_VEC4
        in vec4 a_joints_1;
        #endif

        #ifdef HAS_WEIGHTS_0_VEC4
        in vec4 a_weights_0;
        #endif

        #ifdef HAS_WEIGHTS_1_VEC4
        in vec4 a_weights_1;
        #endif

        #ifdef USE_SKINNING
        uniform sampler2D u_jointsSampler;
        #endif

        #ifdef USE_SKINNING

        mat4 getMatrixFromTexture(sampler2D s, int index)
        {
            mat4 result = mat4(1);
            int texSize = textureSize(s, 0)[0];
            int pixelIndex = index * 4;
            for (int i = 0; i < 4; ++i)
            {
                int x = (pixelIndex + i) % texSize;
                //Rounding mode of integers is undefined:
                //https://www.khronos.org/registry/OpenGL/specs/es/3.0/GLSL_ES_Specification_3.00.pdf (section 12.33)
                int y = (pixelIndex + i - x) / texSize; 
                result[i] = texelFetch(s, ivec2(x,y), 0);
            }
            return result;
        }

        mat4 getSkinningMatrix()
        {
            mat4 skin = mat4(0);

        #if defined(HAS_WEIGHTS_0_VEC4) && defined(HAS_JOINTS_0_VEC4)
            skin +=
                a_weights_0.x * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.x) * 2) +
                a_weights_0.y * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.y) * 2) +
                a_weights_0.z * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.z) * 2) +
                a_weights_0.w * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.w) * 2);
        #endif

        #if defined(HAS_WEIGHTS_1_VEC4) && defined(HAS_JOINTS_1_VEC4)
            skin +=
                a_weights_1.x * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.x) * 2) +
                a_weights_1.y * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.y) * 2) +
                a_weights_1.z * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.z) * 2) +
                a_weights_1.w * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.w) * 2);
        #endif
            if (skin == mat4(0)) { 
                return mat4(1); 
            }
            return skin;
        }


        mat4 getSkinningNormalMatrix()
        {
            mat4 skin = mat4(0);

        #if defined(HAS_WEIGHTS_0_VEC4) && defined(HAS_JOINTS_0_VEC4)
            skin +=
                a_weights_0.x * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.x) * 2 + 1) +
                a_weights_0.y * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.y) * 2 + 1) +
                a_weights_0.z * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.z) * 2 + 1) +
                a_weights_0.w * getMatrixFromTexture(u_jointsSampler, int(a_joints_0.w) * 2 + 1);
        #endif

        #if defined(HAS_WEIGHTS_1_VEC4) && defined(HAS_JOINTS_1_VEC4)
            skin +=
                a_weights_1.x * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.x) * 2 + 1) +
                a_weights_1.y * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.y) * 2 + 1) +
                a_weights_1.z * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.z) * 2 + 1) +
                a_weights_1.w * getMatrixFromTexture(u_jointsSampler, int(a_joints_1.w) * 2 + 1);
        #endif
            if (skin == mat4(0)) { 
                return mat4(1); 
            }
            return skin;
        }

        #endif // !USE_SKINNING

    )"
    },
    {
        "animation2.glsl", R"(
        #ifdef USE_MORPHING

        #ifdef HAS_MORPH_TARGETS
        vec4 getDisplacement(int vertexID, int targetIndex, int texSize)
        {
            int x = vertexID % texSize;
            //Rounding mode of integers is undefined:
            //https://www.khronos.org/registry/OpenGL/specs/es/3.0/GLSL_ES_Specification_3.00.pdf (section 12.33)
            int y = (vertexID - x) / texSize; 
            return texelFetch(u_MorphTargetsSampler, ivec3(x, y, targetIndex), 0);
        }
        #endif


        vec4 getTargetPosition(int vertexID)
        {
            vec4 pos = vec4(0);
        #ifdef HAS_MORPH_TARGET_POSITION
            int texSize = textureSize(u_MorphTargetsSampler, 0)[0];
            for(int i = 0; i < WEIGHT_COUNT; i++)
            {
                vec4 displacement = getDisplacement(vertexID, MORPH_TARGET_POSITION_OFFSET + i, texSize);
                pos += u_morphWeights[i] * displacement;
            }
        #endif

            return pos;
        }

        vec3 getTargetNormal(int vertexID)
        {
            vec3 normal = vec3(0);

        #ifdef HAS_MORPH_TARGET_NORMAL
            int texSize = textureSize(u_MorphTargetsSampler, 0)[0];
            for(int i = 0; i < WEIGHT_COUNT; i++)
            {
                vec3 displacement = getDisplacement(vertexID, MORPH_TARGET_NORMAL_OFFSET + i, texSize).xyz;
                normal += u_morphWeights[i] * displacement;
            }
        #endif

            return normal;
        }


        vec3 getTargetTangent(int vertexID)
        {
            vec3 tangent = vec3(0);

        #ifdef HAS_MORPH_TARGET_TANGENT
            int texSize = textureSize(u_MorphTargetsSampler, 0)[0];
            for(int i = 0; i < WEIGHT_COUNT; i++)
            {
                vec3 displacement = getDisplacement(vertexID, MORPH_TARGET_TANGENT_OFFSET + i, texSize).xyz;
                tangent += u_morphWeights[i] * displacement;
            }
        #endif

            return tangent;
        }

        vec2 getTargetTexCoord0(int vertexID)
        {
            vec2 uv = vec2(0);

        #ifdef HAS_MORPH_TARGET_TEXCOORD_0
            int texSize = textureSize(u_MorphTargetsSampler, 0)[0];
            for(int i = 0; i < WEIGHT_COUNT; i++)
            {
                vec2 displacement = getDisplacement(vertexID, MORPH_TARGET_TEXCOORD_0_OFFSET + i, texSize).xy;
                uv += u_morphWeights[i] * displacement;
            }
        #endif

            return uv;
        }

        vec2 getTargetTexCoord1(int vertexID)
        {
            vec2 uv = vec2(0);

        #ifdef HAS_MORPH_TARGET_TEXCOORD_1
            int texSize = textureSize(u_MorphTargetsSampler, 0)[0];
            for(int i = 0; i < WEIGHT_COUNT; i++)
            {
                vec2 displacement = getDisplacement(vertexID, MORPH_TARGET_TEXCOORD_1_OFFSET + i, texSize).xy;
                uv += u_morphWeights[i] * displacement;
            }
        #endif

            return uv;
        }

        vec4 getTargetColor0(int vertexID)
        {
            vec4 color = vec4(0);

        #ifdef HAS_MORPH_TARGET_COLOR_0
            int texSize = textureSize(u_MorphTargetsSampler, 0)[0];
            for(int i = 0; i < WEIGHT_COUNT; i++)
            {
                vec4 displacement = getDisplacement(vertexID, MORPH_TARGET_COLOR_0_OFFSET + i, texSize);
                color += u_morphWeights[i] * displacement;
            }
        #endif

            return color;
        }

        #endif // !USE_MORPHING
    )"
    },
    {
        "vert_v1_chunk_00.glsl", R"(

        #ifdef HAS_NORMAL_VEC3
        in vec3 a_normal;
        #endif

        #ifdef HAS_NORMAL_VEC3
        #ifdef HAS_TANGENT_VEC4
        in vec4 a_tangent;
        out mat3 v_TBN;
        #else
        out vec3 v_Normal;
        #endif
        #endif

        #ifdef HAS_TEXCOORD_0_VEC2
        in vec2 a_texcoord_0;
        #endif

        #ifdef HAS_TEXCOORD_1_VEC2
        in vec2 a_texcoord_1;
        #endif

        out vec2 v_texcoord_0;
        out vec2 v_texcoord_1;

        #ifdef HAS_COLOR_0_VEC3
        in vec3 a_color_0;
        out vec3 v_Color;
        #endif

        #ifdef HAS_COLOR_0_VEC4
        in vec4 a_color_0;
        out vec4 v_Color;
        #endif

        #ifdef USE_INSTANCING
        in mat4 a_instance_model_matrix;
        #endif

        #ifdef HAS_VERT_NORMAL_UV_TRANSFORM
        uniform mat3 u_vertNormalUVTransform;
        #endif

        vec4 getPosition()
        {
            vec4 pos = vec4(a_position, 1.0);

        #ifdef USE_MORPHING
            pos += getTargetPosition(gl_VertexID);
        #endif

        #ifdef USE_SKINNING
            pos = getSkinningMatrix() * pos;
        #endif

            return pos;
        }


        #ifdef HAS_NORMAL_VEC3
        vec3 getNormal()
        {
            vec3 normal = a_normal;

        #ifdef USE_MORPHING
            normal += getTargetNormal(gl_VertexID);
        #endif

        #ifdef USE_SKINNING
            normal = mat3(getSkinningNormalMatrix()) * normal;
        #endif

            return normalize(normal);
        }
        #endif

        #ifdef HAS_NORMAL_VEC3
        #ifdef HAS_TANGENT_VEC4
        vec3 getTangent()
        {
            vec3 tangent = a_tangent.xyz;

        #ifdef USE_MORPHING
            tangent += getTargetTangent(gl_VertexID);
        #endif
        
    )"
    },
    {
        "vert_v1_chunk_01.glsl", R"(
        #ifdef USE_SKINNING
            tangent = mat3(getSkinningMatrix()) * tangent;
        #endif

            return normalize(tangent);
        }
        #endif
        #endif

        mat4 temp_makeNormalMatrixFromViewProj(mat4 _viewProjModelMatrix) {
            mat4 normMat = _viewProjModelMatrix ;
            normMat[0][0] = 1.0;
            normMat[0][1] = 0.0;
            normMat[0][2] = 0.0;
            normMat[0][3] = 0.0;
            normMat[1][0] = 0.0;
            normMat[1][3] = 0.0;
            normMat[2][0] = 0.0;
            normMat[2][3] = 0.0;
            normMat[3][0] = 0.0;
            normMat[3][1] = 0.0;
            normMat[3][2] = 0.0;
            normMat[3][3] = 1.0;
            return normMat;
        }

        void main()
        {
            gl_PointSize = 1.0f;
        #ifdef USE_INSTANCING
            mat4 modelMatrix = a_instance_model_matrix;
            mat4 normalMatrix = transpose(inverse(modelMatrix));
        #else
            mat4 modelMatrix = u_ModelMatrix;
            //mat4 normalMatrix =  u_NormalMatrix;
            mat4 normalMatrix =  transpose(inverse(modelMatrix));

        #endif
            vec4 pos = modelMatrix * getPosition();
            v_Position = vec3(pos.xyz) / pos.w;

        #ifdef HAS_NORMAL_VEC3
        #ifdef HAS_TANGENT_VEC4
            vec3 tangent = getTangent();
            vec3 normalW = normalize(vec3(normalMatrix * vec4(getNormal(), 0.0)));
            vec3 tangentW = vec3(modelMatrix * vec4(tangent, 0.0));
            vec3 bitangentW = cross(normalW, tangentW) * a_tangent.w;

        #ifdef HAS_VERT_NORMAL_UV_TRANSFORM
            tangentW = u_vertNormalUVTransform * tangentW;
            bitangentW = u_vertNormalUVTransform * bitangentW;
        #endif

            bitangentW = normalize(bitangentW);
            tangentW = normalize(tangentW);

            v_TBN = mat3(tangentW, bitangentW, normalW);
        #else
            v_Normal = normalize(vec3(normalMatrix * vec4(getNormal(), 0.0)));
        #endif
        #endif

            v_texcoord_0 = vec2(0.0, 0.0);
            v_texcoord_1 = vec2(0.0, 0.0);

        #ifdef HAS_TEXCOORD_0_VEC2
            v_texcoord_0 = a_texcoord_0;
        #endif

        #ifdef HAS_TEXCOORD_1_VEC2
            v_texcoord_1 = a_texcoord_1;
        #endif

        #ifdef USE_MORPHING
            v_texcoord_0 += getTargetTexCoord0(gl_VertexID);
            v_texcoord_1 += getTargetTexCoord1(gl_VertexID);
        #endif


        #if defined(HAS_COLOR_0_VEC3) 
            v_Color = a_color_0;
        #if defined(USE_MORPHING)
            v_Color = clamp(v_Color + getTargetColor0(gl_VertexID).xyz, 0.0f, 1.0f);
        #endif
        #endif

        #if defined(HAS_COLOR_0_VEC4) 
            v_Color = a_color_0;
        #if defined(USE_MORPHING)
            v_Color = clamp(v_Color + getTargetColor0(gl_VertexID), 0.0f, 1.0f);
        #endif
        #endif

            gl_Position = u_ViewProjectionMatrix * pos;
        }
    )"
    },
    {
        "frag_v1_chunk_00.glsl", R"(
        out vec4 g_finalColor;
        void main()
        {
        
        vec4 baseColor = getBaseColor();

        #if ALPHAMODE == _OPAQUE
            baseColor.a = 1.0;
        #endif

        vec4 temp_origBaseColor = baseColor;

        vec3 color = vec3(0);

        vec3 v = normalize(u_Camera - v_Position);

        NormalInfo normalInfo = getNormalInfo(v);
        vec3 n = normalInfo.n;
        vec3 t = normalInfo.t;
        vec3 b = normalInfo.b;

        float NdotV = clampedDot(n, v);
        float TdotV = clampedDot(t, v);
        float BdotV = clampedDot(b, v);

        MaterialInfo materialInfo;
        materialInfo.baseColor = baseColor.rgb;
        
        // The default index of refraction of 1.5 yields a dielectric normal incidence reflectance of 0.04.
        materialInfo.ior = 1.5;
        materialInfo.f0_dielectric = vec3(0.04);
        materialInfo.specularWeight = 1.0;

        // Anything less than 2% is physically impossible and is instead considered to be shadowing. Compare to "Real-Time-Rendering" 4th editon on page 325.
        materialInfo.f90 = vec3(1.0);
        materialInfo.f90_dielectric = materialInfo.f90;

        #ifdef MATERIAL_IOR
            materialInfo = getIorInfo(materialInfo);
        #endif

        #ifdef MATERIAL_METALLICROUGHNESS
            materialInfo = getMetallicRoughnessInfo(materialInfo);
        #endif

        #ifdef MATERIAL_SHEEN
            materialInfo = getSheenInfo(materialInfo);
        #endif
    )"
    },
    {
        "frag_v1_chunk_01a.glsl", R"(

        #ifdef MATERIAL_CLEARCOAT
            materialInfo = getClearCoatInfo(materialInfo, normalInfo);
        #endif

        #ifdef MATERIAL_SPECULAR
            materialInfo = getSpecularInfo(materialInfo);
        #endif

        #ifdef MATERIAL_TRANSMISSION
            materialInfo = getTransmissionInfo(materialInfo);
        #endif

        #ifdef MATERIAL_VOLUME
            materialInfo = getVolumeInfo(materialInfo);
        #endif

        #ifdef MATERIAL_IRIDESCENCE
            materialInfo = getIridescenceInfo(materialInfo);
        #endif

        #ifdef MATERIAL_DIFFUSE_TRANSMISSION
            materialInfo = getDiffuseTransmissionInfo(materialInfo);
        #endif

        #ifdef MATERIAL_ANISOTROPY
            materialInfo = getAnisotropyInfo(materialInfo, normalInfo);
        #endif

        materialInfo.perceptualRoughness = clamp(materialInfo.perceptualRoughness, 0.0, 1.0);
        materialInfo.metallic = clamp(materialInfo.metallic, 0.0, 1.0);

        // Roughness is authored as perceptual roughness; as is convention,
        // convert to material roughness by squaring the perceptual roughness.
        materialInfo.alphaRoughness = materialInfo.perceptualRoughness * materialInfo.perceptualRoughness;


        // LIGHTING
        vec3 f_specular_dielectric = vec3(0.0);
        vec3 f_specular_metal = vec3(0.0);
        vec3 f_diffuse = vec3(0.0);
        vec3 f_dielectric_brdf_ibl = vec3(0.0);
        vec3 f_metal_brdf_ibl = vec3(0.0);
        vec3 f_emissive = vec3(0.0);
        vec3 clearcoat_brdf = vec3(0.0);
        vec3 f_sheen = vec3(0.0);
        vec3 f_specular_transmission = vec3(0.0);
        vec3 f_diffuse_transmission = vec3(0.0);

        float clearcoatFactor = 0.0;
        vec3 clearcoatFresnel = vec3(0);
    )"
    },
    {
        "frag_v1_chunk_01b.glsl", R"(
        float albedoSheenScaling = 1.0;
        float diffuseTransmissionThickness = 1.0;

        #ifdef MATERIAL_IRIDESCENCE
            vec3 iridescenceFresnel_dielectric = evalIridescence(1.0, materialInfo.iridescenceIor, NdotV, materialInfo.iridescenceThickness, materialInfo.f0_dielectric);
            vec3 iridescenceFresnel_metallic = evalIridescence(1.0, materialInfo.iridescenceIor, NdotV, materialInfo.iridescenceThickness, baseColor.rgb);

            if (materialInfo.iridescenceThickness == 0.0) {
                materialInfo.iridescenceFactor = 0.0;
            }
        #endif

        #ifdef MATERIAL_DIFFUSE_TRANSMISSION
        #ifdef MATERIAL_VOLUME
            diffuseTransmissionThickness = materialInfo.thickness *
                (length(vec3(u_ModelMatrix[0].xyz)) + length(vec3(u_ModelMatrix[1].xyz)) + length(vec3(u_ModelMatrix[2].xyz))) / 3.0;
        #endif
        #endif

        #ifdef MATERIAL_CLEARCOAT
            clearcoatFactor = materialInfo.clearcoatFactor;
            clearcoatFresnel = F_Schlick(materialInfo.clearcoatF0, materialInfo.clearcoatF90, clampedDot(materialInfo.clearcoatNormal, v));
        #endif

            // Calculate lighting contribution from image based lighting source (IBL)

        #if defined(USE_IBL) || defined(MATERIAL_TRANSMISSION)

            f_diffuse = getDiffuseLight(n) * baseColor.rgb ;

        #ifdef MATERIAL_DIFFUSE_TRANSMISSION
            vec3 diffuseTransmissionIBL = getDiffuseLight(-n) * materialInfo.diffuseTransmissionColorFactor;
        #ifdef MATERIAL_VOLUME
                diffuseTransmissionIBL = applyVolumeAttenuation(diffuseTransmissionIBL, diffuseTransmissionThickness, materialInfo.attenuationColor, materialInfo.attenuationDistance);
        #endif
            f_diffuse = mix(f_diffuse, diffuseTransmissionIBL, materialInfo.diffuseTransmissionFactor);
        #endif


        #if defined(MATERIAL_TRANSMISSION)
            f_specular_transmission = getIBLVolumeRefraction(
                n, v,
                materialInfo.perceptualRoughness,
                baseColor.rgb, v_Position, u_ModelMatrix, u_ViewMatrix, u_ProjectionMatrix,
                materialInfo.ior, materialInfo.thickness, materialInfo.attenuationColor, materialInfo.attenuationDistance, materialInfo.dispersion);
            f_diffuse = mix(f_diffuse, f_specular_transmission, materialInfo.transmissionFactor);
        #endif
    )"
    },
    {
        "frag_v1_chunk_02a.glsl", R"(

        #ifdef MATERIAL_ANISOTROPY
            f_specular_metal = getIBLRadianceAnisotropy(n, v, materialInfo.perceptualRoughness, materialInfo.anisotropyStrength, materialInfo.anisotropicB);
            f_specular_dielectric = f_specular_metal;
        #else
            f_specular_metal = getIBLRadianceGGX(n, v, materialInfo.perceptualRoughness);
            f_specular_dielectric = f_specular_metal;
        #endif

            // Calculate fresnel mix for IBL  

            vec3 f_metal_fresnel_ibl = getIBLGGXFresnel(n, v, materialInfo.perceptualRoughness, baseColor.rgb, 1.0);
            f_metal_brdf_ibl = f_metal_fresnel_ibl * f_specular_metal;
        
            vec3 f_dielectric_fresnel_ibl = getIBLGGXFresnel(n, v, materialInfo.perceptualRoughness, materialInfo.f0_dielectric, materialInfo.specularWeight);
            f_dielectric_brdf_ibl = mix(f_diffuse, f_specular_dielectric,  f_dielectric_fresnel_ibl);

        #ifdef MATERIAL_IRIDESCENCE
            f_metal_brdf_ibl = mix(f_metal_brdf_ibl, f_specular_metal * iridescenceFresnel_metallic, materialInfo.iridescenceFactor);
            f_dielectric_brdf_ibl = mix(f_dielectric_brdf_ibl, rgb_mix(f_diffuse, f_specular_dielectric, iridescenceFresnel_dielectric), materialInfo.iridescenceFactor);
        #endif

        #ifdef MATERIAL_CLEARCOAT
            clearcoat_brdf = getIBLRadianceGGX(materialInfo.clearcoatNormal, v, materialInfo.clearcoatRoughness);
        #endif

        #ifdef MATERIAL_SHEEN
            f_sheen = getIBLRadianceCharlie(n, v, materialInfo.sheenRoughnessFactor, materialInfo.sheenColorFactor);
            albedoSheenScaling = 1.0 - max3(materialInfo.sheenColorFactor) * albedoSheenScalingLUT(NdotV, materialInfo.sheenRoughnessFactor);
        #endif

            color = mix(f_dielectric_brdf_ibl, f_metal_brdf_ibl, materialInfo.metallic);
            color = f_sheen + color * albedoSheenScaling;
            color = mix(color, clearcoat_brdf, clearcoatFactor * clearcoatFresnel);

        #ifdef HAS_OCCLUSION_MAP
            float ao = 1.0;
            ao = texture(u_OcclusionSampler,  getOcclusionUV()).r;
            color = color * (1.0 + u_OcclusionStrength * (ao - 1.0)); 
            //temp_origBaseColor.rgb *= (1.0 + u_OcclusionStrength * (ao - 1.0));

            //color = vec4(1.0, 0.0, 0.5, 1.0);
        #endif

        //#else  // Temporary addition to enable occlusion maps in non-IBL mode, which isn't physically accurate and should eventually be removed.
        //#ifdef HAS_OCCLUSION_MAP
        //    float ao = 1.0;
        //    ao = texture(u_OcclusionSampler,  getOcclusionUV()).r;
        //    //color = vec3(1.0, 0.0, 0.5);
        //    color = color * (1.0 + u_OcclusionStrength * (ao - 1.0)); 
        //#endif
        #endif //end USE_IBL


        f_diffuse = vec3(0.0);

    )"
    },
    {
        "frag_v1_chunk_02b.glsl", R"(

        f_specular_dielectric = vec3(0.0);
        f_specular_metal = vec3(0.0);
        vec3 f_dielectric_brdf = vec3(0.0);
        vec3 f_metal_brdf = vec3(0.0);

        #ifdef USE_PUNCTUAL
        /*
        Light temp_keylight = Light( 
            normalize(vec3(-0.1, -0.75, -0.45)),    //vec3 direction
            -1.0,                  //float range
            vec3(1.0, 1.0, 1.0),    //vec3 color
            1.0,                    //float intensity
            vec3(0.0, 0.0, 0.0),    //vec3 position
            0.0,                    //float innerConeCos
            0.0,                    //float outerConeCos
            0                       //int type;
                                    //  const Type_Directional = 0;
                                    //  const Type_Point = 1;
                                    //  const Type_Spot = 2;
            );
        Light temp_filllight = Light( 
            normalize(-vec3(-0.2, -0.65, -0.35)),    //vec3 direction
            -1.0,                  //float range
            vec3(1.0, 1.0, 1.0),    //vec3 color
            0.5,                    //float intensity
            vec3(0.0, 0.0, 0.0),    //vec3 position
            0.0,                    //float innerConeCos
            0.0,                    //float outerConeCos
            0                       //int type;
                                    //  const Type_Directional = 0;
                                    //  const Type_Point = 1;
                                    //  const Type_Spot = 2;
            );        
    )"
    },
    {
        "frag_v1_chunk_03a.glsl", R"(

        u_Lights[1] = temp_keylight;
        u_Lights[2] = temp_filllight;
        */
        for (int i = 0; i < LIGHT_COUNT; ++i)
        {
            Light light = u_Lights[i+1];

            vec3 pointToLight;
            if (light.type != LightType_Directional)
            {
                pointToLight = light.position - v_Position;
            }
            else
            {
                pointToLight = -light.direction;
            }

            // BSTF

            vec3 l = normalize(pointToLight);   // Direction from surface point to light
            vec3 h = normalize(l + v);          // Direction of the vector between l and v, called halfway vector
            float NdotL = clampedDot(n, l);
            float NdotV = clampedDot(n, v);
            float NdotH = clampedDot(n, h);
            float LdotH = clampedDot(l, h);
            float VdotH = clampedDot(v, h);

            vec3 dielectric_fresnel = F_Schlick(materialInfo.f0_dielectric * materialInfo.specularWeight, materialInfo.f90_dielectric, abs(VdotH));
            vec3 metal_fresnel = F_Schlick(baseColor.rgb, vec3(1.0), abs(VdotH));
            
            vec3 lightIntensity = getLighIntensity(light, pointToLight);
            
            vec3 l_diffuse = lightIntensity * NdotL * BRDF_lambertian(baseColor.rgb);
            vec3 l_specular_dielectric = vec3(0.0);
            vec3 l_specular_metal = vec3(0.0);
            vec3 l_dielectric_brdf = vec3(0.0);
            vec3 l_metal_brdf = vec3(0.0);
            vec3 l_clearcoat_brdf = vec3(0.0);
            vec3 l_sheen = vec3(0.0);
            float l_albedoSheenScaling = 1.0;
          
        #ifdef MATERIAL_DIFFUSE_TRANSMISSION
            l_diffuse = l_diffuse * (1.0 - materialInfo.diffuseTransmissionFactor);
            if (dot(n, l) < 0.0) {
                float diffuseNdotL = clampedDot(-n, l);
                vec3 diffuse_btdf = lightIntensity * diffuseNdotL * BRDF_lambertian(materialInfo.diffuseTransmissionColorFactor);

                vec3 l_mirror = normalize(l + 2.0 * n * dot(-l, n)); // Mirror light reflection vector on surface
                float diffuseVdotH = clampedDot(v, normalize(l_mirror + v));
                dielectric_fresnel = F_Schlick(materialInfo.f0_dielectric * materialInfo.specularWeight, materialInfo.f90_dielectric, abs(diffuseVdotH));

        #ifdef MATERIAL_VOLUME
                    diffuse_btdf = applyVolumeAttenuation(diffuse_btdf, diffuseTransmissionThickness, materialInfo.attenuationColor, materialInfo.attenuationDistance);
        #endif
                    l_diffuse += diffuse_btdf * materialInfo.diffuseTransmissionFactor;
                }
        #endif // MATERIAL_DIFFUSE_TRANSMISSION

        //temp_origBaseColor.rgb = vec3(l_diffuse);
    )"
    },
    {
        "frag_v1_chunk_03b.glsl", R"(
                // BTDF (Bidirectional Transmittance Distribution Function)
        #ifdef MATERIAL_TRANSMISSION
                // If the light ray travels through the geometry, use the point it exits the geometry again.
                // That will change the angle to the light source, if the material refracts the light ray.
                vec3 transmissionRay = getVolumeTransmissionRay(n, v, materialInfo.thickness, materialInfo.ior, u_ModelMatrix);
                pointToLight -= transmissionRay;
                l = normalize(pointToLight);

                vec3 transmittedLight = lightIntensity * getPunctualRadianceTransmission(n, v, l, materialInfo.alphaRoughness, baseColor.rgb, materialInfo.ior);

        #ifdef MATERIAL_VOLUME
                transmittedLight = applyVolumeAttenuation(transmittedLight, length(transmissionRay), materialInfo.attenuationColor, materialInfo.attenuationDistance);
        #endif
                l_diffuse = mix(l_diffuse, transmittedLight, materialInfo.transmissionFactor);
        #endif
                // Calculation of analytical light
                // https://github.com/KhronosGroup/glTF/tree/master/specification/2.0#acknowledgments AppendixB
                vec3 intensity = getLighIntensity(light, pointToLight);

        #ifdef MATERIAL_ANISOTROPY
                l_specular_metal = intensity * NdotL * BRDF_specularGGXAnisotropy(materialInfo.alphaRoughness, materialInfo.anisotropyStrength, n, v, l, h, materialInfo.anisotropicT, materialInfo.anisotropicB);
                l_specular_dielectric = l_specular_metal;
        #else
                l_specular_metal = intensity * NdotL * BRDF_specularGGX(materialInfo.alphaRoughness, NdotL, NdotV, NdotH);
                l_specular_dielectric = l_specular_metal;
        #endif

                l_metal_brdf = metal_fresnel * l_specular_metal;
                l_dielectric_brdf = mix(l_diffuse, l_specular_dielectric, dielectric_fresnel); // Do we need to handle vec3 fresnel here?


        #ifdef MATERIAL_IRIDESCENCE
                l_metal_brdf = mix(l_metal_brdf, l_specular_metal * iridescenceFresnel_metallic, materialInfo.iridescenceFactor);
                l_dielectric_brdf = mix(l_dielectric_brdf, rgb_mix(l_diffuse, l_specular_dielectric, iridescenceFresnel_dielectric), materialInfo.iridescenceFactor);
        #endif

        #ifdef MATERIAL_CLEARCOAT
                l_clearcoat_brdf = intensity * getPunctualRadianceClearCoat(materialInfo.clearcoatNormal, v, l, h, VdotH,
                    materialInfo.clearcoatF0, materialInfo.clearcoatF90, materialInfo.clearcoatRoughness);
        #endif

        #ifdef MATERIAL_SHEEN
                l_sheen = intensity * getPunctualRadianceSheen(materialInfo.sheenColorFactor, materialInfo.sheenRoughnessFactor, NdotL, NdotV, NdotH);
                l_albedoSheenScaling = min(1.0 - max3(materialInfo.sheenColorFactor) * albedoSheenScalingLUT(NdotV, materialInfo.sheenRoughnessFactor),
                    1.0 - max3(materialInfo.sheenColorFactor) * albedoSheenScalingLUT(NdotL, materialInfo.sheenRoughnessFactor));
        #endif

                //temp_origBaseColor.rgb = (l_metal_brdf + l_dielectric_brdf).xyz;
                
                vec3 l_color = mix(l_dielectric_brdf, l_metal_brdf, materialInfo.metallic);
                l_color = l_sheen + l_color * l_albedoSheenScaling;
                l_color = mix(l_color, l_clearcoat_brdf, clearcoatFactor * clearcoatFresnel);
                color += l_color;
            }
        #endif // USE_PUNCTUAL        
    )"
    },
    {
        "frag_v1_chunk_04.glsl", R"(
        f_emissive = u_EmissiveFactor;
        #ifdef MATERIAL_EMISSIVE_STRENGTH
            f_emissive *= u_EmissiveStrength;
        #endif
        #ifdef HAS_EMISSIVE_MAP
            f_emissive *= texture(u_EmissiveSampler, getEmissiveUV()).rgb;
        #endif


        #ifdef MATERIAL_UNLIT
            //#ifdef HAS_EMISSIVE_MAP
            //    color = texture(u_EmissiveSampler, getEmissiveUV()).rgb;
            //#else
                color = baseColor.rgb;
            //#endif
        #elif defined(NOT_TRIANGLE) && !defined(HAS_NORMAL_VEC3)
            //Points or Lines with no NORMAL attribute SHOULD be rendered without lighting and instead use the sum of the base color value and the emissive value.
            color = f_emissive + baseColor.rgb;
        #else
            color = f_emissive * (1.0 - clearcoatFactor * clearcoatFresnel) + color;
        #endif

        //#if DEBUG == DEBUG_NONE

        #if ALPHAMODE == _MASK
            // Late discard to avoid sampling artifacts. See https://github.com/KhronosGroup/glTF-Sample-Viewer/issues/267
            if (baseColor.a < u_AlphaCutoff)
            {
                discard;
            }
            baseColor.a = 1.0;
        #endif

        #ifdef LINEAR_OUTPUT
            g_finalColor = vec4(color.rgb, baseColor.a);
        #else
            g_finalColor = vec4(toneMap(color), baseColor.a);
        #endif


            /*
        #else
            // In case of missing data for a debug view, render a checkerboard.
            g_finalColor = vec4(1.0);
            {
                float frequency = 0.02;
                float gray = 0.9;

                vec2 v1 = step(0.5, fract(frequency * gl_FragCoord.xy));
                vec2 v2 = step(0.5, vec2(1.0) - fract(frequency * gl_FragCoord.xy));
                g_finalColor.rgb *= gray + v1.x * v1.y + v2.x * v2.y;
            }
        #endif
        

        
            // Debug views:

            // Generic:

        #if DEBUG == DEBUG_UV_0 && defined(HAS_TEXCOORD_0_VEC2)
            g_finalColor.rgb = vec3(v_texcoord_0, 0);
        #endif
        #if DEBUG == DEBUG_UV_1 && defined(HAS_TEXCOORD_1_VEC2)
            g_finalColor.rgb = vec3(v_texcoord_1, 0);
        #endif
        #if DEBUG == DEBUG_NORMAL_TEXTURE && defined(HAS_NORMAL_MAP)
            g_finalColor.rgb = (normalInfo.ntex + 1.0) / 2.0;
        #endif
        #if DEBUG == DEBUG_NORMAL_SHADING
            g_finalColor.rgb = (n + 1.0) / 2.0;
        #endif
        #if DEBUG == DEBUG_NORMAL_GEOMETRY
            g_finalColor.rgb = (normalInfo.ng + 1.0) / 2.0;
        #endif
        #if DEBUG == DEBUG_TANGENT
            g_finalColor.rgb = (normalInfo.t + 1.0) / 2.0;
        #endif
        #if DEBUG == DEBUG_BITANGENT
            g_finalColor.rgb = (normalInfo.b + 1.0) / 2.0;
        #endif
        #if DEBUG == DEBUG_ALPHA
            g_finalColor.rgb = vec3(baseColor.a);
        #endif
        #if DEBUG == DEBUG_OCCLUSION && defined(HAS_OCCLUSION_MAP)
            g_finalColor.rgb = vec3(ao);
        #endif
        #if DEBUG == DEBUG_EMISSIVE
            g_finalColor.rgb = linearTosRGB(f_emissive);
        #endif


        #if DEBUG == DEBUG_METALLIC
            g_finalColor.rgb = vec3(materialInfo.metallic);
        #endif
        #if DEBUG == DEBUG_ROUGHNESS
            g_finalColor.rgb = vec3(materialInfo.perceptualRoughness);
        #endif
        #if DEBUG == DEBUG_BASE_COLOR
            g_finalColor.rgb = linearTosRGB(materialInfo.baseColor);
        #endif        
    )"
    },
    {
        "frag_v1_chunk_05.glsl", R"(
        // Clearcoat:
        #ifdef MATERIAL_CLEARCOAT
        #if DEBUG == DEBUG_CLEARCOAT_FACTOR
            g_finalColor.rgb = vec3(materialInfo.clearcoatFactor);
        #endif
        #if DEBUG == DEBUG_CLEARCOAT_ROUGHNESS
            g_finalColor.rgb = vec3(materialInfo.clearcoatRoughness);
        #endif
        #if DEBUG == DEBUG_CLEARCOAT_NORMAL
            g_finalColor.rgb = (materialInfo.clearcoatNormal + vec3(1)) / 2.0;
        #endif
        #endif

            // Sheen:
        #ifdef MATERIAL_SHEEN
        #if DEBUG == DEBUG_SHEEN_COLOR
            g_finalColor.rgb = materialInfo.sheenColorFactor;
        #endif
        #if DEBUG == DEBUG_SHEEN_ROUGHNESS
            g_finalColor.rgb = vec3(materialInfo.sheenRoughnessFactor);
        #endif
        #endif

            // Specular:
        #ifdef MATERIAL_SPECULAR
        #if DEBUG == DEBUG_SPECULAR_FACTOR
            g_finalColor.rgb = vec3(materialInfo.specularWeight);
        #endif

        #if DEBUG == DEBUG_SPECULAR_COLOR
        vec3 specularTexture = vec3(1.0);
        #ifdef HAS_SPECULAR_COLOR_MAP
            specularTexture.rgb = texture(u_SpecularColorSampler, getSpecularColorUV()).rgb;
        #endif
            g_finalColor.rgb = u_KHR_materials_specular_specularColorFactor * specularTexture.rgb;
        #endif
        #endif

            // Transmission, Volume:
        #ifdef MATERIAL_TRANSMISSION
        #if DEBUG == DEBUG_TRANSMISSION_FACTOR
            g_finalColor.rgb = vec3(materialInfo.transmissionFactor);
        #endif
        #endif
        #ifdef MATERIAL_VOLUME
        #if DEBUG == DEBUG_VOLUME_THICKNESS
            g_finalColor.rgb = vec3(materialInfo.thickness / u_ThicknessFactor);
        #endif
        #endif

            // Iridescence:
        #ifdef MATERIAL_IRIDESCENCE
        #if DEBUG == DEBUG_IRIDESCENCE_FACTOR
            g_finalColor.rgb = vec3(materialInfo.iridescenceFactor);
        #endif
        #if DEBUG == DEBUG_IRIDESCENCE_THICKNESS
            g_finalColor.rgb = vec3(materialInfo.iridescenceThickness / 1200.0);
        #endif
        #endif

            // Anisotropy:
        #ifdef MATERIAL_ANISOTROPY
        #if DEBUG == DEBUG_ANISOTROPIC_STRENGTH
            g_finalColor.rgb = vec3(materialInfo.anisotropyStrength);
        #endif
        #if DEBUG == DEBUG_ANISOTROPIC_DIRECTION
            vec2 direction = vec2(1.0, 0.0);
        #ifdef HAS_ANISOTROPY_MAP
            direction = texture(u_AnisotropySampler, getAnisotropyUV()).xy;
            direction = direction * 2.0 - vec2(1.0); // [0, 1] -> [-1, 1]
        #endif
            vec2 directionRotation = u_Anisotropy.xy; // cos(theta), sin(theta)
            mat2 rotationMatrix = mat2(directionRotation.x, directionRotation.y, -directionRotation.y, directionRotation.x);
            direction = (direction + vec2(1.0)) * 0.5; // [-1, 1] -> [0, 1]

            g_finalColor.rgb = vec3(direction, 0.0);
        #endif
        #endif

            // Diffuse Transmission:
        #ifdef MATERIAL_DIFFUSE_TRANSMISSION
        #if DEBUG == DEBUG_DIFFUSE_TRANSMISSION_FACTOR
            g_finalColor.rgb = linearTosRGB(vec3(materialInfo.diffuseTransmissionFactor));
        #endif
        #if DEBUG == DEBUG_DIFFUSE_TRANSMISSION_COLOR_FACTOR
            g_finalColor.rgb = linearTosRGB(materialInfo.diffuseTransmissionColorFactor);
        #endif
        #endif
        */
        }        
    )"
    },

    {
        "cubemap.vert", R"(
        uniform mat4 u_ViewProjectionMatrix;
        uniform mat3 u_EnvRotation;

        in vec3 a_position;
        out vec3 v_TexCoords;

        void main()
        {
            v_TexCoords = u_EnvRotation * a_position;
            mat4 mat = u_ViewProjectionMatrix;
            mat[3] = vec4(0.0, 0.0, 0.0, 0.1);
            vec4 pos = mat * vec4(a_position, 1.0);
            gl_Position = pos.xyww;
        }
    )"
    },
    {
        "cubemap.frag", R"(
        precision highp float;
#include <tonemapping.glsl>
        uniform float u_EnvIntensity;
        uniform float u_EnvBlurNormalized;
        uniform int u_MipCount;
        uniform samplerCube u_GGXEnvSampler;

        out vec4 FragColor;
        in vec3 v_TexCoords;


        void main()
        {
            vec4 color = textureLod(u_GGXEnvSampler, v_TexCoords, u_EnvBlurNormalized * float(u_MipCount - 1));
            color.rgb *= u_EnvIntensity;
            color.a = 1.0;

        #ifdef LINEAR_OUTPUT
            FragColor = color.rgba;
        #else
            FragColor = vec4(toneMap(color.rgb), color.a);
        #endif
        }

    )"
    },
};

static const lv_opengl_shader_t env_src_includes[] = {
    {
        "fullscreen.vert", R"(
        precision highp float;

        in vec2 aPosition;
        in vec2 aTexCoord;

        out vec2 texCoord;

        void main(void) 
        {
            texCoord = aTexCoord;
            gl_Position = vec4(aPosition, 0.0, 1.0);
        }
    )"
    },
    {
        "panorama_to_cubemap.frag", R"(
        #define MATH_PI 3.1415926535897932384626433832795
        #define MATH_INV_PI (1.0 / MATH_PI)

        precision highp float;

        in vec2 texCoord;
        out vec4 fragmentColor;

        uniform int u_currentFace;
        uniform sampler2D u_panorama;

        vec3 uvToXYZ(int face, vec2 uv)
        {
            if(face == 0)
                return vec3(     1.f,   uv.y,    -uv.x);

            else if(face == 1)
                return vec3(    -1.f,   uv.y,     uv.x);

            else if(face == 2)
                return vec3(   +uv.x,   -1.f,    +uv.y);

            else if(face == 3)
                return vec3(   +uv.x,    1.f,    -uv.y);

            else if(face == 4)
                return vec3(   +uv.x,   uv.y,      1.f);

            else //if(face == 5)
            {	return vec3(    -uv.x,  +uv.y,     -1.f);}
        }

        vec2 dirToUV(vec3 dir)
        {
            return vec2(
                0.5f + 0.5f * atan(dir.z, dir.x) / MATH_PI,
                1.f - acos(dir.y) / MATH_PI);
        }

        vec3 panoramaToCubeMap(int face, vec2 texCoord)
        {
            vec2 texCoordNew = texCoord*2.0-1.0;
            vec3 scan = uvToXYZ(face, texCoordNew);
            vec3 direction = normalize(scan);
            vec2 src = dirToUV(direction);

            return  texture(u_panorama, src).rgb;
        }

        void main(void)
        {
            fragmentColor = vec4(0.0, 0.0, 0.0, 1.0);

            fragmentColor.rgb = panoramaToCubeMap(u_currentFace, texCoord);
        }

    )"
    },
    {
        "ibl_filtering.frag", R"(
#include <ibl_filtering1.glsl>
#include <ibl_filtering2.glsl>
#include <ibl_filtering3.glsl>
#include <ibl_filtering4.glsl>
#include <ibl_filtering5.glsl>
#include <ibl_filtering6.glsl>
    )"
    },
    {
        "ibl_filtering1.glsl", R"(

        //#extension GL_ARB_separate_shader_objects : enable

        precision highp float;
        #define MATH_PI 3.1415926535897932384626433832795
        //#define MATH_INV_PI (1.0 / MATH_PI)

        uniform samplerCube u_cubemapTexture;

        // enum
        const int cLambertian = 0;
        const int cGGX = 1;
        const int cCharlie = 2;


        //layout(push_constant) uniform FilterParameters {
        uniform  float u_roughness;
        uniform  int u_sampleCount;
        uniform  int u_width;
        uniform  float u_lodBias;
        uniform  int u_distribution; // enum
        uniform int u_currentFace;
        uniform int u_isGeneratingLUT;

        // 0: Byte Target Texture (normalized) 
        // 1: Float Target Texture
        uniform int u_floatTexture; 

        uniform  float u_intensityScale;

        //layout (location = 0) in vec2 inUV;
        in vec2 texCoord;


        out vec4 fragmentColor;

        //layout(location = 6) out vec3 outLUT;


        vec3 uvToXYZ(int face, vec2 uv)
        {
            if(face == 0)
                return vec3(     1.f,   uv.y,    -uv.x);

            else if(face == 1)
                return vec3(    -1.f,   uv.y,     uv.x);

            else if(face == 2)
                return vec3(   +uv.x,   -1.f,    +uv.y);

            else if(face == 3)
                return vec3(   +uv.x,    1.f,    -uv.y);

            else if(face == 4)
                return vec3(   +uv.x,   uv.y,      1.f);

            else {//if(face == 5)
                return vec3(    -uv.x,  +uv.y,     -1.f);}
        }

        vec2 dirToUV(vec3 dir)
        {
            return vec2(
                    0.5f + 0.5f * atan(dir.z, dir.x) / MATH_PI,
                    1.f - acos(dir.y) / MATH_PI);
        }

        float saturate(float v)
        {
            return clamp(v, 0.0f, 1.0f);
        }

        // Hammersley Points on the Hemisphere
        // CC BY 3.0 (Holger Dammertz)
        // http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
        // with adapted interface
        float radicalInverse_VdC(uint bits)
        {
            bits = (bits << 16u) | (bits >> 16u);
            bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xAAAAAAAAu) >> 1u);
            bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xCCCCCCCCu) >> 2u);
            bits = ((bits & 0x0F0F0F0Fu) << 4u) | ((bits & 0xF0F0F0F0u) >> 4u);
            bits = ((bits & 0x00FF00FFu) << 8u) | ((bits & 0xFF00FF00u) >> 8u);
            return float(bits) * 2.3283064365386963e-10; // / 0x100000000
        }
    )"
    },
    {
        "ibl_filtering2.glsl", R"(
        // hammersley2d describes a sequence of points in the 2d unit square [0,1)^2
        // that can be used for quasi Monte Carlo integration
        vec2 hammersley2d(int i, int N) {
            return vec2(float(i)/float(N), radicalInverse_VdC(uint(i)));
        }

        // Hemisphere Sample

        // TBN generates a tangent bitangent normal coordinate frame from the normal
        // (the normal must be normalized)
        mat3 generateTBN(vec3 normal)
        {
            vec3 bitangent = vec3(0.0, 1.0, 0.0);

            float NdotUp = dot(normal, vec3(0.0, 1.0, 0.0));
            float epsilon = 0.0000001;
            if (1.0 - abs(NdotUp) <= epsilon)
            {
                // Sampling +Y or -Y, so we need a more robust bitangent.
                if (NdotUp > 0.0)
                {
                    bitangent = vec3(0.0, 0.0, 1.0);
                }
                else
                {
                    bitangent = vec3(0.0, 0.0, -1.0);
                }
            }

            vec3 tangent = normalize(cross(bitangent, normal));
            bitangent = cross(normal, tangent);

            return mat3(tangent, bitangent, normal);
        }

        struct MicrofacetDistributionSample
        {
            float pdf;
            float cosTheta;
            float sinTheta;
            float phi;
        };

        float D_GGX(float NdotH, float roughness) {
            float a = NdotH * roughness;
            float k = roughness / (1.0 - NdotH * NdotH + a * a);
            return k * k * (1.0 / MATH_PI);
        }

        // GGX microfacet distribution
        // https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.html
        // This implementation is based on https://bruop.github.io/ibl/,
        //  https://www.tobias-franke.eu/log/2014/03/30/notes_on_importance_sampling.html
        // and https://developer.nvidia.com/gpugems/GPUGems3/gpugems3_ch20.html
        MicrofacetDistributionSample GGX(vec2 xi, float roughness)
        {
            MicrofacetDistributionSample ggx;

            // evaluate sampling equations
            float alpha = roughness * roughness;
            ggx.cosTheta = saturate(sqrt((1.0 - xi.y) / (1.0 + (alpha * alpha - 1.0) * xi.y)));
            ggx.sinTheta = sqrt(1.0 - ggx.cosTheta * ggx.cosTheta);
            ggx.phi = 2.0 * MATH_PI * xi.x;

            // evaluate GGX pdf (for half vector)
            ggx.pdf = D_GGX(ggx.cosTheta, alpha);

            // Apply the Jacobian to obtain a pdf that is parameterized by l
            // see https://bruop.github.io/ibl/
            // Typically you'd have the following:
            // float pdf = D_GGX(NoH, roughness) * NoH / (4.0 * VoH);
            // but since V = N => VoH == NoH
            ggx.pdf /= 4.0;

            return ggx;
        }

        // NDF
        float D_Ashikhmin(float NdotH, float roughness)
        {
            float alpha = roughness * roughness;
            // Ashikhmin 2007, "Distribution-based BRDFs"
            float a2 = alpha * alpha;
            float cos2h = NdotH * NdotH;
            float sin2h = 1.0 - cos2h;
            float sin4h = sin2h * sin2h;
            float cot2 = -cos2h / (a2 * sin2h);
            return 1.0 / (MATH_PI * (4.0 * a2 + 1.0) * sin4h) * (4.0 * exp(cot2) + sin4h);
        }
    )"
    },
    {
        "ibl_filtering3.glsl", R"(
        // NDF
        float D_Charlie(float sheenRoughness, float NdotH)
        {
            sheenRoughness = max(sheenRoughness, 0.000001); //clamp (0,1]
            float invR = 1.0 / sheenRoughness;
            float cos2h = NdotH * NdotH;
            float sin2h = 1.0 - cos2h;
            return (2.0 + invR) * pow(sin2h, invR * 0.5) / (2.0 * MATH_PI);
        }


        MicrofacetDistributionSample Charlie(vec2 xi, float roughness)
        {
            MicrofacetDistributionSample charlie;

            float alpha = roughness * roughness;
            charlie.sinTheta = pow(xi.y, alpha / (2.0*alpha + 1.0));
            charlie.cosTheta = sqrt(1.0 - charlie.sinTheta * charlie.sinTheta);
            charlie.phi = 2.0 * MATH_PI * xi.x;

            // evaluate Charlie pdf (for half vector)
            charlie.pdf = D_Charlie(alpha, charlie.cosTheta);

            // Apply the Jacobian to obtain a pdf that is parameterized by l
            charlie.pdf /= 4.0;

            return charlie;
        }

        MicrofacetDistributionSample Lambertian(vec2 xi, float roughness)
        {
            MicrofacetDistributionSample lambertian;

            // Cosine weighted hemisphere sampling
            // http://www.pbr-book.org/3ed-2018/Monte_Carlo_Integration/2D_Sampling_with_Multidimensional_Transformations.html#Cosine-WeightedHemisphereSampling
            lambertian.cosTheta = sqrt(1.0 - xi.y);
            lambertian.sinTheta = sqrt(xi.y); // equivalent to `sqrt(1.0 - cosTheta*cosTheta)`;
            lambertian.phi = 2.0 * MATH_PI * xi.x;

            lambertian.pdf = lambertian.cosTheta / MATH_PI; // evaluation for solid angle, therefore drop the sinTheta

            return lambertian;
        }


        // getImportanceSample returns an importance sample direction with pdf in the .w component
        vec4 getImportanceSample(int sampleIndex, vec3 N, float roughness)
        {
            // generate a quasi monte carlo point in the unit square [0.1)^2
            vec2 xi = hammersley2d(sampleIndex, u_sampleCount);

            MicrofacetDistributionSample importanceSample;

            // generate the points on the hemisphere with a fitting mapping for
            // the distribution (e.g. lambertian uses a cosine importance)
            if(u_distribution == cLambertian)
            {
                importanceSample = Lambertian(xi, roughness);
            }
            else if(u_distribution == cGGX)
            {
                // Trowbridge-Reitz / GGX microfacet model (Walter et al)
                // https://www.cs.cornell.edu/~srm/publications/EGSR07-btdf.html
                importanceSample = GGX(xi, roughness);
            }
            else if(u_distribution == cCharlie)
            {
                importanceSample = Charlie(xi, roughness);
            }

            // transform the hemisphere sample to the normal coordinate frame
            // i.e. rotate the hemisphere to the normal direction
            vec3 localSpaceDirection = normalize(vec3(
                importanceSample.sinTheta * cos(importanceSample.phi), 
                importanceSample.sinTheta * sin(importanceSample.phi), 
                importanceSample.cosTheta
            ));
            mat3 TBN = generateTBN(N);
            vec3 direction = TBN * localSpaceDirection;

            return vec4(direction, importanceSample.pdf);
        }
    )"
    },
    {
        "ibl_filtering4.glsl", R"(
        // Mipmap Filtered Samples (GPU Gems 3, 20.4)
        // https://developer.nvidia.com/gpugems/gpugems3/part-iii-rendering/chapter-20-gpu-based-importance-sampling
        // https://cgg.mff.cuni.cz/~jaroslav/papers/2007-sketch-fis/Final_sap_0073.pdf
        float computeLod(float pdf)
        {
            // // Solid angle of current sample -- bigger for less likely samples
            //float omegaS = 1.0 / (float(u_sampleCount) * pdf);
            // // Solid angle of texel
            // // note: the factor of 4.0 * MATH_PI 
            //float omegaP = 4.0 * MATH_PI / (6.0 * float(u_width) * float(u_width));
            // // Mip level is determined by the ratio of our sample's solid angle to a texel's solid angle 
            // // note that 0.5 * log2 is equivalent to log4
            //float lod = 0.5 * log2(omegaS / omegaP);

            // babylon introduces a factor of K (=4) to the solid angle ratio
            // this helps to avoid undersampling the environment map
            // this does not appear in the original formulation by Jaroslav Krivanek and Mark Colbert
            // log4(4) == 1
            // lod += 1.0;

            // We achieved good results by using the original formulation from Krivanek & Colbert adapted to cubemaps

            // https://cgg.mff.cuni.cz/~jaroslav/papers/2007-sketch-fis/Final_sap_0073.pdf
            float lod = 0.5 * log2( 6.0 * float(u_width) * float(u_width) / (float(u_sampleCount) * pdf));
            //float lod = 0.5 * log2( 3.0 * float(u_width) * float(u_width) / (float(u_sampleCount) * pdf));


            return lod;
        }

        vec3 filterColor(vec3 N)
        {
            //return  textureLod(u_cubemapTexture, N, 3.0).rgb;
            vec3 color = vec3(0.f);
            float weight = 0.0f;

            for(int i = 0; i < u_sampleCount; ++i)
            {
                vec4 importanceSample = getImportanceSample(i, N, u_roughness);

                vec3 H = vec3(importanceSample.xyz);
                float pdf = importanceSample.w;

                // mipmap filtered samples (GPU Gems 3, 20.4)
                float lod = computeLod(pdf);

                // apply the bias to the lod
                lod += u_lodBias;

                if(u_distribution == cLambertian)
                {
                    // sample lambertian at a lower resolution to avoid fireflies
                    vec3 lambertian = textureLod(u_cubemapTexture, H, lod).rgb * u_intensityScale;

                    //// the below operations cancel each other out
                    // lambertian *= NdotH; // lamberts law
                    // lambertian /= pdf; // invert bias from importance sampling
                    // lambertian /= MATH_PI; // convert irradiance to radiance https://seblagarde.wordpress.com/2012/01/08/pi-or-not-to-pi-in-game-lighting-equation/

                    color += lambertian;
                }
                else if(u_distribution == cGGX || u_distribution == cCharlie)
                {
                    // Note: reflect takes incident vector.
                    vec3 V = N;
                    vec3 L = normalize(reflect(-V, H));
                    
                    float NdotL = dot(N, L);

                    if (NdotL > 0.0)
                    {
                        if(u_roughness == 0.0)
                        {
                            // without this the roughness=0 lod is too high
                            lod = u_lodBias;
                        }
                        vec3 sampleColor = textureLod(u_cubemapTexture, L, lod).rgb * u_intensityScale;
                        color += sampleColor * NdotL;
                        weight += NdotL;
                    }
                }
            }

            if(weight != 0.0f)
            {
                color /= weight;
            }
            else
            {
                color /= float(u_sampleCount);
            }

            return color.rgb ;
        }
    )"
    },
    {
        "ibl_filtering5.glsl", R"(
        // From the filament docs. Geometric Shadowing function
        // https://google.github.io/filament/Filament.html#toc4.4.2
        float V_SmithGGXCorrelated(float NoV, float NoL, float roughness) {
            float a2 = pow(roughness, 4.0);
            float GGXV = NoL * sqrt(NoV * NoV * (1.0 - a2) + a2);
            float GGXL = NoV * sqrt(NoL * NoL * (1.0 - a2) + a2);
            return 0.5 / (GGXV + GGXL);
        }

        // https://github.com/google/filament/blob/master/shaders/src/brdf.fs#L136
        float V_Ashikhmin(float NdotL, float NdotV)
        {
            return clamp(1.0 / (4.0 * (NdotL + NdotV - NdotL * NdotV)), 0.0, 1.0);
        }

        // Compute LUT for GGX distribution.
        // See https://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
        vec3 LUT(float NdotV, float roughness)
        {
            // Compute spherical view vector: (sin(phi), 0, cos(phi))
            vec3 V = vec3(sqrt(1.0 - NdotV * NdotV), 0.0, NdotV);

            // The macro surface normal just points up.
            vec3 N = vec3(0.0, 0.0, 1.0);

            // To make the LUT independant from the material's F0, which is part of the Fresnel term
            // when substituted by Schlick's approximation, we factor it out of the integral,
            // yielding to the form: F0 * I1 + I2
            // I1 and I2 are slighlty different in the Fresnel term, but both only depend on
            // NoL and roughness, so they are both numerically integrated and written into two channels.
            float A = 0.0;
            float B = 0.0;
            float C = 0.0;

            for(int i = 0; i < u_sampleCount; ++i)
            {
                // Importance sampling, depending on the distribution.
                vec4 importanceSample = getImportanceSample(i, N, roughness);
                vec3 H = importanceSample.xyz;
                // float pdf = importanceSample.w;
                vec3 L = normalize(reflect(-V, H));

                float NdotL = saturate(L.z);
                float NdotH = saturate(H.z);
                float VdotH = saturate(dot(V, H));
                if (NdotL > 0.0)
                {
                    if (u_distribution == cGGX)
                    {
                        // LUT for GGX distribution.

                        // Taken from: https://bruop.github.io/ibl
                        // Shadertoy: https://www.shadertoy.com/view/3lXXDB
                        // Terms besides V are from the GGX PDF we're dividing by.
                        float V_pdf = V_SmithGGXCorrelated(NdotV, NdotL, roughness) * VdotH * NdotL / NdotH;
                        float Fc = pow(1.0 - VdotH, 5.0);
                        A += (1.0 - Fc) * V_pdf;
                        B += Fc * V_pdf;
                        C += 0.0;
                    }

                    if (u_distribution == cCharlie)
                    {
                        // LUT for Charlie distribution.
                        float sheenDistribution = D_Charlie(roughness, NdotH);
                        float sheenVisibility = V_Ashikhmin(NdotL, NdotV);

                        A += 0.0;
                        B += 0.0;
                        C += sheenVisibility * sheenDistribution * NdotL * VdotH;
                    }
                }
            }

            // The PDF is simply pdf(v, h) -> NDF * <nh>.
            // To parametrize the PDF over l, use the Jacobian transform, yielding to: pdf(v, l) -> NDF * <nh> / 4<vh>
            // Since the BRDF divide through the PDF to be normalized, the 4 can be pulled out of the integral.
            return vec3(4.0 * A, 4.0 * B, 4.0 * 2.0 * MATH_PI * C) / float(u_sampleCount);
        }
    )"
    },
    {
        "ibl_filtering6.glsl", R"(


        // entry point
        void main()
        {
            vec3 color = vec3(0);

            if(u_isGeneratingLUT == 0)
            {
                vec2 newUV = texCoord ;

                newUV = newUV*2.0-1.0;

                vec3 scan = uvToXYZ(u_currentFace, newUV);

                vec3 direction = normalize(scan);
            
                color = filterColor(direction);
            }
            else
            {
                color = LUT(texCoord.x, texCoord.y);
                fragmentColor.rgb = color;
                fragmentColor.a = 1.0;
                return;
            }
        
            fragmentColor.a = 1.0;
            
            if(u_floatTexture == 0) 
            { 
                float maxV = max(max(color.r,color.g),color.b);   
                color /= u_intensityScale;       
                color = clamp(color, 0.0f, 1.0f);
            }
            
            fragmentColor.rgb = color;
        }
    )"
    },
    {
        "debug.frag", R"(

        precision highp float;

        in vec2 texCoord;
        out vec4 fragmentColor;
        
        uniform int u_currentFace;
        uniform samplerCube u_inputTexture;
        
        vec3 uvToXYZ(int face, vec2 uv)
        {
            if(face == 0)
                return vec3(     1.f,   uv.y,    -uv.x);
                
            else if(face == 1)
                return vec3(    -1.f,   uv.y,     uv.x);
                
            else if(face == 2)
                return vec3(   +uv.x,   -1.f,    +uv.y);		
            
            else if(face == 3)
                return vec3(   +uv.x,    1.f,    -uv.y);
                
            else if(face == 4)
                return vec3(   +uv.x,   uv.y,      1.f);
                
            else //if(face == 5)
            {	return vec3(    -uv.x,  +uv.y,     -1.f);}
        }
        
        
        void main(void)   
        {
            fragmentColor = vec4(texCoord.x*10.0, 0.0, texCoord.y*10.0, 1.0);
            vec2 newUV =texCoord;
            newUV = newUV*2.0-1.0;
        
            vec4 textureColor = vec4(0.0, 0.0, 0.0, 1.0);
        
            vec3 direction = normalize(uvToXYZ(u_currentFace, newUV.xy));
         
            textureColor = textureLod(u_inputTexture, direction,1.0);
            //textureColor = texture(u_inputTexture, texCoord);
            
            if(texCoord.x>0.1)
            {
                fragmentColor = textureColor;
            }
        
            if(texCoord.y>0.1)
            {
                fragmentColor = textureColor;
            }
        
        }
    )"
    },
};

static const char * src_vertex_shader = R"(
    uniform mat4 u_ViewProjectionMatrix;
    uniform mat4 u_ModelMatrix;
    uniform mat4 u_NormalMatrix;

    in vec3 a_position;
    out vec3 v_Position;
#include <animation1.glsl>
#include <animation2.glsl>
#include <vert_v1_chunk_00.glsl>
// CHUNK 00 -> 01
#include <vert_v1_chunk_01.glsl>
)";

static const char *src_fragment_shader = R"(

    //#define LIGHT_COUNT 0

    //
    // This fragment shader defines a reference implementation for Physically Based Shading of
    // a microfacet surface material defined by a glTF model.
    //
    // References:
    // [1] Real Shading in Unreal Engine 4
    //     http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
    // [2] Physically Based Shading at Disney
    //     http://blog.selfshadow.com/publications/s2012-shading-course/burley/s2012_pbs_disney_brdf_notes_v3.pdf
    // [3] README.md - Environment Maps
    //     https://github.com/KhronosGroup/glTF-WebGL-PBR/#environment-maps
    // [4] "An Inexpensive BRDF Model for Physically based Rendering" by Christophe Schlick
    //     https://www.cs.virginia.edu/~jdl/bib/appearance/analytic%20models/schlick94b.pdf
    // [5] "KHR_materials_clearcoat"
    //     https://github.com/KhronosGroup/glTF/tree/master/extensions/2.0/Khronos/KHR_materials_clearcoat

    precision highp float;

#include <tonemapping.glsl>
#include <textures1.glsl>
#include <textures2.glsl>
#include <textures3.glsl>
#include <textures4.glsl>
#include <functions.glsl>
#include <brdf1.glsl>
#include <brdf2.glsl>
#include <brdf3.glsl>
#include <punctual1.glsl>
#include <punctual2.glsl>
#include <ibl1.glsl>
#include <ibl2.glsl>
#include <material_info1.glsl>
#include <material_info2.glsl>
#include <material_info3.glsl>
#include <material_info4.glsl>
#include <material_info5.glsl>

    #ifdef MATERIAL_IRIDESCENCE
#include <iridescence.glsl>
    #endif

#include <frag_v1_chunk_00.glsl>

// CHUNK 00 -> 01

#include <frag_v1_chunk_01a.glsl>
#include <frag_v1_chunk_01b.glsl>

// CHUNK 01 -> 02

#include <frag_v1_chunk_02a.glsl>
#include <frag_v1_chunk_02b.glsl>

// CHUNK 02 -> 03

#include <frag_v1_chunk_03a.glsl>
#include <frag_v1_chunk_03b.glsl>

// CHUNK 03 -> 04

#include <frag_v1_chunk_04.glsl>

// CHUNK 04 -> 05

#include <frag_v1_chunk_05.glsl>

)";

static const size_t src_includes_count = sizeof src_includes / sizeof src_includes[0];

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

char* lv_gltf_view_shader_get_vertex(void) {
    return lv_opengl_shader_manager_process_includes(src_vertex_shader, src_includes, src_includes_count);
}

char* lv_gltf_view_shader_get_fragment(void) {
    return lv_opengl_shader_manager_process_includes(src_fragment_shader, src_includes, src_includes_count);
}

void lv_gltf_view_shader_get_src(lv_opengl_shader_portions_t *portions)
{
    portions->all = src_includes;
    portions->count = sizeof(src_includes) / sizeof(src_includes[0]);
}
void lv_gltf_view_shader_get_env(lv_opengl_shader_portions_t *portions)
{
    portions->all = env_src_includes;
    portions->count = sizeof(env_src_includes) / sizeof(env_src_includes[0]);
}

#endif /*LV_USE_GLTF*/
