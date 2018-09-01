#pragma once

#include "aabb.h"
#include "common.h"
#include "hitable.h"

#include <algorithm>

class bvh_node : public hitable {
public:
    bvh_node() {}
    bvh_node(hitable** l, int n, float t0, float t1);
    bool hit(const ray& r, float tmin, float tmax, hit_record& rec) const;
    bool bounding_box(float t0, float t1, aabb& box) const;
    hitable* left = nullptr;
    hitable* right = nullptr;
    aabb box;
};

bool bvh_node::bounding_box(float t0, float t1, aabb& b) const
{
    b = box;
    return true;
}

bool bvh_node::hit(const ray& r, float tmin, float tmax, hit_record& rec) const
{
    if (box.hit(r, tmin, tmax)) {
        hit_record rec_left, rec_right;
        bool hit_left = left->hit(r, tmin, tmax, rec_left);
        bool hit_right = right->hit(r, tmin, tmax, rec_right);
        if (hit_left && hit_right) {
            if (rec_left.t < rec_right.t)
                rec = rec_left;
            else
                rec = rec_right;
        } else if (hit_left)
            rec = rec_left;
        else if (hit_right)
            rec = rec_right;
        else
            return false;
        return true;
    }
    return false;
}

bool compare_box_x(hitable* l, hitable* r)
{
    aabb box_left, box_right;
    if (!l->bounding_box(0, 0, box_left) || !r->bounding_box(0, 0, box_right))
        std::cerr << "No bounding box in bvh_node constructor!" << std::endl;

    return box_left.min().x() < box_right.min().x();
}

bool compare_box_y(hitable* l, hitable* r)
{
    aabb box_left, box_right;
    if (!l->bounding_box(0, 0, box_left) || !r->bounding_box(0, 0, box_right))
        std::cerr << "No bounding box in bvh_node constructor!" << std::endl;

    return box_left.min().y() < box_right.min().y();
}

bool compare_box_z(hitable* l, hitable* r)
{
    aabb box_left, box_right;
    if (!l->bounding_box(0, 0, box_left) || !r->bounding_box(0, 0, box_right))
        std::cerr << "No bounding box in bvh_node constructor!" << std::endl;

    return box_left.min().z() < box_right.min().z();
}

bvh_node::bvh_node(hitable** l, int n, float t0, float t1)
{
    int axis = int(3 * rand_float());
    if (axis == 0)
        std::sort(l, l + n, compare_box_x);
    else if (axis == 1)
        std::sort(l, l + n, compare_box_y);
    else if (axis == 1)
        std::sort(l, l + n, compare_box_z);

    if (n == 1)
        left = right = l[0];
    else if (n == 2) {
        left = l[0];
        right = l[1];
    } else {
        left = new bvh_node(l, n / 2, t0, t1);
        right = new bvh_node(l + n / 2, n - n / 2, t0, t1);
    }
    aabb box_left, box_right;
    if (!left->bounding_box(t0, t1, box_left) || !right->bounding_box(t0, t1, box_right))
        std::cerr << "No bounding box in bvh_node constructor!" << std::endl;
    box = surrounding_box(box_left, box_right);
}
