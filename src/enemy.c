#include "enemy.h"
#include "raymath.h"
#include "util.h"

static Color enemy_colors[] = {ENEMY_COLORS};

static Color enemy_get_random_color(void) {
    return enemy_colors[random_int(0, ARRAY_LEN(enemy_colors) - 1)];
}

void enemy_setup(Enemy *enemy, float min_size, float max_size, float min_speed,
                 float max_speed, bool follow) {
    enemy->radius = random_float(min_size, max_size);
    enemy->speed = random_float(min_speed, max_speed);
    enemy->follow = follow;
}

void enemy_spawn(Enemy *enemy, int min_x, int min_y, int max_x, int max_y) {
    // whether to (re)spawn along x-axis (as opposed to y-axis)
    bool width_spawn = random_bool();

    if (width_spawn) {
        enemy->position.x = random_int(min_x, max_x);
        bool top = random_bool();
        if (top) {
            enemy->position.y = min_y - (enemy->radius);
        } else {
            enemy->position.y = max_y + (enemy->radius);
        }
    } else {
        enemy->position.y = random_int(min_y, max_y);
        bool left = random_bool();
        if (left) {
            enemy->position.x = min_x - (enemy->radius);
        } else {
            enemy->position.x = max_x + (enemy->radius);
        }
    }

    enemy->direction =
        (Vector2){.x = random_float(-1, 1), .y = random_float(-1, 1)};
    enemy->direction = Vector2Normalize(enemy->direction);

    enemy->color = enemy_get_random_color();
}

bool enemy_in_bounds(Enemy *enemy, int min_x, int min_y, int max_x, int max_y) {
    if (enemy->position.x + (enemy->radius * 2) < min_x) return false;
    if (enemy->position.x - (enemy->radius * 2) > max_x) return false;
    if (enemy->position.y + (enemy->radius * 2) < min_y) return false;
    if (enemy->position.y - (enemy->radius * 2) > max_y) return false;
    return true;
}

void enemy_move(Enemy *enemy, Vector2 player_pos, float speed_mod) {
    if (enemy->follow) {
        Vector2 dir_vec = {player_pos.x - enemy->position.x,
                           player_pos.y - enemy->position.y};

        // if at position of player (already game over)
        if (Vector2Length(dir_vec) < 1) { return; }

        enemy->direction = Vector2Normalize(dir_vec);
    }
    enemy->position.x += enemy->direction.x * enemy->speed * speed_mod * GetFrameTime();
    enemy->position.y += enemy->direction.y * enemy->speed * speed_mod * GetFrameTime();
}
