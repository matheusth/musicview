#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(xs) sizeof(xs) / sizeof(xs[0])

int32_t global_frames[4080];
size_t global_frames_count = 0;

void callback(void *bufferData, unsigned int frames) {

  if (frames > ARRAY_LEN(global_frames)) {
    frames = ARRAY_LEN(global_frames);
  }

  memcpy(global_frames, bufferData, frames * sizeof(int32_t));
  global_frames_count = frames;
}

int getFormattedTime(char **out, int time_in_milli) {
  int minutes = time_in_milli / 60000;
  int seconds = (time_in_milli % 60000) / 1000;
  int miliseconds = time_in_milli % 1000;
  return asprintf(out, "Time played: %d:%d.%d", minutes, seconds, miliseconds);
}

int main(int argc, char *argv[]) {
    if(argc < 2){
        printf("1 argument is required\n");
        return -1;
    }

  InitWindow(800, 600, "Music VW");
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
    
    float cell_width = (float)w/global_frames_count;
    for(size_t i=0; i < global_frames_count; ++i){
        int16_t sample = *(int16_t*) &global_frames[i];        
        if(sample > 0 ){
            float normalized_sample = (float)sample / INT16_MAX;
            DrawRectangle(cell_width*i, h/2 - h/2*normalized_sample,cell_width, normalized_sample * (h/2), RED);
        }else{
            float normalized_sample = (float)sample / INT16_MIN;
            DrawRectangle(cell_width*i, h/2,cell_width, normalized_sample * (h/2), RED);
        }
    }
    DrawText(text, 0, 0, 32, WHITE);

    EndDrawing();
  }
  UnloadMusicStream(music);
  return 0;
}
