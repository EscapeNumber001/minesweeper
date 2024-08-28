/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  08/28/24 10:41:43
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <iostream>
#include <vector>

struct Point;
class Board;
class Tile;

struct Point
{
public:
  int x;
  int y;
public:
  Point(int x, int y)
  {
    this->x = x;
    this->y = y;
  }

  Point(void) {Point(0, 0);};
};

class Board
{
public:
  std::vector<Tile*> tiles;
};

class Tile
{
public:
  bool isMine;
  bool isRevealed;
public:
  Tile(int x, int y, Board* board)
  {
    isMine = false;
    isRevealed = false;
    position = (Point){x, y};
    this->board = board;
  }

  Point GetPosition()
  {
    return (Point){position.x, position.y};
  }

  int CountAdjacentMines()
  {
    int minesCounted = 0;

    for (Tile* tile : board->tiles)
    {
      if (tile == this)
	continue;

      // Ensure the tile is adjacent
      if (tile->position.x > position.x + 1 || tile->position.x < position.x - 1
	  || tile->position.y > position.y + 1 || tile->position.y < position.y - 1
	  )
	continue;

      if (tile->isMine)
	minesCounted++;
    }

    return minesCounted;
  }
private:
  Board* board;
  Point position;
};

int main()
{
  Board* b = new Board();
  for (int y = 0; y < 50; y++)
  {
    for (int x = 0; x < 50; x++)
    {
      Tile* t = new Tile(x, y, b);
      t->isMine = true;
      b->tiles.push_back(t);
    }
  }
  std::cout << b->tiles[75]->CountAdjacentMines() << std::endl;
  return 0;
}
