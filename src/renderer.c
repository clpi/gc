#include "renderer.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

static Vec3 g_light_dir; // direction from the light source (unit vector)

void renderer_init() {
    g_light_dir = vec3_normalize(vec3_create(-0.6, -0.3, -0.8));
}

void renderer_set_light_dir(Vec3 dir) {
    g_light_dir = vec3_normalize(dir);
}

void renderer_rotate_light(double yaw_delta, double pitch_delta) {
    // Apply yaw (around Y), then pitch (around X) to light direction
    Vec3 d = g_light_dir;
    d = vec3_rotate_y(d, yaw_delta);
    d = vec3_rotate_x(d, pitch_delta);
    g_light_dir = vec3_normalize(d);
}

void camera_init(Camera *camera, Vec3 position, double angle) {
    camera->position = position;
    camera->angle = angle;
    camera_update_direction(camera);
}

void camera_update_direction(Camera *camera) {
    camera->direction = vec3_create(cos(camera->angle), 0, sin(camera->angle));
}

RaycastHit raycast(Map *map, Vec3 start, Vec3 direction) {
    RaycastHit hit = {RENDER_DISTANCE, 0, 0};
    
    double delta_dist_x = fabs(1.0 / direction.x);
    double delta_dist_z = fabs(1.0 / direction.z);
    
    int map_x = (int)start.x;
    int map_z = (int)start.z;
    
    double side_dist_x, side_dist_z;
    int step_x, step_z;
    
    if (direction.x < 0) {
        step_x = -1;
        side_dist_x = (start.x - map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = (map_x + 1.0 - start.x) * delta_dist_x;
    }
    
    if (direction.z < 0) {
        step_z = -1;
        side_dist_z = (start.z - map_z) * delta_dist_z;
    } else {
        step_z = 1;
        side_dist_z = (map_z + 1.0 - start.z) * delta_dist_z;
    }
    
    int hit_wall = 0;
    int side;
    
    while (!hit_wall) {
        if (side_dist_x < side_dist_z) {
            side_dist_x += delta_dist_x;
            map_x += step_x;
            side = 0;
        } else {
            side_dist_z += delta_dist_z;
            map_z += step_z;
            side = 1;
        }
        
        if (map_x < 0 || map_x >= MAP_WIDTH || map_z < 0 || map_z >= MAP_HEIGHT) {
            hit_wall = 1;
            hit.wall_type = 1;
        } else if (map->map[map_z][map_x] != ' ') {
            hit_wall = 1;
            hit.wall_type = map->map[map_z][map_x] - '0';
        }
    }
    
    if (side == 0) {
        hit.distance = (map_x - start.x + (1 - step_x) / 2) / direction.x;
    } else {
        hit.distance = (map_z - start.z + (1 - step_z) / 2) / direction.z;
    }
    
    hit.hit_side = side;
    return hit;
}

// Legacy API retained for compatibility; now unused in favor of brightness mapping
char get_wall_char(double distance, int wall_type) {
    (void)wall_type;
    if (distance > 12.0) return '.';
    if (distance > 8.0) return ':';
    if (distance > 5.0) return '!';
    if (distance > 3.0) return '|';
    if (distance > 1.5) return '#';
    return '@';
}

static inline double clamp01(double v) {
    if (v < 0.0) return 0.0;
    if (v > 1.0) return 1.0;
    return v;
}

static char shade_from_brightness(double b) {
    // From darkest to brightest; tuned for ncurses legibility
    static const char ramp[] = " .:-=+*#%@"; // 10 levels
    int levels = (int)(sizeof(ramp) - 1);
    int idx = (int)floor(b * (levels - 1));
    if (idx < 0) idx = 0;
    if (idx >= levels) idx = levels - 1;
    return ramp[idx];
}

static double compute_wall_brightness(Vec3 normal, Vec3 view_dir, double distance, double tex_u) {
    // For a directional light, use L as direction from surface toward the light
    Vec3 L = vec3_mul(g_light_dir, -1.0);
    double ndotl = clamp01(vec3_dot(normal, L));

    // Simple Phong-like specular
    // Reflect L around N: R = 2(N·L)N - L
    Vec3 R = vec3_sub(vec3_mul(normal, 2.0 * ndotl), L);
    R = vec3_normalize(R);
    double rdotv = clamp01(vec3_dot(R, vec3_mul(view_dir, -1.0)));
    double spec = pow(rdotv, 32.0) * 0.25; // small highlight

    // Subtle wall texturing via column-based variation
    double tex_mod = 0.85 + 0.15 * (0.5 * (cos(tex_u * 2.0 * PI * 4.0) + 1.0));

    // Distance attenuation
    double atten = 1.0 / (1.0 + 0.15 * distance * distance);

    double ambient = 0.18;
    double diffuse = 0.82 * ndotl;
    double brightness = (ambient + diffuse) * atten * tex_mod + spec * atten;
    return clamp01(brightness);
}

void render_frame(TUI *tui, Camera *camera, Map *map) {
    tui_clear_render(tui);
    
    double plane_x = -sin(camera->angle) * tan(DEG_TO_RAD(FOV / 2));
    double plane_z = cos(camera->angle) * tan(DEG_TO_RAD(FOV / 2));
    
    for (int x = 0; x < RENDER_WIDTH - 2; x++) {
        double camera_x = 2 * x / (double)(RENDER_WIDTH - 2) - 1;
        Vec3 ray_dir = vec3_create(
            camera->direction.x + plane_x * camera_x,
            0,
            camera->direction.z + plane_z * camera_x
        );
        ray_dir = vec3_normalize(ray_dir);
        
        RaycastHit hit = raycast(map, camera->position, ray_dir);
        
        int line_height = (int)(RENDER_HEIGHT / hit.distance);
        if (line_height > RENDER_HEIGHT - 2) line_height = RENDER_HEIGHT - 2;
        
        int draw_start = (RENDER_HEIGHT - line_height) / 2;
        int draw_end = draw_start + line_height;
        
        // Compute surface normal based on which grid side we hit
        Vec3 normal = vec3_create(0, 0, 0);
        if (hit.hit_side == 0) {
            normal = vec3_create((ray_dir.x < 0.0) ? 1.0 : -1.0, 0.0, 0.0);
        } else {
            normal = vec3_create(0.0, 0.0, (ray_dir.z < 0.0) ? 1.0 : -1.0);
        }

        // Fractional coordinate along the wall for subtle texture modulation
        double wall_x;
        if (hit.hit_side == 0) {
            wall_x = camera->position.z + hit.distance * ray_dir.z;
        } else {
            wall_x = camera->position.x + hit.distance * ray_dir.x;
        }
        wall_x -= floor(wall_x);

        // Lighting model (Lambert + specular + attenuation)
        double brightness = compute_wall_brightness(normal, ray_dir, hit.distance, wall_x);
        if (hit.hit_side == 1) brightness *= 0.85; // slight darkening for side faces

        char wall_char = shade_from_brightness(brightness);

        // Color/attrs based on brightness
        short wall_pair = TUI_CP_WALL_NEAR;
        int wall_attr = A_NORMAL;
        if (brightness < 0.35) { wall_pair = TUI_CP_WALL_FAR; wall_attr = A_DIM; }
        else if (brightness < 0.7) { wall_pair = TUI_CP_WALL_MID; wall_attr = A_NORMAL; }
        else { wall_pair = TUI_CP_WALL_NEAR; wall_attr = A_BOLD; }

        for (int y = 1; y < RENDER_HEIGHT - 1; y++) {
            if (y >= draw_start && y <= draw_end) {
                tui_draw_pixel_color(tui, x + 1, y, wall_char, wall_pair, wall_attr);
            } else if (y < draw_start) {
                // ceiling: subtle darkening near top for depth
                double t = (double)(draw_start - y) / (double)(RENDER_HEIGHT / 2);
                double b = clamp01(0.05 + 0.15 * (1.0 - t));
                char ch = shade_from_brightness(b);
                tui_draw_pixel(tui, x + 1, y, (b < 0.1 ? ' ' : ch));
            } else {
                // floor: gradient shading by screen row (cheap floor-cast approximation)
                double rel = (double)(y - draw_end) / (double)(RENDER_HEIGHT - draw_end);
                double fb = clamp01(0.9 * (1.0 - rel) * (1.0 - rel));
                char fch = shade_from_brightness(fb);
                int fa = (fb < 0.35) ? A_DIM : (fb < 0.7 ? A_NORMAL : A_BOLD);
                // keep a slight green tint if available
                if (has_colors()) {
                    tui_draw_pixel_color(tui, x + 1, y, fch == ' ' ? '.' : fch, TUI_CP_FLOOR, fa);
                } else {
                    tui_draw_pixel(tui, x + 1, y, fch);
                }
            }
        }
    }
}

void renderer_draw_minimap(TUI *tui, Map *map, Camera *camera) {
    if (!tui || !tui->status_win) return;
    int win_h = getmaxy(tui->status_win);
    int win_w = getmaxx(tui->status_win);

    int origin_y = 2; // below the status line
    int origin_x = 2;
    int avail_h = win_h - 3; // exclude top status line and bottom border
    int avail_w = win_w - 4;
    if (avail_h <= 0 || avail_w <= 0) return;

    // Compute sampling steps to fit the 16x16 map
    int step_y = (MAP_HEIGHT + avail_h - 1) / avail_h; // ceil
    if (step_y < 1) step_y = 1;
    int draw_h = MAP_HEIGHT / step_y;
    if (draw_h > avail_h) draw_h = avail_h;

    int step_x = (MAP_WIDTH + avail_w - 1) / avail_w; // ceil
    if (step_x < 1) step_x = 1;
    int draw_w = MAP_WIDTH / step_x;
    if (draw_w > avail_w) draw_w = avail_w;

    // Draw the minimap tiles
    for (int my = 0; my < draw_h; my++) {
        int map_y = my * step_y;
        for (int mx = 0; mx < draw_w; mx++) {
            int map_x = mx * step_x;
            char tile = map->map[map_y][map_x];
            chtype ch = (tile == ' ') ? '.' : '#';
            int pair = (tile == ' ') ? TUI_CP_FLOOR : TUI_CP_WALL_MID;
            if (has_colors()) wattron(tui->status_win, COLOR_PAIR(pair) | ((tile==' ')?A_DIM:A_BOLD));
            mvwaddch(tui->status_win, origin_y + my, origin_x + mx, ch);
            if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(pair) | ((tile==' ')?A_DIM:A_BOLD));
        }
    }

    // Draw player indicator
    int px = (int)(camera->position.x);
    int pz = (int)(camera->position.z);
    int sx = origin_x + px / step_x;
    int sy = origin_y + pz / step_y;
    if (sx >= origin_x && sx < origin_x + draw_w && sy >= origin_y && sy < origin_y + draw_h) {
        char arrow = '>';
        // Decide arrow by major direction component
        double dx = camera->direction.x;
        double dz = camera->direction.z;
        if (fabs(dx) > fabs(dz)) arrow = (dx >= 0) ? '>' : '<';
        else arrow = (dz >= 0) ? 'v' : '^';
        if (has_colors()) wattron(tui->status_win, COLOR_PAIR(TUI_CP_WALL_NEAR) | A_BOLD);
        mvwaddch(tui->status_win, sy, sx, arrow);
        if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(TUI_CP_WALL_NEAR) | A_BOLD);
    }
}
