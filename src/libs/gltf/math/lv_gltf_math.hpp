/**
 * @file lv_gltf_math.hpp
 * @brief GLTF math utilities and helper functions
 */

#ifndef LV_GLTF_MATH_HPP
#define LV_GLTF_MATH_HPP

/*********************
 *      INCLUDES
 *********************/

#include "../../../lvgl_public.h"

#if LV_USE_GLTF

#include <fastgltf/math.hpp>

/*********************
 *      DEFINES
 *********************/

#ifndef M_PI
    #define M_PI 3.14159265358979323846264338327950288
#endif

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

fastgltf::math::fmat4x4 lv_gltf_math_look_at_rh(const fastgltf::math::fvec3 & eye, const fastgltf::math::fvec3 & center,
                                                const fastgltf::math::fvec3 & up) noexcept;
fastgltf::math::fmat4x4 lv_gltf_math_perspective_rh(float fov, float ratio, float z_near, float z_far) noexcept;

/**
 * Build a rotation quaternion from Euler angles given in radians.
 * The angles are the rotations around the X, Y and Z axis, in that order.
 */
template <typename T>
[[nodiscard]] fastgltf::math::quat<T> lv_gltf_math_euler_to_quaternion(T x, T y, T z)
{
    T H = T(0.5);
    x *= H;
    y *= H;
    z *= H;
    T cx = cos(x), sx = sin(x), cy = cos(y), sy = sin(y), cz = cos(z), sz = sin(z);
    T cx_cy = cx * cy, sy_sz = sy * sz, sx_cy = sx * cy, sy_cz = sy * cz;
    return fastgltf::math::quat<T>(
               sx_cy * cz - cx * sy_sz, // X
               cx * sy_cz + sx_cy * sz, // Y
               cx_cy * sz - sx * sy_cz, // Z
               cx_cy * cz + sx * sy_sz  // W
           );
}

/**
 * Decompose a rotation quaternion into Euler angles in radians.
 * The result holds the rotations around the X, Y and Z axis, in that order,
 * so that it can be passed back to lv_gltf_math_euler_to_quaternion().
 */
template <typename T>
[[nodiscard]] fastgltf::math::vec<T, 3> lv_gltf_math_quaternion_to_euler(fastgltf::math::quat<T> q)
{
    T Q11 = q[1] * q[1];
    /* Rotation around X */
    T sinx_cosy = T(2.0) * (q[3] * q[0] + q[1] * q[2]);
    T cosx_cosy = T(1.0) - T(2.0) * (q[0] * q[0] + Q11);
    /* Rotation around Y */
    T siny = T(2.0) * (q[3] * q[1] - q[2] * q[0]);
    /* Rotation around Z */
    T sinz_cosy = T(2.0) * (q[3] * q[2] + q[0] * q[1]);
    T cosz_cosy = T(1.0) - T(2.0) * (Q11 + q[2] * q[2]);

    return fastgltf::math::vec<T, 3>(
               std::atan2(sinx_cosy, cosx_cosy),
               (std::abs(siny) >= T(1)) ? std::copysign(T(M_PI) / T(2), siny) : std::asin(siny),
               std::atan2(sinz_cosy, cosz_cosy)
           );
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/
#endif /*LV_GLTF_MATH_HPP*/
