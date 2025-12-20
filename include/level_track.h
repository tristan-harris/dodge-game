#pragma once

#include "raylib.h"

typedef struct {
    char *name;
    float bpm;
    Music music;
} LevelTrack;

extern LevelTrack *level_tracks[];

LevelTrack *level_track_get(char *name);
