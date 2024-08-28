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
public:
  Tile(int, int, Board*);
  Point GetPosition();
  int CountAdjacentMines();
private:
  Board* board;
  Point position;
};

class Board
{
public:
  std::vector<Tile*> tiles;
public:
  void PrintBoard();
};

#endif
