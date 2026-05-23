#ifndef INTRO_H
#define INTRO_H

#include "../gen/assets/backgrounds.h"
#include "../gen/assets/sfx.h"
#include "../gen/assets/music.h"
#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"
#include "input.h"

SpriteSlot background;
SpriteSlot loseback;

bool isOnIntro = true;
bool isOnLose = false;
char seed = 0;

void initializeIntroScreen() {
  background = allocate_sprite(&ASSET__backgrounds__intro_bmp_load_list);
  loseback = allocate_sprite(&ASSET__backgrounds__lose_bmp_load_list);
  play_song(&ASSET__music__paparazzi_mid, REPEAT_LOOP);
}

void checkIntroInput() {
  if (player1_new_buttons & INPUT_MASK_START) {
    isOnIntro = false;
    isOnLose = false;
    stop_music();
  }
}

void beginDraw() {
  queue_clear_border(0);
  queue_clear_screen(0);
}

void endDraw() {
  await_draw_queue();
  await_vsync(1);
  flip_pages();
  tick_music();
  update_inputs();
}

char renderLoseScreen() {
  isOnLose = true;
  initializeIntroScreen();
  while (isOnLose) {
    beginDraw();
    queue_draw_sprite(0, 0, 127, 127, 0, 0, loseback);
    ++seed;
    endDraw();
    checkIntroInput();
  }
  return seed;
}

char renderIntroScreen() {
  isOnIntro = true;
  initializeIntroScreen();
  while (isOnIntro) {
    beginDraw();
    queue_draw_sprite(0, 0, 127, 127, 0, 0, background);
    ++seed;
    endDraw();
    checkIntroInput();
  }
  return seed;
}


#endif