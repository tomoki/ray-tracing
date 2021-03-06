#pragma once

#include "ray.h"
#include "common.h"

class camera {
public:
    // aspect = width / height
    camera(vec3 lookfrom, vec3 lookat, vec3 vup, float vfov, float aspect,
           float aperture, float focus_dist, float t0, float t1) {
        lens_radius = aperture / 2;
        time0 = t0;
        time1 = t1;
        float theta = vfov * 3.1415926535 / 180;
        float half_height = tan(theta / 2);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);
        lower_left_corner = origin - half_width * focus_dist * u
                                   - half_height * focus_dist * v
                                   - focus_dist * w;
        horizontal = 2 * half_width * focus_dist * u;
        vertical = 2  * half_height * focus_dist * v;
    }

    ray get_ray(float s, float t) const {
        vec3 rd = lens_radius * random_in_unit_disk();
        vec3 offset = u * rd.x() + v * rd.y();
        float time = time0 + rand_float() * (time1-time0);
        return ray(origin + offset,
                  lower_left_corner +
                  s * horizontal +
                  t * vertical - origin - offset,
                  time);
    }

    vec3 u, v, w;
    vec3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
    vec3 origin;
    float lens_radius;
    float time0, time1;
};
