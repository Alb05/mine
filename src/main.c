#include <gb/gb.h>
#include <stdint.h>
#include <stdbool.h>
#include <rand.h>
#include "tiles/MineTiles.h"
#include "tiles/SplashScreenTiles.h"
#include "maps/MineMap.h"
#include "maps/GameOverMap.h"
#include "maps/WinMap.h"
#include "maps/LoseMap.h"
#include "maps/MenuMap.h"
#include "maps/SplashScreenMap.h"
#include "maps/WindowMap.h"


#define NUM_CELLS ((uint16_t)360U)
#define CURSOR ((uint8_t)0U)
#define MENU_SEL ((uint8_t)1U)


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


void setMines(Cell* cells, const uint8_t* numMines)
{
  for (uint16_t i = 0; i < *numMines; i++)
  {
    uint16_t index = randw() % NUM_CELLS;
    while ((cells+index)->isMinePresent) index = randw() % NUM_CELLS;
    (cells+index)->isMinePresent = true;
    (cells+index)->adjacentMines = 62U;
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
    if ((cells-firstCell) > 19U && (cells-20U)->isMinePresent) cells->adjacentMines++;
    if ((cells-firstCell) < 340U && (cells+20U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) > 19U && (cells-21U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) > 19U && (cells-19U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) < 340U && (cells+19U)->isMinePresent) cells->adjacentMines++;
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) < 340U && (cells+21U)->isMinePresent) cells->adjacentMines++;
    cells++;
  }
}


void revealCell(Cell* cells, const Cell* firstCell, bool* gameOver, uint16_t* revealed, const uint8_t* numMines)
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
    *gameOver = (NUM_CELLS - *numMines) <= *revealed;
  }

  if (cells->isMinePresent)
  {
    *gameOver = true;
    *revealed = (*revealed) - 1U;
    NR41_REG = 0x3FU;
    NR42_REG = 0xF7U;
    NR43_REG = 0x80U;
    NR44_REG = 0x80U;
  }
  else if (cells->adjacentMines == 0)
  {
    if (((cells-firstCell)%20U) > 0U) revealCell((cells-1U), firstCell, gameOver, revealed, numMines);
    if (((cells-firstCell)%20U) < 19U) revealCell((cells+1U), firstCell, gameOver, revealed, numMines);
    if ((cells-firstCell) > 19U) revealCell((cells-20U), firstCell, gameOver, revealed, numMines);
    if ((cells-firstCell) < 340U) revealCell((cells+20U), firstCell, gameOver, revealed, numMines);
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) > 19U) revealCell((cells-21U), firstCell, gameOver, revealed, numMines);
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) > 19U) revealCell((cells-19U), firstCell, gameOver, revealed, numMines);
    if (((cells-firstCell)%20U) > 0U && (cells-firstCell) < 340U) revealCell((cells+19U), firstCell, gameOver, revealed, numMines);
    if (((cells-firstCell)%20U) < 19U && (cells-firstCell) < 340U) revealCell((cells+21U), firstCell, gameOver, revealed, numMines);
  }
}


void setFlag(Cell* cells, const uint8_t cursorX, const uint8_t cursorY, uint8_t* minesLeft)
{
  if (!cells->isRevealed && (*minesLeft > 0U || cells->isFlagPresent))
  {
    cells->isFlagPresent = !cells->isFlagPresent;
    *minesLeft = cells->isFlagPresent ? (*minesLeft)-1U : (*minesLeft)+1U;
    set_bkg_tile_xy(((cursorX-8U)>>3U), ((cursorY-16U)>>3U), (9U+cells->isFlagPresent));
  }
}


void resetGame(Cell* cells, const Cell* firstCell, const Cell* lastCell, bool* gameOver, uint16_t* revealed, uint8_t* cursorX, uint8_t* cursorY, joypads_t* jp, uint8_t* numMines, uint8_t* minesLeft, bool* buttonPressed)
{
  hide_sprite(CURSOR);
  set_bkg_tiles(0U, 0U, MenuMapWidth, MenuMapHeight, MenuMap);
  *cursorX = 56U;
  *cursorY = 56U;
  move_sprite(MENU_SEL, *cursorX, *cursorY);
  
  SHOW_BKG;
  SHOW_SPRITES;
  DISPLAY_ON;

  do
  {
    *buttonPressed = false;
    joypad_ex(jp);
    if (jp->joy0 & J_DOWN)
    {
      *cursorY += 16U;
      *buttonPressed = true;
    }
    if (jp->joy0 & J_UP)
    {
      *cursorY -= 16U;
      *buttonPressed = true;
    }
    if (*buttonPressed)
    {
      if (*cursorY > 88U) *cursorY = 56U;
      if (*cursorY < 56U) *cursorY = 88U;
      move_sprite(MENU_SEL, *cursorX, *cursorY);
      NR10_REG = 0x00U;
      NR11_REG = 0x90U;
      NR12_REG = 0x43U;
      NR13_REG = 0x73U;
      NR14_REG = 0x86U;
    }
    vsync();
    if (*buttonPressed) delay(200U);
  } while ((jp->joy0 & J_A) == 0U);
  NR21_REG = 0xC0U;
  NR22_REG = 0xB5U;
  NR23_REG = 0x49U;
  NR24_REG = 0x87U;

  switch (*cursorY)
  {
    case 56U:
      *numMines = 44U;
      break;
      
    case 88U:
      *numMines = 74U;
      break;
    
    default:
      *numMines = 56U;
      break;
  }
  *minesLeft = *numMines;
  
  hide_sprite(MENU_SEL);

  display_off();
  HIDE_SPRITES;
  HIDE_BKG;

  *gameOver = false;
  *revealed = 0U;
  *cursorX = 8U;
  *cursorY = 16U;
  move_sprite(CURSOR, *cursorX, *cursorY);

  cleanCells(cells, lastCell);
  setMines(cells, numMines);
  countMines(cells, firstCell, lastCell);

  set_bkg_tiles(0U, 0U, MineMapWidth, MineMapHeight, MineMap);

  SHOW_BKG;
  SHOW_SPRITES;
  DISPLAY_ON;
}


