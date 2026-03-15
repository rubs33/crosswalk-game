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
    Scene *scene = &game->scene;
    Player *player = &game->player;
    Car *car1 = &game->car1;
    Car *car2 = &game->car2;

    game->redraw = true;

    update_car(car1, scene);
    update_car(car2, scene);

    // Crash detection
    if (is_crash(car1, player)) {
        car1->speed = 0;
        if (!on_crosswalk(player)) {
            player->crash = car1->direction;
        }
    } else if (is_crash(car2, player)) {
        car2->speed = 0;
        if (!on_crosswalk(player)) {
            player->crash = car2->direction;
        }
    } else {
        if (!car1->speed) {
            car1->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
        }
        if (!car2->speed) {
            car2->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
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
void update_car(Car *car, Scene *scene) {
    if (car->direction == DIRECTION_LEFT) {
        car->pos.x -= car->speed;
        if (car->pos.x < -car->size.w) {
            car->pos.x = scene->xmax + (rand() % scene->xmax);
            car->pos.y = STREET_INIT_Y + rand() % (STREET_END_Y - STREET_INIT_Y);
            car->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
        }
    } else {
        car->pos.x += car->speed;
        if (car->pos.x > scene->xmax) {
            car->pos.x = -car->size.w - (rand() % scene->xmax);
            car->pos.y = STREET_INIT_Y + rand() % (STREET_END_Y - STREET_INIT_Y);
            car->speed = MIN_CAR_SPEED + (rand() % (MAX_CAR_SPEED - MIN_CAR_SPEED));
        }
    }
}

