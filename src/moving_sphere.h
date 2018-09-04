#pragma once

#include "aabb.h"
#include "hitable.h"
#include "sphere.h"

class moving_sphere : public hitable {
public:
    moving_sphere() {}
    moving_sphere(vec3 cen0, vec3 cen1,
                  float t0, float t1,
                  float r, material* mat)
                   : center0(cen0),
                     center1(cen1),
                     time0(t0),
                     time1(t1),
                     radius(r),
                     mat_ptr(mat) { }
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override;
    bool bounding_box(float t0, float t1, aabb& box) const override;

    vec3 center(float time) const;
    vec3 center0, center1;
    float time0, time1;
    float radius;
    material* mat_ptr;
};

vec3 moving_sphere::center(float time) const
{
    return center0 + ((time - time0) / (time1-time0)) * (center1 - center0);
}

bool moving_sphere::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
    vec3 oc = r.origin() - center(r.time());
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
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p - center(r.time())) / radius, rec.u, rec.v);
            return true;
        } else if(tmin < t2 && t2 < tmax) {
            rec.t = t2;
            rec.p = r.point_at_parameter(t2);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = mat_ptr;
            get_sphere_uv((rec.p - center(r.time())) / radius, rec.u, rec.v);
            return true;
        }
    }
    return false;
}

bool moving_sphere::bounding_box(float t0, float t1, aabb& box) const
{
    aabb a0, a1;
    // always return true.
    sphere(center(t0), radius, mat_ptr).bounding_box(t0, t1, a0);
    sphere(center(t1), radius, mat_ptr).bounding_box(t0, t1, a1);
    box = surrounding_box(a0, a1);
    return true;
}
