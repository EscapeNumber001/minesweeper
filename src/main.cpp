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

#include <vector>
#include "game.hpp"
#include "interface.hpp"
#include <gtk/gtk.h>
#include <time.h>

struct Point;
class Board;
class Tile;

Board* b = NULL;


int main(int argc, char* argv[])
{
  srand(time(NULL));
  b = new Board();
  interface_init(b);

  int board_size_x = 10;
  int board_size_y = 10;
  int num_mines = 4;

  // Create tiles
  for (int y = 0; y < board_size_y; y++)
  {
    for (int x = 0; x < board_size_x; x++)
    {
      Tile* t = new Tile(x, y, b);
      b->tiles.push_back(t);
    }
  }

  int square_size = board_size_x * board_size_y;
  for (int i = 0; i < num_mines; i++)
  {
    int index = rand() % square_size;
    b->tiles[index]->isMine = true;
  }
  return interface_run_game(argc, argv);
}
