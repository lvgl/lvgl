#ifndef IBL_SAMPLER_H
#define IBL_SAMPLER_H

#include "../lv_gltf_view_internal.hpp"
#if LV_USE_GLTF
#include "../../lv_gl_shader/lv_gl_shader_internal.h"
#include "../../../../misc/lv_types.h"

class iblSampler
{
        uint32_t textureSize;
        uint32_t ggxSampleCount;
        uint32_t lambertianSampleCount;
        uint32_t sheenSamplCount;
        uint32_t lutSampleCount;

        float lodBias;
        uint32_t lowestMipLevel;
        uint32_t lutResolution;

        uint32_t inputTextureID;
        uint32_t cubemapTextureID;
        uint32_t framebuffer;
        uint32_t mipmapCount;

        lv_gl_shader_manager_t * shader_manager;

        struct t_texture {
            uint32_t internalFormat;
            uint32_t format;
            uint32_t type;
            uint8_t * data;
        };

        struct t_image {
            uint32_t width;
            uint32_t height;
            float * dataFloat;
            uint64_t dataFloatLength;
        };

        uint32_t internalFormat(void);
        uint32_t textureTargetType(void);
        t_texture prepareTextureData(t_image * image);
        uint32_t loadTextureHDR(t_image * image);
        uint32_t createCubemapTexture(bool withMipmaps);
        uint32_t createLutTexture(void);
        void panoramaToCubeMap(void);
        void applyFilter(uint32_t distribution, float roughness, uint32_t targetMipLevel, uint32_t targetTexture,
                         uint32_t sampleCount, float lodBias, const char * _strProgress, float _baseProgress);
        void cubeMapToLambertian(void);
        void cubeMapToGGX(void);
        void cubeMapToSheen(void);
        void sampleLut(uint32_t distribution, uint32_t targetTexture, uint32_t currentTextureSize);
        void sampleGGXLut(void);
        void sampleCharlieLut(void);

    public:

        uint32_t lambertianTextureID;
        uint32_t ggxTextureID;
        uint32_t sheenTextureID;
        uint32_t ggxLutTextureID;
        uint32_t charlieLutTextureID;
        float scaleValue;
        uint32_t mipmapLevels;
        iblSampler(void);
        void doinit(const char * env_filename);
        void filterAll(void (*callback)(const char *, float, float));
        void destroy_iblSampler(void);
};

void lv_gltf_view_ibl_set_loadphase_callback(void (*_load_progress_callback)(const char *, const char *, float, float,
                                                                             float, float));

lv_gltf_view_env_textures_t lv_gltf_view_ibl_sampler_setup(lv_gltf_view_env_textures_t * last_env,
                                                                   const char * env_file_path,
                                                                   int32_t env_rotation_x10);

#endif /*LV_USE_GLTF*/
#endif /*IBL_SAMPLER_H*/
