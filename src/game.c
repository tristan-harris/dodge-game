#include "game.h"
#include "blur_shader.h"
#include "enemy.h"
#include "gui.h"
#include "high_scores.h"
#include "level_config.h"
#include "level_track.h"
#include "player.h"
#include "raylib.h"
#include "raymath.h"
#include "util.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// ===== GLOBALS ===============================================================

static Game game;

static BlurShader gaus_shader_ver;
static BlurShader gaus_shader_hor;

static RenderTexture2D target;
static RenderTexture2D second_target;

// ===== PROTOTYPES ============================================================

static void game_btn_level_select(Button *btn);
static void game_btn_restart(Button *btn);
static void game_btn_menu(Button *btn);
static void game_btn_quit(Button *btn);

static void game_play_level_track(char *track_name);
static void game_load_music(void);

// ===== STATES ================================================================

static void game_transition_state(const GameState *new_state, void *data) {
    if (game.state) { game.state->exit_fn(); }
    game.state = new_state;
    if (game.state) { game.state->entry_fn(data); }
}

static void game_state_menu_entry(void *data) {
    (void)data;

    int left_anchor = 566;
    int top_anchor = 560;

    int text_size = 30;
    int button_padding = 10;
    int button_margin = 20;

    int increment = 0;

    Button l1_btn;
    button_init(&l1_btn, "level_1", "Level 1", text_size, left_anchor,
                top_anchor, button_padding, RAYWHITE, SKYBLUE, BLUE, OFFBLACK,
                game_btn_level_select);
    increment += l1_btn.bounds.width + button_margin;

    Button l2_btn;
    button_init(&l2_btn, "level_2", "Level 2", text_size,
                left_anchor + increment, top_anchor, button_padding, RAYWHITE,
                SKYBLUE, BLUE, OFFBLACK, game_btn_level_select);
    increment += l2_btn.bounds.width + button_margin;

    Button l3_btn;
    button_init(&l3_btn, "level_3", "Level 3", text_size,
                left_anchor + increment, top_anchor, button_padding, RAYWHITE,
                SKYBLUE, BLUE, OFFBLACK, game_btn_level_select);
    increment += l3_btn.bounds.width + button_margin;

    Button quit_btn;
    button_init(&quit_btn, "quit", "Quit", text_size, left_anchor + increment,
                top_anchor, button_padding, RAYWHITE, SKYBLUE, BLUE, OFFBLACK,
                game_btn_quit);

    ButtonList_add(&game.buttons, l1_btn);
    ButtonList_add(&game.buttons, l2_btn);
    ButtonList_add(&game.buttons, l3_btn);
    ButtonList_add(&game.buttons, quit_btn);

    if (game.level_track) {
        if (IsMusicStreamPlaying(game.level_track->music)) {
            StopMusicStream(game.level_track->music);
            SeekMusicStream(game.level_track->music, 0);
        }
    }

    ShowCursor();
}

