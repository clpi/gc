#include "player.h"

void player_init(Player *player, Vec3 position) {
    camera_init(&player->camera, position, 0.0);
    player->health = 100;
    player->max_health = 100;
    player->experience = 0;
    player->level = 1;
}

int check_collision(Map *map, Vec3 position) {
    int x = (int)(position.x);
    int z = (int)(position.z);
    
    if (x < 0 || x >= MAP_WIDTH || z < 0 || z >= MAP_HEIGHT) {
        return 1;
    }
    
    if (map->map[z][x] != ' ') {
        return 1;
    }
    
    double dx = position.x - (x + 0.5);
    double dz = position.z - (z + 0.5);
    
    int check_positions[][2] = {
        {x-1, z}, {x+1, z}, {x, z-1}, {x, z+1},
        {x-1, z-1}, {x+1, z-1}, {x-1, z+1}, {x+1, z+1}
    };
    
    for (int i = 0; i < 8; i++) {
        int check_x = check_positions[i][0];
        int check_z = check_positions[i][1];
        
        if (check_x >= 0 && check_x < MAP_WIDTH && check_z >= 0 && check_z < MAP_HEIGHT) {
            if (map->map[check_z][check_x] != ' ') {
                double wall_dx = position.x - (check_x + 0.5);
                double wall_dz = position.z - (check_z + 0.5);
                double distance = sqrt(wall_dx * wall_dx + wall_dz * wall_dz);
                
                if (distance < COLLISION_RADIUS + 0.5) {
                    return 1;
                }
            }
        }
    }
    
    return 0;
}

void player_move_forward(Player *player, Map *map) {
    Vec3 move_vec = vec3_mul(player->camera.direction, MOVE_SPEED);
    Vec3 new_pos = vec3_add(player->camera.position, move_vec);
    
    if (!check_collision(map, new_pos)) {
        player->camera.position = new_pos;
    }
}

void player_move_backward(Player *player, Map *map) {
    Vec3 move_vec = vec3_mul(player->camera.direction, -MOVE_SPEED);
    Vec3 new_pos = vec3_add(player->camera.position, move_vec);
    
    if (!check_collision(map, new_pos)) {
        player->camera.position = new_pos;
    }
}

void player_move_left(Player *player, Map *map) {
    Vec3 left_dir = vec3_rotate_y(player->camera.direction, -PI/2);
    Vec3 move_vec = vec3_mul(left_dir, MOVE_SPEED);
    Vec3 new_pos = vec3_add(player->camera.position, move_vec);
    
    if (!check_collision(map, new_pos)) {
        player->camera.position = new_pos;
    }
}

void player_move_right(Player *player, Map *map) {
    Vec3 right_dir = vec3_rotate_y(player->camera.direction, PI/2);
    Vec3 move_vec = vec3_mul(right_dir, MOVE_SPEED);
    Vec3 new_pos = vec3_add(player->camera.position, move_vec);
    
    if (!check_collision(map, new_pos)) {
        player->camera.position = new_pos;
    }
}

void player_turn_left(Player *player) {
    player->camera.angle -= TURN_SPEED;
    camera_update_direction(&player->camera);
}

void player_turn_right(Player *player) {
    player->camera.angle += TURN_SPEED;
    camera_update_direction(&player->camera);
}