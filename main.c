#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>

#define NO_CRASH 0
#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

#define WALK_SPEED 8
#define WALK_DISTANCE 10

#define COLISION_TOLERANCE 30

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    ALLEGRO_BITMAP *img;
} Scene;

typedef struct {
    Position pos;
    unsigned int width;
    unsigned int height;
    ALLEGRO_BITMAP *img;
    unsigned char direction;
    bool walking;
    unsigned char crash;
    unsigned char frameWalk;
    unsigned char step;
} Player;

typedef struct {
    Position pos;
    unsigned int width;
    unsigned int height;
    ALLEGRO_BITMAP *img;
    unsigned int speed;
    char direction;
} Car;

typedef struct {
    Position pos;
    unsigned int width;
    unsigned int height;
    ALLEGRO_BITMAP *img;
} Drawable;

static bool near(int a, int b) {
    return abs(a - b) <= COLISION_TOLERANCE;
}

bool on_crosswalk(Player player) {
    int crosswalk_left = 1300;
    int crosswalk_right = 1800;

    return player.pos.x > crosswalk_left && player.pos.x < crosswalk_right;
}

bool is_crash(Car car, Player player) {
    int player_bottom = player.pos.y + player.height;
    int car_bottom = car.pos.y + car.height;

    if (!near(player_bottom, car_bottom)) {
        return false;
    }

    if (car.direction == DIRECTION_LEFT) {
        int player_right = player.pos.x + player.width;
        return near(player_right, car.pos.x);
    } else if (car.direction == DIRECTION_RIGHT) {
        int car_right = car.pos.x + car.width;
        return near(player.pos.x, car_right);
    }

    return false;
}

int compare_y(const void *a, const void *b)
{
    Drawable *da = *(Drawable **)a;
    Drawable *db = *(Drawable **)b;

    return da->pos.y - db->pos.y;
}

