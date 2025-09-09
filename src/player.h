#ifndef PLAYER_H
#define PLAYER_H

#include "math3d.h"
#include "renderer.h"

#define MOVE_SPEED 0.1
#define TURN_SPEED 0.05
#define COLLISION_RADIUS 0.2

typedef struct {
    Camera camera;
    int health;
    int max_health;
    int experience;
    int level;
} Player;

void player_init(Player *player, Vec3 position);
void player_move_forward(Player *player, Map *map);
void player_move_backward(Player *player, Map *map);
void player_move_left(Player *player, Map *map);
void player_move_right(Player *player, Map *map);
void player_turn_left(Player *player);
void player_turn_right(Player *player);
int check_collision(Map *map, Vec3 position);

#endif