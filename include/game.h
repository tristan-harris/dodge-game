#pragma once

#include "enemy.h"
#include "gui.h"
#include "level_config.h"
#include "level_track.h"
#include "list.h"
#include "player.h"

#define OFFBLACK CLITERAL(Color){7, 7, 7, 255}

DEFINE_ARRAY_TYPE(ButtonList, Button);

typedef enum {
    STATE_MENU,   // level selection
    STATE_ACTIVE, // active gameplay
    STATE_OVER,   // game over screen
    STATE_CLOSED  // quitting game
} GameStateEnum;

typedef struct {
    GameStateEnum type;
    void (*entry_fn)(void *data);
    void (*exit_fn)(void);
} GameState;

typedef struct {
    int screen_width;
    int screen_height;
    LevelConfig *level_config;
    LevelTrack *level_track;
    const GameState *state;
    Player player;
    Enemy *enemies;
    float score;    // time passed since level start (seconds)
    int high_score; // like score (seconds)
    ButtonList buttons;
    bool debug; // whether to display debug info
} Game;

Game game_create(void);

// should only be called once at beginning of game
void game_init(int screen_width, int screen_height);

// called every time a new level begins
void game_setup(LevelConfig *config);

void game_update(void);
void game_draw(void);
bool game_is_closed(void);
void game_unload(void);
