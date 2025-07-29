#include "lv_gltf_view_ibl_sampler.hpp"
#include <src/libs/gltf/lv_gltf_view/assets/test.h>

#if LV_USE_GLTF
#include <cstdint>
#include "../../../../drivers/glfw/lv_opengles_debug.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>


#include "lv_gltf_view_ibl_sampler.hpp"
#include "../../lv_gl_shader/lv_gl_shader_internal.h"

#include <src/libs/gltf/lv_gltf_view/assets/lv_gltf_view_shader.h>
#include <unistd.h> /* usleep */
#include <string>
#include <iostream>
#include <algorithm>
#include <cstring>
#include <cmath>  /* floor */

#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image/stb_image.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "../../stb_image/stb_image_resize.h"


uint8_t min(uint8_t a, uint8_t b)
{
    return (a < b) ? a : b;
}

iblSampler::iblSampler(void)
{
    //textureSize = 256;
    textureSize = 128;
    //ggxSampleCount = 1024;
    ggxSampleCount = 128;
    //lambertianSampleCount = 2048;
    lambertianSampleCount = 256;
    sheenSamplCount = 32;
    lodBias = 0.0;
    lowestMipLevel = 3;
    //lowestMipLevel = 4;
    lutResolution = 1024;
    //lutSampleCount = 512;
    lutSampleCount = 64;
    //lutResolution = 512;

    scaleValue = 1.0;

    mipmapCount = GL_NONE;
    mipmapLevels = 0;

    lambertianTextureID = GL_NONE;
    ggxTextureID = GL_NONE;
    sheenTextureID = GL_NONE;
    ggxLutTextureID = GL_NONE;
    charlieLutTextureID = GL_NONE;
    inputTextureID = GL_NONE;
    cubemapTextureID = GL_NONE;
    framebuffer = GL_NONE;
    /*lv_gltf_view_shader shader;*/
    /*lv_gltf_view_shader_get_env(&shader);*/
    shader_manager = lv_gl_shader_manager_create(env_src_includes, sizeof(env_src_includes) / sizeof(*env_src_includes),
                                                 NULL, NULL);

}

uint32_t iblSampler::internalFormat(void)
{
    return GL_RGBA8;
}

uint32_t iblSampler::textureTargetType(void)
{
    return GL_UNSIGNED_BYTE;
}

iblSampler::t_texture iblSampler::prepareTextureData(t_image * image)
{
    t_texture texture = { GL_RGB32F, GL_RGB, GL_FLOAT, NULL };
    // Reset scaling of hdrs
    scaleValue = 1.0;

    {
        texture.internalFormat = internalFormat();
        texture.format = GL_RGBA;
        texture.type =  GL_UNSIGNED_BYTE;

        uint64_t numPixels = image->dataFloatLength / 3;

        float max_value = 0.0;
        float clamped_sum = 0.0;
        float diff_sum = 0.0;
        uint32_t src = 0;
        uint32_t dst = 0;
        for(uint32_t i = 0; i < numPixels; i++) {
            float max_component = std::max(*(image->dataFloat + src + 0), *(image->dataFloat + src + 1));
            max_component = std::max(max_component, *(image->dataFloat + src + 2));
            if(max_component > 1.0) {
                diff_sum += max_component - 1.0;
            }
            clamped_sum += std::min(max_component, 1.0f);
            max_value =  std::max(max_component, max_value);
            src += 3;
            dst += 4;
        }

        float scaleFactor = 1.0;
        if(clamped_sum > 1.0) {
            // Apply global scale factor to compensate for intensity lost when clamping
            scaleFactor = (clamped_sum + diff_sum) / clamped_sum;
            std::cout << "  +--> HDR Intensity Scale = " << std::to_string(scaleFactor) << " \n";
        }

        if(max_value > 1.0) {
            std::cout << " [!]-> Environment light intensity cannot be displayed correctly on this device \n";
        }

        //texture.data = new Uint8Array(numPixels * 4);
        texture.data = (uint8_t *)malloc(numPixels * 4);
        src = 0;
        dst = 0;
        for(uint32_t i = 0; i < numPixels; ++i) {
            // copy the pixels and pad the alpha channel
            texture.data[dst + 0] = std::min(int((image->dataFloat[src + 0]) * 255), 255);
            texture.data[dst + 1] = std::min(int((image->dataFloat[src + 1]) * 255), 255);
            texture.data[dst + 2] = std::min(int((image->dataFloat[src + 2]) * 255), 255);
            // unused
            texture.data[dst + 3] = -1;
            src += 3;
            dst += 4;
        }

        scaleValue =  scaleFactor;
        return texture;
    }
}

