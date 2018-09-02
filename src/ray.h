#pragma once

#include "vec3.h"

class ray {
public:
    ray() {}
    ray(const vec3& a, const vec3& b, float ti = 0) : A(a), B(b), t(ti) { }

    vec3 origin() const { return A; }
    vec3 direction() const { return B; }
    float time() const { return t; }
    vec3 point_at_parameter(float t) const { return A + t * B; }
    vec3 A;
    vec3 B;
    float t;
};
