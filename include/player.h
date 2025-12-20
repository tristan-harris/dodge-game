#pragma once

#include "enemy.h"
#include "raylib.h"

#define PLAYER_RADIUS 10
#define PLAYER_COLOR RAYWHITE

typedef struct Player {
    Vector2 position;
    float radius;
    Color color;
} Player;

bool player_enemy_collision(Player *player, Enemy *enemy);
