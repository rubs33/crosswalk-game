#ifndef EVENTS_H
#define EVENTS_H

#include <allegro5/allegro5.h>
#include <allegro5/allegro_image.h>
#include "types.h"

#define NO_CRASH 0
#define DIRECTION_UP 1
#define DIRECTION_DOWN 2
#define DIRECTION_LEFT 3
#define DIRECTION_RIGHT 4

#define WALK_SPEED 8
#define WALK_DISTANCE 10

#define MIN_CAR_SPEED 10
#define MAX_CAR_SPEED 40

#define COLISION_TOLERANCE_Y 30
#define COLISION_TOLERANCE_X 10

#define STREET_INIT_Y 300
#define STREET_END_Y 530

#define CROSSWALK_INIT_X 1300
#define CROSSWALK_END_X 1800

void init_event(Game *game, Assets *assets);
void timer_event(ALLEGRO_EVENT *event, Game *game, Assets *assets);
void key_down_event(ALLEGRO_EVENT *event, Game *game, Assets *assets);
void display_close_event(ALLEGRO_EVENT *event, Game *game, Assets *assets);

bool on_crosswalk(Player *player);
bool is_crash(Car *car, Player *player);
void init_player(Player *player, int x, int y, char dir, ALLEGRO_BITMAP *img);
void init_car(Car *car, int x, int y, char dir, int speed, ALLEGRO_BITMAP *img);
void update_car(Car *car, Scene *scene);

#endif
