#ifndef TYPES_H
#define TYPES_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>

typedef struct {
    int x;
    int y;
} Position;

typedef struct {
    int w;
    int h;
} Size;

typedef struct {
    int xmin;
    int xmax;
    int ymin;
    int ymax;
    ALLEGRO_BITMAP *img;
} Scene;

typedef struct {
    Position pos;
    Size size;
    ALLEGRO_BITMAP *img;
    unsigned char direction;
    bool walking;
    unsigned char crash;
    unsigned char frame_walk;
    unsigned char step;
} Player;

typedef struct {
    Position pos;
    Size size;
    ALLEGRO_BITMAP *img;
    unsigned int speed;
    char direction;
} Car;

typedef struct {
    Position pos;
    Size size;
    ALLEGRO_BITMAP *img;
} Drawable;

typedef struct {
    int error_code;

    ALLEGRO_BITMAP *scene;

    ALLEGRO_BITMAP *car_left;
    ALLEGRO_BITMAP *car_right;

    ALLEGRO_BITMAP *player_front;
    ALLEGRO_BITMAP *player_back;
    ALLEGRO_BITMAP *player_left;
    ALLEGRO_BITMAP *player_right;

    ALLEGRO_BITMAP *player_front1;
    ALLEGRO_BITMAP *player_front2;

    ALLEGRO_BITMAP *player_back1;
    ALLEGRO_BITMAP *player_back2;

    ALLEGRO_BITMAP *player_left1;
    ALLEGRO_BITMAP *player_left2;

    ALLEGRO_BITMAP *player_right1;
    ALLEGRO_BITMAP *player_right2;

    ALLEGRO_BITMAP *player_crash_left;
    ALLEGRO_BITMAP *player_crash_right;

} Assets;

typedef struct {
    bool active;
    bool redraw;
    int error_code;

    ALLEGRO_TIMER* timer;
    ALLEGRO_EVENT_QUEUE* queue;
    ALLEGRO_DISPLAY* display;
    ALLEGRO_FONT *debug_font;

    Scene scene;
    Player player;
    Car car1;
    Car car2;
} Game;

#endif
