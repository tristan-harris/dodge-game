#pragma once

#include <stdbool.h>

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof((arr)[0]))
#define MAX(a,b) (((a)>(b))?(a):(b))

bool random_bool(void);

// inclusive both ends
int random_int(int min, int max);

// inclusive both ends
float random_float(float min, float max);
