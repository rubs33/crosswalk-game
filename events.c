#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include "events.h"
#include "types.h"

void init_event(Game *game, Assets *assets) {
    srand(time(NULL));

    game->active = true;
    game->redraw = true;
    game->status = STATUS_INTRO;
    game->intro_music_started = false;

    game->scene.xmin = 0;
    game->scene.xmax = 1920;
    game->scene.ymin = 240;
    game->scene.ymax = 1080;
    game->scene.img = assets->scene;

    init_player(&game->player, 700, 240, DIRECTION_DOWN, assets->player_front);

    init_car(&game->car1, game->scene.xmax, 300, DIRECTION_LEFT, 30, assets->car_left);
    init_car(&game->car2, -458, 500, DIRECTION_RIGHT, 20, assets->car_right);

    al_register_event_source(game->queue, al_get_keyboard_event_source());
    al_register_event_source(game->queue, al_get_display_event_source(game->display));
    al_register_event_source(game->queue, al_get_timer_event_source(game->timer));
    al_start_timer(game->timer);
}

/**
 * Timer event
 */
void timer_event(ALLEGRO_EVENT *event, Game *game, Assets *assets) {
    game->redraw = true;

    if (game->status == STATUS_INTRO) {
        intro_timer_event(event, game, assets);
    }

    if (game->status == STATUS_STAGE1) {
        stage1_timer_event(event, game, assets);
    }
}

void intro_timer_event(ALLEGRO_EVENT *event, Game *game, Assets *assets) {
    if (!game->intro_music_started) {
        game->intro_music_started = true;
        al_play_sample(assets->intro_music, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &game->intro_music_id);
    }
}