uint32_t iblSampler::loadTextureHDR(t_image * image)
{
    t_texture texture = prepareTextureData(image);
    uint32_t textureID;
    GL_CALL(glGenTextures(1, &textureID));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, textureID));
    GL_CALL(glTexImage2D(
                GL_TEXTURE_2D, // target
                0, // level
                texture.internalFormat,
                image->width,
                image->height,
                0, // border
                texture.format, // format of the pixel data
                texture.type, // type of the pixel data
                texture.data));
    free(texture.data);
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    return textureID;
}

uint32_t iblSampler::createCubemapTexture(bool withMipmaps)
{
    uint32_t targetTexture;
    GL_CALL(glGenTextures(1, &targetTexture));
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, targetTexture));
    for(int32_t i = 0; i < 6; ++i) {
        GL_CALL(glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat(), textureSize, textureSize, 0, GL_RGBA,
                             textureTargetType(), NULL));
    }
    if(withMipmaps) {
        GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
    }
    else {
        GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    }
    GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    return targetTexture;
}

uint32_t iblSampler::createLutTexture(void)
{
    uint32_t targetTexture;
    GL_CALL(glGenTextures(1, &targetTexture));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, targetTexture));
    GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat(), lutResolution, lutResolution, 0, GL_RGBA, textureTargetType(),
                         NULL));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
    GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
    return targetTexture;
}

bool getExtension(const char * _ext_name)
{
    int32_t NumberOfExtensions;
    glGetIntegerv(GL_NUM_EXTENSIONS, &NumberOfExtensions);
    for(uint32_t i = 0; i < (uint32_t)NumberOfExtensions; i++) {
        const GLubyte * ccc = glGetStringi(GL_EXTENSIONS, i);
        if(strcmp((const char *)ccc, _ext_name) == 0) {
            return true;
        }
    }
    return false;
}

float * resizeImage(float * input, int inputWidth, int inputHeight, int newWidth, int newHeight)
{
    float * output = (float *)lv_malloc(inputWidth * inputHeight *
                                        3); //new float[newWidth * newHeight * 3]; // Assuming 3 channels (RGB)
    stbir_resize_float(input, inputWidth, inputHeight, 0, output, newWidth, newHeight, 0, 3);
    return output;
}