static void game_state_menu_exit(void) {
    ButtonList_clear(&game.buttons);
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

// send LevelConfiguration as data
static void game_state_active_entry(void *data) {
    LevelConfig *lc = (LevelConfig *)data;
    game_setup(lc);

    HideCursor();
}

static void game_state_active_exit(void) {
    if ((int)game.score > game.high_score) {
        high_score_set(game.score, game.level_config->internal_name);
    }
}

static void game_state_over_entry(void *data) {
    (void)data;

    int left_anchor = 604; // starting x position of button row
    int top_anchor = 600;  // y position of button row

    int text_size = 30;
    int button_padding = 10;
    int button_margin = 20;

    int increment = 0;

    Button restart_btn;
    button_init(&restart_btn, "restart", "Restart", text_size, left_anchor,
                top_anchor, button_padding, RAYWHITE, SKYBLUE, BLUE, OFFBLACK,
                game_btn_restart);
    increment += restart_btn.bounds.width + button_margin;

    Button menu_btn;
    button_init(&menu_btn, "menu", "Menu", text_size, left_anchor + increment,
                top_anchor, button_padding, RAYWHITE, SKYBLUE, BLUE, OFFBLACK,
                game_btn_menu);
    increment += menu_btn.bounds.width + button_margin;

    Button quit_btn;
    button_init(&quit_btn, "quit", "Quit", text_size, left_anchor + increment,
                top_anchor, button_padding, RAYWHITE, SKYBLUE, BLUE, OFFBLACK,
                game_btn_quit);

    ButtonList_add(&game.buttons, restart_btn);
    ButtonList_add(&game.buttons, menu_btn);
    ButtonList_add(&game.buttons, quit_btn);

    ShowCursor();
}

static void game_state_over_exit(void) {
    ButtonList_clear(&game.buttons);
    SetMouseCursor(MOUSE_CURSOR_DEFAULT);
}

static void game_state_closed_entry(void *data) {
    (void)data;
}

static void game_state_closed_exit(void) {}

static const GameState menu_state = {.entry_fn = game_state_menu_entry,
                                     .exit_fn = game_state_menu_exit,
                                     .type = STATE_MENU};
static const GameState active_state = {.entry_fn = game_state_active_entry,
                                       .exit_fn = game_state_active_exit,
                                       .type = STATE_ACTIVE};
static const GameState over_state = {.entry_fn = game_state_over_entry,
                                     .exit_fn = game_state_over_exit,
                                     .type = STATE_OVER};
static const GameState closed_state = {.entry_fn = game_state_closed_entry,
                                       .exit_fn = game_state_closed_exit,
                                       .type = STATE_CLOSED};

// ===== INITIALISATION ========================================================

void game_init(int screen_width, int screen_height) {
    game.screen_width = screen_width;
    game.screen_height = screen_height;

    game.level_config = NULL;
    game.level_track = NULL;
    game.state = NULL;

    game.player = (Player){.position.x = 0,
                           .position.y = 0,
                           .radius = PLAYER_RADIUS,
                           .color = PLAYER_COLOR};

    game.enemies = NULL;
    game.debug = false;

    ButtonList_init(&game.buttons);

    blur_shader_load(&gaus_shader_ver, "gaussian_vertical",
                     (Vector2){screen_width / 2.0, screen_height / 2.0});
    blur_shader_load(&gaus_shader_hor, "gaussian_horizontal",
                     (Vector2){screen_width / 2.0, screen_height / 2.0});

    target = LoadRenderTexture(960, 540);
    second_target = LoadRenderTexture(960, 540);

    game_load_music();

    high_scores_init();

    game_transition_state(&menu_state, NULL);
}

void game_setup(LevelConfig *config) {
    game.level_config = config;

    int enemy_count = config->line_enemy_count + config->follow_enemy_count;

    if (game.enemies) {
        game.enemies = realloc(game.enemies, sizeof(Enemy) * enemy_count);
    } else {
        game.enemies = malloc(sizeof(Enemy) * enemy_count);
    }

    int i = 0;
    while (i < config->follow_enemy_count) {
        Enemy *enemy = &game.enemies[i];
        enemy_setup(enemy, config->follow_enemy_min_radius,
                    config->follow_enemy_max_radius,
                    config->follow_enemy_min_speed,
                    config->follow_enemy_max_speed, true);
        enemy_spawn(enemy, 0, 0, game.screen_width, game.screen_height);

        i++;
    }

    while (i < enemy_count) {
        Enemy *enemy = &game.enemies[i];
        enemy_setup(enemy, config->line_enemy_min_radius,
                    config->line_enemy_max_radius, config->line_enemy_min_speed,
                    config->line_enemy_max_speed, false);
        enemy_spawn(enemy, 0, 0, game.screen_width, game.screen_height);

        i++;
    }

    game.score = 0;
    game.high_score = high_score_get(game.level_config->internal_name);

    if (!game.level_track || !IsMusicStreamPlaying(game.level_track->music)) {
        game_play_level_track(config->track_name);
    }

    blur_shader_set_bpm(&gaus_shader_ver, game.level_track->bpm);
    blur_shader_set_bpm(&gaus_shader_hor, game.level_track->bpm);
}

static void game_play_level_track(char *track_name) {
    LevelTrack *lt = level_track_get(track_name);
    if (!lt) {
        TraceLog(LOG_ERROR, "Could not get level track '%s'", track_name);
        game_transition_state(&closed_state, NULL);
        return;
    }

    game.level_track = lt;

    PlayMusicStream(lt->music);
}

// ===== UPDATE ================================================================

static void game_enemies_update(void) {
    int enemy_count = game.level_config->line_enemy_count +
                      game.level_config->follow_enemy_count;

    // make enemies move faster the longer the level goes on for
    // the larger log_base is, the slower the speed increases past the
    // threshold is
    float score_threshold = 30;
    float log_base = 2;
    float speed_mod = log(game.score / score_threshold) / log(log_base);
    speed_mod = Clamp(speed_mod, 1, 2);

    for (int i = 0; i < enemy_count; i++) {
        Enemy *enemy = &game.enemies[i];

        enemy_move(enemy, game.player.position, speed_mod);
        if (!enemy_in_bounds(enemy, 0, 0, game.screen_width,
                             game.screen_height)) {
            enemy_spawn(enemy, 0, 0, game.screen_width, game.screen_height);
        }
    }
}

// returns index of hovered button, -1 if none
static int game_buttons_update(void) {
    int mx = GetMouseX();
    int my = GetMouseY();

    int hovered_idx = -1;

    for (int i = 0; i < game.buttons.count; i++) {
        Button *btn = &game.buttons.data[i];
        if (button_is_hovered(btn, mx, my)) {
            btn->hovered = true;
            hovered_idx = i;
        } else {
            btn->hovered = false;
        }
    }

    hovered_idx != -1 ? SetMouseCursor(MOUSE_CURSOR_POINTING_HAND)
                      : SetMouseCursor(MOUSE_CURSOR_DEFAULT);

    return hovered_idx;
}

static void game_intro_update(void) {
    int hovered_button_idx = game_buttons_update();

    // if clicked on button
    if (hovered_button_idx != -1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Button *btn = &game.buttons.data[hovered_button_idx];
        btn->function(btn);
    }
}

static void game_active_update(void) {
    game.player.position.x = GetMouseX();
    game.player.position.y = GetMouseY();

    game_enemies_update();

    // collision detection
    int enemy_count = game.level_config->line_enemy_count +
                      game.level_config->follow_enemy_count;
    for (int i = 0; i < enemy_count; i++) {
        Enemy *enemy = &game.enemies[i];
        if (player_enemy_collision(&game.player, enemy)) {
            game_transition_state(&over_state, NULL);
        }
    }

    UpdateMusicStream(game.level_track->music);
    game.score += GetFrameTime();
}

static void game_over_update(void) {
    game_enemies_update();
    int hovered_button_idx = game_buttons_update();

    if (hovered_button_idx != -1 && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Button *btn = &game.buttons.data[hovered_button_idx];
        btn->function(btn);
    }
    UpdateMusicStream(game.level_track->music);
}

void game_update(void) {
#ifdef DEBUG
    if (IsKeyPressed(KEY_D)) { game.debug = !game.debug; }
#endif

    if (IsKeyPressed(KEY_Q)) { game_transition_state(&closed_state, NULL); }

    switch (game.state->type) {
    case STATE_MENU:
        game_intro_update();
        break;

    case STATE_ACTIVE:
        game_active_update();
        break;

    case STATE_OVER:
        game_over_update();
        break;

    case STATE_CLOSED:
        break;
    }
}

// ===== DRAW ==================================================================

static void game_draw_enemies(void) {
    int enemy_count = game.level_config->line_enemy_count +
                      game.level_config->follow_enemy_count;
    for (int i = 0; i < enemy_count; i++) {
        Enemy *enemy = &game.enemies[i];
        DrawCircle(enemy->position.x, enemy->position.y, enemy->radius,
                   enemy->color);
    }
}

static void game_draw_enemies_scaled(float scale_mod, float radius_mod) {
    int enemy_count = game.level_config->line_enemy_count +
                      game.level_config->follow_enemy_count;
    for (int i = 0; i < enemy_count; i++) {
        Enemy *enemy = &game.enemies[i];

        // HACK: smaller enemies need to be drawn larger for the blur effect to
        // be visible
        float drawn_radius = enemy->radius * radius_mod;
        drawn_radius = MAX(drawn_radius, 4);

        DrawCircle(enemy->position.x * scale_mod, enemy->position.y * scale_mod,
                   drawn_radius, enemy->color);
    }
}

static void game_draw_player_scaled(float scale_mod, float radius_mod) {
    DrawCircle(game.player.position.x * scale_mod,
               game.player.position.y * scale_mod,
               game.player.radius * radius_mod, game.player.color);
}

static void game_draw_player(void) {
    DrawCircle(game.player.position.x, game.player.position.y,
               game.player.radius, game.player.color);
}

static void game_draw_game_state(bool draw_player) {
    blur_shader_update_time(&gaus_shader_ver);
    blur_shader_update_time(&gaus_shader_hor);

    // draw circles on scaled down render texture
    BeginTextureMode(target);
    ClearBackground(OFFBLACK);
    game_draw_enemies_scaled(0.25, 0.25);
    if (draw_player) { game_draw_player_scaled(0.25, 0.5); }
    EndTextureMode();

    // vertical blur
    BeginTextureMode(second_target);
    BeginShaderMode(gaus_shader_ver.shader);
    DrawTextureRec(
        target.texture,
        (Rectangle){0, 0, target.texture.width, target.texture.height},
        (Vector2){0, 0}, WHITE);
    EndShaderMode();
    EndTextureMode();

    // draw render texture scaled up (with horizontal blur)
    BeginShaderMode(gaus_shader_hor.shader);
    DrawTextureEx(second_target.texture, (Vector2){0, 0}, 0, 4, WHITE);
    EndShaderMode();

    // draw clear shapes on top of glow
    game_draw_enemies();
    if (draw_player) { game_draw_player(); }
}

static void game_draw_buttons(void) {
    for (int i = 0; i < game.buttons.count; i++) {
        button_draw(&game.buttons.data[i]);
    }
}

static void game_draw_text_centered(char *text, int x_pos, int y_pos, int size,
                                    Color color) {
    DrawText(text, x_pos - (MeasureText(text, size) / 2), y_pos - (size / 2),
             size, color);
}

static void game_intro_draw(void) {
    ClearBackground(OFFBLACK);
    game_draw_text_centered("DODGE GAME", game.screen_width / 2,
                            game.screen_height / 2 - 60, 120, RAYWHITE);
    DrawText("Game created by Tristan Harris. Music created by Eric Matyas "
             "(www.soundimage.org).",
             10, game.screen_height - 30, 20, RAYWHITE);
    game_draw_buttons();
}

static void game_active_draw(void) {
    game_draw_game_state(true);

    // draw score
    char score_text[64];
    snprintf(score_text, sizeof(score_text), "Score: %d", (int)game.score);
    DrawText(score_text, 15, 15, 30, RAYWHITE);

    // draw high score
    if (game.high_score != 0) {
        char high_score_text[64];
        snprintf(high_score_text, sizeof(high_score_text), "High Score: %d",
                 game.high_score);
        DrawText(high_score_text, 15, 50, 30, RAYWHITE);
    }
}

static void game_over_draw(void) {
    game_draw_game_state(false);

    game_draw_text_centered("GAME OVER", game.screen_width / 2,
                            game.screen_height / 2 - 60, 120, RAYWHITE);

    char score_text[128];
    snprintf(score_text, sizeof(score_text), "Score: %d   High Score: %d",
             (int)game.score, game.high_score);
    DrawText(score_text, 594, 540, 30, RAYWHITE);

    game_draw_buttons();
}

void game_draw(void) {
    BeginDrawing();

    switch (game.state->type) {
    case STATE_MENU:
        game_intro_draw();
        break;

    case STATE_ACTIVE:
        game_active_draw();
        break;

    case STATE_OVER:
        game_over_draw();
        break;

    case STATE_CLOSED:
        break;
    }

    if (game.debug) { DrawFPS(game.screen_width - 100, 10); }

    EndDrawing();
}

// ===== BUTTONS ===============================================================

static void game_btn_level_select(Button *btn) {
    LevelConfig *lc = level_configuration_get(btn->internal_text);

    if (!lc) {
        TraceLog(LOG_ERROR, "Could not find level config with name '%s'",
                 btn->internal_text);
        game_transition_state(&closed_state, NULL);
    } else {
        game_transition_state(&active_state, lc);
    }
}

static void game_btn_restart(Button *btn) {
    (void)btn;
    game_transition_state(&active_state, game.level_config);
}

static void game_btn_menu(Button *btn) {
    (void)btn;
    game_transition_state(&menu_state, NULL);
}

static void game_btn_quit(Button *btn) {
    (void)btn;
    game_transition_state(&closed_state, NULL);
}

// ===== OTHER =================================================================

bool game_is_closed(void) {
    return game.state->type == STATE_CLOSED;
}

static void game_load_music(void) {
    for (int i = 0; level_tracks[i]; i++) {
        LevelTrack *lt = level_tracks[i];
        char file_path[100];
        snprintf(file_path, sizeof(file_path), "resources/music/%s.mp3",
                 lt->name);
        lt->music = LoadMusicStream(file_path);
    }
}

void game_unload(void) {
    if (game.enemies) { free(game.enemies); }

    blur_shader_unload(&gaus_shader_ver);
    blur_shader_unload(&gaus_shader_hor);

    UnloadRenderTexture(target);

    for (int i = 0; level_tracks[i]; i++) {
        LevelTrack *lt = level_tracks[i];
        UnloadMusicStream(lt->music);
    }

    high_scores_free();
}
