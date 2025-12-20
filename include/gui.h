#pragma once

#include "raylib.h"

#define BUTTON_BORDER_WIDTH 2

typedef struct button {
    char *internal_text;
    char *display_text;
    int text_size;
    int x_pos;
    int y_pos;
    Rectangle bounds; // cursor 'hover space' around text
    Color normal_color;
    Color highlight_color;
    Color select_color; // when clicked
    Color background_color;
    bool hovered;
    bool selected;
    void (*function)(struct button *data);
} Button;

void button_init(Button *btn, char *internal_text, char *display_text,
                 int text_size, int x_pos, int y_pos, int padding,
                 Color normal_color, Color highlight_color, Color select_color,
                 Color background_color, void (*function)(Button *data));
void button_draw(Button *btn);
bool button_is_hovered(Button *btn, int mouse_x, int mouse_y);
