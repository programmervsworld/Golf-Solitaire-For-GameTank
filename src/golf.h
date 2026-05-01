#ifndef GOLF_H
#define GOLF_H

#include "gametank.h"
#include "gt/feature/random/random.h"
#include "gt/gfx/draw_queue.h"
#include "input.h"
#include "../gen/assets/cardframes.h"
#include "../gen/assets/backgrounds.h"

#define TABLEAU_COLS 7
#define TABLEAU_ROWS 5
#define TABLEAU_Y_SHIFT 12
#define TABLEAU_X_SHIFT 1

// The is the deck that we deal from and where the next discard will come from
unsigned char deck[52];

char board[TABLEAU_ROWS][TABLEAU_COLS];

char animationx[52];
char animationy[52];
char directionx[52];
char directiony[52];
char remainingCardCount;

SpriteSlot background;
SpriteSlot cardSprites;

void loadPyramidDeck(unsigned char *deck) {
  char i = 0;
  for (i = 1; i < 52; i++) {
    deck[i - 1] = i;
    animationx[i - 1] = 64;
    animationy[i - 1] = 64;
    directionx[i - 1] = rnd_range(-8, 8);
    directiony[i - 1] = rnd_range(-8, 8);
  }
}

void loadPyramidBoard() {
  char r, c, num, counter;
  num = 0;
  for (r = 0; r < TABLEAU_ROWS; r++) {
    for (c = 0; c < TABLEAU_COLS; c++) {
      board[r][c] = deck[num];
      num++;
    }
  }
  remainingCardCount = num;
}

void renderGolfBoardNormal(){
    char rows, cols, cardx, cardy;
    //queue_draw_sprite(0, 0, 127, 127, 0, 0, background);
    for(rows = 0; rows < TABLEAU_ROWS; rows++){
        cardy = 12 * (rows+1);
        for(cols = 0; cols < TABLEAU_COLS; cols++){
            cardx = 16 * (cols+1);
            queue_draw_sprite_frame(cardSprites, cardx - TABLEAU_X_SHIFT, cardy + TABLEAU_Y_SHIFT, board[rows][cols], false);
        }
    }    
}

void initalizeGolfScene() {
    background = allocate_sprite(&ASSET__backgrounds__background_bmp_load_list);
    cardSprites = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
    set_sprite_frametable(cardSprites, &ASSET__cardframes__frame_deck_json);
    
    loadPyramidDeck(deck);
    loadPyramidBoard();
}

void renderGolfBoard() {
    bool isRunning = true;
    initalizeGolfScene();
    
    while(isRunning){
        queue_clear_screen(187);
        queue_clear_border(186);
        
        renderGolfBoardNormal();
        
        
        await_draw_queue();
        await_vsync(1);
        flip_pages();
        tick_music();
    }
}


#endif