void iblSampler::doinit(const char * env_filename)
{
    // vv -- WebGL Naming
    if(getExtension("GL_NV_float") &&
       getExtension("GL_ARB_color_buffer_float")) {  //&& getExtension("OES_texture_float_linear")) {
        std::cout << "THIS DEVICE SUPPORTS FLOAT FORMAT TEXTURES\n";
    }
    // Native naming #2
    if(getExtension("GL_ARB_color_buffer_float") || getExtension("GL_NV_half_float")) {
        std::cout << "THIS DEVICE SUPPORTS HALF_FLOAT FORMAT TEXTURES\n";
    }

    int32_t src_width, src_height, src_nrChannels;

    float * data;
    if(env_filename != NULL) {
        data = stbi_loadf(env_filename, &src_width, &src_height, &src_nrChannels, 3);
    }
    else {
        extern uint8_t chromatic_jpg[];
        extern int chromatic_jpg_len;
        data = stbi_loadf_from_memory(chromatic_jpg, chromatic_jpg_len, &src_width, &src_height, &src_nrChannels, 3);
    }

    t_image panoramaImage = t_image();
    panoramaImage.width = src_width;
    panoramaImage.height = src_height;
    panoramaImage.dataFloatLength = panoramaImage.width * panoramaImage.height * 3;
    panoramaImage.dataFloat = (float *)calloc(panoramaImage.dataFloatLength, sizeof(float));
    for(uint64_t i = 0; i < panoramaImage.dataFloatLength; i++) {
        panoramaImage.dataFloat[i] = data[i];
    }
    stbi_image_free(data);
    inputTextureID = loadTextureHDR(&panoramaImage);
    free(panoramaImage.dataFloat);

    GL_CALL(glGenFramebuffers(1, &framebuffer));
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));

    cubemapTextureID = createCubemapTexture(true);
    lambertianTextureID = createCubemapTexture(false);
    ggxTextureID = createCubemapTexture(true);
    sheenTextureID = createCubemapTexture(true);
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, ggxTextureID));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, sheenTextureID));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
    mipmapLevels = std::floor(std::log2(textureSize)) + 1 - lowestMipLevel;
}

void iblSampler::panoramaToCubeMap(void)
{
    for(int32_t i = 0; i < 6; ++i) {

        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                                       cubemapTextureID, 0));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID));
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        while(status != GL_FRAMEBUFFER_COMPLETE) {
            status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
            std::cerr << "ENV RENDER ERROR: Framebuffer not complete - Delaying one cycle.  Status is: " << status << " but " <<
                      GL_FRAMEBUFFER_COMPLETE << " was expected." << std::endl;
        }
        GL_CALL(glViewport(0, 0, textureSize, textureSize));
        GL_CALL(glClearColor(1.0, 0.0, 0.0, 0.0));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        uint32_t frag_shader = lv_gl_shader_manager_select_shader(shader_manager, "panorama_to_cubemap.frag", nullptr, 0);
        uint32_t vert_shader = lv_gl_shader_manager_select_shader(shader_manager, "fullscreen.vert", nullptr, 0);
        lv_gl_shader_program_t * program = lv_gl_shader_manager_get_program(shader_manager, frag_shader, vert_shader);
        GLuint program_id = lv_gl_shader_program_get_id(program);
        GLint success;
        //GL_CALL(glGetShaderiv(shader->program, GL_COMPILE_STATUS, &success));
        GL_CALL(glGetProgramiv(program_id, GL_LINK_STATUS, &success));
        if(!success) {
            // Handle shader compilation error
            std::cout << "ENV RENDER ERROR: Some error compiling the cubemap shader, IBL will be corrupted.\n";
        }
        GL_CALL(glUseProgram(program_id));
        //  TEXTURE0 = active.
        GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
        // Bind texture ID to active texture
        GL_CALL(glBindTexture(GL_TEXTURE_2D, inputTextureID));
        // map shader uniform to texture unit (TEXTURE0)
        GLuint location;
        GL_CALL(location = glGetUniformLocation(program_id, "u_panorama"));
        GL_CALL(glUniform1i(location, 0)); // texture unit 0 (TEXTURE0)
        program->update_uniform_1i(program, "u_currentFace", i);
        //fullscreen triangle

        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
    }

    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID));
    GL_CALL(glGenerateMipmap(GL_TEXTURE_CUBE_MAP));
}

