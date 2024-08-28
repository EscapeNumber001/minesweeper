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
#include "game.hpp"

struct Point;
class Board;
class Tile;

Point::Point(int x, int y)
{
  this->x = x;
  this->y = y;
}

Point::Point()
{
  Point(0, 0);
}

void Board::PrintBoard()
{
  for (Tile* t : tiles)
  {
    std::cout << "\033[" << t->GetPosition().y + 1 << ";" << t->GetPosition().x + 1 << "H" << "#";
  }
  std::cout << std::endl;
}

Tile::Tile(int x, int y, Board* board)
{
  isMine = false;
  isRevealed = false;
  position = (Point){x, y};
  this->board = board;
}

Point Tile::GetPosition()
{
  return (Point){position.x, position.y};
}


// TODO: this needs re-indented
int Tile::CountAdjacentMines()
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

int main()
{
  Board* b = new Board();
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 5; x++)
    {
      Tile* t = new Tile(x, y, b);
      t->isMine = true;
      b->tiles.push_back(t);
    }
  }
  b->PrintBoard();
  return 0;
}
