/*
 * =====================================================================================
 *
 *       Filename:  interface.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/01/24 14:23:06
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include "interface.hpp"

// === Global Variables ===
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

GtkWidget* window;  // Initialized in on_activate().
std::vector<UITileButton*>* tileButtons;  // Initialized in on_activate().
Board* board = nullptr;
int mode = REVEAL_MODE;
// ==========================

void interface_init(Board* gameBoard)
{
  board = gameBoard;
}

UITileButton::UITileButton(GtkWidget* button, Tile* tile)
{
  this->button = button;
  this->associatedTile = tile;
}

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
  updateAllTileButtons(tileButtons);
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
  gtk_css_provider_load_from_path(css, "../interface_style.css", &error);
  if (error) {
     g_printerr("Error loading CSS file: %s\n", error->message);
     g_clear_error(&error);
      return;
  }
  gtk_style_context_add_provider_for_screen(gdk_screen_get_default(), GTK_STYLE_PROVIDER(css), GTK_STYLE_PROVIDER_PRIORITY_USER);

  window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));
  gtk_window_set_application(GTK_WINDOW(window), app);
  GtkWidget* grid = GTK_WIDGET(gtk_builder_get_object(builder, "buttonsGrid"));
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "flagModeButton")), "clicked", G_CALLBACK(setModeToFlag), NULL);  // TODO: Connect these from Glade directly because it's a bit hacky doing it this way
  g_signal_connect(GTK_WIDGET(gtk_builder_get_object(builder, "revealModeButton")), "clicked", G_CALLBACK(setModeToReveal), NULL);

  tileButtons = new std::vector<UITileButton*>;
  for (Tile* t : board->tiles)
  {
    Point pos = t->GetPosition();
    GtkWidget* button = gtk_button_new_with_label(t->GetDisplayCharacter());
    gtk_widget_set_visible(button, true);
    g_signal_connect(button, "clicked", G_CALLBACK(tileClicked), t);
    gtk_grid_attach(GTK_GRID(grid), button, pos.x, pos.y, 1, 1);

    UITileButton* newButton = new UITileButton(button, t);
    tileButtons->push_back(newButton);
  }
  gtk_window_present(GTK_WINDOW(window));
}

int interface_run_game(int argc, char* argv[])
{
  GtkApplication* app = gtk_application_new("com.EscapeNumber001.Minesweeper", \
      G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);
  return g_application_run(G_APPLICATION(app), argc, argv);
}
