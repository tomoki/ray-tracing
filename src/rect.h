#pragma once

#include "hitable.h"

class xy_rect : public hitable {
public:
    xy_rect(float _x0, float _y0, float _x1, float _y1, float _z, material* mat)
        : x0(_x0), y0(_y0), x1(_x1), y1(_y1), z(_z), mat_ptr(mat) { }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        float t = (z - r.origin().z()) / r.direction().z();
        if (t < t_min || t_max < t)
            return false;
        float x = r.origin().x() + t * r.direction().x();
        float y = r.origin().y() + t * r.direction().y();
        if (x < x0 || x1 < x || y < y0 || y1 < y)
            return false;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (y - y0) / (y1 - y0);
        rec.t = t;
        rec.mat_ptr = mat_ptr;
        rec.p = r.point_at_parameter(t);
        rec.normal = vec3(0, 0, 1);
        return true;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        box = aabb(vec3(x0, y0, z - 0.0001), vec3(x1, y1, z + 0.0001));
        return true;
    }

    material* mat_ptr;
    float x0, y0, x1, y1, z;
};
