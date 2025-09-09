#include "dungeon.h"
#include <stdio.h>
#include <string.h>

void dungeon_init(Dungeon *dungeon) {
    dungeon->room_count = 0;
    dungeon->map.width = MAP_WIDTH;
    dungeon->map.height = MAP_HEIGHT;
    
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            dungeon->map.map[y][x] = '1';
        }
    }
}

int rooms_overlap(Room *a, Room *b) {
    return !(a->x + a->width < b->x || b->x + b->width < a->x ||
             a->y + a->height < b->y || b->y + b->height < a->y);
}

void create_room(Map *map, Room *room) {
    for (int y = room->y; y < room->y + room->height; y++) {
        for (int x = room->x; x < room->x + room->width; x++) {
            if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
                map->map[y][x] = ' ';
            }
        }
    }
}

void create_corridor(Map *map, int x1, int y1, int x2, int y2) {
    int x = x1;
    int y = y1;
    
    while (x != x2) {
        if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
            map->map[y][x] = ' ';
        }
        x += (x2 > x1) ? 1 : -1;
    }
    
    while (y != y2) {
        if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
            map->map[y][x] = ' ';
        }
        y += (y2 > y1) ? 1 : -1;
    }
    
    if (x >= 0 && x < MAP_WIDTH && y >= 0 && y < MAP_HEIGHT) {
        map->map[y][x] = ' ';
    }
}

void dungeon_generate(Dungeon *dungeon) {
    srand(time(NULL));
    
    dungeon_init(dungeon);
    
    for (int attempts = 0; attempts < MAX_ROOMS * 3; attempts++) {
        Room new_room;
        new_room.width = ROOM_MIN_SIZE + rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1);
        new_room.height = ROOM_MIN_SIZE + rand() % (ROOM_MAX_SIZE - ROOM_MIN_SIZE + 1);
        new_room.x = 1 + rand() % (MAP_WIDTH - new_room.width - 2);
        new_room.y = 1 + rand() % (MAP_HEIGHT - new_room.height - 2);
        
        int overlaps = 0;
        for (int i = 0; i < dungeon->room_count; i++) {
            if (rooms_overlap(&new_room, &dungeon->rooms[i])) {
                overlaps = 1;
                break;
            }
        }
        
        if (!overlaps && dungeon->room_count < MAX_ROOMS) {
            create_room(&dungeon->map, &new_room);
            
            if (dungeon->room_count > 0) {
                Room *prev_room = &dungeon->rooms[dungeon->room_count - 1];
                create_corridor(&dungeon->map,
                    new_room.x + new_room.width / 2,
                    new_room.y + new_room.height / 2,
                    prev_room->x + prev_room->width / 2,
                    prev_room->y + prev_room->height / 2
                );
            }
            
            dungeon->rooms[dungeon->room_count] = new_room;
            dungeon->room_count++;
        }
    }
}

Vec3 find_spawn_position(Dungeon *dungeon) {
    if (dungeon->room_count > 0) {
        Room *first_room = &dungeon->rooms[0];
        return vec3_create(
            first_room->x + first_room->width / 2,
            0.5,
            first_room->y + first_room->height / 2
        );
    }
    
    for (int y = 1; y < MAP_HEIGHT - 1; y++) {
        for (int x = 1; x < MAP_WIDTH - 1; x++) {
            if (dungeon->map.map[y][x] == ' ') {
                return vec3_create(x + 0.5, 0.5, y + 0.5);
            }
        }
    }
    
    return vec3_create(1.5, 0.5, 1.5);
}