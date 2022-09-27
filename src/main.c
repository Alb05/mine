#include <gb/gb.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>
#include "tiles\MineTiles.h"
#include "maps\MineMap.h"
#include "maps\GameOverMap.h"
#include "maps\WinMap.h"
#include "maps\LoseMap.h"


#define NUM_CELLS ((uint16_t)360U)
#define NUM_MINES ((uint8_t)56U)
#define CURSOR ((uint8_t)0U)


typedef struct _cell
{
  bool isMinePresent;
  bool isFlagPresent;
  bool isRevealed;
  uint8_t adjacentMines;
} Cell;


void cleanCells(Cell* cells, const Cell* lastCell)
{
  while (cells <= lastCell)
  {
    cells->adjacentMines = 0;
    cells->isFlagPresent = false;
    cells->isRevealed = false;
    cells->isMinePresent = false;
    cells++;
  }
}


void setMines(Cell* cells)
{
  for (uint16_t i = 0; i < NUM_MINES; i++)
  {
    uint16_t index = randw() % NUM_CELLS;
    while ((cells+index)->isMinePresent) index = randw() % NUM_CELLS;
    (cells+index)->isMinePresent = true;
    (cells+index)->adjacentMines = 11U;
  }
}


void countMines(Cell* cells, const Cell* firstCell, const Cell* lastCell)
{
  while (cells <= lastCell)
  {
    if (cells->isMinePresent) {
      cells++;
      continue;
    }
    if (((cells-firstCell)%20U) > 0 && (cells-1U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) < 19 && (cells+1U)->isMinePresent) cells->adjacentMines++;
    if ((cells-firstCell) > 20U && (cells-20U)->isMinePresent) cells->adjacentMines++;
    if ((cells-firstCell) < 340U && (cells+20U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) > 20U && (cells-21U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) > 20U && (cells-19U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) < 340U && (cells+19U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) < 340U && (cells+21U)->isMinePresent) cells->adjacentMines++;
    cells++;
  }
}


void revealCell(Cell* cells, const Cell* firstCell, bool* gameOver, uint16_t* revealed)
{
  if (cells->isRevealed || cells->isFlagPresent)
  {
    return;
  }
  else
  {
    cells->isRevealed = true;
    *revealed = (*revealed) + 1U;
    set_bkg_tile_xy((cells-firstCell)%20U, (cells-firstCell)/20U, cells->adjacentMines);
    *gameOver = (NUM_CELLS - NUM_MINES) <= *revealed;
  }

  if (cells->isMinePresent)
  {
    *gameOver = true;
    *revealed = (*revealed) - 1U;
  }
  else if (cells->adjacentMines == 0)
  {
    if (((cells-firstCell)%20U) > 0U) revealCell((cells-1U), firstCell, gameOver, revealed);
    if (((cells-firstCell)%20U) < 19U) revealCell((cells+1U), firstCell, gameOver, revealed);
    if ((cells-firstCell) > 20U) revealCell((cells-20U), firstCell, gameOver, revealed);
    if ((cells-firstCell) < 340U) revealCell((cells+20U), firstCell, gameOver, revealed);
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) > 20U) revealCell((cells-21U), firstCell, gameOver, revealed);
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) > 20U) revealCell((cells-19U), firstCell, gameOver, revealed);
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) < 340U) revealCell((cells+19U), firstCell, gameOver, revealed);
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) < 340U) revealCell((cells+21U), firstCell, gameOver, revealed);
  }
}


void setFlag(Cell* cells, const uint8_t cursorX, const uint8_t cursorY)
{
  if (!cells->isRevealed)
  {
    cells->isFlagPresent = !cells->isFlagPresent;
    set_bkg_tile_xy(((cursorX-8U)>>3U), ((cursorY-16U)>>3U), (9U+cells->isFlagPresent));  
  }
}

void resetGame(Cell* cells, const Cell* firstCell, const Cell* lastCell, bool* gameOver, uint16_t* revealed, uint8_t* cursorX, uint8_t* cursorY)
{
  display_off();
  HIDE_SPRITES;
  HIDE_BKG;

  *gameOver = false;
  *revealed = 0U;
  *cursorX = 8U;
  *cursorY = 16U;
  move_sprite(CURSOR, *cursorX, *cursorY);

  cleanCells(cells, lastCell);
  setMines(cells);
  countMines(cells, firstCell, lastCell);

  set_bkg_tiles(0U, 0U, MineMapWidth, MineMapHeight, MineMap);

  SHOW_BKG;
  SHOW_SPRITES;
  DISPLAY_ON;
}

Cell cells[NUM_CELLS];
const Cell* lastCell = &cells[NUM_CELLS-1U];
const Cell* firstCell = &cells[0U];
uint16_t seed;
joypads_t jp;
uint8_t cursorX;
uint8_t cursorY;
bool gameOver;
uint16_t revealed;


void main(void)
{
  set_bkg_data(0U, 62U, MineTiles);

  set_sprite_data(0U, 1U, MineTiles+208U);
  set_sprite_tile(CURSOR, 0U);

  joypad_init(1U, &jp);
  
  seed = DIV_REG;
  seed |= (uint16_t)DIV_REG << 8U;
  initarand(seed);
  
  while (true)
  {
    resetGame(cells, firstCell, lastCell, &gameOver, &revealed, &cursorX, &cursorY);

    while (!gameOver)
    {
      joypad_ex(&jp);

      if (jp.joy0 & J_UP) cursorY -= 8U;
      if (jp.joy0 & J_DOWN) cursorY += 8U;
      if (jp.joy0 & J_LEFT) cursorX -= 8U;
      if (jp.joy0 & J_RIGHT) cursorX += 8U;
      
      if (cursorX < 8U) cursorX = 160U;
      if (cursorX > 160U) cursorX = 8U;
      if (cursorY < 16U) cursorY = 152U;
      if (cursorY > 152U) cursorY = 16U;

      move_sprite(CURSOR, cursorX, cursorY);
      uint16_t index = ((((cursorY - 16U)>>3U)*20U) + ((cursorX - 8U)>>3U));
      if (jp.joy0 & J_A) revealCell(((Cell*)(firstCell+index)), firstCell, &gameOver, &revealed);
      if (jp.joy0 & J_B) setFlag(((Cell*)(firstCell+index)), cursorX, cursorY);

      delay(150U);
      wait_vbl_done();
    }

    delay(1000);
    hide_sprite(CURSOR);
    set_bkg_tiles(0U, 0U, GameOverMapWidth, GameOverMapHeight, GameOverMap);
    if (NUM_CELLS - NUM_MINES <= revealed) set_bkg_tiles(11U, 6U, WinMapWidth, WinMapHeight, WinMap);
    else set_bkg_tiles(11U, 6U, LoseMapWidth, LoseMapHeight, LoseMap);
    wait_vbl_done();
    delay(1000U);
  }
}
