#pragma once

#include "vec3.h"
#include <random>

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
