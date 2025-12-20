#include "util.h"
#include <stdlib.h>

bool random_bool(void) {
    return rand() & 1;
}

int random_int(int min, int max) {
    return min + rand() / (RAND_MAX / (max - min + 1) + 1);
}

float random_float(float min, float max) {
    return ((max - min) * ((float)rand() / RAND_MAX)) + min;
}
