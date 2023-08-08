#include <math.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])

typedef struct {
    float left;
    float right;
} Frame;

Frame global_frames[4080];
size_t global_frames_count = 0;

void callback(void *bufferData, unsigned int frames) {
    size_t capacity = ARRAY_LEN(global_frames);

    if (frames <= capacity - global_frames_count) {
        memcpy(global_frames, bufferData, frames * sizeof(Frame));
        global_frames_count += frames;
    } else if (frames <= capacity) {
        memmove(global_frames, global_frames + frames,
                sizeof(Frame) * (capacity - frames));
        memcpy(global_frames + (capacity - frames), bufferData,
               sizeof(Frame) * frames);
    } else {
        memcpy(global_frames, bufferData, sizeof(Frame) * capacity);
        global_frames_count = capacity;
    }

    if (frames > ARRAY_LEN(global_frames)) {
        frames = ARRAY_LEN(global_frames);
    }

    memcpy(global_frames, bufferData, frames * sizeof(Frame));
    global_frames_count = frames;
}

int getFormattedTime(char **out, int time_in_milli) {
    int minutes = time_in_milli / 60000;
    int seconds = (time_in_milli % 60000) / 1000;
    int miliseconds = time_in_milli % 1000;
    return asprintf(out, "Time played: %d:%d.%d", minutes, seconds,
                    miliseconds);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("1 argument is required\n");
        return -1;
    }

    InitWindow(1280, 900, "Music VW");
    SetTargetFPS(30.0);
    InitAudioDevice();

    Music music = LoadMusicStream(argv[1]);
    PlayMusicStream(music);
    SetMusicVolume(music, 0.5);

    AttachAudioStreamProcessor(music.stream, callback);

    while (!WindowShouldClose()) {
        UpdateMusicStream(music);

        char *text;
        getFormattedTime(&text, GetMusicTimePlayed(music) * 1000);

        if (IsKeyPressed(KEY_SPACE)) {
            if (IsMusicStreamPlaying(music))
                PauseMusicStream(music);
            else
                PlayMusicStream(music);
        }

        BeginDrawing();
        ClearBackground(CLITERAL(Color){0x18, 0x18, 0x18, 0xFF});
        int w = GetScreenWidth();
        int h = GetScreenHeight();

        float cell_width = (float)w / global_frames_count;
        for (size_t i = 0; i < global_frames_count; ++i) {
            float sample_left = sinf(global_frames[i].left);
            float sample_right = cosf(global_frames[i].right);
            DrawRectangle(cell_width * i, h / 2 - h / 2 * sample_left,
                          cell_width, sample_left * (h / 2), RED);
            DrawRectangle(cell_width * i, h / 2, cell_width,
                          sample_right * (h / 2), BLUE);
        }
        DrawText(text, 0, 0, 32, WHITE);

        EndDrawing();
    }
    UnloadMusicStream(music);
    return 0;
}
