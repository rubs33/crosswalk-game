#include <stdio.h>
#include "draw.h"
#include "game.h"

int main() {
    Game game;
    Assets assets;

    printf("Initializing game\n");
    game_init(&game);
    if (game.error_code > 0) {
        return game.error_code;
    }

    printf("Loading assets\n");
    game_load_assets(&assets);
    if (assets.error_code > 0) {
        return assets.error_code;
    }

    printf("Starting game\n");
    game_start(&game, &assets);
    while (game.active) {
        game_update(&game, &assets);
        game_draw(&game);
    }

    printf("Finishing game\n");
    game_destroy(&game);
    game_destroy_assets(&assets);

    return 0;
}
