#pragma once

#include "hitable.h"

class sphere : public hitable {
public:
    sphere() {}
    sphere(vec3 cen, float r, material* mat) : center(cen), radius(r), mat_ptr(mat) {}
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override;
    bool bounding_box(float t0, float t1, aabb& box) const override;
    vec3 center;
    float radius;
    material* mat_ptr;
};

bool sphere:: hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
    vec3 oc = r.origin() - center;
    float a = dot(r.direction(), r.direction());
    float b = 2.0 * dot(oc, r.direction());
    float c = dot(oc, oc) - radius * radius;
    float discriminant = b*b - 4*a*c;

    if (discriminant > 0) {
        float t1 = (-b - sqrt(discriminant)) / (2*a);
        float t2 = (-b + sqrt(discriminant)) / (2*a);

        if (tmin < t1 && t1 < tmax) {
            rec.t = t1;
            rec.p = r.point_at_parameter(t1);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        } else if(tmin < t2 && t2 < tmax) {
            rec.t = t2;
            rec.p = r.point_at_parameter(t2);
            rec.normal = (rec.p - center) / radius;
            rec.mat_ptr = mat_ptr;
            return true;
        }
    }
    return false;
}

bool sphere::bounding_box(float t0, float t1, aabb& box) const
{
    box = aabb(center - vec3(radius, radius, radius),
               center + vec3(radius, radius, radius));
    return true;
}
