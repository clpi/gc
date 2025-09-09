#ifndef TUI_H
#define TUI_H

#include <ncurses.h>

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 24
#define RENDER_WIDTH 60
#define RENDER_HEIGHT 20
#define STATUS_HEIGHT 8

typedef struct {
    WINDOW *main_win;
    WINDOW *render_win;
    WINDOW *status_win;
    int width;
    int height;
} TUI;

int tui_init(TUI *tui);
void tui_cleanup(TUI *tui);
void tui_clear_render(TUI *tui);
void tui_draw_pixel(TUI *tui, int x, int y, char ch);
void tui_draw_pixel_color(TUI *tui, int x, int y, char ch, short color_pair, int attrs);
void tui_draw_string(TUI *tui, int x, int y, const char *str);
void tui_update_status(TUI *tui, const char *status);
void tui_update_status_hud(TUI *tui, const char *status, int hp, int max_hp);
void tui_refresh(TUI *tui);
int tui_get_input(TUI *tui);

// TUI color pair identifiers
enum {
    TUI_CP_TITLE = 1,
    TUI_CP_BORDER,
    TUI_CP_STATUS,
    TUI_CP_FLOOR,
    TUI_CP_WALL_FAR,
    TUI_CP_WALL_MID,
    TUI_CP_WALL_NEAR
};

#endif
