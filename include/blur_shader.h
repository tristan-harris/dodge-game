#pragma once

#include "raylib.h"

// to be used with the two gaussian blur shaders

#define SHADER_DIR_PATH "resources/shaders/"

typedef struct {
    Shader shader;
    Vector2 frame_buf_size;
    int frame_buf_size_loc;
    float bpm;
    int bpm_loc;
    int time_loc;
} BlurShader;

void blur_shader_load(BlurShader *bs, char *shader_name,
                      Vector2 frame_buf_size);
void blur_shader_set_bpm(BlurShader *bs, float bpm);
void blur_shader_update_time(BlurShader *bs);
void blur_shader_unload(BlurShader *bs);
