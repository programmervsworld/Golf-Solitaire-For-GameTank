#ifndef GOLF_H
#define GOLF_H

#include "../gen/assets/backgrounds.h"
#include "../gen/assets/cardframes.h"
#include "../gen/assets/sfx.h"
#include "gametank.h"
#include "gt/audio/music.h"
#include "gt/feature/random/random.h"
#include "gt/feature/text/text.h"
#include "gt/gfx/draw_queue.h"
#include "gt/input.h"
#include "input.h"

#define TABLEAU_COLS 7
#define TABLEAU_ROWS 5
#define TABLEAU_Y_SHIFT 12
#define TABLEAU_X_SHIFT 1
#define DISCARD_PILE_X 16
#define DISCARD_PILE_Y 104
#define FLIPPED_PILE_X 32
#define FLIPPED_PILE_Y 104
#define DISCARD_BEGINS 35
#define SHUFFLE_SEED 9

typedef struct {
  char row;
  char card;
} CardPosition;

// The is the deck that we deal from and where the next discard will come from
unsigned char deck[52];

char board[TABLEAU_ROWS][TABLEAU_COLS];

char animationx[52];
char animationy[52];
char directionx[52];
char directiony[52];
char remainingCardCount;
char cursorRow = TABLEAU_ROWS - 1;
char cursorCol = 0;
char discardPtr = 0; // This points to where in the deck the next flip is
char discard =
    0; // This points to what is current showing at the top of the discare pile
char discardValue = 0;
unsigned char remainingCards = 35;

SpriteSlot background;
SpriteSlot cardSprites;
SpriteSlot winBack;
CardPosition eligibileCards[7];
SpriteSlot font;

bool isRunning = true;

/*
% 13 may compile to relatively expensive division code on an 8-bit target. Since
this only maps 1-52 to 1-13, a small branch version may be cheaper:
*/
unsigned char getCardValueFromNo(unsigned char cardNo) {
  while (cardNo > 13)
    cardNo -= 13;
  return cardNo;
}

bool hasWon() { return remainingCards == 0; }
bool hasLost() { return discardPtr == 52; }

void loadGolfDeck(unsigned char *deck) {
  char i = 0;
  for (i = 1; i <= 52; i++) {
    deck[i - 1] = i;
    animationx[i - 1] = 64;
    animationy[i - 1] = 64;
    directionx[i - 1] = rnd_range(-8, 8);
    directiony[i - 1] = rnd_range(-8, 8);
  }
}

void loadGolfBoard() {
  char r, c, num, counter;
  num = 0;
  for (r = 0; r < TABLEAU_ROWS; r++) {
    for (c = 0; c < TABLEAU_COLS; c++) {
      board[r][c] = deck[num];
      num++;
    }
  }
  remainingCardCount = num;
  discardPtr = DISCARD_BEGINS;
  discard = deck[discardPtr];
  discardValue = getCardValueFromNo(discard);
}

void shuffleGolfDeck(unsigned char *cards, unsigned char times) {
  char valueToSwap;
  char indexToSwap;
  char i, t;

  for (t = 0; t < times; t++) {
    play_sound_effect(ASSET__sfx__flip_sfx_ID, 1);
    for (i = 0; i < 51; i++) {
      indexToSwap = rnd_range(1, 50);
      valueToSwap = cards[indexToSwap];
      cards[indexToSwap] = cards[1];
      cards[1] = valueToSwap;
    }
  }
}

CardPosition findNextPositionToTheRight() {
  char row, col;
  CardPosition pos;
  pos.card = cursorCol;
  pos.row = cursorRow;

  for (col = cursorCol + 1; col < TABLEAU_COLS; col++) {
    row = TABLEAU_ROWS;
    while (row > 0) {
      row--;
      if (board[row][col] > 0) {
        pos.card = col;
        pos.row = row;
        return pos;
      }
    }
  }
  return pos;
}

CardPosition findNextPositionToTheLeft() {
  char row, col;
  CardPosition pos;
  pos.card = cursorCol;
  pos.row = cursorRow;

  col = cursorCol;
  while (col > 0) {
    col--;
    row = TABLEAU_ROWS;
    while (row > 0) {
      row--;
      if (board[row][col] > 0) {
        pos.card = col;
        pos.row = row;
        return pos;
      }
    }
  }
  return pos;
}

bool canMove(unsigned char discard, unsigned char cardValue) {
  return discardValue + 1 == cardValue || discardValue == cardValue + 1;
}

