#include "math3d.h"

Vec3 vec3_create(double x, double y, double z) {
    Vec3 v = {x, y, z};
    return v;
}

Vec2 vec2_create(double x, double y) {
    Vec2 v = {x, y};
    return v;
}

Vec3 vec3_add(Vec3 a, Vec3 b) {
    return vec3_create(a.x + b.x, a.y + b.y, a.z + b.z);
}

Vec3 vec3_sub(Vec3 a, Vec3 b) {
    return vec3_create(a.x - b.x, a.y - b.y, a.z - b.z);
}

Vec3 vec3_mul(Vec3 v, double scalar) {
    return vec3_create(v.x * scalar, v.y * scalar, v.z * scalar);
}

double vec3_length(Vec3 v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

Vec3 vec3_normalize(Vec3 v) {
    double len = vec3_length(v);
    if (len == 0.0) return vec3_create(0, 0, 0);
    return vec3_mul(v, 1.0 / len);
}

double vec3_dot(Vec3 a, Vec3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec2 vec2_add(Vec2 a, Vec2 b) {
    return vec2_create(a.x + b.x, a.y + b.y);
}

Vec2 vec2_mul(Vec2 v, double scalar) {
    return vec2_create(v.x * scalar, v.y * scalar);
}

double vec2_length(Vec2 v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

Vec2 vec2_normalize(Vec2 v) {
    double len = vec2_length(v);
    if (len == 0.0) return vec2_create(0, 0);
    return vec2_mul(v, 1.0 / len);
}

Vec3 vec3_rotate_y(Vec3 v, double angle) {
    double cos_a = cos(angle);
    double sin_a = sin(angle);
    return vec3_create(
        v.x * cos_a - v.z * sin_a,
        v.y,
        v.x * sin_a + v.z * cos_a
    );
}

Vec3 vec3_rotate_x(Vec3 v, double angle) {
    double cos_a = cos(angle);
    double sin_a = sin(angle);
    return vec3_create(
        v.x,
        v.y * cos_a - v.z * sin_a,
        v.y * sin_a + v.z * cos_a
    );
}

Vec2 vec2_rotate(Vec2 v, double angle) {
    double cos_a = cos(angle);
    double sin_a = sin(angle);
    return vec2_create(
        v.x * cos_a - v.y * sin_a,
        v.x * sin_a + v.y * cos_a
    );
}
