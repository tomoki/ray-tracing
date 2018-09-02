#pragma once

#include "vec3.h"

class texture {
public:
    virtual vec3 value(float u, float v, const vec3& p) const = 0;
};

class constant_texture : public texture
{
public:
    constant_texture(vec3 c) : color(c) { }
    vec3 value(float u, float v, const vec3& p) const override {
        return color;
    }
    vec3 color;
};

class checker_texture : public texture
{
public:
    checker_texture(texture* t0, texture* t1) : even(t0), odd(t1) { }

    vec3 value(float u, float v, const vec3& p) const override {
        float sines = sin(10 * p.x()) * sin(10 * p.y()) * sin(10 * p.z());
        if (sines < 0)
            return odd->value(u, v, p);
        else
            return even->value(u, v, p);
    }

    texture* odd;
    texture* even;
};

// perlin
float trilinear_interp(vec3 c[2][2][2], float u, float v, float w)
{
    float uu = u * u * (3 - 2 * u);
    float vv = v * v * (3 - 2 * v);
    float ww = w * w * (3 - 2 * w);

    float accum = 0;
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            for (int k = 0; k < 2; k++) {
                vec3 weight_v(u - i, v - j, w - k);
                accum += (i * uu + (1 - i) * (1 - uu)) * (j * vv + (1 - j) * (1 - vv)) * (k * ww + (1 - k) * (1 - ww)) * dot(c[i][j][k], weight_v);
            }
    return accum;
}

class perlin {
public:
    float noise(const vec3& p) const {
        float u = p.x() - floor(p.x());
        float v = p.y() - floor(p.y());
        float w = p.z() - floor(p.z());
        int i = floor(p.x());
        int j = floor(p.y());
        int k = floor(p.z());
        vec3 c[2][2][2];
        for (int di = 0; di < 2; di++)
            for (int dj = 0; dj < 2; dj++)
                for (int dk = 0; dk < 2; dk++)
                    c[di][dj][dk] = ranvec[perm_x[(i+di)&255] ^ perm_y[(j+dj)&255] ^ perm_z[(k+dk)&255]];

        return trilinear_interp(c, u, v, w);
    }
    float turb(const vec3& p, int depth=7) const {
        float accum = 0;
        vec3 temp_p = p;
        float weight = 1.0;
        for (int i = 0; i < depth; i++) {
            accum += weight * noise(temp_p);
            weight *= 0.5;
            temp_p *= 2;
        }
        return fabs(accum);
    }

    static vec3* ranvec;
    static int* perm_x;
    static int* perm_y;
    static int* perm_z;
};

vec3* perlin_generate()
{
    vec3* p = new vec3[256];
    for (int i = 0; i < 256; i++)
        p[i] = unit_vector(vec3(-1 + 2 * rand_float(), -1 + 2 * rand_float(), -1 + 2 * rand_float()));
    return p;
}

void permute(int* p, int n)
{
    for (int i = n - 1; i > 0; i--) {
        int target = int(rand_float() * (i + 1));
        std::swap(p[i], p[target]);
    }
}

int* perlin_generate_perm()
{
    int* p = new int[256];
    for (int i = 0; i < 256; i++)
        p[i] = i;
    permute(p, 256);
    return p;
}

vec3* perlin::ranvec = perlin_generate();
int* perlin::perm_x = perlin_generate_perm();
int* perlin::perm_y = perlin_generate_perm();
int* perlin::perm_z = perlin_generate_perm();

class noise_texture : public texture {
public:
    noise_texture() : scale(1.0) { }
    noise_texture(float sc) : scale(sc) { }
    vec3 value(float u, float v, const vec3& p) const override {
        // return vec3(1, 1, 1) * noise.turb(scale*p);
        // return vec3(1, 1, 1) * noise.noise(scale*p);
        return vec3(1, 1, 1) * 0.5 * (1 + sin(scale*p.z() + 10 * noise.turb(p)));
    }
    perlin noise;
    float scale;
};
