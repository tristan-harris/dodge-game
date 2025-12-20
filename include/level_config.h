#pragma once

typedef struct {
    char *internal_name; // used for highscore file
    char *display_name;  // shown in UI
    char *track_name;    // level music

    // enemies that just move across the screen
    int line_enemy_count;
    float line_enemy_min_radius;
    float line_enemy_max_radius;
    float line_enemy_min_speed;
    float line_enemy_max_speed;

    // enemies that follow player
    int follow_enemy_count;
    float follow_enemy_min_radius;
    float follow_enemy_max_radius;
    float follow_enemy_min_speed;
    float follow_enemy_max_speed;
} LevelConfig;

extern LevelConfig *level_configs[];

LevelConfig *level_configuration_get(char *name);
