#ifndef GOLF_H
#define GOLF_H

#include "../gen/assets/backgrounds.h"
#include "../gen/assets/cardframes.h"
#include "gametank.h"
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

bool isOnBoard = true;

SpriteSlot background;
SpriteSlot cardSprites;
CardPosition eligibileCards[7];
SpriteSlot font;

/*
% 13 may compile to relatively expensive division code on an 8-bit target. Since
this only maps 1-52 to 1-13, a small branch version may be cheaper:
*/
unsigned char getCardValueFromNo(unsigned char cardNo) {
  while (cardNo > 13)
    cardNo -= 13;
  return cardNo;
}

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
  for (col = cursorCol + 1; col < TABLEAU_COLS; col++) {
    for (row = TABLEAU_ROWS - 1; row >= 0; row--) {
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
  for (col = cursorCol - 1; col >= 0; col--) {
    for (row = TABLEAU_ROWS - 1; row >= 0; row--) {
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
    if (cursorCol < TABLEAU_COLS) {
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
      discard = selection;
      discardValue = selectionValue;
      board[cursorRow][cursorCol] = 0;
      if (cursorRow > 0) {
        cursorRow--;
      }
    }
  }
}

void renderGolfBoardNormal() {
  char rows, cols, cardx, cardy;
  // queue_draw_sprite(0, 0, 127, 127, 0, 0, background);
  for (rows = 0; rows < TABLEAU_ROWS; rows++) {
    cardy = 12 * (rows + 1);
    for (cols = 0; cols < TABLEAU_COLS; cols++) {
      cardx = 16 * (cols + 1);
      if (board[rows][cols] != 0) {
        if (cursorRow == rows && cursorCol == cols && isOnBoard) {
          queue_draw_box((cardx - TABLEAU_X_SHIFT) - 8,
                         (cardy + TABLEAU_Y_SHIFT) - 8, 16, 16,
                         rnd_range(0, 255));
        }

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
  background = allocate_sprite(&ASSET__backgrounds__background_bmp_load_list);
  cardSprites = allocate_sprite(&ASSET__cardframes__frame_deck_bmp_load_list);
  set_sprite_frametable(cardSprites, &ASSET__cardframes__frame_deck_json);
  font = text_load_font();
  loadGolfDeck(deck);
  shuffleGolfDeck(deck, SHUFFLE_SEED);
  loadGolfBoard();

  // Some testing code that omits some cards to test selection hopping
  /*board[TABLEAU_ROWS - 1][5]=0;
  board[TABLEAU_ROWS - 2][5]=0;
  board[TABLEAU_ROWS - 3][5]=0;
  board[TABLEAU_ROWS - 4][5]=0;
  board[TABLEAU_ROWS - 5][5]=0;

  board[TABLEAU_ROWS - 1][6]=0;
  board[TABLEAU_ROWS - 2][6]=0;
  board[TABLEAU_ROWS - 3][6]=0;
  board[TABLEAU_ROWS - 4][6]=0;
  board[TABLEAU_ROWS - 5][6]=0;*/
}

void renderGolfBoard() {
  bool isRunning = true;
  initalizeGolfScene();
  text_init();
  while (isRunning) {
    queue_clear_screen(187);
    queue_clear_border(186);

    renderGolfBoardNormal();

    await_draw_queue();
    await_vsync(1);
    flip_pages();
    tick_music();
    update_inputs();
  }
}

#endif