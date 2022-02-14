#ifndef RTX_MATH_HPP
#define RTX_MATH_HPP

#include <core/math/functions.hpp>
#include <core/math/vector3.hpp>
#include <core/math/ray.hpp>
#include <core/math/linear.hpp>
#include <random>

SX_INLINE float Length(const Vector3f &vector){
    return Math::Sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z);
}

SX_INLINE Vector3f Normalize(const Vector3f &vector){
    return vector / Length(vector);
}

SX_INLINE float RandomFloat() {
    return rand() / float(RAND_MAX);
}

SX_INLINE float RandomFloat(float min, float max) {
    SX_ASSERT(min <= max);
    return min + RandomFloat() * (max - min);
}

SX_INLINE Vector3f RandomVector3f() {
    return {RandomFloat(), RandomFloat(), RandomFloat()};
}

SX_INLINE Vector3f RandomVector3f(float min, float max) {
    return {RandomFloat(min, max), RandomFloat(min, max), RandomFloat(min, max)};
}

SX_INLINE Vector3f RandomVector3fInUnitSphere() {
    float fi = RandomFloat() * Math::Pi;
    float theta = RandomFloat() * Math::Pi;
    return {
        Math::Sin(theta) * Math::Cos(fi),
        Math::Sin(theta) * Math::Sin(fi),
        Math::Cos(theta)
    };
}

#endif//RTX_MATH_HPP