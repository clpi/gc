#ifndef MATH3D_H
#define MATH3D_H

#include <math.h>

typedef struct {
    double x, y, z;
} Vec3;

typedef struct {
    double x, y;
} Vec2;

typedef struct {
    Vec3 position;
    Vec3 direction;
} Ray;

Vec3 vec3_create(double x, double y, double z);
Vec2 vec2_create(double x, double y);
Vec3 vec3_add(Vec3 a, Vec3 b);
Vec3 vec3_sub(Vec3 a, Vec3 b);
Vec3 vec3_mul(Vec3 v, double scalar);
Vec3 vec3_normalize(Vec3 v);
double vec3_dot(Vec3 a, Vec3 b);
double vec3_length(Vec3 v);

Vec2 vec2_add(Vec2 a, Vec2 b);
Vec2 vec2_mul(Vec2 v, double scalar);
double vec2_length(Vec2 v);
Vec2 vec2_normalize(Vec2 v);

Vec3 vec3_rotate_y(Vec3 v, double angle);
Vec3 vec3_rotate_x(Vec3 v, double angle);
Vec2 vec2_rotate(Vec2 v, double angle);

#define PI 3.14159265359
#define DEG_TO_RAD(deg) ((deg) * PI / 180.0)
#define RAD_TO_DEG(rad) ((rad) * 180.0 / PI)

#endif