int main() {
    srand(time(NULL));

    if (!al_init()) {
        printf("couldn't initialize allegro\n");
        return 1;
    }

    if (!al_init_primitives_addon()) {
        printf("could't initialize primitives addon\n");
        return 1;
    }

    if (!al_init_image_addon()) {
        printf("could't initialize image addon\n");
        return 1;
    }

    if (!al_install_keyboard()) {
        printf("couldn't initialize keyboard\n");
        return 1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 30.0);
    if (!timer) {
        printf("couldn't initialize timer\n");
        return 1;
    }

    ALLEGRO_EVENT_QUEUE* queue = al_create_event_queue();
    if (!queue) {
        printf("couldn't initialize queue\n");
        return 1;
    }

    /* al_set_new_display_flags(ALLEGRO_FULLSCREEN); */
    al_set_new_display_flags(ALLEGRO_FULLSCREEN_WINDOW);

    ALLEGRO_DISPLAY* display = al_create_display(1920, 1080);
    if (!display) {
        printf("couldn't initialize display\n");
        return 1;
    }

    al_register_event_source(queue, al_get_keyboard_event_source());
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));

    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT event;
    ALLEGRO_KEYBOARD_STATE state;

    al_start_timer(timer);

    ALLEGRO_BITMAP *car_right1 = al_load_bitmap("media/car_right1.png");
    ALLEGRO_BITMAP *car_left1 = al_load_bitmap("media/car_left1.png");

    ALLEGRO_BITMAP *player_front = al_load_bitmap("media/player_front.png");
    ALLEGRO_BITMAP *player_back = al_load_bitmap("media/player_back.png");
    ALLEGRO_BITMAP *player_left = al_load_bitmap("media/player_left.png");
    ALLEGRO_BITMAP *player_right = al_load_bitmap("media/player_right.png");

    ALLEGRO_BITMAP *player_crash_right = al_load_bitmap("media/player_crash_right.png");
    ALLEGRO_BITMAP *player_crash_left = al_load_bitmap("media/player_crash_left.png");

    ALLEGRO_BITMAP *player_front1 = al_load_bitmap("media/player_front1.png");
    ALLEGRO_BITMAP *player_back1 = al_load_bitmap("media/player_back1.png");
    ALLEGRO_BITMAP *player_left1 = al_load_bitmap("media/player_left1.png");
    ALLEGRO_BITMAP *player_right1 = al_load_bitmap("media/player_right1.png");

    ALLEGRO_BITMAP *player_front2 = al_load_bitmap("media/player_front2.png");
    ALLEGRO_BITMAP *player_back2 = al_load_bitmap("media/player_back2.png");
    ALLEGRO_BITMAP *player_left2 = al_load_bitmap("media/player_left2.png");
    ALLEGRO_BITMAP *player_right2 = al_load_bitmap("media/player_right2.png");

    Scene scene = {
        .xmin = 0,
        .xmax = 1920,
        .ymin = 240,
        .ymax = 1080,
        .img = al_load_bitmap("media/scene.png")
    };

    Player player = {
        .pos = {
            .x = 760,
            .y = 240
        },
        .direction = DIRECTION_DOWN,
        .walking = false,
        .crash = 0,
        .frameWalk = 0,
        .step = 0,
        .width = 100,
        .height = 200,
        .img = player_front
    };

    Car car1 = {
        .pos = {
            .x = scene.xmax,
            .y = 300
        },
        .width = 458,
        .height = 248,
        .speed = 30,
        .direction = DIRECTION_LEFT,
        .img = car_left1
    };

    Car car2 = {
        .pos = {
            .x = -458,
            .y = 500
        },
        .width = 458,
        .height = 248,
        .speed = 20,
        .direction = DIRECTION_RIGHT,
        .img = car_right1
    };

    while (1) {
        al_wait_for_event(queue, &event);

        switch(event.type) {
            case ALLEGRO_EVENT_TIMER:
                redraw = true;

                // Car 1
                if (car1.direction == DIRECTION_LEFT) {
                    car1.pos.x -= car1.speed;
                    if (car1.pos.x < (int) (-1 * (int) car1.width)) {
                        car1.pos.x = scene.xmax + (rand() % scene.xmax);
                        car1.pos.y = 300 + rand() % 230;
                        car1.speed = 10 + (rand() % 30);
                    }
                } else {
                    car1.pos.x += car1.speed;
                    if (car1.pos.x > scene.xmax) {
                        car1.pos.x = (-1 * (int) car1.width) - (rand() % scene.xmax);
                        car1.pos.y = 300 + rand() % 230;
                        car1.speed = 10 + (rand() % 30);
                    }
                }

                // Car 2
                if (car2.direction == DIRECTION_LEFT) {
                    car2.pos.x -= car2.speed;
                    if (car2.pos.x < (int) (-1 * (int) car2.width)) {
                        car2.pos.x = scene.xmax + (rand() % scene.xmax);
                        car2.pos.y = 300 + rand() % 230;
                        car2.speed = 10 + (rand() % 30);
                    }
                } else {
                    car2.pos.x += car2.speed;
                    if (car2.pos.x > scene.xmax) {
                        car2.pos.x = -458 - (rand() % scene.xmax);
                        car2.pos.y = 300 + rand() % 230;
                        car2.speed = 10 + (rand() % 30);
                    }
                }

                // Crash detection
                if (is_crash(car1, player)) {
                    car1.speed = 0;
                    if (!on_crosswalk(player)) {
                        player.crash = car1.direction;
                    }
                } else if (is_crash(car2, player)) {
                    car2.speed = 0;
                    if (!on_crosswalk(player)) {
                        player.crash = car2.direction;
                    }
                } else {
                    car1.speed = 10 + (rand() % 30);
                    car2.speed = 10 + (rand() % 30);
                    player.crash = NO_CRASH;
                }

                // Player
                al_get_keyboard_state(&state);
                if (al_key_down(&state, ALLEGRO_KEY_LEFT)) {
                    player.pos.x -= player.pos.x > WALK_DISTANCE ? WALK_DISTANCE : 0;
                    player.walking = true;
                } else if (al_key_down(&state, ALLEGRO_KEY_RIGHT)) {
                    player.pos.x += WALK_DISTANCE;
                    player.walking = true;
                } else if (al_key_down(&state, ALLEGRO_KEY_UP)) {
                    player.pos.y -= player.pos.y > WALK_DISTANCE ? WALK_DISTANCE : 0;
                    player.walking = true;
                } else if (al_key_down(&state, ALLEGRO_KEY_DOWN)) {
                    player.pos.y += WALK_DISTANCE;
                    player.walking = true;
                } else {
                    player.walking = false;
                }

                // Scene limits
                if (player.pos.x < scene.xmin) {
                    player.pos.x = scene.xmin;
                } else if (player.pos.x > (scene.xmax - player.width)) {
                    player.pos.x = scene.xmax - player.width;
                }
                if (player.pos.y < scene.ymin) {
                    player.pos.y = scene.ymin;
                } else if (player.pos.y > (scene.ymax - player.height)) {
                    player.pos.y = scene.ymax - player.height;
                }

                // Detect player image
                if (player.crash == DIRECTION_RIGHT) {
                    player.img = player_crash_right;
                } else if (player.crash == DIRECTION_LEFT) {
                    player.img = player_crash_left;
                } else if (player.walking) {
                    player.frameWalk++;
                    if (player.frameWalk >= WALK_SPEED) {
                        player.step = !player.step;
                        player.frameWalk = 0;
                    }
                    switch (player.direction) {
                        case DIRECTION_UP:
                            player.img = player.step ? player_back1 : player_back2;
                            break;
                        case DIRECTION_DOWN:
                            player.img = player.step ? player_front1 : player_front2;
                            break;
                        case DIRECTION_RIGHT:
                            player.img = player.step ? player_right1 : player_right2;
                            break;
                        case DIRECTION_LEFT:
                            player.img = player.step ? player_left1 : player_left2;
                            break;
                    }
                } else {
                    player.frameWalk = 0;
                    switch (player.direction) {
                        case DIRECTION_UP:
                            player.img = player_back;
                            break;
                        case DIRECTION_DOWN:
                            player.img = player_front;
                            break;
                        case DIRECTION_RIGHT:
                            player.img = player_right;
                            break;
                        case DIRECTION_LEFT:
                            player.img = player_left;
                            break;
                    }
                }
                break;

            case ALLEGRO_EVENT_KEY_DOWN:
                if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
                    done = true;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_LEFT) {
                    player.direction = DIRECTION_LEFT;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_RIGHT) {
                    player.direction = DIRECTION_RIGHT;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_UP) {
                    player.direction = DIRECTION_UP;
                } else if (event.keyboard.keycode == ALLEGRO_KEY_DOWN) {
                    player.direction = DIRECTION_DOWN;
                }

                break;
            case ALLEGRO_EVENT_DISPLAY_CLOSE:
                done = true;
                break;
        }

        if (done) {
            break;
        }

        if (redraw && al_is_event_queue_empty(queue)) {
            al_clear_to_color(al_map_rgb(0, 0, 0));

            al_draw_bitmap(scene.img, 0, 0, 0);

            Drawable *objects[3];

            objects[0] = (Drawable *)&player;
            objects[1] = (Drawable *)&car1;
            objects[2] = (Drawable *)&car2;

            qsort(objects, 3, sizeof(Drawable *), compare_y);

            for (int i = 0; i < 3; i++) {
                Drawable *obj = objects[i];
                al_draw_bitmap(obj->img, obj->pos.x, obj->pos.y, 0);
            }

            al_flip_display();
            redraw = false;
        }
    }

    al_destroy_display(display);
    al_destroy_timer(timer);
    al_destroy_event_queue(queue);

    return 0;
}