void stage1_timer_event(ALLEGRO_EVENT *event, Game *game, Assets *assets) {
    Scene *scene = &game->scene;
    Player *player = &game->player;
    Car *car1 = &game->car1;
    Car *car2 = &game->car2;

    update_car(car1, scene, assets);
    update_car(car2, scene, assets);

    // Crash detection
    if (is_crash(car1, player)) {
        car1->speed = 0;
        if (!on_crosswalk(player)) {
            if (player->crash == NO_CRASH) {
                al_play_sample(assets->crash_scream_sound, 1.0, ((float) player->pos.x / (1920.0f - player->size.w)) * 2.0f - 1.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }
            player->crash = car1->direction;
        }
    } else if (is_crash(car2, player)) {
        car2->speed = 0;
        if (!on_crosswalk(player)) {
            if (player->crash == NO_CRASH) {
                al_play_sample(assets->crash_scream_sound, 1.0, ((float) player->pos.x / (1920.0f - player->size.w)) * 2.0f - 1.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
            }
            player->crash = car2->direction;
        }
    } else {
        if (!car1->speed) {
            car1->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
            car1->sound_started = false;
        }
        if (!car2->speed) {
            car2->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
            car2->sound_started = false;
        }
        player->crash = NO_CRASH;
    }

    // Player
    ALLEGRO_KEYBOARD_STATE state;
    al_get_keyboard_state(&state);
    if (al_key_down(&state, ALLEGRO_KEY_LEFT)) {
        player->pos.x -= WALK_DISTANCE;
        player->walking = true;
    } else if (al_key_down(&state, ALLEGRO_KEY_RIGHT)) {
        player->pos.x += WALK_DISTANCE;
        player->walking = true;
    } else if (al_key_down(&state, ALLEGRO_KEY_UP)) {
        player->pos.y -= WALK_DISTANCE;
        player->walking = true;
    } else if (al_key_down(&state, ALLEGRO_KEY_DOWN)) {
        player->pos.y += WALK_DISTANCE;
        player->walking = true;
    } else {
        player->walking = false;
    }

    // Player step
    if (player->walking) {
        player->frame_walk++;
        if (player->frame_walk >= WALK_SPEED) {
            player->step = !player->step;
            player->frame_walk = 0;
            al_play_sample(assets->step_sound, 1.0, ((float) player->pos.x / (1920.0f - player->size.w)) * 2.0f - 1.0f, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        }
    } else {
        player->frame_walk = 0;
    }

    // Scene limits
    if (player->pos.x < scene->xmin) {
        player->pos.x = scene->xmin;
    } else if (player->pos.x > (scene->xmax - player->size.w)) {
        player->pos.x = scene->xmax - player->size.w;
    }
    if (player->pos.y < scene->ymin) {
        player->pos.y = scene->ymin;
    } else if (player->pos.y > (scene->ymax - player->size.h)) {
        player->pos.y = scene->ymax - player->size.h;
    }

    // Detect player image
    if (player->crash == DIRECTION_RIGHT) {
        player->img = assets->player_crash_right;
    } else if (player->crash == DIRECTION_LEFT) {
        player->img = assets->player_crash_left;
    } else if (player->walking) {
        switch (player->direction) {
        case DIRECTION_UP:
            player->img = player->step ? assets->player_back1 : assets->player_back2;
            break;
        case DIRECTION_DOWN:
            player->img = player->step ? assets->player_front1 : assets->player_front2;
            break;
        case DIRECTION_RIGHT:
            player->img = player->step ? assets->player_right1 : assets->player_right2;
            break;
        case DIRECTION_LEFT:
            player->img = player->step ? assets->player_left1 : assets->player_left2;
            break;
        }
    } else {
        switch (player->direction) {
        case DIRECTION_UP:
            player->img = assets->player_back;
            break;
        case DIRECTION_DOWN:
            player->img = assets->player_front;
            break;
        case DIRECTION_RIGHT:
            player->img = assets->player_right;
            break;
        case DIRECTION_LEFT:
            player->img = assets->player_left;
            break;
        }
    }
}

/**
 * Key-down event
 */
void key_down_event(ALLEGRO_EVENT *event, Game *game, Assets *assets) {
    switch (game->status) {
    case STATUS_INTRO:
        intro_key_down_event(event, game, assets);
        break;
    case STATUS_STAGE1:
        stage1_key_down_event(event, game, assets);
        break;
    }
}

void intro_key_down_event(ALLEGRO_EVENT *event, Game *game, Assets *assets) {
    if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
        game->active = false;
    } else if (event->keyboard.keycode == ALLEGRO_KEY_ENTER) {
        game->status = STATUS_STAGE1;
    } else if (event->keyboard.keycode == ALLEGRO_KEY_SPACE) {
        al_stop_sample(&game->intro_music_id);
        game->status = STATUS_STAGE1;
    }
}

void stage1_key_down_event(ALLEGRO_EVENT *event, Game *game, Assets *assets) {
    if (event->keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
        game->active = false;
    } else if (event->keyboard.keycode == ALLEGRO_KEY_LEFT) {
        game->player.direction = DIRECTION_LEFT;
    } else if (event->keyboard.keycode == ALLEGRO_KEY_RIGHT) {
        game->player.direction = DIRECTION_RIGHT;
    } else if (event->keyboard.keycode == ALLEGRO_KEY_UP) {
        game->player.direction = DIRECTION_UP;
    } else if (event->keyboard.keycode == ALLEGRO_KEY_DOWN) {
        game->player.direction = DIRECTION_DOWN;
    }

    // Talks
    if (event->keyboard.keycode == ALLEGRO_KEY_O) {
        al_play_sample(assets->talk_hi_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (event->keyboard.keycode == ALLEGRO_KEY_T) {
        al_play_sample(assets->talk_how_are_you_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (event->keyboard.keycode == ALLEGRO_KEY_C) {
        al_play_sample(assets->talk_how_day_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (event->keyboard.keycode == ALLEGRO_KEY_E) {
        al_play_sample(assets->talk_im_fine_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    } else if (event->keyboard.keycode == ALLEGRO_KEY_V) {
        al_play_sample(assets->talk_did_you_like_sound, 1.0, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
    }
}

/**
 * Display close event
 */
void display_close_event(ALLEGRO_EVENT *event, Game *game, Assets *assets) {
    game->active = false;
}

/**
 * Checks whether two values are near, considering a tolerance
 */
static bool near(int a, int b, int tolerance) {
    return abs(a - b) <= tolerance;
}

/**
 * Checks whether the player is on the crosswalk
 */
bool on_crosswalk(Player *player) {
    return player->pos.x > CROSSWALK_INIT_X
        && player->pos.x < CROSSWALK_END_X;
}

/**
 * Checks whether the car hit the player
 */
bool is_crash(Car *car, Player *player) {
    int player_bottom = player->pos.y + player->size.h;
    int car_bottom = car->pos.y + car->size.h;

    if (!near(player_bottom, car_bottom, COLISION_TOLERANCE_Y)) {
        return false;
    }

    if (car->direction == DIRECTION_LEFT) {
        return near(player->pos.x + player->size.w, car->pos.x, COLISION_TOLERANCE_X);
    } else if (car->direction == DIRECTION_RIGHT) {
        return near(player->pos.x, car->pos.x + car->size.w, COLISION_TOLERANCE_X);
    }

    return false;
}

/**
 * Initializes the player
 */
void init_player(Player *player, int x, int y, char dir, ALLEGRO_BITMAP *img) {
    player->pos.x = x;
    player->pos.y = y;
    player->direction = dir;
    player->walking = false;
    player->crash = NO_CRASH;
    player->frame_walk = 0;
    player->step = 0;
    player->size.w = 100;
    player->size.h = 200;
    player->img = img;
}

/**
 * Initializes a car
 */
void init_car(Car *car, int x, int y, char dir, int speed, ALLEGRO_BITMAP *img) {
    car->pos.x = x;
    car->pos.y = y;
    car->size.w = 458;
    car->size.h = 248;
    car->direction = dir;
    car->speed = speed;
    car->img = img;
}

/**
 * Update the car position
 */
void update_car(Car *car, Scene *scene, Assets *assets) {
    if (!car->sound_started) {
        car->sound_started = true;
        al_play_sample(assets->car_sound, 1.4, 0.0, 0.5f + ((float) car->speed - 10.0f) * 1.5f / 30.0f, ALLEGRO_PLAYMODE_ONCE, NULL);
    }

    if (car->direction == DIRECTION_LEFT) {
        car->pos.x -= car->speed;
        if (car->pos.x < -car->size.w) {
            car->pos.x = scene->xmax + (rand() % scene->xmax);
            car->pos.y = STREET_INIT_Y + rand() % (STREET_END_Y - STREET_INIT_Y);
            car->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
            car->sound_started = false;
        }
    } else {
        car->pos.x += car->speed;
        if (car->pos.x > scene->xmax) {
            car->pos.x = -car->size.w - (rand() % scene->xmax);
            car->pos.y = STREET_INIT_Y + rand() % (STREET_END_Y - STREET_INIT_Y);
            car->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
            car->sound_started = false;
        }
    }
}

