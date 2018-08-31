#pragma once

#include "hitable.h"

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
                     material(mat) { }
    virtual bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const override;

    vec3 center(float time) const;
    vec3 center0, center1;
    float time0, time1;
    float radius;
    material* material;
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
            rec.mat_ptr = material;
            return true;
        } else if(tmin < t2 && t2 < tmax) {
            rec.t = t2;
            rec.p = r.point_at_parameter(t2);
            rec.normal = (rec.p - center(r.time())) / radius;
            rec.mat_ptr = material;
            return true;
        }
    }
    return false;
}
