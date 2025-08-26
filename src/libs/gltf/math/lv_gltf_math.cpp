/**
 * @file lv_gltf_math.cpp
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_gltf_math.hpp"

#if LV_USE_GLTF

#include <fastgltf/math.hpp>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/** Creates a right-handed view matrix */
fastgltf::math::fmat4x4 lv_gltf_math_look_at_rh(const fastgltf::math::fvec3 & eye, const fastgltf::math::fvec3 & center,
                                                const fastgltf::math::fvec3 & up) noexcept
{
    auto dir = normalize(center - eye);
    auto lft = normalize(cross(dir, up));
    auto rup = cross(lft, dir);

    fastgltf::math::fmat4x4 ret(1.f);
    ret.col(0) = { lft.x(), rup.x(), -dir.x(), 0.f };
    ret.col(1) = { lft.y(), rup.y(), -dir.y(), 0.f };
    ret.col(2) = { lft.z(), rup.z(), -dir.z(), 0.f };
    ret.col(3) = { -dot(lft, eye), -dot(rup, eye), dot(dir, eye), 1.f };
    return ret;
}

/**
 * Creates a right-handed perspective matrix, with the near and far clips at -1 and +1, respectively.
 * @param fov The FOV in radians
 */
[[nodiscard]] fastgltf::math::fmat4x4 lv_gltf_math_perspective_rh(float fov, float ratio, float z_near,
                                                                  float z_far) noexcept
{
    fastgltf::math::fmat4x4 ret(0.f);
    auto tanHalfFov = std::tan(fov / 2.f);
    ret.col(0).x() = 1.f / (ratio * tanHalfFov);
    ret.col(1).y() = 1.f / tanHalfFov;
    ret.col(2).z() = -(z_far + z_near) / (z_far - z_near);
    ret.col(2).w() = -1.f;
    ret.col(3).z() = -(2.f * z_far * z_near) / (z_far - z_near);
    return ret;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif /*LV_USE_GLTF*/
