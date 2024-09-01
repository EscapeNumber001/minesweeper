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

// TODO: Replace TUI code with GTK or similar GUI interface


#include <iostream>
#include <vector>
#include "game.hpp"
#include <gtk/gtk.h>

struct Point;
class Board;
class Tile;



const char* tileDisplayChars[] = {
  "0",
  "1",
  "2",
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "!",  // Flagged
  " ",  // Unrevealed tile
  "X"   // Revealed mine
};

Point::Point(int x, int y)
{
  this->x = x;
  this->y = y;
}

Point::Point()
{
  Point(0, 0);
}

Tile::Tile(int x, int y, Board* board)
{
  isMine = false;
  isRevealed = false;
  surroundingMineCount = -1;  // This value is set by TryRevealTile().
  position = (Point){x, y};
  this->board = board;
}


Point Tile::GetPosition()
{
  return (Point){position.x, position.y};
}

Tile* Board::GetTileAtPoint(Point p)
{
  for (Tile* t : tiles)
  {
    if (t->GetPosition().x == p.x && t->GetPosition().y == p.y)
    {
      return t;
    }
  }
  return nullptr;
}

void Tile::TryRevealTile()
{
  if (isRevealed)
    return;

  if (!isMine)
    surroundingMineCount = CountAdjacentMines();

  isRevealed = true;
}

// TODO: Implement this system more elegantly
const char* Tile::GetDisplayCharacter()
{
  if (!isRevealed)
    return tileDisplayChars[10];

  if (!isMine)
    return tileDisplayChars[CountAdjacentMines()];

  // TODO: Flagged state
  
  return tileDisplayChars[11];
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

Board* b = NULL;
GtkWidget* window;


static void revealTileGtk(GtkButton* btn, gpointer userdata)
{
  Tile* t = (Tile*)userdata;
  t->TryRevealTile();
  gtk_button_set_label(GTK_BUTTON(btn), t->GetDisplayCharacter());
  //gtk_window_present(GTK_WINDOW(ctx->window));
}


static void on_activate(GtkApplication* app) {
  window = gtk_application_window_new(app);
  GtkWidget* grid = gtk_grid_new();
  gtk_window_set_child(GTK_WINDOW(window), grid);

  int i = 0;
  for (Tile* t : b->tiles)
  {
    Point pos = t->GetPosition();
    GtkWidget* button = gtk_button_new_with_label(t->GetDisplayCharacter());
    g_signal_connect(button, "clicked", G_CALLBACK(revealTileGtk), t);
    gtk_grid_attach(GTK_GRID(grid), button, pos.x, pos.y, 1, 1);
    i++;
  }
  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char* argv[])
{
  b = new Board();
  for (int y = 0; y < 5; y++)
  {
    for (int x = 0; x < 5; x++)
    {
      Tile* t = new Tile(x, y, b);
      t->isMine = true;
      b->tiles.push_back(t);
    }
  }
  b->tiles[0]->isMine = false;
  b->tiles[0]->isRevealed = true;

  GtkApplication* app = gtk_application_new("com.EscapeNumber001.Minesweeper", \
      G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}
