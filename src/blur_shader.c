#include "blur_shader.h"
#include "raylib.h"
#include <stdio.h>

void blur_shader_load(BlurShader *bs, char *shader_name,
                      Vector2 frame_buf_size) {
    char shader_path[128];
    snprintf(shader_path, sizeof(shader_path), "%s%s.glsl", SHADER_DIR_PATH,
             shader_name);
    bs->shader = LoadShader(NULL, shader_path);

    bs->frame_buf_size = frame_buf_size;
    bs->frame_buf_size_loc = GetShaderLocation(bs->shader, "frameBufferSize");
    SetShaderValue(bs->shader, bs->frame_buf_size_loc, &bs->frame_buf_size,
                   SHADER_UNIFORM_VEC2);

    bs->bpm = 0;
    bs->bpm_loc = GetShaderLocation(bs->shader, "bpm");
    bs->time_loc = GetShaderLocation(bs->shader, "time");
}

void blur_shader_set_bpm(BlurShader *bs, float bpm) {
    bs->bpm = bpm;
    SetShaderValue(bs->shader, bs->bpm_loc, &bs->bpm, SHADER_UNIFORM_FLOAT);
}

void blur_shader_update_time(BlurShader *bs) {
    float time = (float)GetTime();
    SetShaderValue(bs->shader, bs->time_loc, &time, SHADER_UNIFORM_FLOAT);
}

void blur_shader_unload(BlurShader *bs) {
    UnloadShader(bs->shader);
}
