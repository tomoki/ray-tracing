#pragma once

#include "hitable.h"

class xy_rect : public hitable {
public:
    xy_rect(float _x0, float _x1, float _y0, float _y1, float _z, material* mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), z(_z), mat_ptr(mat) { }

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

class xz_rect : public hitable {
public:
    xz_rect(float _x0, float _x1, float _z0, float _z1, float _y, material* mat)
        : x0(_x0), z0(_z0), x1(_x1), z1(_z1), y(_y), mat_ptr(mat) { }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        float t = (y - r.origin().y()) / r.direction().y();
        if (t < t_min || t_max < t)
            return false;
        float x = r.origin().x() + t * r.direction().x();
        float z = r.origin().z() + t * r.direction().z();
        if (x < x0 || x1 < x || z < z0 || z1 < z)
            return false;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        rec.mat_ptr = mat_ptr;
        rec.p = r.point_at_parameter(t);
        rec.normal = vec3(0, 1, 0);
        return true;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        box = aabb(vec3(x0, y - 0.0001, z0), vec3(x1, y + 0.0001, z1));
        return true;
    }

    material* mat_ptr;
    float x0, z0, x1, z1, y;
};

class yz_rect : public hitable {
public:
    yz_rect(float _y0, float _y1, float _z0, float _z1, float _x, material* mat)
        : y0(_y0), z0(_z0), y1(_y1), z1(_z1), x(_x), mat_ptr(mat) { }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        float t = (x - r.origin().x()) / r.direction().x();
        if (t < t_min || t_max < t)
            return false;
        float y = r.origin().y() + t * r.direction().y();
        float z = r.origin().z() + t * r.direction().z();
        if (y < y0 || y1 < y || z < z0 || z1 < z)
            return false;

        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        rec.mat_ptr = mat_ptr;
        rec.p = r.point_at_parameter(t);
        rec.normal = vec3(1, 0, 0);
        return true;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        box = aabb(vec3(x - 0.0001, y0, z0), vec3(x + 0.0001, y1, z1));
        return true;
    }

    material* mat_ptr;
    float y0, z0, y1, z1, x;
};

class box : public hitable {
public:
    box(const vec3& p0, const vec3& p1, material* mat) : pmin(p0), pmax(p1), mat_ptr(mat) {
        hitable** list = new hitable*[6];
        list[0] = new flip_normals(new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p0.z(), mat));
        list[1] = new xy_rect(p0.x(), p1.x(), p0.y(), p1.y(), p1.z(), mat);
        list[2] = new flip_normals(new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p0.y(), mat));
        list[3] = new xz_rect(p0.x(), p1.x(), p0.z(), p1.z(), p1.y(), mat);
        list[4] = new flip_normals(new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p0.x(), mat));
        list[5] = new yz_rect(p0.y(), p1.y(), p0.z(), p1.z(), p1.x(), mat);
        list_ptr = new hitable_list(list, 6);
    }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        return list_ptr->hit(r, t_min, t_max, rec);
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        box = aabb(pmin, pmax);
        return true;
    }

    vec3 pmin, pmax;
    material* mat_ptr;
    hitable_list* list_ptr;
};

bool is_inside_of_triangle(float tx, float ty, float x0, float y0, float x1, float y1, float x2, float y2)
{
    vec3 t(tx, ty, 0);
    std::vector<vec3> pos(3);
    pos[0] = vec3(x0, y0, 0);
    pos[1] = vec3(x1, y1, 0);
    pos[2] = vec3(x2, y2, 0);

    auto edge1 = pos[1] - pos[0];
    auto targ1 = t - pos[0];
    float f = cross_length_including_minus(edge1, targ1);
    // TODO, consider on the edge.
    for (int i = 0; i < pos.size(); i++) {
        auto e1 = pos[(i + 1)%pos.size()] - pos[i];
        auto e2 = t - pos[i];
        auto cc = cross_length_including_minus(e1, e2);
        if ((cc > 0 && f > 0) || (cc < 0 && f < 0)) {
            continue;
        }
        return false;
    }
    return true;
}

struct triangle_parameter {
    vec3 v0, v1, v2;
    vec3 vt0, vt1, vt2;
    vec3 vn0, vn1, vn2;
    bool has_tex_coord { false };
    bool has_normal { false };
};

// calculated by Tomas Moller's algrithm.
// See Fast, Minimum Storage Ray/Triangle Intersection
class triangle : public hitable {
public:
    triangle(triangle_parameter param, material* mat) : p(param), mat_ptr(mat) { }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        const vec3 edge1 = p.v1 - p.v0;
        const vec3 edge2 = p.v2 - p.v0;
        const vec3 pvec = cross(r.direction(), edge2);
        const float det = dot(edge1, pvec);
        float inv_det = 1.0 / det;
        const float EPSILON = 1e-6;
        const vec3 tvec = r.origin() - p.v0;
        // backfacing
        if (det < EPSILON)
            return false;

        rec.u = dot(tvec, pvec);
        if (rec.u < 0.0 || rec.u > det)
            return false;

        vec3 qvec = cross(tvec, edge1);

        rec.v = dot(r.direction(), qvec);
        if (rec.v < 0.0 || rec.u + rec.v > det)
            return false;

