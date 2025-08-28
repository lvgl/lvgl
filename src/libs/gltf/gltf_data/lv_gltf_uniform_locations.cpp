
#include "lv_gltf_data_internal.hpp"
#if LV_USE_GLTF

lv_gltf_uniform_locations_t lv_gltf_uniform_locations_create(GLuint program)
{
    lv_gltf_uniform_locations_t uniforms;
    lv_memset(&uniforms, 0, sizeof(uniforms));

    // *** IMAGE QUALITY UNIFORMS ***********************************************************************
    uniforms.exposure = glGetUniformLocation(program, "u_Exposure");
    // *** CAMERA/VIEW/PROJECTION/MODEL MATRIX UNIFORMS *************************************************
    uniforms.camera = glGetUniformLocation(program, "u_Camera");
    uniforms.model_matrix = glGetUniformLocation(program, "u_ModelMatrix");
    uniforms.view_projection_matrix = glGetUniformLocation(program, "u_ViewProjectionMatrix");
    uniforms.view_matrix = glGetUniformLocation(program, "u_ViewMatrix");
    uniforms.projection_matrix = glGetUniformLocation(program, "u_ProjectionMatrix");
    // *** IMAGE BASED LIGHTING (IBL) UNIFORMS **********************************************************
    uniforms.env_intensity = glGetUniformLocation(program, "u_EnvIntensity");
    uniforms.env_diffuse_sampler = glGetUniformLocation(program, "u_LambertianEnvSampler");
    uniforms.env_specular_sampler = glGetUniformLocation(program, "u_GGXEnvSampler");
    uniforms.env_sheen_sampler = glGetUniformLocation(program, "u_CharlieEnvSampler");
    uniforms.env_ggx_lut_sampler = glGetUniformLocation(program, "u_GGXLUT");
    uniforms.env_charlie_lut_sampler = glGetUniformLocation(program, "u_CharlieLUT");
    uniforms.env_mip_count = glGetUniformLocation(program, "u_MipCount");
    // *** BASE COLOR / TEXTURE UNIFORMS ****************************************************************
    uniforms.base_color_factor = glGetUniformLocation(program, "u_BaseColorFactor");
    uniforms.base_color_sampler = glGetUniformLocation(program, "u_BaseColorSampler");
    uniforms.base_color_uv_set = glGetUniformLocation(program, "u_BaseColorUVSet");
    uniforms.base_color_uv_transform = glGetUniformLocation(program, "u_BaseColorUVTransform");
    // *** CUTOFF / IOR / DISPERSION UNIFORMS ***********************************************************
    uniforms.alpha_cutoff = glGetUniformLocation(program, "u_AlphaCutoff");
    uniforms.ior = glGetUniformLocation(program, "u_Ior");
    uniforms.dispersion = glGetUniformLocation(program, "u_Dispersion");
    // *** METALLIC / ROUGHNESS UNIFORMS ****************************************************************
    uniforms.metallic_factor = glGetUniformLocation(program, "u_MetallicFactor");
    uniforms.roughness_factor = glGetUniformLocation(program, "u_RoughnessFactor");
    uniforms.metallic_roughness_sampler = glGetUniformLocation(program, "u_MetallicRoughnessSampler");
    uniforms.metallic_roughness_uv_set = glGetUniformLocation(program, "u_MetallicRoughnessUVSet");
    uniforms.metallic_roughness_uv_transform = glGetUniformLocation(program, "u_MetallicRoughnessUVTransform");
    // *** EMISSION UNIFORMS ****************************************************************************
    uniforms.emissive_factor = glGetUniformLocation(program, "u_EmissiveFactor");
    uniforms.emissive_sampler = glGetUniformLocation(program, "u_EmissiveSampler");
    uniforms.emissive_uv_set = glGetUniformLocation(program, "u_EmissiveUVSet");
    uniforms.emissive_uv_transform = glGetUniformLocation(program, "u_EmissiveUVTransform");
    uniforms.emissive_strength = glGetUniformLocation(program, "u_EmissiveStrength");
    // *** OCCLUSION UNIFORMS ***************************************************************************
    uniforms.occlusion_strength = glGetUniformLocation(program, "u_OcclusionStrength");
    uniforms.occlusion_sampler = glGetUniformLocation(program, "u_OcclusionSampler");
    uniforms.occlusion_uv_set = glGetUniformLocation(program, "u_OcclusionUVSet");
    uniforms.occlusion_uv_transform = glGetUniformLocation(program, "u_OcclusionUVTransform");
    // *** NORMAL MAP UNIFORMS **************************************************************************
    uniforms.normal_sampler = glGetUniformLocation(program, "u_NormalSampler");
    uniforms.normal_scale = glGetUniformLocation(program, "u_NormalScale");
    uniforms.normal_uv_set = glGetUniformLocation(program, "u_NormalUVSet");
    uniforms.normal_uv_transform = glGetUniformLocation(program, "u_NormalUVTransform");
    // *** VOLUME / TRANSMISSION UNIFORMS ***************************************************************
    uniforms.attenuation_distance = glGetUniformLocation(program, "u_AttenuationDistance");
    uniforms.attenuation_color = glGetUniformLocation(program, "u_AttenuationColor");
    uniforms.transmission_factor = glGetUniformLocation(program, "u_TransmissionFactor");
    uniforms.transmission_sampler = glGetUniformLocation(program, "u_TransmissionSampler");
    uniforms.transmission_uv_set = glGetUniformLocation(program, "u_TransmissionUVSet");
    uniforms.transmission_uv_transform = glGetUniformLocation(program, "u_TransmissionUVTransform");
    uniforms.transmission_framebuffer_sampler = glGetUniformLocation(program, "u_TransmissionFramebufferSampler");
    uniforms.transmission_framebuffer_size = glGetUniformLocation(program, "u_TransmissionFramebufferSize");
    uniforms.screen_size = glGetUniformLocation(program, "u_ScreenSize");
    uniforms.thickness = glGetUniformLocation(program, "u_ThicknessFactor");
    uniforms.thickness_sampler = glGetUniformLocation(program, "u_ThicknessSampler");
    uniforms.thickness_uv_set = glGetUniformLocation(program, "u_ThicknessUVSet");
    uniforms.thickness_uv_transform = glGetUniformLocation(program, "u_ThicknessUVTransform");
    // *** CLEARCOAT UNIFORMS ***************************************************************************
    uniforms.clearcoat_factor = glGetUniformLocation(program, "u_ClearcoatFactor");
    uniforms.clearcoat_roughness_factor = glGetUniformLocation(program, "u_ClearcoatRoughnessFactor");
    uniforms.clearcoat_sampler = glGetUniformLocation(program, "u_ClearcoatSampler");
    uniforms.clearcoat_uv_set = glGetUniformLocation(program, "u_ClearcoatUVSet");
    uniforms.clearcoat_uv_transform = glGetUniformLocation(program, "u_ClearcoatUVTransform");
    uniforms.clearcoat_roughness_sampler = glGetUniformLocation(program, "u_ClearcoatRoughnessSampler");
    uniforms.clearcoat_roughness_uv_set = glGetUniformLocation(program, "u_ClearcoatRoughnessUVSet");
    uniforms.clearcoat_roughness_uv_transform = glGetUniformLocation(program, "u_ClearcoatRoughnessUVTransform");
    uniforms.clearcoat_normal_scale = glGetUniformLocation(program, "u_ClearcoatNormalScale");
    uniforms.clearcoat_normal_sampler = glGetUniformLocation(program, "u_ClearcoatNormalSampler");
    uniforms.clearcoat_normal_uv_set = glGetUniformLocation(program, "u_ClearcoatNormalUVSet");
    uniforms.clearcoat_normal_uv_transform = glGetUniformLocation(program, "u_ClearcoatNormalUVTransform");
    // *** DIFFUSE TRANSMISSION UNIFORMS ****************************************************************
    uniforms.diffuse_transmission_factor = glGetUniformLocation(program, "u_DiffuseTransmissionFactor");
    uniforms.diffuse_transmission_sampler = glGetUniformLocation(program, "u_DiffuseTransmissionSampler");
    uniforms.diffuse_transmission_uv_set = glGetUniformLocation(program, "u_DiffuseTransmissionUVSet");
    uniforms.diffuse_transmission_uv_transform = glGetUniformLocation(program, "u_DiffuseTransmissionUVTransform");
    uniforms.diffuse_transmission_color_factor = glGetUniformLocation(program, "u_DiffuseTransmissionColorFactor");
    uniforms.diffuse_transmission_color_sampler = glGetUniformLocation(program, "u_DiffuseTransmissionColorSampler");
    uniforms.diffuse_transmission_color_uv_set = glGetUniformLocation(program, "u_DiffuseTransmissionColorUVSet");
    uniforms.diffuse_transmission_color_uv_transform = glGetUniformLocation(program,
                                                                            "u_DiffuseTransmissionColorUVTransform");
    // *** LEGACY SUPPORT - PBR_SPECULARGLOSS ***********************************************************
    uniforms.diffuse_factor = glGetUniformLocation(program, "u_DiffuseFactor");
    uniforms.specular_factor = glGetUniformLocation(program, "u_SpecularFactor");
    uniforms.glossiness_factor = glGetUniformLocation(program, "u_GlossinessFactor");
    uniforms.diffuse_sampler = glGetUniformLocation(program, "u_DiffuseSampler");
    uniforms.diffuse_uv_set = glGetUniformLocation(program, "u_DiffuseUVSet");
    uniforms.diffuse_uv_transform = glGetUniformLocation(program, "u_DiffuseUVTransform");
    uniforms.specular_glossiness_sampler = glGetUniformLocation(program, "u_SpecularGlossinessSampler");
    uniforms.specular_glossiness_uv_set = glGetUniformLocation(program, "u_SpecularGlossinessUVSet");
    uniforms.specular_glossiness_uv_transform = glGetUniformLocation(program, "u_SpecularGlossinessUVTransform");
    // *** [PARTIALLY SUPPORTED / IN DEVELOPMENT] UNIFORMS **********************************************
    uniforms.sheen_color_factor = glGetUniformLocation(program, "u_SheenColorFactor");
    uniforms.sheen_roughness_factor = glGetUniformLocation(program, "u_SheenRoughnessFactor");
    //
    uniforms.specular_color_factor = glGetUniformLocation(program, "u_KHR_materials_specular_specularColorFactor");
    uniforms.specular_factor = glGetUniformLocation(program, "u_KHR_materials_specular_specularFactor");
    //
    uniforms.joints_sampler = glGetUniformLocation(program, "u_jointsSampler");
    return uniforms;
}

#endif /*LV_USE_GLTF*/
