#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <stdio.h>
#include "game.h"
#include "events.h"
#include "types.h"

/**
 * Initializes Allegro
 */
void game_init(Game *game) {
    game->error_code = 0;

    if (!al_init()) {
        printf("could not initialize allegro\n");
        game->error_code = 1;
        return;
    }

    if (!al_init_primitives_addon()) {
        printf("could not initialize primitives addon\n");
        game->error_code = 1;
        return;
    }

    if (!al_init_image_addon()) {
        printf("could not initialize image addon\n");
        game->error_code = 1;
        return;
    }

    if (!al_install_keyboard()) {
        printf("could not initialize keyboard\n");
        game->error_code = 1;
        return;
    }

    game->timer = al_create_timer(1.0 / 30.0);
    if (!game->timer) {
        printf("could not initialize timer\n");
        game->error_code = 1;
        return;
    }

    game->queue = al_create_event_queue();
    if (!game->queue) {
        printf("could not initialize queue\n");
        game->error_code = 1;
        return;
    }

    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

    game->display = al_create_display(1920, 1080);
    if (!game->display) {
        printf("could not initialize display\n");
        game->error_code = 1;
        return;
    }

    #ifdef DEBUG
    game->debug_font = al_create_builtin_font();
    #endif
}

/**
 * Load assets
 */
void game_load_assets(Assets *assets) {
    assets->error_code = 0;

    assets->scene = al_load_bitmap("media/scene.png");
    assets->car_right = al_load_bitmap("media/car_right1.png");
    assets->car_left = al_load_bitmap("media/car_left1.png");
    assets->player_front = al_load_bitmap("media/player_front.png");
    assets->player_back = al_load_bitmap("media/player_back.png");
    assets->player_left = al_load_bitmap("media/player_left.png");
    assets->player_right = al_load_bitmap("media/player_right.png");
    assets->player_crash_right = al_load_bitmap("media/player_crash_right.png");
    assets->player_crash_left = al_load_bitmap("media/player_crash_left.png");
    assets->player_front1 = al_load_bitmap("media/player_front1.png");
    assets->player_back1 = al_load_bitmap("media/player_back1.png");
    assets->player_left1 = al_load_bitmap("media/player_left1.png");
    assets->player_right1 = al_load_bitmap("media/player_right1.png");
    assets->player_front2 = al_load_bitmap("media/player_front2.png");
    assets->player_back2 = al_load_bitmap("media/player_back2.png");
    assets->player_left2 = al_load_bitmap("media/player_left2.png");
    assets->player_right2 = al_load_bitmap("media/player_right2.png");

    if (
        !assets->scene
        || !assets->car_right
        || !assets->car_left
        || !assets->player_front
        || !assets->player_back
        || !assets->player_right
        || !assets->player_left
        || !assets->player_crash_right
        || !assets->player_crash_left
        || !assets->player_front1
        || !assets->player_front2
        || !assets->player_back1
        || !assets->player_back2
        || !assets->player_left1
        || !assets->player_left2
        || !assets->player_right1
        || !assets->player_right2
    ) {
        printf("could not load assets");
        assets->error_code = 1;
    }
}

/**
 * Destroy assets
 */
void game_destroy_assets(Assets *assets) {
    al_destroy_bitmap(assets->scene);
    al_destroy_bitmap(assets->car_right);
    al_destroy_bitmap(assets->car_left);
    al_destroy_bitmap(assets->player_front);
    al_destroy_bitmap(assets->player_back);
    al_destroy_bitmap(assets->player_left);
    al_destroy_bitmap(assets->player_right);
    al_destroy_bitmap(assets->player_crash_right);
    al_destroy_bitmap(assets->player_crash_left);
    al_destroy_bitmap(assets->player_front1);
    al_destroy_bitmap(assets->player_front2);
    al_destroy_bitmap(assets->player_back1);
    al_destroy_bitmap(assets->player_back2);
    al_destroy_bitmap(assets->player_left1);
    al_destroy_bitmap(assets->player_left2);
    al_destroy_bitmap(assets->player_right1);
    al_destroy_bitmap(assets->player_right2);
}

/**
 * Starts the game
 */
void game_start(Game *game, Assets *assets) {
    init_event(game, assets);
}

/**
 * Main game logic to detect events
 */
void game_update(Game *game, Assets *assets) {
    ALLEGRO_EVENT event;
    al_wait_for_event(game->queue, &event);

    switch(event.type) {
    case ALLEGRO_EVENT_TIMER:
        timer_event(&event, game, assets);
        break;

    case ALLEGRO_EVENT_KEY_DOWN:
        key_down_event(&event, game, assets);
        break;

    case ALLEGRO_EVENT_DISPLAY_CLOSE:
        display_close_event(&event, game, assets);
        break;
    }
}

/**
 * Destroy the game
 */
void game_destroy(Game *game) {
    al_destroy_display(game->display);
    al_destroy_timer(game->timer);
    al_destroy_event_queue(game->queue);
    al_shutdown_font_addon();
}