        // std::cerr << inv_det << std::endl;
        rec.t = dot(edge2, qvec);
        rec.t *= inv_det;
        rec.u *= inv_det;
        rec.v *= inv_det;
        {
            // Check texture coordinate and set
            vec3 vt1 = p.vt1 - p.vt0;
            vec3 vt2 = p.vt2 - p.vt0;
            if (vt1.norm() < 1e-7 && vt2.norm() < 1e-7) {
                // FIXME: probably it doesn't have texture coord.
            } else {
                vec3 uv = p.vt0 + vt1 * rec.u + vt2 * rec.v;
                rec.u = uv.x();
                rec.v = uv.y();
            }
        }
        rec.mat_ptr = mat_ptr;
        rec.p = r.point_at_parameter(rec.t);
        rec.normal = unit_vector(cross(edge1, edge2));
        if (rec.t < t_min || t_max < rec.t)
            return false;

        return true;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        vec3 mins(std::min({ p.v0.x(), p.v1.x(), p.v2.x() }),
                  std::min({ p.v0.y(), p.v1.y(), p.v2.y() }),
                  std::min({ p.v0.z(), p.v1.z(), p.v2.z() }));
        vec3 maxs(std::max({ p.v0.x(), p.v1.x(), p.v2.x() }),
                  std::max({ p.v0.y(), p.v1.y(), p.v2.y() }),
                  std::max({ p.v0.z(), p.v1.z(), p.v2.z() }));
        // Triangle may be axis-aligned.
        // Even in such case, boundingbox must have volume.
        for (int i = 0; i < 3; i++) {
            mins[i] -= 0.0001;
            maxs[i] += 0.0001;
        }
        box = aabb(mins, maxs);
        return true;
    }
    triangle_parameter p;
    material* mat_ptr;
};

class xy_triangle : public hitable {
public:
    xy_triangle(float _x0, float _x1, float _x2, float _y0, float _y1, float _y2, float _z, material* mat)
        : x0(_x0), x1(_x1), x2(_x2), y0(_y0), y1(_y1), y2(_y2), z(_z), mat_ptr(mat) {
    }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        float t = (z - r.origin().z()) / r.direction().z();
        if (t < t_min || t_max < t)
            return false;
        float x = r.origin().x() + t * r.direction().x();
        float y = r.origin().y() + t * r.direction().y();
        if (!is_inside_of_triangle(x, y, x0, y0, x1, y1, x2, y2))
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
        float min_x = std::min({ x0, x1, x2 });
        float max_x = std::max({ x0, x1, x2 });
        float min_y = std::min({ y0, y1, y2 });
        float max_y = std::max({ y0, y1, y2 });

        box = aabb(vec3(min_x, min_y, z - 0.0001), vec3(max_x, max_y, z + 0.0001));
        return true;
    }
    float x0, x1, x2;
    float y0, y1, y2;
    float z;
    material* mat_ptr;
};

class xz_triangle : public hitable {
public:
    xz_triangle(float _x0, float _x1, float _x2, float _z0, float _z1, float _z2, float _y, material* mat)
        : x0(_x0), x1(_x1), x2(_x2), z0(_z0), z1(_z1), z2(_z2), y(_y), mat_ptr(mat) {
    }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        float t = (y - r.origin().y()) / r.direction().y();
        if (t < t_min || t_max < t)
            return false;
        float x = r.origin().x() + t * r.direction().x();
        float z = r.origin().z() + t * r.direction().z();
        if (!is_inside_of_triangle(x, z, x0, z0, x1, z1, x2, z2))
            return false;

        rec.u = (x - x0) / (x1 - x0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        rec.mat_ptr = mat_ptr;
        rec.p = r.point_at_parameter(t);
        rec.normal = vec3(0, 1, 0);
        return true;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        float min_x = std::min({ x0, x1, x2 });
        float max_x = std::max({ x0, x1, x2 });
        float min_z = std::min({ z0, z1, z2 });
        float max_z = std::max({ z0, z1, z2 });

        box = aabb(vec3(min_x, y - 0.0001, min_z), vec3(max_x, y + 0.0001, max_z));
        return true;
    }
    float x0, x1, x2;
    float z0, z1, z2;
    float y;
    material* mat_ptr;
};

class yz_triangle : public hitable {
public:
    yz_triangle(float _y0, float _y1, float _y2, float _z0, float _z1, float _z2, float _x, material* mat)
        : y0(_y0), y1(_y1), y2(_y2), z0(_z0), z1(_z1), z2(_z2), x(_x), mat_ptr(mat) {
    }

    bool hit(const ray& r, float t_min, float t_max, hit_record& rec) const {
        float t = (x - r.origin().x()) / r.direction().x();
        if (t < t_min || t_max < t)
            return false;
        float y = r.origin().y() + t * r.direction().y();
        float z = r.origin().z() + t * r.direction().z();
        if (!is_inside_of_triangle(y, z, y0, z0, y1, z1, y2, z2))
            return false;

        rec.u = (y - y0) / (y1 - y0);
        rec.v = (z - z0) / (z1 - z0);
        rec.t = t;
        rec.mat_ptr = mat_ptr;
        rec.p = r.point_at_parameter(t);
        rec.normal = vec3(1, 0, 0);
        return true;
    }

    bool bounding_box(float t0, float t1, aabb& box) const {
        float min_y = std::min({ y0, y1, y2 });
        float max_y = std::max({ y0, y1, y2 });
        float min_z = std::min({ z0, z1, z2 });
        float max_z = std::max({ z0, z1, z2 });

        box = aabb(vec3(x - 0.0001, min_y , min_z), vec3(x + 0.0001, max_y, max_z));
        return true;
    }
    float y0, y1, y2;
    float z0, z1, z2;
    float x;
    material* mat_ptr;
};
