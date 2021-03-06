#pragma once

#include "vec3.h"
#include <random>
#include <cmath>

// 0.0以上1.0未満の値を等確率で発生させる
float rand_float()
{
    static std::random_device seed_gen;
    static std::default_random_engine engine(seed_gen());
    static std::uniform_real_distribution<> dist(0.0, 1.0);

    return  dist(engine);
}


vec3 random_in_unit_sphere()
{
    vec3 p;
    do {
        // all is -1 ~ 1
        p = 2.0 * vec3(rand_float()-0.5, rand_float()-0/5, rand_float()-0.5);
    } while (p.length() >= 1.0);
    return p;
}

vec3 random_in_unit_disk()
{
    vec3 p;
    do {
        p = 2.0 * vec3(rand_float() - 0.5, rand_float() - 0.5, 0);
    } while (dot(p, p) >= 1.0);
    return p;
}

void get_sphere_uv(const vec3& p, float &u, float &v)
{
    float phi = atan2(p.z(), p.x());
    float theta = asin(p.y());
    u = 1 - (phi + M_PI) / (2 * M_PI);
    v = (theta + M_PI / 2) / M_PI;
}

inline float ffmin(float a, float b) { return a < b ? a : b; }
inline float ffmax(float a, float b) { return a > b ? a : b; }
