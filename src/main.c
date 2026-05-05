#include "golf.h"
#include "gt/audio/audio_coprocessor.h"
#include "gt/audio/music.h"
#include "gt/feature/random/random.h"
#include "gt/gametank.h"
#include "gt/gfx/draw_queue.h"

char box_x = 30, box_y = 20;
char dx = 1, dy = 1;

void main() {

  // TODO: make the intro screen change this value
  char seed = 2;
  rnd_seed = seed;

  init_audio_coprocessor();
  init_music();

  while (1) {
    renderGolfBoard();
  }
}