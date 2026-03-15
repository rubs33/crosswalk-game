#ifndef DRAW_H
#define DRAW_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include "types.h"

#define COLOR_BLACK al_map_rgb(0, 0, 0)
#define COLOR_WHITE al_map_rgb(255, 255, 255)

void game_draw(Game *game, Assets *assets);

void draw_intro(Game *game, Assets *assets);
void draw_stage1(Game *game, Assets *assets);

int compare_y(const void *a, const void *b);

#ifdef DEBUG
void debug_text(ALLEGRO_FONT *font, int y, const char *fmt, ...);
#endif

#endif
