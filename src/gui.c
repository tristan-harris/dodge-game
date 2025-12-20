#include "gui.h"
#include "raylib.h"

void button_init(Button *btn, char *internal_text, char *display_text,
                 int text_size, int x_pos, int y_pos, int padding,
                 Color normal_color, Color highlight_color, Color select_color,
                 Color background_color, void (*function)(Button *data)) {
    btn->internal_text = internal_text;
    btn->display_text = display_text;
    btn->text_size = text_size;
    btn->x_pos = x_pos;
    btn->y_pos = y_pos;

    btn->bounds = (Rectangle){.x = x_pos - padding,
                              .y = y_pos - padding,
                              .width = MeasureText(display_text, text_size) +
                                       (padding * 2),
                              .height = text_size + (padding * 2)};

    btn->normal_color = normal_color;
    btn->highlight_color = highlight_color;
    btn->select_color = select_color;
    btn->background_color = background_color;
    btn->function = function;

    btn->hovered = false;
    btn->selected = false;
}

void button_draw(Button *btn) {
    Color *color = btn->hovered ? &btn->highlight_color : &btn->normal_color;
    DrawRectangleRec(btn->bounds, btn->background_color);
    DrawText(btn->display_text, btn->x_pos, btn->y_pos, btn->text_size, *color);
    DrawRectangleLinesEx(btn->bounds, BUTTON_BORDER_WIDTH, *color);
}

bool button_is_hovered(Button *btn, int mouse_x, int mouse_y) {
    if (mouse_x < btn->bounds.x) return false;
    if (mouse_x > btn->bounds.x + btn->bounds.width) return false;
    if (mouse_y < btn->bounds.y) return false;
    if (mouse_y > btn->bounds.y + btn->bounds.height) return false;
    return true;
}
