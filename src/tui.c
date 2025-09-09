#include "tui.h"
#include <stdlib.h>
#include <string.h>

int tui_init(TUI *tui) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    timeout(16);
    curs_set(0);
    
    if (has_colors()) {
        start_color();
        use_default_colors();
        // Foreground-only color pairs (background -1 keeps terminal default)
        init_pair(TUI_CP_TITLE, COLOR_CYAN, -1);
        init_pair(TUI_CP_BORDER, COLOR_WHITE, -1);
        init_pair(TUI_CP_STATUS, COLOR_YELLOW, -1);
        init_pair(TUI_CP_FLOOR, COLOR_GREEN, -1);
        init_pair(TUI_CP_WALL_FAR, COLOR_CYAN, -1);
        init_pair(TUI_CP_WALL_MID, COLOR_YELLOW, -1);
        init_pair(TUI_CP_WALL_NEAR, COLOR_WHITE, -1);
    }
    
    getmaxyx(stdscr, tui->height, tui->width);
    
    tui->main_win = stdscr;
    tui->render_win = newwin(RENDER_HEIGHT, RENDER_WIDTH, 2, 2);
    // Place status panel near bottom; overlap with render is fine in ncurses
    int status_y = tui->height - (STATUS_HEIGHT + 2);
    if (status_y < 2) status_y = 2; // guard tiny terminals
    tui->status_win = newwin(STATUS_HEIGHT, tui->width - 4, status_y, 2);
    
    if (!tui->render_win || !tui->status_win) {
        tui_cleanup(tui);
        return -1;
    }
    
    if (has_colors()) wattron(tui->render_win, COLOR_PAIR(TUI_CP_BORDER));
    box(tui->render_win, 0, 0);
    if (has_colors()) wattroff(tui->render_win, COLOR_PAIR(TUI_CP_BORDER));

    if (has_colors()) wattron(tui->status_win, COLOR_PAIR(TUI_CP_BORDER));
    box(tui->status_win, 0, 0);
    if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(TUI_CP_BORDER));
    
    if (has_colors()) wattron(tui->main_win, COLOR_PAIR(TUI_CP_TITLE) | A_BOLD);
    mvwprintw(tui->main_win, 0, 2, "Dungeon3D - 3D Roguelike");
    if (has_colors()) wattroff(tui->main_win, COLOR_PAIR(TUI_CP_TITLE) | A_BOLD);
    if (has_colors()) wattron(tui->status_win, COLOR_PAIR(TUI_CP_STATUS));
    mvwprintw(tui->status_win, 1, 2, "Use WASD/Arrows, Q quit");
    if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(TUI_CP_STATUS));
    
    refresh();
    wrefresh(tui->render_win);
    wrefresh(tui->status_win);
    
    return 0;
}

void tui_cleanup(TUI *tui) {
    if (tui->render_win) delwin(tui->render_win);
    if (tui->status_win) delwin(tui->status_win);
    endwin();
}

void tui_clear_render(TUI *tui) {
    wclear(tui->render_win);
    if (has_colors()) wattron(tui->render_win, COLOR_PAIR(TUI_CP_BORDER));
    box(tui->render_win, 0, 0);
    if (has_colors()) wattroff(tui->render_win, COLOR_PAIR(TUI_CP_BORDER));
}

void tui_draw_pixel(TUI *tui, int x, int y, char ch) {
    if (x >= 1 && x < RENDER_WIDTH - 1 && y >= 1 && y < RENDER_HEIGHT - 1) {
        mvwaddch(tui->render_win, y, x, ch);
    }
}

void tui_draw_pixel_color(TUI *tui, int x, int y, char ch, short color_pair, int attrs) {
    if (x >= 1 && x < RENDER_WIDTH - 1 && y >= 1 && y < RENDER_HEIGHT - 1) {
        if (has_colors()) wattron(tui->render_win, COLOR_PAIR(color_pair) | attrs);
        mvwaddch(tui->render_win, y, x, ch);
        if (has_colors()) wattroff(tui->render_win, COLOR_PAIR(color_pair) | attrs);
    }
}

void tui_draw_string(TUI *tui, int x, int y, const char *str) {
    if (x >= 1 && x < RENDER_WIDTH - 1 && y >= 1 && y < RENDER_HEIGHT - 1) {
        mvwprintw(tui->render_win, y, x, "%s", str);
    }
}

void tui_update_status(TUI *tui, const char *status) {
    wclear(tui->status_win);
    if (has_colors()) wattron(tui->status_win, COLOR_PAIR(TUI_CP_BORDER));
    box(tui->status_win, 0, 0);
    if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(TUI_CP_BORDER));
    if (has_colors()) wattron(tui->status_win, COLOR_PAIR(TUI_CP_STATUS));
    mvwprintw(tui->status_win, 1, 2, "%s", status);
    if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(TUI_CP_STATUS));
}

static void draw_bar(WINDOW *win, int y, int x, int width, int value, int max_value) {
    if (width <= 2) return;
    if (max_value <= 0) max_value = 1;
    double ratio = (double)value / (double)max_value;
    if (ratio < 0) ratio = 0;
    if (ratio > 1) ratio = 1;
    int filled = (int)((width - 2) * ratio);
    // Border
    mvwaddch(win, y, x, '[');
    mvwaddch(win, y, x + width - 1, ']');
    // Fill
    int color_pair = TUI_CP_WALL_NEAR;
    if (ratio < 0.33) color_pair = TUI_CP_WALL_FAR; // low -> cyan
    else if (ratio < 0.66) color_pair = TUI_CP_WALL_MID; // mid -> yellow
    else color_pair = TUI_CP_WALL_NEAR; // high -> white
    if (has_colors()) wattron(win, COLOR_PAIR(color_pair) | A_BOLD);
    for (int i = 0; i < filled; i++) mvwaddch(win, y, x + 1 + i, '#');
    if (has_colors()) wattroff(win, COLOR_PAIR(color_pair) | A_BOLD);
    for (int i = filled; i < width - 2; i++) mvwaddch(win, y, x + 1 + i, ' ');
}

void tui_update_status_hud(TUI *tui, const char *status, int hp, int max_hp) {
    wclear(tui->status_win);
    if (has_colors()) wattron(tui->status_win, COLOR_PAIR(TUI_CP_BORDER));
    box(tui->status_win, 0, 0);
    if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(TUI_CP_BORDER));
    // Status line
    if (has_colors()) wattron(tui->status_win, COLOR_PAIR(TUI_CP_STATUS));
    mvwprintw(tui->status_win, 1, 2, "%s", status);
    if (has_colors()) wattroff(tui->status_win, COLOR_PAIR(TUI_CP_STATUS));
    // Health bar on the right side
    int maxw = getmaxx(tui->status_win);
    const char *label = " HP ";
    int bar_w = 20;
    int bar_x = maxw - (bar_w + 6);
    if (bar_x < 10) bar_x = 10;
    mvwprintw(tui->status_win, 1, bar_x - (int)strlen(label), "%s", label);
    draw_bar(tui->status_win, 1, bar_x, bar_w, hp, max_hp);
}

void tui_refresh(TUI *tui) {
    wrefresh(tui->render_win);
    wrefresh(tui->status_win);
    refresh();
}

int tui_get_input(TUI *tui) {
    (void)tui;
    return getch();
}
