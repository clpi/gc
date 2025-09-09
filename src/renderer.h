#ifndef RENDERER_H
#define RENDERER_H

#include "tui.h"
#include "math3d.h"

#define MAP_WIDTH 16
#define MAP_HEIGHT 16
#define WALL_HEIGHT 1.0
#define FOV 60.0
#define RENDER_DISTANCE 16.0

typedef struct {
    Vec3 position;
    double angle;
    Vec3 direction;
} Camera;

typedef struct {
    char map[MAP_HEIGHT][MAP_WIDTH];
    int width;
    int height;
} Map;

typedef struct {
    double distance;
    int wall_type;
    int hit_side;
} RaycastHit;

void renderer_init();
void camera_init(Camera *camera, Vec3 position, double angle);
void camera_update_direction(Camera *camera);
RaycastHit raycast(Map *map, Vec3 start, Vec3 direction);
void render_frame(TUI *tui, Camera *camera, Map *map);
char get_wall_char(double distance, int wall_type);

// Lighting controls
void renderer_set_light_dir(Vec3 dir);
void renderer_rotate_light(double yaw_delta, double pitch_delta);

// UI helpers
void renderer_draw_minimap(TUI *tui, Map *map, Camera *camera);

#endif
