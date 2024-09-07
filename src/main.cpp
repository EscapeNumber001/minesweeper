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
  int num_mines = 8;
  return interface_run_game(argc, argv);
}
