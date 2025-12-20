#include "player.h"
#include "enemy.h"
#include "raymath.h"

bool player_enemy_collision(Player *player, Enemy *enemy) {
    float distance = Vector2Distance(player->position, enemy->position);
    return distance - player->radius - enemy->radius < 0;
}
