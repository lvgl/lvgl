/**
 * @file lv_gltf_math.hpp
 * @brief GLTF math utilities and helper functions
 */

#ifndef LV_GLTF_MATH_HPP
#define LV_GLTF_MATH_HPP

/*********************
 *      INCLUDES
 *********************/

#include "../../../lv_conf_internal.h"

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

fastgltf::math::fmat4x4 lv_gltf_math_look_at_rh(const fastgltf::math::fvec3 & eye, const fastgltf::math::fvec3 & center, const fastgltf::math::fvec3 & up) noexcept;
fastgltf::math::fmat4x4 lv_gltf_math_perspective_rh(float fov, float ratio, float z_near, float z_far) noexcept;

template <typename T>
[[nodiscard]] fastgltf::math::quat<T> lv_gltf_math_euler_to_quaternion(T P, T Y, T R)
{
    // Convert degrees to radians if necessary
    // roll = roll * (M_PI / 180.0);
    // pitch = pitch * (M_PI / 180.0);
    // yaw = yaw * (M_PI / 180.0);
    T H = T(0.5);
    Y *= H;
    P *= H;
    R *= H;
    T cy = cos(Y), sy = sin(Y), cp = cos(P), sp = sin(P), cr = cos(R), sr = sin(R);
    T cr_cp = cr * cp, sp_sy = sp * sy, sr_cp = sr * cp, sp_cy = sp * cy;
    return fastgltf::math::quat<T>(
               sr_cp * cy - cr * sp_sy, // X
               sr_cp * sy + cr * sp_cy, // Y
               cr_cp * sy - sr * sp_cy, // Z
               cr_cp * cy + sr * sp_sy  // W
           );
}

template <typename T>
[[nodiscard]] fastgltf::math::vec<T, 3> lv_gltf_math_quaternion_to_euler(fastgltf::math::quat<T> q)
{
    T Q11 = q[1] * q[1];
    // Roll (Z)
    T sinr_cosp = T(2.0) * (q[3] * q[0] + q[1] * q[2]);
    T cosr_cosp = T(1.0) - T(2.0) * (q[0] * q[0] + Q11);
    // Pitch (X)
    T sinp = T(2.0) * (q[3] * q[1] - q[2] * q[0]);
    // Yaw (Y)
    T siny_cosp = T(2.0) * (q[3] * q[2] + q[0] * q[1]);
    T cosy_cosp = T(1.0) - T(2.0) * (Q11 + q[2] * q[2]);

    return fastgltf::math::vec<T, 3>(
               (std::abs(sinp) >= T(1)) ? std::copysign(T(M_PI) / T(2), sinp) : std::asin(sinp),
               std::atan2(siny_cosp, cosy_cosp),
               std::atan2(sinr_cosp, cosr_cosp)
           );
}

/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_GLTF*/
#endif /*LV_GLTF_MATH_HPP*/
