#include "level_config.h"
#include <string.h>

static LevelConfig l1 = {.internal_name = "level_1",
                         .display_name = "Level 1",
                         .track_name = "binary_dreamscape",
                         .line_enemy_count = 500,
                         .line_enemy_min_radius = 5,
                         .line_enemy_max_radius = 5,
                         .line_enemy_min_speed = 90,
                         .line_enemy_max_speed = 95};

static LevelConfig l2 = {.internal_name = "level_2",
                         .display_name = "Level 2",
                         .track_name = "techno_tronic_1",
                         .line_enemy_count = 50,
                         .line_enemy_min_radius = 7,
                         .line_enemy_max_radius = 7,
                         .line_enemy_min_speed = 300,
                         .line_enemy_max_speed = 500,
                         .follow_enemy_count = 1,
                         .follow_enemy_min_speed = 200,
                         .follow_enemy_max_speed = 200,
                         .follow_enemy_min_radius = 100,
                         .follow_enemy_max_radius = 100};

static LevelConfig l3 = {.internal_name = "level_3",
                         .display_name = "Level 3",
                         .track_name = "techno_tronic_2",
                         .line_enemy_count = 50,
                         .line_enemy_min_radius = 150,
                         .line_enemy_max_radius = 200,
                         .line_enemy_min_speed = 100,
                         .line_enemy_max_speed = 130};

LevelConfig *level_configs[] = {&l1, &l2, &l3, NULL};

LevelConfig *level_configuration_get(char *name) {
    for (int i = 0; level_configs[i]; i++) {
        LevelConfig *lc = level_configs[i];
        if (strcmp(name, lc->internal_name) == 0) { return lc; }
    }
    return NULL;
}