void iblSampler::applyFilter(
    uint32_t distribution,
    float roughness,
    uint32_t targetMipLevel,
    uint32_t targetTexture,
    uint32_t sampleCount,
    float _lodBias,
    const char * _strProgress,
    float _baseProgress)
{
    LV_UNUSED(_strProgress);
    LV_UNUSED(_baseProgress);

    uint32_t currentTextureSize = textureSize >> targetMipLevel;
    for(uint32_t i = 0; i < 6; ++i) {
        GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
        GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, targetTexture,
                                       targetMipLevel));
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, targetTexture));
        GL_CALL(glViewport(0, 0, currentTextureSize, currentTextureSize));
        GL_CALL(glClearColor(0.0, 1.0, 0.0, 0.0));
        GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

        uint32_t frag_shader = lv_gl_shader_manager_select_shader(shader_manager, "ibl_filtering.frag", nullptr, 0);
        uint32_t vert_shader = lv_gl_shader_manager_select_shader(shader_manager, "fullscreen.vert", nullptr, 0);
        lv_gl_shader_program_t * program = lv_gl_shader_manager_get_program(shader_manager, frag_shader, vert_shader);
        GLuint program_id = lv_gl_shader_program_get_id(program);

        GL_CALL(glUseProgram(program_id));
        //  TEXTURE0 = active.
        GL_CALL(glActiveTexture(GL_TEXTURE0));
        // Bind texture ID to active texture
        GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID));
        // map shader uniform to texture unit (TEXTURE0)
        uint32_t location = glGetUniformLocation(program_id, "u_cubemapTexture");
        GL_CALL(glUniform1i(location, 0)); // texture unit 0
        program->update_uniform_1f(program, "u_roughness", roughness);
        program->update_uniform_1i(program, "u_sampleCount", sampleCount);
        //shader->update_uniform_1i(shader, "u_width", currentTextureSize);  // Software rendered mode looks better with this and horrible with below
        program->update_uniform_1i(program, "u_width",
                                   textureSize);  // Standard mode looks best with this and somewhat worse with above
        program->update_uniform_1f(program, "u_lodBias", _lodBias);
        program->update_uniform_1i(program, "u_distribution", distribution);
        program->update_uniform_1i(program, "u_currentFace", i);
        program->update_uniform_1i(program, "u_isGeneratingLUT", 0);
        program->update_uniform_1i(program, "u_floatTexture", 0);
        program->update_uniform_1f(program, "u_intensityScale", scaleValue);
        //fullscreen triangle
        GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));
        //    usleep(199000);

    }


}

void iblSampler::cubeMapToLambertian(void)
{
    applyFilter(
        0,
        0.0,
        0,
        lambertianTextureID,
        lambertianSampleCount,
        0.0,
        "Processing Lambertian filter...",
        6.f
    );
}

void iblSampler::cubeMapToGGX(void)
{
    for(uint32_t currentMipLevel = 0; currentMipLevel <= mipmapLevels; ++currentMipLevel) {
        float roughness = (float)(currentMipLevel) / (float)(mipmapLevels - 1);
        applyFilter(
            1,
            roughness,
            currentMipLevel,
            ggxTextureID,
            ggxSampleCount,
            0.0,
            "Processing GGX filter...",
            (currentMipLevel == 0) ? 12.f : -1.f
        );
    }
}

void iblSampler::cubeMapToSheen(void)
{
    for(uint32_t currentMipLevel = 0; currentMipLevel <= mipmapLevels; ++currentMipLevel) {
        float roughness = (float)(currentMipLevel) / (float)(mipmapLevels - 1);
        applyFilter(
            2,
            roughness,
            currentMipLevel,
            sheenTextureID,
            sheenSamplCount,
            0.0,
            "Processing Sheen filter...",
            (currentMipLevel == 0) ? 18.f : -1.f
        );
    }
}

