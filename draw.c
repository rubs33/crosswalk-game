#include <stdio.h>
#include <stdlib.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_primitives.h>
#include "draw.h"
#include "events.h"
#include "types.h"

/**
 * Draw the elements of the game
 */
void game_draw(Game *game, Assets *assets) {
    game->redraw = false;

    switch (game->status) {
    case STATUS_INTRO:
        draw_intro(game, assets);
        break;
    case STATUS_STAGE1:
        draw_stage1(game, assets);
    }

    al_flip_display();
}

void draw_intro(Game *game, Assets *assets) {
    al_draw_bitmap(assets->intro_background, 0, 0, 0);
    al_draw_bitmap(assets->intro_title, 280, 400, 0);
}

void draw_stage1(Game *game, Assets *assets) {
    // Scene
    al_draw_bitmap(game->scene.img, 0, 0, 0);

    // Player + 2 cars
    Drawable *objects[3];
    objects[0] = (Drawable *)&game->player;
    objects[1] = (Drawable *)&game->car1;
    objects[2] = (Drawable *)&game->car2;

    qsort(objects, 3, sizeof(Drawable *), compare_y);
    for (int i = 0; i < 3; i++) {
        Drawable *obj = objects[i];
        al_draw_bitmap(obj->img, obj->pos.x, obj->pos.y, 0);
    }

    #ifdef DEBUG
    debug_text(game->debug_font,  0, "Player: %d x %d", game->player.pos.x, game->player.pos.y);
    debug_text(game->debug_font, 10, "Car 1:  %d x %d", game->car1.pos.x, game->car1.pos.y);
    debug_text(game->debug_font, 20, "Car 2:  %d x %d", game->car2.pos.x, game->car2.pos.y);
    #endif
}

/**
 * Compare the position Y of two values
 */
int compare_y(const void *a, const void *b) {
    Drawable *da = *(Drawable **)a;
    Drawable *db = *(Drawable **)b;

    return da->pos.y - db->pos.y;
}

#ifdef DEBUG
/**
 * Prints a debug message on screen
 */
void debug_text(ALLEGRO_FONT *font, int y, const char *fmt, ...) {
    char buffer[256];

    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    int h = al_get_font_line_height(font);
    int w = al_get_text_width(font, buffer);

    al_draw_filled_rectangle(0, y, w + 2, y + h + 2, COLOR_WHITE);
    al_draw_text(font, COLOR_BLACK, 1, y + 1, 0, buffer);
}
#endif
