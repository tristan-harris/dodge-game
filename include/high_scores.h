#pragma once

#define HIGH_SCORES_FILE_NAME "highscores.dat"

typedef struct {
    char level_name[32]; // internal name
    int score;
} HighScore;

void high_scores_init(void);
void high_scores_free(void);
int high_score_get(char *level_name);
void high_score_set(int score, char *level_name);
