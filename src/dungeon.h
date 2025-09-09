#ifndef DUNGEON_H
#define DUNGEON_H

#include "renderer.h"
#include <time.h>
#include <stdlib.h>

#define ROOM_MIN_SIZE 4
#define ROOM_MAX_SIZE 8
#define MAX_ROOMS 10

typedef struct {
    int x, y;
    int width, height;
} Room;

typedef struct {
    Room rooms[MAX_ROOMS];
    int room_count;
    Map map;
} Dungeon;

void dungeon_init(Dungeon *dungeon);
void dungeon_generate(Dungeon *dungeon);
int rooms_overlap(Room *a, Room *b);
void create_room(Map *map, Room *room);
void create_corridor(Map *map, int x1, int y1, int x2, int y2);
Vec3 find_spawn_position(Dungeon *dungeon);

#endif