void iblSampler::sampleLut(uint32_t distribution, uint32_t targetTexture, uint32_t currentTextureSize)
{
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, framebuffer));
    GL_CALL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, targetTexture, 0));
    GL_CALL(glBindTexture(GL_TEXTURE_2D, targetTexture));
    GL_CALL(glViewport(0, 0, currentTextureSize, currentTextureSize));
    GL_CALL(glClearColor(0.0, 1.0, 1.0, 0.0));
    GL_CALL(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

    uint32_t frag_shader = lv_gl_shader_manager_select_shader(shader_manager, "ibl_filtering.frag", nullptr, 0);
    uint32_t vert_shader = lv_gl_shader_manager_select_shader(shader_manager, "fullscreen.vert", nullptr, 0);
    lv_gl_shader_program_t * program = lv_gl_shader_manager_get_program(shader_manager, frag_shader, vert_shader);
    GLuint program_id = lv_gl_shader_program_get_id(program);

    GL_CALL(glUseProgram(program_id));
    //  TEXTURE0 = active.
    GL_CALL(glActiveTexture(GL_TEXTURE0 + 0));
    // Bind texture ID to active texture
    GL_CALL(glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureID));
    // map shader uniform to texture unit (TEXTURE0)
    uint32_t location = glGetUniformLocation(program_id, "u_cubemapTexture");
    GL_CALL(glUniform1i(location, 0)); // texture unit 0
    program->update_uniform_1f(program, "u_roughness", 0.0);
    program->update_uniform_1i(program, "u_sampleCount", lutSampleCount);
    //shader->update_uniform_1i( shader, "u_sampleCount", 512);
    program->update_uniform_1i(program, "u_width", 0.0);
    program->update_uniform_1f(program, "u_lodBias", 0.0);
    program->update_uniform_1i(program, "u_distribution", distribution);
    program->update_uniform_1i(program, "u_currentFace", 0);
    program->update_uniform_1i(program, "u_isGeneratingLUT", 1);
    //fullscreen triangle
    GL_CALL(glDrawArrays(GL_TRIANGLES, 0, 3));

}

void iblSampler::sampleGGXLut(void)
{
    ggxLutTextureID = createLutTexture();
    sampleLut(1, ggxLutTextureID, lutResolution);
}

void iblSampler::sampleCharlieLut(void)
{
    charlieLutTextureID = createLutTexture();
    sampleLut(2, charlieLutTextureID, lutResolution);
}

void iblSampler::filterAll()
{
    GLint previousFramebuffer;
    GL_CALL(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &previousFramebuffer));

    panoramaToCubeMap();
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer));

    cubeMapToLambertian();

    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer));

    cubeMapToGGX();
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer));

    cubeMapToSheen();
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer));

    sampleGGXLut();
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer));

    sampleCharlieLut();
    GL_CALL(glBindFramebuffer(GL_FRAMEBUFFER, previousFramebuffer));
}

void iblSampler::destroy_iblSampler(void)
{
    lv_gl_shader_manager_destroy(shader_manager);
}

// ------------------------------------------------------------

lv_gltf_view_env_textures_t lv_gltf_view_ibl_sampler_setup(lv_gltf_view_env_textures_t * last_env,
                                                           const char * env_file_path,
                                                           int32_t env_rotation_x10)
{
    lv_gltf_view_env_textures_t _ret;
    if((last_env != NULL) && (last_env->loaded == true)) {
        _ret.loaded = true;
        _ret.angle = last_env->angle;
        _ret.diffuse = last_env->diffuse;
        _ret.specular = last_env->specular;
        _ret.sheen = last_env->sheen;
        _ret.ggxLut = last_env->ggxLut;
        _ret.charlieLut = last_env->charlieLut;
        _ret.mipCount = last_env->mipCount;
        _ret.iblIntensityScale = last_env->iblIntensityScale;
        return _ret;
    }
    auto environmentFiltering = iblSampler();
    environmentFiltering.doinit(env_file_path);
    environmentFiltering.filterAll();

    _ret.loaded             = true;
    _ret.angle = (float)env_rotation_x10 / 10.0f;
    _ret.diffuse            = environmentFiltering.lambertianTextureID;
    _ret.specular           = environmentFiltering.ggxTextureID;
    _ret.sheen              = environmentFiltering.sheenTextureID;
    _ret.ggxLut             = environmentFiltering.ggxLutTextureID;
    _ret.charlieLut         = environmentFiltering.charlieLutTextureID;
    _ret.mipCount           = environmentFiltering.mipmapLevels;
    _ret.iblIntensityScale  = environmentFiltering.scaleValue;

    environmentFiltering.destroy_iblSampler();
    return _ret;
}

#endif /*LV_USE_GLTF*/
