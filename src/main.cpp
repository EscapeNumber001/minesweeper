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
#include <gtk/gtk.h>
#include <time.h>

struct Point;
class Board;
class Tile;

#define REVEAL_MODE 0
#define FLAG_MODE 1

const char* tileDisplayChars[] = {
  " ",  // 0
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

std::vector<UITileButton*>* buttons;

Point::Point(int x, int y)
{
  this->x = x;
  this->y = y;
}

Point::Point()
{
  Point(0, 0);
}

UITileButton::UITileButton(GtkWidget* button, Tile* tile)
{
  this->button = button;
  this->associatedTile = tile;
}

Tile::Tile(int x, int y, Board* board)
{
  isMine = false;
  isRevealed = false;
  isFlagged = false;
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
  if (surroundingMineCount == 0)
    RevealAdjacentZeroTiles();
}

// TODO: Implement this system more elegantly
const char* Tile::GetDisplayCharacter()
{
  if (isFlagged)
    return tileDisplayChars[9];

  if (!isRevealed)
    return tileDisplayChars[10];

  if (!isMine)
    return tileDisplayChars[CountAdjacentMines()];
  
  return tileDisplayChars[11];
}

void Tile::RevealAdjacentZeroTiles()
{
  Tile* adjacentTiles[4] = {
    board->GetTileAtPoint((Point){position.x + 1, position.y}),
    board->GetTileAtPoint((Point){position.x - 1, position.y}),
    board->GetTileAtPoint((Point){position.x, position.y + 1}),
    board->GetTileAtPoint((Point){position.x, position.y - 1}),
  };

  for (Tile* t : adjacentTiles)
  {
    if (t == nullptr || t->isMine || t->isRevealed)
      continue;

    t->TryRevealTile();
  }
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
bool mode = REVEAL_MODE;

void updateAllTileButtons(std::vector<UITileButton*>* buttons)
{
  for (UITileButton* b : *buttons)
  {
    gtk_button_set_label(GTK_BUTTON(b->button), b->associatedTile->GetDisplayCharacter());
    if (b->associatedTile->isRevealed)
    {
      gtk_widget_set_sensitive(GTK_WIDGET(b->button), false);
      gtk_style_context_add_class(GTK_STYLE_CONTEXT(gtk_widget_get_style_context(b->button)), "empty_tile");
    }
  }
}

void setModeToReveal(GtkButton* button, gpointer user_data)
{
  mode = REVEAL_MODE;
}

void setModeToFlag(GtkButton* button, gpointer user_data)
{
  mode = FLAG_MODE;
}

static void revealTileGtk(GtkButton* btn, gpointer userdata)
{
  if (mode != REVEAL_MODE)
    return;

  Tile* t = (Tile*)userdata;
  t->TryRevealTile();
  updateAllTileButtons(buttons);
}


static void tileClicked(GtkButton* btn, gpointer userdata)
{
  if (mode == REVEAL_MODE)
  {
    revealTileGtk(btn, userdata);
  }
  else if (mode == FLAG_MODE)
  {
    Tile* t = (Tile*)userdata;
    if (t->isRevealed)
      return;

    t->isFlagged = !t->isFlagged;
    gtk_button_set_label(GTK_BUTTON(btn), t->GetDisplayCharacter());
  }
}

static void on_activate(GtkApplication* app) {
  GtkBuilder* builder = gtk_builder_new();
  GError *error = NULL;
  gtk_builder_add_from_file(builder, "../interface.ui", &error);  // FIXME: This local filepath won't work once the program is moved to any other directory!
  if (error) {
     g_printerr("Error loading UI file: %s\n", error->message);
     g_clear_error(&error);
      return;
  }
  GtkCssProvider* css = gtk_css_provider_new();
  gtk_css_provider_load_from_path(css, "../interface_style.css", NULL);
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);
  GtkWidget* grid = GTK_WIDGET(gtk_builder_get_object(builder, "buttonsGrid"));
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "flagModeButton")), "clicked", G_CALLBACK(setModeToFlag), NULL);  // TODO: Connect these from Glade directly because it's a bit hacky doing it this way
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "revealModeButton")), "clicked", G_CALLBACK(setModeToReveal), NULL);

  buttons = new std::vector<UITileButton*>;
  for (Tile* t : b->tiles)
  {
    Point pos = t->GetPosition();
    GtkWidget* button = gtk_button_new_with_label(t->GetDisplayCharacter());
    gtk_widget_set_visible(button, true);
    g_signal_connect(button, "clicked", G_CALLBACK(tileClicked), t);
    gtk_grid_attach(GTK_GRID(grid), button, pos.x, pos.y, 1, 1);

    UITileButton* newButton = new UITileButton(button, t);
    buttons->push_back(newButton);
  }
  gtk_window_present(GTK_WINDOW(window));

  //GtkWidget* grid = gtk_grid_new();
  //gtk_window_set_child(GTK_WINDOW(window), grid);

  /*for (Tile* t : b->tiles)
  {
    Point pos = t->GetPosition();
    GtkWidget* button = gtk_button_new_with_label(t->GetDisplayCharacter());
    g_signal_connect(button, "clicked", G_CALLBACK(revealTileGtk), t);

    gtk_grid_attach(GTK_GRID(grid), button, pos.x, pos.y, 1, 1);
  }
  GtkWidget* revealModeBtn = gtk_check_button_new_with_label("Reveal");
  gtk_check_button_set_active(GTK_CHECK_BUTTON(revealModeBtn), true);
  GtkWidget* flagModeBtn   = gtk_check_button_new_with_label("Flag");
  gtk_check_button_set_group(GTK_CHECK_BUTTON(revealModeBtn), GTK_CHECK_BUTTON(flagModeBtn));
  gtk_grid_attach(GTK_GRID(grid), revealModeBtn, 0, 6, 500, 1);
  gtk_grid_attach(GTK_GRID(grid), flagModeBtn, 2, 6, 2, 1);
  gtk_window_present(GTK_WINDOW(window));*/
}

int main(int argc, char* argv[])
{
  srand(time(NULL));
  b = new Board();

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


  GtkApplication* app = gtk_application_new("com.EscapeNumber001.Minesweeper", \
      G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}
