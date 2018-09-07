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
