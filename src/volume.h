#pragma once

#include "material.h"
#include "hitable.h"

#include <iostream>

class constant_medium : public hitable {
public:
    constant_medium(hitable* b, float d, texture* a) : boundary(b), density(d) {
        phase_function = new isotropic(a);
    }
    bool bounding_box(float t0, float t1, aabb& box) const {
        return boundary->bounding_box(t0, t1, box);
    }
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        bool db = rand_float() < 0.00001;
        db = false; // debug setting
        hit_record rec1, rec2;
        // rec1.t = 最初にあたる場所
        // rec2.t = 次にあたる場所
        if (boundary->hit(r, std::numeric_limits<float>::lowest(), std::numeric_limits<float>::max(), rec1)) {
            if (boundary->hit(r, rec1.t + 0.0001f, std::numeric_limits<float>::max(), rec2)) {
                if (db)
                    std::cerr << "t0 = " << rec1.t << " " << "t1 = " << rec2.t << std::endl;
                rec1.t = std::max(rec1.t, t_min);
                rec2.t = std::min(rec2.t, t_max);
                if (rec1.t >= rec2.t)
                    return false;

                rec1.t = std::max(rec1.t, 0.0f);
                float distance_inside_boundary = (rec2.t - rec1.t) * r.direction().length();
                float hit_distance = -(1 / density) * log(rand_float());
                if (hit_distance < distance_inside_boundary) {
                    rec.t = rec1.t + hit_distance / r.direction().length();
                    rec.p = r.point_at_parameter(rec.t);
                    rec.normal = vec3(1, 0, 0); // arbitrary
                    rec.mat_ptr = phase_function;
                    if (db) {
                        std::cerr << "hit_distance = " << hit_distance << std::endl;
                        std::cerr << "rec.t = " << rec.t << std::endl;
                        std::cerr << "rec.p = " << rec.p << std::endl;
                    }
                    return true;
                }
            }
        }
        return false;
    }
    hitable* boundary;
    float density;
    material* phase_function;
};