void showMines(Cell* cells, const Cell* firstCell, const Cell* lastCell, const Cell* index)
{
  while (cells <= lastCell)
  {
    if (cells->isMinePresent && cells != index) set_bkg_tile_xy((cells-firstCell)%20U, (cells-firstCell)/20U, 11U);
    cells++;
  }
}


void waitForInput(joypads_t* jp, const uint8_t button)
{
  do
  {
    joypad_ex(jp);
    vsync();
  } while ((jp->joy0 & button) == 0U);
}


void toggleWindow(bool* state, const uint8_t* minesLeft)
{
  *state = !(*state);
  if (*state)
  {
    set_win_tiles(0U, 0U, WindowMapWidth, WindowMapHeight, WindowMap);
    uint8_t units = *minesLeft % 10U;
    uint8_t tens = *minesLeft / 10U;
    units = (units > 0U) ? (units+39U) : 28U;
    tens = (tens > 0U) ? (tens+39U) : 28U;
    set_win_tile_xy(18U, 0U, tens);
    set_win_tile_xy(19U, 0U, units);
    move_win(7U, 136U);
    SHOW_WIN;
  }
  else
  {
    HIDE_WIN;
  }
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
uint8_t numMines;
uint8_t minesLeft;
uint16_t index;
bool windowState;
bool buttonPressed;


void main(void)
{
  NR52_REG = 0x80U;
  NR50_REG = 0x77U;
  NR51_REG = 0xFFU;

  windowState = false;

  set_bkg_data(0U, 203U, SplashScreenTiles);
  set_bkg_tiles(0U, 0U, SplashScreenMapWidth, SplashScreenMapHeight, SplashScreenMap);

  joypad_init(1U, &jp);

  SHOW_BKG;
  SHOW_SPRITES;
  DISPLAY_ON;

  waitForInput(&jp, J_START);

  display_off();
  HIDE_SPRITES;
  HIDE_BKG;

  set_bkg_data(0U, 63U, MineTiles);

  set_sprite_data(0U, 1U, MineTiles+208U);
  set_sprite_data(1U, 1U, MineTiles+592U);
  set_sprite_tile(CURSOR, 0U);
  set_sprite_tile(MENU_SEL, 1U);
  
  seed = DIV_REG;
  seed |= (uint16_t)DIV_REG << 8U;
  initarand(seed);
  
  while (true)
  {
    resetGame(cells, firstCell, lastCell, &gameOver, &revealed, &cursorX, &cursorY, &jp, &numMines, &minesLeft, &buttonPressed);

    while (!gameOver)
    {
      buttonPressed = false;
      joypad_ex(&jp);
      if (!windowState)
      {
        if (jp.joy0 & J_UP)
        {
          cursorY -= 8U;
          buttonPressed = true;
        }
        if (jp.joy0 & J_DOWN)
        {
          cursorY += 8U;
          buttonPressed = true;
        }
        if (jp.joy0 & J_LEFT)
        {
          cursorX -= 8U;
          buttonPressed = true;
        }
        if (jp.joy0 & J_RIGHT)
        {
          cursorX += 8U;
          buttonPressed = true;
        }
        
        if (buttonPressed)
        {
          if (cursorX < 8U) cursorX = 160U;
          if (cursorX > 160U) cursorX = 8U;
          if (cursorY < 16U) cursorY = 152U;
          if (cursorY > 152U) cursorY = 16U;

          move_sprite(CURSOR, cursorX, cursorY);
        }
        index = ((((cursorY - 16U)>>3U)*20U) + ((cursorX - 8U)>>3U));
        if (jp.joy0 & J_A)
        {
          revealCell(((Cell*)(firstCell+index)), firstCell, &gameOver, &revealed, &numMines);
          buttonPressed = true;
        }
        if (jp.joy0 & J_B)
        {
          setFlag(((Cell*)(firstCell+index)), cursorX, cursorY, &minesLeft);
          buttonPressed = true;
        }
      }
      if (jp.joy0 & J_SELECT)
      {
        toggleWindow(&windowState, &minesLeft);
        buttonPressed = true;
        NR21_REG = 0xC0U;
        NR22_REG = 0xB5U;
        NR23_REG = 0x49U;
        NR24_REG = 0x87U;
      }

      vsync();
      if (buttonPressed) delay(200U);
    }
    showMines(cells, firstCell, lastCell, ((Cell*)(firstCell+index)));
    delay(2000U);
    hide_sprite(CURSOR);
    set_bkg_tiles(0U, 0U, GameOverMapWidth, GameOverMapHeight, GameOverMap);
    if (NUM_CELLS - numMines <= revealed) set_bkg_tiles(11U, 6U, WinMapWidth, WinMapHeight, WinMap);
    else set_bkg_tiles(11U, 6U, LoseMapWidth, LoseMapHeight, LoseMap);
    waitForInput(&jp, J_START);
  }
}
