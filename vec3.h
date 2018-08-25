#pragma once

#include <cmath>
#include <cassert>
#include <iostream>

class vec3 {
public:
    vec3() {
        e[0] = 0;
        e[1] = 0;
        e[2] = 0;
    }
    vec3(float e0, float e1, float e2) {
        e[0] = e0;
        e[1] = e1;
        e[2] = e2;
    }
    vec3(const vec3& v) {
        e[0] = v.e[0];
        e[1] = v.e[1];
        e[2] = v.e[2];
    }
    inline float x() { return e[0]; }
    inline float y() { return e[1]; }
    inline float z() { return e[2]; }
    inline float r() { return e[0]; }
    inline float g() { return e[1]; }
    inline float b() { return e[2]; }

    inline vec3 operator+() const { return *this; }
    inline vec3 operator-() const {
        return vec3(-e[0], -e[1], -e[2]);
    }
    inline float operator[](int i) const {
        assert(0 <= i && i <= 2);
        return e[i];
    }
    inline float& operator[](int i) {
        assert(0 <= i && i <= 2);
        return e[i];
    }
    inline vec3& operator+=(const vec3& v2);
    inline vec3& operator-=(const vec3& v2);
    inline vec3& operator*=(const vec3& v2);
    inline vec3& operator/=(const vec3& v2);
    inline vec3& operator*=(float t);
    inline vec3& operator/=(float t);

    inline float norm() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }
    inline float length() const {
        return sqrt(norm());
    }

    inline void make_unit_vector(){
        float k = 1.0 / length();
        e[0] *= k;
        e[1] *= k;
        e[2] *= k;
    }
    float e[3];
};

inline std::istream& operator>>(std::istream& is, vec3& v)
{
    is >> v.e[0] >> v.e[1] >> v.e[2];
    return is;
}

inline std::ostream& operator<<(std::ostream& os, const vec3& v)
{
    os << v.e[0] << " " << v.e[1] << v.e[2];
    return os;
}

inline vec3 operator+(const vec3& left, const vec3& right)
{
    return vec3(left.e[0] + right.e[0],
                left.e[1] + right.e[1],
                left.e[2] + right.e[2]);
}

inline vec3 operator-(const vec3& left, const vec3& right)
{
    return vec3(left.e[0] - right.e[0],
                left.e[1] - right.e[1],
                left.e[2] - right.e[2]);
}

inline vec3 operator*(const vec3& left, const vec3& right)
{
    return vec3(left.e[0] * right.e[0],
                left.e[1] * right.e[1],
                left.e[2] * right.e[2]);
}

inline vec3 operator/(const vec3& left, const vec3& right)
{
    return vec3(left.e[0] / right.e[0],
                left.e[1] / right.e[1],
                left.e[2] / right.e[2]);
}

inline vec3 operator*(float t, const vec3& v)
{
    return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline vec3 operator*(const vec3& v, float t)
{
    return t * v;
}

inline vec3 operator/(const vec3& v, float t)
{
    return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

inline float dot(const vec3& left, const vec3& right)
{
    return left[0] * right[0] +
           left[1] * right[1] +
           left[2] * right[2];
}

inline vec3 cross(const vec3& left, const vec3& right)
{
    return vec3(
        left[1] * right[2] - left[2] * right[1],
        - (left[0] * right[2] - left[2] * right[0]),
        left[0] * right[1] - left[1] * right[0]
    );
}

inline vec3& vec3::operator+=(const vec3& v)
{
    e[0] += v[0];
    e[1] += v[1];
    e[2] += v[2];
    return *this;
}

inline vec3& vec3::operator-=(const vec3& v)
{
    e[0] -= v[0];
    e[1] -= v[1];
    e[2] -= v[2];
    return *this;
}

inline vec3& vec3::operator*=(const vec3& v)
{
    e[0] *= v[0];
    e[1] *= v[1];
    e[2] *= v[2];
    return *this;
}

inline vec3& vec3::operator/=(const vec3& v)
{
    e[0] /= v[0];
    e[1] /= v[1];
    e[2] /= v[2];
    return *this;
}

inline vec3& vec3::operator*=(float t)
{
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
}

inline vec3& vec3::operator/=(float t)
{
    e[0] /= t;
    e[1] /= t;
    e[2] /= t;
    return *this;
}

inline vec3 unit_vector(vec3 v)
{
    return v / v.length();
}
