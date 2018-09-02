#pragma once

#include "common.h"
#include "ray.h"

class aabb {
public:
    aabb() {}
    aabb(const vec3& a, const vec3& b)
        : _min(a)
        , _max(b)
    {
    }

    vec3 min() const { return _min; }
    vec3 max() const { return _max; }

    bool hit(const ray& r, float tmin, float tmax) const
    {
        for (int a = 0; a < 3; a++) {
            float t0 = ffmin((_min[a] - r.origin()[a]) / r.direction()[a], (_max[a] - r.origin()[a]) / r.direction()[a]);
            float t1 = ffmax((_min[a] - r.origin()[a]) / r.direction()[a], (_max[a] - r.origin()[a]) / r.direction()[a]);

            tmin = ffmax(t0, tmin);
            tmax = ffmin(t1, tmax);
            if (tmax <= tmin)
                return false;
        }
        return true;
    }

    vec3 _min, _max;
};

aabb surrounding_box(const aabb& a0, const aabb& a1)
{
    vec3 small(ffmin(a0.min().x(), a1.min().x()),
        ffmin(a0.min().y(), a1.min().y()),
        ffmin(a0.min().z(), a1.min().z()));

    vec3 big(ffmax(a0.max().x(), a1.max().x()),
        ffmax(a0.max().y(), a1.max().y()),
        ffmax(a0.max().z(), a1.max().z()));

    return aabb(small, big);
}
