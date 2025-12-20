#include "high_scores.h"
#include "list.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>

DEFINE_ARRAY_TYPE(HighScoreList, HighScore)

static void high_scores_read_file(char *file_path);

static HighScoreList hs_list;
static char file_path[256];

void high_scores_init(void) {
    HighScoreList_init(&hs_list);

    // application directory ends with '/'
    snprintf(file_path, sizeof(file_path), "%s%s", GetApplicationDirectory(),
             HIGH_SCORES_FILE_NAME);

    if (FileExists(file_path)) {
        high_scores_read_file(file_path);
    }
}

static void high_scores_read_file(char *file_path) {
    char *file_txt = LoadFileText(file_path);

    if (!file_txt) {
        TraceLog(LOG_ERROR, "Could not read high scores file '%s'", file_path);
        return;
    }

    int file_txt_size = strlen(file_txt) + 1; // +1 for '\0'
    char *scores_txt = malloc(file_txt_size);
    memcpy(scores_txt, file_txt, file_txt_size);

    char level_name[32];
    int score;

    for (char *line = strtok(scores_txt, "\n"); line != NULL;
         line = strtok(NULL, "\n")) {

        int n = sscanf(line, "%s = %d", level_name, &score);

        if (n != 2) {
            TraceLog(LOG_ERROR, "Could not parse highscore string '%s'", line);
            continue;
        }

        HighScore hs = {.score = score};
        strcpy(hs.level_name, level_name);
        HighScoreList_add(&hs_list, hs);
    }

    UnloadFileText(file_txt);
    free(scores_txt);
}

void high_scores_free(void) {
    HighScoreList_free(&hs_list);
}

int high_score_get(char *level_internal_name) {
    for (int i = 0; i < hs_list.count; i++) {
        HighScore *hs = &hs_list.data[i];
        if (strcmp(level_internal_name, hs->level_name) == 0) {
            return hs->score;
        }
    }
    return 0;
}

void high_score_set(int score, char *level_internal_name) {
    bool exists = false;

    // if entry already exists update it
    for (int i = 0; i < hs_list.count; i++) {
        HighScore *hs = &hs_list.data[i];
        if (strcmp(level_internal_name, hs->level_name) == 0) {
            hs->score = score;
            exists = true;
            break;
        }
    }

    // else add new one
    if (!exists) {
        HighScore hs = {.score = score};
        strcpy(hs.level_name, level_internal_name);
        HighScoreList_add(&hs_list, hs);
    }

    // write to file
    char hs_buf[512];
    int hs_buf_idx = 0;

    for (int i = 0; i < hs_list.count; i++) {
        HighScore *hs = &hs_list.data[i];
        hs_buf_idx += snprintf(&hs_buf[hs_buf_idx], sizeof(hs_buf) - hs_buf_idx,
                               "%s = %d\n", hs->level_name, hs->score);
    }
    SaveFileText(file_path, hs_buf);
}
