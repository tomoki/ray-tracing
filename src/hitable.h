#pragma once

#include "aabb.h"
#include "ray.h"

#include <algorithm>
#include <limits>

class material;

struct hit_record {
    float t;
    vec3 p;
    vec3 normal;
    material *mat_ptr;
    // texture coordinates
    float u = 0;
    float v = 0;
};

class hitable {
public:
    virtual bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const = 0;
    virtual bool bounding_box(float t0, float t1, aabb& box) const = 0;
};

class flip_normals : public hitable {
public:
    flip_normals(hitable* p) : ptr(p) { }
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        if (ptr->hit(r, t_min, t_max, rec)) {
            rec.normal = -rec.normal;
            return true;
        }
        return false;
    }
    bool bounding_box(float t0, float t1, aabb& box) const {
        return ptr->bounding_box(t0, t1, box);
    }
    hitable* ptr;
};

class translate : public hitable {
public:
    translate(hitable* p, const vec3& displacement) : ptr(p), offset(displacement) { }
    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        ray moved(r.origin() - offset, r.direction(), r.time());
        if (ptr->hit(moved, t_min, t_max, rec)) {
            rec.p += offset;
            return true;
        }
        return false;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        if (ptr->bounding_box(t0, t1, box)) {
            box = aabb(box.min() + offset, box.max() + offset);
            return true;
        }
        return false;
    }

    hitable* ptr;
    vec3 offset;
};

class rotate_y : public hitable {
public:
    rotate_y(hitable* p, float angle) : ptr(p) {
        float radians = (M_PI / 180.0) * angle;
        sin_theta = sin(radians);
        cos_theta = cos(radians);
        sin_minus_theta = sin(-radians);
        cos_minus_theta = cos(-radians);
        hasbox = ptr->bounding_box(0, 1, bbox);

        if (hasbox) {
            vec3 minv(std::numeric_limits<float>::max() , std::numeric_limits<float>::max() , std::numeric_limits<float>::max());
            vec3 maxv(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());

            for(int i = 0; i < 2; i++) {
                for(int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        float x = i * bbox.max().x() + (1 - i) * bbox.min().x();
                        float y = j * bbox.max().y() + (1 - j) * bbox.min().y();
                        float z = k * bbox.max().z() + (1 - k) * bbox.min().z();
                        float rx = cos_theta * x + sin_theta * z;
                        float rz = -sin_theta * x + cos_theta * z;
                        vec3 r(rx, y, rz);
                        for (int c = 0; c < 3; c++) {
                            maxv[c] = std::max(maxv[c], r[c]);
                            minv[c] = std::min(minv[c], r[c]);
                        }
                    }
                }
            }
            bbox = aabb(minv, maxv);
        }
    }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        vec3 origin = r.origin();
        vec3 direction = r.direction();
        origin[0] = cos_theta * r.origin()[0] - sin_theta * r.origin()[2];
        origin[2] = sin_theta * r.origin()[0] + cos_theta * r.origin()[2];
        direction[0] = cos_theta * r.direction()[0] - sin_theta * r.direction()[2];
        direction[2] = sin_theta * r.direction()[0] + cos_theta * r.direction()[2];
        ray rotated_r(origin, direction, r.time());
        if (ptr->hit(rotated_r, t_min, t_max, rec)) {
            vec3 p = rec.p;
            vec3 normal = rec.normal;
            p[0] = cos_minus_theta * rec.p[0] - sin_minus_theta * rec.p[2];
            p[2] = sin_minus_theta * rec.p[0] + cos_minus_theta * rec.p[2];
            normal[0] = cos_minus_theta * rec.normal[0] - sin_minus_theta * rec.normal[2];
            normal[2] = sin_minus_theta * rec.normal[0] + cos_minus_theta * rec.normal[2];
            rec.p = p;
            rec.normal = normal;
            return true;
        }
        return false;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        box = bbox;
        return hasbox;
    }

    float sin_theta;
    float cos_theta;
    float sin_minus_theta;
    float cos_minus_theta;

    hitable* ptr;

    bool hasbox;
    aabb bbox;
};
