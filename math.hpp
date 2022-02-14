#ifndef RTX_MATH_HPP
#define RTX_MATH_HPP

#include <core/math/functions.hpp>
#include <core/math/vector3.hpp>
#include <core/math/ray.hpp>
#include <core/math/linear.hpp>

SX_INLINE float Length(const Vector3f &vector){
    return Math::Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

SX_INLINE Vector3f Normalize(const Vector3f &vector){
    return vector / Length(vector);
}

#endif//RTX_MATH_HPP