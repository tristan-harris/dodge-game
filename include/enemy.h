#pragma once

#include "raylib.h"

#define ENEMY_COLORS YELLOW, RED, BLUE, GREEN, PURPLE, SKYBLUE

typedef struct Enemy {
    Vector2 position;
    Vector2 direction;
    float speed;
    float radius;
    Color color;
    bool follow;
} Enemy;

void enemy_setup(Enemy *enemy, float min_size, float max_size, float min_speed,
                 float max_speed, bool follow);
void enemy_spawn(Enemy *enemy, int min_x, int min_y, int max_x, int max_y);
bool enemy_in_bounds(Enemy *enemy, int min_x, int min_y, int max_x, int max_y);
void enemy_move(Enemy *enemy, Vector2 player_pos, float speed_mod);
