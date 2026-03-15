#ifndef GAME_H
#define GAME_H

#include "types.h"

static bool near(int a, int b, int tolerance);
void game_init(Game *game);
void game_load_assets(Assets *assets);
void game_destroy_assets(Assets *assets);
void game_start(Game *game, Assets *assets);
void game_update(Game *game, Assets *assets);
void game_destroy(Game *game);

#endif
