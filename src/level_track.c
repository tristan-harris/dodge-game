#include "level_track.h"
#include <string.h>

// bpm calculated with `aubio tempo [FILE]`
static LevelTrack t1 = {.name = "binary_dreamscape", .bpm = 115.06};
static LevelTrack t2 = {.name = "future_city_life", .bpm = 125.52};
static LevelTrack t3 = {.name = "future_industry_1", .bpm = 95.08};
static LevelTrack t4 = {.name = "techno_tronic_1", .bpm = 128.55};
static LevelTrack t5 = {.name = "techno_tronic_2", .bpm = 126.38};

LevelTrack *level_tracks[] = {&t1, &t2, &t3, &t4, &t5, NULL};

LevelTrack *level_track_get(char *name) {
    for (int i = 0; level_tracks[i]; i++) {
        if (strcmp(name, level_tracks[i]->name) == 0) {
            return level_tracks[i];
        }
    }
    return NULL;
}