void checkInput() {
  CardPosition newPos;
  if (player1_new_buttons & INPUT_MASK_RIGHT) {
    if (cursorCol < TABLEAU_COLS - 1) {
      newPos = findNextPositionToTheRight();
      cursorCol = newPos.card;
      cursorRow = newPos.row;
    }
  } else if (player1_new_buttons & INPUT_MASK_LEFT) {
    if (cursorCol > 0) {
      newPos = findNextPositionToTheLeft();
      cursorCol = newPos.card;
      cursorRow = newPos.row;
    }
  } else if (player1_new_buttons & INPUT_MASK_A) {
    char selection = board[cursorRow][cursorCol];
    char selectionValue = getCardValueFromNo(selection);
    if (canMove(discardValue, selectionValue)) {
      play_sound_effect(ASSET__sfx__dog_sfx_ID,1);
      discard = selection;
      discardValue = selectionValue;
      board[cursorRow][cursorCol] = 0;
      remainingCards--;
      if (cursorRow > 0) {
        cursorRow--;
      }
    }
  } else if (player1_new_buttons & INPUT_MASK_C) {
    if (!hasLost()) {
      play_sound_effect(ASSET__sfx__click_sfx_ID, 1);
      discardPtr++;
      discard = deck[discardPtr];
      discardValue = getCardValueFromNo(discard);
    }
  } else if (player1_new_buttons & INPUT_MASK_B) {
      //remainingCards = 0;
  }
}

void renderGolfBoardNormal() {
  char rows, cols, cardx, cardy;
  // queue_draw_sprite(0, 0, 127, 127, 0, 0, background);
  for (rows = 0; rows < TABLEAU_ROWS; rows++) {
    cardy = 12 * (rows + 1);
    for (cols = 0; cols < TABLEAU_COLS; cols++) {
      cardx = 16 * (cols + 1);
      if (cursorRow == rows && cursorCol == cols) {
        queue_draw_box((cardx - TABLEAU_X_SHIFT) - 8,
                       (cardy + TABLEAU_Y_SHIFT) - 8, 16, 16,
                       rnd_range(0, 255));
      }
      if (board[rows][cols] != 0) {
        queue_draw_sprite_frame(cardSprites, cardx - TABLEAU_X_SHIFT,
                                cardy + TABLEAU_Y_SHIFT, board[rows][cols],
                                false);
      }
    }
  }
  queue_draw_sprite_frame(cardSprites, DISCARD_PILE_X, DISCARD_PILE_Y, 0,
                          false);
  queue_draw_sprite_frame(cardSprites, FLIPPED_PILE_X, FLIPPED_PILE_Y, discard,
                          false);
  checkInput();
}

void initalizeGolfScene() {
  isRunning = true;
  background = allocate_sprite(&ASSET__backgrounds__background_bmp_load_list);
  cardSprites = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
  set_sprite_frametable(cardSprites, &ASSET__cardframes__frame_deck_json);
  font = text_load_font();
  loadGolfDeck(deck);
  shuffleGolfDeck(deck, SHUFFLE_SEED);
  loadGolfBoard();
}

void checkInputFromWin() {
  if (player1_new_buttons & INPUT_MASK_START) {
    remainingCards=1;
    initalizeGolfScene();
  }
}

void renderWinSequence() {

  char deckpos;
  // queue_clear_screen(rnd_range(0, 255));
  // queue_draw_sprite(0, 0, 127, 127, 0, 0, winBack);

  for (deckpos = 0; deckpos < 52; deckpos++) {
    if (directionx[deckpos] == 0)
      directionx[deckpos] = 1;
    if (directiony[deckpos] == 0)
      directiony[deckpos] = 1;
    animationx[deckpos] += directionx[deckpos];
    animationy[deckpos] += directiony[deckpos];

    if (animationx[deckpos] == 8) {
      directionx[deckpos] = 1;
    } else if (animationx[deckpos] == 120) {
      directionx[deckpos] = -1;
    }

    if (animationy[deckpos] == 8) {
      directiony[deckpos] = 1;
    } else if (animationy[deckpos] == 112) {
      directiony[deckpos] = -1;
    }

    queue_draw_sprite_frame(cardSprites, animationx[deckpos],
                            animationy[deckpos], deckpos, false);
    // queue_draw_sprite_frame(testSlot, rnd_range(8, 110), rnd_range(8, 110),
    // deckpos, false);
  }
  checkInputFromWin();
}

void renderGolfBoard() {

  char score[4];

  initalizeGolfScene();
  text_init();
  while (isRunning) {
    queue_clear_screen(187);
    queue_clear_border(186);

    if (hasWon()) {
      renderWinSequence();
    } else if (hasLost()) {
      isRunning = false;
    } else {
      renderGolfBoardNormal();
    }

    await_draw_queue();
    await_vsync(1);
    flip_pages();
    tick_music();
    update_inputs();
  }
}

#endif
