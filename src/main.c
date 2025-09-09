#include "tui.h"
#include "renderer.h"
#include "dungeon.h"
#include "player.h"
#include "math3d.h"

#include <stdio.h>
#include <stdlib.h>

static void update_status(TUI *tui, Player *player) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Pos:(%.2f, %.2f, %.2f) Angle: %.2f",
             player->camera.position.x,
             player->camera.position.y,
             player->camera.position.z,
             player->camera.angle);
    tui_update_status_hud(tui, buf, player->health, player->max_health);
}

int main(void) {
    TUI tui = {0};
    if (tui_init(&tui) != 0) {
        fprintf(stderr, "Failed to init TUI (ncurses)\n");
        return 1;
    }

    renderer_init();

    Dungeon dungeon = {0};
    dungeon_generate(&dungeon);

    Player player = {0};
    Vec3 spawn = find_spawn_position(&dungeon);
    player_init(&player, spawn);

    int running = 1;
    while (running) {
        int ch = tui_get_input(&tui);
        switch (ch) {
            case 'q':
            case 'Q':
                running = 0;
                break;
            // Light controls: yaw (J/L) and pitch (I/K), reset (R)
            case 'j':
            case 'J':
                renderer_rotate_light(-0.08, 0.0);
                break;
            case 'l':
            case 'L':
                renderer_rotate_light(0.08, 0.0);
                break;
            case 'i':
            case 'I':
                renderer_rotate_light(0.0, -0.06);
                break;
            case 'k':
            case 'K':
                renderer_rotate_light(0.0, 0.06);
                break;
            case 'r':
            case 'R':
                renderer_set_light_dir(vec3_create(-0.6, -0.3, -0.8));
                break;
            case 'w':
            case 'W':
                player_move_forward(&player, &dungeon.map);
                break;
            case 's':
            case 'S':
                player_move_backward(&player, &dungeon.map);
                break;
            case 'a':
            case 'A':
                player_move_left(&player, &dungeon.map);
                break;
            case 'd':
            case 'D':
                player_move_right(&player, &dungeon.map);
                break;
            case KEY_LEFT:
                player_turn_left(&player);
                break;
            case KEY_RIGHT:
                player_turn_right(&player);
                break;
            default:
                break;
        }

        render_frame(&tui, &player.camera, &dungeon.map);
        update_status(&tui, &player);
        renderer_draw_minimap(&tui, &dungeon.map, &player.camera);
        tui_refresh(&tui);
    }

    tui_cleanup(&tui);
    return 0;
}
