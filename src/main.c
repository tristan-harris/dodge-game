#include "game.h"
#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

int main(void) {
    srand((unsigned int)time(NULL)); // random seed

    SetConfigFlags(FLAG_MSAA_4X_HINT); // anti-aliasing

    InitWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Dodge Game");
    InitAudioDevice();

    game_init(WINDOW_WIDTH, WINDOW_HEIGHT);

    SetTargetFPS(100);

    while (!WindowShouldClose())
    {
        game_update();
        game_draw();
        if (game_is_closed()) { break; }
    }

    game_unload();
    CloseAudioDevice();
    CloseWindow();

    return EXIT_SUCCESS;
}
