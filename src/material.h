#pragma once

#include "common.h"
#include "hitable.h"
#include "ray.h"
#include "texture.h"
#include "obj_loader.h"

#include <algorithm>
#include <cmath>

vec3 reflect(const vec3& v, const vec3& n)
{
      return v + (-2*dot(v, n) * n);
}

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
    vec3 uv = unit_vector(v);
    float dt = dot(uv, n);
    float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1-dt*dt);
    if (discriminant > 0) {
        refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
        return true;
    }
    return false;
}

float schlick(float cosine, float ref_idx)
{
    float r0 = (1-ref_idx) / (1 + ref_idx);
    r0 = r0 * r0;
    return r0 + (1-r0) * pow(1-cosine, 5);
}

class material {
public:
    virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
    virtual vec3 emitted(float u, float v, const vec3& p) const { return vec3(0, 0, 0); }
};

class lambertian : public material {
public:
    lambertian(texture* a) : albedo(a) {}
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override
    {
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target-rec.p);
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    texture* albedo;
};

class metal : public material {
public:
    metal(const vec3& a, float f) : albedo(a), fuzz(std::min(1.0f, f)) {}
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override
    {
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere(), r_in.time());
        attenuation = albedo;
        return dot(scattered.direction(), rec.normal) > 0;
    }
    vec3 albedo;
    float fuzz;
};

class dielectric : public material {
public:
    dielectric(float ri) : ref_idx(ri) {}
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const override
    {
        vec3 outward_normal;
        vec3 reflected = reflect(unit_vector(r_in.direction()), rec.normal);
        float ni_over_nt;
        attenuation = vec3(1.0, 1.0, 1.0);
        vec3 refracted;
        float reflect_prob;
        float cosine;
        if (dot(r_in.direction(), rec.normal) > 0) {
            outward_normal = -rec.normal;
            ni_over_nt = ref_idx;
            cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
        } else {
            outward_normal = rec.normal;
            ni_over_nt = 1.0 / ref_idx;
            cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
        }
        if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted)) {
            // scattered = ray(rec.p, refracted);
            reflect_prob = schlick(cosine, ref_idx);
        } else {
            reflect_prob = 1.0;
        }
        if (rand_float() < reflect_prob) {
            scattered = ray(rec.p, reflected, r_in.time());
        } else {
            scattered = ray(rec.p, refracted, r_in.time());
        }
        return true;
    }

    float ref_idx;
};

class diffuse_light : public material {
public:
    diffuse_light(texture* a) : emit(a) { }
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        return false;
    }
    vec3 emitted(float u, float v, const vec3& p) const {
        return emit->value(u, v, p);
    }
    texture* emit;
};

class isotropic : public material {
public:
    isotropic(texture* a) : albedo(a) { }
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        scattered = ray(rec.p, random_in_unit_sphere());
        attenuation = albedo->value(rec.u, rec.v, rec.p);
        return true;
    }

    texture* albedo;
};

class custom_material : public material {
public:
    // Copy obj_material in case of it's allocated in stack
    custom_material(obj_material mat) : obj_mat(mat) { }
    bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const {
        attenuation = obj_mat.diffuse;
        vec3 target = rec.p + rec.normal + random_in_unit_sphere();
        scattered = ray(rec.p, target-rec.p);

        // read texture
        if (obj_mat.tex_color.size() > 0) {
            int nx = obj_mat.tex_width;
            int ny = obj_mat.tex_height;
            int x = obj_mat.tex_width * rec.u;
            int y = (1 - rec.v) * ny - 0.001;
            x = std::clamp(x, 0, nx - 1);
            y = std::clamp(y, 0, ny - 1);
            // std::cerr << "x = " << x << " y = " << y << " nx = " << nx << " ny = " << ny << std::endl;

            float r = int(obj_mat.tex_color[4 * x + 4 * nx * y + 0]) / 255.0;
            float g = int(obj_mat.tex_color[4 * x + 4 * nx * y + 1]) / 255.0;
            float b = int(obj_mat.tex_color[4 * x + 4 * nx * y + 2]) / 255.0;
            float a = int(obj_mat.tex_color[4 * x + 4 * nx * y + 3]) / 255.0;
            // std::cerr << r << " " << g << " " << b << " " << a << std::endl;
            attenuation = vec3(r, g, b);
            // if (a < 0.99)
            //     return false;

        }
        return true;
    }
    vec3 emitted(float u, float v, const vec3& p) const {
        return obj_mat.emissive_coefficient;
    }
    obj_material obj_mat;
};
