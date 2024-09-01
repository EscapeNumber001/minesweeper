/*
 * =====================================================================================
 *
 *       Filename:  game.hpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/28/24 11:17:13
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef _GAME_HPP_
#define _GAME_HPP_

#include <vector>
#include <gtk/gtk.h>

class Board;
struct Point;
class Tile;

struct Point
{
public:
  int x;
  int y;
public:
  Point(int x, int y);
  Point(void);
};

class Tile
{
public:
  bool isMine;
  bool isRevealed;
  bool isFlagged;
  int surroundingMineCount;
public:
  Tile(int x, int y, Board* board);
  Point GetPosition();
  int CountAdjacentMines();
  const char* GetDisplayCharacter();

  // Reveals the tile 
  void TryRevealTile();
  void RevealAdjacentZeroTiles();
private:
  Board* board;
  Point position;
};

struct UITileButton
{
public:
  GtkWidget* button;
  Tile* associatedTile;
public:
  UITileButton(GtkWidget* button, Tile* tile);
};

class Board
{
public:
  std::vector<Tile*> tiles;
public:
  Tile* GetTileAtPoint(Point p);
  void PrintBoard();
};

#endif
